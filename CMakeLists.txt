cmake_minimum_required(VERSION 3.16)
project(Calculator LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "This calculator uses Windows DLLs.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Приложение и DLL используют одну динамическую библиотеку среды C++.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

add_executable(calc
    src/main.cpp
    src/Application.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/PluginManager.cpp
    src/SharedLibrary.cpp
)
set_target_properties(calc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)

foreach(name IN ITEMS sin ln sqrt)
    add_library(${name}_plugin MODULE plugins/${name}.cpp)
    set_target_properties(${name}_plugin PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/plugins"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/plugins"
    )
endforeach()

foreach(target IN ITEMS calc sin_plugin ln_plugin sqrt_plugin)
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_compile_definitions(${target} PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8 /EHsc)
    endif()
endforeach()

add_dependencies(calc sin_plugin ln_plugin sqrt_plugin)
