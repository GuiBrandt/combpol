#include "io/parser.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>

#include <io.hpp>
#include <iterator>
#include <linalg.hpp>
#include <polyhedral.hpp>
#include <string>

using namespace io;
using namespace linalg;
using namespace polyhedral;

using scalar_type = double;

class file_processor {
  private:
    const char* m_filename;
    std::ifstream m_stream;
    size_t lineno = 1;

  public:
    file_processor(const char* filename) : m_filename(filename) {}

    void run() {
        std::cout << "[" << m_filename << "]" << std::endl;
        try {
            m_stream.open(m_filename);
        } catch (std::exception& ex) {
            std::cerr << "Could not read file: " << ex.what() << std::endl;
            return;
        }

        polyhedron<scalar_type> P;
        read_polyhedron(P);

        std::cout << "(P) " << P.A().rows() << " x " << P.A().cols()
                  << std::endl
                  << P << std::endl;

        std::cout << "P is" << (P.empty() ? " " : " not ") << "empty"
                  << std::endl
                  << std::endl;

        vecn<scalar_type> c;
        while (read_vector(c)) {
            std::cout << "Projected on direction " << c << ":" << std::endl;
            std::cout << P.project(c) << std::endl;
        }
    }

  private:
    void read_polyhedron(polyhedron<scalar_type>& P) {
        std::vector<parser::linear_inequality<scalar_type>> inequalities;

        // Lê as inequações que definem o poliedro, uma por linha, e computa
        // o número de colunas da matriz (i.e. o número de variáveis no
        // sistema).
        size_t n = 0;
        for (std::string line; std::getline(m_stream, line); lineno++) {
            // Termina a leitura quando encontra uma linha em branco
            if (line.find_first_not_of(' ') == std::string::npos) {
                break;
            }

            parser::linear_inequality<scalar_type> inequality;
            parser::parser<scalar_type>(m_filename, lineno, line.begin(),
                                        line.end())
                .parse_linear_inequality(line.begin(), line.end(), inequality);

            n = std::max(n, inequality.lhs.max_variable + 1);
            inequalities.push_back(std::move(inequality));
        }

        // Constrói a matriz A e vetor b correspondentes às inequações
        // lidas.
        auto m = inequalities.size();
        matnxm<scalar_type> A(m, n);
        vecn<scalar_type> b(m);

        for (size_t i = 0; i < m; i++) {
            const auto& inequality = inequalities[i];
            for (const auto& m : inequality.lhs.parts) {
                A(i, m.variable) = m.coefficient;
            }
            b[i] = inequality.rhs;
        }

        P = {std::move(A), std::move(b)};
    }

    bool read_vector(vecn<scalar_type>& c) {
        std::string line;
        if (!std::getline(m_stream, line) ||
            parser::skip_blank(line.cbegin(), line.cend()) == line.cend()) {
            return false;
        }

        lineno++;

        std::vector<scalar_type> coords;
        parser::parser<scalar_type>(m_filename, lineno, line.begin(),
                                    line.end())
            .parse_vector(line.begin(), line.end(), coords);

        c = vecn<scalar_type>(coords.size());
        for (size_t i = 0; i < coords.size(); i++) {
            c[i] = coords[i];
        }
        return true;
    }
};

int main(int argc, char** argv) {
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << " files..." << std::endl;
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        file_processor processor(argv[i]);
        try {
            processor.run();
        } catch (std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }
    }

    return 0;
}
