cmake_minimum_required(VERSION 3.12.0)

set(TARGET_NAME render)

set(SOURCES
	blender.cpp
	buffer.cpp
	debug_layer.cpp
	frame_buffer.cpp
	index_buffer.cpp
	input_layout.cpp
	pixel_shader.cpp
	rasterizer.cpp
	renderer.cpp
	sampler.cpp
	texture.cpp
	vertex_buffer.cpp
	vertex_shader.cpp
)
set(HEADERS
	blender.hpp
	buffer.hpp
	constant_buffer.hpp
	debug_layer.hpp
	frame_buffer.hpp
	index_buffer.hpp
	input_layout.hpp
	pixel_shader.hpp
	rasterizer.hpp
	renderer.hpp
	sampler.hpp
	texture.hpp
	vertex_buffer.hpp
	vertex_shader.hpp
)
source_group("" FILES ${SOURCES} ${HEADERS})

add_library(
	${TARGET_NAME}
	STATIC
	${SOURCES} ${HEADERS}
)

set_target_properties(
	${TARGET_NAME}
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	FOLDER ${INTERNALS_FOLDER}
	ARCHIVE_OUTPUT_DIRECTORY ${INTERNALS_BIN_DIR}
	LIBRARY_OUTPUT_DIRECTORY ${INTERNALS_BIN_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${INTERNALS_BIN_DIR}
)

target_compile_options(
	${TARGET_NAME}
	PRIVATE
	/W4
	/WX
	/MP
)

target_link_libraries(
	${TARGET_NAME}
	PRIVATE
	# system
	d3d11
	D3DCompiler
	dxguid
	# external
	DirectXMath
	DirectXTex
	# internal
	core
)

target_include_directories(
	${TARGET_NAME}
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
