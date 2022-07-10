cmake_minimum_required(VERSION 3.10)

set(TARGET_NAME ozz)
set(TARGET_EXT .hdll)

project(${TARGET_NAME})

file(GLOB SOURCES "extension/*.cpp")
file(GLOB_RECURSE SOURCES_OZZ "extension/ozz/src/animation/runtime/*.cc"
    "extension/ozz/src/base/*.cc"
    "extension/ozz/src/geometry/*.cc"
    "extension/ozz/src/geometry/*.cc"
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(MSVC)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNOMINMAX)
ENDIF()

# Configure ozz
SET(ozz_build_samples OFF CACHE BOOL "" FORCE)
SET(ozz_build_howtos OFF CACHE BOOL "" FORCE)
SET(ozz_build_tests OFF CACHE BOOL "" FORCE)
SET(ozz_build_fbx OFF CACHE BOOL "" FORCE) # Disable FBX for now, re-enable later when things aren't on fire

add_subdirectory(extension/ozz)
add_library(${TARGET_NAME}${TARGET_EXT} SHARED ${SOURCES} ${SOURCES_OZZ} )


set_target_properties(${TARGET_NAME}${TARGET_EXT}
    PROPERTIES
    PREFIX ""
    OUTPUT_NAME ${TARGET_NAME}
    SUFFIX ${TARGET_EXT}
)

IF (WIN32)
    target_include_directories(${TARGET_NAME}${TARGET_EXT} PUBLIC $ENV{HASHLINKPATH}/include extension/ozz/include/ozz/ extension/ozz/include/ extension/ozz/src/ )
    target_link_libraries(${TARGET_NAME}${TARGET_EXT} $ENV{HASHLINKPATH}/libhl.lib)
ELSEIF (APPLE)
    find_library(APPLICATION_SERVICES_FRAMEWORK ApplicationServices REQUIRED)
    set(HLPATH /usr/local/opt/hashlink/libexec)
    target_include_directories(${TARGET_NAME}${TARGET_EXT} PUBLIC ${HLPATH}/include)
    target_link_libraries(${TARGET_NAME}${TARGET_EXT} ${HLPATH}/lib/libhl.dylib ${APPLICATION_SERVICES_FRAMEWORK})
ELSE()
    target_link_libraries(${TARGET_NAME}${TARGET_EXT} hl)
ENDIF()

add_custom_command(TARGET ${TARGET_NAME}${TARGET_EXT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}${TARGET_EXT}> ../..
)
