
BUILD_DIR := build

all: ${BUILD_DIR}

${BUILD_DIR}: cmake
	$(MAKE) -C ${BUILD_DIR} --no-print-directory

cmake: ${BUILD_DIR}/Makefile

${BUILD_DIR}/Makefile:
	cmake \
		-G "Unix Makefiles" \
		-B${BUILD_DIR} \
		-DCMAKE_TOOLCHAIN_FILE=avr-gcc.cmake

clean:
	@rm -rf ${BUILD_DIR}
	@echo "Delete ${BUILD_DIR} directory"


.PHONY: all ${BUILD_DIR} cmake clean
