# Vcpkg path
VCPKG_PATH = /Users/henriquemartins/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
BUILD_DIR = build

all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_PATH) ..
	cd $(BUILD_DIR) && make
