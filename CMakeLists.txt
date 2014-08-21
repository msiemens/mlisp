project(mlisp)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE "Debug")

# Setting the compiler
#################################################################################
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
#set(CMAKE_C_COMPILER "gcc")
#set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_C_COMPILER "C:\\Program Files (x86)\\LLVM\\bin\\ccc-analyzer")
#set(CMAKE_CXX_COMPILER "C:\\Program Files (x86)\\LLVM\\bin\\c++-analyzer")

# Add sources
#################################################################################
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/builtins)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/profiler)

include_directories(${PROJECT_SOURCE_DIR}/lib/mpc)
include_directories(${PROJECT_SOURCE_DIR}/lib/hash)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

# Configure custom preprocessing
#################################################################################
set_source_files_properties(${PROJECT_SOURCE_DIR}/gen/parser.c
                            PROPERTIES GENERATED true)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/gen/parser.c
                   COMMAND python ../preprocess.py
                   DEPENDS ${PROJECT_SOURCE_DIR}/src/grammar.ebnf
                   DEPENDS ${PROJECT_SOURCE_DIR}/src/parser.c)

# Configure custom postprocessing
#################################################################################
set(GENERATE_PDB "false" CACHE BOOL "Generate .pdb files for Visual Studio using cv2pdb")

if (GENERATE_PDB)
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/mlisp.pdb
                       COMMAND cv2pdb.exe ${CMAKE_BINARY_DIR}/mlisp.exe
                       COMMENT "Generating pdb file for mlisp.exe"
                       DEPENDS mlisp-bin)
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libmlisp.pdb
                       COMMAND cv2pdb.exe ${CMAKE_BINARY_DIR}/libmlisp.dll
                       COMMENT "Generating pdb file for libmlisp.dll"
                       DEPENDS libmlisp)
    add_custom_target(generate_pdb ALL
                       DEPENDS ${CMAKE_BINARY_DIR}/mlisp.pdb
                       DEPENDS ${CMAKE_BINARY_DIR}/libmlisp.pdb)
endif(GENERATE_PDB)

# Configure executables and libraries
#################################################################################
add_library(mpc               ${PROJECT_SOURCE_DIR}/lib/mpc/mpc.c)
add_library(hash              ${PROJECT_SOURCE_DIR}/lib/hash/hash.c)
add_library(profiler          ${PROFILER_SOURCES})
add_library(mlisp OBJECT      ${MLISP_SOURCES})
add_library(libmlisp SHARED   $<TARGET_OBJECTS:mlisp>)
add_executable(mlisp-bin      $<TARGET_OBJECTS:mlisp> ${PROJECT_SOURCE_DIR}/src/main.c)
add_executable(mlisp-profiler $<TARGET_OBJECTS:mlisp> ${PROJECT_SOURCE_DIR}/src/profiler/main.c)

target_link_libraries(libmlisp mpc hash)
target_link_libraries(mlisp-bin mpc hash)
target_link_libraries(mlisp-profiler profiler mpc hash)

if (NOT WIN32)
    target_link_libraries(mlisp-bin readline m)
endif(NOT WIN32)

# Setting compiler flags
#################################################################################
set(DEBUG "false" CACHE BOOL "Compile with debug information")

set(CUSTOM_FLAGS "-std=c11")
set(CUSTOM_FLAGS "${CUSTOM_FLAGS} -pedantic -Wextra -Wall")
set(CUSTOM_FLAGS "${CUSTOM_FLAGS} -Wuninitialized -Winit-self -Waggregate-return -Wconversion -Wswitch-enum -Wformat -Wfloat-equal -Wconversion -Wshadow -Wunreachable-code")
set(CUSTOM_FLAGS "${CUSTOM_FLAGS} -Wstrict-aliasing -fstrict-aliasing")
if (DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
else (DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif (DEBUG)

set_target_properties(mlisp          PROPERTIES COMPILE_FLAGS ${CUSTOM_FLAGS})
set_target_properties(mlisp-bin      PROPERTIES COMPILE_FLAGS ${CUSTOM_FLAGS})
set_target_properties(mlisp-profiler PROPERTIES COMPILE_FLAGS ${CUSTOM_FLAGS})

if (WIN32)
    set_target_properties(libmlisp       PROPERTIES OUTPUT_NAME libmlisp)
else (WIN32)
    set_target_properties(libmlisp       PROPERTIES OUTPUT_NAME mlisp)
endif(WIN32)
set_target_properties(mlisp-bin      PROPERTIES OUTPUT_NAME mlisp)
