cmake_minimum_required(VERSION 3.12.0)

set(TARGET_NAME ${PROJECT_NAME})

set(SOURCES
    main.cpp

    application.cpp
    camera.cpp
    debug_layer.cpp
    exceptions.cpp
    index_buffer.cpp
    renderer.cpp
    scene.cpp
    timer.cpp
    vertex_buffer.cpp
    window.cpp
)
set(HEADERS
    application.hpp
    camera.hpp
    debug_layer.hpp
    exceptions.hpp
	index_buffer.hpp
    renderer.hpp
    scene.hpp
    timer.hpp
	vertex_buffer.hpp
    window.hpp

    utils.hpp
)

set(SHADERS_SOURCE_DIR
    ${PROJECT_SOURCE_DIR}/res/shaders
)

set(VERTEX_SHADERS
    ${SHADERS_SOURCE_DIR}/color.vs.hlsl
    ${SHADERS_SOURCE_DIR}/light.vs.hlsl
    ${SHADERS_SOURCE_DIR}/phong.vs.hlsl
    ${SHADERS_SOURCE_DIR}/texture.vs.hlsl
)

set(PIXEL_SHADERS
    ${SHADERS_SOURCE_DIR}/color.ps.hlsl
    ${SHADERS_SOURCE_DIR}/light.ps.hlsl
    ${SHADERS_SOURCE_DIR}/phong.ps.hlsl
    ${SHADERS_SOURCE_DIR}/texture.ps.hlsl
)

set_property(SOURCE ${VERTEX_SHADERS} PROPERTY VS_SHADER_TYPE Vertex)
set_property(SOURCE ${VERTEX_SHADERS} PROPERTY VS_SHADER_MODEL 4.0_level_9_3)
set_property(SOURCE ${PIXEL_SHADERS} PROPERTY VS_SHADER_TYPE Pixel)
set_property(SOURCE ${PIXEL_SHADERS} PROPERTY VS_SHADER_MODEL 4.0_level_9_3)
source_group("shaders" FILES ${VERTEX_SHADERS} ${PIXEL_SHADERS})

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS} ${VERTEX_SHADERS} ${PIXEL_SHADERS})

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
	
	/W44324  # C4324 'struct_name' : structure was padded due to __declspec(align())
)

set(LIBS
    d3d11
    D3DCompiler
    dxguid

    DirectXMath
    DirectXTex
)

target_link_libraries(${TARGET_NAME} PRIVATE ${LIBS})

target_include_directories(
    ${TARGET_NAME}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/ext/DirectXMath/Inc/
    ${PROJECT_SOURCE_DIR}/ext/DirectXTex/DirectXTex/
)
