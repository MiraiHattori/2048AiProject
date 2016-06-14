#!/bin/bash
g++ 2048.cpp -o AI -std=c++11 -Wall -Wextra -Wconversion
if [ ! -e logs -a -f 2048.cpp ]; then
    mkdir logs
fi

if [ -e logs ]; then
    ./AI
else
    echo "aiプログラムやlogs等があるディレクトリで実行してください"
    echo "logsフォルダを作成することができません"
fi
