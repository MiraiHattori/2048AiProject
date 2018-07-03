#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <utility>

#include "config/params.hpp"
#include "util/math.hpp"
#include "util/random.hpp"

using Params::ROW_SIZE;
using Params::COL_SIZE;

namespace Board
{

template <int ROW_SIZE_, int COL_SIZE_>
class Board
{
public:
    using BoardArray = std::array<std::array<int, COL_SIZE_>, ROW_SIZE_>;
    enum class Manipulation : int {
        None = 0,
        Up,
        Left,
        Down,
        Right,
    };

    explicit Board()
    {
        this->placeTwoOrFour();
        this->eagerMoveEvaluation();
    }

    const int& turn() const { return m_turn; }
    const int& score() const { return m_score; }
    const int& scoreIfUp() const { return m_additional_score_if_up; }
    const int& scoreIfLeft() const { return m_additional_score_if_left; }
    const int& scoreIfDown() const { return m_additional_score_if_down; }
    const int& scoreIfRight() const { return m_additional_score_if_right; }

    const BoardArray& boardArrayIfUp() const { return m_board_array_if_up; }
    const BoardArray& boardArrayIfLeft() const { return m_board_array_if_left; }
    const BoardArray& boardArrayIfDown() const { return m_board_array_if_down; }
    const BoardArray& boardArrayIfRight() const { return m_board_array_if_right; }

    void up() { m_next_manip = Manipulation::Up; }
    void left() { m_next_manip = Manipulation::Left; }
    void down() { m_next_manip = Manipulation::Down; }
    void right() { m_next_manip = Manipulation::Right; }

    void clear()
    {
        m_turn = 0;
        m_score = 0;
        m_board_array = BoardArray();
        this->placeTwoOrFour();
        this->eagerMoveEvaluation();
    }

    const BoardArray& boardArray() const
    {
        return m_board_array;
    }

    // 空いているセルに2か4を入れる
    bool placeTwoOrFour()
    {
        std::vector<std::array<int, 2>> blank{};
        for (int row = 0; row < ROW_SIZE_; row++) {
            for (int col = 0; col < COL_SIZE_; col++) {
                if (m_board_array[row][col] == 0) {
                    blank.emplace_back(std::array<int, 2>({row, col}));
                }
            }
        }
        bool has_space = not blank.empty();
        // m_board_array上は1(90%)か2(10%)
        int two_or_four = 0;
        if (Util::randUniform<int>(0, 10) != 0) {
            two_or_four = 1;
        } else {
            two_or_four = 2;
        }
        if (has_space) {
            std::array<int, 2> pos = blank.at(Util::randUniform<int>(0, blank.size()));
            m_board_array[pos[0]][pos[1]] = two_or_four;
        }
        return has_space;
    }

    bool move()
    {
        using Params::PRINT_COMMAND;
        BoardArray board_array_pre = m_board_array;
        int additional_score;

        switch (m_next_manip) {
        case Manipulation::Up:
            if constexpr (PRINT_COMMAND) {
                std::cout << "up" << std::endl;
            }
            m_board_array = m_board_array_if_up;
            additional_score = m_additional_score_if_up;
            break;
        case Manipulation::Left:
            if constexpr (PRINT_COMMAND) {
                std::cout << "left" << std::endl;
            }
            m_board_array = m_board_array_if_left;
            additional_score = m_additional_score_if_left;
            break;
        case Manipulation::Down:
            if constexpr (PRINT_COMMAND) {
                std::cout << "down" << std::endl;
            }
            m_board_array = m_board_array_if_down;
            additional_score = m_additional_score_if_down;
            break;
        case Manipulation::Right:
            if constexpr (PRINT_COMMAND) {
                std::cout << "right" << std::endl;
            }
            m_board_array = m_board_array_if_right;
            additional_score = m_additional_score_if_right;
            break;
        default:
            std::cout << "Unknown Manipulation" << std::endl;
            additional_score = 0;
            return false;
            break;
        }
        m_next_manip = Manipulation::None;
        // 操作をしても盤面が変わらなかったらfalseになる
        bool is_board_changed = board_array_pre != m_board_array;
        // 移動完了
        if (is_board_changed) {
            m_turn++;
            m_score += additional_score;
        }

        return is_board_changed;
    }

