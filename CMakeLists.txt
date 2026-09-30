cmake_minimum_required(VERSION 3.15)

project(MyProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# List all source files
set(SOURCES
    main.cpp
    player/player.cpp
    game/game.cpp
    card/card.cpp
    games/coinflip.cpp
    games/guessthecard.cpp
    games/blackjack.cpp
)

# Create executable
add_executable(main ${SOURCES})

# Include current directory (equivalent to -I.)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR})

# cd build
# build % cmake ..
# cmake --build build
