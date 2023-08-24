#ifndef __POLYHEDRAL__
#define __POLYHEDRAL__

#include "internal.hpp"
#include <cassert>
#include <vector>

#include <linalg.hpp>

namespace polyhedral {
using namespace linalg;

/**
 * @brief Classe de poliedros sobre um corpo F.
 *
 * @tparam F Tipo de escalar.
 */
template <typename F> class polyhedron {
  private:
    matnxm<F> m_A;
    vecn<F> m_b;

  public:
    using scalar_type = F;

    polyhedron() = delete;

    polyhedron(const polyhedron& other) : m_A(other.A()), m_b(other.b()) {}

    polyhedron(polyhedron&& other)
        : m_A(std::move(other.A())), m_b(std::move(other.b())) {}

    polyhedron& operator=(const polyhedron& other) {
        m_A = other.A();
        m_b = other.b();
        return *this;
    }

    polyhedron& operator=(polyhedron&& other) {
        m_A = std::move(other.A());
        m_b = std::move(other.b());
        return *this;
    }

    /**
     * @brief Constrói um poliedro P(A, b) copiando uma matriz A e um vetor b
     * dados.
     *
     * @param A Matriz A da definição do poliedro.
     * @param b Vetor b da definição do poliedro.
     */
    polyhedron(const matnxm<scalar_type>& A, const vecn<scalar_type>& b)
        : m_A(A), m_b(b) {
        internal::validate("cannot create polyhedron from matrix and "
                           "vector of incompatible dimensions",
                           [&]() { return A.rows() == b.size(); });
    }

    /**
     * @brief Constrói um poliedro P(A, b) movendo uma matriz A e um vetor b
     * dados.
     *
     * @param A Matriz A da definição do poliedro.
     * @param b Vetor b da definição do poliedro.
     */
    polyhedron(matnxm<scalar_type>&& A, vecn<scalar_type>&& b)
        : m_A(A), m_b(b) {
        internal::validate("cannot create polyhedron from matrix and "
                           "vector of incompatible dimensions",
                           [&]() { return A.rows() == b.size(); });
    }

    /**
     * @brief Número de dimensões do espaço vetorial no qual o poliedro existe.
     *
     * @return size_t O número de dimensões do espaço vetorial no qual o
     * poliedro existe.
     */
    size_t dimensions() const { return m_A.cols(); }

    /**
     * @brief Matriz A da definição do poliedro.
     *
     * @return matnxm<scalar_type>& Uma referência para a matriz da definição do
     * poliedro.
     */
    matnxm<scalar_type>& A() { return m_A; }

    /**
     * @brief Matriz A da definição do poliedro.
     *
     * @return const matnxm<scalar_type>& Uma referência imutável para a matriz
     * A da definição do poliedro.
     */
    const matnxm<scalar_type>& A() const { return m_A; }

    /**
     * @brief Vetor b da definição do poliedro.
     *
     * @return matnxm<scalar_type>& Uma referência para o vetor b da definição
     * do poliedro.
     */
    vecn<scalar_type>& b() { return m_b; }

    /**
     * @brief Vetor b da definição do poliedro.
     *
     * @return matnxm<scalar_type>& Uma referência imutável para o vetor b da
     * definição do poliedro.
     */
    const vecn<scalar_type>& b() const { return m_b; }

    /**
     * @brief Calcula o poliedro de projeção P(D, d) do poliedro na direção
     * dada.
     *
     * @param direction Vetor que indica a direção da projeção.
     * @return polyhedron<scalar_type> Um poliedro P(D, d) tal que a projeção
     * de P(A, b) na direção dada sobre qualquer conjunto H é P(D, d) ∩ H.
     */
    polyhedron<scalar_type> project(const vecn<scalar_type>& direction) const {
        internal::validate("cannot project polyhedron on direction with "
                           "incompatible dimensions",
                           [&]() { return dimensions() == direction.size(); });

        std::vector<size_t> N, Z, P;
        partition_for_projection(direction, N, Z, P);

        // Como N, P e Z são disjuntos, r = |Z ∪ (N × P)| = |Z| + |N| × |P|.
        size_t r = Z.size() + N.size() * P.size();

        matnxm<scalar_type> D(r, m_A.cols());
        vecn<scalar_type> d(r);

        // Faça R = {0, ..., r - 1}.
        //
        // Construímos uma bijeção implícita p : R -> Z ∪ (N × P) tal que
        //  p(0...|Z|-1) = Z (em ordem)
        //  p(|Z|...r-1) = N × P (em ordem lexicográfica)
        //
        // Os laços abaixo naturalmente iteram Z ∪ (N × P) na ordem induzida
        // por p.

        for (size_t i = 0; i < Z.size(); i++) {
            size_t pi = Z[i];
            D[i] = m_A[pi];
            d[i] = m_b[pi];
        }

        for (size_t j = 0, i = Z.size(); j < N.size(); j++) {
            size_t s = N[j];
            for (size_t k = 0; k < P.size(); k++, i++) {
                size_t t = P[k];
                scalar_type np = m_A[s].to_vec() * direction,
                            pp = m_A[t].to_vec() * direction;
                D[i] = m_A[t].to_vec() * np - m_A[s].to_vec() * pp;
                d[i] = m_b[t] * np - m_b[s] * pp;
            }
        }

        return {std::move(D), std::move(d)};
    }

    /**
     * @brief Colapsa uma dimensão do poliedro, projetando-o na direção da
     * dimensão (a.k.a. Eliminação de Fourier–Motzkin).
     *
     * @param coord Índice da dimensão a ser colapsada.
     * @return polyhedron<scalar_type> Um poliedro P(D, d) equivalente a P(A, b)
     * com a dimensão dada colapsada em 0.
     */
    polyhedron<scalar_type> collapse_dimension(size_t coord) const {
        internal::validate("cannot project polyhedron on direction with "
                           "incompatible dimensions",
                           [&]() { return coord < dimensions(); });

        vecn<scalar_type> direction(dimensions());
        direction[coord] = 1;
        return project(direction);
    }

    /**
     * @brief Determina se o poliedro é vazio.
     *
     * @return true se o poliedro for vazio.
     * @return false caso contrário.
     */
    bool empty() const {
        // Eliminamos cada dimensão do poliedro por Fourier-Motzkin.
        polyhedron<scalar_type> projection = collapse_dimension(0);
        for (size_t i = 1; i < dimensions(); i++) {
            projection = projection.collapse_dimension(i);
        }

        // Temos um poliedro P(D, d) tal que D e_i = 0 para todo 0 <= i < n,
        // para n o número de dimensões do poliedro. Como o conjunto
        // {e_0, ..., e_n} forma uma base do espaço vetorial subjacente, D = 0.
        assert(projection.A().zero());

        // Como D = 0, o poliedro é vazio sse existe coordenada negativa em d.
        const auto& d = projection.b();
        for (size_t i = 0; i < d.size(); i++) {
            if (d[i] < 0) {
                return true;
            }
        }
        return false;
    }

  private:
    /**
     * @brief Particiona o conjunto de linhas da matriz A baseado no sinal do
     * produto interno com um vetor direcional dado.
     *
     * @param direction Vetor direcional.
     * @param negative Conjunto de linhas onde o produto interno tem sinal
     * negativo.
     * @param zero Conjunto de linhas onde o produto interno é zero.
     * @param positive Conjunto de linhas onde o produto interno tem sinal
     * positivo.
     */
    void partition_for_projection(const vecn<scalar_type>& direction,
                                  std::vector<size_t>& negative,
                                  std::vector<size_t>& zero,
                                  std::vector<size_t>& positive) const {
        negative.reserve(m_A.rows());
        zero.reserve(m_A.rows());
        positive.reserve(m_A.rows());

        for (size_t i = 0; i < m_A.rows(); i++) {
            scalar_type dot = m_A[i].to_vec() * direction;
            if (dot < 0) {
                negative.push_back(i);
            } else if (dot > 0) {
                positive.push_back(i);
            } else {
                zero.push_back(i);
            }
        }
    }
};
}; // namespace polyhedral

#endif // __POLYHEDRAL__
