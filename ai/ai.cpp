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

std::shared_ptr<GD> gd = nullptr;
using QNet = FullConnectedNetwork<SoftmaxWithLoss<4, 4>, Affine<16, 10>, Relu<10, 10>, Affine<10, 4>>;
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

Eigen::VectorXd boardArrayToEigenVec(const std::array<std::array<int, Params::COL_SIZE>, Params::ROW_SIZE>& arr_)
{
    Eigen::VectorXd arr = Eigen::VectorXd::Zero(Params::COL_SIZE * Params::ROW_SIZE);
    for (std::size_t i = 0; i < ROW_SIZE; i++) {
        for (std::size_t j = 0; j < COL_SIZE; j++) {
            arr[i * COL_SIZE + j] = arr_[i][j];
        }
    }
    return arr;
}

void chooseMove()
{
    // std::cout << "###########" << std::endl;
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
     * q(s, a) = q(s, a) + lambda * loss;
     */

    // NNに入れる状態s(ROW_SIZE * COL_SIZE次元のEigen::VectorXd)に変換
    Eigen::VectorXd s = boardArrayToEigenVec(board->boardArray());
    /*
     * 0: Up, 1: Left, 2: Down, 3:Right
     */
    Eigen::VectorXd /* 4d */ acts = q->predict(s);
    // std::cout << "s: " << s.transpose() << std::endl;
    double max = std::numeric_limits<double>::min();
    Manipulation manip = Manipulation::None;
    // std::cout << "acts: " << acts.transpose() << std::endl;
    std::array<std::array<std::array<int, COL_SIZE>, ROW_SIZE>, 4> board_array_ifs;
    board_array_ifs[0] = board->boardArrayIfUp();
    board_array_ifs[1] = board->boardArrayIfLeft();
    board_array_ifs[2] = board->boardArrayIfDown();
    board_array_ifs[3] = board->boardArrayIfRight();
    for (int i = 0; i < acts.size(); i++) {
        if (max < acts[i] and board->boardArray() != board_array_ifs[i]) {
            max = acts[i];
            manip = static_cast<Manipulation>(i + 1);
        }
    }
    // epsilon-greedy でときどきランダムな行動
    if (Util::randUniform(0.0, 100.0) < 10.0) {
        manip = static_cast<Manipulation>(Util::randUniform(1, 5));
    }
    // std::cout << "manip: " << static_cast<int>(manip) << std::endl;
    switch (manip) {
    case Manipulation::Up:
        board->up();
        break;
    case Manipulation::Left:
        board->left();
        break;
    case Manipulation::Down:
        board->down();
        break;
    case Manipulation::Right:
        board->right();
        break;
    default:
        std::cout << "Unknown Manipulation detected; manip is "
                  << static_cast<int>(manip) << std::endl;
        break;
    }
    Eigen::VectorXd /* 4d */ rewards = Eigen::VectorXd::Zero(4);
    rewards << board->scoreIfUp(), board->scoreIfLeft(),
        board->scoreIfDown(), board->scoreIfRight();
    constexpr double GANMA = 0.99;
    Eigen::VectorXd /* 4d */ next_qs = Eigen::VectorXd::Zero(4);
    next_qs << q->predict(boardArrayToEigenVec(board->boardArrayIfUp()))[0],
        q->predict(boardArrayToEigenVec(board->boardArrayIfLeft()))[1],
        q->predict(boardArrayToEigenVec(board->boardArrayIfDown()))[2],
        q->predict(boardArrayToEigenVec(board->boardArrayIfRight()))[3];
    // std::cout << "rewards: " << rewards.transpose() << std::endl;
    // std::cout << "next_qs: " << next_qs.transpose() << std::endl;
    Eigen::VectorXd /* 4d */ targets = rewards + GANMA * next_qs;
    Eigen::VectorXd /* 4d */ loss = targets - next_qs;
    q->setLossAndBackProp(s, loss);
}


}  // namespace AI
