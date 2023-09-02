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
    polyhedron<int> P;
    vecn<int> c;

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
            
            std::cin >> c;
            std::cout << std::endl << "(c) 1 x " << c.size() << std::endl;
            std::cout << c << std::endl << std::endl;
            
            polyhedron<int> P_H = P.project(c);
            std::cout << "(P_H)" << std::endl;
            std::cout << P.project(c) << std::endl;            
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }
    }

    return 0;
}
