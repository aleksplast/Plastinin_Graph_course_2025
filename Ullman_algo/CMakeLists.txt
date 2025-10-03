cmake_minimum_required(VERSION 3.20)
project(ullman VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package( Boost 1.40 REQUIRED )
find_package(OpenMP REQUIRED)

add_executable(ullman)

target_include_directories(ullman PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src)
target_link_libraries(ullman PRIVATE ${Boost_LIBRARIES} )
target_link_libraries(ullman PRIVATE OpenMP::OpenMP_CXX)
