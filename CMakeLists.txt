cmake_minimum_required(VERSION 3.29)
project(rest_api_cpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
add_executable(rest_api_cpp
        src/main.cpp
        src/sqlite3.c
)

# Link the Winsock2 library
target_link_libraries(rest_api_cpp Ws2_32)