project(mlisp)

set(WARN_FLAGS "-pedantic-errors -Wextra -Wall")
set(WARN_FLAGS "${WARN_FLAGS} -Wold-style-cast -Wuninitialized -Wmissing-declarations -Winit-self")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -std=c99")  # DEBUG FLAG IS SET!
cmake_minimum_required(VERSION 2.6)

# Setting the compiler
#################################################################################
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

# Add sources
#################################################################################
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_SOURCE_DIR}/lib/mpc)
include_directories(${PROJECT_SOURCE_DIR}/src/)

# Configure custom preprocessing
#################################################################################
set_source_files_properties(${PROJECT_SOURCE_DIR}/gen/grammar.c
                            PROPERTIES GENERATED true)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/gen/grammar.c
                   COMMAND python ../preprocess.py
                   DEPENDS ${PROJECT_SOURCE_DIR}/src/grammar.ebnf
                   DEPENDS ${PROJECT_SOURCE_DIR}/src/grammar.c)

# Configure executables and libraries
#################################################################################
add_library(mpc ${PROJECT_SOURCE_DIR}/lib/mpc/mpc.c)
add_executable(mlisp ${PROJECT_SOURCE_DIR}/src/eval.c ${PROJECT_SOURCE_DIR}/gen/grammar.c ${PROJECT_SOURCE_DIR}/src/main.c)
target_link_libraries(mlisp mpc)

set_target_properties(mlisp
                      PROPERTIES COMPILE_FLAGS ${WARN_FLAGS})