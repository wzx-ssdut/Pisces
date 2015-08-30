#/****************************************************************************
# Copyright (c) 2013 cocos2d-x.org
# Copyright (c) 2014 martell malone
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 2.8)
# It ensures that when Find*.cmake files included from cmake's Modules dir
# include another *.cmake file with relative path, that file will be included
# also from cmake's Modules dir, to not clash with per-project files.
cmake_policy(SET CMP0017 NEW)

# Use new behaviour with cmake >= 3.1:
# Only interpret if() arguments as variables or keywords when unquoted.
if(CMAKE_VERSION VERSION_GREATER 3.1)
    cmake_policy(SET CMP0054 NEW)
endif()

project (Cocos2d-X)

# The version number
set(COCOS2D_X_VERSION 3.7.1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")
include(CocosBuildHelpers)

message(${BUILDING_STRING})

set(USE_WEBP_DEFAULT ON)
set(USE_PREBUILT_LIBS_DEFAULT ON)

option(USE_CHIPMUNK "Use chipmunk for physics library" ON)
option(USE_BOX2D "Use box2d for physics library" OFF)
option(USE_BULLET "Use bullet for physics3d library" ON)
option(USE_RECAST "Use Recast for navigation mesh" ON)
option(USE_WEBP "Use WebP codec" ${USE_WEBP_DEFAULT})
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(DEBUG_MODE "Debug or release?" ON)
option(BUILD_EXTENSIONS "Build extension library" ON)
option(BUILD_EDITOR_SPINE "Build editor support for spine" ON)
option(BUILD_EDITOR_COCOSTUDIO "Build editor support for cocostudio" ON)
option(BUILD_EDITOR_COCOSBUILDER "Build editor support for cocosbuilder" ON)
option(USE_PREBUILT_LIBS "Use prebuilt libraries in external directory" ${USE_PREBUILT_LIBS_DEFAULT})


if(DEBUG_MODE)
    set(CMAKE_BUILD_TYPE DEBUG)
else(DEBUG_MODE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif(DEBUG_MODE)


# Compiler options
set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wno-unused-parameter -Wno-unused-variable -Wextra -DCOCOS2D_DEBUG=1")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -std=c++11 -Wno-deprecated-declarations -Wno-reorder")


set(COCOS_EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

# Some macro definitions
ADD_DEFINITIONS(-DLINUX)
set(PLATFORM_FOLDER linux)


# architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "64-bit")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCH_DIR "32-bit")
else()
    message( FATAL_ERROR "Unsupported architecture, CMake will exit" )
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/cocos
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/deprecated
    ${CMAKE_CURRENT_SOURCE_DIR}/cocos/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/extensions
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

if(USE_PREBUILT_LIBS)
    include(CocosUsePrebuiltLibs)
endif()

# desktop platforms
cocos_find_package(OpenGL OPENGL REQUIRED)
cocos_find_package(GLEW GLEW REQUIRED)
#TODO: implement correct schema for pass cocos2d specific requirements to projects
include_directories(${GLEW_INCLUDE_DIRS})

cocos_find_package(GLFW3 GLFW3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIRS})


set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
set(THREADS_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})


cocos_find_package(FMODEX FMODEX REQUIRED)
cocos_find_package(Fontconfig FONTCONFIG REQUIRED)


# Freetype required on all platforms
cocos_find_package(Freetype FREETYPE REQUIRED)

# WebP required if used
if(USE_WEBP)
    cocos_find_package(WebP WEBP REQUIRED)
endif(USE_WEBP)

# Chipmunk
if(USE_CHIPMUNK)
    cocos_find_package(Chipmunk CHIPMUNK REQUIRED)
    add_definitions(-DCC_ENABLE_CHIPMUNK_INTEGRATION=1)
else()
    add_definitions(-DCC_USE_PHYSICS=0)
endif()

# Box2d (not prebuilded, exists as source)
if(USE_BOX2D)
    if(USE_PREBUILT_LIBS)
        add_subdirectory(external/Box2D)
        set(Box2D_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/box2d/include)
        set(Box2D_LIBRARIES box2d)
    else()
        find_package(Box2D REQUIRED CONFIG)
        # actually Box2D in next line is not a library, it is target exported from Box2DConfig.cmake
        set(Box2D_LIBRARIES Box2D)
    endif()
    message(STATUS "Box2D include dirs: ${Box2D_INCLUDE_DIRS}")
    add_definitions(-DCC_ENABLE_BOX2D_INTEGRATION=1)
