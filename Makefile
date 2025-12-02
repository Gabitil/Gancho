# CXX = g++
# # Define onde o compilador deve procurar arquivos de cabeçalho (.h)
# # Adicionamos -Isrc/core, -Isrc/ui, e -Isrc/util para que os includes funcionem
# CXXFLAGS = -I. -Iinclude -Isrc/core -Isrc/ui -Isrc/util -g -Wall -std=c++17 -DGLEW_STATIC
# LDFLAGS = -Llib

# OBJ_DIR = obj
# BIN_DIR = bin

# # Define ONDE estão os arquivos .cpp
# # Adicione '.' se o main.cpp estiver na raiz, caso contrário, remova.
# SRC_DIRS := . src/core src/ui src/util

# # pega todos os .cpp dessas pastas
# SOURCES := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.cpp))
# # mapeia para objetos, espelhando a subpasta (core/game.cpp -> obj/core/game.o)
# OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# ifeq ($(OS),Windows_NT)
#     EXEC_NAME = jogo.exe
#     SHELL := cmd.exe
#     SHELLFLAGS := /C
#     MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
#     RM = if exist $(subst /,\,$(1)) rmdir /S /Q $(subst /,\,$(1))
#     LIBS = -lfreeglut -lglu32 -lopengl32 -lgdi32 -lwinmm
# else
#     EXEC_NAME = jogo
#     MKDIR = mkdir -p $(1)
#     RM = rm -rf $(1)
#     LIBS = -lglut -lGLEW -lGL -lGLU
# endif

# EXEC = $(BIN_DIR)/$(EXEC_NAME)

# all: $(EXEC)
# compilar: all
# run: executar

# $(EXEC): $(OBJECTS)
# 	@echo [LINK] Criando executável $@
# 	$(call MKDIR,$(BIN_DIR))
# 	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)
# 	@echo [OK] Compilação concluída.

# # cria subpastas em obj/ automaticamente
# $(OBJ_DIR)/%.o: %.cpp
# 	@echo [C++] Compilando $<
# 	$(call MKDIR,$(dir $@))
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# executar: all
# 	@echo --- Executando o jogo ---
# ifeq ($(OS),Windows_NT)
# 	$(subst /,\,$(EXEC))
# else
# 	./$(EXEC)
# endif
# 	@echo --- Jogo finalizado ---

# limpar:
# 	$(call RM,$(OBJ_DIR))
# 	$(call RM,$(BIN_DIR))
# 	@echo Limpeza concluída.

# .PHONY: all compilar executar limpar run

CXX = g++
# Define onde o compilador deve procurar arquivos de cabeçalho (.h)
# Adicionamos -Isrc/core, -Isrc/ui, e -Isrc/util para que os includes funcionem
CXXFLAGS = -I. -Iinclude -Isrc/core -Isrc/ui -Isrc/util -Isrc/audio -g -Wall -std=c++17 -DGLEW_STATIC
LDFLAGS =

OBJ_DIR = obj
BIN_DIR = bin

# Define ONDE estão os arquivos .cpp
# Adicione '.' se o main.cpp estiver na raiz, caso contrário, remova.
SRC_DIRS := . src/core src/ui src/util src/audio src/gfx

# pega todos os .cpp dessas pastas
SOURCES := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.cpp))
# mapeia para objetos, espelhando a subpasta (core/game.cpp -> obj/core/game.o)
OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

ifeq ($(OS),Windows_NT)
    EXEC_NAME = jogo.exe
    SHELL := cmd.exe
    SHELLFLAGS := /C
    MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
    RM = if exist $(subst /,\,$(1)) rmdir /S /Q $(subst /,\,$(1))

	LDFLAGS += -Llib

    # SDL2 + SDL2_mixer (esperando os .a/.lib em lib/)
    LIBS = -lSDL2_mixer -lSDL2\
           -lglew32 -lfreeglut -lglu32 -lopengl32 -lgdi32 -lwinmm
else
    EXEC_NAME = jogo
    MKDIR = mkdir -p $(1)
    RM = rm -rf $(1)
    # adiciona flags do SDL2/SDL2_mixer automaticamente via pkg-config
    CXXFLAGS += $(shell pkg-config --cflags sdl2 SDL2_mixer)
    LIBS = -lglut -lGLEW -lGL -lGLU $(shell pkg-config --libs sdl2 SDL2_mixer)
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

# cria subpastas em obj/ automaticamente
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
	$(call RM,$(OBJ_DIR))
	@echo Limpeza concluída.
ifeq ($(OS),Windows_NT)
	if exist $(BIN_DIR)\$(EXEC_NAME) del /Q $(BIN_DIR)\$(EXEC_NAME)
else
	rm -f $(BIN_DIR)/$(EXEC_NAME)
endif

.PHONY: all compilar executar limpar run
