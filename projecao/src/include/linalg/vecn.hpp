#ifndef __LINALG_VECN__
#define __LINALG_VECN__

#include <algorithm>
#include <memory>
#include <utility>

#include <internal.hpp>

namespace linalg {
/**
 * @brief Classe para vetores sobre um corpo F.
 *
 * @tparam F Tipo de escalar.
 */
template <typename F> class vecn {
  private:
    size_t m_size;
    std::unique_ptr<F[]> m_coords;

  public:
    using scalar_type = F;
    using reference = scalar_type&;

    vecn() = default;

    vecn(vecn&& other) = default;
    vecn& operator=(vecn&& other) = default;

    vecn(const vecn& other) { *this = other; }

    vecn& operator=(const vecn& other) {
        m_size = other.m_size;
        m_coords = std::make_unique<F[]>(m_size);
        std::copy(other.m_coords.get(), other.m_coords.get() + m_size,
                  m_coords.get());
        return *this;
    }

    vecn(std::initializer_list<F>&& coords) {
        m_size = coords.size();
        m_coords = std::make_unique<F[]>(m_size);
        std::move(coords.begin(), coords.end(), m_coords.get());
    }

    vecn& operator=(std::initializer_list<F>&& coords) {
        std::move(coords.begin(), coords.end(), m_coords.get());
        m_size = coords.size();
        return *this;
    }

    /**
     * @brief Constrói um vetor zero de tamanho `size`.
     *
     * @param size Tamanho do vetor.
     */
    explicit vecn(size_t size) {
        m_size = size;
        m_coords = std::make_unique<F[]>(size);
    }

    /**
     * @brief Constrói um vetor constante de tamanho `size`.
     *
     * @param size Tamanho do vetor.
     * @param value Valor copiado em cada coordenada do vetor.
     */
    vecn(size_t size, scalar_type value) {
        m_size = size;
        m_coords = new scalar_type[size];
        std::fill(m_coords, m_coords + size, value);
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
            result[i] += other[i];
        }
        return result;
    }

    /**
     * @brief Inverso aditivo do vetor.
     *
     * @return vecn O inverso aditivo do vetor original.
     */
    vecn operator-() const {
        vecn result(*this);
        for (size_t i = 0; i < m_size; i++) {
            result[i] *= -1;
        }
        return result;
    }

    /**
     * @brief Inverso aditivo do vetor.
     *
     * @return vecn O inverso aditivo do vetor original.
     */
    vecn operator-(const vecn& other) const {
        internal::validate("cannot subtract vectors of different sizes",
                           [&]() { return other.size() == size(); });

        vecn result(*this);
        for (size_t i = 0; i < m_size; i++) {
            result[i] -= other[i];
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

    /**
     * @brief Determina se o vetor é o vetor 0.
     *
     * @return true se toda coordenada no vetor é igual a 0.
     * @return false caso contrário.
     */
    bool zero() const {
        for (size_t i = 0; i < size(); i++) {
            if (this->operator[](i) != 0) {
                return false;
            }
        }
        return true;
    }
};
}; // namespace linalg

#endif // __LINALG_VECN__
