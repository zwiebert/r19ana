cmake_minimum_required(VERSION 3.16)

option(BUILD_HOST_TESTS "build host test" OFF)
option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)
option(BUILD_DOXY "build doxy docs" OFF)
option(ASAN "enable compiler sanitizer" OFF)
option(GPROF "enable gprof" OFF)

# find local versions of my components (MonoRepo-like)
# make sure this relative path does not work when building binary-dist
set(EXTRA_COMPONENT_DIRS "")
if(NOT DEFINED ENV{DISTRO})
  set(master_components_dir "${CMAKE_CURRENT_LIST_DIR}/../libs")
  file(GLOB master_components LIST_DIRECTORIES YES  ${master_components_dir}/*/components)
endif()
# now find the normal external components which include my components when not found as master components
file(GLOB external_components LIST_DIRECTORIES YES ${CMAKE_CURRENT_LIST_DIR}/external/*/components)
# if any components with same names are found, the last component in list takes precedence
list(APPEND EXTRA_COMPONENT_DIRS ${external_components} ${master_components})
message(WARNING "external_components: ${EXTRA_COMPONENT_DIRS}")

if("${PRESET_NAME}" STREQUAL "host-test" OR ${BUILD_HOST_TESTS} STREQUAL "ON")

### Unit Testing On Host Project

set(TARGET_PLATFORM_FILTER "host/.*")

project(r19anal-tests)

# delete this folder in build, or a second configure will break the build (XXX)
execute_process(COMMAND bash -c "rm -r ${PROJECT_BINARY_DIR}/comp")
# copy the config from config subdirectory. TODO: build config here instead of root-makefile
if(${BUILD_HOST_TESTS})
  message("configure host tests")
  add_custom_target(HOST_TEST_SDKCONFIG COMMAND make flavor=host-test test.cm.configure
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs/host-test/config/sdkconfig.h DESTINATION ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs/host-test/config/sdkconfig.cmake DESTINATION ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs/host-test/config/sdkconfig.json DESTINATION ${PROJECT_BINARY_DIR}/config)
endif()


set(comp_compile_feats cxx_std_20)
set(comp_compile_opts -O0 -ggdb  # -fanalyzer
)

if(${GPROF})
  add_link_options(-pg)
  add_compile_options(-pg)
endif()

if(ASAN)
  add_link_options(-fsanitize=address)
  add_compile_options(-fsanitize=address -fstack-protector-all)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/external/host-test-idf/components/test_host/host_test.cmake)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/rv_netmcu ${CMAKE_BINARY_DIR}/${BIN_COMP_ROOT}/rv_netmcu)

else()

# ESP-IDF project

### for idf without presets (prior esp-idf v6), provide locatio of sdkconfig files
if("${PRESET_NAME}" STREQUAL "")
    set(CACHE{SDKCONFIG_DEFAULTS} VALUE "configs/esp32-debug/sdkconfig.defaults" )
    set(CACHE{SDKCONFIG} VALUE "configs/esp32-debug/sdkconfig" )
endif()


set(PROJECT_NAME "rv-netmcu")

option(LINKER_DEMANGLE "set linker option --demangle" ON)


set(COMPONENTS app main webapp esptool_py)
#add_compile_options("-Wno-error=missing-field-initializers" "-include" "sdkconfig.h")

#set environment variables to be used in components
set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "-Wno-error=missing-field-initializers" "-Wno-error=switch")



include($ENV{IDF_PATH}/tools/cmake/project.cmake)

add_compile_definitions(MCU_TYPE="esp32")
add_compile_options("$<$<COMPILE_LANGUAGE:C,CXX>:-include>" "$<$<COMPILE_LANGUAGE:C,CXX>:esp_attr.h>")


if(DEFINED ENV{DISTRO})
add_compile_definitions(DISTRIBUTION)
endif()

if(${LINKER_DEMANGLE})
  add_link_options(-Wl,--demangle)
endif()


function(component_compile_features)
    target_compile_features(${COMPONENT_LIB} PRIVATE ${ARGV})
endfunction()


project("r19anal-esp32")
#set_target_properties(__idf_stm32 PROPERTIES LINK_INTERFACE_MULTIPLICITY 3)

##########################################################################


include(CTest)


endif()

