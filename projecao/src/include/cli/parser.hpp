#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

namespace cli {
namespace parser {

using parse_pos = std::string::const_iterator;

/**
 * @brief Estrutura para um termo linear.
 *
 * @tparam F Tipo de escalar.
 */
template <typename F> struct linear_term {
    F coefficient;
    size_t variable;
};

/**
 * @brief Estrutura para uma combinação de termos lineares.
 *
 * @tparam F Tipo de escalar.
 */
template <typename F> struct linear_combination {
    std::vector<linear_term<F>> parts;
    size_t max_variable;

    /**
     * @brief Limpa a estrutura.
     */
    void reset() {
        parts.clear();
        max_variable = 0;
    }

    /**
     * @brief Adiciona um termo à combinação linear.
     *
     * @param term Termo linear a ser adicionado.
     */
    void add_term(linear_term<F>&& term) {
        max_variable = std::max(max_variable, term.variable);
        parts.push_back(term);
    }
};

/**
 * @brief Estrutura para uma desigualdade linear.
 *
 * @tparam F Tipo de escalar.
 */
template <typename F> struct linear_inequality {
    linear_combination<F> lhs;
    F rhs;
};

/**
 * @brief Classe para um erro de sintaxe.
 */
class syntax_error : public std::exception {
  private:
    char m_what[1024];

  public:
    template <typename... Params>
    syntax_error(const char* filename, size_t line, size_t col,
                 const char* format, Params&&... params) {
        char reformat[1024];
        std::snprintf(reformat, sizeof(reformat),
                      "[%s@%zu:%zu] syntax error: %s", filename, line, col,
                      format);
        std::snprintf(m_what, sizeof(m_what), reformat,
                      std::forward<Params>(params)...);
    }

