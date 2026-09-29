cmake_minimum_required(VERSION 3.16)

project(ParticleSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(ParticleSimulator src/main.cpp)

add_custom_command(TARGET ParticleSimulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/fonts"
    "${CMAKE_BINARY_DIR}/fonts"
)

target_link_libraries(ParticleSimulator PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
)
