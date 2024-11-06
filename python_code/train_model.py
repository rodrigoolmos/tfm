import argparse
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, roc_auc_score
import lightgbm as lgb
import matplotlib.pyplot as plt
from graphviz import Digraph
from collections import deque
import struct

#######################################################
##                                                   ##
##  dataset source: https://www.kaggle.com/datasets  ##
##                                                   ##
#######################################################

# Function to process a tree and export in the specified format "(Depth-First Search, DFS), (Pre-order Traversal)"
def process_tree(tree, n_nodes_and_leaves):
    node_leaf_value = []
    feature_index = []
    next_node_right_index = []
    leaf_or_node = []

    node_stack = [(tree, 0)]
    node_index = 0
    index_mapping = {}

    while node_stack:
        node, current_index = node_stack.pop()
        index_mapping[current_index] = node_index

        if 'split_index' in node:
            # Internal node
            node_leaf_value.append(node['threshold'])
            feature_index.append(node['split_feature'])
            leaf_or_node.append(1)  # Internal node

            left_child = node['left_child']
            right_child = node['right_child']

            left_index = current_index * 2 + 1
            right_index = current_index * 2 + 2

            node_stack.append((right_child, right_index))
            node_stack.append((left_child, left_index))

            next_node_right_index.append(right_index)

            node_index += 1
        else:
            # Leaf
            node_leaf_value.append(int(round(node['leaf_value'] * 1000000)))
            feature_index.append(0)  # Not relevant for leaves
            leaf_or_node.append(0)  # Leaf
            next_node_right_index.append(0)  # Not relevant for leaves

            node_index += 1

    # Remap the indices to ensure they are correct
    next_node_right_index = [index_mapping.get(idx, 0) for idx in next_node_right_index]

    # Pad with zeros up to n_nodes_and_leaves
    while len(node_leaf_value) < n_nodes_and_leaves:
        node_leaf_value.append(0.0)
        feature_index.append(0)
        next_node_right_index.append(0)
        leaf_or_node.append(0)

    return node_leaf_value, feature_index, next_node_right_index, leaf_or_node

# Process the model to create the C structure
def parse_model(booster, n_nodes_and_leaves):
    model = booster.dump_model()
    trees = []
    
    for tree_info in model['tree_info']:
        tree = tree_info['tree_structure']
        tree_data = process_tree(tree, n_nodes_and_leaves)
        trees.append(tree_data)
    
    return trees

def train_model_parse_and_store(data, output_model_name, num_trees=200, learning_rate=0.1, n_jobs=72, test_size=0.8, max_depth = 8):
    # Separate features and label
    # The last column is assumed to be the prediction (Outcome)
    X = data.iloc[:, :-1]
    y = data.iloc[:, -1]
    num_leaves = int(2**(max_depth - 1))
    num_nodes_and_leaves = num_leaves * 2
    compact_data = [0] * num_nodes_and_leaves
    
    # Split the dataset into training and test sets 0.2 test 0.8 training
    train_size = 1 - test_size
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=test_size, train_size=train_size, random_state=42)

    lightgbm_model = lgb.LGBMClassifier(objective='binary', learning_rate=learning_rate, 
                                        n_estimators=num_trees, n_jobs=n_jobs, num_leaves=num_leaves , max_depth=max_depth)
    lightgbm_model.fit(X_train, y_train)
    y_pred_lgb = lightgbm_model.predict(X_test)
    lgb_accuracy = accuracy_score(y_test, y_pred_lgb)
    lgb_auc = roc_auc_score(y_test, y_pred_lgb)

    print(f"LightGBM - Number of Trees: {lightgbm_model.n_estimators}")
    print(f"LightGBM - Accuracy: {lgb_accuracy:.4f}, AUC: {lgb_auc:.4f}")

    booster_json = lightgbm_model.booster_.dump_model()
    tree = booster_json['tree_info'][0]['tree_structure']

    print(lightgbm_model.get_params())
    trees = parse_model(lightgbm_model.booster_, num_nodes_and_leaves)

    # Save the structures in a binary file
    with open(output_model_name, 'wb') as f:
        f.write(b'model')
        for node_leaf_value, feature_index, next_node_right_index, leaf_or_node in trees:
            for index in range(len(feature_index)):
                if leaf_or_node[index] == 0:
                    f.write(struct.pack('B', leaf_or_node[index]))
                    f.write(struct.pack('B', feature_index[index]))
                    f.write(struct.pack('B', next_node_right_index[index]))
                    f.write(struct.pack('B', 0xff))
                    f.write(struct.pack('i', int(node_leaf_value[index])))  # Save leaf values as int for FPGA reources
                else:
                    f.write(struct.pack('B', leaf_or_node[index]))
                    f.write(struct.pack('B', feature_index[index]))
                    f.write(struct.pack('B', next_node_right_index[index]))
                    f.write(struct.pack('B', 0xff))
                    f.write(struct.pack('f', node_leaf_value[index]))  # Save node values as float
        


def main():
    parser = argparse.ArgumentParser(description="Train a LightGBM model and export it as binary ready to be used in the FPGA.")
    parser.add_argument('--data_path', type=str, required=True, help="Path to the dataset CSV file. Data must be preprocessed with only numerical values allowed, and the prediction column must be the last column.")
    parser.add_argument('--output_model_name', type=str, required=True, help="Path to save the output model file.")
    parser.add_argument('--num_trees', type=int, default=1024, help="Number of trees for LightGBM.")
    parser.add_argument('--learning_rate', type=float, default=0.1, help="Learning rate for LightGBM.")
    parser.add_argument('--n_jobs', type=int, default=72, help="Number of parallel jobs for LightGBM.")
    parser.add_argument('--test_size', type=float, default=0.2, help="Proportion of dataset for testing.")
    parser.add_argument('--max_depth', type=int, default=8, help="Maximum depth of the trees <= 8.")
    
    args = parser.parse_args()
    
    if args.max_depth > 8:
        raise ValueError("The maximum depth of the trees (max_depth) cannot exceed 8.")

    data = pd.read_csv(args.data_path)

    train_model_parse_and_store(data, args.output_model_name, args.num_trees, 
                                args.learning_rate, args.n_jobs, args.test_size, args.max_depth)

if __name__ == "__main__":
    main()