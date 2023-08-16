cmake_minimum_required(VERSION 3.12.0)

# DirectXMath
add_subdirectory(DirectXMath)


# DirectXTex
set(BUILD_TOOLS OFF)
set(BUILD_SAMPLE OFF)
set(BUILD_DX11 OFF)
set(BUILD_DX12 OFF)
set(BC_USE_OPENMP OFF)
add_subdirectory(DirectXTex)


# TinyGLTF
# set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
# set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
# add_subdirectory(tinygltf)


# ImGUI
set(IMGUI_TARGET_NAME imgui)
set(IMGUI_SUBMODULE_DIR "${PROJECT_SOURCE_DIR}/ext/imgui/")

set(IMGUI_SOURCES
    ${IMGUI_SUBMODULE_DIR}imgui.cpp
    ${IMGUI_SUBMODULE_DIR}imgui_demo.cpp
    ${IMGUI_SUBMODULE_DIR}imgui_draw.cpp
    ${IMGUI_SUBMODULE_DIR}imgui_tables.cpp
    ${IMGUI_SUBMODULE_DIR}imgui_widgets.cpp
    ${IMGUI_SUBMODULE_DIR}backends/imgui_impl_dx11.cpp
    ${IMGUI_SUBMODULE_DIR}backends/imgui_impl_win32.cpp
)
set(IMGUI_HEADERS
    ${IMGUI_SUBMODULE_DIR}imconfig.h
    ${IMGUI_SUBMODULE_DIR}imgui.h
    ${IMGUI_SUBMODULE_DIR}imgui_internal.h
    ${IMGUI_SUBMODULE_DIR}imstb_rectpack.h
    ${IMGUI_SUBMODULE_DIR}imstb_textedit.h
    ${IMGUI_SUBMODULE_DIR}imstb_truetype.h
    ${IMGUI_SUBMODULE_DIR}backends/imgui_impl_dx11.h
    ${IMGUI_SUBMODULE_DIR}backends/imgui_impl_win32.h
)

add_library(${IMGUI_TARGET_NAME} STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})

set_target_properties(
    ${IMGUI_TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(
    ${IMGUI_TARGET_NAME}
    PUBLIC
    ${IMGUI_SUBMODULE_DIR}
)
