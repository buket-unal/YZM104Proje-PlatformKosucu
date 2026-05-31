cmake_minimum_required(VERSION 3.11)
project(PlatformKosucu VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

add_executable(PlatformKosucu 
        src/main.cpp 
        src/Player.cpp 
        src/Player.hpp
        src/Platform.cpp
        src/Platform.hpp
        src/Enemy.cpp
        src/Enemy.hpp
        src/Coin.cpp
        src/Coin.hpp
        src/FlyingEnemy.cpp
        src/FlyingEnemy.hpp
        src/GameFunctions.cpp
        src/GameFunctions.hpp
        )

target_link_libraries(PlatformKosucu sfml-graphics sfml-window sfml-system sfml-audio)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")