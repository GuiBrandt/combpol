SHELL = /bin/sh
.SUFFIXES:

#==============================================================================
# Configurações de compilação
#==============================================================================

#CXX=g++
CXXFLAGS=-std=c++14 -Wall -Wno-format-security -march=native

# Flags usadas na compilação para depuração
DBG_CXXFLAGS=-g -Og

# Flags usadas na compilação final
REL_CXXFLAGS=-O3 -DNOEXCEPT -DNDEBUG

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
# Targets de compilação e execução
#==============================================================================

.PHONY: build build-release build-debug
build: build-release
build-release: $(BINDIR)/release/combpol-projecao
build-debug: $(BINDIR)/debug/combpol-projecao

.PHONY: run run-debug
run: build
	@$(BINDIR)/release/combpol-projecao

run-debug: build-debug
	@$(BINDIR)/debug/combpol-projecao

#==============================================================================
# Targets de teste
#==============================================================================

.PHONY: memcheck-debug memcheck-release
memcheck-debug: build-debug
	@valgrind $(BINDIR)/debug/combpol-projecao

memcheck-release: build-release
	@valgrind $(BINDIR)/release/combpol-projecao

#==============================================================================
# Targets binários
#==============================================================================

$(BINDIR)/debug/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p build/debug
	$(CXX) $(DBG_CXXFLAGS) -I$(INCDIR) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@

$(BINDIR)/release/combpol-projecao: $(SOURCES) $(HEADERS)
	@mkdir -p $(BINDIR)/release
	$(CXX) $(REL_CXXFLAGS) -I$(INCDIR) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@
