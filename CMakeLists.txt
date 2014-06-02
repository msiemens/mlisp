project(mlisp)
cmake_minimum_required(VERSION 2.6)

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
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_SOURCE_DIR}/lib/ptest)
include_directories(${PROJECT_SOURCE_DIR}/lib/mpc)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

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
add_library(ptest ${PROJECT_SOURCE_DIR}/lib/ptest/ptest.c)
add_library(mpc   ${PROJECT_SOURCE_DIR}/lib/mpc/mpc.c)
add_library(mlisp SHARED
                  ${PROJECT_SOURCE_DIR}/src/lval.c    ${PROJECT_SOURCE_DIR}/src/lenv.c
                  ${PROJECT_SOURCE_DIR}/src/builtin.c ${PROJECT_SOURCE_DIR}/gen/grammar.c
                  ${PROJECT_SOURCE_DIR}/src/eval.c    ${PROJECT_SOURCE_DIR}/src/utils.c)
add_executable(mlisp-bin ${PROJECT_SOURCE_DIR}/src/main.c)
target_link_libraries(mlisp mpc)
target_link_libraries(mlisp-bin mlisp mpc)

# Setting compiler flags
#################################################################################
set(CUSTOM_FLAGS "-std=c11")
set(CUSTOM_FLAGS "${CUSTOM_FLAGS} -pedantic -Wextra -Wall")
set(CUSTOM_FLAGS "${CUSTOM_FLAGS} -Wuninitialized -Wmissing-declarations -Winit-self")
if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif (CMAKE_BUILD_TYPE EQUAL "DEBUG")

set_target_properties(mlisp     PROPERTIES COMPILE_FLAGS ${CUSTOM_FLAGS})
set_target_properties(mlisp-bin PROPERTIES COMPILE_FLAGS ${CUSTOM_FLAGS})
set_target_properties(mlisp-bin PROPERTIES OUTPUT_NAME mlisp)