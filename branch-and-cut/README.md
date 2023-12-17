# Combinatória Poliédrica - Branch-and-Cut

## Estrutura

- `main.py`: Ponto de entrada
- `steiner/`: Implementação do problema
  - `basic.py`: Funções de validação e custo de solução
  - `solver.py`: Implementação da heurística e modelo de programação inteira
  - `steinlib.py`: Leitura de arquivos no formato da SteinLib
- `instancias/`: Instâncias da SteinLib utilizadas, por grupo.
- `resultados/`: Resultados obtidos para as instâncias utilizadas, por grupo. Arquivos `.log` contém o log do Gurobi e arquivos `.sol` contém a melhor solução obtida (pode não ser ótima).

## Instruções

O programa utiliza a versão 10.0.3 do Gurobi e a versão 3.2.1 da biblioteca Python NetworkX, que pode ser instalada com o comando

```sh
pip install networkx==3.2.1
```

O programa pode ser executado com
```sh
python3 main.py <entrada>
```

Onde `<entrada>` corresponde ao caminho de um arquivo `.stp` dentro da pasta
`instancias`.
