
CMAKE ?= /usr/bin/cmake
CTEST ?= /usr/bin/ctest
BUILD_DIR ?= build
BUILD_TOOL ?= Ninja
CXX_STARDARD ?= 20
JOBS ?= 8

.PHONY: install all exec-test test exec-benchmark benchmark init check-format format clang-tidy lint

$(BUILD_DIR)/rules.ninja:
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && \
	$(CMAKE) \
   	-DCMAKE_CXX_STANDARD=$(CXX_STANDARD) \
    -G$(BUILD_TOOL) \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	-Wdev \
	-Wall \
	..

init: $(BUILD_DIR)/rules.ninja
	ln -sf $(BUILD_DIR)/compile_commands.json .

clean:
	@rm -Rf $(BUILD_DIR)

all:
	$(CMAKE) --build $(BUILD_DIR) --target all -- -j$(JOBS)

$(BUILD_DIR)/test/unit/tests.hh.tsk: init
	$(CMAKE) --build $(BUILD_DIR) --target tests.hh.tsk -- -j$(JOBS)

build-test: $(BUILD_DIR)/test/unit/tests.hh.tsk

exec-test:
	@cd $(BUILD_DIR) && \
	GTEST_COLOR=1 $(CTEST) -VV

test: build-test exec-test

$(BUILD_DIR)/test/benchmark/cxxtendbenchmark.tsk: init
	$(CMAKE) --build $(BUILD_DIR) --target cxxtendbenchmark.tsk -- -j$(JOBS)

build-benchmark: $(BUILD_DIR)/test/unit/tests.hh.tsk

exec-benchmark: build-benchmark
	$(CMAKE) --build $(BUILD_DIR) --target benchmark -- -j$(JOBS)

check-format: init
	$(CMAKE) --build $(BUILD_DIR) --target check-format

format: init
	$(CMAKE) --build $(BUILD_DIR) --target format

benchmark: build-benchmark exec-benchmark

clang-tidy: init
	$(CMAKE) --build $(BUILD_DIR) --target clang-tidy

lint: clang-tidy
