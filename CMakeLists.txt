cmake_minimum_required(VERSION 3.11)
project(MaratonKosu VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(FreeType_INCLUDE_DIR_ft2build /opt/homebrew/include/freetype2)
set(FreeType_LIBRARY /opt/homebrew/lib/libfreetype.dylib)

# MONO'YU TAMAMEN DEVRE DIŞI BIRAKAN KESİN ÇÖZÜM
set(CMAKE_IGNORE_PATH "/Library/Frameworks/Mono.framework")
set(CMAKE_SYSTEM_IGNORE_PATH "/Library/Frameworks/Mono.framework")
set(CMAKE_FRAMEWORK_PATH "")
# Mevcut satırlarının altına bunu ekle
set(CMAKE_FIND_FRAMEWORK NEVER)
# OpenGL ve SDK yollarını zorla gösteriyoruz
set(CMAKE_OSX_SYSROOT /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk)
set(OPENGL_gl_LIBRARY /System/Library/Frameworks/OpenGL.framework)
set(OPENGL_INCLUDE_DIR /System/Library/Frameworks/OpenGL.framework/Headers)

include(FetchContent)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

# SFML'e dışarıdaki hiçbir kütüphaneye bakmamasını söylüyoruz
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

add_executable(MaratonKosu src/main.cpp src/Player.cpp src/Platform.cpp)
target_link_libraries(MaratonKosu sfml-graphics sfml-window sfml-system)