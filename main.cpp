/**
 * Para a segunda entrega do trabalho, optamos por fazer uma lógica separada
 * para o jogo 3D e controlar os estados do jogo e as atualizações pela
 * "main.cpp". Isso irá facilitar a manutenção e evitar retrabalho de
 * refatoração nas funções já criadas para o jogo 2D. As funções foram nomeadas
 * para fins de visualização e organização e todas os nomes pertinentes ao 3D
 * possuem o sufixo "_3D" ao final. Tentamos manter o máximo possível de
 * igualdade entre as lógicas da física e mecânica do jogo. Todas as funções de
 * callback feitas pela main tiveram que ser incrementadas de uma lógica de
 * máquina de estados para verificação de se está no modo 2D ou 3D e, além
 * disso, tratamentos tiveram de ser feitos devido á diferença entre montagem de
 * cenas no modo 2D e no modo 3D (questões de perspectiva, iluminação,
 * profundidade, matrizes de projeção, ponteiro do mouse etc). Todos os headers
 * tiveram de ser refeitos pois tivemos muitos problemas com incompatibilidade
 * de ordem das bibliotecas, excepcionalmente a GLEW e a FreeGlut
 */

// Tipos fundamentais que são cruciais para o uso da GLEW
#include <cstddef>
#include <cstdint>

// GLEW - Deve estar antes do FreeGlut
#include <GL/glew.h>

// FreeGLUT
#include <GL/freeglut.h>

// Bibliotecas Padrão
#include <stdio.h>

#include <string>

// Headers ".h"
#include "audio.h"
#include "game.h"
#include "game_3D.h"
#include "menu.h"
#include "utils.h"

// ------------------------------------------------------------------------------------------------------------------

const int NUM_MODE_SELECT_BUTTONS = 3;
const int NUM_SUB_MENU_BUTTONS = 3;
const int INITIAL_WIN_WIDTH = 800;
const int INITIAL_WIN_HEIGHT = 600;
const int NUM_LEVELS = 3;

// Controle de estados amplificado para suportar o modo 2D e 3D
enum GameState {
  STATE_MODE_SELECT,
  STATE_MENU_2D,
  STATE_MENU_3D,
  STATE_INSTRUCTIONS_2D,
  STATE_INSTRUCTIONS_3D,
  STATE_LEVEL_SELECT_2D,
  STATE_LEVEL_SELECT_3D,
  STATE_GAME_2D,
  STATE_GAME_3D
};

GameState currentState = STATE_MODE_SELECT;

// IDs das texturas de menu
GLuint texMenuBackground = 0;
GLuint texButtonNormal = 0;
GLuint texButtonHover = 0;
GLuint texButtonDisabled = 0;
GLuint texLevelSelectBackground = 0;
GLuint texInstructionsBackground = 0;

// Texturas do Jogador (animação básica)
GLuint texPlayer = 0;
GLuint texPlayerRunRight = 0;
GLuint texPlayerRunLeft = 0;
GLuint texPlayerJump = 0;
GLuint texPlayerLose = 0;

// Texturas do Level 1
GLuint texBackground = 0;
GLuint texBackgroundFar = 0;
GLuint texBackgroundMid = 0;
GLuint texBackgroundNear = 0;
GLuint texFloor = 0;
GLuint texRiver = 0;
GLuint texPlatform = 0;
GLuint texObstacleBottom = 0;
GLuint texDoor = 0;

GLuint texWinGame = 0;
GLuint texGameOver = 0;

GLuint texDisplayGrappler = 0;  // Textura do gancho

Button modeSelectButtons[NUM_MODE_SELECT_BUTTONS];  // Botões do menu principal
                                                    // (2D, 3D, etc.)
Button menu2DButtons[NUM_SUB_MENU_BUTTONS];         // Botões do sub-menu 2D
Button menu3DButtons[NUM_SUB_MENU_BUTTONS];         // Botões do sub-menu 3D

Button instrBackButton;  // Boão de voltar contido na tela de instruções
Button levelButtons[NUM_LEVELS];  // Botões para selecionar qual o level o
                                  // player quer jogar
Button levelSelectBackButton;  // Botão de "voltar" contido na seleção de levels
Button gameBackButton;  // Botão de "voltar" contido dentro do próprio jogo

