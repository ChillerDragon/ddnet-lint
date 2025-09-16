cmake_minimum_required(VERSION 3.12...3.27.4)
project(ddnet_lint C)

set(CMAKE_C_STANDARD 99)

set(SOURCES
	src/base/math.c
	src/base/math.h
	src/base/str.c
	src/base/str.h
	src/base/detect.h
	src/checks/floats.c
	src/checks/floats.h
	src/checks/func_order.c
	src/checks/func_order.h
	src/main.c
)

add_executable(ddnet_lint ${SOURCES})
target_include_directories(ddnet_lint PRIVATE src)

# TODO: use proper cmake find thingy to also support windows and shits
target_link_options(ddnet_lint PRIVATE -lclang)

