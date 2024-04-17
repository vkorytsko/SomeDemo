# Some Demo
Small C++17 graphics demo using DirectX 11 backend.

## Getting started
#### TL;DR
```
git clone --recurse-submodules --depth 1 https://github.com/vkorytsko/SomeDemo && cd .\SomeDemo\ && .\build.cmd && .\SomeDemo.sln
```
```
git clone --recurse-submodules https://github.com/vkorytsko/SomeDemo
```

## Controls
**W, A, S, D, Q, E** - Camera\
**F** - Spot Light\
**Ctrl** - Cursor

## Requirements
- [DirectX 11](https://support.microsoft.com/en-us/topic/how-to-install-the-latest-version-of-directx-d1f5ffa5-dae2-246c-91b1-ee1e973ed8c2)
- [Visual Studio 2019](https://visualstudio.microsoft.com/ru/downloads/)
- [CMake 3.12+](https://cmake.org/install/)

## Dependencies
- [DirectXMath](https://github.com/microsoft/DirectXMath) - an all inline SIMD C++ linear algebra library for use in games and graphics apps.
- [DirectXTex](https://github.com/microsoft/DirectXTex) - a shared source library for reading and writing .DDS files, and performing various texture content processing operations including resizing, format conversion, mip-map generation, block compression for Direct3D runtime texture resources, and height-map to normal-map conversion.
- [tinygltf](https://github.com/syoyo/tinygltf) - a header only C++11 glTF 2.0 https://github.com/KhronosGroup/glTF library.
- [ImGui](https://github.com/ocornut/imgui) - Dear ImGui is a bloat-free graphical user interface library for C++.
