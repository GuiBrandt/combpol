SHELL = /bin/sh
.SUFFIXES:

#==============================================================================
# Configurações de compilação
#==============================================================================

#CXX=g++
CXXFLAGS=-std=c++14 -Wall -Wno-format-security

# Flags usadas na compilação para depuração
DBG_CXXFLAGS=-g -Og

# Flags usadas na compilação final
REL_CXXFLAGS=-O2 -DNOEXCEPT -DNDEBUG

#==============================================================================
# Variáveis de estrutura do projeto
#==============================================================================

# Diretórios
SRCDIR=./src
INCDIR=$(SRCDIR)/include
BINDIR=./build

# Arquivos de fonte
SOURCES=$(SRCDIR)/*.cpp
HEADERS=$(INCDIR)/**/*.hpp

#==============================================================================
# Targets gerais
#==============================================================================

.PHONY: all
all: build

.PHONY: clean
clean:
	@rm -rf $(BINDIR)

#==============================================================================
# Targets de compilação e execução (Docker)
#==============================================================================

.PHONY:
build-docker: Dockerfile
	docker build -t ghcr.io/guibrandt/combpol/combpol-projecao:gcc-8-ubuntu .

.PHONY:
run-docker: Dockerfile
	docker run -v ./examples:/examples ghcr.io/guibrandt/combpol/combpol-projecao:gcc-8-ubuntu

#==============================================================================
# Targets de compilação e execução
#==============================================================================

.PHONY: build build-release build-debug
build: build-release
build-release: $(BINDIR)/release/combpol-projecao
build-debug: $(BINDIR)/debug/combpol-projecao

.PHONY: run run-debug
run: build
	@$(BINDIR)/release/combpol-projecao $(wildcard ./examples/*.in)

run-debug: build-debug
	@$(BINDIR)/debug/combpol-projecao $(wildcard ./examples/*.in)

#==============================================================================
# Targets de teste
#==============================================================================

.PHONY: memcheck-debug memcheck-release
memcheck-debug: build-debug
	@valgrind $(BINDIR)/debug/combpol-projecao ./examples/2d.in

memcheck-release: build-release
	@valgrind $(BINDIR)/release/combpol-projecao ./examples/2d.in

#==============================================================================
# Targets binários
#==============================================================================

$(BINDIR)/debug/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p build/debug
	$(CXX) $(DBG_CXXFLAGS) -I$(INCDIR) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@

$(BINDIR)/release/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p $(BINDIR)/release
	$(CXX) $(REL_CXXFLAGS) -I$(INCDIR) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@
