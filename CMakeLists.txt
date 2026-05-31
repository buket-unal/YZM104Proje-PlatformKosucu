cmake_minimum_required(VERSION 3.14) 
project(PlatformKosucu VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_BUILD_AUDIO ON CACHE INTERNAL "Build SFML Audio module")

# --- SFML'i İnternetten Otomatik İndirme Bölümü ---
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1  
)
FetchContent_MakeAvailable(SFML)
# -------------------------------------------------

# Kaynak dosyalarım
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

# SFML hedeflerini projeme bağlıyorum
target_link_libraries(PlatformKosucu sfml-graphics sfml-window sfml-system sfml-audio)

# Görsel, ses gibi assets klasörünü derleme klasörüme kopyalar
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

if(WIN32)
    add_custom_command(TARGET PlatformKosucu POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:PlatformKosucu>
        $<TARGET_FILE_DIR:PlatformKosucu>
        COMMAND_EXPAND_LISTS
    )
endif()