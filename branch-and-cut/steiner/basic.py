import networkx as nx

from typing import Set, Tuple


SteinerTree = Set[Tuple[int, int]]
"""Tipo para uma árvore de steiner."""


def cost(G: nx.Graph, solution: SteinerTree) -> int:
    """Calcula o custo de uma solução em um grafo dado."""
    return sum(G[u][v]['weight'] for (u, v) in solution)


def is_minimal_valid_solution(
    G: nx.Graph,
    Z: set[int],
    solution: SteinerTree
) -> bool:
    """Determina se uma solução é válida minimal para um grafo e conjunto de
    terminais dado."""
    T = nx.Graph()
    T.add_edges_from(solution)
    return nx.is_tree(T) and \
        all(z in T.nodes for z in Z) and \
        all(v in Z for v in T.nodes if T.degree(v) == 1)


__all__ = ["SteinerTree", "cost", "is_minimal_valid_solution"]
