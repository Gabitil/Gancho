
# ============================================
# Makefile Multiplataforma (Windows / Linux)
# ============================================

CXX = g++
CXXFLAGS = -Iinclude -g -Wall -std=c++17
LDFLAGS = -Llib

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# --- Detecção de SO e libs corretas ---
ifeq ($(OS),Windows_NT)
    EXEC_NAME = jogo.exe
    SHELL := cmd.exe
    SHELLFLAGS := /C
    MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
    RM = if exist $(subst /,\,$(1)) rmdir /S /Q $(subst /,\,$(1))
    LIBS = -lfreeglut -lglew32 -lopengl32 -lglu32
else
    EXEC_NAME = jogo
    MKDIR = mkdir -p $(1)
    RM = rm -rf $(1)
    # Em Linux (Ubuntu/Mint):
    # FreeGLUT geralmente é -lglut (às vezes -lfreeglut)
    LIBS = -lglut -lGLEW -lGL -lGLU
    # Se precisar, pode acrescentar: -lX11 -lXrandr -lpthread
endif

EXEC = $(BIN_DIR)/$(EXEC_NAME)

# --- Regras principais ---
all: $(EXEC)
compilar: all
run: executar   # alias para 'make run'

$(EXEC): $(OBJECTS)
	@echo [LINK] Criando executável $@
	$(call MKDIR,$(BIN_DIR))
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)
	@echo [OK] Compilação concluída.

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo [C++] Compilando $<
	$(call MKDIR,$(OBJ_DIR))
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

.PHONY: all compilar executar limpar run
