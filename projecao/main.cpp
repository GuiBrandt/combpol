#include <linalg.hpp>
#include <polyhedral.hpp>

#include <iostream>

using namespace linalg;
using namespace polyhedral;

int main(int argc, char** argv) {
    matnxm<int> A(7, 2);
    A[0] = {1, 2};
    A[1] = {3, 2};
    A[2] = {5, 1};
    A[3] = {0, 1};
    A[4] = {-3, -1};
    A[5] = {-1, 0};
    A[6] = {0, -1};

    vecn<int> b = {5, 10, 15, 2, -2, 0, 0};

    polyhedron<int> P(A, b);
    std::cout << "P empty? " << (P.empty() ? "yes" : "no") << std::endl;

    return 0;
}
