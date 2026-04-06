set(srcs 
    app_cli/cli.cc
    app_cli/parm_cmd.cc
    app_cli/parm_config.cc app_cli/parm_config_comp.cc app_cli/parm_config_app.cc app_cli/parm_config_help.cc
    app_cli/parm_help.cc
    app_cli/parm_kvs.cc
    app_cli/parm_mcu.cc
    app_cli/parm_status.cc
    app_cli/obj_json.cc
    
    app_main/esp32/main.cc app_main/esp32/main_setup.cc app_main/esp32/main_loop.cc
    app_main/esp32/unity_setup.cc app_main/esp32/main_loop_periodic.cc
 
    app_misc/rtc.cc    app_misc/esp32/kvstore.cc  app_misc/opt_map.cc

    app_uout/so_print.cc app_uout/so_write_msg.cc app_uout/so_write_config.cc app_uout/callbacks.cc

    app_mqtt/mqtt.cc
    
    app_settings/config.cc app_settings/config_kvs.cc app_settings/config_comps.cc app_settings/app_settings.cc 
    app_http_server/esp32/uri_handlers.cc app_http_server/web_content.cc
    
)

# Add experimental host sources, which are not yet part of the firmware
if (TEST_HOST)
  set(srcs ${srcs}  full_auto/scratch/configure_structs.cc
                    full_auto/scratch/fhemutils.cc  
)
endif()

if(CONFIG_APP_USE_PROTOBUF)
  set(srcs ${srcs}  app_cli/parm_proto_buf.cc)
endif()

if(CONFIG_APP_USE_WEATHER_AUTO OR TEST_HOST)
  set(srcs ${srcs}  full_auto/fa_weather.cc full_auto/setup.cc
     full_auto/automatic_timer.cc
  )
set(incdirs ${incdirs} full_auto/include)
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS . ${incdirs} app_cli/include app_misc/include app_net/include app_uout/include 
                   app_mqtt/include  app_http_server/include app_settings/include
    REQUIRES       app_config cli firmware_update uout utils_debug utils_misc utils_time weather jsoneat kvs
                   esp_driver_gpio
    PRIV_REQUIRES app_update cli config_kvs driver esp_event esp_netif kvs lwip net_http_server
                  main_loop mbedtls net  net_mqtt_client nvs_flash storage txtio uout utils_debug
                  utils_misc utils_time 
                  espcoredump
)
 
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
