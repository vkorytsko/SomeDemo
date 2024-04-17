cmake_minimum_required(VERSION 3.12.0)

set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin/$<CONFIG>/)
set(INTERNALS_BIN_DIR ${PROJECT_SOURCE_DIR}/bin/$<CONFIG>/src/)
set(INTERNALS_FOLDER internal)


add_subdirectory(core)
add_subdirectory(engine)
add_subdirectory(render)
add_subdirectory(shaders)

set(TARGET_NAME ${PROJECT_NAME})

set(SOURCES
	main.cpp
)
set(HEADERS
)
source_group("" FILES ${SOURCES} ${HEADERS})

add_executable(${TARGET_NAME}
	${SOURCES} ${HEADERS}
)

set_target_properties(
	${TARGET_NAME}
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
	LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
	VS_DEBUGGER_WORKING_DIRECTORY ${BIN_DIR}
)

target_compile_options(${TARGET_NAME}
	PRIVATE
	/W4
	/WX
	/MP
)

target_link_libraries(${TARGET_NAME}
	PRIVATE
	# internal
	core
	engine
)