    // 上下左右に移動した場合、盤面がどうなるのか先行評価する
    void eagerMoveEvaluation()
    {
        /*
             * m_board_array_if_.*を更新する
             * 上下の場合、COL_SIZE本のROW_SIZEの長さのarrayがあって、それぞれのarrayが上下方向にflushされる
             */
        this->flushUp();
        this->flushLeft();
        this->flushDown();
        this->flushRight();
    }

    // 先頭方向に数字列をflushした後の配列と、何点追加されるかを返す
    template <typename T>
    std::pair<T, unsigned int> flush(const T& arr_) const
    {
        T arr{};
        unsigned int score = 0;
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
                    score += Util::power(2u, static_cast<unsigned int>(cand + 1));
                    cand = 0;
                    arr_index++;
                }
            }
        }
        return std::make_pair<T, unsigned int>(std::move(arr), std::move(score));
    }

    void flushUp()
    {
        m_additional_score_if_up = 0;
        for (int col = 0; col < COL_SIZE_; col++) {
            std::array<int, ROW_SIZE_> a{};
            for (int row = 0; row < ROW_SIZE_; row++) {
                a[row] = m_board_array[row][col];
            }
            auto array_and_score = this->flush(a);
            a = array_and_score.first;
            m_additional_score_if_up += array_and_score.second;
            for (int row = 0; row < ROW_SIZE_; row++) {
                m_board_array_if_up[row][col] = a[row];
            }
        }
    }

    void flushLeft()
    {
        m_additional_score_if_left = 0;
        for (int row = 0; row < ROW_SIZE_; row++) {
            auto array_and_score = this->flush(m_board_array[row]);
            m_board_array_if_left[row] = array_and_score.first;
            m_additional_score_if_left += array_and_score.second;
        }
    }

    void flushDown()
    {
        for (int col = 0; col < COL_SIZE_; col++) {
            std::array<int, ROW_SIZE_> a{};
            for (int row = 0; row < ROW_SIZE_; row++) {
                a[ROW_SIZE_ - row - 1] = m_board_array[row][col];
            }
            auto array_and_score = this->flush(a);
            a = array_and_score.first;
            m_additional_score_if_down += array_and_score.second;
            for (int row = 0; row < ROW_SIZE_; row++) {
                m_board_array_if_down[row][col] = a[ROW_SIZE_ - row - 1];
            }
        }
    }

    void flushRight()
    {
        for (int row = 0; row < ROW_SIZE_; row++) {
            std::array<int, COL_SIZE_> a{};
            for (int col = 0; col < COL_SIZE_; col++) {
                a[COL_SIZE_ - col - 1] = m_board_array[row][col];
            }
            auto array_and_score = this->flush(a);
            a = array_and_score.first;
            m_additional_score_if_right += array_and_score.second;
            for (int col = 0; col < COL_SIZE_; col++) {
                m_board_array_if_right[row][col] = a[COL_SIZE_ - col - 1];
            }
        }
    }


private:
    // 動く予定の方向
    Manipulation m_next_manip = Manipulation::None;
    // 現在のターン数
    int m_turn = 0;
    // 現在のスコア
    int m_score = 0;
    // 2の指数で保持される盤面データ。0は空タイルを表す
    BoardArray m_board_array{};
    // 上下左右に移動したときの盤面データ。2 or 4のタイルを追加した直後に必ず計算すること
    BoardArray m_board_array_if_up{};
    BoardArray m_board_array_if_left{};
    BoardArray m_board_array_if_down{};
    BoardArray m_board_array_if_right{};
    // 上下左右に移動したときに増えるスコアを先行評価。2 or 4のタイルを追加した直後に必ず計算すること
    int m_additional_score_if_up = 0;
    int m_additional_score_if_left = 0;
    int m_additional_score_if_down = 0;
    int m_additional_score_if_right = 0;
};

extern std::unique_ptr<Board<Params::ROW_SIZE, Params::COL_SIZE>> board;
}  // namespace Board
