#include "util/random.hpp"

#include <iostream>

int main()
{
    std::cout << "randUniform(0, 5)" << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << Util::randUniform(0, 5) << std::endl;
    }
    std::cout << "randUniform(0.0, 5.0)" << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << Util::randUniform(0.0, 5.0) << std::endl;
    }
    return 0;
}