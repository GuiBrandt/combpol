CXX=g++
CXXFLAGS=-Wall -march=native

.PHONY: clean build build-release build-debug run run-debug

SOURCES=main.cpp
HEADERS=linalg/*.hpp

build/debug/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p build/debug
	$(CXX) -Og $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@
	
build/release/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p build/release
	$(CXX) -O3 -DNOEXCEPT $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@

build-debug: build/debug/combpol-projecao
build-release: build/release/combpol-projecao
build: build-release

run: build
	@build/release/combpol-projecao

run-debug: build-debug
	@build/debug/combpol-projecao

clean:
	@rm -rf build
