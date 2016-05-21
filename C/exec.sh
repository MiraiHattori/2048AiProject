#!/bin/bash
gcc ai.c -o AI -std=c99 -Wall -Wextra -Wconversion -D _BSD_SOURCE
ruby 2048/2048.rb -exec "./AI"
