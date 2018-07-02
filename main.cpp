#include <iomanip>
#include <iostream>
#include <memory>

#include <Eigen/Core>

#include "config/params.hpp"
#include "board/board.hpp"

int main()
{
    try {
        std::unique_ptr<Board<4, 4>> board = std::make_unique<Board<4, 4>>();
        while (true) {
            // board->boardArray()で、arrayのarrayが返ってきます
            if (DEBUG_PRINTBOARD) {
                for (const auto& row : board->boardArray()) {
                    for (const auto& cell : row) {
                        std::cout << std::fixed << std::setw(MAX_DIGIT_TO_SHOW) << cell;
                    }
                    std::cout << std::endl;
                }
            }
            board->down();
            /*
            board->boardArrayIfUp();
            board->boardArrayIfLeft();
            board->boardArrayIfDown();
            board->boardArrayIfRight();
            board->up();
            board->left();
            board->down();
            board->right();
            */
            // 選択された動作に基づいて実際に盤面を上下左右動かし、ターンや点数を更新する
            if (not board->update()) {
                std::cout << "Invalid Move. Game Over." << std::endl;
                break;
            }
            // 2か4を盤面に加える
            if (not board->placeTwoOrFour()) {
                std::cout << "No space left. Game Over." << std::endl;
                break;
            }
            // 盤面を動かすとどうなるか遅延評価
            board->eagerEvaluationIfMoved();
        }
    } catch (const std::exception& e) {
        std::cout << "Invalid Error." << e.what() << std::endl;
    }
    return 0;
}