    const char* what() const noexcept override { return m_what; }
};

/**
 * @brief Ignora espaços em branco.
 *
 * @param begin Começo da string.
 * @param end Fim da string.
 * @return parse_pos Primeira posição em [begin, end) com um caractere
 * não-branco.
 */
inline parse_pos skip_blank(parse_pos begin, parse_pos end) {
    return std::find_if_not(begin, end, [](char c) { return std::isblank(c); });
}

/**
 * @brief Parser para leitura de escalares inteiros.
 */
template <typename F> struct integral_scalar_parser {
    using scalar_type = F;

    static_assert(std::is_convertible<long long, F>::value,
                  "scalar type must be convertible from long long");

    /**
     * @brief Lê um escalar inteiro de uma string dada.
     *
     * Gramática:
     *  <int-scalar> ::= (-)? [0-9]+
     *
     * @param begin Iterador para o início da string.
     * @param end Iterador para o fim da string.
     * @param output Referência para o resultado da leitura.
     * @return parse_pos Posição onde a leitura terminou.
     */
    parse_pos parse_scalar(parse_pos begin, parse_pos end,
                           scalar_type& output) const {
        // Lê o valor com a função padrão do C.
        const char* value_start_p = &(*begin);
        char* value_end_p;
        output = F(std::strtoll(value_start_p, &value_end_p, 10));

        // Ignora espaços em branco e retorna a última posição lida.
        return skip_blank(begin + (value_end_p - value_start_p), end);
    }
};

/**
 * @brief Parser para leitura de escalares de ponto flutuante.
 */
template <typename F> struct floating_point_scalar_parser {
    using scalar_type = F;

    static_assert(std::is_convertible<long double, scalar_type>::value,
                  "scalar type must be convertible from long double");

    /**
     * @brief Lê um escalar de ponto flutuante de uma string dada.
     *
     * Gramática:
     *  <float-scalar> ::= <int-scalar> ("." [0-9]+)?
     *
     * @param begin Iterador para o início da string.
     * @param end Iterador para o fim da string.
     * @param output Referência para o resultado da leitura.
     * @return parse_pos Posição onde a leitura terminou.
     */
    parse_pos parse_scalar(parse_pos begin, parse_pos end,
                           scalar_type& output) const {
        // Lê o valor com a função padrão do C.
        const char* value_start_p = &(*begin);
        char* value_end_p;
        output = F(std::strtold(value_start_p, &value_end_p));

        // Ignora espaços em branco e retorna a última posição lida.
        return skip_blank(begin + (value_end_p - value_start_p), end);
    }
};

/**
 * @brief Estrutura de controle para leitura de escalares.
 *
 * @tparam F Tipo de escalar.
 */
template <typename F> struct scalar_parser_traits {};

template <> struct scalar_parser_traits<double> {
    using scalar_parser = floating_point_scalar_parser<double>;
};

template <> struct scalar_parser_traits<int> {
    using scalar_parser = integral_scalar_parser<int>;
};

/**
 * @brief Classe para um parser do programa.
 *
 * @tparam F Tipo de escalar.
 */
template <typename F> class parser {
    using scalar_parser = typename scalar_parser_traits<F>::scalar_parser;

    static_assert(std::is_default_constructible<scalar_parser>::value,
                  "scalar parser must be default constructible");

  private:
    const char* m_filename;
    size_t m_line;
    parse_pos m_begin;
    parse_pos m_end;
    scalar_parser m_scalar_parser;

    /**
     * @brief Falha o processamento de uma string com um erro de sintaxe.
     *
     * @tparam Params Tipos de parâmetro da mensagem de erro.
     *
     * @param ctx Contexto de processamento.
     * @param pos Posição correspondente ao erro.
     * @param format Formato da mensagem de erro.
     * @param params Parâmetros do formato da mensagem de erro.
     */
    template <typename... Params>
    inline void fail(parse_pos pos, const char* format,
                     Params&&... params) const {
        throw syntax_error(m_filename, m_line, std::distance(m_begin, pos) + 1,
                           format, std::forward<Params>(params)...);
    }

  public:
    parser() = delete;

    parser(const char* filename, size_t line)
        : m_filename(filename), m_line(line), m_scalar_parser() {}

    parser(const char* filename) : parser(filename, 1) {}

    /**
     * @brief Lê um escalar de uma string dada.
     *
     * A gramática é definida pelo tipo de escalar.
     *
     * @param begin Iterador para o início da string.
     * @param end Iterador para o fim da string.
     * @param output Referência para o resultado da leitura.
     * @return parse_pos Posição onde a leitura terminou.
     */
    parse_pos parse_scalar(parse_pos begin, parse_pos end, F& output) const {
        // Delega o trabalho para o parser de valores escalares.
        auto value_end = m_scalar_parser.parse_scalar(begin, end, output);

        // Garante que a string foi lida até o fim.
        if (value_end < end) {
            fail(value_end, "expected scalar continuation, found '%c'",
                 *value_end);
        }
        return end;
    }

    /**
     * @brief Lê um número inteiro não-negativo de uma string dada.
     *
     * Gramática:
     *  <uint> ::= [0-9]+
     *
     * @param begin Iterador para o início da string.
     * @param end Iterador para o fim da string.
     * @param output Referência para o resultado da leitura.
     * @return parse_pos Posição onde a leitura terminou.
     */
    parse_pos parse_unsigned(parse_pos begin, parse_pos end,
                             size_t& output) const {
        // Lê o valor com a função padrão do C.
        const char* value_start_p = &(*begin);
        char* value_end_p;
        output = std::strtoull(value_start_p, &value_end_p, 10);

        // Ignora espaços em branco e garante que a string foi lida até o fim.
        auto value_end = skip_blank(begin + (value_end_p - value_start_p), end);
        if (value_end < end) {
            fail(value_end, "expected digit or whitespace, found '%c'",
                 *value_end);
        }
        return end;
    }

    /**
     * @brief Lê um termo linear de uma string dada.
     *
     * Gramática:
     *  <termo> ::= <escalar> "x" <uint>
     *
     * Obs.: O número da variável deve ser positivo.
     *
     * @param begin Iterador para o início da string.
     * @param end Iterador para o fim da string.
     * @param output Referência para o resultado da leitura.
     * @return parse_pos Posição onde a leitura terminou.
     */
    parse_pos parse_linear_term(parse_pos begin, parse_pos end,
                                linear_term<F>& output) const {
        // Todo termo contém um 'x'.
        auto x_index = std::find(begin, end, 'x');
        if (x_index == end) {
            fail(x_index, "expected 'x'");
        }

        // O coeficiente é um escalar.
        parse_scalar(begin, x_index, output.coefficient);

        // O número da variável é um inteiro positivo.
        auto variable_begin = skip_blank(x_index + 1, end);
        auto variable_end =
            parse_unsigned(variable_begin, end, output.variable);
        if (output.variable == 0) {
            fail(x_index + 1, "variable index must be > 0");
        }

        // Internamente, as variáveis começam em 0.
        output.variable--;

        // Ignora espaços em branco e garante que a string foi lida até o fim.
        auto term_end = skip_blank(variable_end, end);
        if (term_end != end) {
            fail(term_end, "expected whitespace, found '%c'", *term_end);
        }
        return term_end;
    }

    /**
     * @brief Lê uma combinação linear de uma string dada.
     *
     * Gramática:
     *  <comb-lin> ::= <termo> ("+" <termo>)*
     *
     * @param begin Iterador para o início da string.
     * @param end Iterador para o fim da string.
     * @param output Referência para o resultado da leitura.
     * @return parse_pos Posição onde a leitura terminou.
     */
    parse_pos parse_linear_combination(parse_pos begin, parse_pos end,
                                       linear_combination<F>& output) const {
        output.reset();

        // Uma combinação linear é uma sequência (não-vazia) de termos separados
        // por '+'.
        // Se a string não contém um sinal de '+', o laço roda exatamente uma
        // vez. Se não, para cada sinal de '+' o laço roda uma vez adicional.
        auto term_start = begin;
        parse_pos plus_index;
        do {
            plus_index = std::find(term_start, end, '+');

            // Lê um termo.
            linear_term<F> part;
            auto term_end = parse_linear_term(term_start, plus_index, part);

            // Garante que a string foi lida até o próximo sinal de '+' (ou o
            // fim da string, caso não haja próximo sinal).
            auto part_end = skip_blank(term_end, plus_index);
            if (part_end != plus_index) {
                fail(part_end, "expected '+', found '%c'", *part_end);
            }

            // Adiciona o termo na lista e reposiciona o índice de leitura para
            // o primeiro caractere não-branco após o próximo sinal de '+' (ou o
            // fim da string, caso não haja, e nesse caso o loop termina).
            output.add_term(std::move(part));
            term_start = skip_blank(plus_index + 1, end);
        } while (plus_index != end);

        // Garante que a string foi lida até o fim.
        if (term_start != end) {
            fail(term_start, "unexpected character '%c'", *term_start);
        }
        return end;
    }

    /**
     * @brief Lê uma desigualdade linear de uma string dada.
     *
     * Gramática:
     *  <desig-lin> ::= <comb-lin> "<=" <escalar>
     *
     * @param begin Iterador para o início da string.
     * @param end Iterador para o fim da string.
     * @param output Referência para o resultado da leitura.
     * @return parse_pos Posição onde a leitura terminou.
     */
    parse_pos parse_linear_inequality(parse_pos begin, parse_pos end,
                                      linear_inequality<F>& output) const {
        // Ignora espaços em branco no começo da string.
        begin = skip_blank(begin, end);

        // Toda desigualdade tem um sinal de "<=".
        static const char LE[] = "<=";

        auto le_index = std::search(begin, end, LE, LE + sizeof(LE) - 1);
        if (le_index == end) {
            fail(le_index, "expected '<=', found <EOL>");
        }

        // À esquerda do sinal de "<=", temos uma combinação linear (o lado
        // esquerdo da desigualdade).
        auto lhs_end = parse_linear_combination(begin, le_index, output.lhs);
        if (lhs_end != le_index) {
            fail(lhs_end, "expected '<=', found '%c'", *lhs_end);
        }

        // À direita do sinal de "<=", temos uma constante escalar (o lado
        // direito da desigualdade).
        auto rhs_begin = skip_blank(le_index + 2, end);
        auto rhs_end = parse_scalar(rhs_begin, end, output.rhs);

        // Ignora espaços em branco e garante que a string foi lida até o fim.
        auto inequality_end = skip_blank(rhs_end, end);
        if (inequality_end != end) {
            fail(inequality_end, "expected <EOL>, found '%c'", *inequality_end);
        }
        return inequality_end;
    }
};

}; // namespace parser
}; // namespace cli

#endif // __PARSER_HPP__
