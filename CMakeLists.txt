cmake_minimum_required(VERSION 3.10)
project(wallet_system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find nlohmann_json package
find_package(nlohmann_json REQUIRED)

# Add executable
add_executable(wallet_system
    main.cpp
)

# Include directories
target_include_directories(wallet_system PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(wallet_system PRIVATE
    nlohmann_json::nlohmann_json
) 