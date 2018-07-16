# 2048 AI Project

- [2048](https://ja.wikipedia.org/wiki/2048_(%E3%82%B2%E3%83%BC%E3%83%A0))というゲームをC++で解いてみるためのリポジトリ  
  - This is a repository to play [2048](https://en.wikipedia.org/wiki/2048_%28video_game%29) with c++.  

# Requirements

- C++17をコンパイルできる環境
  - Compiler which supports C++17 or upper
- Python (>=3)
- 作者はPython3.6で動作を確認しています。
  - The author uses Python 3.6.

# Usage
- 動かし方
  - How to execute program.
``` shell
$ git clone https://github.com/future731/2048AiProject.git
$ cd 2048AiProject
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./main
```

- 2048のAIは `ai/ai.cpp` にある `void init(), void initGame(), void chooseMove()` という関数内を変更して書いてください。
  - You can improve the AI solver in function `void init(), void initGame(), void chooseMove()` in file `ai/ai.cpp`
  
- `board->boardArray()`で盤面の状態を取得できます。`const std::array<std::array<int, ROW_SIZE>, COL_SIZE>&`型で取得することができます。
  - You can get board data by `board->boardArray()`. The return type is `const std::array<std::array<int, ROW_SIZE>, COL_SIZE>&`.
  
- 盤面データのそれぞれのマスは2の指数で与えられています。0は0を表します。
  - The cells in the board data are the exponent of 2. 0 means the cell is blank.
  
- 盤面を上下左右に動かしたときの盤面はすでに計算されていて、`board->boardArrayIfUp()`, `board->boardArrayIfLeft()`, `board->boardArrayIfDown()`, `board->boardArrayIfRight()`などで取得できます。
  - The four move cases are eagerly evaluated. You can get the future board data by functions `board->boardArrayIfUp()`, `board->boardArrayIfLeft()`, `board->boardArrayIfDown()`, `board->boardArrayIfRight()`.

- 盤面を上下左右に動かしたときに追加で得られる点数もすでに計算されていて、`board->scoreIfUp()`, `board->scoreIfLeft()`, `board->scoreIfDown()`, `board->scoreIfRight()`などで取得できます。
  - The scores are also eagerly evaluated. You can get the future board data by functions `board->scoreIfUp()`, `board->scoreIfLeft()`, `board->scoreIfDown()`, `board->scoreIfRight()`.
  
- `board->up()`, `board->left()`, `board->down()`, `board->right()`の4種類でタイルをどちら側に動かすか操作してください。`void ai()`内で最後に呼ばれたものが動きとして採用されます。
  - Call `board->up()`, `board->left()`, `board->down()` or `board->right()` function in `void ai()`. The next manipulation is decided by the latest called function among them.
  
# Updates
- Version 1.0 (2018/07)
  - 2048 basic game function is implemented.
