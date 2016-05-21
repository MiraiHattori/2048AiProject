require "open3"

# Constant numbers------------------------------------------------
COLUMN = 4
ROW = 4
MAX_DIGIT_TO_SHOW = 5
DEBUG = false
DEBUG_SEND = false
DEBUG_SEND_TIME = false
DEBUG_RECEIVE = false
DEBUG_RANDOM_TILE = false
DEBUG_GAME_STATUS = false
DEBUG_SORT_JUDGE = false
DEBUG_SORT_SLEEP = false
LOG_ENABLE = true
LOG_PATH_2048 = "logs/2048.log"
LOG_PATH_AI = "logs/ai.log"

# puts log from 2048/2048.rb
def putss(str)
    puts str
    if LOG_ENABLE
        File.open(LOG_PATH_2048, "a") do |file|
          file.puts(str)
        end
    end
end
# print log from 2048/2048.rb
def prints(str)
    print str
    if LOG_ENABLE
        File.open(LOG_PATH_2048, "a") do |file|
            file.print(str)
        end
    end
end
# puts log from ai.cpp
def putsa(str)
    puts str
    if LOG_ENABLE
        File.open(LOG_PATH_AI, "a") do |file|
          file.puts(str)
        end
    end
end
# print log from ai.cpp
def printa(str)
    print str
    if LOG_ENABLE
        File.open(LOG_PATH_AI, "a") do |file|
            file.print(str)
        end
    end
end
# puts log from ai.cpp without writing on the terminal
def putsa_log(str)
    if LOG_ENABLE
        File.open(LOG_PATH_AI, "a") do |file|
          file.puts(str)
        end
    end
end
# print log from ai.cpp without writing on the terminal
def printa_log(str)
    if LOG_ENABLE
        File.open(LOG_PATH_AI, "a") do |file|
            file.print(str)
        end
    end
end


# Public Variables------------------------------------------------
@turn_count = 0
@send_data = ""
@game_state = 0

# Preparing the data table----------------------------------------
@board_data = Array.new(ROW){Array.new(COLUMN, 0)}

# Initialize------------------------------------------------------
if LOG_ENABLE
    begin
        File.unlink(LOG_PATH_2048)
    rescue Errno::ENOENT => file_e
        File.open(LOG_PATH_2048, "w").close()
        puts "Expected error happened when deleting the latest 2048 log: #{file_e.message} Continue.."
    rescue => general_e
        putss("Unknown error when deleting the latest 2048 log: #{general_e.message} Continue..")
    end
    begin
        File.unlink(LOG_PATH_AI)
    rescue Errno::ENOENT => file_e
        File.open(LOG_PATH_AI, "w").close()
        puts "Expected error happened when deleting the latest ai log: #{file_e.message} Continue.."
    rescue => general_e
        putss("Unknown error when deleting the latest ai log: #{general_e.message} Continue..")
    end
end

# Execute the AI program via stdin/stdout.------------------------
stdin, stdout, stderr, wait_thr = *Open3.popen3('./ai.rb')

# Check the Constant value
if COLUMN < 3 || ROW < 3
    putss("COLUMN or ROW value is invalid.")
    exit 1
end
if MAX_DIGIT_TO_SHOW < 5
    putss("MAX_DIGIT_TO_SHOW value is too small.")
    exit 1
end

# Sending the status from this file to the AI file----------------
send_start = false
Thread.fork {
    sleep 0.1
    while @game_state != 2
        while !send_start
            # wait until the flag becomes false
            sleep 0.001
            if DEBUG_SEND_TIME
                p send_start
            end
        end
        if DEBUG_SEND
            prints("sending data : ")
            putss(@send_data)
        end
        stdin.puts @send_data
        if DEBUG_SEND
            putss("Sending data end.")
        end
        send_start = false
    end
    if DEBUG_SEND
        putss("Closing stdin")
    end
    stdin.close    # close_write is also available.
}
# function definition---------------------------------------------
def judgeGameover()
    gameover = true
    for zero_i in 0..(ROW - 1)
        for zero_j in 0..(ROW - 1)
            if @board_data[zero_i][zero_j] == 0
                gameover = false
            end
        end
    end
    # check the value horizontally
    for equal_i in 0..(ROW - 1)
        for equal_j in 0..(COLUMN - 2)
            if @board_data[equal_i][equal_j] == @board_data[equal_i][equal_j + 1]
                gameover = false
            end
        end
    end
    # check the value vertically
    for i in 0..(ROW - 2)
        for j in 0..(COLUMN - 1)
            if @board_data[i][j] == @board_data[i + 1][j]
                gameover = false
            end
        end
    end
    if gameover
        @game_state = 2
    end
