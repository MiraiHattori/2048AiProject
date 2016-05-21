#!/bin/bash
if [ ! -e logs -a -f 2048/2048.rb ]; then
    mkdir logs
fi
if [ -e logs ]; then
    ruby 2048/2048.rb -exec "ruby ai.rb"
else
    echo "aiプログラムやlogs等があるディレクトリで実行してください"
    echo "logsフォルダを作成することができません"
fi
