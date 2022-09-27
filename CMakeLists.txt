cmake_minimum_required(VERSION 3.16)
project(8051Emulator C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(examples)
add_subdirectory(tests)

add_library(8051emu
        src/mcs51.c
        src/opcode_map_gen.c
        src/mcs51_register.c
        src/nvic.c
        src/opcode.c
        src/opcode_impl.c
        src/opcode_impl_weak_gen.c
        src/sfr_map_gen.c)
target_include_directories(8051emu PUBLIC include/ PRIVATE src/)

add_custom_command(OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opcode_impl_gen.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opcode_impl_weak_gen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opcode_map_gen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opcode_map_gen.h

        DEPENDS generate_opcode.py opcodes.md
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_opcode.py ${CMAKE_CURRENT_SOURCE_DIR}/opcodes.md
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Generating mcs51 opcode declarations and opcode registry")

add_custom_command(OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sfr_definitions_gen.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sfr_map_gen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sfr_map_gen.h

        DEPENDS generate_sfr.py sfrs.md
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_sfr.py ${CMAKE_CURRENT_SOURCE_DIR}/sfrs.md
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Generating mcs51 SFR definitions and SFR registry")
