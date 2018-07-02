#include "board/board.hpp"

template <typename T>
T flush(const T& arr_)
{
    for (const auto& e : arr_) {
        std::cout << e;
    }
    std::cout << std::endl;
    T arr{};
    // 先頭から走査する
    // 左詰めするときのindex
    int arr_index = 0;
    int cand = 0;
    for (unsigned int i = 0; i < arr_.size(); i++) {
        if (arr_[i] == 0) {
            continue;
        } else {
            if (arr_[i] != cand) {
                if (cand != 0) {
                    arr_index++;
                }
                arr[arr_index] = arr_[i];
                cand = arr_[i];
            } else {
                arr[arr_index] = cand + 1;
                cand = 0;
                arr_index++;
            }
        }
    }
    for (const auto& e : arr) {
        std::cout << e;
    }
    std::cout << std::endl;
    return arr;
}

int main()
{
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
}
