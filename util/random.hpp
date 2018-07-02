#pragma once

#include <random>


template <typename T>
T randUniform(T min, T max)
{
    static std::random_device seed;
    static std::mt19937 rand_generator(seed());
    std::uniform_real_distribution<double> rand(min, max);
    return static_cast<int>(rand(rand_generator));
}
