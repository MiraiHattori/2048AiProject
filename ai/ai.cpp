#include "ai/ai.hpp"

#include <iomanip>
#include <iostream>

#include <Eigen/Core>

#include "board/board.hpp"

namespace AI
{
void ai()
{
    using Board::board;
    using Params::ROW_SIZE;
    using Params::COL_SIZE;

    // board->boardArray()で、2048の現在の盤面がstd::array<std::array<int, COL_SIZE>, ROW_SIZE>で返ってきます
    if constexpr (Params::PRINT_BOARD) {
        for (const auto& row : board->boardArray()) {
            for (const auto& cell : row) {
                std::cout << std::fixed
                          << std::setw(Params::MAX_DIGIT_TO_SHOW) << (cell == 0 ? 0 : Util::power(2u, static_cast<unsigned int>(cell)));
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    if constexpr (Params::PRINT_TURN) {
        std::cout << "turn: " << board->turn() << std::endl;
    }
    if constexpr (Params::PRINT_SCORE) {
        std::cout << "score: " << board->score() << std::endl;
    }

    /*
     * 上下左右に移動した後の盤面を先行評価してある
     * boardArrayIfUp(),
     * boardArrayIfLeft(),
     * boardArrayIfDown(),
     * boardArrayIfRight()
     * などの関数で移動後の盤面が取得できる
     * scoreIfUp(),
     * scoreIfLeft(),
     * scoreIfDown(),
     * scoreIfRight()
     * などの関数で移動後の追加点が取得できる
    */
}
}  // namespace AI
