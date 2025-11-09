cmake_minimum_required(VERSION 3.15)
project(RIPDANYA_ChessBot)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings and optimization
add_compile_options(-Wall -Wextra -O2)

# Add your source files
set(SOURCES
    main.cpp
    game/bitboard.cpp
    game/board.cpp
    game/movegen.cpp  
)

# Create executable target
add_executable(ChessBot ${SOURCES})

