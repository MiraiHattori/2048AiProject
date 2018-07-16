#include "ai/ai.hpp"

#include <iomanip>
#include <iostream>

#include <Eigen/Core>

#include "board/board.hpp"

namespace AI
{

void init()
{
    // initialization
}

void initGame()
{
    // initialization
}

void chooseMove()
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
     * board->boardArrayIfUp(),
     * board->boardArrayIfLeft(),
     * board->boardArrayIfDown(),
     * board->boardArrayIfRight()
     * などの関数で移動後の盤面が const std::array<std::array<int, COL_SIZE>, ROW_SIZE>& 型で取得できる
     * board->scoreIfUp(),
     * board->scoreIfLeft(),
     * board->scoreIfDown(),
     * board->scoreIfRight()
     * などの関数で移動したときの追加点が取得できる
    */

    if (board->boardArrayIfUp() != board->boardArray()) {
        board->up();
    } else {
        if (board->boardArrayIfLeft() != board->boardArray()) {
            board->left();
        } else {
            if (board->boardArrayIfDown() != board->boardArray()) {
                board->down();
            } else {
                board->right();
            }
        }
    }
}
}  // namespace AI
