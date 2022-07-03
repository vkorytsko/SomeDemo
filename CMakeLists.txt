cmake_minimum_required(VERSION 3.12.0)

project(SomeDemo)

add_definitions(
    -DUNICODE
    -D_UNICODE
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

add_subdirectory(ext)
add_subdirectory(src)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
