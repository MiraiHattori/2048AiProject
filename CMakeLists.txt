cmake_minimum_required(VERSION 3.0)
project(2048-game CXX)
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-missing-field-initializers -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
message("Build type:\n\t${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Compile flags:\n\t${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Compile flags:\n\t${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
else()
    message("Compile flags:\n\t${CMAKE_CXX_FLAGS}")
endif()


# Sources and headers
set(EIGEN3_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/eigen-git-mirror)
include_directories(${EIGEN3_INCLUDE_DIR})
add_definitions(-DEIGEN_NO_DEBUG)
add_definitions(-DEIGEN_INITIALIZE_MATRICES_BY_ZERO)
file(GLOB MAIN_SOURCES ${CMAKE_CURRENT_LIST_DIR} *.cpp)
set(MAIN_SOURCE_DIR ai board config util)
foreach(source_dir IN LISTS MAIN_SOURCE_DIR)
    file(GLOB dir_sources ${CMAKE_CURRENT_LIST_DIR} ${source_dir}/*.cpp)
    set(MAIN_SOURCES ${MAIN_SOURCES} ${dir_sources})
endforeach()
include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(main ${MAIN_SOURCES})
target_link_libraries(main
        pthread
        glut
        GLU
        GL)

function(add_test_target TESTNAME)
    file(GLOB TEST_SOURCES test/${TESTNAME}.cpp)
    set(TEST_SOURCE_DIR ai board config util)
    foreach(source_dir IN LISTS TEST_SOURCE_DIR)
        file(GLOB dir_sources ${CMAKE_CURRENT_LIST_DIR} ${source_dir}/*.cpp)
        set(TEST_SOURCES ${TEST_SOURCES} ${dir_sources})
    endforeach()
    include_directories(${CMAKE_CURRENT_LIST_DIR})
    add_executable(${TESTNAME}_test EXCLUDE_FROM_ALL ${TEST_SOURCES})
    target_link_libraries(${TESTNAME}_test
        pthread
        glut
        GLU
        GL)
    add_custom_target(${TESTNAME} DEPENDS ${TESTNAME}_test
    COMMAND ./${TESTNAME}_test
    )
endfunction(add_test_target)

# ClangFormat
add_custom_target(format
    COMMAND git ls-files `git rev-parse --show-toplevel` | grep \"[ch]pp\" | xargs clang-format-6.0-robotech -style=file -i)
# add_dependencies(robot format)

add_test_target(flush)
