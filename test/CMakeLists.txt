add_subdirectory(lib/googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gmock_SOURCE_DIR}/include)

add_executable(runUnitsTests src/BPlusTreeTest.cpp src/NewAllocatorTest.cpp)

target_link_libraries(runUnitsTests gmock gtest gtest_main)
