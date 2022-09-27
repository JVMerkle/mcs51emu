add_executable(8051emu-example example.c example.bin)
target_link_libraries(8051emu-example 8051emu)

add_custom_command(OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/example.bin
        ${CMAKE_CURRENT_SOURCE_DIR}/example.array.txt

        DEPENDS Makefile example.S
        COMMAND make
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Assembling mcs51 examples"
        )
