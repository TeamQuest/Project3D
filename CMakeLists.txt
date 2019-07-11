cmake_minimum_required(VERSION 3.2.3)

if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    cmake_policy (SET CMP0022 NEW)
    cmake_policy (SET CMP0042 NEW)
    cmake_policy (SET CMP0063 OLD)
    cmake_policy (SET CMP0026 OLD)
endif()

project(Project3D)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
set(CMAKE_CXX_STANDARD 17)
include(UrhoCommon)
add_compile_options(
    # Warnings
    -Wall
    -Wextra
    -Weffc++
    -Wpedantic
    -Wno-inconsistent-missing-override
)

set(TARGET_NAME project3D)

# define_source_files()
file(GLOB_RECURSE CPP_FILES src/*.cpp)

# file(GLOB H_FILES src/*.h* src/*/*.h* RECURSE GROUP)
include_directories(src)

set(SOURCE_FILES ${CPP_FILES} ${H_FILES})

setup_main_executable()
