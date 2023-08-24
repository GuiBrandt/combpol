#include "cli/parser.hpp"
#include <cli.hpp>
#include <exception>
#include <linalg.hpp>
#include <polyhedral.hpp>

#include <iostream>

using namespace linalg;
using namespace polyhedral;
using namespace cli;

int main(int argc, char** argv) {
    // matnxm<int> A(7, 2);
    // A[0] = {1, 2};
    // A[1] = {3, 2};
    // A[2] = {5, 1};
    // A[3] = {0, 1};
    // A[4] = {-3, -1};
    // A[5] = {-1, 0};
    // A[6] = {0, -1};

    // vecn<int> b = {5, 10, 15, 2, -2, 0, 0};

    polyhedron<int> P;

    while (1) {
        try {
            std::cin >> P;
            if (P.dimensions() == 0) {
                break;
            }

            std::cout << "(P) " << P.A().rows() << " x " << P.A().cols()
                      << std::endl;
            std::cout << P << std::endl;
            std::cout << "P empty? " << (P.empty() ? "yes" : "no") << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }
    }

    return 0;
}
