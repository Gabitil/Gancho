# Nome do compilador
CC = gcc

# --- Configuração dos diretórios ---
# Diretório do executável
BIN_DIR = bin
# Diretório dos arquivos-fonte
SRC_DIR = src
# Diretório dos arquivos-objeto
OBJ_DIR = obj
# Diretório dos cabeçalhos das libs
INCLUDE_DIR = -Iinclude
# Diretório das libs
LIB_DIR = -Llib

# --- Configuração do executável e fontes ---
# Nome do executável
EXEC = $(BIN_DIR)/jogo
# Encontra todos os arquivos .c em SRC_DIR e os armazena na variável
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# Gera o nome dos arquivos-objeto, trocando o diretório e a extensão
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# --- Flags de compilação e linkagem ---
# -g: Adiciona info de debug. -Wall: Mostra todos os warnings.
CFLAGS = $(INCLUDE_DIR) -g -Wall
# Bibliotecas a serem linkadas
LIBS = -lfreeglut -lglew32 -lopengl32 -lglu32

# --- Cláusulas ---

# Cláusula principal (default)
all: $(EXEC)

# Cláusula para criar o executável
$(EXEC): $(OBJECTS)
	@if not exist $(@D) mkdir $(@D)
	$(CC) $^ -o $@ $(LIB_DIR) $(LIBS)
	@echo "Compilação concluída! Executável criado em: $(EXEC)"

# Cláusula para criar os arquivos-objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist $(@D) mkdir $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Cláusula para executar
executar: all
	@echo "--- Executando o Jogo ---"
	./$(EXEC)
	@echo "--- Jogo Finalizado ---"

# Cláusula para limpar tudo
limpar:
	@echo "Limpando arquivos de compilação..."
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all executar limpar