end

def chooseRandomTile()
    if rand(100) < 90
        1
    else
        2
    end
end

def initTile()
    if @game_state == 0
        empty_tiles = Array.new()
        for i in 0..(ROW - 1)
            for j in 0..(COLUMN - 1)
                if @board_data[i][j] == 0
                    # push the available board data
                    empty_tiles[empty_tiles.length] = [i, j]
                end
            end
        end
        empty_tile_num = empty_tiles.length
        if empty_tile_num == 0
            if DEBUG_RANDOM_TILE
                putss("No tiles has been filled.")
            end
        else
            random_tile1 = rand(empty_tile_num)
            tile_info = empty_tiles[random_tile1]
            @board_data[tile_info[0]][tile_info[1]] = chooseRandomTile()
            empty_tiles.delete_at(random_tile1)
            random_tile2 = rand(empty_tile_num - 1)
            tile_info = empty_tiles[random_tile2]
            @board_data[tile_info[0]][tile_info[1]] = chooseRandomTile()
        end
    end
end

def putRandomTile()
    if @game_state == 0
        empty_tiles = Array.new()
        for i in 0..(ROW - 1)
            for j in 0..(COLUMN - 1)
                if @board_data[i][j] == 0
                    # push the available board data
                    empty_tiles[empty_tiles.length] = [i, j]
                end
            end
        end
        empty_tile_num = empty_tiles.length
        if empty_tile_num == 0
            if DEBUG_RANDOM_TILE
                putss("No tiles has been filled.")
            end
        else
            random_tile = rand(empty_tile_num)
            tile_info = empty_tiles[random_tile]
            @board_data[tile_info[0]][tile_info[1]] = chooseRandomTile()
        end
    end
end

def printBoard()
    for i in 0..(ROW - 1)
        for j in 0..(COLUMN - 1)
            elem_num = @board_data[i][j]
            if elem_num == 0
                elem_value = 0
            else
                elem_value = 2 ** elem_num
            end
            for k in 0..(MAX_DIGIT_TO_SHOW - elem_value.to_s.length)
                prints(" ")
            end
            prints(elem_value)
        end
        prints("\n")
    end
    prints("\n")
end
def setDataToSend()
    @send_data = ""
    for i in 0..(ROW - 1)
        for j in 0..(COLUMN - 1)
            @send_data += @board_data[i][j].to_s + " "
        end
    end
    @send_data += @game_state.to_s
end

def flushZero(jarray)
    if DEBUG_SORT_JUDGE
        p "before"
        p jarray
    end
    # Flushing the numbers to a certain direction
    cnt = 0
    for i in 0..(ROW - 1)
        if jarray[cnt] == 0
            jarray.delete_at(cnt)
            jarray[ROW - 1] = 0
        else
            cnt += 1
        end
    end
    if DEBUG_SORT_JUDGE
        p "center(0 is flushed.)"
        p jarray
    end
    # integration
    iterator = 0
    while iterator != ROW - 1 && jarray[iterator] != 0
        if jarray[iterator] == jarray[iterator + 1]
            jarray[iterator] += 1
            jarray.delete_at(iterator + 1)
            jarray[ROW - 1] = 0
        end
        iterator += 1
    end
    if DEBUG_SORT_JUDGE
        p "after"
        p jarray
    end
    if DEBUG_SORT_JUDGE
        printBoard()
    end
    jarray
end
#{{{goUp, goDown, goLeft, goRight
def goUp()
    if DEBUG_SORT_JUDGE
        printBoard()
    end
    board_before = Array.new(ROW){Array.new(COLUMN, 0)}
    for copy_i in 0..(ROW - 1)
        for copy_j in 0..(COLUMN - 1)
            board_before[copy_i][copy_j] = @board_data[copy_i][copy_j]
        end
    end
    for j in 0..(COLUMN - 1)
        jarray = Array.new(ROW, 0)
        for array_it in 0..(ROW - 1)
            jarray[array_it] = @board_data[array_it][j]
        end
        jarray = flushZero(jarray)
        for array_it in 0..(ROW - 1)
            @board_data[array_it][j] = jarray[array_it]
        end
        if DEBUG_SORT_JUDGE
            printBoard()
        end
    end
    if @board_data == board_before
        @game_state = 1
    end
end

