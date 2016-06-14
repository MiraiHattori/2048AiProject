#include <iostream>
#include <random>
#include <string>
#include <array>
#include <fstream>

/*constant------------------------------------------------*/
#define COLUMN 4
#define ROW 4
#define STDOUT_BOARD false
#define STDOUT_COMMAND false
#define STDOUT_TURN false
#define STDOUT_GAMESTATUS false
#define STDOUT_NOAVAILABLETILE false
#define STDOUT_SCORE false
#define STDOUT_RESULT_BOARD false
#define STDOUT_RESULT_TURN false
#define STDOUT_RESULT_MAXNUM false
#define STDOUT_RESULT_SCORE false
#define STDOUT_RESULT_STATUS false
#define LOGPATH_STDOUT_ALL "./logs/stdout_all.log"
#define LOGPATH_STDOUT_RESULT "./logs/result.log"
#define FILEIO_STDOUT false
#define FILEIO_RESULT true
#define MAX_DIGIT_TO_SHOW 4
#define DEBUG_PRINTBOARD false


// {{{ initial definition

namespace
{
/*initial function definition-----------------------------*/
void init();                                                              // initialize
void turnInit();                                                          // 毎ターン処理
void printBoard();                                                        // 盤面の状態を出力
std::array<int, ROW> flushVLine(std::array<int, ROW> array_vline);        // 縦一列を寄せる
std::array<int, COLUMN> flushHLine(std::array<int, COLUMN> array_hline);  // 横一列を寄せる
std::array<std::array<int, COLUMN>, ROW> ifMoveUp();                      // 上に動いた時の盤面
std::array<std::array<int, COLUMN>, ROW> ifMoveDown();                    // 下に動いた時の盤面
std::array<std::array<int, COLUMN>, ROW> ifMoveLeft();                    // 左に動いた時の盤面
std::array<std::array<int, COLUMN>, ROW> ifMoveRight();                   // 右に動いた時の盤面
bool canMoveUp();                                                         // 上に動かせるか
bool canMoveDown();                                                       // 下に動かせるか
bool canMoveLeft();                                                       // 左に動かせるか
bool canMoveRight();                                                      // 右に動かせるか
void moveUp();                                                            // 上に動かす
void moveDown();                                                          // 下に動かす
void moveLeft();                                                          // 左に動かす
void moveRight();                                                         // 右に動かす
void judgeGameState();                                                    // gameの状態を判定
void gameOver();                                                          // gameover処理
int randomInt(int start_num, int end_num);                                // 乱数を返す
int twoOrFour();                                                          // 追加タイルが2か4か
void putRandomTile();                                                     // タイル追加
void printGameState();                                                    // ターン数、スコアなどを表示
void turnEnd();                                                           // 毎ターン終了後の処理
/*class---------------------------------------------------*/
/*struct--------------------------------------------------*/
enum GameState {
    start = 0,  // 初期状態
    normal,     // 動かせる
    gameover,   // 動かせない
};

/*public variables----------------------------------------*/
int turn_count = 0;                                        // 総ターン数
int score = 0;                                             // 総スコア
std::array<std::array<int, COLUMN>, ROW> board_data;       // 盤面データ。2の指数で保持される。0は空タイルを表す。
std::array<std::array<int, COLUMN>, ROW> last_board_data;  // 1回前の盤面データ。2の指数で保持される。0は空タイルを表す。
GameState game_state = start;                              // ゲームの状態
std::ofstream stdout_all(LOGPATH_STDOUT_ALL);
std::ofstream result_all(LOGPATH_STDOUT_RESULT);
}  // anonymous namespace

// }}} initial definition

int main()
{
    try {
        init();

        while (true) {
            turnInit();
            // AIはここから
            static bool hoge = false;
            if (canMoveDown() == true) {
                if (hoge) {
                    moveDown();
                    hoge = false;
                } else {
                    moveRight();
                    hoge = true;
                }
            } else {
                if (canMoveRight() == true) {
                    moveRight();
                } else {
                    if (canMoveLeft() == true) {
                        moveLeft();
                    } else {
                        if (canMoveUp() == true) {
                            moveUp();
                        } else {
                            std::cout << "えらぁ！" << std::endl;
                        }
                    }
                }
            }

            turnEnd();
        }
    } catch (const char* str) {
        std::cout << "Error: " << str << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "Error: " << str << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Invalid Error." << e.what() << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "Invalid Error." << e.what() << std::endl;
        }
    }
    return 0;
}


// {{{ definition

/*function------------------------------------------------*/


