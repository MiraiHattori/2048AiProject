#!/bin/bash
g++ ai.cpp -o AI -std=c++11 -Wall -Wextra -Wconversion
ruby 2048/2048.rb -exec "./AI"
