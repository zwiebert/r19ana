cmake_minimum_required(VERSION 3.16)
set(srcs select_model.cc models/model_table.cc models/x53b_740/PrintDiag.cc models/exp/PrintDiag.cc models/raw/PrintDiag.cc 
"XR25Frame.cc" "FrameProcessor.cc" "i18n.cc" )
set(srcs_esp32 esp32/main.cc esp32/pers_storage.cc "../bt_classic/esp32/spp.cc" "../bt_classic/esp32/spp_task.cc" "../bt_classic/esp32/spp2.cc" esp32/UartTransport.cc)
set(srcs_host host/main.cc host/pers_storage.cc "../bt_classic/host/mock.cc")

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


