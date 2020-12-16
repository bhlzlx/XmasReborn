cmake_minimum_required (VERSION 3.4)

project( Xmas )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/GxCMakeModule )

include( basicEnv )
include( outputPath )

#include( FindOpenAL )

find_library( OAL_LIB
    NAMES OpenAL32 HINTS ${CMAKE_CURRENT_SOURCE_DIR}/3rd/libs
    REQUIRED
    NO_DEFAULT_PATH
    NO_PACKAGE_ROOT_PATH
    NO_CMAKE_PATH
    NO_CMAKE_ENVIRONMENT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_CMAKE_SYSTEM_PATH
)

find_path( OAL_INC_DIR
    NAMES openal/al.h
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/3rd/include
    REQUIRED
    NO_DEFAULT_PATH
    NO_PACKAGE_ROOT_PATH
    NO_CMAKE_PATH
    NO_CMAKE_ENVIRONMENT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_CMAKE_SYSTEM_PATH
    PATH_SUFFIXES AL
)

message("OpenAL library : "${OAL_LIB} )
message("OpenAL include : "${OAL_INC_DIR} )

set( XMAS_SOLUTION_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( ARCHIVE_IMP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set( OGG_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/ogg/include )
set( VORBIS_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/vorbis/include )

add_subdirectory( 3rd/ogg )
add_subdirectory( 3rd/vorbis )
add_subdirectory( 3rd/SimpleAudioPlayer )

add_subdirectory( src )