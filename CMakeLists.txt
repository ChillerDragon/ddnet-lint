cmake_minimum_required(VERSION 3.5)
project(print_floats_libclang C)

set(CMAKE_C_STANDARD 99)

find_package(Clang QUIET)

if (Clang_FOUND)
    message(STATUS "Found Clang via CMake: ${Clang_INCLUDE_DIRS}")
    include_directories(${Clang_INCLUDE_DIRS})
    link_libraries(${Clang_LIBRARIES})
    add_executable(print_floats_libclang print_floats_with_lines_libclang.c)
    target_link_libraries(print_floats_libclang ${Clang_LIBRARIES})
else()
    # Fallback: try pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBCLANG REQUIRED libclang)
    include_directories(${LIBCLANG_INCLUDE_DIRS})
    link_directories(${LIBCLANG_LIBRARY_DIRS})
    add_executable(print_floats_libclang print_floats_with_lines_libclang.c)
    target_link_libraries(print_floats_libclang ${LIBCLANG_LIBRARIES})
endif()
