add_executable(test_lib
        test_finder/test_finder.c
        test_reader/test_reader.c
        test_writer/test_writer.c
)

target_link_libraries(test_lib
        PRIVATE
        lib
        Unity
)

add_test(NAME test_lib COMMAND test_lib)