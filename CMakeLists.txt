cmake_minimum_required(VERSION 3.16)

project(ParticleSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(ParticleSimulator src/main.cpp)

# Convert font file to a C header during build/configure
file(READ "${CMAKE_SOURCE_DIR}/fonts/DejaVuSans.ttf" FONT_HEX HEX)
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " FONT_C_ARRAY ${FONT_HEX})
file(WRITE "${CMAKE_BINARY_DIR}/autogen/DejaVuSans_font.h" "const unsigned char embedded_font[] = { ${FONT_C_ARRAY} };\nconst unsigned int embedded_font_len = sizeof(embedded_font);")

target_include_directories(ParticleSimulator PRIVATE "${CMAKE_BINARY_DIR}/autogen")

target_link_libraries(ParticleSimulator PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
)