namespace
{
// initialize
void init()
{
    // 0で埋める
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            board_data[i][j] = 0;
        }
    }
}
// 毎ターン処理
void turnInit()
{
    putRandomTile();
    judgeGameState();
    printGameState();
    if (STDOUT_BOARD) {
        printBoard();
    }
}
// 盤面の状態を出力
void printBoard()
{
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            int elem_num = board_data[i][j];  // i,j番目のタイルの指数
            int elem_value;                   // 実際の値、または空タイルなら0
            if (elem_num == 0) {
                // 指数が0なら0
                elem_value = 0;
            } else {
                // 指数が1以上なら2^n
                elem_value = 1;
                for (int k = 0; k < elem_num; k++) {
                    elem_value *= 2;
                }
            }
            // MAX_DIGIT_TO_SHOWの長さの中で数字を右寄せ表示
            for (int l = 0; l < MAX_DIGIT_TO_SHOW - static_cast<int>(std::to_string(elem_value).length()); l++) {
                std::cout << " ";

                if (FILEIO_STDOUT) {
                    stdout_all << " ";
                }
            }
            std::cout << elem_value;
            if (FILEIO_STDOUT) {
                stdout_all << elem_value;
            }
        }
        std::cout << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << std::endl;
        }
    }
}
// 縦一列を寄せる
std::array<int, ROW> flushVLine(std::array<int, ROW> array_vline)
{
    std::array<int, ROW> array_line = array_vline;
    // 空白(0)を逆サイドに寄せる
    int cnt = 0;
    for (int i = 0; i < ROW; i++) {
        if (array_line[cnt] == 0) {
            for (int j = cnt; j < ROW - 1; j++) {
                array_line[j] = array_line[j + 1];
            }
            array_line[ROW - 1] = 0;
        } else {
            cnt++;
        }
    }
    // 同じ数字を合体する
    int iterator = 0;
    while (iterator != ROW - 1 && array_line[iterator] != 0) {
        if (array_line[iterator] == array_line[iterator + 1]) {
            // スコア加算
            int additional_score = 1;
            for (int pow = 0; pow < array_line[iterator + 1]; pow++) {
                additional_score *= 2;
            }
            score += additional_score;
            array_line[iterator]++;
            for (int j = iterator + 1; j < ROW - 1; j++) {
                array_line[j] = array_line[j + 1];
            }
            array_line[ROW - 1] = 0;
        }
        iterator++;
    }
    return array_line;
}
// 横一列を寄せる
std::array<int, COLUMN> flushHLine(std::array<int, COLUMN> array_hline)
{
    std::array<int, COLUMN> array_line = array_hline;
    // 空白(0)を逆サイドに寄せる
    int cnt = 0;
    for (int i = 0; i < COLUMN; i++) {
        if (array_line[cnt] == 0) {
            for (int j = cnt; j < ROW - 1; j++) {
                array_line[j] = array_line[j + 1];
            }
            array_line[ROW - 1] = 0;
        } else {
            cnt++;
        }
    }
    // 同じ数字を合体する
    int iterator = 0;
    while (iterator != COLUMN - 1 && array_line[iterator] != 0) {
        if (array_line[iterator] == array_line[iterator + 1]) {
            // スコア加算
            int additional_score = 1;
            for (int pow = 0; pow < array_line[iterator + 1]; pow++) {
                additional_score *= 2;
            }
            score += additional_score;
            array_line[iterator]++;
            for (int j = iterator + 1; j < COLUMN - 1; j++) {
                array_line[j] = array_line[j + 1];
            }
            array_line[COLUMN - 1] = 0;
        }
        iterator++;
    }
    return array_line;
}
// 上に動いた時の盤面
std::array<std::array<int, COLUMN>, ROW> ifMoveUp()
{
    std::array<std::array<int, COLUMN>, ROW> board_if_up;
    board_if_up = board_data;
    for (int j = 0; j < COLUMN; j++) {
        std::array<int, COLUMN> iarray;
        for (int array_it = 0; array_it < ROW; array_it++) {
            iarray[array_it] = board_if_up[array_it][j];
        }
        iarray = flushVLine(iarray);
        for (int array_it = 0; array_it < ROW; array_it++) {
            board_if_up[array_it][j] = iarray[array_it];
        }
    }
    return board_if_up;
}
// 下に動いた時の盤面
std::array<std::array<int, COLUMN>, ROW> ifMoveDown()
{
    std::array<std::array<int, COLUMN>, ROW> board_if_down;
    board_if_down = board_data;
    for (int j = 0; j < COLUMN; j++) {
        std::array<int, COLUMN> iarray;
        for (int array_it = 0; array_it < ROW; array_it++) {
            iarray[array_it] = board_if_down[ROW - 1 - array_it][j];
        }
        iarray = flushVLine(iarray);
        for (int array_it = 0; array_it < ROW; array_it++) {
            board_if_down[ROW - 1 - array_it][j] = iarray[array_it];
        }
    }
    return board_if_down;
}
// 左に動いた時の盤面
std::array<std::array<int, COLUMN>, ROW> ifMoveLeft()
{
    std::array<std::array<int, COLUMN>, ROW> board_if_left;
    board_if_left = board_data;
    for (int i = 0; i < ROW; i++) {
        std::array<int, ROW> jarray;
        for (int array_it = 0; array_it < COLUMN; array_it++) {
            jarray[array_it] = board_if_left[i][array_it];
        }
        jarray = flushHLine(jarray);
        for (int array_it = 0; array_it < COLUMN; array_it++) {
            board_if_left[i][array_it] = jarray[array_it];
        }
    }
    return board_if_left;
}
// 右に動いた時の盤面
std::array<std::array<int, COLUMN>, ROW> ifMoveRight()
{
    std::array<std::array<int, COLUMN>, ROW> board_if_right;
    board_if_right = board_data;
    for (int i = 0; i < ROW; i++) {
        std::array<int, ROW> jarray;
        for (int array_it = 0; array_it < COLUMN; array_it++) {
            jarray[array_it] = board_if_right[i][COLUMN - 1 - array_it];
        }
        jarray = flushHLine(jarray);
        for (int array_it = 0; array_it < COLUMN; array_it++) {
            board_if_right[i][COLUMN - 1 - array_it] = jarray[array_it];
        }
    }
    return board_if_right;
}
// 上に動かせるか
bool canMoveUp()
{
    if (ifMoveUp() == board_data) {
        return false;
    } else {
        return true;
    }
}
// 下に動かせるか
bool canMoveDown()
{
    if (ifMoveDown() == board_data) {
        return false;
    } else {
        return true;
    }
}
// 左に動かせるか
bool canMoveLeft()
{
    if (ifMoveLeft() == board_data) {
        return false;
    } else {
        return true;
    }
}
// 右に動かせるか
bool canMoveRight()
{
    if (ifMoveRight() == board_data) {
        return false;
    } else {
        return true;
    }
}
// 上に動かす
void moveUp()
{
    if (DEBUG_PRINTBOARD) {
        std::cout << "printboard debug before" << std::endl;

        if (FILEIO_STDOUT) {
            stdout_all << "printboard debug before" << std::endl;
        }
        printBoard();
    }
    if (STDOUT_COMMAND) {
        std::cout << "up" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "up" << std::endl;
        }
    }
    board_data = ifMoveUp();
    if (DEBUG_PRINTBOARD) {
        std::cout << "printboard debug after" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "printboard debug after" << std::endl;
        }
        printBoard();
    }
}
// 下に動かす
void moveDown()
{
    if (DEBUG_PRINTBOARD) {
        std::cout << "printboard debug before" << std::endl;

        if (FILEIO_STDOUT) {
            stdout_all << "printboard debug before" << std::endl;
        }
        printBoard();
    }
    if (STDOUT_COMMAND) {
        std::cout << "down" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "down" << std::endl;
        }
    }
    board_data = ifMoveDown();
    if (DEBUG_PRINTBOARD) {
        std::cout << "printboard debug after" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "printboard debug after" << std::endl;
        }
        printBoard();
    }
}
// 左に動かす
void moveLeft()
{
    if (DEBUG_PRINTBOARD) {
        std::cout << "printboard debug before" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "printboard debug before" << std::endl;
        }
        printBoard();
    }
    if (STDOUT_COMMAND) {
        std::cout << "left" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "left" << std::endl;
        }
    }
    board_data = ifMoveLeft();
    if (DEBUG_PRINTBOARD) {
        std::cout << "printboard debug after" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "printboard debug after" << std::endl;
        }
        printBoard();
    }
}
// 右に動かす
void moveRight()
{
    if (DEBUG_PRINTBOARD) {
        std::cout << "printboard debug before" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "printboard debug before" << std::endl;
        }
        printBoard();
    }
    if (STDOUT_COMMAND) {
        std::cout << "right" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "right" << std::endl;
        }
    }
    board_data = ifMoveRight();
    if (DEBUG_PRINTBOARD) {
        std::cout << "printboard debug after" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "printboard debug after" << std::endl;
        }
        printBoard();
    }
}
// gameの状態を判定
void judgeGameState()
{
    if (game_state == start) {
        throw "game_state shoudn't be 'start' in function judgeGameState";
    } else if (!canMoveUp() && !canMoveDown() && !canMoveLeft() && !canMoveRight()) {
        game_state = gameover;
    }
}
// gameover処理
void gameOver()
{
    if (STDOUT_RESULT_STATUS) {
        std::cout << "Game Over" << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << "Game Over" << std::endl;
        }
    }
    if (STDOUT_RESULT_TURN) {
        std::cout << "Turn: " << turn_count << " ";

        if (FILEIO_STDOUT) {
            stdout_all << "Turn: " << turn_count << " ";
        }
    }
    if (FILEIO_RESULT) {
        result_all << "Turn: " << turn_count << std::endl;
    }
    if (STDOUT_RESULT_SCORE) {
        std::cout << "Score: " << score << " ";
        if (FILEIO_STDOUT) {
            stdout_all << "Score: " << score << " ";
        }
    }
    if (FILEIO_RESULT) {
        result_all << "Score: " << score << std::endl;
    }
    // 最大タイル
    int max_num_temp = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (board_data[i][j] > max_num_temp) {
                max_num_temp = board_data[i][j];
            }
        }
    }
    int max_num = 1;
    for (int i = 0; i < max_num_temp; i++) {
        max_num *= 2;
    }
    if (STDOUT_RESULT_MAXNUM) {
        std::cout << "Max_Num: " << max_num << " ";
        if (FILEIO_STDOUT) {
            stdout_all << "Max_Num: " << max_num << " ";
        }
    }
    if (FILEIO_RESULT) {
        result_all << "Max_Num: " << max_num << std::endl;
    }
    if (STDOUT_RESULT_TURN || STDOUT_RESULT_SCORE || STDOUT_RESULT_MAXNUM) {
        std::cout << std::endl;
        if (FILEIO_STDOUT) {
            stdout_all << std::endl;
        }
    }
    if (STDOUT_RESULT_BOARD) {
        std::cout << "Result board data" << std::endl;

        if (FILEIO_STDOUT) {
            stdout_all << "Result board data" << std::endl;
        }
        printBoard();
    }
    exit(0);
}
// 乱数を返す
int randomInt(int start_num, int end_num)
{
    std::random_device random_num;
    std::mt19937 mt(random_num());
    std::uniform_int_distribution<> rand(start_num, end_num);
    return rand(mt);
}
// 追加タイルが2か4か
int twoOrFour()
{
    if (randomInt(0, 9) == 0) {
        return 4;
    } else {
        return 2;
    }
}
// タイル追加
void putRandomTile()
{
    if (game_state == start) {
        int first_tile = randomInt(0, ROW * COLUMN - 1);
        board_data[first_tile / COLUMN][first_tile % COLUMN] = twoOrFour() / 2;
        int second_tile = randomInt(0, ROW * COLUMN - 2);
        if (first_tile > second_tile) {
            board_data[second_tile / COLUMN][second_tile % COLUMN] = twoOrFour() / 2;
        } else if (first_tile <= second_tile) {
            board_data[(second_tile + 1) / COLUMN][(second_tile + 1) % COLUMN] = twoOrFour() / 2;
        }
        game_state = normal;
    } else if (game_state == normal) {
        // 空タイルの数を調べる
        int available_tile_num = 0;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COLUMN; j++) {
                if (board_data[i][j] == 0) {
                    available_tile_num++;
                }
            }
        }
        if (available_tile_num == 0) {
            if (STDOUT_NOAVAILABLETILE) {
                std::cout << "no available tile" << std::endl;

                if (FILEIO_STDOUT) {
                    stdout_all << "no available tile" << std::endl;
                }
            }
            return;
        }
        // 空タイルの座標保持
        std::array<std::array<int, 2>, ROW * COLUMN> available_tile_info;
        int iterator = 0;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COLUMN; j++) {
                if (board_data[i][j] == 0) {
                    available_tile_info[iterator][0] = i;
                    available_tile_info[iterator][1] = j;
                    iterator++;
                }
            }
        }
        // 一致するはず
        if (iterator != available_tile_num) {
            throw "available_tile_num error";
        }
        int random_tile = randomInt(0, available_tile_num - 1);
        board_data[available_tile_info[random_tile][0]][available_tile_info[random_tile][0]] = twoOrFour() / 2;
    } else {
        throw "game_state should be normal";
    }
}
// ターン数、スコアなどを表示
void printGameState()
{
    if (game_state == start) {
        throw "game_state shoundn't be 'start'";
    } else if (game_state == normal) {
        if (STDOUT_GAMESTATUS) {
            std::cout << "game status is normal." << std::endl;

            if (FILEIO_STDOUT) {
                stdout_all << "game status is normal." << std::endl;
            }
        }
        if (STDOUT_TURN) {
            std::cout << "Turn: " << turn_count << std::endl;
            if (FILEIO_STDOUT) {
                stdout_all << "Turn: " << turn_count << std::endl;
            }
        }
        if (STDOUT_SCORE) {
            std::cout << "Score: " << score << std::endl;
            if (FILEIO_STDOUT) {
                stdout_all << "Score: " << score << std::endl;
            }
        }
    } else if (game_state == gameover) {
        gameOver();
    }
}
// 毎ターン終了後の処理
void turnEnd()
{
    if (board_data != last_board_data) {
        turn_count++;
    }
    last_board_data = board_data;  // 一回前の盤面を保存
}

}  // anonymous namespace
// }}} definition
