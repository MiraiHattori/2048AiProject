#pragma once

#include <random>

namespace Util
{

template <typename T>
T randUniform(T min, T max)
{
    static std::random_device seed;
    static std::mt19937 rand_generator(seed());
    std::uniform_real_distribution<double> rand(min, max - std::numeric_limits<double>::epsilon());
    if constexpr (std::numeric_limits<T>::is_integer) {
        return static_cast<int>(rand(rand_generator));
    } else {
        return rand(rand_generator);
    }
}

}  // namespace Util
