cmake_minimum_required(VERSION 3.29)
project(rest-api-cpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
add_executable(rest-api-cpp
        main.cpp
        database/database.cpp
        sqlite3.c
        handlers/user.cpp
        main.cpp
)

# Link the Winsock2 library
target_link_libraries(rest-api-cpp Ws2_32)