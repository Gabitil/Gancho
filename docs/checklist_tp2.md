# Checklist TP2 – Jogo Educativo 2D/3D

## 📝 Informações Gerais

- **Grupo:** até 3 integrantes
- **Entrega:** 30/11/2025
- **Valor:** 30 pontos
- **Obrigatório:** usar texturas em todos os elementos

---

## ✅ 1. Menu Inicial (Requisito)

- [ ] Atualizar menu inicial
- [ ] Botão **Iniciar** → inicia o jogo
- [ ] Botão **Como Jogar** → abre tela com instruções
- [ ] Tela “Como Jogar” com botão **Voltar para o Menu**
- [ ] Efeito visual de *mouse hover* nos botões

---

## ✅ 2. Fases 2D + Novas Fases com Paralaxe (5 pts)

- [ ] Adicionar **3 fases novas** após as fases do TP1
- [ ] Fases bonitas, com movimento paralaxe
- [ ] Cada fase dura **2–5 minutos**
- [ ] Não ter partes sem desafio (evitar osciosidade)

---

## ✅ 3. Modelos 3D, Chefão e Desafios (5 pts)

- [ ] Criar **pelo menos 4 personagens novos em 3D**
- [ ] Criar **1 chefão 3D** com comportamento diferente
- [ ] Personagens podem ser objetos 3D equivalentes

---

## ✅ 4. Transformações 3D + Vertex Array + VBO (5 pts)

- [ ] Aplicar transformações ensinadas:
  - [ ] translação
  - [ ] rotação
  - [ ] escala
- [ ] Utilizar **Vertex Arrays**
- [ ] Utilizar **VBO (Vertex Buffer Objects)**
- [ ] Incluir animações com essas transformações

---

## ✅ 5. Colisões em 3D (4 pts)

- [ ] Implementar sistema de colisão entre objetos 3D
- [ ] Integrar com física do jogo
- [ ] Integrar com vida/dano/etc.

---

## ✅ 6. Pontuação e Vida

- [ ] Sistema de pontuação
- [ ] Sistema de vidas

---

## ✅ 7. Game Over + Virada de Jogo com efeitos 3D (4 pts)

- [ ] Criar animação de **Game Over**
- [ ] Criar animação de **Vitória / Virada de Jogo**
- [ ] Incluir efeitos 3D
- [ ] Após a animação, retornar ao menu inicial

---

## ✅ 8. Iluminação (4 pts)

- [ ] Sistema de iluminação nas fases 2D
- [ ] Sistema de iluminação nas fases 3D
- [ ] Pelo menos **2 tipos de fontes de luz**
- [ ] Configuração de material
- [ ] Texturas integradas com luz

---

## ✅ 9. Gameplay + Apresentação no YouTube (3 pts)

- [ ] Vídeo de até **10 minutos**
- [ ] Mostrar execução do jogo
- [ ] Explicar **cada requisito** funcionando
- [ ] Explicar onde está no **código-fonte**
- [ ] Enviar link no README

---

# 📁 Entrega — Arquivos Obrigatórios

### Pasta Raiz

- [ ] `makefile` com comandos:
  - [ ] `compilar`
  - [ ] `executar`
  - [ ] `limpar`
- [ ] Códigos-fonte (`.cpp`, `.h`)
- [ ] `readme.txt` com:
  - [ ] nomes e matrículas
  - [ ] como compilar/executar
  - [ ] instalação das libs no Linux Debian-based
  - [ ] como jogar
  - [ ] link do YouTube

### Pasta `imagens/`

- [ ] Arquivos `.png`

### Pasta `modelos/`

- [ ] Arquivos 2D (textos / definições de modelo)

### Importante

- [ ] **Não enviar executável**
- [ ] **Não enviar arquivos inúteis**

- Se enviar → **-5 pontos automático**
- Se não compilar → **não corrige**

---

# ⭐ Pontuação Extra (1 ponto cada)

## 🎵 1. Música e efeitos sonoros

- [ ] Música de fundo
- [ ] Efeitos sonoros

- Sugestão: **SDL_mixer**

## 💥 2. Efeitos visuais

- [ ] Explosão ao destruir inimigo
- [ ] Efeito de colisão projétil ↔ inimigo

## ❤️ 3. Coleta de itens

- [ ] Itens de vida
- [ ] Power-ups
- [ ] Reset de estado ao perder vida (ou comportamento equivalente)

## ➕ 4. Conteúdo Extra

- [ ] +2 fases novas
- [ ] +2 inimigos simples novos
- [ ] +1 chefão adicional
