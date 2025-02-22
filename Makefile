# Directories
SRC_DIR = src
BUILD_DIR = out

# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11

all:
	make server
	make client

server:
	$(CXX) $(CXXFLAGS) ./$(SRC_DIR)/server.cpp -o ./$(BUILD_DIR)/server

client:
	$(CXX) $(CXXFLAGS) ./$(SRC_DIR)/client.cpp -o ./$(BUILD_DIR)/client

clean:
	rm -rf $(BUILD_DIR)/*
