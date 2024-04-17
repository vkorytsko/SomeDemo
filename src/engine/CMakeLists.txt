cmake_minimum_required(VERSION 3.12.0)

set(TARGET_NAME engine)

set(SOURCES
	application.cpp
	camera.cpp
	debug_world.cpp
	render_system.cpp
	space.cpp
	timer.cpp
	window.cpp
	world.cpp

	node_properties_panel.cpp
	scene_browser_panel.cpp
	space_settings_panel.cpp
	viewport_panel.cpp
)
set(HEADERS
	application.hpp
	camera.hpp
	debug_world.hpp
	render_system.hpp
	space.hpp
	timer.hpp
	window.hpp
	world.hpp

	node_properties_panel.hpp
	scene_browser_panel.hpp
	space_settings_panel.hpp
	viewport_panel.hpp
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
	# external
	DirectXMath
	imgui
	tinygltf
	# internal
	core
	render
)

target_include_directories(
	${TARGET_NAME}
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_dependencies(${TARGET_NAME} shaders)
