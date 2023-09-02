#ifndef __CLI_HPP__
#define __CLI_HPP__

#include <iostream>
#include <string>
#include <utility>

#include <cli/parser.hpp>
#include <linalg.hpp>
#include <polyhedral.hpp>

namespace cli {
using namespace linalg;
using namespace polyhedral;

/**
 * @brief Escreve um vetor em uma stream.
 *
 * @tparam F Tipo de escalar.
 *
 * @param os Stream de saída.
 * @param vec Vetor.
 *
 * @return std::ostream& Uma referência para a stream.
 */
template <typename F>
std::ostream& operator<<(std::ostream& os, const vecn<F>& vec) {
    os << "(";
    for (size_t i = 0; i < vec.size(); i++) {
        os << " " << vec[i];
    }
    os << " )";
    return os;
}

template <typename F>
std::ostream&& operator<<(std::ostream&& os, const vecn<F>& vec) {
    return std::move(os << vec);
}

/**
 * @brief Lê um vetor de uma stream.
 *
 * @tparam F Tipo de escalar.
 *
 * @param os Stream de entrada.
 * @param vec Vetor.
 *
 * @return std::ostream& Uma referência para a stream.
 */
template <typename F> std::istream& operator>>(std::istream& is, vecn<F>& vec) {
    std::string line;
    if (std::getline(is, line)) {
        parser::parser<F>("<stdin>", line.begin(), line.end())
            .parse_vector(line.begin(), line.end(), vec);
    } else {
        throw std::ios_base::failure(
            "could not read vector from input stream: reached EOS");
    }
    return is;
}

template <typename F>
std::istream&& operator>>(std::istream&& is, vecn<F>& vec) {
    return std::move(is >> vec);
}

/**
 * @brief Escreve um poliedro em uma stream.
 *
 * @tparam F Tipo de escalar.
 *
 * @param os Stream de saída.
 * @param P Poliedro.
 *
 * @return std::ostream& Uma referência para a stream.
 */
template <typename F>
std::ostream& operator<<(std::ostream& os, const polyhedron<F>& P) {
    for (size_t i = 0; i < P.A().rows(); i++) {
        auto row = P.A()[i];
        bool zero = true;
        for (size_t j = 0; j < row.size(); j++) {
            if (row[j] == 0) {
                continue;
            }
            if (!zero) {
                os << " + ";
            }
            os << row[j] << "x" << (j + 1);
            zero = false;
        }
        if (zero) {
            os << "0 ";
        }
        os << " <= ";
        os << P.b()[i];
        os << std::endl;
    }
    return os;
}

template <typename F>
std::ostream&& operator<<(std::ostream&& os, const polyhedron<F>& P) {
    return std::move(os << P);
}

/**
 * @brief Le um poliedro de uma stream.
 *
 * @tparam F Tipo de escalar.
 *
 * @param os Stream de entrada.
 * @param P Referência para o poliedro.
 *
 * @return std::ostream& Uma referência para a stream.
 */
template <typename F>
std::istream& operator>>(std::istream& is, polyhedron<F>& P) {
    std::vector<parser::linear_inequality<F>> inequalities;

    // Lê as inequações que define o poliedro, uma por linha, e computa o
    // número de colunas da matriz (i.e. o número de variáveis no sistema).
    size_t n = 0;
    size_t lineno = 1;
    for (std::string line; std::getline(is, line); lineno++) {
        // Termina a leitura quando encontra uma linha em branco
        if (line.find_first_not_of(' ') == std::string::npos) {
            break;
        }

        parser::linear_inequality<F> inequality;
        parser::parser<F>("<stdin>", lineno, line.begin(), line.end())
            .parse_linear_inequality(line.begin(), line.end(), inequality);

        n = std::max(n, inequality.lhs.max_variable + 1);
        inequalities.push_back(std::move(inequality));
    }

    // Constrói a matriz A e vetor b correspondentes às inequações lidas.
    auto m = inequalities.size();
    matnxm<F> A(m, n);
    vecn<F> b(m);

    for (size_t i = 0; i < m; i++) {
        const auto& inequality = inequalities[i];
        for (const auto& m : inequality.lhs.parts) {
            A(i, m.variable) = m.coefficient;
        }
        b[i] = inequality.rhs;
    }

    P = {std::move(A), std::move(b)};
    return is;
}

template <typename F>
std::istream&& operator>>(std::istream&& is, polyhedron<F>& P) {
    return std::move(is >> P);
}

}; // namespace cli

#endif // __CLI_HPP__
