# ✅ TP1 — Jogo Educativo 2D

**Pontuação total obrigatória:** 20 pontos
**Pontuação extra opcional:** até +4 pontos

---

## 🧩 Requisitos Obrigatórios

### 🎮 Estrutura e Navegação

- [X] **(1 pt)** Criar menu inicial com:
  - [X] Botão “Iniciar” que começa o jogo
  - [X] Botão “Como jogar” que abre explicações
  - [X] Botão “Voltar para o Menu”
  - [X] Efeito visual ao passar o mouse sobre os botões

---

### 🌄 Fases e Cenário

- [X] **(3 pts)** Criar **pelo menos 3 fases bonitas**, com:
  - [X] Backgrounds bem modelados
  - [X] Movimento **paralaxe de fundo**
  - [ ] Duração de 2 a 5 minutos cada
  - [ ] Desafios constantes (sem períodos de inatividade)

---

### 🧠 Personagens e Desafios

- [ ] **(5 pts)** Implementar:
  - [ ] **4 personagens** (ou elementos equivalentes) com comportamentos diferentes
  - [ ] **1 desafio maior (chefão)** com comportamento diferenciado

---

### 🌀 Animações e Transformações

- [X] **(3 pts)** Aplicar **todas as transformações aprendidas em aula**
- [X] Usar **listas de apresentação (display lists)** para renderização

---

### 💥 Mecânicas e Interação

- [X] **(2 pts)** Implementar **colisão de retângulos**
- [ ] **(2 pts)** Adicionar **sistema de pontuação**
- [ ] **(2 pts)** Adicionar **sistema de vida**

---

### ☠️ Game Over e Virada de Jogo

- [ ] **(2 pts)** Criar animações de:
  - [X] **Game Over**
  - [ ] **Virada de jogo (vitória)**
  - [ ] Retornar ao menu inicial após o fim da partida

---

### 🎥 Apresentação e Submissão

- [ ] **(2 pts)** Fazer **vídeo de gameplay** (até 10 min) mostrando:

  - [ ] Execução de todos os requisitos
  - [ ] Explicação do código-fonte
  - [ ] Link incluso no `readme.txt`
- [X] Aplicar **texturas obrigatórias** em todos os elementos visuais

---

## 📁 Estrutura de Entrega

- [X] `Makefile` na **pasta raiz** (com cláusulas `compilar`, `executar`, `limpar`)
- [ ] **Códigos-fonte** na pasta raiz
- [ ] Pasta **modelos/** com arquivos de modelos 2D
- [X] Pasta **imagens/** com arquivos `.png`
- [X] Arquivo `readme.txt` contendo:

  - [X] Nomes e matrículas do grupo
  - [X] Instruções de compilação e execução
  - [X] Observações para instalação de bibliotecas adicionais (Debian/Linux)
  - [X] Instruções de jogabilidade
  - [ ] Link do vídeo de gameplay
- [ ] Compactar em **.zip** antes de enviar
- [ ] ❗ **Não enviar executável nem arquivos desnecessários**

  - Penalidade: **–5 pontos**
- [ ] Caso ultrapasse 20MB, incluir link de download no ZIP

---

## ⭐ Pontos Extras (1 ponto cada)

- [ ] **( +1 pt )** Inserir **música de fundo e efeitos sonoros**
  - Dica: usar **SDL_mixer** ou similar
- [ ] **( +1 pt )** Adicionar **efeitos visuais de explosão** e **colisão**
- [ ] **( +1 pt )** Adicionar **itens power-up / vidas extras**
  - Restaurar estado padrão ao perder uma vida
- [ ] **( +1 pt )** Criar **+2 fases**, **+2 inimigos simples** e **+1 chefão final**
