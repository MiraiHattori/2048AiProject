#include "board/board.hpp"


template <typename T>
void flush(const T& arr_)
{
    for (const auto& elem : arr_) {
        std::cout << elem;
    }
    std::cout << " -> ";
    T arr;
    arr = Board::board->flush(arr_);
    for (const auto& elem : arr) {
        std::cout << elem;
    }
    std::cout << std::endl;
}

int main()
{
    Board::board.reset();
    std::array<int, 4> arr_0({0, 1, 1, 1});
    std::array<int, 4> arr_1({1, 1, 1, 1});
    std::array<int, 4> arr_2({1, 0, 1, 1});
    std::array<int, 4> arr_3({1, 0, 1, 0});
    std::array<int, 4> arr_4({1, 0, 0, 1});
    std::array<int, 4> arr_5({1, 2, 3, 4});
    std::array<int, 4> arr_6({1, 2, 1, 4});
    std::array<int, 4> arr_7({0, 0, 1, 0});
    std::array<int, 4> arr_8({0, 0, 1, 1});
    std::array<int, 4> arr_9({0, 1, 0, 1});
    std::array<int, 4> arr_10({1, 0, 1, 0});
    std::array<int, 5> arr_11({1, 0, 0, 0, 1});
    std::array<int, 4> arr_12({0, 1, 0, 1});
    std::array<int, 4> arr_13({0, 1, 1, 0});
    flush(arr_0);
    flush(arr_1);
    flush(arr_2);
    flush(arr_3);
    flush(arr_4);
    flush(arr_5);
    flush(arr_6);
    flush(arr_7);
    flush(arr_8);
    flush(arr_9);
    flush(arr_10);
    flush(arr_11);
    flush(arr_12);
    flush(arr_13);
    return 0;
}
