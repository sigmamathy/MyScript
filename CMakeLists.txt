cmake_minimum_required (VERSION 3.20.0)

project("MyScript")

add_library(${PROJECT_NAME} "src/myscript.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_include_directories(${PROJECT_NAME} PUBLIC "include")

# tests
add_executable(minimal "test/test.cpp")
target_include_directories(minimal PUBLIC "include")
target_link_libraries(minimal PRIVATE ${PROJECT_NAME})
set_property(TARGET minimal PROPERTY CXX_STANDARD 20)

# enable testing functionality
enable_testing()

# define tests
add_test(
	NAME minimal
	COMMAND $<TARGET_FILE:minimal>
)