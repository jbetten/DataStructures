add_subdirectory(lib/googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gmock_SOURCE_DIR}/include)

add_executable(runUnitTests src/BPlusTreeTest.cpp src/NewAllocatorTest.cpp)

target_link_libraries(runUnitTests gmock gtest gtest_main)
