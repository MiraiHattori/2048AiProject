#!/usr/bin/ruby
STDOUT.sync = true
require 'pp'
# Constant--------------------------------------------------------
COLUMN = 4
ROW = 4
DEBUG = true

# Public Variables------------------------------------------------
@debug_i = 0;

# Preparing the data table----------------------------------------
@board_data = Array.new(ROW){Array.new(COLUMN, 0)}

# Funtion definition----------------------------------------------

def getTileInfo(received_data)
    # Splitting the line by Space.
    # Ignoring multi-space.
    numbers = received_data.split(/\s+/)
    for i in 0..(ROW - 1)
        for j in 0..(COLUMN - 1)
            @board_data[i][j] = numbers[i * COLUMN + j]
        end
    end
end

def getGameStatus(received_data)
    numbers = received_data.split(/\s+/)
    numbers[ROW * COLUMN]
end

def goUp()
    puts "up"
end
def goDown()
    puts "down"
end
def goLeft()
    puts "left"
end
def goRight()
    puts "right"
end


#public variables-------------------------------------------------


#main program-----------------------------------------------------

begin
    while true
        received_data = gets.chomp

        if received_data == ""
            puts "an error has occured in 2048/2048.rb"
            next
        end
        # gameover judge
        if getGameStatus(received_data) == 2
            puts "Game Over"
            exit 1
            next
        end

        #AI program-------------------------------------------------------
        getTileInfo(received_data)
        for i in 0..(COLUMN - 1)
            for j in 0..(ROW - 1)
                print @board_data[i][j] + " "
            end
            puts ""
        end
        puts "GameStatus: #{getGameStatus(received_data)}"

        case @debug_i
        when 0 then
            goUp()
        when 1 then
            goRight()
        when 2 then
            goDown()
        when 3 then
            goLeft()
        else
            puts "Invalid Command"
            puts __LINE__
        end
        if @debug_i == 3
            @debug_i = 0
        else
            @debug_i += 1
        end
    end
rescue => general_exception
    puts "General Error: #{general_exception.message}"
    pp general_exception
end
