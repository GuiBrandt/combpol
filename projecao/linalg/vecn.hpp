#ifndef __LINALG_VECN__
#define __LINALG_VECN__

#include <algorithm>
#include <utility>

#include "internal.hpp"

namespace linalg {
/**
 * @brief Classe para vetores sobre um corpo F.
 *
 * @tparam F Tipo de escalar.
 */
template <typename F> class vecn {
  private:
    size_t m_size;
    F* m_coords;

  public:
    using scalar_type = F;
    using reference = scalar_type&;

    vecn() = delete;

    ~vecn() { delete[] m_coords; }

    /**
     * @brief Constrói um vetor zero de tamanho `size`.
     *
     * @param size Tamanho do vetor.
     */
    explicit vecn(size_t size) {
        m_size = size;
        m_coords = new scalar_type[size]();
    }

    /**
     * @brief Move um vetor para um novo vetor.
     *
     * @param other Vetor a ser movido.
     */
    vecn(vecn&& other) {
        m_size = other.m_size;
        std::swap(m_coords, other.m_coords);
    }

    /**
     * @brief Copia um vetor.
     *
     * @param other Vetor a ser copiado.
     */
    vecn(const vecn& other) {
        m_size = other.size();
        m_coords = new scalar_type[m_size];
        std::copy(other.m_coords, other.m_coords + m_size, m_coords);
    }

    /**
     * @brief Inicializa um vetor com uma lista de valores.
     *
     * @param coords Valores das coordenadas do vetor.
     */
    vecn(std::initializer_list<F>&& coords) {
        m_size = coords.size();
        m_coords = new scalar_type[m_size];
        std::move(coords.begin(), coords.end(), m_coords);
    }

    /**
     * @brief Copia um vetor de mesmo tamanho neste.
     *
     * @param other Vetor a ser copiado.
     *
     * @return vecn& Uma referência para este vetor.
     */
    vecn& operator=(const vecn& other) {
        internal::validate("cannot assign vectors of different sizes",
                           [&]() { return other.size() == size(); });

        std::copy(other.m_coords, other.m_coords + other.size(), m_coords);
        return *this;
    }

    /**
     * @brief Move um vetor de mesmo tamanho neste.
     *
     * @param other Vetor a ser copiado.
     *
     * @return vecn& Uma referência para este vetor.
     */
    vecn& operator=(vecn&& other) {
        internal::validate("cannot assign vectors of different sizes",
                           [&]() { return other.size() == size(); });

        std::swap(m_coords, other.m_coords);
        return *this;
    }

    /**
     * @brief Move uma lista de valores de tamanho apropriado neste vetor.
     *
     * @param coords Lista de valores.
     *
     * @return vecn& Uma referência para este vetor.
     */
    vecn& operator=(std::initializer_list<F>&& coords) {
        internal::validate(
            "cannot assign initializer list to vector of different size",
            [&]() { return coords.size() == size(); });

        std::move(coords.begin(), coords.end(), m_coords);
        return *this;
    }

    /**
     * @brief Tamanho do vetor.
     *
     * @return size_t O tamanho do vetor.
     */
    size_t size() const { return m_size; }

    /**
     * @brief Soma de vetores.
     *
     * @param other Outro vetor.
     *
     * @return vecn Vetor resultante da soma deste vetor com `other`.
     */
    vecn operator+(const vecn& other) const {
        internal::validate("cannot add vectors of different sizes",
                           [&]() { return other.size() == size(); });

        vecn result(*this);
        for (size_t i = 0; i < m_size; i++) {
            result[i] = other.m_result[i];
        }
        return result;
    }

    /**
     * @brief Produto vetor-escalar.
     *
     * @param scalar Escalar.
     *
     * @return vecn Vetor resultante do produto deste vetor com o escalar
     * `scalar`.
     */
    vecn operator*(scalar_type scalar) const {
        vecn result(*this);
        for (size_t i = 0; i < m_size; i++) {
            result[i] *= scalar;
        }
        return result;
    }

    /**
     * @brief Produto interno de vetores.
     *
     * @param other Outro vetor.
     *
     * @return F Escalar resultante do produto interno deste vetor e `other`.
     */
    scalar_type operator*(const vecn& other) const {
        internal::validate(
            "cannot take dot product of vectors of different sizes",
            [&]() { return other.size() == size(); });

        F result = 0;
        for (size_t i = 0; i < m_size; i++) {
            result += this->operator[](i) * other[i];
        }
        return result;
    }

    /**
     * @brief Acessa uma coordenada do vetor.
     *
     * @param index Índice da coordenada.
     *
     * @return F& Referência para a coordenada do vetor.
     */
    reference operator[](size_t index) {
        internal::validate("vector index out of bounds",
                           [&]() { return index < size(); });

        return m_coords[index];
    }

    /**
     * @brief Acessa uma coordenada do vetor.
     *
     * @param index Índice da coordenada.
     *
     * @return F Valor da coordenada do vetor.
     */
    scalar_type operator[](size_t index) const {
        internal::validate("vector index out of bounds",
                           [&]() { return index < size(); });

        return m_coords[index];
    }
};
}; // namespace linalg

#endif // __LINALG_VECN__
