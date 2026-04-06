cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "")
set(master_components_dir "${CMAKE_CURRENT_LIST_DIR}/../libs")

file(GLOB master_components LIST_DIRECTORIES YES  ${master_components_dir}/*/components)
file(GLOB external_components LIST_DIRECTORIES YES ${CMAKE_CURRENT_LIST_DIR}/external/*/components)

list(APPEND EXTRA_COMPONENT_DIRS ${external_components} )
list(APPEND EXTRA_COMPONENT_DIRS ${master_components} )

add_compile_options("-Wno-error=missing-field-initializers" "-include" "sdkconfig.h")
 
message(WARNING "external_components: ${EXTRA_COMPONENT_DIRS}")

function(component_compile_features)
    target_compile_features(${COMPONENT_LIB} PRIVATE ${ARGV})
endfunction()

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project("r19xr25-esp32")

