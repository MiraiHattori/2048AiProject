#!/usr/bin/ruby
# puts log from 2048/2048.rb
def putss(str)
    if STDOUT_ENABLE
        puts str
    end
    if LOG_ENABLE
        File.open(LOG_PATH_2048, "a") do |file|
          file.puts(str)
        end
    end
end
# print log from 2048/2048.rb
def prints(str)
    if STDOUT_ENABLE
        print str
    end
    if LOG_ENABLE
        File.open(LOG_PATH_2048, "a") do |file|
            file.print(str)
        end
    end
end
# puts log from ai.cpp
def putsa(str)
    if STDOUT_ENABLE
        puts str
    end
    if LOG_ENABLE
        File.open(LOG_PATH_AI, "a") do |file|
          file.puts(str)
        end
    end
end
# print log from ai.cpp
def printa(str)
    if STDOUT_ENABLE
        print str
    end
    if LOG_ENABLE
        File.open(LOG_PATH_AI, "a") do |file|
            file.print(str)
        end
    end
end
# puts log from ai.cpp without writing on the terminal
def putss_log(str)
    if LOG_ENABLE
        File.open(LOG_PATH_2048, "a") do |file|
          file.puts(str)
        end
    end
end
# print log from ai.cpp without writing on the terminal
def prints_log(str)
    if LOG_ENABLE
        File.open(LOG_PATH_2048, "a") do |file|
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
