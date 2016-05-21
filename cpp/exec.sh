#!/bin/bash
g++ ai.cpp -o AI -std=c++11 -Wall -Wextra -Wconversion
if [ ! -e logs -a -f 2048/2048.rb ]; then
    mkdir logs
elif [ -e logs ]; then
    ruby 2048/2048.rb -exec "./AI"
else
    echo "aiプログラムやlogs等があるディレクトリで実行してください"
    echo "logsフォルダを作成することができません"
fi
