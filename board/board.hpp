#pragma once

#include <array>
#include <iostream>
#include <memory>

#include "config/params.hpp"
#include "util/random.hpp"

namespace Board
{

template <int ROW_SIZE_, int COL_SIZE_>
class Board
{
public:
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

    void clear()
    {
        m_board_array = std::array<std::array<int, COL_SIZE_>, ROW_SIZE_>();
        this->placeTwoOrFour();
        this->eagerMoveEvaluation();
    }

    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> boardArray() const
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
        if (randUniform<int>(0, 10) != 0) {
            two_or_four = 1;
        } else {
            two_or_four = 2;
        }
        if (has_space) {
            std::array<int, 2> pos = blank.at(randUniform<int>(0, blank.size()));
            m_board_array[pos[0]][pos[1]] = two_or_four;
        }
        return has_space;
    }

    bool move()
    {
        using Params::DEBUG_PRINTCOMMAND;
        std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> board_array_pre = m_board_array;
        switch (m_next_manip) {
        case Manipulation::Up:
            if (DEBUG_PRINTCOMMAND) {
                std::cout << "up" << std::endl;
            }
            m_board_array = m_board_array_if_up;
            break;
        case Manipulation::Left:
            if (DEBUG_PRINTCOMMAND) {
                std::cout << "left" << std::endl;
            }
            m_board_array = m_board_array_if_left;
            break;
        case Manipulation::Down:
            if (DEBUG_PRINTCOMMAND) {
                std::cout << "down" << std::endl;
            }
            m_board_array = m_board_array_if_down;
            break;
        case Manipulation::Right:
            if (DEBUG_PRINTCOMMAND) {
                std::cout << "right" << std::endl;
            }
            m_board_array = m_board_array_if_right;
            break;
        default:
            std::cout << "Unknown Manipulation" << std::endl;
            return false;
            break;
        }
        m_next_manip = Manipulation::None;
        // 操作をしても盤面が変わらなかったらfalseを返す
        return board_array_pre != m_board_array;
    }

    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> boardArrayIfUp() const { return m_board_array_if_up; }

    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> boardArrayIfLeft() const { return m_board_array_if_left; }

    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> boardArrayIfDown() const { return m_board_array_if_down; }

    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> boardArrayIfRight() const { return m_board_array_if_right; }

    void up() { m_next_manip = Manipulation::Up; }

    void left() { m_next_manip = Manipulation::Left; }

    void down() { m_next_manip = Manipulation::Down; }

    void right() { m_next_manip = Manipulation::Right; }

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

    template <typename T>
    T flush(const T& arr_)
    {
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
        return arr;
    }

    void flushUp()
    {
        for (int col = 0; col < COL_SIZE_; col++) {
            std::array<int, ROW_SIZE_> a{};
            for (int row = 0; row < ROW_SIZE_; row++) {
                a[row] = m_board_array[row][col];
            }
            a = this->flush(a);
            for (int row = 0; row < ROW_SIZE_; row++) {
                m_board_array_if_up[row][col] = a[row];
            }
        }
    }

    void flushLeft()
    {
        for (int row = 0; row < ROW_SIZE_; row++) {
            m_board_array_if_left[row] = this->flush(m_board_array[row]);
        }
    }

    void flushDown()
    {
        for (int col = 0; col < COL_SIZE_; col++) {
            std::array<int, ROW_SIZE_> a{};
            for (int row = 0; row < ROW_SIZE_; row++) {
                a[ROW_SIZE_ - row - 1] = m_board_array[row][col];
            }
            a = this->flush(a);
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
            a = this->flush(a);
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
    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> m_board_array{};
    // 上下左右に移動したときの盤面データ。2 or 4のタイルを追加した直後に必ず計算すること
    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> m_board_array_if_up{};
    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> m_board_array_if_left{};
    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> m_board_array_if_down{};
    std::array<std::array<int, COL_SIZE_>, ROW_SIZE_> m_board_array_if_right{};
};

extern std::unique_ptr<Board<Params::ROW_SIZE, Params::COL_SIZE>> board;
}  // namespace Board
