cmake_minimum_required(VERSION 3.12.0)

set(TARGET_NAME ${PROJECT_NAME})

add_executable(${TARGET_NAME} main.cpp)

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(
    ${TARGET_NAME}
    PRIVATE
    /W4
    /WX
    /MP
)
