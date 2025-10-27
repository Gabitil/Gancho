# Gancho

## Integrantes

- Gabriel Augusto de Lima Maia — Matrícula *20193023300*
- Geovane da Silva Lelis — Matrícula *20233008539*
- Otavio Andrade e Ferreira — Matrícula *20233004899*

## Visão Geral

Gancho é um jogo 2D desenvolvido com OpenGL e FreeGLUT que desafia o jogador a atravessar fases utilizando um gancho para se locomover por plataformas suspensas. Cada estágio combina elementos de física, zonas de vento e obstáculos que exigem planejamento dos disparos do gancho e gerenciamento dos recursos disponíveis.

### Principais funcionalidades

- **Máquina de estados** que organiza o fluxo entre menu principal, instruções, seleção de fases e jogo.
- **Três fases** com parâmetros de física próprios, incluindo gravidade, velocidade do jogador e número máximo de disparos do gancho.
- **Mecânica de gancho** com projétil, corda e forças de tração que influenciam a movimentação.
- **Obstáculos variados**, como plataformas, zonas de vento que alteram a trajetória, paredes destrutíveis e áreas com espinhos fatais.
- **HUD informativo** com contador de disparos restantes, indicadores de velocidade e vetores de força para auxiliar o jogador.
- **Listas de exibição (display lists)** para otimizar a renderização de elementos estáticos de cada fase.

## Requisitos de Sistema

- Sistema operacional: Windows, Linux ou outro compatível com OpenGL 2.0+
- Compilador C++ com suporte ao padrão C++17 (g++, clang ou MSVC)
- Bibliotecas gráficas OpenGL, FreeGLUT e GLEW

### Instalação de dependências em Debian/Ubuntu

```bash
sudo apt update
sudo apt install build-essential freeglut3-dev libglew-dev mesa-common-dev mesa-utils
sudo apt-get install libsoil-dev
```

## Compilação

O projeto disponibiliza um `Makefile` multiplataforma. No Linux ou WSL, execute:

```bash
make
```

O binário final será criado em `bin/jogo`.

Para limpar artefatos de compilação:

```bash
make limpar
```

## Execução

Após compilar, rode o jogo com:

```bash
make executar
```

ou diretamente:

```bash
./bin/jogo
```

Em Windows, utilize um terminal do MinGW ou similar e execute `mingw32-make` com os mesmos alvos (`compilar`, `executar`, `limpar`).

## Instruções de Jogabilidade

1. **Navegação de menus**
   - Use o mouse para selecionar entre *Iniciar*, *Como Jogar* e *Sair*.
   - Na tela de instruções, clique em **Voltar** para retornar ao menu principal.
   - Na seleção de fases, escolha uma das fases liberadas para jogar ou clique em **Voltar**.
2. **Controles dentro do jogo**
   - Mova o personagem no chão usando `A`/`D` ou as setas esquerda/direita.
   - Mire com o mouse e **clique e segure o botão esquerdo** para disparar o gancho.
   - Solte o botão do mouse para desprender o gancho quando estiver preso.
   - Gerencie o número limitado de disparos exibido no HUD.
3. **Objetivo e obstáculos**
   - Alcance a porta no fim da fase antes de ficar sem disparos ou tocar em espinhos.
   - Aproveite as plataformas para ganhar altura e utilize as correntes de vento para ajustar trajetórias.
   - Quebre paredes específicas aplicando força suficiente com o gancho.

## Vídeo de Gameplay

Assista à demonstração completa do jogo em: https://youtu.be/INSIRA_O_LINK_DO_VIDEO

---

Caso encontre problemas ou tenha dúvidas, abra uma issue no repositório com detalhes do ambiente e dos passos realizados.
