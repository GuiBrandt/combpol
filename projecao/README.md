# Combinatória Poliédrica - Projeções

## Instruções

O programa acompanha um arquivo [Makefile](./Makefile) com targets para compilar
e rodar o programa com as entradas de exemplo.

O target `run` compila o programa caso necessário e executa com os arquivos de
entrada de exemplo (todos os arquivos com extensão `.in` na pasta
[examples](./examples/)):

```sh
make run
```

Isso gera um arquivo executável `build/release/combpol-projecao` relativo à
pasta de execução do comando `make`.

O programa aceita nomes de arquivos de entrada como argumento de linha de
comando; por exemplo:

```sh
build/release/combpol-projecao examples/2d-offset.in
```

Executa o programa apenas com o arquivo `examples/2d-offset.in`.

### Configuração de compilador

A compilação usa o compilador de C++ padrão escolhido pelo Make. É possível
configurar o comando usado para compilação com a variável `CXX` na execução do
Make. Por exemplo, para compilar com o Clang:

```sh
make clean build CXX=clang++
```

A compilação foi testada com GCC 13.2.1 e Clang 16.0.5 em Linux. O programa
requer suporte a C++14 ou mais recente.

## Formato de entrada

Os arquivos de entrada do programa têm duas seções, separadas por uma linha em
branco:
- Uma lista de desigualdades lineares, que definem um poliedro;
- Uma lista de vetores de direção nas quais projetar o poliedro dado.

As linhas da entrada seguem a seguinte gramática (espaços em branco são
ignorados):

```bnf
<vetor>     ::= "[" (<float>)+ "]"              // [1.0 0.0]
<desig-lin> ::= <comb-lin> "<=" <float>         // 1.0 x1 + -2.0 x2 <= 1.0
<comb-lin>  ::= <termo> ("+" <termo>)*          // 1.0 x1 + -2.0 x2
<termo>     ::= <float> "x" <uint>              // 1.0 x1, -2.0 x2
<float>     ::= ("-")? [0-9]+ ("." [0-9]+)?     // 0, 0.5, -1.0, -2
<uint>      ::= [0-9]+                          // 1, 2, 3, ...
```

Onde cada linha do arquivo é uma desigualdade linear (`<desig-lin>`) ou um vetor
(`<vetor>`), conforme as seções.

Valores de `<uint>` devem ser números inteiros positivos (i.e. 0 não é um valor
válido).

## Saída

O programa exibe o poliedro lido, e em seguida diz se é vazio ou não.

Para cada vetor na entrada, o programa exibe o poliedro `P(D, d)` obtido a
partir da projeção do poliedro na direção do vetor.

Quando um poliedro seria vazio, o programa imprime `0 <= 0` (i.e. um poliedro
que corresponde ao espaço inteiro).

## Exemplos

Exemplos de entrada estão disponíveis na pasta [examples](./examples/), e
incluem:
- ([2d.in](./examples/2d.in) / [2d.out](./examples/2d.out)) Um poliedro
  não-vazio limitado em ℝ² que inclui a origem, projetado em várias direções.
- ([2d-offset.in](./examples/2d-offset.in) /
  [2d-offset.out](./examples/2d-offset.out)) Um poliedro não-vazio limitado em ℝ² que
  não inclui a origem, projetado em várias direções.
- ([2d-unlimited.in](./examples/2d-unlimited.in) /
   [2d-unlimited.out](./examples/2d-unlimited.out)) Um poliedro ilimitado em ℝ²
  que não inclui a origem, projetado em várias direções.
- ([3d.in](./examples/3d.in) / [3d.out](./examples/3d.out)) Um poliedro
  não-vazio em ℝ³ que inclui a origem, projetado em várias direções.
- ([empty.in](./examples/empty.in) / [empty.out](./examples/empty.out)) Um
  poliedro vazio, projetado em várias direções.
