#ifndef __CLI_HPP__
#define __CLI_HPP__

#include <iostream>
#include <string>
#include <utility>

#include <linalg.hpp>
#include <polyhedral.hpp>

#include <io/parser.hpp>

namespace io {
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
    os << "[";
    for (size_t i = 0; i < vec.size(); i++) {
        os << " " << vec[i];
    }
    os << " ]";
    return os;
}

template <typename F>
std::ostream&& operator<<(std::ostream&& os, const vecn<F>& vec) {
    return std::move(os << vec);
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

}; // namespace io

#endif // __CLI_HPP__
