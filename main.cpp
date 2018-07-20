#include <numeric>
#include <vector>

#include "ai/ai.hpp"
#include "board/board.hpp"

int main()
{
    using Board::board;
    Board::board = std::make_unique<Board::Board<ROW_SIZE, COL_SIZE>>();
    try {
        // 複数回のゲームでのターン数をまとめておく変数
        std::vector<double> turns{};
        // 風数回のゲームでのスコアをまとめておく変数
        std::vector<double> scores{};
        // ai/ai.cppに実体のある関数。プログラム開始時のAI処理
        AI::init();
        // 予め決められた回数のゲームを行う
        int i = 0;
        double e_greedy = 0.0;
        while (true) {
            // 1ゲーム
            // ai/ai.cppに実体のある関数。ゲームごとのAI初期化処理
            AI::initGame();
            while (true) {
                // ai/ai.cppに実体のある関数。基本この中身だけ変更すれば良い
                AI::chooseMove(e_greedy);

                // 選択された動作に基づいて実際に盤面を上下左右動かし、ターンや点数を更新する
                if (not board->move()) {
                    if constexpr (Params::PRINT_GAMEOVER) {
                        std::cout << "Invalid Move. Game Over." << std::endl;
                    }
                    if constexpr (Params::PRINT_RESULT) {
                        std::cout << "Turn: " << board->turn()
                                  << " Score: " << board->score() << std::endl;
                    }
                    turns.emplace_back(board->turn());
                    scores.emplace_back(board->score());
                    break;
                }
                // 2か4を盤面に加える
                if (not board->placeTwoOrFour()) {
                    if constexpr (Params::PRINT_GAMEOVER) {
                        std::cout << "No space left. Game Over." << std::endl;
                    }
                    if constexpr (Params::PRINT_RESULT) {
                        std::cout << "Turn: " << board->turn()
                                  << " Score: " << board->score() << std::endl;
                    }
                    turns.emplace_back(board->turn());
                    scores.emplace_back(board->score());
                    break;
                }
                // 盤面を動かすとどうなるか先行評価
                board->eagerMoveEvaluation();
            }
            board->clear();
            if (i % 10000 == 0) {
                std::cout << "Weights and Biases" << std::endl;
                AI::printWeight();
                std::cout << "Epsilon_greedy[%]: " << e_greedy << std::endl;
                std::cout << "Total iteration: " << i + 1 << std::endl;
                std::cout << "Turn average: "
                          << std::accumulate(turns.begin(), turns.end(), 0)
                                 / turns.size()
                          << std::endl;
                std::cout << "Score average: "
                          << std::accumulate(scores.begin(), scores.end(), 0)
                                 / scores.size()
                          << std::endl;
                turns.clear();
                scores.clear();
                e_greedy = Util::power(Util::randUniform(0.0, 1.0) - 1, 2U);
                if (Util::randUniform(0.0, 100.0) < 30.0) {
                    e_greedy = 0.0;
                }
            }
            i++;
        }
        std::cout << "Total iteration: " << Params::GAME_ITERATION << std::endl;
        std::cout << "Turn average: "
                  << std::accumulate(turns.begin(), turns.end(), 0)
                         / turns.size()
                  << std::endl;
        std::cout << "Score average: "
                  << std::accumulate(scores.begin(), scores.end(), 0)
                         / scores.size()
                  << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Invalid Error." << e.what() << std::endl;
    }
    return 0;
}