def goDown()
    if DEBUG_SORT_JUDGE
        printBoard()
    end
    board_before = Array.new(ROW){Array.new(COLUMN, 0)}
    for copy_i in 0..(ROW - 1)
        for copy_j in 0..(COLUMN - 1)
            board_before[copy_i][copy_j] = @board_data[copy_i][copy_j]
        end
    end
    for j in 0..(COLUMN - 1)
        jarray = Array.new(ROW, 0)
        for array_it in 0..(ROW - 1)
            jarray[array_it] = @board_data[ROW - 1 - array_it][j]
        end
        jarray = flushZero(jarray)
        for array_it in 0..(ROW - 1)
            @board_data[ROW - 1 - array_it][j] = jarray[array_it]
        end
        if DEBUG_SORT_JUDGE
            printBoard()
        end
    end
    if @board_data == board_before
        @game_state = 1
    end
end

def goLeft()
    if DEBUG_SORT_JUDGE
        printBoard()
    end
    board_before = Array.new(ROW){Array.new(COLUMN, 0)}
    for copy_i in 0..(ROW - 1)
        for copy_j in 0..(COLUMN - 1)
            board_before[copy_i][copy_j] = @board_data[copy_i][copy_j]
        end
    end
    for i in 0..(ROW - 1)
        iarray = Array.new(COLUMN, 0)
        for array_it in 0..(COLUMN - 1)
            iarray[array_it] = @board_data[i][array_it]
        end
        iarray = flushZero(iarray)
        for array_it in 0..(ROW - 1)
            @board_data[i][array_it] = iarray[array_it]
        end
        if DEBUG_SORT_JUDGE
            printBoard()
        end
    end
    if @board_data == board_before
        @game_state = 1
    end
end

def goRight()
    if DEBUG_SORT_JUDGE
        printBoard()
    end
    board_before = Array.new(ROW){Array.new(COLUMN, 0)}
    for copy_i in 0..(ROW - 1)
        for copy_j in 0..(COLUMN - 1)
            board_before[copy_i][copy_j] = @board_data[copy_i][copy_j]
        end
    end
    for i in 0..(ROW - 1)
        iarray = Array.new(COLUMN, 0)
        for array_it in 0..(COLUMN - 1)
            iarray[array_it] = @board_data[i][COLUMN - 1 - array_it]
        end
        iarray = flushZero(iarray)
        for array_it in 0..(COLUMN - 1)
            @board_data[i][COLUMN - 1 - array_it] = iarray[array_it]
        end
        if DEBUG_SORT_JUDGE
            printBoard()
        end
    end
    if @board_data == board_before
        @game_state = 1
    end
end
#}}}

#{{{ getTileInfo(line) --for debug
def getTileInfo(line)
    # Splitting the line by Space.
    # Ignoring multi-space.
    numbers = line.split(/\s+/)
    for i in 0..(ROW - 1)
        for j in 0..(COLUMN - 1)
            @board_data[i][j] = numbers[i * COLUMN + j]
        end
    end
end
#}}}

# Initializing the board------------------------------------------
if DEBUG
    putss("debug_mode")
end
putss("Initial board state")
initTile()
setDataToSend()
printBoard()
if DEBUG
    putss("send_start")
end
send_start = true


# Getting the commands from the AI program.-----------------------
stdout.each do |line|
    line.chomp! #改行削除
    if DEBUG_RECEIVE
        putss("received_data #{line}")
    end
    judgeGameover()
    if @game_state == 2
        putss("Game Over")
        putss("total turn: #{@turn_count}")
        exit 1
    end
    @game_state = 0
    case line
    when "up", "2", "u" then
        putss("up")
        putsa_log("up")
        goUp()
    when "down", "4", "d" then
        putss("down")
        putsa_log("down")
        goDown()
    when "right", "1", "r" then
        putss("right")
        putsa_log("right")
        goRight()
    when "left", "3", "l" then
        putss("left")
        putsa_log("left")
        goLeft()
    else
        putsa("Message from AI: #{line}")
        @game_state = 1
        next
    end
    if DEBUG_GAME_STATUS
        case @game_state
        when 0 then
            putss("Move success. Continue..")
        when 1 then
            putss("Move failed, but there's another way to move the tiles.")
        when 2 then
            putss("Game Over")
        else
            putss("Invalid game_state value.")
        end
    end
    if DEBUG_SORT_SLEEP
        putss("sleep 5")
        sleep 5
    end
    putRandomTile()
    if @game_state == 0
        @turn_count += 1
    end
    putss("turn : #{@turn_count}")
    setDataToSend()
    printBoard()
    send_start = true
    if DEBUG_SEND
        putss("send_started")
    end
end
putss("end") if wait_thr.value.exitstatus
