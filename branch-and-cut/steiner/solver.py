from .steinlib import SteinLibInstance

import networkx as nx

import gurobipy as gp
from gurobipy import GRB


def _steiner_cut_constraint(instance: SteinLibInstance, x, W: set[int]):
    cut_edges = (
        (u, v)
        for u in W
        for v in range(1, instance.nodes + 1)
        if v not in W and (u, v) in x
    )
    return gp.quicksum(x[u, v] for u, v in cut_edges) >= 1


def _gomory_hu_tree_min_cut(T, u, v):
    P = nx.bidirectional_shortest_path(T, u, v)
    return min((T[i][j]['weight'], (i, j)) for i, j in zip(P, P[1:]))


def _find_violated_constraints(instance: SteinLibInstance, x, solution):
    G = nx.Graph()
    G.add_weighted_edges_from((i, j, solution[i, j]) for i, j in x)

    Z = instance.terminals
    all_pairs = ((Z[i], Z[j]) for i in range(len(Z)) for j in range(i + 1, len(Z)))

    T = nx.gomory_hu_tree(G, 'weight')
    for u, v in all_pairs:
        min_cut, (i, j) = _gomory_hu_tree_min_cut(T, u, v)
        if min_cut < 1:
            w = T[i][j]['weight']
            T.remove_edge(i, j)
            R, _ = nx.connected_components(T)
            yield _steiner_cut_constraint(instance, x, R)
            T.add_edge(i, j, weight=w)


def solve(instance: SteinLibInstance) -> int:
    model = gp.Model()

    # Função de peso das arestas da instância.
    weights = gp.tupledict({ (u, v): w for (u, v, w) in instance.edges })
    for i, j in weights.keys(): weights[j, i] = weights[i, j]

    # Cria as variáveis x_ij para cada aresta ij na instância.
    x = model.addVars(weights.keys(), obj=weights, vtype=GRB.BINARY, name='x')

    # Replica as variáveis na direção oposta, por conveniência.
    for i, j in x.keys(): x[j, i] = x[i, j]

    # Adiciona algumas restrições de Steiner simples para inicializar o
    # problema. 
    if len(instance.terminals) > 1:
        for v in instance.terminals:
            model.addConstr(_steiner_cut_constraint(instance, x, {v}))

    model.update()

    def _callback(model, where):
        if where == GRB.Callback.MIPSOL:
            solution = model.cbGetSolution(x)
            for constr in _find_violated_constraints(instance, x, solution):
                model.cbLazy(constr)

    model.Params.LazyConstraints = 1
    model.optimize(_callback)

    solution = model.getAttr('X', x)

    edges = [(u, v) for (u, v, _) in instance.edges if solution[u, v] == 1]
    cost = sum(w for (u, v, w) in instance.edges if solution[u, v] == 1)

    return edges, cost
