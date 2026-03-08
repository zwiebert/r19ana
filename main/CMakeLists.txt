cmake_minimum_required(VERSION 3.16)
set(srcs "main.cc" "R19Frame.cc" "XR25Frame.cc" "FrameProcessor.cc")
set(srcs_esp32 "../bt_classic/esp32/spp.cc" "../bt_classic/esp32/spp_task.cc" esp32/UartTransport.cc)
set(srcs_host "../bt_classic/host/mock.cc")

if(COMMAND idf_component_register)
idf_component_register(SRCS ${srcs} ${srcs_esp32}
                    REQUIRES bt nvs_flash vfs esp_driver_uart esp_driver_gpio pthread
                    INCLUDE_DIRS "." ".." "esp32")
else()
project("r19xr25-esp32")
add_compile_options("-std=c++20")
include_directories("." ".." "host")
add_executable(main_test ${srcs} ${srcs_host})
endif()


