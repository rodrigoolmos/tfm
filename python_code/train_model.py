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

def train_model_parse_and_store(data, output_model_name, num_trees=200, learning_rate=0.1, n_jobs=72, test_size=0.8, max_depth = 10):
    # Separate features and label
    X = data.drop('Outcome', axis=1)
    y = data['Outcome']
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
        


def preprocess_data(data):
    data.replace({'M': 0, 'F': 1, 'M ': 0, 'F ': 1, ' M ': 0, ' F ': 1, ' M': 0, ' F': 1,
                  'Yes': 1, 'No': 0, 'YES': 1, 'NO': 0}, inplace=True)
    return data

# Load the diabetes dataset
path = "./datasets/diabetes.csv"
column_names = ['Pregnancies', 'Glucose', 'BloodPressure', 'SkinThickness', 'Insulin', 'BMI', 'DiabetesPedigreeFunction', 'Age', 'Outcome']
data = pd.read_csv(path, names=column_names)
train_model_parse_and_store(data, "./trained_models/diabetes.model", num_trees=512, learning_rate=0.5, n_jobs=72, test_size=0.2, max_depth = 8)

# Load the Heart Attack dataset
path = "./datasets/Heart_Attack.csv"
column_names = ['age', 'sex', 'cp', 'trtbps', 'chol', 'fbs', 'restecg', 'thalachh', 'exng', 'oldpeak', 'slp', 'caa', 'thall', 'Outcome']
data = pd.read_csv(path, names=column_names)
train_model_parse_and_store(data, "./trained_models/heart_attack.model", num_trees=512, learning_rate=0.5, n_jobs=72, test_size=0.2, max_depth = 8)

# Load the Lung Cancer raw dataset
path = "./datasets/Lung_Cancer_raw.csv"
column_names = ['GENDER', 'AGE', 'SMOKING', 'YELLOW_FINGERS', 'ANXIETY', 'PEER_PRESSURE', 'CHRONIC_DISEASE', 'FATIGUE', 'ALLERGY', 
                'WHEEZING', 'ALCOHOL_CONSUMING', 'COUGHING', 'SHORTNESS_OF_BREATH', 'SWALLOWING_DIFFICULTY', 'CHEST_PAIN', 'Outcome']
data = pd.read_csv(path, names=column_names)
# This dataset has strings so it needs preprocessing
processed_data = preprocess_data(data)
# Save the transformed DataFrame in a new CSV file
output_path = "./datasets/Lung_Cancer_processed_dataset.csv"
processed_data.to_csv(output_path, index=False, header=False)
train_model_parse_and_store(processed_data, "./trained_models/lung_cancer.model", num_trees=512, learning_rate=0.5, n_jobs=72, test_size=0.1, max_depth = 8)

# Load the anemia dataset
path = "./datasets/anemia.csv"
column_names = ['Number', 'Sex', 'Red_Pixel', 'Green_pixel', 'Blue_pixel', 'Hb', 'Outcome']
data = pd.read_csv(path, names=column_names)
# First column is useless info so eliminate it
sorted_data = data.iloc[:, 1:]
# This dataset has strings so it needs preprocessing
processed_data = preprocess_data(sorted_data)
# Save the transformed DataFrame in a new CSV file
output_path = "./datasets/anemia_processed_dataset.csv"
processed_data.to_csv(output_path, index=False, header=False)
train_model_parse_and_store(processed_data, "./trained_models/anemia.model", num_trees=512, learning_rate=0.5, n_jobs=72, test_size=0.6, max_depth = 8)

# Load the alzheimers dataset
path = "./datasets/alzheimers_disease_data.csv"
column_names = ['PatientID',  'Age',  'Gender',  'Ethnicity',  'EducationLevel',  'BMI',  'Smoking',  'AlcoholConsumption',  'PhysicalActivity',
                'DietQuality',  'SleepQuality',  'FamilyHistoryAlzheimers',  'CardiovascularDisease',  'Diabetes',  'Depression',  'HeadInjury',
                'Hypertension',  'SystolicBP',  'DiastolicBP',  'CholesterolTotal',  'CholesterolLDL',  'CholesterolHDL',  'CholesterolTriglycerides',
                'MMSE',  'FunctionalAssessment',  'MemoryComplaints',  'BehavioralProblems',  'ADL',  'Confusion',  'Disorientation',
                'PersonalityChanges',  'DifficultyCompletingTasks',  'Forgetfulness',  'Outcome',  'DoctorInCharge']

data = pd.read_csv(path, names=column_names)
# First column is useless info so eliminate it
sorted_data = data.iloc[:, 1:]
# Last column is useless info so eliminate it
sorted_data = sorted_data.iloc[:, :-1]
# Save the transformed DataFrame in a new CSV file
output_path = "./datasets/alzheimers_processed_dataset.csv"
sorted_data.to_csv(output_path, index=False, header=False)
train_model_parse_and_store(sorted_data, "./trained_models/alzheimers.model", num_trees=512, learning_rate=0.5, n_jobs=72, test_size=0.6, max_depth = 8)
