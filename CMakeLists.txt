# 2021-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
# See license.md for details.

# Version 3.2 and upwards.
CMAKE_MINIMUM_REQUIRED (VERSION 3.2)

# Setup versioning.
set(BUILD_MAJOR 0)
set(BUILD_MINOR 3)
set(BUILD_PATCH 1)
set(BUILD_VERSION "${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH}")

# Expose version information to config.
set(VERSION_NAME_BUFFER_SIZE 16)
set(VERSION_MAJOR ${BUILD_MAJOR})
set(VERISON_MINOR ${BUILD_MINOR})
set(VERSION_PATCH ${BUILD_PATCH})
set(VERSION_FORMAT "v%d.%d.%d")

# Specify version for this project.
project (kaji-sdl-example VERSION ${BUILD_VERSION})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE PATH "" FORCE)
endif()

if (USE_GEDANKEN)
ADD_DEFINITIONS(-DUSE_GEDANKEN=1)
endif()

string(
    COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
    IS_STANDALON_PROJECT
)

# .
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(SDL_SHARED "Build sdl2 as shared library" ON)

# Make sure libraries will be places beside executable.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (MSVC)
    # warning level 4 and all warnings as errors
    #add_compile_options(/W4 /WX)
    # Add crt dll dynamically (in either release or debug variant)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/MD)
    else ()
        add_compile_options(/MDd)
    endif ()

    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wunused-result)
endif()

# Setup git submodules.
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Update submodules ...")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update \
                --init --recursive failed with ${GIT_SUBMOD_RESULT}, \
                please checkout submodules")
        endif()
    endif()
endif()

# 
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/deps/SDL/CMakeLists.txt")
    message(FATAL_ERROR "The submodule 'SDL' could not be fetched! /
        GIT_SUBMODULE was turned off or failed. \
        Please update submodules and try again.")
endif()
# Tell find_package where to find SDL sources.
set(SDL2_SEARCH_PATHS "${PROJECT_SOURCE_DIR}/deps/SDL")
find_package(SDL2 REQUIRED)

# 
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/deps/kaji/CMakeLists.txt")
    message(FATAL_ERROR "The submodule 'kaji' could not be fetched! \
        GIT_SUBMODULE was turned off or failed. \
        Please update submodules and try again.")
endif()
add_subdirectory(deps/kaji)


# .
include_directories(inc)

# .
add_library(kaji-sdl-common SHARED
    ${PROJECT_SOURCE_DIR}/src/common.c
)
set_property(TARGET kaji-sdl-common PROPERTY C_STANDARD 17)

# .
add_executable(kaji-sdl-gui
    ${PROJECT_SOURCE_DIR}/src/main-sdl.c
)
set_property(TARGET kaji-sdl-gui PROPERTY C_STANDARD 17)
add_dependencies(kaji-sdl-gui kaji-sdl-common kaji)
target_include_directories(kaji-sdl-gui PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_options(kaji-sdl-gui PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)
target_link_libraries(kaji-sdl-gui kaji-sdl-common kaji ${SDL2_LIBRARIES})


# .
add_executable(kaji-sdl-cli
    ${PROJECT_SOURCE_DIR}/src/main-cli.c
)
set_property(TARGET kaji-sdl-cli PROPERTY C_STANDARD 17)
add_dependencies(kaji-sdl-cli kaji-sdl-common kaji)
target_link_options(kaji-sdl-cli PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)
target_link_libraries(kaji-sdl-cli kaji-sdl-common kaji)