CXX=g++
CXXFLAGS=-Wall -march=native

SOURCES_DIR=src
HEADERS_DIR=$(SOURCES_DIR)/include
BUILD_DIR=build

SOURCES=$(SOURCES_DIR)/main.cpp
HEADERS=$(HEADERS_DIR)/**/*.hpp

.PHONY: all
all: build

$(BUILD_DIR)/debug/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p build/debug
	$(CXX) -g -Og -I$(HEADERS_DIR) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@
	
$(BUILD_DIR)/release/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p $(BUILD_DIR)/release
	$(CXX) -O3 -DNOEXCEPT -DNDEBUG -I$(HEADERS_DIR) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@

.PHONY: build build-release build-debug
build: build-release
build-release: $(BUILD_DIR)/release/combpol-projecao
build-debug: $(BUILD_DIR)/debug/combpol-projecao

.PHONY: run run-debug
run: build
	@$(BUILD_DIR)/release/combpol-projecao

run-debug: build-debug
	@$(BUILD_DIR)/debug/combpol-projecao

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)

.PHONY: memcheck-debug memcheck-release
memcheck-debug: build-debug
	@valgrind $(BUILD_DIR)/debug/combpol-projecao

memcheck-release: build-release
	@valgrind $(BUILD_DIR)/release/combpol-projecao
