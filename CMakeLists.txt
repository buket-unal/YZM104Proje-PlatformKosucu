cmake_minimum_required(VERSION 3.11)
project(MaratonKosu VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MONO'YU TAMAMEN DEVRE DIŞI BIRAKAN KESİN ÇÖZÜM
set(CMAKE_IGNORE_PATH "/Library/Frameworks/Mono.framework")
set(CMAKE_SYSTEM_IGNORE_PATH "/Library/Frameworks/Mono.framework")
set(CMAKE_FRAMEWORK_PATH "")

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

# SFML'e dışarıdaki hiçbir kütüphaneye bakmamasını söylüyoruz
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

add_executable(MaratonKosu src/main.cpp src/Player.cpp)
target_link_libraries(MaratonKosu sfml-graphics sfml-window sfml-system)