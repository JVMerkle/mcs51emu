add_executable(8051emu-tests tests.c)
target_link_libraries(8051emu-tests 8051emu)
target_include_directories(8051emu-tests PRIVATE ../src)
