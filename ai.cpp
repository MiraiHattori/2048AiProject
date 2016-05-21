#include <iostream>
#include <string>
#include <array>
#include <vector>

/*constant-------------------------------------------------------*/
#define COLUMN 4
#define ROW 4
#define DEBUG true
#define COMMAND_HISTORY_NUM 5

//{{{関数宣言、型宣言、変数宣言(vimでの折りたたみ記号はzaで開ける)
/*initial funtion definition-------------------------------------*/
void getTileInfo();
int getGameStatus();
std::array<int, ROW * COLUMN + 1> splitLine(const std::string& str, const char* delim);
void initTurn();

void goUp();
void goDown();
void goLeft();
void goRight();
std::array<int, ROW> flushZero(std::array<int, ROW> single_array);
void boardIf();
void boardIfMoveUp();
void boardIfMoveDown();
void boardIfMoveLeft();
void boardIfMoveRight();
bool canMoveUp();
bool canMoveDown();
bool canMoveLeft();
bool canMoveRight();
void updateCommandBefore(std::string command);

/*public class---------------------------------------------------*/

/*public struct--------------------------------------------------*/
struct command_data {
    // constructer
    command_data()
    {
        command = "";
        status = 0;
    }
    std::string command;
    int status;
};

namespace
{
/*public variables-----------------------------------------------*/
// 過去N回のコマンドを記憶{1個前, 2個前, ..., N個前}
std::array<command_data, COMMAND_HISTORY_NUM> command_before;
// 受信データ
std::string received_data = "";
// 盤面データ
std::array<std::array<int, ROW>, COLUMN> board_data;
// 上下左右に動かした時の盤面データ
std::array<std::array<int, ROW>, COLUMN> board_if_up;
std::array<std::array<int, ROW>, COLUMN> board_if_down;
std::array<std::array<int, ROW>, COLUMN> board_if_left;
std::array<std::array<int, ROW>, COLUMN> board_if_right;
}  // anonymous namespace

//}}} vimでの折りたたみ記号終了

/*main program---------------------------------------------------*/

int main()
{
    try {
        while (true) {
            // Turnごとの初期化
            initTurn();

            /*AI program-----------------------------------------------------*/
            static bool hoge = false;
            if (canMoveDown() == true) {
                if (hoge) {
                    goDown();
                    hoge = false;
                } else {
                    goRight();
                    hoge = true;
                }
            } else {
                if (canMoveRight() == true) {
                    goRight();
                } else {
                    if (canMoveLeft() == true) {
                        goLeft();
                    } else {
                        if (canMoveUp() == true) {
                            goUp();
                        } else {
                            std::cout << "えらぁ！" << std::endl;
                        }
                    }
                }
            }
        }
    } catch (std::exception& e) {
        std::cout << "Invalid Error in AI file." << e.what() << std::endl;
    }
    return 0;
}

//{{{関数の実体(vimだとzaで展開)

void getTileInfo()
{
    std::array<int, ROW* COLUMN + 1> elems = splitLine(received_data, " ");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            board_data[i][j] = elems[i * COLUMN + j];
        }
    }
}

int getGameStatus()
{
    // return 0(continue) or 1(stuck) or 2(gameover)
    return splitLine(received_data, " ")[ROW * COLUMN];
}

