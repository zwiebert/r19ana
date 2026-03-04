cmake_minimum_required(VERSION 3.16)
set(srcs "main.cc" "R19Frame.cc" "XR25Frame.cc" "FrameProcessor.cc")

if(COMMAND idf_component_register)
idf_component_register(SRCS ${srcs}
                    REQUIRES nvs_flash
                    INCLUDE_DIRS ".")
else()
add_executable(main_test ${srcs})
endif()


