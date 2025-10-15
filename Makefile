# ============================================
# Makefile Multiplataforma (Windows / Linux)
# ============================================

# --- Compilador e Flags ---
CXX = g++
CXXFLAGS = -Iinclude -g -Wall -std=c++17
LDFLAGS = -Llib
LIBS = -lfreeglut -lglew32 -lopengl32 -lglu32

# --- Diretórios ---
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# --- Arquivos fonte e objeto ---
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# --- Detecção de sistema operacional ---
ifeq ($(OS),Windows_NT)
	EXEC_NAME = jogo.exe
	SHELL := cmd.exe
	SHELLFLAGS := /C
	MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
	RM = if exist $(subst /,\,$(1)) rmdir /S /Q $(subst /,\,$(1))
else
	EXEC_NAME = jogo
	MKDIR = mkdir -p $(1)
	RM = rm -rf $(1)
endif

EXEC = $(BIN_DIR)/$(EXEC_NAME)

# --- Regras principais ---
all: $(EXEC)

compilar: all

# Cria e linka o executável
$(EXEC): $(OBJECTS)
	@echo [LINK] Criando executável $@
	$(call MKDIR,$(BIN_DIR))
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)
	@echo [OK] Compilação concluída.

# Compila cada .cpp em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo [C++] Compilando $<
	$(call MKDIR,$(OBJ_DIR))
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Executa o jogo
executar: all
	@echo --- Executando o jogo ---
ifeq ($(OS),Windows_NT)
	$(subst /,\,$(EXEC))
else
	./$(EXEC)
endif
	@echo --- Jogo finalizado ---

# Limpa arquivos gerados
limpar:
	@echo Limpando arquivos...
	$(call RM,$(OBJ_DIR))
	$(call RM,$(BIN_DIR))
	@echo Limpesa concluída.

.PHONY: all compilar executar limpar
