if(COMMAND idf_component_register)
idf_component_register(
    INCLUDE_DIRS . "./include"
    REQUIRES utils_misc
)

else()
add_library(app_config)
target_include_directories(app_config PUBLIC ./include)
target_link_libraries(app_config PUBLIC utils_misc)

endif()