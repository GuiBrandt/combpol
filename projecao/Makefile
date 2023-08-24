CXX=g++
CXXFLAGS=-Wall -march=native

SOURCES=main.cpp
HEADERS=include/**/*.hpp

build/debug/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p build/debug
	$(CXX) -g -Og -Iinclude $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@
	
build/release/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p build/release
	$(CXX) -O3 -DNOEXCEPT -DNDEBUG -Iinclude $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@

.PHONY: build build-release build-debug
build: build-release
build-release: build/release/combpol-projecao
build-debug: build/debug/combpol-projecao

.PHONY: run run-debug
run: build
	@build/release/combpol-projecao

run-debug: build-debug
	@build/debug/combpol-projecao

.PHONY: clean
clean:
	@rm -rf build

.PHONY: memcheck-debug memcheck-release
memcheck-debug: build-debug
	@valgrind build/debug/combpol-projecao

memcheck-release: build-release
	@valgrind build/release/combpol-projecao
