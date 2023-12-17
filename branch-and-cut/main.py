#!/usr/env python3

import os
from pathlib import Path
import argparse
import networkx as nx

from steiner.steinlib import read_steinlib_instance
from steiner import solver, cost

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="steiner",
        description="resolve instâncias do problema de Steiner em grafos no formato da SteinLib (https://steinlib.zib.de/steinlib.php).")
    parser.add_argument('instance', help='caminho do arquivo com a instância.')

    args = parser.parse_args()

    with open(args.instance, 'r') as file:        
        instance = read_steinlib_instance(file)

        instance_name, _ = os.path.splitext(args.instance)
        output_name = instance_name.replace('instancias', 'resultados')
        Path(os.path.dirname(output_name)).mkdir(parents=True, exist_ok=True)

        G = nx.Graph()
        G.add_weighted_edges_from(instance.edges)
        Z = set(instance.terminals)

        solution = solver.solve(G, Z, log_file = f"{output_name}.log")
        
        with open(f"{output_name}.sol", 'w') as out:
            print(cost(G, solution), file=out)
            print(len(solution), file=out)
            for u, v in solution:
                print(u, v, file=out)
