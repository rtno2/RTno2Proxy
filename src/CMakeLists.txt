set(rtno_main_srcs rtno_main.cpp)
set(com2tcp_main_srcs com2tcp_main.cpp)
set(rtno_test_srcs rtno_test.cpp)

add_subdirectory(hal)
add_subdirectory(rtno2proxy)

add_executable(rtno2 ${rtno_main_srcs})
target_link_libraries(rtno2 rtno_hal rtno_proxy)


add_executable(com2tcp ${com2tcp_main_srcs})
target_link_libraries(com2tcp rtno_hal )


add_executable(rtno_test ${rtno_test_srcs})
target_link_libraries(rtno_test rtno_hal rtno_proxy)