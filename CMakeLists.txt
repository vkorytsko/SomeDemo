cmake_minimum_required(VERSION 3.12.0)

project(SomeDemo)

add_definitions(
	-DUNICODE
	-D_UNICODE
	
	-DWIN32_LEAN_AND_MEAN
	-DNOMINMAX
	
	-DSD_RES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res/"
	-DSD_CFG_DIR="${CMAKE_CURRENT_SOURCE_DIR}/cfg/"
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

add_subdirectory(ext)
add_subdirectory(src)

set_property(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	PROPERTY
	VS_STARTUP_PROJECT ${PROJECT_NAME}
)
