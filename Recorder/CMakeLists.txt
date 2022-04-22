cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
message(STATUS "Building Recorder")

if(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -ldl")    
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -fPIC -O0")
endif()

include_directories(../portaudio-19.7.0/include)
link_directories(../portaudio-19.7.0/lib)

#Set Target Name
set(target_name USAudio)

#Set Compile Definition
set(defines "SCAUDIO_DLLIMPL")

#Set Compile Option
set(options )

#Source Files
set(SOURCE
  src/Recorder.cpp
  src/Device.cpp
  src/SourceImpl.cpp
  src/WAVImpl.cpp
)

#Header Files
set(HEADER
  include/Device.h
  include/Recorder.h
  include/SourceImpl.h
  include/WAVImpl.h
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library (${target_name} SHARED ${SOURCE} ${HEADER})

target_include_directories(${target_name}
  PUBLIC 
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Recorder>
)

target_compile_options(${target_name} PRIVATE ${options})

target_compile_definitions(${target_name} PRIVATE ${defines})
	
set (system_libs

)

set (internal_libs

)

set (external_libs
  libportaudio
)

set_target_properties(${headers_target}
  PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

get_property(INCLUDE_LIBPORTAUDIO_PATH GLOBAL PROPERTY "INCLUDE_LIBPORTAUDIO")

target_include_directories(${target_name}
	PUBLIC ${INCLUDE_LIBPORTAUDIO_PATH}
)

target_link_libraries(${target_name}
  PUBLIC ${headers_target}
	PUBLIC ${system_libs} ${internal_libs} ${external_libs}
)

# set_property(GLOBAL PROPERTY INCLUDE_LIB_USAUDIO ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${target_name}
	PROPERTIES
	FOLDER "recorder"
)

ADD_DEFINITIONS(-DBUILD_TYPE=\"${CMAKE_BUILD_TYPE}\")
	
install(TARGETS ${target_name}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
