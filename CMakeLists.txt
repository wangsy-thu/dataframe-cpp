cmake_minimum_required(VERSION 3.26)
project(dataframe_cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(dataframe_cpp main.cpp
        utils/utils.cpp
        utils/utils.h
        libs/data_table.h
        libs/data_table.cpp
        libs/date_time.cpp
        libs/date_time.h)
