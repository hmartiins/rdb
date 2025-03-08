cmake_minimum_required(VERSION 3.15)
project(rdb-socket)

find_package(nlohmann_json CONFIG REQUIRED)

# Directories
set(SRC_DIR src)
set(BUILD_DIR out)
set(PORT 3332)

# Compiler
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# Executables
add_executable(server ${SRC_DIR}/server.cpp)
add_executable(client ${SRC_DIR}/client.cpp)

# Output directory
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

# Linking libraries
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)

# Execution targets
add_custom_target(run-client
    COMMAND ${BUILD_DIR}/client --port ${PORT}
    DEPENDS client)

add_custom_target(run-server
    COMMAND ${BUILD_DIR}/server --port ${PORT}
    DEPENDS server)

add_custom_target(run-file
    COMMAND ${BUILD_DIR}/file
    DEPENDS file)
