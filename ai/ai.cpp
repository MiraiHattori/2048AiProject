#include "ai/ai.hpp"

#include <iomanip>
#include <iostream>
#include <limits>

#include <Eigen/Core>

#include "ai/full_connected_network.hpp"
#include "board/board.hpp"
#include "util/random.hpp"

namespace AI
{

/* example
    std::shared_ptr<GD> gd = std::make_shared<GD>(0.01);
    FullConnectedNetwork<SoftmaxWithLoss<4, 4>, Affine<4, 10>, Relu<10, 10>, Affine<10, 4>> net{gd};
    for (int i = 0; i < 100000; i++) {
        net.learn(x0, t0);
        net.learn(x1, t1);
    }
    std::cout << net.predict(x0) << std::endl;
*/

std::shared_ptr<GD> gd = nullptr;
using QNet = FullConnectedNetwork<SoftmaxWithLoss<4, 4>, Affine<4, 10>, Relu<10, 10>, Affine<10, 4>>;
std::unique_ptr<QNet> q = nullptr;

void init()
{
    // initialization
    gd = std::make_shared<GD>(0.01);
    q = std::make_unique<QNet>(gd);
}

void initGame()
{
    // initialization
}

void chooseMove()
{
    using Board::board;
    using Board::Manipulation;
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

    /*
     * q(s, a)にsを入れて
     * percentage = randUniform(0.0, 100.0);
     * if (percentage < EPSILON) {
     *     a = random;
     * }
     * target = r(s, a) + 0.99 * q(s', a');
     * loss = target - q(s, a);
     * q(s, a) = q(s, a) + 0.1 * loss;
     */

    // NNに入れる状態s(ROW_SIZE * COL_SIZE次元のEigen::VectorXd)に変換
    Eigen::VectorXd s = Eigen::VectorXd::Zero(ROW_SIZE * COL_SIZE);
    const auto& tmp = board->boardArray();
    for (std::size_t i = 0; i < ROW_SIZE; i++) {
        for (std::size_t j = 0; i < COL_SIZE; j++) {
            s[i * COL_SIZE + j] = tmp[i][j];
        }
    }
    /*
     * 0: Up, 1: Left, 2: Down, 3:Right
     */
    Eigen::VectorXd/* 4d */ acts = q->predict(s);
    double max = std::numeric_limits<double>::min();
    Manipulation manip = Manipulation::None;
    for (int i = 0; i < acts.size(); i++) {
        if (max < acts[i]) {
            max = acts[i];
            manip = static_cast<Manipulation>(i);
        }
    }
    double percentage = Util::randUniform(0.0, 100.0);
    if (percentage < 10) {
        manip = static_cast<Manipulation>(Util::randUniform(0, 4));
    }

}


}  // namespace AI
