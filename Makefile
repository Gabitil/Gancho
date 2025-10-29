CXX = g++
CXXFLAGS = -Iinclude -g -Wall -std=c++17
LDFLAGS = -Llib

OBJ_DIR = obj
BIN_DIR = bin

# ---- adicione aqui todas as pastas onde há .cpp ----
SRC_DIRS := . core ui util gfx audio
# se você ainda tiver coisas em src/xyz, pode incluir:
# SRC_DIRS += src/core src/ui src/util src/gfx src/audio

# pega todos os .cpp do projeto (exceta libs/bin/obj)
SOURCES := $(shell find . -name '*.cpp' \
                     -not -path './lib/*' \
                     -not -path './bin/*' \
                     -not -path './obj/*')
# mapeia para objetos espelhando a subpasta
OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# --- Detecção de SO e libs corretas ---
ifeq ($(OS),Windows_NT)
    EXEC_NAME = jogo.exe
    SHELL := cmd.exe
    SHELLFLAGS := /C
    MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
    RM = if exist $(subst /,\,$(1)) rmdir /S /Q $(subst /,\,$(1))
    LIBS = -lfreeglut -lglew32 -lglu32 -lopengl32
else
    EXEC_NAME = jogo
    MKDIR = mkdir -p $(1)
    RM = rm -rf $(1)
    LIBS = -lglut -lGLEW -lGL -lGLU
endif

EXEC = $(BIN_DIR)/$(EXEC_NAME)

all: $(EXEC)
compilar: all
run: executar

$(EXEC): $(OBJECTS)
	@echo [LINK] Criando executável $@
	$(call MKDIR,$(BIN_DIR))
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)
	@echo [OK] Compilação concluída.

# regra de compilação que cria a subpasta em obj/ se precisar
$(OBJ_DIR)/%.o: %.cpp
	@echo [C++] Compilando $<
	$(call MKDIR,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

executar: all
	@echo --- Executando o jogo ---
ifeq ($(OS),Windows_NT)
	$(subst /,\,$(EXEC))
else
	./$(EXEC)
endif
	@echo --- Jogo finalizado ---

limpar:
	@echo Limpando arquivos...
	$(call RM,$(OBJ_DIR))
	$(call RM,$(BIN_DIR))
	@echo Limpeza concluída.

.PHONY: all compilar executar limpar run
