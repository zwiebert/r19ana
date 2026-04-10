cmake_minimum_required(VERSION 3.16)

set(srcs main.cc select_model.cc models/model_table.cc models/x53b_740/PrintDiag.cc models/exp/PrintDiag.cc models/raw/PrintDiag.cc 
         XR25Frame.cc FrameProcessor.cc i18n.cc cli.cc )


set(srcs_esp32 esp32/main.cc esp32/pers_storage.cc 
               esp32/UartTransport.cc esp32/DataLogFile.cc)

if(CONFIG_BT_CLASSIC_ENABLED)
  list(APPEND srcs_esp32 ../bt_classic/esp32/spp.cc ../bt_classic/esp32/spp_task.cc ../bt_classic/esp32/spp2.cc)
endif()

if(CONFIG_LEAN_APP_ENABLED)
  list(APPEND srcs_esp32 ${srcs})
endif()

set(srcs_host  host/pers_storage.cc ../bt_classic/host/mock.cc)
list(APPEND srcs_host ${srcs})


if(COMMAND idf_component_register)
idf_component_register(SRCS  ${srcs_esp32}
                    REQUIRES bt nvs_flash vfs esp_driver_uart esp_driver_gpio pthread fatfs sdmmc sd_card
                             net net_http_server
                    EMBED_FILES data/r19data.bin
                    INCLUDE_DIRS "." ".." "esp32")

component_compile_options("-Wno-error=missing-field-initializers" "-Wno-error=type-limits")

else()

project("r19xr25-esp32")
add_compile_options("-std=c++20")
include_directories("." ".." "host")
add_executable(main_test  ${srcs_host} host/main_test.cc host/DataLogFile.cc)
add_executable(xr25-hex2human  ${srcs_host} host/xr25-hex2human.cc)
add_executable(xr25-bin2human  ${srcs_host} host/xr25-bin2human.cc)
add_executable(xr25-hex2bin  ${srcs_host} host/xr25-hex2bin.cc)

endif()


