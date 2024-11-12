cmake_minimum_required(VERSION 3.5)
project(ArcadeApplication C)

set(CMAKE_C_STANDARD 11)

# Relatieve paden voor SDL2, SDL2_image, SDL2_ttf en SDL2_mixer
set(LIBRARY_PATH "${CMAKE_SOURCE_DIR}/libs")
set(SDL2_INCLUDE_DIR "${LIBRARY_PATH}/SDL2-2.30.9/x86_64-w64-mingw32/include/SDL2")
set(SDL2_IMAGE_INCLUDE_DIR "${LIBRARY_PATH}/SDL2_image-2.8.2/x86_64-w64-mingw32/include/SDL2")
set(SDL2_TTF_INCLUDE_DIR "${LIBRARY_PATH}/SDL2_ttf-2.22.0/x86_64-w64-mingw32/include/SDL2")
set(SDL2_MIXER_INCLUDE_DIR "${LIBRARY_PATH}/SDL2_mixer-2.8.0/x86_64-w64-mingw32/include/SDL2")

add_definitions(-DSDL_MAIN_HANDLED)

set(SDL2_LIB_DIR "${LIBRARY_PATH}/SDL2-2.30.9/x86_64-w64-mingw32/lib")
set(SDL2_IMAGE_LIB_DIR "${LIBRARY_PATH}/SDL2_image-2.8.2/x86_64-w64-mingw32/lib")
set(SDL2_TTF_LIB_DIR "${LIBRARY_PATH}/SDL2_ttf-2.22.0/x86_64-w64-mingw32/lib")
set(SDL2_MIXER_LIB_DIR "${LIBRARY_PATH}/SDL2_mixer-2.8.0/x86_64-w64-mingw32/lib")

# Voeg de include directories toe
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR})

# Definieer de executable
add_executable(arcade_application main.c)

# Link de SDL2 bibliotheken
target_link_libraries(arcade_application
        "${SDL2_LIB_DIR}/libSDL2.a"
        "${SDL2_LIB_DIR}/libSDL2main.a"
        "${SDL2_IMAGE_LIB_DIR}/libSDL2_image.a"
        "${SDL2_TTF_LIB_DIR}/libSDL2_ttf.a"
        "${SDL2_MIXER_LIB_DIR}/libSDL2_mixer.a"
        -mconsole
        -lrpcrt4 -lsetupapi -limm32 -lversion -lwinmm -ldxguid -lkernel32
        -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid
        -lcomdlg32 -ladvapi32
)

# Kopieer de assets naar de build map (fonts, img en music mappen)
add_custom_command(TARGET arcade_application POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/fonts" $<TARGET_FILE_DIR:arcade_application>/fonts
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/img" $<TARGET_FILE_DIR:arcade_application>/img
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/music" $<TARGET_FILE_DIR:arcade_application>/music
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/img" $<TARGET_FILE_DIR:arcade_application>/img
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/img/CoverArtNotFound.png" $<TARGET_FILE_DIR:arcade_application>/img
)
