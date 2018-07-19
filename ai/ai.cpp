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
using QNet = FullConnectedNetworkBatch<DummyBatch<4, 4>, AffineBatch<16, 40>, ReluBatch<40, 40>, AffineBatch<40, 4>>;
std::unique_ptr<QNet> q = nullptr;

void init()
{
    // initialization
    gd = std::make_shared<GD>(0.001);
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

struct Experience {
    explicit Experience() = default;
    explicit Experience(const Eigen::VectorXd& s_now,
        const Eigen::VectorXd& rs,
        const std::array<Eigen::VectorXd, 4>& s_nexts)
        : s_now(s_now), rs(rs), s_nexts(s_nexts) {}
    Eigen::VectorXd s_now;
    Eigen::VectorXd rs;
    std::array<Eigen::VectorXd, 4> s_nexts;
};

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
                          << std::setw(Params::MAX_DIGIT_TO_SHOW)
                          << (cell == 0 ? 0 : Util::power(2u, static_cast<unsigned int>(cell)));
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
    Eigen::VectorXd /* 4d */ acts = q->predict(s).col(0);
    Manipulation manip = Manipulation::None;
    std::array<std::array<std::array<int, COL_SIZE>, ROW_SIZE>, 4> board_array_ifs;
    board_array_ifs[0] = board->boardArrayIfUp();
    board_array_ifs[1] = board->boardArrayIfLeft();
    board_array_ifs[2] = board->boardArrayIfDown();
    board_array_ifs[3] = board->boardArrayIfRight();
    std::array<Eigen::VectorXd, 4> board_array_ifs_eigen;
    for (int i = 0; i < 4 /*acts.size()*/; i++) {
        board_array_ifs_eigen[i] = boardArrayToEigenVec(board_array_ifs[i]);
    }

    double max = -std::numeric_limits<double>::max();
    for (int i = 0; i < 4 /*acts.size()*/; i++) {
        if (max < acts[i] and board->boardArray() != board_array_ifs[i]) {
            max = acts[i];
            manip = static_cast<Manipulation>(i + 1);
        }
    }
    /*
    // epsilon-greedy でときどきランダムな行動
    if (Util::randUniform(0.0, 100.0) < 10.0) {
        manip = static_cast<Manipulation>(Util::randUniform(1, 5));
    }
    */
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
        break;
    }

    Eigen::VectorXd /* 4d */ rewards(4);
    std::array<bool, 4> can_go(
        {board_array_ifs[0] != board->boardArray(),
            board_array_ifs[1] != board->boardArray(),
            board_array_ifs[2] != board->boardArray(),
            board_array_ifs[3] != board->boardArray()});
    rewards << (can_go[0] ? 1.0 : 0.0),
        (can_go[1] ? 1.0 : 0.0),
        (can_go[2] ? 1.0 : 0.0),
        (can_go[3] ? 1.0 : 0.0);

    // 学習段
    // TODO 埋め込みされている
    // 何試行ごとに学習するか
    constexpr int BATCH_SIZE = 10000;
    constexpr int SAMPLE_SIZE = 1000;

    static int cnt = -1;
    cnt++;
    static std::array<Experience, BATCH_SIZE> data;
    // Experience Replayのためデータをためておく
    data[cnt % BATCH_SIZE] = Experience(s, rewards, board_array_ifs_eigen);

    // 学習のデータ
    if ((cnt + BATCH_SIZE) % BATCH_SIZE == BATCH_SIZE - 1) {

        // std::sampleで無作為抽出
        std::vector<Experience> e_selected;
        static std::random_device seed_gen;
        static std::mt19937 engine(seed_gen());
        std::sample(data.begin(), data.end(), std::back_inserter(e_selected),
            SAMPLE_SIZE, engine);
        // 長いほうがいいので
        /* constexpr double GAMMA = 1.00; */

        Eigen::MatrixXd s_batch(16, SAMPLE_SIZE);
        Eigen::MatrixXd /* 4d */ rewards_batch(4, SAMPLE_SIZE);
        Eigen::MatrixXd /* 4d */ next_qs_batch(4, SAMPLE_SIZE);
        for (int i = 0; i < SAMPLE_SIZE; i++) {
            s_batch.col(i) = e_selected.at(i).s_now;
            rewards_batch.col(i) = e_selected[i].rs;
            std::array<Eigen::VectorXd, 4> s_nexts = e_selected[i].s_nexts;
            Eigen::VectorXd a(4);
            a << q->predict(s_nexts[0]).maxCoeff(),
                q->predict(s_nexts[1]).maxCoeff(),
                q->predict(s_nexts[2]).maxCoeff(),
                q->predict(s_nexts[3]).maxCoeff();
            next_qs_batch.col(i) = a;
        }

        Eigen::MatrixXd /* 4d */ targets_batch = rewards_batch + (/*GAMMA * */ next_qs_batch);

        Eigen::VectorXd next_qs_average(4);
        for (int i = 0; i < 4; i++) {
            next_qs_average[i] = next_qs_batch.row(i).mean();
        }
        Eigen::VectorXd rewards_average(4);
        for (int i = 0; i < 4; i++) {
            rewards_average[i] = rewards_batch.row(i).mean();
        }

        Eigen::MatrixXd /* 4d */ loss_batch(4, SAMPLE_SIZE);
        for (int i = 0; i < SAMPLE_SIZE; i++) {
            for (int j = 0; j < 4; j++) {
                // 2乗誤差
                loss_batch(j, i) = (targets_batch.col(i) - next_qs_batch.col(i))[j]
                                   * (targets_batch.col(i) - next_qs_batch.col(i))[j] / 2.0;
            }
        }
        Eigen::VectorXd loss_average(4);
        for (int i = 0; i < 4; i++) {
            loss_average[i] = loss_batch.row(i).mean();
        }

        Eigen::MatrixXd dout_batch(4, SAMPLE_SIZE);
        dout_batch = targets_batch - next_qs_batch;

        /*
        std::cout << "loss: " << std::endl;
        std::cout << loss_average << std::endl;
        std::cout << "rewards: " << std::endl;
        std::cout << rewards_average << std::endl;
        std::cout << "next_qs: " << std::endl;
        std::cout << next_qs_average << std::endl;
        */
        q->setDoutAndBackProp(s_batch, dout_batch);
    }
}

void printWeight()
{
    q->printWeight();
}


}  // namespace AI
