cmake_minimum_required(VERSION 3.20)
project(johnson VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -fsanitize=address)
add_link_options(-fsanitize=address)

find_package( Boost 1.40 REQUIRED )

if (ENABLE_TESTING)
    add_executable(test_johnson)
endif()

add_executable(johnson)
target_include_directories(johnson PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})
add_subdirectory(src)
target_link_libraries(johnson LINK_PUBLIC ${Boost_LIBRARIES} )

if (ENABLE_TESTING)
    add_subdirectory(tests)

    #GTest
    find_package(GTest REQUIRED)
    enable_testing()
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    #Tests
    target_include_directories(test_johnson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(test_johnson gtest gtest_main)
    target_link_libraries(test_johnson LINK_PUBLIC ${Boost_LIBRARIES})
    add_test(test_johnson test_johnson)
endif()
