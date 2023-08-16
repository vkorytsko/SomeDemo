cmake_minimum_required(VERSION 3.12.0)

set(TARGET_NAME ${PROJECT_NAME})

set(SHADERS_SOURCE_DIR
    ${PROJECT_SOURCE_DIR}/res/shaders/
)
set(ENGINE_SOURCE_DIR
    ${PROJECT_SOURCE_DIR}/src/engine/
)
set(RENDER_SOURCE_DIR
    ${PROJECT_SOURCE_DIR}/src/render/
)

set(SOURCES
    main.cpp
)
set(HEADERS
)
source_group(" " FILES ${SOURCES} ${HEADERS})

set(ENGINE_SOURCES
    ${ENGINE_SOURCE_DIR}application.cpp
    ${ENGINE_SOURCE_DIR}camera.cpp
    ${ENGINE_SOURCE_DIR}debug_world.cpp
    ${ENGINE_SOURCE_DIR}exceptions.cpp
    ${ENGINE_SOURCE_DIR}space.cpp
    ${ENGINE_SOURCE_DIR}timer.cpp
    ${ENGINE_SOURCE_DIR}window.cpp
    ${ENGINE_SOURCE_DIR}world.cpp
)
set(ENGINE_HEADERS
    ${ENGINE_SOURCE_DIR}application.hpp
    ${ENGINE_SOURCE_DIR}camera.hpp
    ${ENGINE_SOURCE_DIR}debug_world.hpp
    ${ENGINE_SOURCE_DIR}exceptions.hpp
    ${ENGINE_SOURCE_DIR}space.hpp
    ${ENGINE_SOURCE_DIR}timer.hpp
    ${ENGINE_SOURCE_DIR}utils.hpp
    ${ENGINE_SOURCE_DIR}window.hpp
    ${ENGINE_SOURCE_DIR}world.hpp
)
source_group("engine" FILES ${ENGINE_SOURCES} ${ENGINE_HEADERS})

set(RENDER_SOURCES
    ${RENDER_SOURCE_DIR}blender.cpp
    ${RENDER_SOURCE_DIR}buffer.cpp
    ${RENDER_SOURCE_DIR}debug_layer.cpp
    ${RENDER_SOURCE_DIR}index_buffer.cpp
    ${RENDER_SOURCE_DIR}input_layout.cpp
    ${RENDER_SOURCE_DIR}pixel_shader.cpp
    ${RENDER_SOURCE_DIR}rasterizer.cpp
    ${RENDER_SOURCE_DIR}renderer.cpp
    ${RENDER_SOURCE_DIR}sampler.cpp
    ${RENDER_SOURCE_DIR}texture.cpp
    ${RENDER_SOURCE_DIR}vertex_buffer.cpp
    ${RENDER_SOURCE_DIR}vertex_shader.cpp
)
set(RENDER_HEADERS
    ${RENDER_SOURCE_DIR}blender.hpp
    ${RENDER_SOURCE_DIR}buffer.hpp
    ${RENDER_SOURCE_DIR}constant_buffer.hpp
    ${RENDER_SOURCE_DIR}debug_layer.hpp
    ${RENDER_SOURCE_DIR}index_buffer.hpp
    ${RENDER_SOURCE_DIR}input_layout.hpp
    ${RENDER_SOURCE_DIR}pixel_shader.hpp
    ${RENDER_SOURCE_DIR}rasterizer.hpp
    ${RENDER_SOURCE_DIR}renderer.hpp
    ${RENDER_SOURCE_DIR}sampler.hpp
    ${RENDER_SOURCE_DIR}texture.hpp
    ${RENDER_SOURCE_DIR}vertex_buffer.hpp
    ${RENDER_SOURCE_DIR}vertex_shader.hpp
)
source_group("render" FILES ${RENDER_SOURCES} ${RENDER_HEADERS})

set(VERTEX_SHADERS
    ${SHADERS_SOURCE_DIR}blinn_phong.vs.hlsl
    ${SHADERS_SOURCE_DIR}color.vs.hlsl
    ${SHADERS_SOURCE_DIR}light.vs.hlsl
    ${SHADERS_SOURCE_DIR}phong.vs.hlsl
    ${SHADERS_SOURCE_DIR}shadow_map.vs.hlsl
    ${SHADERS_SOURCE_DIR}shadow_map_debug.vs.hlsl
    ${SHADERS_SOURCE_DIR}texture.vs.hlsl
)
set(PIXEL_SHADERS
    ${SHADERS_SOURCE_DIR}blinn_phong.ps.hlsl
    ${SHADERS_SOURCE_DIR}color.ps.hlsl
    ${SHADERS_SOURCE_DIR}light.ps.hlsl
    ${SHADERS_SOURCE_DIR}phong.ps.hlsl
    ${SHADERS_SOURCE_DIR}shadow_map_debug.ps.hlsl
    ${SHADERS_SOURCE_DIR}texture.ps.hlsl
)
source_group("shaders" FILES ${VERTEX_SHADERS} ${PIXEL_SHADERS})

set_property(SOURCE ${VERTEX_SHADERS} PROPERTY VS_SHADER_TYPE Vertex)
set_property(SOURCE ${VERTEX_SHADERS} PROPERTY VS_SHADER_MODEL 5.0)
set_property(SOURCE ${PIXEL_SHADERS} PROPERTY VS_SHADER_TYPE Pixel)
set_property(SOURCE ${PIXEL_SHADERS} PROPERTY VS_SHADER_MODEL 5.0)

add_executable(${TARGET_NAME}
    ${SOURCES} ${HEADERS}
    ${ENGINE_SOURCES} ${ENGINE_HEADERS}
    ${RENDER_SOURCES} ${RENDER_HEADERS}
    ${VERTEX_SHADERS} ${PIXEL_SHADERS}
)

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
	imgui
)

target_link_libraries(${TARGET_NAME} PRIVATE ${LIBS})

target_include_directories(
    ${TARGET_NAME}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/ext/DirectXMath/Inc/
    ${PROJECT_SOURCE_DIR}/ext/DirectXTex/DirectXTex/
    ${PROJECT_SOURCE_DIR}/ext/tinygltf/
    ${PROJECT_SOURCE_DIR}/ext/imgui/

    ${ENGINE_SOURCE_DIR}
    ${RENDER_SOURCE_DIR}
)
