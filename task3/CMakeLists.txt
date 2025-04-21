cmake_minimum_required(VERSION 3.20)
project(task3 VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(task3)
target_include_directories(task3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src)

