#!/usr/env python3

import argparse
import networkx as nx

from steiner.steinlib import read_steinlib_instance
import steiner.solver as solver

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="steiner",
        description="resolve instâncias do problema de Steiner em grafos no formato da SteinLib (https://steinlib.zib.de/steinlib.php).")
    parser.add_argument('instance', help='caminho do arquivo com a instância.')

    args = parser.parse_args()

    with open(args.instance, 'r') as file:
        instance = read_steinlib_instance(file)

        G = nx.Graph()
        G.add_weighted_edges_from(instance.edges)
        Z = set(instance.terminals)

        print(solver.solve(G, Z))
