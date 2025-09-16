cmake_minimum_required(VERSION 3.5)
project(ddnet_lint C)

set(CMAKE_C_STANDARD 99)

find_package(Clang QUIET)

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
target_link_options(ddnet_lint PRIVATE -lclang)

