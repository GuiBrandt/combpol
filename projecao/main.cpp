#include "linalg/linalg.hpp"

#include <iostream>

using namespace linalg;

int main(int argc, char** argv) {
    vecn<int> v = {1, 1};
    std::cout << "vec " << v.size() << " = " << v[0] << " " << v[1]
              << std::endl;

    matnxm<int> A(2, 2);
    A[0] = {1, 2};
    A[1] = {3, 2};
    std::cout << "mat " << A.rows() << " " << A.cols() << " = " << A(0, 0)
              << " " << A(0, 1) << " / " << A(1, 0) << " " << A(1, 1)
              << std::endl;

    auto r = A * v;

    std::cout << "vec " << r.size() << " = " << r[0] << " " << r[1]
              << std::endl;

    return 0;
}
