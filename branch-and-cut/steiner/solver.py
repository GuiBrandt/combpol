import networkx as nx

import gurobipy as gp
from gurobipy import GRB

from itertools import combinations


def _gomory_hu_tree_min_cut(T, u, v):
    P = nx.bidirectional_shortest_path(T, u, v)
    return min((T[i][j]['weight'], (i, j)) for i, j in zip(P, P[1:]))


def solve(G: nx.Graph, Z: set[int]) -> (set[(int, int)], int):
    """Encontra uma solução ótima para o problema de Steiner no grafo G com os
    terminais Z.

    Usa o atributo `weight` da arestas de G como peso.
    """
    def cut_edges(W: set[int]):
        """Gera as arestas no corte definido por um conjunto de vértices W"""
        return ((u, v) for u in W for v in G.neighbors(u) if v not in W)

    def steiner_cut_constraint(W: set[int]):
        """Devolve a restrição de corte de Steiner para um conjunto dado.

        Assume-se implicitamente que o conjunto define um corte de Steiner
        válido (i.e. existe pelo menos um terminal dentro e um fora do
        conjunto)."""
        return gp.quicksum(x[u, v] for u, v in cut_edges(W)) >= 1

    def build_gomory_hu_tree(solution):
        """Constrói a árvore de Gomory-Hu para o grafo, utilizando os
        coeficientes da solução dada como pesos."""
        H = nx.Graph()
        H.add_weighted_edges_from((u, v, solution[u, v]) for u, v in G.edges)
        return nx.gomory_hu_tree(H, 'weight')

    def violated_constraints(solution):
        """Gera restrições violadas para uma solução dada, caso existam."""
        T = build_gomory_hu_tree(solution)
        for u, v in combinations(Z, 2):
            min_cut, (r, s) = _gomory_hu_tree_min_cut(T, u, v)
            # Se o corte mínimo entre os terminais u e v é menor que 1, temos
            # uma restrição violada.
            if min_cut < 1:
                # Removemos a aresta de corte mais leve para encontrar as
                # componentes que resultam; como se trata de um u-v corte, e
                # ambos u e v são terminais, as componentes definem um corte de
                # Steiner.
                weight = T[r][s]['weight']
                T.remove_edge(r, s)
                W, _ = nx.connected_components(T)
                yield steiner_cut_constraint(W)
                # Devolvemos a aresta removida para continuar o processo.
                T.add_edge(r, s, weight=weight)

    model = gp.Model()

    # Temos variáveis da forma x[u,v] para cada aresta {u, v} na instância.
    # O coeficiente de cada variável na função objetivo é exatamente o peso da
    # aresta.
    x = gp.tupledict({
        (u, v): model.addVar(obj=weight, vtype=GRB.BINARY, name=f'x[{u},{v}]')
        for u, v, weight in G.edges.data("weight")
    })

    # Replica as variáveis e pesos na direção oposta, por conveniência.
    for u, v in x.keys():
        x[v, u] = x[u, v]

    # Adicionamos algumas restrições de Steiner simples para inicializar o
    # problema.
    # Em particular, se existem pelo menos dois terminais na instância, cada
    # terminal, sozinho, define um corte de Steiner.
    if len(Z) > 1:
        model.addConstrs(steiner_cut_constraint({v}) for v in Z)

    model.Params.LazyConstraints = 1

    def callback(model, where):
        if where == GRB.Callback.MIPSOL:
            solution = model.cbGetSolution(x)
            for constr in violated_constraints(solution):
                model.cbLazy(constr)
    model.optimize(callback)

    optimal = model.getAttr('X', x)

    # Selecionamos para a solução arestas com valor ótimo acima de 0.5, para
    # evitar problemas de arredondamento (e.g. valores ótimos como 0.99999...).
    solution = {(u, v) for (u, v) in G.edges if optimal[u, v] > 0.5}
    cost = sum(w for (u, v, w) in G.edges.data("weight") if (u, v) in solution)
    return solution, cost


__all__ = ['solve']
