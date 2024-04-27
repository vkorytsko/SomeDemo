cmake_minimum_required(VERSION 3.12.0)

set(TARGET_NAME shaders)

set(SHADERS_BIN_DIR ${INTERNALS_BIN_DIR}/shaders/)

set(VERTEX_SHADERS
	pbr.vs.hlsl
	brdf.vs.hlsl
	background.vs.hlsl
	prefilter.vs.hlsl
	cubemap.vs.hlsl
	blinn_phong.vs.hlsl
	color.vs.hlsl
	light.vs.hlsl
	phong.vs.hlsl
	shadow_map.vs.hlsl
	shadow_map_debug.vs.hlsl
	texture.vs.hlsl
)
set(PIXEL_SHADERS
	pbr.ps.hlsl
	brdf.ps.hlsl
	background.ps.hlsl
	prefilter.ps.hlsl
	equirectangular_to_cubemap.ps.hlsl
	irradiance_convolution.ps.hlsl
	blinn_phong.ps.hlsl
	color.ps.hlsl
	light.ps.hlsl
	phong.ps.hlsl
	shadow_map_debug.ps.hlsl
	texture.ps.hlsl
)
source_group("" FILES ${VERTEX_SHADERS} ${PIXEL_SHADERS})

set_property(SOURCE ${VERTEX_SHADERS} PROPERTY VS_SHADER_TYPE Vertex)
set_property(SOURCE ${VERTEX_SHADERS} PROPERTY VS_SHADER_MODEL 5.0)
set_property(SOURCE ${PIXEL_SHADERS} PROPERTY VS_SHADER_TYPE Pixel)
set_property(SOURCE ${PIXEL_SHADERS} PROPERTY VS_SHADER_MODEL 5.0)

add_library(
	${TARGET_NAME}
	STATIC
	${VERTEX_SHADERS} ${PIXEL_SHADERS}
)

set_target_properties(${TARGET_NAME}
	PROPERTIES
	LINKER_LANGUAGE HLSL
	FOLDER ${INTERNALS_FOLDER}
	ARCHIVE_OUTPUT_DIRECTORY ${SHADERS_BIN_DIR}
	LIBRARY_OUTPUT_DIRECTORY ${SHADERS_BIN_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${SHADERS_BIN_DIR}
)
