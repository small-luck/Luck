cmake_minimum_required(VERSION 3.3)
project(Luck)

include_directories(/usr/software/yaml-cpp/include)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC
    src/log.cpp
    src/util.cpp
    src/config.cpp
    )

#生成一个链接库
add_library(Luck SHARED ${LIB_SRC})
  
add_executable(test_log test/test_log.cpp)
add_dependencies(test_log Luck)
target_link_libraries(test_log Luck /usr/local/lib/libyaml-cpp.so)

add_executable(test_config test/test_config.cpp)
add_dependencies(test_config Luck)
target_link_libraries(test_config Luck /usr/local/lib/libyaml-cpp.so)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
