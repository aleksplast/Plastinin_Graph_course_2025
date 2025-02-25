cmake_minimum_required(VERSION 3.20)
project(red_black_tree VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(red_black_tree)
target_include_directories(red_black_tree PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src)

