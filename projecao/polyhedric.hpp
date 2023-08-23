#ifndef __POLYHEDRIC__
#define __POLYHEDRIC__

#include "linalg/linalg.hpp"

namespace polyhedric {
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
    polyhedron() = delete;

    ~polyhedron() = default;

    polyhedron(const polyhedron&) = default;
    polyhedron(polyhedron&&) = default;

    polyhedron& operator=(const polyhedron&) = default;
    polyhedron& operator=(polyhedron&&) = default;

    polyhedron(const matnxm<F>& A, const vecn<F>& b) : m_A(A), m_b(b){};
    polyhedron(matnxm<F>&& A, vecn<F>&& b) : m_A(A), m_b(b){};
};
}; // namespace polyhedric

#endif // __POLYHEDRIC__
