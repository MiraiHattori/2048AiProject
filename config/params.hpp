#pragma once

namespace Params
{

extern const int GAME_ITERATION;
extern const int MAX_DIGIT_TO_SHOW;
constexpr int ROW_SIZE = 4;
constexpr int COL_SIZE = 4;

// 毎周期表示するデバッグ用出力フラグ
constexpr bool PRINT_BOARD = false;
constexpr bool PRINT_COMMAND = false;
constexpr bool PRINT_TURN = false;
constexpr bool PRINT_SCORE = false;

// 毎ゲーム表示するデバッグ用出力フラグ
constexpr bool PRINT_GAMEOVER = false;
constexpr bool PRINT_RESULT = true;

}  // namespace Params
