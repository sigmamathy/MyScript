cmake_minimum_required (VERSION 3.20.0)

project("MyScript")

add_library(${PROJECT_NAME} src/myscript.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PUBLIC "include")