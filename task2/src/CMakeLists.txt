target_sources(johnson PRIVATE main.cpp utils.cpp)

if (ENABLE_TESTING)
    target_sources(test_johnson PRIVATE utils.cpp)
endif()
