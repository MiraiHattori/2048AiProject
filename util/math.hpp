#pragma once
#include <type_traits>

namespace Util
{
template <typename T1, typename T2>
T1 power(const T1& base, const T2& exponent)
{
    static_assert(std::is_unsigned<T1>::value, "T1 must be unsigned integer type.");
    static_assert(std::is_unsigned<T2>::value, "T2 must be unsigned integer type.");
    T1 result = 1;
    for (T1 i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}
}  // namespace Util