void initTurn()
{
    // cinでは空白で区切られる
    received_data = "";
    std::string received_data_elem;
    for (int i = 0; i < ROW * COLUMN; i++) {
        std::cin >> received_data_elem;
        received_data += received_data_elem + " ";
    }
    // 最後の一要素をゲット
    std::cin >> received_data_elem;
    received_data += received_data_elem;

    // 改行だけのときエラーを吐いて次のループへ
    if (received_data == "") {
        std::cout << "an error has occured in 2048/2048.rb" << std::endl;
    }
    // status 2を取得した時ゲームオーバー
    if (getGameStatus() == 2) {
        std::cout << "Game Over" << std::endl;
        exit(1);
    }
    // board_dataに格納
    getTileInfo();
    for (int i = 0; i < COLUMN; i++) {
        for (int j = 0; j < ROW; j++) {
            std::cout << board_data[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "GameStatus: " << getGameStatus() << std::endl;
    std::cout << "Up: " << canMoveUp() << " Down: " << canMoveDown() << " Left: " << canMoveLeft() << " Right: " << canMoveRight() << std::endl;
    // boardが上下左右に動いた時のデータをboard_if_(direction)に格納
    boardIf();
}

std::array<int, ROW * COLUMN + 1> splitLine(const std::string& str, const char* delim)
{
    std::array<int, ROW * COLUMN + 1> elems;
    std::string item;
    int item_num = 0;  // count number when splitting
    for (char ch : str) {
        if (ch == *delim) {
            if (!item.empty()) {
                elems[item_num] = std::stoi(item);
                item_num++;
            }
            item.clear();
        } else {
            item += ch;
        }
    }
    if (!item.empty()) {
        elems[item_num] = std::stoi(item);
    }
    return elems;
}

void goUp()
{
    std::cout << "up" << std::endl;
    updateCommandBefore("up");
}
void goDown()
{
    std::cout << "down" << std::endl;
    updateCommandBefore("down");
}
void goLeft()
{
    std::cout << "left" << std::endl;
    updateCommandBefore("left");
}
void goRight()
{
    std::cout << "right" << std::endl;
    updateCommandBefore("right");
}
std::array<int, ROW> flushZero(std::array<int, ROW> single_array)
{
    std::array<int, ROW> array_temp = single_array;
    int cnt = 0;
    for (int i = 0; i < ROW; i++) {
        if (array_temp[cnt] == 0) {
            for (int j = cnt; j < ROW - 1; j++) {
                array_temp[j] = array_temp[j + 1];
            }
            array_temp[ROW - 1] = 0;
        } else {
            cnt++;
        }
    }
    int integration_it = 0;
    while (integration_it != ROW - 1 && array_temp[integration_it] != 0) {
        if (array_temp[integration_it] == array_temp[integration_it + 1]) {
            array_temp[integration_it] += 1;
            for (int i = integration_it + 1; i < ROW - 1; i++) {
                array_temp[i] = array_temp[i + 1];
            }
            array_temp[ROW - 1] = 0;
        }
        integration_it++;
    }
    return array_temp;
}

void boardIf()
{
    boardIfMoveUp();
    boardIfMoveDown();
    boardIfMoveLeft();
    boardIfMoveRight();
}
void boardIfMoveUp()
{
    // コピー
    board_if_up = board_data;
    for (int j = 0; j < COLUMN; j++) {
        std::array<int, COLUMN> iarray;
        for (int array_it = 0; array_it < ROW; array_it++) {
            iarray[array_it] = board_if_up[array_it][j];
        }
        iarray = flushZero(iarray);
        for (int array_it = 0; array_it < ROW; array_it++) {
            board_if_up[array_it][j] = iarray[array_it];
        }
    }
}
void boardIfMoveDown()
{
    // コピー
    board_if_down = board_data;
    for (int j = 0; j < COLUMN; j++) {
        std::array<int, COLUMN> iarray;
        for (int array_it = 0; array_it < ROW; array_it++) {
            iarray[array_it] = board_if_down[ROW - 1 - array_it][j];
        }
        iarray = flushZero(iarray);
        for (int array_it = 0; array_it < ROW; array_it++) {
            board_if_down[ROW - 1 - array_it][j] = iarray[array_it];
        }
    }
}
void boardIfMoveLeft()
{
    // コピー
    board_if_left = board_data;
    for (int i = 0; i < ROW; i++) {
        std::array<int, COLUMN> jarray;
        for (int array_it = 0; array_it < COLUMN; array_it++) {
            jarray[array_it] = board_if_left[i][array_it];
        }
        jarray = flushZero(jarray);
        for (int array_it = 0; array_it < COLUMN; array_it++) {
            board_if_left[i][array_it] = jarray[array_it];
        }
    }
}
void boardIfMoveRight()
{
    // コピー
    board_if_right = board_data;
    for (int i = 0; i < ROW; i++) {
        std::array<int, COLUMN> jarray;
        for (int array_it = 0; array_it < COLUMN; array_it++) {
            jarray[array_it] = board_if_right[i][COLUMN - 1 - array_it];
        }
        jarray = flushZero(jarray);
        for (int array_it = 0; array_it < COLUMN; array_it++) {
            board_if_right[i][COLUMN - 1 - array_it] = jarray[array_it];
        }
    }
}
bool canMoveUp()
{
    bool is_arrays_equal = true;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (board_data[i][j] != board_if_up[i][j]) {
                is_arrays_equal = false;
            }
        }
    }
    return !is_arrays_equal;
}
bool canMoveDown()
{
    bool is_arrays_equal = true;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (board_data[i][j] != board_if_down[i][j]) {
                is_arrays_equal = false;
            }
        }
    }
    return !is_arrays_equal;
}
bool canMoveLeft()
{
    bool is_arrays_equal = true;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (board_data[i][j] != board_if_left[i][j]) {
                is_arrays_equal = false;
            }
        }
    }
    return !is_arrays_equal;
}
bool canMoveRight()
{
    bool is_arrays_equal = true;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (board_data[i][j] != board_if_right[i][j]) {
                is_arrays_equal = false;
            }
        }
    }
    return !is_arrays_equal;
}

void updateCommandBefore(std::string command)
{
    for (int i = 0; i < COMMAND_HISTORY_NUM - 1; i++) {
        command_before[COMMAND_HISTORY_NUM - i - 1] = command_before[COMMAND_HISTORY_NUM - i - 2];
    }
    command_before[0].command = command;
    command_before[0].status = getGameStatus();
}
//}}}折りたたみ記号
