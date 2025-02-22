# Directories
SRC_DIR = src
BUILD_DIR = out
PORT = 3333

# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11

all: server client check

check:
	@echo "Checking if necessary directories exist..."
	@mkdir -p $(BUILD_DIR)
	@echo "Checking if source files exist..."
	@if [ ! -f ./$(SRC_DIR)/server.cpp ]; then echo "Error: server.cpp not found!"; exit 1; fi
	@if [ ! -f ./$(SRC_DIR)/client.cpp ]; then echo "Error: client.cpp not found!"; exit 1; fi
	@echo "All checks passed!"


server:
	@echo "Compiling server..."
	$(CXX) $(CXXFLAGS) ./$(SRC_DIR)/server.cpp -o ./$(BUILD_DIR)/server
	@if [ $$? -ne 0 ]; then echo "Error: Compilation failed for server!"; exit 1; fi
	@echo "Server compiled successfully.\n\n"

client:
	@echo "Compiling client..."
	$(CXX) $(CXXFLAGS) ./$(SRC_DIR)/client.cpp -o ./$(BUILD_DIR)/client
	@if [ $$? -ne 0 ]; then echo "Error: Compilation failed for client!"; exit 1; fi
	@echo "Client compiled successfully.\n\n"

clean:
	rm -rf $(BUILD_DIR)/*

run-client: 
	./$(BUILD_DIR)/client --port $(PORT)

run-server: 
	./$(BUILD_DIR)/server --port $(PORT)