else()
    add_definitions(-DCC_ENABLE_BOX2D_INTEGRATION=0)
endif()

# Bullet (not prebuilded, exists as source)
if(USE_BULLET)
    if(USE_PREBUILT_LIBS)
        add_subdirectory(external/bullet)
        set(BULLET_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/bullet)
        set(BULLET_LIBRARIES bullet)
    else()
        cocos_find_package(bullet BULLET REQUIRED)
        set(BULLET_LIBRARIES bullet)
    endif()
    message(STATUS "Bullet include dirs: ${BULLET_INCLUDE_DIRS}")
    add_definitions(-DCC_ENABLE_BULLET_INTEGRATION=1)
    add_definitions(-DCC_USE_PHYSICS=1)
else()
    add_definitions(-DCC_ENABLE_BULLET_INTEGRATION=0)
    add_definitions(-DCC_USE_3D_PHYSICS=0)
endif(USE_BULLET)

# Recast (not prebuilded, exists as source)
if(USE_RECAST)
    if(USE_PREBUILT_LIBS)
        add_subdirectory(external/recast)
        set(RECAST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/recast)
        set(RECAST_LIBRARIES recast)
    else()
        cocos_find_package(recast RECAST REQUIRED)
        set(RECAST_LIBRARIES recast)
    endif()
    message(STATUS "Recast include dirs: ${RECAST_INCLUDE_DIRS}")
    add_definitions(-DCC_USE_NAVMESH=1)
else()
    add_definitions(-DCC_USE_NAVMESH=0)
endif(USE_RECAST)

# Tinyxml2 (not prebuilded, exists as source)
if(USE_PREBUILT_LIBS)
    add_subdirectory(external/tinyxml2)
    set(TinyXML2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyxml2)
    set(TinyXML2_LIBRARIES tinyxml2)
else()
    cocos_find_package(TinyXML2 TinyXML2 REQUIRED)
endif()
message(STATUS "TinyXML2 include dirs: ${TinyXML2_INCLUDE_DIRS}")


# libjpeg
cocos_find_package(JPEG JPEG REQUIRED)
cocos_find_package(ZLIB ZLIB REQUIRED)


# minizip (we try to migrate to minizip from https://github.com/nmoinvaz/minizip)
# only msys2 currently provides package for this variant, all other
# dists have packages from zlib, thats very old for us.
# moreover our embedded version modified to quick provide
# functionality needed by cocos.
if(USE_PREBUILT_LIBS OR NOT MINGW)
    #TODO: hack! should be in external/unzip/CMakeLists.txt
    include_directories(${ZLIB_INCLUDE_DIRS})
    add_subdirectory(external/unzip)
    set(MINIZIP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/unzip ${ZLIB_INCLUDE_DIRS})
    set(MINIZIP_LIBRARIES unzip ${ZLIB_LIBRARIES})
    message(STATUS "MINIZIP include dirs: ${MINIZIP_INCLUDE_DIRS}")
else()
    cocos_find_package(MINIZIP MINIZIP REQUIRED)
    # double check that we have needed functions
    include(CheckLibraryExists)
    check_library_exists(${MINIZIP_LIBRARIES} "unzGoToFirstFile2" "" MINIZIP_HAS_GOTOFIRSTFILE2)
    if(NOT MINIZIP_HAS_GOTOFIRSTFILE2)
        message(FATAL_ERROR "Minizip library on you system very old. Please use recent version from https://github.com/nmoinvaz/minizip or enable USE_PREBUILT_LIBS")
    endif()
    add_definitions(-DMINIZIP_FROM_SYSTEM)
endif()

cocos_find_package(PNG PNG REQUIRED)
cocos_find_package(TIFF TIFF REQUIRED)
cocos_find_package(WEBSOCKETS WEBSOCKETS REQUIRED)
cocos_find_package(CURL CURL REQUIRED)

# flatbuffers
if(USE_PREBUILT_LIBS)
    add_subdirectory(external/flatbuffers)
    set(FLATBUFFERS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external)
    message(STATUS "Flatbuffers include dirs: ${FLATBUFFERS_INCLUDE_DIRS}")
else()
    cocos_find_package(flatbuffers flatbuffers REQUIRED)
endif()

# xxhash
if(USE_PREBUILT_LIBS)
    add_subdirectory(external/xxhash)
    set(XXHASH_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/xxhash)
    set(XXHASH_LIBRARIES xxhash)
else()
    cocos_find_package(xxhash xxhash REQUIRED)
endif()
# libcocos2d.a
add_subdirectory(cocos)
