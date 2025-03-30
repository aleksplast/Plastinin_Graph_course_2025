cmake_minimum_required(VERSION 3.20)
project(johnson VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -fsanitize=address)
add_link_options(-fsanitize=address)

find_package( Boost 1.40 REQUIRED )

add_executable(johnson)
target_include_directories(johnson PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})
add_subdirectory(src)
target_link_libraries(johnson LINK_PUBLIC ${Boost_LIBRARIES} )
