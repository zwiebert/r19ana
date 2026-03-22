cmake_minimum_required(VERSION 3.16)
set(srcs "main.cc" "models/x53b_740/X53B_740_frame.cc" "XR25Frame.cc" "FrameProcessor.cc" "i18n.cc" select_model.cc)
set(srcs_esp32 "../bt_classic/esp32/spp.cc" "../bt_classic/esp32/spp_task.cc" "../bt_classic/esp32/spp2.cc" esp32/UartTransport.cc)
set(srcs_host "../bt_classic/host/mock.cc")

if(COMMAND idf_component_register)
idf_component_register(SRCS ${srcs} ${srcs_esp32}
                    REQUIRES bt nvs_flash vfs esp_driver_uart esp_driver_gpio pthread
                    EMBED_FILES data/r19data.bin
                    INCLUDE_DIRS "." ".." "esp32")
else()
project("r19xr25-esp32")
add_compile_options("-std=c++20")
include_directories("." ".." "host")
add_executable(main_test ${srcs} ${srcs_host})
endif()


