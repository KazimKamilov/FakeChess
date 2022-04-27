cmake_minimum_required(VERSION 3.0.0)
project(FakeChess LANGUAGES CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /Zi /Od /Ob0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Zi /Od /Ob0")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin/)

include_directories(source)
include_directories(deps/include)

file(GLOB_RECURSE game_source source/*.cpp source/*.hpp)

add_compile_definitions(_SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE SFML_STATIC)
link_directories(${CMAKE_CURRENT_LIST_DIR}/deps/lib/)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

add_executable(FakeChess ${game_source} FakeChess.rc)

target_link_libraries(FakeChess sfml-system-s sfml-window-s sfml-graphics-s sfml-audio-s freetype opengl32 winmm)
