import math

class Nodo:
    def __init__(self, valor, es_hoja=False):
        self.valor = valor
        self.izquierda = None
        self.derecha = None
        self.es_hoja = es_hoja

def generar_arbol(num_nodos, num_hojas):
    total_nodos = num_nodos + num_hojas
    nodos = {i: Nodo(i) for i in range(1, total_nodos + 1)}

    for i in range(1, num_nodos + 1):
        if i * 2 <= total_nodos:
            nodos[i].izquierda = nodos[i * 2]
        if i * 2 + 1 <= total_nodos:
            nodos[i].derecha = nodos[i * 2 + 1]

    for i in range(num_nodos + 1, total_nodos + 1):
        nodos[i].es_hoja = True

    return nodos[1], nodos  

def process_tree(nodo, num_nodos, num_hojas):
    next_node_right_index = []

    node_stack = [(nodo, 0)]
    node_index = 0
    index_mapping = {}

    while node_stack:
        node, current_index = node_stack.pop()
        index_mapping[current_index] = node_index

        if not node.es_hoja:

            if node.derecha:
                right_child_index = current_index * 2 + 2
                node_stack.append((node.derecha, right_child_index))

            if node.izquierda:
                left_child_index = current_index * 2 + 1
                node_stack.append((node.izquierda, left_child_index))

            next_node_right_index.append(current_index * 2 + 2)

        else:
            next_node_right_index.append(0)

        node_index += 1

    next_node_right_index = [index_mapping.get(idx, 0) for idx in next_node_right_index]

    return next_node_right_index

if __name__ == "__main__":

    deepth = 7

    num_hojas =  2**deepth
    num_nodos = num_hojas - 1
    lenght = num_hojas + num_nodos
    arbol_raiz, nodos = generar_arbol(num_nodos, num_hojas)
    next_node_right_index = process_tree(arbol_raiz, num_nodos, num_hojas)
    print("right_index[", lenght, "] = ", next_node_right_index)
