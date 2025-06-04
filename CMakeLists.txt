cmake_minimum_required(VERSION 3.15)
project(rdb-socket)

find_package(nlohmann_json CONFIG REQUIRED)

# Directories
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(BUILD_DIR ${PROJECT_SOURCE_DIR}/out)
set(PORT 3332)

# Header directories
include_directories(
    ${SRC_DIR}/utils        # Contém logger.hpp, split.hpp, etc.
    ${SRC_DIR}/json         # Contém create-file.hpp, read-file.hpp, update-file.hpp, delete-file.hpp
    ${SRC_DIR}/socket_utils # Caso tenha abstrações de socket em src/socket_utils.hpp
)

# Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Executables
add_executable(server ${SRC_DIR}/server.cpp)
add_executable(client ${SRC_DIR}/client.cpp)

# Output directory
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})      

# Linking libraries
# - nlohmann_json para manipulação de JSON
# - pthread para uso de std::thread no servidor (e cliente, se necessário)
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json pthread)
target_link_libraries(client PRIVATE pthread)

# Execution targets
add_custom_target(run-client
    COMMAND ${BUILD_DIR}/client --port ${PORT}
    DEPENDS client)

add_custom_target(run-server
    COMMAND ${BUILD_DIR}/server --port ${PORT}
    DEPENDS server)

#add_custom_target(run-file
#    COMMAND ${BUILD_DIR}/file
#    DEPENDS file)
