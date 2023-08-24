#include <linalg.hpp>
#include <polyhedral.hpp>

#include <iostream>

using namespace linalg;
using namespace polyhedral;

int main(int argc, char** argv) {

    matnxm<int> A(2, 2);
    A[0] = {1, 2};
    A[1] = {3, 2};

    vecn<int> b = {1, 1};

    polyhedron<int> P(A, b);
    std::cout << "P empty? " << (P.empty() ? "yes" : "no") << std::endl;

    return 0;
}
