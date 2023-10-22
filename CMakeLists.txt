cmake_minimum_required(VERSION 3.22)
project(mmfileio)

set(CMAKE_CXX_STANDARD 17)


add_library(mmfilebuilder mmfilebuilder.cc)
add_library(mmfilereader mmfilereader.cc)


add_subdirectory(third/googletest)
add_subdirectory(test)