MenuOption selectedMenuOption = NONE;
int maxLevelUnlocked_2D = 1;
int maxLevelUnlocked_3D = 1;  // Começa com apenas o nível 1 desbloqueado

int activeLevel_2D = 1;
int activeLevel_3D = 1;

// ------------------------------------------------------------------------------------------------------------------

// Callbacks padrões da própria main
void display();
void reshape(int w, int h);
void mouseMotion(int x, int y);
void mouseClick(int button, int state, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void specialDown(int key, int x, int y);
void specialUp(int key, int x, int y);
void timer(int value);

// ------------------------------------------------------------------------------------------------------------------

/**
 * Versões das funções para o 2D (nomes mantidos) e para o 3D. Optamos por
 * manter o mesmo padrão de nomes para facilitar a codificação
 */
extern void gameStartLevel(int level);
extern GameAction gameUpdate();
extern void gameDisplay();
extern void gameReshape(int w, int h);
extern void gameMouseMotion(int x, int y);
extern void gameMouseClick(int button, int state);
extern GameAction gameKeyDown(unsigned char key, int x, int y);
extern void gameKeyUp(unsigned char key, int x, int y);
extern void gameSpecialDown(int key, int x, int y);
extern void gameSpecialUp(int key, int x, int y);

void renderInstructions_3D();
void renderLevelSelect_3D();
void drawGameHUD_3D();
extern void gameStartLevel_3D(int level);
extern void gameDisplay_3D();
extern void gameReshape_3D(int w, int h);
extern void gameMouseMotion_3D(int x, int y);
extern void gameMouseClick_3D(int button, int state);
extern GameAction gameKeyDown_3D(unsigned char key, int x, int y);
extern void gameKeyUp_3D(unsigned char key, int x, int y);
extern void gameSpecialDown_3D(int key, int x, int y);
extern void gameSpecialUp_3D(int key, int x, int y);
extern GameAction gameUpdate_3D();
extern void loadGameTextures_3D();

// ------------------------------------------------------------------------------------------------------------------

/**
 * Carrega todas as texturas do jogo 2D.
 */

void loadGameTextures() {
  // Texturas do Jogador
  texPlayer = loadTexture_2D("assets/images/player/player.png");
  texPlayerRunRight =
      loadTexture_2D("assets/images/player/player_run_right.png");
  texPlayerRunLeft = loadTexture_2D("assets/images/player/player_run_left.png");
  texPlayerJump = loadTexture_2D("assets/images/player/player_jump.png");
  texPlayerLose = loadTexture_2D("assets/images/player/player_lose.png");
  texDoor = loadTexture_2D("assets/images/global/door.png");

  std::string levelPath;
  switch (CURRENT_LEVEL) {
    case 1:
      levelPath = "assets/images/level1/";
      break;
    case 2:
      levelPath = "assets/images/level2/";
      break;
    case 3:
      levelPath = "assets/images/level3/";
      break;
    default:
      levelPath = "assets/images/level1/";
      break;
  }

  texBackgroundFar = loadTexture_2D((levelPath + "bg-far.png").c_str());
  texBackgroundMid = loadTexture_2D((levelPath + "bg-mid.png").c_str());
  texBackgroundNear = loadTexture_2D((levelPath + "bg-near.png").c_str());
  texFloor = loadTexture_2D((levelPath + "floor.png").c_str());
  texRiver = loadTexture_2D((levelPath + "river.png").c_str());
  texPlatform = loadTexture_2D((levelPath + "complete_platform.png").c_str());
  texObstacleBottom =
      loadTexture_2D((levelPath + "obstacle_bottom.png").c_str());
  texWinGame = loadTexture_2D("assets/images/global/win_game.png");
  texGameOver = loadTexture_2D("assets/images/global/game_over.png");
  texDisplayGrappler =
      loadTexture_2D("assets/images/global/display_grappler.png");
}

void renderInstructions_2D() {
  glClear(GL_COLOR_BUFFER_BIT);
  float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
  float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
  drawTexturedRect(0, 0, winW, winH, texInstructionsBackground, false, false);
  drawText(50, 80, "INSTRUCOES DE COMO JOGAR (2D): ");
  drawText(50, 120,
           "- Use 'A' e 'D' ou setas (<- ->) para andar no chao. Nao e "
           "possivel pular");
  drawText(50, 150, "- Mire com o mouse para onde quer disparar o gancho");
  drawText(50, 180,
           "- Clique e segure para disparar o gancho e, ao ser preso, solte "
           "para desprende-lo");
  drawText(
      50, 210,
      "- Observe os valores no canto superior direito para se guiar melhor");
  drawText(50, 240, "- Haverao diversos obstaculos. Dentre eles:");
  drawText(100, 270,
           "- Plataformas para andar por cima e/ou se prender com o gancho.");
  drawText(100, 300,
           "- Correntes de vento que podem mudar a direcao do seu lancamento");
  drawText(100, 330, "- Espinhos fatais (aconselhamos a nao toca-los)");
  drawText(50, 390, "Seu objetivo e chegar ate a porta final =)");
  drawText(50, 420, "Economize disparos! Boa sorte!");

  drawButton(instrBackButton);
  glutSwapBuffers();
}

void renderLevelSelect_2D() {
  glClear(GL_COLOR_BUFFER_BIT);
  float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
  float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
  drawTexturedRect(0, 0, winW, winH, texLevelSelectBackground, false, false);
  drawText(50, 80, "Escolha a fase (2D)");
  for (int i = 0; i < NUM_LEVELS; i++) {
    drawButton(levelButtons[i]);
  }
  drawButton(levelSelectBackButton);
  glutSwapBuffers();
}

void drawGameHUD_2D() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  drawButton(gameBackButton);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

// ----------------------------------------------------------------------------------------------------

/**
 * Um problema que tivemos foi as questões de visualização que se alteram do
 * modo 2D para o 3D. Portanto, para encerrar questões de iluminação,
 * perspectiva, profundidade e matrizes de projeção, fizemos essa função que
 * visa sair do modo 3D para não gerar inconsistência na visualização do 2D
 */
void exitGame3DMode() {
  // Desliga sistemas 3D
  glDisable(GL_LIGHTING);        // Desliga cálculo de luz
  glDisable(GL_LIGHT0);          // Desliga a luz específica
  glDisable(GL_DEPTH_TEST);      // Desliza profundidade
  glDisable(GL_COLOR_MATERIAL);  // Desliga materiais
  glDisable(GL_NORMALIZE);       // Desliga normalização de vetores

  // Reseta a Matriz de Projeção para evitar distorções
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Reseta a cor para Branco Puro
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  // Mostra o Mouse
  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

  // Restaura callbacks (parte importante devido ao modo FPS do mouse no jogo
  // 3D)
  glutPassiveMotionFunc(mouseMotion);
  glutMotionFunc(mouseMotion);
}

void renderInstructions_3D() {
  glClear(GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT);  // É necessário a profundidade para escrita e
                                 // formação de cenas em 3D

  float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
  float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
  drawTexturedRect(0, 0, winW, winH, texInstructionsBackground, false, false);

  drawText(50, 120, "- Use 'W', 'A', 'S', 'D' para se mover no chao.");
  drawText(50, 150, "- Use o MOUSE para olhar e mirar.");
  drawText(
      50, 190,
      "- BOTAO ESQUERDO DO MOUSE (SEGURAR): Carrega a forca do lancamento.");
  drawText(50, 220,
           "- BOTAO ESQUERDO DO MOUSE (SOLTAR): Dispara o gancho na mira.");
  drawText(50, 250, "- ESPACO: Solta a corda se estiver pendurado.");
  drawText(50, 300,
           "- A fisica 3D usa balanco e movimento. Aproveite o impulso!");
  drawText(50, 330,
           "- Algumas paredes podem ser quebradas com alta velocidade.");
  drawText(50, 380, "- OBJETIVO: Encontre a porta para vencer.");
  drawText(50, 430, "- Para sair do jogo, pressione 'Q' ou 'ESC'.");
  drawText(50, 460, "- Para soltar/prender o mouse, pressione 'M' ou 'ALT'.");

  drawButton(instrBackButton);
  glutSwapBuffers();
}

void renderLevelSelect_3D() {
  glClear(GL_COLOR_BUFFER_BIT);
  float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
  float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
  drawTexturedRect(0, 0, winW, winH, texLevelSelectBackground, false, false);

  drawText(50, 80, "Escolha a fase 3D");
  for (int i = 0; i < NUM_LEVELS; i++) {
    drawButton(levelButtons[i]);
  }
  drawButton(levelSelectBackButton);
  glutSwapBuffers();
}

void drawGameHUD_3D() { drawGameHUD_2D(); }

// ----------------------------------------------------------------------------------------------------

/**
 * Agora a função de display é uma máquina de estados mais complexa para
 * controlar qual o tipo de jogo que está sendo jogado. É a mesma coisa da
 * função display para 2D, mas agora com mais verificações de estado
 */
void display() {
  switch (currentState) {
    case STATE_MODE_SELECT:
      glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
      renderMenu(modeSelectButtons, NUM_MODE_SELECT_BUTTONS);
      break;
    case STATE_MENU_2D:
      glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
      renderMenu(menu2DButtons, NUM_SUB_MENU_BUTTONS);
      break;
    case STATE_MENU_3D:
      glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
      renderMenu(menu3DButtons, NUM_SUB_MENU_BUTTONS);
      break;
    case STATE_INSTRUCTIONS_2D:
      glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
      renderInstructions_2D();
      break;
    case STATE_INSTRUCTIONS_3D:
      glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
      renderInstructions_3D();
      break;
    case STATE_LEVEL_SELECT_2D:
      glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
      renderLevelSelect_2D();
      break;
    case STATE_LEVEL_SELECT_3D:
      glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
      renderLevelSelect_3D();
      break;
    case STATE_GAME_2D:
      glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
      gameDisplay();
      drawGameHUD_2D();
      glutSwapBuffers();
      break;
    case STATE_GAME_3D:
      // Não define glClearColor aqui - deixa o gameStartLevel_3D controlar a cor do céu
      gameDisplay_3D();
      drawGameHUD_3D();
      glutSwapBuffers();
      break;
  }
}

/**
 * Atualiza a posição de TODOS os botões. Função aproveitada do modo 2D e usada
 * também para o 3D
 */
void updateAllButtonPositions(int w, int h) {
  float buttonWidth = 220.0f;
  float buttonHeight = 50.0f;
  float spacing = 20.0f;
  float margin = 20.0f;
  float startX = (w - buttonWidth) / 2.0f;

  float totalModeSelectH = (NUM_MODE_SELECT_BUTTONS * buttonHeight) +
                           ((NUM_MODE_SELECT_BUTTONS - 1) * spacing);
  float startModeSelectY = (h - totalModeSelectH) / 2.0f;
  for (int i = 0; i < NUM_MODE_SELECT_BUTTONS; ++i) {
    modeSelectButtons[i].x = startX;
    modeSelectButtons[i].y = startModeSelectY + i * (buttonHeight + spacing);
  }

  float totalSubMenuH = (NUM_SUB_MENU_BUTTONS * buttonHeight) +
                        ((NUM_SUB_MENU_BUTTONS - 1) * spacing);
  float startSubMenuY = (h - totalSubMenuH) / 2.0f;
  for (int i = 0; i < NUM_SUB_MENU_BUTTONS; i++) {
    // Ambos os menus 2D e 3D podem compartilhar as mesmas coordenadas pois
    // nunca são exibidos ao mesmo tempo.
    menu2DButtons[i].x = startX;
    menu2DButtons[i].y = startSubMenuY + i * (buttonHeight + spacing);
    menu3DButtons[i].x = startX;
    menu3DButtons[i].y = startSubMenuY + i * (buttonHeight + spacing);
  }

  for (int i = 0; i < NUM_LEVELS; i++) {
    levelButtons[i].x = startX;
    levelButtons[i].y = startSubMenuY + i * (buttonHeight + spacing);
    if (currentState == STATE_LEVEL_SELECT_2D)
      levelButtons[i].enabled = (i + 1 <= maxLevelUnlocked_2D);
    else
      levelButtons[i].enabled = (i + 1 <= maxLevelUnlocked_3D);
  }

  instrBackButton.x = margin;
  instrBackButton.y = h - buttonHeight - margin;

  levelSelectBackButton.x = margin;
  levelSelectBackButton.y = h - buttonHeight - margin;

  gameBackButton.x = margin;
  gameBackButton.y = margin;
}

/**
 * Por questões de perspectiva, profundidade, matrizes e etc, o reshape não pôde
 * ser aproveitado entre as versões 2D e 3D. Portanto, também é uma máquina de
 * estados e que chama o reshape corretamente de acordo com o tipo de jogo
 */
void reshape(int w, int h) {
  if (h == 0) h = 1;

  if (currentState == STATE_GAME_2D) {
    gameReshape(w, h);  // Chama o reshape 2D
  } else if (currentState == STATE_GAME_3D) {
    gameReshape_3D(w, h);  // Chama o reshape 3D
  } else {
    // Somente os MENUS usam esta projeção 2D. Isso pôde ser igual para ambos
    // pois o menu sempre será 2D
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  updateAllButtonPositions(w, h);
}

/**
 * Mouse motion adaptado para todos os estados
 */
void mouseMotion(int x, int y) {
  if (currentState == STATE_GAME_2D) {
    gameBackButton.hovered =
        (x >= gameBackButton.x && x <= gameBackButton.x + gameBackButton.w &&
         y >= gameBackButton.y && y <= gameBackButton.y + gameBackButton.h);
    gameMouseMotion(x, y);
  } else if (currentState == STATE_GAME_3D) {
    /**
     * As funções de hover do 3D tiveram de ser passadas para antes do
     * mouseMotion pois haviam casos, no meio do jogo, em que o hover não
     * funcionava corretamente. Além disso, parte do controle do mouse foi
     * passado para a main pois é ela que desenha o botão de voltar enquanto
     * está no jogo. Dessa forma, esse controle do mouseMotion passa pela main
     * antes de transferir a função para o jogo 3D.
     */
    gameBackButton.hovered =
        (x >= gameBackButton.x && x <= gameBackButton.x + gameBackButton.w &&
         y >= gameBackButton.y && y <= gameBackButton.y + gameBackButton.h);

    gameMouseMotion_3D(x, y);
  } else {
    // Lógica de hover para todos os menus (padrão)
    if (currentState == STATE_MODE_SELECT) {
      for (int i = 0; i < NUM_MODE_SELECT_BUTTONS; i++) {
        modeSelectButtons[i].hovered =
            (x >= modeSelectButtons[i].x &&
             x <= modeSelectButtons[i].x + modeSelectButtons[i].w &&
             y >= modeSelectButtons[i].y &&
             y <= modeSelectButtons[i].y + modeSelectButtons[i].h);
      }
    } else if (currentState == STATE_MENU_2D) {
      for (int i = 0; i < NUM_SUB_MENU_BUTTONS; i++) {
        menu2DButtons[i].hovered =
            (x >= menu2DButtons[i].x &&
             x <= menu2DButtons[i].x + menu2DButtons[i].w &&
             y >= menu2DButtons[i].y &&
             y <= menu2DButtons[i].y + menu2DButtons[i].h);
      }
    } else if (currentState == STATE_MENU_3D) {
      for (int i = 0; i < NUM_SUB_MENU_BUTTONS; i++) {
        menu3DButtons[i].hovered =
            (x >= menu3DButtons[i].x &&
             x <= menu3DButtons[i].x + menu3DButtons[i].w &&
             y >= menu3DButtons[i].y &&
             y <= menu3DButtons[i].y + menu3DButtons[i].h);
      }
    } else if (currentState == STATE_INSTRUCTIONS_2D ||
               currentState == STATE_INSTRUCTIONS_3D) {
      instrBackButton.hovered = (x >= instrBackButton.x &&
                                 x <= instrBackButton.x + instrBackButton.w &&
                                 y >= instrBackButton.y &&
                                 y <= instrBackButton.y + instrBackButton.h);
    } else if (currentState == STATE_LEVEL_SELECT_2D ||
               currentState == STATE_LEVEL_SELECT_3D) {
      for (int i = 0; i < NUM_LEVELS; i++) {
        if (levelButtons[i].enabled) {
          levelButtons[i].hovered =
              (x >= levelButtons[i].x &&
               x <= levelButtons[i].x + levelButtons[i].w &&
               y >= levelButtons[i].y &&
               y <= levelButtons[i].y + levelButtons[i].h);
        }
      }
      levelSelectBackButton.hovered =
          (x >= levelSelectBackButton.x &&
           x <= levelSelectBackButton.x + levelSelectBackButton.w &&
           y >= levelSelectBackButton.y &&
           y <= levelSelectBackButton.y + levelSelectBackButton.h);
    }
  }

  glutPostRedisplay();
}

/**
 * Assim como na função de mouseMotion, é o menu que controla o botão de voltar
 * contido dentro do jogo. Portanto, nesse caso, é necessário um tratamento do
 * clique do mouse pela main antes de transferir a lógica para as callbacks dos
 * jogos. A maioria da lógica foi duplicada da versão 2D
 */
void mouseClick(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (currentState == STATE_GAME_2D) {
      if (state == GLUT_DOWN && gameBackButton.hovered) {
        currentState = STATE_LEVEL_SELECT_2D;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        Audio::stopMusic();
        Audio::playMusic("tema_menu", -1);
        Audio::playSound("clickButton");
      } else {
        gameMouseClick(button, state);
      }
    } else if (currentState == STATE_GAME_3D) {
      bool clickedBack =
          (x >= gameBackButton.x && x <= gameBackButton.x + gameBackButton.w &&
           y >= gameBackButton.y && y <= gameBackButton.y + gameBackButton.h);

      /**
       * Apenas se o mouse estiver livre significa que clicou no botão. Apesar
       * de no jogo o mouse ser reatualizado o tempo inteiro para estar no
       * centro da tela, adicionamos essa verificação para garantir
       * consistência. Para isso, foi necessário usar uma variável externa
       * "isMouseFree_3D"
       */

      if (isMouseFree_3D && clickedBack && state == GLUT_DOWN) {
        exitGame3DMode();  // Sai do modo de jogo 3D para evitar inconsistências
                           // de montagem de tela
        currentState = STATE_LEVEL_SELECT_3D;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        Audio::stopMusic();
        Audio::playMusic("tema_menu", -1);
        Audio::playSound("clickButton");
      } else {
        // Caso contrário, manda o clique para a mecânica de tiro pois significa
        // que o mouse estava em modo FPS
        gameMouseClick_3D(button, state);
      }
    } else if (state == GLUT_DOWN) {
      switch (currentState) {
        case STATE_MODE_SELECT:
          selectedMenuOption =
              handleMenuInput(x, y, modeSelectButtons, NUM_MODE_SELECT_BUTTONS);
          switch (selectedMenuOption) {
            case 1:
              Audio::playSound("clickButton");
              currentState = STATE_MENU_2D;
              break;
            case 2:
              Audio::playSound("clickButton");
              currentState = STATE_MENU_3D;
              break;
            case 3:  // Sair
              Audio::playSound("clickButton");
              glutLeaveMainLoop();
              break;
          }
          break;

        case STATE_MENU_2D:
          selectedMenuOption =
              handleMenuInput(x, y, menu2DButtons, NUM_SUB_MENU_BUTTONS);
          switch (selectedMenuOption) {
            case 1:
              Audio::playSound("clickButton");
              currentState = STATE_LEVEL_SELECT_2D;
              break;
            case 2:
              Audio::playSound("clickButton");
              currentState = STATE_INSTRUCTIONS_2D;
              break;
            case 3:
              Audio::playSound("clickButton");
              currentState = STATE_MODE_SELECT;
              break;
          }
          break;

        case STATE_MENU_3D:
          selectedMenuOption =
              handleMenuInput(x, y, menu3DButtons, NUM_SUB_MENU_BUTTONS);
          switch (selectedMenuOption) {
            case 1:
              Audio::playSound("clickButton");
              currentState = STATE_LEVEL_SELECT_3D;
              break;
            case 2:
              Audio::playSound("clickButton");
              currentState = STATE_INSTRUCTIONS_3D;
              break;
            case 3:
              Audio::playSound("clickButton");
              currentState = STATE_MODE_SELECT;
              break;
          }
          break;

        case STATE_INSTRUCTIONS_2D:
          if (instrBackButton.hovered) {
            Audio::playSound("clickButton");
            currentState = STATE_MENU_2D;
          }
          break;

        case STATE_INSTRUCTIONS_3D:
          if (instrBackButton.hovered) {
            Audio::playSound("clickButton");
            currentState = STATE_MENU_3D;
          }
          break;

        case STATE_LEVEL_SELECT_2D:
          for (int i = 0; i < NUM_LEVELS; i++) {
            if (levelButtons[i].hovered && levelButtons[i].enabled) {
              Audio::playSound("clickButton");
              activeLevel_2D = i + 1;
              currentState = STATE_GAME_2D;
              gameStartLevel(activeLevel_2D);
              loadGameTextures();
              Audio::stopMusic();
              switch (activeLevel_2D) {
                case 1:
                  Audio::playMusic("fase1_2D", -1);
                  break;
                case 2:
                  Audio::playMusic("fase2_2D", -1);
                  break;
                case 3:
                  Audio::playMusic("fase3_2D", -1);
                  break;
                default:
                  break;
              }
              glutTimerFunc(16, timer, 0);
              break;
            }
          }
          if (levelSelectBackButton.hovered) {
            Audio::playSound("clickButton");
            currentState = STATE_MENU_2D;
          }
          break;

        case STATE_LEVEL_SELECT_3D:
          for (int i = 0; i < NUM_LEVELS; i++) {
            if (levelButtons[i].hovered && levelButtons[i].enabled) {
              Audio::playSound("clickButton");
              activeLevel_3D = i + 1;
              currentState = STATE_GAME_3D;
              gameStartLevel_3D(activeLevel_3D);
              loadGameTextures_3D();
              // Audio::stopMusic();
              //  switch (activeLevel_3D)
              //  {
              //    case 1: Audio::playMusic("fase1_2D", -1); break;
              //    case 2: Audio::playMusic("fase2_2D", -1); break;
              //    case 3: Audio::playMusic("fase3_2D", -1); break;
              //    default: break;
              //  }
              reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

              glutTimerFunc(16, timer, 0);
              break;
            }
          }
          if (levelSelectBackButton.hovered) {
            Audio::playSound("clickButton");
            currentState = STATE_MENU_3D;
          }
          break;
      }

      if (currentState != STATE_GAME_2D && currentState != STATE_GAME_3D) {
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
      }
    }
  }
  glutPostRedisplay();
}

// ------------------------------------------------------------------------------------------------------------------------

void keyboardDown(unsigned char key, int x, int y) {
  if (currentState == STATE_GAME_2D) {
    GameAction action = gameKeyDown(key, x, y);
    if (action == GAME_ACTION_EXIT_TO_MENU) {
      currentState = STATE_LEVEL_SELECT_2D;
      Audio::stopMusic();
      Audio::playMusic("tema_menu", -1);
      reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
      glutPostRedisplay();
    }
  }
  /**
   * Para retornar ao menu no modo 3D a lógica não pôde se manter a mesma pois é
   * necessário retransferir as funções das callbacks para a main devido às
   * diferenças.
   */
  else if (currentState == STATE_GAME_3D) {
    GameAction action = gameKeyDown_3D(key, x, y);
    if (action == GAME_ACTION_EXIT_TO_MENU) {
      exitGame3DMode();  // Sai do modo de jogo 3D para evitar inconsistências
                         // de montagem de tela

      currentState = STATE_LEVEL_SELECT_3D;
      Audio::stopMusic();
      Audio::playMusic("tema_menu", -1);

      glutMotionFunc(mouseMotion);
      glutPassiveMotionFunc(mouseMotion);

      reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
      glutPostRedisplay();
    }
  }
}

void keyboardUp(unsigned char key, int x, int y) {
  if (currentState == STATE_GAME_2D) {
    gameKeyUp(key, x, y);
  } else if (currentState == STATE_GAME_3D) {
    gameKeyUp_3D(key, x, y);
  }
}

void specialDown(int key, int x, int y) {
  if (currentState == STATE_GAME_2D) {
    gameSpecialDown(key, x, y);
  } else if (currentState == STATE_GAME_3D) {
    gameSpecialDown_3D(key, x, y);
  }
}

void specialUp(int key, int x, int y) {
  if (currentState == STATE_GAME_2D) {
    gameSpecialUp(key, x, y);
  } else if (currentState == STATE_GAME_3D) {
    gameSpecialUp_3D(key, x, y);
  }
}

/**
 * Timer adaptado para 2D e 3D
 */
void timer(int value) {
  // Se não estivermos em nenhum estado de jogo, pare o timer
  if (currentState != STATE_GAME_2D && currentState != STATE_GAME_3D) {
    return;
  }

  GameAction action;
  GameState originalState = currentState;

  if (currentState == STATE_GAME_2D) {
    action = gameUpdate();
  } else {
    action = gameUpdate_3D();
  }

  if (action == GAME_ACTION_LEVEL_WON) {
    if (originalState == STATE_GAME_2D) {
      if (activeLevel_2D == maxLevelUnlocked_2D &&
          maxLevelUnlocked_2D < NUM_LEVELS)
        maxLevelUnlocked_2D++;
      currentState = STATE_LEVEL_SELECT_2D;
      Audio::stopMusic();
      Audio::playMusic("tema_menu", -1);
    } else  // 3D
    {
      exitGame3DMode();  // Sai do modo de jogo 3D para evitar inconsistências
                         // de montagem de tela

      if (activeLevel_3D == maxLevelUnlocked_3D &&
          maxLevelUnlocked_3D < NUM_LEVELS)
        maxLevelUnlocked_3D++;
      currentState = STATE_LEVEL_SELECT_3D;
      Audio::stopMusic();
      Audio::playMusic("tema_menu", -1);
    }

    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  } else if (action == GAME_ACTION_LEVEL_LOST) {
    if (originalState == STATE_GAME_3D) {
      exitGame3DMode();  // Sai do modo de jogo 3D para evitar inconsistências
                         // de montagem de tela
    }

    // Volta para o menu de seleção de nível correto
    currentState = (originalState == STATE_GAME_2D) ? STATE_LEVEL_SELECT_2D
                                                    : STATE_LEVEL_SELECT_3D;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    Audio::stopMusic();
    Audio::playMusic("tema_menu", -1);
  } else {
    // Se o jogo continua, agenda o próximo timer
    glutTimerFunc(16, timer, 0);
  }

  glutPostRedisplay();
}

void init() {
  // Botões do Menu Principal (Seleção de Modo)
  modeSelectButtons[0] = {0, 0, 220, 50, "Jogo 2D", false, true};
  modeSelectButtons[1] = {0, 0, 220, 50, "Jogo 3D", false, true};
  modeSelectButtons[2] = {0, 0, 220, 50, "Sair", false, true};

  // Botões do Sub-Menu 2D
  menu2DButtons[0] = {0, 0, 220, 50, "Iniciar Jogo", false, true};
  menu2DButtons[1] = {0, 0, 220, 50, "Como Jogar", false, true};
  menu2DButtons[2] = {0, 0, 220, 50, "Voltar ao Menu Anterior", false, true};

  // Botões do Sub-Menu 3D
  menu3DButtons[0] = {0, 0, 220, 50, "Iniciar Jogo", false, true};
  menu3DButtons[1] = {0, 0, 220, 50, "Como Jogar", false, true};
  menu3DButtons[2] = {0, 0, 220, 50, "Voltar ao Menu Anterior", false, true};

  // Botão Voltar (na tela de instruções)
  instrBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

  // Botões de Seleção de Fase
  levelButtons[0] = {0, 0, 220, 50, "Fase 1", false, false};
  levelButtons[1] = {0, 0, 220, 50, "Fase 2", false, false};
  levelButtons[2] = {0, 0, 220, 50, "Fase 3", false, false};

  // Botão Voltar (na seleção de fases)
  levelSelectBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

  // Botão Voltar (no jogo)
  gameBackButton = {0, 0, 100, 35, "Voltar", false, true};

  updateAllButtonPositions(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
  gameStartLevel(activeLevel_2D);  // Pré-carrega o nível 1 2D

  // Carrega as texturas de menu
  texMenuBackground = loadTexture_2D("assets/images/menu/background_menu.png");
  texLevelSelectBackground =
      loadTexture_2D("assets/images/menu/background_level_select.png");
  texInstructionsBackground =
      loadTexture_2D("assets/images/menu/background_instructions.png");

  // Carrega as texturas dos botões
  texButtonNormal = loadTexture_2D("assets/images/menu/button_normal.png");
  texButtonHover = loadTexture_2D("assets/images/menu/button_hover.png");
  texButtonDisabled = loadTexture_2D("assets/images/menu/button_disabled.png");

  // Carrega as texturas do jogo 2D (pré-carregamento)
  loadGameTextures();
  loadGameAudio();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Gancho");

  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Erro GLEW: %s\n", glewGetErrorString(err));
    return 1;
  }

  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouseClick);
  glutPassiveMotionFunc(mouseMotion);
  glutMotionFunc(mouseMotion);

  glutKeyboardFunc(keyboardDown);
  glutKeyboardUpFunc(keyboardUp);
  glutSpecialFunc(specialDown);
  glutSpecialUpFunc(specialUp);

  glutMainLoop();
  return 0;
}