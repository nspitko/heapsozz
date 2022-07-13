cmake_minimum_required(VERSION 3.15)

set(TARGET_NAME ozz)
set(TARGET_EXT .hdll)

project(${TARGET_NAME})

# Configure CMake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

# Search for FBX sdk package
find_package(Fbx 2016)

file(GLOB SOURCES "extension/*.cpp")
file(GLOB SOURCES_OZZ   "extension/ozz/src/animation/runtime/*.cc"
                        "extension/ozz/src/base/*.cc"
                        "extension/ozz/src/base/containers/*.cc"
                        "extension/ozz/src/base/io/*.cc"
                        "extension/ozz/src/base/maths/*.cc"
                        "extension/ozz/src/base/memory/*.cc"
                        "extension/ozz/src/geometry/runtime/*.cc"
                        "extension/ozz/src/options/*.cc"
)

file(GLOB SOURCES_OZZ_OFFLINE   "extension/ozz/src/geometry/offline/*.cc"
                                "extension/ozz/src/animation/offline/*.cc"
                                "extension/ozz/src/animation/offline/fbx/*.cc"
                                "extension/ozz/src/animation/offline/gltf/*.cc"
                                "extension/ozz/src/animation/offline/tools/*.cc"
                                "extension/ozz/extern/jsoncpp/dist/*.cpp"
)

# Remove stuff with main functions
list(REMOVE_ITEM SOURCES_OZZ_OFFLINE    ${PROJECT_SOURCE_DIR}/extension/ozz/src/animation/offline/tools/dump2ozz.cc
                                        ${PROJECT_SOURCE_DIR}/extension/ozz/src/animation/offline/fbx/fbx2ozz.cc
                                        ${PROJECT_SOURCE_DIR}/extension/ozz/src/animation/offline/gltf/gltf2ozz.cc )

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


add_library(${TARGET_NAME}${TARGET_EXT} SHARED ${SOURCES} ${SOURCES_OZZ} )


set_target_properties(${TARGET_NAME}${TARGET_EXT}
    PROPERTIES
    PREFIX ""
    OUTPUT_NAME ${TARGET_NAME}
    SUFFIX ${TARGET_EXT}
)

target_compile_definitions(${TARGET_NAME}${TARGET_EXT} PUBLIC HL=1)

IF (WIN32)
    target_include_directories(${TARGET_NAME}${TARGET_EXT} PUBLIC $ENV{HASHLINKPATH}/include ${FBX_INCLUDE_DIR} extension/ozz/include/ozz/ extension/ozz/include/ extension/ozz/src/ )
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


## Importer
if( FBX_FOUND )
    #
    # fbx2mesh
    #
    add_executable(fbx2mesh
        "extension/tools/fbx2mesh.cpp"
        ${SOURCES_OZZ}
        ${SOURCES_OZZ_OFFLINE}
        ${PROJECT_SOURCE_DIR}/extension/mesh.cpp
        ${PROJECT_SOURCE_DIR}/extension/mesh.h )

    set_target_properties(fbx2mesh
        PROPERTIES FOLDER "tools" )

    set_target_properties(fbx2mesh PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY_DEBUG tools/debug
                        RUNTIME_OUTPUT_DIRECTORY_RELEASE tools/release
    )

    target_include_directories(fbx2mesh PUBLIC ${FBX_INCLUDE_DIRS} extension/ozz/include/ extension/ozz/src/ extension/ozz/extern/jsoncpp/dist )
    target_link_libraries(fbx2mesh ${FBX_LIBRARIES_DEBUG})
    set_target_properties(fbx2mesh PROPERTIES LINK_FLAGS "/ignore:4099")

    #
    # fbx2ozz
    #
    add_executable(fbx2ozz
        ${PROJECT_SOURCE_DIR}/extension/ozz/src/animation/offline/fbx/fbx2ozz.cc
        ${SOURCES_OZZ}
        ${SOURCES_OZZ_OFFLINE} )

    set_target_properties(fbx2ozz
        PROPERTIES FOLDER "tools")

    set_target_properties(fbx2ozz PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY_DEBUG tools/debug
                        RUNTIME_OUTPUT_DIRECTORY_RELEASE tools/release
    )

    target_include_directories(fbx2ozz PUBLIC ${FBX_INCLUDE_DIRS} extension/ozz/include/ extension/ozz/src/ extension/ozz/extern/jsoncpp/dist )
    target_link_libraries(fbx2ozz ${FBX_LIBRARIES_DEBUG})
    set_target_properties(fbx2ozz PROPERTIES LINK_FLAGS "/ignore:4099")
endif()