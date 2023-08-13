cmake_minimum_required(VERSION 3.12.0)

add_subdirectory(DirectXMath)

set(BUILD_TOOLS OFF)
set(BUILD_SAMPLE OFF)
set(BUILD_DX11 OFF)
set(BUILD_DX12 OFF)
set(BC_USE_OPENMP OFF)
add_subdirectory(DirectXTex)

# set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
# set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
# add_subdirectory(tinygltf)
