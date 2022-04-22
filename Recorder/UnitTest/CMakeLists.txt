file(GLOB_RECURSE SOURCE  ./*.cpp ../src/*.cpp)
message(${SOURCE})

cmake_minimum_required(VERSION 3.8)
SET(CMAKE_CXX_STANDARD 17)
MESSAGE(STATUS "Building RecorderTest")

SET(target_name RecorderTest)

set(CMAKE_VERBOSE_MAKEFILE ON)

if(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -ldl")    
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -fPIC -O0")
endif()
include_directories(../../thirdparty/portaudio-19.7.0/include)
link_directories(../../thirdparty/portaudio-19.7.0/lib)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include_directories(
  ../include/
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(${target_name} ${SOURCE})

SET(internal_libs

)

SET(external_libs
  libportaudio
)

get_property(INCLUDE_LIBPORTAUDIO_PATH GLOBAL PROPERTY "INCLUDE_LIBPORTAUDIO")

target_include_directories(${target_name}
	PUBLIC ${INCLUDE_LIBPORTAUDIO_PATH}
)

target_link_libraries(${target_name} 
	PUBLIC ${system_libs} ${internal_libs} ${external_libs}
)

# set_target_properties(${target_name}
#   PROPERTIES
#   FOLDER "Test"
# )

install(TARGETS ${target_name}
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/UnitTest
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
