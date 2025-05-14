cmake_minimum_required(VERSION 3.10)
project(BubbleSortApp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(BubbleSortApp main.cpp ArrayOperations.cpp ArrayOperations.h MenuFunctions.h)