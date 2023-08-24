#ifndef __LINALG_MATNXM__
#define __LINALG_MATNXM__

#include <algorithm>
#include <initializer_list>
#include <type_traits>
#include <utility>

#include "vecn.hpp"
#include <internal.hpp>

namespace linalg {
/**
 * @brief Estrutura auxiliar para seleção de linhas e colunas em matrizes.
 */
struct all_t {};

/**
 * @brief Objeto auxiliar para seleção de linhas e colunas em matrizes.
 */
static constexpr all_t all = {};

/**
 * @brief Classe para matrizes sobre um corpo F.
 *
 * @tparam F Tipo de escalar.
 */
template <typename F> class matnxm {
  private:
    F* m_cells;
    size_t m_rows;
    size_t m_cols;

  public:
    using scalar_type = F;
    using reference = scalar_type&;
    using const_reference = const scalar_type&;

    /**
     * @brief Classe auxiliar para manipulação de linhas em uma matriz.
     */
    template <typename MRef> class row_t {
      private:
        MRef m_matrix;
        size_t m_row;

        row_t(MRef matrix, size_t row) : m_matrix(matrix), m_row(row) {}
        friend class matnxm;

      public:
        using reference = std::conditional_t<
            std::is_const<std::remove_reference_t<MRef>>::value,
            matnxm::const_reference,
            matnxm::reference>;

        row_t() = delete;
        row_t(const row_t&) = default;
        row_t(row_t&&) = default;

        /**
         * @brief Copia a linha para um vetor.
         *
         * @return vecn<F> Vetor correspondente à linha da matriz.
         */
        vecn<scalar_type> vec() const {
            vecn<scalar_type> row(size());
            for (size_t j = 0; j < size(); j++) {
                row[j] = m_matrix(m_row, j);
            }
            return row;
        }

        /**
         * @brief Converte a linha em um vetor.
         *
         * @return vecn<F> Vetor correspondente à linha da matriz.
         */
        vecn<scalar_type> to_vec() && { return vec(); }

        /**
         * @brief Copia a linha para uma matriz.
         *
         * @return matnxm<F> Matriz correspondente à linha da matriz original.
         */
        matnxm mat() const { return matnxm::row(vec()); }

        /**
         * @brief Converte a linha em uma matriz.
         *
         * @return matnxm<F> Matriz correspondente à linha da matriz original.
         */
        matnxm to_mat() && { return matnxm::row(to_vec()); }

        /**
         * @brief Acessa uma coluna da linha.
         *
         * @param col Coluna a ser acessada.
         *
         * @return F Valor da célula correspondente na matriz.
         */
        const_reference operator[](size_t col) const {
            return m_matrix(m_row, col);
        }
        
        /**
         * @brief Acessa uma coluna da linha.
         *
         * @param col Coluna a ser acessada.
         *
         * @return F& Referência para a célula correspondente na matriz.
         */
        reference operator[](size_t col) { return m_matrix(m_row, col); }

        /**
         * @brief Copia um vetor nesta linha, sobrescrevendo a matriz original.
         *
         * @param mrow_t Linha a ser copiada.
         *
         * @return mrow_t& Referência para esta linha.
         */
        row_t& operator=(std::initializer_list<scalar_type>&& coords) {
            return (*this) = vecn<scalar_type>(std::move(coords));
        }

        /**
         * @brief Copia um vetor, linha ou coluna nesta linha, sobrescrevendo a
         * matriz original.
         *
         * @param mrow_t Vetor, linha ou coluna a ser copiado.
         *
         * @return mrow_t& Referência para esta linha.
         */
        template <typename VecLike> row_t& operator=(const VecLike& other) {
            internal::validate(
                "cannot assign vector-like to row of different size",
                [&]() { return size() == other.size(); });

            for (size_t j = 0; j < size(); j++) {
                this->operator[](j) = other[j];
            }
            return *this;
        }

        /**
         * @brief Tamanho da linha.
         *
         * @return size_t O tamanho da linha.
         */
        size_t size() const { return m_matrix.cols(); }
    };

    /**
     * @brief Classe auxiliar para manipulação de colunas em uma matriz.
     */
    template <typename MRef> class column_t {
      private:
        MRef m_matrix;
        size_t m_col;

        column_t(MRef matrix, size_t col) : m_matrix(matrix), m_col(col) {}
        friend class matnxm;

      public:
        using reference = std::conditional_t<
            std::is_const<std::remove_reference_t<MRef>>::value,
            matnxm::const_reference,
            matnxm::reference>;
            
        column_t() = delete;
        column_t(const column_t&) = default;
        column_t(column_t&&) = default;

        /**
         * @brief Copia a coluna para um vetor.
         *
         * @return vecn<F> Vetor correspondente à coluna da matriz.
         */
        vecn<scalar_type> vec() const {
            vecn<scalar_type> col(size());
            for (size_t i = 0; i < size(); i++) {
                col[i] = m_matrix(i, m_col);
            }
            return col;
        }

        /**
         * @brief Converte a coluna para um vetor.
         *
         * @return vecn<F> Vetor correspondente à coluna da matriz.
         */
        vecn<scalar_type> to_vec() && { return vec(); }

        /**
         * @brief Copia a coluna para uma matriz.
         *
         * @return matnxm<F> Matriz correspondente à coluna da matriz original.
         */
        matnxm mat() const { return matnxm::column(vec()); }

        /**
         * @brief Converte a coluna em uma matriz.
         *
         * @return matnxm<F> Matriz correspondente à coluna da matriz original.
         */
        matnxm to_mat() && { return matnxm::column(to_vec()); }

        /**
         * @brief Acessa uma linha da coluna.
         *
         * @param row Linha a ser acessada.
         *
         * @return F Valor da célula correspondente na matriz.
         */
        const_reference operator[](size_t row) const {
            return m_matrix(row, m_col);
        }

        /**
         * @brief Acessa uma linha da coluna.
         *
         * @param row Linha a ser acessada.
         *
         * @return F& Referência para a célula correspondente na matriz.
         */
        reference operator[](size_t row) { return m_matrix(row, m_col); }

        /**
         * @brief Copia um vetor nesta coluna, sobrescrevendo a matriz original.
         *
         * @param vec<F> Vetor a ser copiado.
         *
         * @return mcol_t& Referência para esta coluna.
         */
        column_t& operator=(std::initializer_list<scalar_type>&& coords) {
            return (*this) = vecn<scalar_type>(std::move(coords));
        }

        /**
         * @brief Copia um vetor, linha ou coluna nesta coluna, sobrescrevendo a
         * matriz original.
         *
         * @param mcol_t Vetor, linha ou coluna a ser copiada.
         *
         * @return mcol_t& Referência para esta coluna.
         */
        template <typename VecLike> column_t& operator=(const VecLike& other) {
            internal::validate(
                "cannot assign vector-like to column of different size",
                [&]() { return size() == other.size(); });

            for (size_t i = 0; i < size(); i++) {
                this->operator[](i) = other[i];
            }
            return *this;
        }

        /**
         * @brief Tamanho da linha.
         *
         * @return size_t O tamanho da linha.
         */
        size_t size() const { return m_matrix.rows(); }
    };

    using column_type = column_t<matnxm&>;
    using const_column_type = column_t<const matnxm&>;

    using row_type = row_t<matnxm&>;
    using const_row_type = row_t<const matnxm&>;

    matnxm() : m_cells(nullptr), m_rows(0), m_cols(0) {}

    ~matnxm() { delete[] m_cells; }

    matnxm(matnxm&& other) {
        std::swap(other.m_cells, m_cells);
        m_rows = other.m_rows;
        m_cols = other.m_cols;
    }

    matnxm(const matnxm& other) {
        m_rows = other.rows();
        m_cols = other.cols();
        m_cells = new scalar_type[other.rows() * other.cols()];
        std::copy(other.m_cells, other.m_cells + m_rows * m_cols, m_cells);
    }

    matnxm& operator=(const matnxm& other) {
        internal::validate("cannot assign matrices of different sizes", [&]() {
            return rows() == other.rows() && cols() == other.cols();
        });

        std::copy(other.m_cells, other.m_cells + other.rows() * other.cols(), m_cells);
        return *this;
    }

    matnxm& operator=(matnxm&& other) {
        std::swap(m_cells, other.m_cells);
        m_rows = other.m_rows;
        m_cols = other.m_cols;
        return *this;
    }

    /**
     * @brief Constrói uma matriz zero com `rows` linhas e `cols` colunas.
     *
     * @param rows Número de linhas da matriz.
     * @param cols Número de colunas da matriz.
     */
    matnxm(size_t rows, size_t cols) {
        m_rows = rows;
        m_cols = cols;
        m_cells = new scalar_type[rows * cols]();
    }

    /**
     * @brief Número de linhas da matriz.
     *
     * @return size_t O número de linhas da matriz.
     */
    size_t rows() const { return m_rows; }

    /**
     * @brief Número de colunas da matriz.
     *
     * @return size_t O número de colunas da matriz.
     */
    size_t cols() const { return m_cols; }

    /**
     * @brief Lê uma célula da matriz.
     *
     * @param row Linha da célula desejada.
     * @param col Coluna da célula desejada.
     *
     * @return F& Referência para a célula da matriz.
     */
    reference operator()(size_t row, size_t col) {
        internal::validate("matrix cell out of bounds",
                           [&]() { return row < m_rows && col < m_cols; });

        return m_cells[col + row * m_cols];
    }

    /**
     * @brief Lê uma célula da matriz.
     *
     * @param row Linha da célula desejada.
     * @param col Coluna da célula desejada.
     *
     * @return F Valor da célula na matriz.
     */
    const_reference operator()(size_t row, size_t col) const {
        internal::validate("matrix cell out of bounds",
                           [&]() { return row < m_rows && col < m_cols; });

        return m_cells[col + row * m_cols];
    }

    /**
     * @brief Acessa uma coluna da matriz.
     *
     * @param col Coluna desejada.
     *
     * @return const_mcol Uma referência imutável para a coluna na matriz.
     */
    const_column_type operator()(all_t, size_t col) const {
        internal::validate("column index out of bounds",
                           [&]() { return col < cols(); });

        return const_column_type(*this, col);
    }

    /**
     * @brief Acessa uma coluna da matriz.
     *
     * @param col Coluna desejada.
     *
     * @return mcol Uma referência para a coluna na matriz.
     */
    column_type operator()(all_t, size_t col) {
        internal::validate("column index out of bounds",
                           [&]() { return col < cols(); });

        return column_type(*this, col);
    }

    /**
     * @brief Acessa uma linha da matriz.
     *
     * @param row Linha desejada.
     *
     * @return const_mrow Uma referência imutável para a linha na matriz.
     */
    const_row_type operator()(size_t row, all_t) const {
        internal::validate("row index out of bounds",
                           [&]() { return row < rows(); });

        return const_row_type(*this, row);
    }

    /**
     * @brief Acessa uma linha da matriz.
     *
     * @param row Linha desejada.
     *
     * @return mrow Uma referência para a linha na matriz.
     */
    row_type operator()(size_t row, all_t) {
        internal::validate("row index out of bounds",
                           [&]() { return row < rows(); });

        return row_type(*this, row);
    }

    /**
     * @brief Acessa uma linha da matriz.
     *
     * @param row Linha desejada.
     *
     * @return const_mrow Uma referência imutável para a linha na matriz.
     */
    const_row_type operator[](size_t row) const {
        return this->operator()(row, all);
    }

    /**
     * @brief Acessa uma linha da matriz.
     *
     * @param row Linha desejada.
     *
     * @return mrow Uma referência para a linha na matriz.
     */
    row_type operator[](size_t row) { return this->operator()(row, all); }

    /**
     * @brief Soma de matrizes.
     *
     * @param other Outra matriz.
     *
     * @return mrow Matriz resultante da soma desta matriz com `other`.
     */
    matnxm operator+(const matnxm& other) const {
        internal::validate("cannot add matrices of different sizes", [&]() {
            return other.rows() == rows() && other.cols() == cols();
        });

        matnxm result(*this);
        for (size_t i = 0; i < rows(); i++) {
            for (size_t j = 0; j < cols(); j++) {
                result(i, j) += other(i, j);
            }
        }
        return result;
    }

    /**
     * @brief Produto matriz-escalar.
     *
     * @param scalar Escalar.
     *
     * @return matnxm Matriz resultante do produto desta matriz com o escalar
     * `scalar`.
     */
    matnxm operator*(scalar_type scalar) const {
        matnxm result(*this);
        for (size_t i = 0; i < rows(); i++) {
            for (size_t j = 0; j < cols(); j++) {
                result(i, j) *= scalar;
            }
        }
        return result;
    }

    /**
     * @brief Multiplicação de matrizes.
     *
     * @param other Outra matriz.
     *
     * @return matnxm Matriz resultante da multiplicação desta matriz com
     * `other`.
     */
    matnxm operator*(const matnxm& other) const {
        internal::validate(
            "cannot multiply matrices of incompatible dimensions",
            [&]() { return cols() == other.rows(); });

        matnxm result(rows(), other.cols());
        for (size_t i = 0; i < rows(); i++) {
            for (size_t j = 0; j < other.cols(); j++) {
                result(i, j) =
                    this->operator()(i, all).to_vec() * other(all, j).to_vec();
            }
        }
        return result;
    }

    /**
     * @brief Produto matriz-vetor.
     *
     * @param other Vetor.
     *
     * @return vecn<F> Vetor resultante da multiplicação desta matriz com
     * o vetor-coluna `other`.
     */
    vecn<scalar_type> operator*(const vecn<scalar_type>& other) const {
        return ((*this) * matnxm::column(other))(all, 0).to_vec();
    }

    /**
     * @brief Transposta da matriz.
     *
     * @return matnxm Matriz transposta desta matriz.
     */
    matnxm T() const {
        matnxm result(cols(), rows());
        for (size_t j = 0; j < cols(); j++) {
            for (size_t i = 0; i < rows(); i++) {
                result(j, i) = this->operator()(i, j);
            }
        }
        return result;
    }

    /**
     * @brief Determina se a matriz é a matriz 0.
     *
     * @return true se toda célula na matriz é igual a 0.
     * @return false caso contrário.
     */
    bool zero() const {
        for (size_t i = 0; i < rows(); i++) {
            for (size_t j = 0; j < cols(); j++) {
                if (this->operator()(i, j) != 0) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @brief Constrói uma matriz-coluna a partir de um vetor.
     *
     * @param vec Vetor.
     *
     * @return matnxm Matriz-coluna correspondente a `vec`.
     */
    static matnxm column(const vecn<scalar_type>& vec) {
        matnxm result(vec.size(), 1);
        result(all, 0) = vec;
        return result;
    }

    /**
     * @brief Constrói uma matriz-linha a partir de um vetor.
     *
     * @param vec Vetor.
     *
     * @return matnxm Matriz-linha correspondente a `vec`.
     */
    static matnxm row(const vecn<scalar_type>& vec) {
        matnxm result(1, vec.size());
        result(0, all) = vec;
        return result;
    }

    /**
     * @brief Constrói uma matriz identidade com tamanho dado.
     *
     * @param size Tamanho dos lados da matriz.
     *
     * @return matnxm Matriz identidade de tamanho `size`.
     */
    static matnxm identity(size_t size) {
        matnxm result(size, size);
        for (size_t i = 0; i < size; i++) {
            result(i, i) = 1;
        }
        return result;
    }
};
}; // namespace linalg

#endif // __LINALG_MATNXM__
