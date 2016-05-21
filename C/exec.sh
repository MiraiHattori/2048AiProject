#!/bin/bash
gcc ai.c -o AI -std=c99 -Wall -Wextra -Wconversion -D _BSD_SOURCE
if [ ! -e logs -a -f 2048/2048.rb ]; then
    mkdir logs
elif [ -e logs ]; then
    ruby 2048/2048.rb -exec "./AI"
else
    echo "aiプログラムやlogs等があるディレクトリで実行してください"
    echo "logsフォルダを作成することができません"
fi
