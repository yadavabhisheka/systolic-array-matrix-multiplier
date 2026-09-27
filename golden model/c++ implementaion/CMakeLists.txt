cmake_minimum_required(VERSION 3.10)
project(SystolicArrayGoldenModel)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(test_run
    src/matmul.cpp 
    test/main.cpp 
)
target_include_directories(test_run PRIVATE include)
if(MINGW)
    target_link_options(test_run PRIVATE -mconsole)
endif()