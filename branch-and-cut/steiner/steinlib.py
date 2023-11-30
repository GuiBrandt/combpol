from typing import IO
from dataclasses import dataclass

import logging


@dataclass
class SteinLibInstance:
    """
    Classe representando uma instância da SteinLib

    Atributos
    ---------
    nodes : int
        Número de nós no grafo da instância.
    edges : [(int, int, int)]
        Lista de arestas do grafo, na forma de tuplas (u, v, w), onde u e v são
        os extremos da aresta e w é seu peso.
    terminals : [int]
        Lista de nós terminais da instância.
    """
    nodes: int
    edges: [(int, int, int)]
    terminals: [int]


def _next_line(file: IO) -> str:
    """Encontra e devolve a próxima linha não-vazia do arquivo."""
    while True:
        line = file.readline()
        if len(line) == 0: # readline retorna vazio no fim do arquivo
            raise EOFError()
        line = line.rstrip()
        if len(line) > 0: # ignora linhas em branco
            return line


def _ignore_section(file: IO) -> None:
    while True:
        line = _next_line(file)
        if line == "END":
            break


def _read_graph(file: IO) -> (int, [(int, int, int)]):
    nodes = 0
    n_edges = 0
    edges = []
    while True:
        line = _next_line(file)
        if line.startswith("Nodes"):
            nodes = int(line.removeprefix("Nodes"))
        elif line.startswith("Edges"):
            n_edges = int(line.removeprefix("Edges"))
        elif line.startswith("E "):
            edges.append(tuple(map(int, line.removeprefix("E ").split(" "))))
        elif line == "END":
            break
    if len(edges) != n_edges:
        logging.warn(f"expected {n_edges} edges, got {len(edges)}")
    return nodes, edges


def _read_terminals(file: IO) -> [int]:
    n_terminals = 0
    terminals = []
    while True:
        line = _next_line(file)
        if line.startswith("Terminals"):
            n_terminals = int(line.removeprefix("Terminals"))
        elif line.startswith("T"):
            terminals.append(int(line.removeprefix("T ")))
        elif line == "END":
            break
    if len(terminals) != n_terminals:
        logging.warn(f"expected {n_terminals} terminals, got {len(terminals)}")
    return terminals


def read_steinlib_instance(file: IO) -> SteinLibInstance:
    """Lê uma instância de problema de Steiner em formato STP de um arquivo.
    
    Argumentos
    ----------
    file : IO
        Arquivo do qual ler a instância.

    Retorno
    -------
    SteinLibInstance
        Um objeto com informações da instância lida.

    Exceções
    --------
    EOFError
        Caso chegue ao fim do arquivo durante o processamento.
    ValueError
        Caso falhe em converter algum valor durante o processamento.
    """
    header = file.readline().strip()
    if header != "33D32945 STP File, STP Format Version 1.0":
        logging.warn("file does not start with STP format magic.")
    nodes: int = 0
    edges: [(int, int, int)] = []
    terminals: [int] = []
    while True:
        line = _next_line(file)
        if line.startswith("SECTION"):
            section = line.removeprefix("SECTION").strip()
            if section == "Graph":
                nodes, edges = _read_graph(file)
            elif section == "Terminals":
                terminals = _read_terminals(file)
            else:
                if section not in {"Comment", "Coordinates"}:
                    logging.warn(f"ignoring foreign section: {section}")
                _ignore_section(file)
        elif line == "EOF":
            break
        else:
            logging.warn(f"ignoring unexpected line: {line}")
    if nodes <= 0:
        logging.warn(f"possibly an invalid instance ({nodes} nodes)")
    return SteinLibInstance(nodes, edges, terminals)


__all__ = ['SteinLibInstance', 'read_steinlib_instance']
