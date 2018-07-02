#include <iomanip>
#include <iostream>

#include <Eigen/Core>

#include "board/board.hpp"

int main()
{
    using Board::board;
    using Params::DEBUG_PRINTBOARD;
    using Params::ROW_SIZE;
    using Params::COL_SIZE;
    board = std::make_unique<Board::Board<ROW_SIZE, COL_SIZE>>();
    try {
        while (true) {
            // board->boardArray()で、arrayのarray(ROW * COLUMN)が返ってきます
            if (DEBUG_PRINTBOARD) {
                for (const auto& row : board->boardArray()) {
                    for (const auto& cell : row) {
                        std::cout << std::fixed
                                  << std::setw(Params::MAX_DIGIT_TO_SHOW) << cell;
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }

            /*
             * boardArrayIfUp(),
             * boardArrayIfLeft(),
             * boardArrayIfDown(),
             * boardArrayIfRight()などの関数で
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
            // 選択された動作に基づいて実際に盤面を上下左右動かし、ターンや点数を更新する
            if (not board->move()) {
                std::cout << "Invalid Move. Game Over." << std::endl;
                break;
            }
            // 2か4を盤面に加える
            if (not board->placeTwoOrFour()) {
                std::cout << "No space left. Game Over." << std::endl;
                break;
            }
            // 盤面を動かすとどうなるか先行評価
            board->eagerMoveEvaluation();
        }
    } catch (const std::exception& e) {
        std::cout << "Invalid Error." << e.what() << std::endl;
    }
    return 0;
}
