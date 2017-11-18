cmake_minimum_required(VERSION 3.5)
project(backsub_test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES feature.cpp axidma.c axidma.h frame1.h drivers/xbacksub.c drivers/xbacksub.h include/xparameters.h)
add_executable(backsub_test ${SOURCE_FILES})

target_link_libraries( backsub_test -lpthread -lncurses )
