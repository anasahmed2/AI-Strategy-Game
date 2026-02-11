cmake_minimum_required(VERSION 3.16)
project(AIStrategyGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Add Engine library
add_subdirectory(Engine)

# Add Game executable
add_subdirectory(Game)
