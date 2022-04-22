SET(target_name libportaudio)

add_library(${target_name} IMPORTED STATIC GLOBAL)

list(APPEND LIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${target_name}
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LIB_INCLUDE_DIRS}"
)

set_target_properties(${target_name}
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}.so.2
        
)

set_property(
    GLOBAL PROPERTY INCLUDE_LIBPORTAUDIO 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}.so.2
	DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
