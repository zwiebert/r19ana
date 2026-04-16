cmake_minimum_required(VERSION 3.16)
include_directories(". .. ../host")

add_executable(test_destuffing test_destuffing.cc ../XR25Frame.cc)
target_link_libraries(test_destuffing unity)
target_compile_definitions(test_destuffing PRIVATE TEST)

add_test(test_destuffing ${CMAKE_BINARY_DIR}/test/test_destuffing)


