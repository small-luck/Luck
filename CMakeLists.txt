cmake_minimum_required(VERSION 3.3)
project(Luck)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC
    src/utils/log.cpp
    src/utils/utils.cpp
   )

#生成一个链接库
add_library(Luck SHARED ${LIB_SRC})

add_executable(test test/test_log.cpp)
add_dependencies(test Luck)
target_link_libraries(test Luck)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
