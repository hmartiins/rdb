cmake_minimum_required(VERSION 3.15)
project(rdb-socket)

find_package(nlohmann_json CONFIG REQUIRED)

# Directories
set(SRC_DIR src)
set(BUILD_DIR out)
set(PORT 3332)

# Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# Executables
add_executable(server ${SRC_DIR}/server.cpp)
add_executable(client ${SRC_DIR}/client.cpp)

# Output directory
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

# Linking libraries
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(client PRIVATE nlohmann_json::nlohmann_json)

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

# --- Tests ---
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
    tests/test_random_id.cpp
    tests/test_split.cpp
    tests/test_socket_utils.cpp
    tests/test_thread_pool.cpp
    tests/test_room_manager.cpp
)

target_include_directories(unit_tests PRIVATE ${SRC_DIR})
target_link_libraries(unit_tests PRIVATE
    GTest::gtest_main
    nlohmann_json::nlohmann_json
)

set_target_properties(unit_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

include(GoogleTest)
gtest_discover_tests(unit_tests)

add_custom_target(run-tests
    COMMAND ${BUILD_DIR}/unit_tests
    DEPENDS unit_tests)
