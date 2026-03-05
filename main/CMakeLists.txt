cmake_minimum_required(VERSION 3.16)
set(srcs "main.cc" "R19Frame.cc" "XR25Frame.cc" "FrameProcessor.cc")
set(srcs_esp32 "../bt_classic/esp32/spp.cc" "../bt_classic/esp32/spp_task.cc")
set(srcs_host "../bt_classic/host/mock.cc")

if(COMMAND idf_component_register)
idf_component_register(SRCS ${srcs} ${srcs_esp32}
                    REQUIRES bt nvs_flash vfs
                    INCLUDE_DIRS "." "..")
else()
project("r19xr25-esp32")
include_directories("." "..")
add_executable(main_test ${srcs} ${srcs_host})
endif()


