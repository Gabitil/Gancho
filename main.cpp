// /**
//  * Para fazermos a main, pesquisamos os melhores padrões de projeto para
//  * organizar a melhor forma de estruturar as funções de callback. Com isso,
//  * dividimos o projeto em estados (uma máquina de estados definida pelo
//  * "GameState"). As funções terão as suas atividades subdivididas e a main
//  * cuidará de tudo que não for totalmente relacionado ao jogo. As funções de
//  * tratamento de teclado, mouse e clique são separadas para garantir a
//  * modularização do projeto.
//  */

// #include <GL/freeglut.h>
// #include <stdio.h>

// #include <string>

// #include "game.h"
// #include "game_3D.h"
// #include "menu.h"
// #include "utils.h"

// // ------------------------------------------------------------------------------------------------------------------

// const int NUM_MENU_BUTTONS = 5;
// const int INITIAL_WIN_WIDTH = 800;
// const int INITIAL_WIN_HEIGHT = 600;
// const int NUM_LEVELS = 3;

// // enum GameState {
// //   STATE_MENU,
// //   STATE_INSTRUCTIONS,
// //   STATE_INSTRUCTIONS_3D,
// //   STATE_LEVEL_SELECT,
// //   STATE_LEVEL_SELECT_3D,
// //   STATE_GAME,
// //   STATE_GAME_3D
// // };

// enum GameState {
//   STATE_MODE_SELECT,       // O menu principal (5 botões: 2D, 3D, Instruções 2D/3D, Sair)
//   STATE_MENU_2D,             // NOVO: Sub-menu 2D (Iniciar, Como Jogar, Voltar)
//   STATE_MENU_3D,             // NOVO: Sub-menu 3D (Iniciar, Como Jogar, Voltar)
//   STATE_INSTRUCTIONS_2D, // Renomeado
//   STATE_INSTRUCTIONS_3D,
//   STATE_LEVEL_SELECT_2D, // Renomeado
//   STATE_LEVEL_SELECT_3D,
//   STATE_GAME_2D,             // Renomeado
//   STATE_GAME_3D
// };

// GameState currentState = STATE_MODE_SELECT;

// // IDs das texturas de menu
// GLuint texMenuBackground = 0;
// GLuint texButtonNormal = 0;
// GLuint texButtonHover = 0;
// GLuint texButtonDisabled = 0;
// GLuint texLevelSelectBackground = 0;
// GLuint texInstructionsBackground = 0;

// // Texturas do Jogador (animação básica)
// GLuint texPlayer = 0;
// GLuint texPlayerRunRight = 0;
// GLuint texPlayerRunLeft = 0;
// GLuint texPlayerJump = 0;
// GLuint texPlayerLose = 0;

// // Texturas do Level 1
// GLuint texBackground = 0;
// GLuint texBackgroundFar = 0;
// GLuint texBackgroundMid = 0;
// GLuint texBackgroundNear = 0;
// GLuint texFloor = 0;
// GLuint texRiver = 0;
// GLuint texPlatform = 0;
// GLuint texObstacleBottom = 0;
// GLuint texDoor = 0;

// GLuint texWinGame = 0;
// GLuint texGameOver = 0;

// GLuint texDisplayGrappler = 0;  // Textura do gancho

// Button menuButtons[NUM_MENU_BUTTONS];  // Botões do menu principal
// Button instrBackButton;  // Boão de voltar contido na tela de instruções
// Button levelButtons[NUM_LEVELS];  // Botões para selecionar qual o level o
//                                   // player quer jogar
// Button levelSelectBackButton;  // Botão de "voltar" contido na seleção de levels
// Button gameBackButton;  // Botão de "voltar" contido dentro do próprio jogo

// MenuOption selectedMenuOption = NONE;
// int maxLevelUnlocked = 1;
// int activeLevel = 1;

// // ------------------------------------------------------------------------------------------------------------------

// // extern void loadTexture();
// void display();
// void reshape(int w, int h);
// void mouseMotion(int x, int y);
// void mouseClick(int button, int state, int x, int y);
// void keyboardUp(unsigned char key, int x, int y);
// void keyboardDown(unsigned char key, int x, int y);
// void timer(int value);

// // ------------------------------------------------------------------------------------------------------------------

// /**
//  * Versão das funções para o jogo 3D
//  */
// void renderInstructions_3D();
// void renderLevelSelect_3D();
// void drawGameHUD_3D();

// extern void gameStartLevel_3D(int level);
// extern void gameDisplay_3D();
// extern void gameReshape_3D(int w, int h);
// extern void gameMouseMotion_3D(int x, int y);
// extern void gameMouseClick_3D(int button, int state);
// extern GameAction gameKeyDown_3D(unsigned char key, int x, int y);
// extern void gameKeyUp_3D(unsigned char key, int x, int y);
// extern void gameSpecialDown_3D(int key, int x, int y);
// extern void gameSpecialUp_3D(int key, int x, int y);
// extern GameAction gameUpdate_3D();
// extern void loadGameTextures_3D();

// // ------------------------------------------------------------------------------------------------------------------

// /**
//  * Carrega todas as texturas do jogo, incluindo as do jogador e do primeiro
//  * nível.
//  */
// void loadGameTextures() {
//   // Texturas do Jogador
//   texPlayer = loadTexture("assets/images/player/player.png");
//   texPlayerRunRight = loadTexture("assets/images/player/player_run_right.png");
//   texPlayerRunLeft = loadTexture("assets/images/player/player_run_left.png");
//   texPlayerJump = loadTexture("assets/images/player/player_jump.png");
//   texPlayerLose = loadTexture("assets/images/player/player_lose.png");
//   texDoor = loadTexture("assets/images/global/door.png");

//   // ------------------------------------------------
//   // CARREGAMENTO DE TEXTURAS POR NÍVEL
//   // ------------------------------------------------
//   std::string levelPath;
//   switch (CURRENT_LEVEL) {
//     case 1:
//       levelPath = "assets/images/level1/";
//       break;
//     case 2:
//       levelPath = "assets/images/level2/";
//       break;
//     case 3:
//       levelPath = "assets/images/level3/";
//       break;
//     default:
//       // Caso não exista a pasta, usa o default (level1)
//       levelPath = "assets/images/level1/";
//       break;
//   }

//   // Texturas do Nível (caminho dinâmico)
//   // TODOS os caminhos agora usam 'levelPath'
//   texBackgroundFar = loadTexture((levelPath + "bg-far.png").c_str());
//   texBackgroundMid = loadTexture((levelPath + "bg-mid.png").c_str());
//   texBackgroundNear = loadTexture((levelPath + "bg-near.png").c_str());

//   texFloor = loadTexture((levelPath + "floor.png").c_str());
//   texRiver = loadTexture((levelPath + "river.png").c_str());
//   texPlatform = loadTexture((levelPath + "complete_platform.png").c_str());
//   texObstacleBottom = loadTexture((levelPath + "obstacle_bottom.png").c_str());
//   texRiver = loadTexture((levelPath + "river.png").c_str());

//   texWinGame = loadTexture("assets/images/global/win_game.png");
//   texGameOver = loadTexture("assets/images/global/game_over.png");
//   texDisplayGrappler = loadTexture("assets/images/global/display_grappler.png");
// }

// void renderInstructions() {
//   glClear(GL_COLOR_BUFFER_BIT);
//   // Desenha o fundo de instruções
//   float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
//   float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
//   drawTexturedRect(0, 0, winW, winH, texInstructionsBackground, false,
//                    false);  // Usa o ID da textura de fundo
//   drawText(50, 80, "INSTRUCOES DE COMO JOGAR: ");
//   drawText(50, 120,
//            "- Use 'A' e 'D' ou setas (<- ->) para andar no chao. Nao e "
//            "possivel pular");
//   drawText(50, 150, "- Mire com o mouse para onde quer disparar o gancho");
//   drawText(50, 180,
//            "- Clique e segure para disparar o gancho e, ao ser preso, solte "
//            "para desprende-lo");
//   drawText(
//       50, 210,
//       "- Observe os valores no canto superior direito para se guiar melhor");
//   drawText(50, 240, "- Haverao diversos obstaculos. Dentre eles:");
//   drawText(100, 270,
//            "- Plataformas para andar por cima e/ou se prender com o gancho.");
//   drawText(100, 300,
//            "- Correntes de vento que podem mudar a direcao do seu lancamento");
//   drawText(100, 330, "- Espinhos fatais (aconselhamos a nao toca-los)");
//   drawText(50, 390, "Seu objetivo e chegar ate a porta final =)");
//   drawText(50, 420, "Economize disparos! Boa sorte!");

//   drawButton(instrBackButton);
//   glutSwapBuffers();
// }

// void renderLevelSelect() {
//   glClear(GL_COLOR_BUFFER_BIT);
//   // Desenha o fundo de seleção de níveis
//   float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
//   float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
//   drawTexturedRect(0, 0, winW, winH, texLevelSelectBackground, false,
//                    false);  // Usa o ID da textura de fundo
//   drawText(50, 80, "Escolha a fase");
//   for (int i = 0; i < NUM_LEVELS; i++) {
//     drawButton(levelButtons[i]);
//   }
//   drawButton(levelSelectBackButton);
//   glutSwapBuffers();
// }

// void drawGameHUD() {
//   glMatrixMode(GL_PROJECTION);
//   glPushMatrix();
//   glLoadIdentity();
//   gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
//   glMatrixMode(GL_MODELVIEW);
//   glPushMatrix();
//   glLoadIdentity();
//   drawButton(gameBackButton);
//   glMatrixMode(GL_PROJECTION);
//   glPopMatrix();
//   glMatrixMode(GL_MODELVIEW);
//   glPopMatrix();
// }

// // ----------------------------------------------------------------------------------------------------

// void renderInstructions_3D() {
//   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Necessário o DEPTH para 3D
//     // ... Desenha o fundo, o botão Voltar (instrBackButton)

//     drawText(50, 80, "INSTRUCOES DO MUNDO 3D: ");
//     drawText(50, 120, "- Use 'W', 'A', 'S', 'D' para movimentacao no plano horizontal.");
//     drawText(50, 150, "- Use o mouse para controlar a camera.");
//     drawText(50, 180, "- O gancho (grappling hook) e lancado com o botao esquerdo do mouse.");
//     drawText(50, 210, "- Se prender a objetos o puxara na direcao, mas a fisica 3D e mais complexa!");
//     drawText(50, 240, "- O objetivo e encontrar a saida no espaco 3D.");

//     drawButton(instrBackButton);
//     glutSwapBuffers();
// }

// void renderLevelSelect_3D() {
//     glClear(GL_COLOR_BUFFER_BIT);
//     // Desenha o fundo de seleção de níveis
//     float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
//     float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
//     drawTexturedRect(0, 0, winW, winH, texLevelSelectBackground, false, false);

//     // Assumindo que você usa os mesmos botões de nível por enquanto
//     drawText(50, 80, "Escolha a fase 3D");
//     for (int i = 0; i < NUM_LEVELS; i++) {
//         drawButton(levelButtons[i]);
//     }
//     drawButton(levelSelectBackButton);
//     glutSwapBuffers();
// }

// void drawGameHUD_3D() {
//   // Você pode chamar a versão 2D se o HUD for o mesmo, ou refazer a lógica.
//     drawGameHUD();
// }

// // ----------------------------------------------------------------------------------------------------

// /**
//  * Essa função é responsável por verificar o estado do jogo e, de acordo com o
//  * valor da constante global, chama as funções de renderização (desenho dos
//  * elementos na tela)
//  */
// void display() {
//   glClearColor(0.9f, 0.9f, 0.9f, 1.0f);  // Cor de fundo padrão

//   switch (currentState) {
//     case STATE_MENU:
//       renderMenu(menuButtons, NUM_MENU_BUTTONS);
//       break;
//     case STATE_INSTRUCTIONS:
//       renderInstructions();
//       break;
//     case STATE_INSTRUCTIONS_3D:
//       renderInstructions_3D();
//       break;
//     case STATE_LEVEL_SELECT:
//       renderLevelSelect();
//       break;
//     case STATE_LEVEL_SELECT_3D:
//       renderLevelSelect_3D();
//       break;
//     case STATE_GAME:
//       gameDisplay();
//       drawGameHUD();
//       glutSwapBuffers();
//       break;
//     case STATE_GAME_3D:
//       gameDisplay_3D();
//       drawGameHUD_3D();
//       glutSwapBuffers();
//       break;
//   }
// }

// /**
//  * Essa função tem um comportamento similar ao "reshape". Ela atualiza a posição
//  * de todos os botões na tela de acordo com o o dimensionamento da janela do
//  * freeGlut
//  */
// void updateAllButtonPositions(int w, int h) {
//   /**
//    * Parâmetros padrões de configuração para os botões (margem, espaçamento etc)
//    */
//   float buttonWidth = 220.0f;
//   float buttonHeight = 50.0f;
//   float spacing = 20.0f;
//   float margin = 20.0f;
//   float startX = (w - buttonWidth) / 2.0f;

//   float totalMenuH =
//       (NUM_MENU_BUTTONS * buttonHeight) + ((NUM_MENU_BUTTONS - 1) * spacing);
//   float startMenuY = (h - totalMenuH) / 2.0f;
//   for (int i = 0; i < NUM_MENU_BUTTONS; ++i) {
//     menuButtons[i].x = startX;
//     menuButtons[i].y = startMenuY + i * (buttonHeight + spacing);
//   }

//   instrBackButton.x = margin;
//   instrBackButton.y = h - buttonHeight - margin;

//   float totalLevelH =
//       (NUM_LEVELS * buttonHeight) + ((NUM_LEVELS - 1) * spacing);
//   float startLevelY = (h - totalLevelH) / 2.0f;
//   for (int i = 0; i < NUM_LEVELS; i++) {
//     levelButtons[i].x = startX;
//     levelButtons[i].y = startLevelY + i * (buttonHeight + spacing);
//     levelButtons[i].enabled = (i + 1 <= maxLevelUnlocked);
//   }

//   levelSelectBackButton.x = margin;
//   levelSelectBackButton.y = h - buttonHeight - margin;

//   gameBackButton.x = margin;
//   gameBackButton.y = margin;
// }

// /**
//  * Para adaptar o reshape, separamos entre o reshape do game ou o do menu (visto
//  * que ele só tem botões)
//  */
// void reshape(int w, int h) {
//   if (h == 0) h = 1;

//   if (currentState == STATE_GAME) {
//     gameReshape(w, h);
//   } else {
//     glViewport(0, 0, w, h);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluOrtho2D(0, w, h, 0);
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();
//   }

//   updateAllButtonPositions(w, h);
// }

// /**
//  * O callback que será passado para a máquina de estados do freeGlut é o da
//  * main, no entanto o jogo cadastra e controla a sua própria função de controle
//  * do mouse. A main apenas controla questões de visualização dos botões (hover e
//  * ativação)
//  */
// void mouseMotion(int x, int y) {
//   if (currentState == STATE_GAME) {
//     gameMouseMotion(x, y);
//     gameBackButton.hovered =
//         (x >= gameBackButton.x && x <= gameBackButton.x + gameBackButton.w &&
//          y >= gameBackButton.y && y <= gameBackButton.y + gameBackButton.h);
//   } else {
//     if (currentState == STATE_MENU) {
//       for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
//         menuButtons[i].hovered =
//             (x >= menuButtons[i].x &&
//              x <= menuButtons[i].x + menuButtons[i].w &&
//              y >= menuButtons[i].y && y <= menuButtons[i].y + menuButtons[i].h);
//       }
//     } else if (currentState == STATE_INSTRUCTIONS) {
//       instrBackButton.hovered = (x >= instrBackButton.x &&
//                                  x <= instrBackButton.x + instrBackButton.w &&
//                                  y >= instrBackButton.y &&
//                                  y <= instrBackButton.y + instrBackButton.h);
//     } else if (currentState == STATE_LEVEL_SELECT) {
//       for (int i = 0; i < NUM_LEVELS; i++) {
//         if (levelButtons[i].enabled) {
//           levelButtons[i].hovered =
//               (x >= levelButtons[i].x &&
//                x <= levelButtons[i].x + levelButtons[i].w &&
//                y >= levelButtons[i].y &&
//                y <= levelButtons[i].y + levelButtons[i].h);
//         }
//       }
//       levelSelectBackButton.hovered =
//           (x >= levelSelectBackButton.x &&
//            x <= levelSelectBackButton.x + levelSelectBackButton.w &&
//            y >= levelSelectBackButton.y &&
//            y <= levelSelectBackButton.y + levelSelectBackButton.h);
//     }
//   }

//   glutPostRedisplay();
// }

// /**
//  * Assim como a callback de movumento do mouse tem o seu controle dividido entre
//  * a main e as callback do game, a callback de controle de clique do mouse
//  * também tem o seu controle separado. Assim como explicado em sala, a função de
//  * mouse é chamada sempre que o usuário pressiona (GLUT_DOWN) ou solta (GLUT_UP)
//  * o botão do mouse. As callbacks do game controlam apenas as questões de
//  * jogabilidade e a main é responsável por controlar os botões de voltar e
//  * outras coisas na tela (ainda que esteja em modo de jogo).
//  */
// void mouseClick(int button, int state, int x, int y) {
//   if (button == GLUT_LEFT_BUTTON) {
//     if (currentState == STATE_GAME) {
//       if (state == GLUT_DOWN && gameBackButton.hovered) {
//         currentState = STATE_LEVEL_SELECT;
//         reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
//       } else {
//         gameMouseClick(button, state);
//       }
//     } else if (state == GLUT_DOWN) {
//       if (currentState == STATE_MENU) {
//         selectedMenuOption =
//             handleMenuInput(x, y, menuButtons, NUM_MENU_BUTTONS);
//         switch (selectedMenuOption) {
//           case START_GAME:
//             currentState = STATE_LEVEL_SELECT;
//             break;
//           case HOW_TO_PLAY:
//             currentState = STATE_INSTRUCTIONS;
//             break;
//           case EXIT_GAME:
//             glutLeaveMainLoop();
//             break;
//           default:
//             break;
//         }
//       } else if (currentState == STATE_INSTRUCTIONS) {
//         if (instrBackButton.hovered) {
//           currentState = STATE_MENU;
//         }
//       } else if (currentState == STATE_LEVEL_SELECT) {
//         for (int i = 0; i < NUM_LEVELS; i++) {
//           if (levelButtons[i].hovered && levelButtons[i].enabled) {
//             activeLevel = i + 1;
//             currentState = STATE_GAME;
//             gameStartLevel(activeLevel);
//             loadGameTextures();
//             glutTimerFunc(16, timer, 0);
//             break;
//           }
//         }
//         if (levelSelectBackButton.hovered) {
//           currentState = STATE_MENU;
//         }
//       }

//       if (currentState != STATE_GAME) {
//         reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
//       }
//     }
//   }
//   glutPostRedisplay();
// }

// // ------------------------------------------------------------------------------------------------------------------------

// void keyboardDown(unsigned char key, int x, int y) {
//   if (currentState == STATE_GAME) {
//     GameAction action = gameKeyDown(key, x, y);
//     if (action == GAME_ACTION_EXIT_TO_MENU) {
//       currentState = STATE_LEVEL_SELECT;
//       reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
//       glutPostRedisplay();
//     }
//   }
// }

// void keyboardUp(unsigned char key, int x, int y) {
//   if (currentState == STATE_GAME) {
//     gameKeyUp(key, x, y);
//   }
// }

// void specialDown(int key, int x, int y) {
//   if (currentState == STATE_GAME) {
//     gameSpecialDown(key, x, y);
//   }
// }

// void specialUp(int key, int x, int y) {
//   if (currentState == STATE_GAME) {
//     gameSpecialUp(key, x, y);
//   }
// }

// /**
//  * Controla toda a continuidade do game. É responsável por chamar as funções de
//  * atualização do jogo e verificar os estados retornados; com isso, essa função
//  * trata os desbloqueios de fase e os estados que irão proceder após algum
//  * retorno do game
//  */
// void timer(int value) {
//   if (currentState != STATE_GAME) {
//     return;
//   }

//   GameAction action = gameUpdate();

//   if (action == GAME_ACTION_LEVEL_WON) {
//     if (activeLevel == maxLevelUnlocked && maxLevelUnlocked < NUM_LEVELS) {
//       maxLevelUnlocked++;
//     }
//     currentState = STATE_LEVEL_SELECT;
//     reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
//   } else if (action == GAME_ACTION_LEVEL_LOST) {
//     currentState = STATE_LEVEL_SELECT;
//     reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
//   } else {
//     glutTimerFunc(16, timer, 0);  // Faz o reagendamento da função em 60 FPS
//   }

//   glutPostRedisplay();
// }

// void init() {
//   // Botões do Menu Principal
//   // menuButtons[0] = {0, 0, 220, 50, "Iniciar Jogo", false, true};
//   // menuButtons[1] = {0, 0, 220, 50, "Como Jogar", false, true};
//   // menuButtons[2] = {0, 0, 220, 50, "Sair", false, true};

//   menuButtons[0] = {0, 0, 220, 50, "Iniciar Jogo 2D", false, true};
//   menuButtons[1] = {0, 0, 220, 50, "Iniciar Jogo 3D", false, true};
//   menuButtons[2] = {0, 0, 220, 50, "Instruções 2D", false, true};
//   menuButtons[3] = {0, 0, 220, 50, "Instruções 3D", false, true};
//   menuButtons[4] = {0, 0, 220, 50, "Sair", false, true};

//   // Botão Voltar (na tela de instruções)
//   instrBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

//   // Botões de Seleção de Fase
//   levelButtons[0] = {0, 0, 220, 50, "Fase 1", false, false};
//   levelButtons[1] = {0, 0, 220, 50, "Fase 2", false, false};
//   levelButtons[2] = {0, 0, 220, 50, "Fase 3", false, false};

//   // Botão Voltar (na seleção de fases)
//   levelSelectBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

//   // Botão Voltar (no jogo)
//   gameBackButton = {0, 0, 100, 35, "Voltar", false, true};

//   updateAllButtonPositions(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
//   gameStartLevel(activeLevel);

//   // Carrega as texturas de menu
//   texMenuBackground = loadTexture("assets/images/menu/background_menu.png");

//   // Carrega a textura de seleção de níveis
//   texLevelSelectBackground =
//       loadTexture("assets/images/menu/background_level_select.png");

//   // Carrega a textura de instruções
//   texInstructionsBackground =
//       loadTexture("assets/images/menu/background_instructions.png");

//   // Carrega as texturas dos botões
//   texButtonNormal = loadTexture("assets/images/menu/button_normal.png");
//   texButtonHover = loadTexture("assets/images/menu/button_hover.png");
//   texButtonDisabled = loadTexture("assets/images/menu/button_disabled.png");

//   // Carrega as texturas do jogo
//   loadGameTextures();
// }

// int main(int argc, char** argv) {
//   glutInit(&argc, argv);
//   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//   glutInitWindowSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
//   glutInitWindowPosition(100, 100);
//   glutCreateWindow("Gancho");

//   init();

//   glutDisplayFunc(display);
//   glutReshapeFunc(reshape);
//   glutMouseFunc(mouseClick);
//   glutPassiveMotionFunc(mouseMotion);
//   glutMotionFunc(mouseMotion);

//   glutKeyboardFunc(keyboardDown);
//   glutKeyboardUpFunc(keyboardUp);
//   glutSpecialFunc(specialDown);
//   glutSpecialUpFunc(specialUp);

//   glutMainLoop();
//   return 0;
// }

/**
 * Para a segunda entrega do trabalho, optamos por fazer uma lógica separada para o jogo 3D e controlar os estados do jogo
 * e as atualizações pela main. Isso irá facilitar a manutenção e evitar retrabalho de refatoração nas funções já criadas para 
 * o jogo 2D. Algumas funções tiveram de ser renomeadas para fins de visualização e organização.
 */

#include <GL/freeglut.h>
#include <stdio.h>
#include <string>

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

enum GameState
{
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

GLuint texDisplayGrappler = 0; // Textura do gancho

Button modeSelectButtons[NUM_MODE_SELECT_BUTTONS]; // Botões do menu principal (2D, 3D, etc.)
Button menu2DButtons[NUM_SUB_MENU_BUTTONS]; // Botões do sub-menu 2D
Button menu3DButtons[NUM_SUB_MENU_BUTTONS]; // Botões do sub-menu 3D

Button instrBackButton; // Boão de voltar contido na tela de instruções
Button levelButtons[NUM_LEVELS]; // Botões para selecionar qual o level o player quer jogar
Button levelSelectBackButton; // Botão de "voltar" contido na seleção de levels
Button gameBackButton; // Botão de "voltar" contido dentro do próprio jogo

MenuOption selectedMenuOption = NONE;
int maxLevelUnlocked = 1;
int activeLevel = 1;
int activeLevel_3D = 1;

// ------------------------------------------------------------------------------------------------------------------

// Callbacks
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
 * Versões das funções para o 2D (nomes mantidos) e para o 3D. Optamos por manter o mesmo padrão de nomes
 * para facilitar a codificação
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

// Funções 3D (nomes _3D)
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
void loadGameTextures()
{
  // Texturas do Jogador
  texPlayer = loadTexture("assets/images/player/player.png");
  texPlayerRunRight = loadTexture("assets/images/player/player_run_right.png");
  texPlayerRunLeft = loadTexture("assets/images/player/player_run_left.png");
  texPlayerJump = loadTexture("assets/images/player/player_jump.png");
  texPlayerLose = loadTexture("assets/images/player/player_lose.png");
  texDoor = loadTexture("assets/images/global/door.png");

  std::string levelPath;
  switch (CURRENT_LEVEL)
  {
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

  texBackgroundFar = loadTexture((levelPath + "bg-far.png").c_str());
  texBackgroundMid = loadTexture((levelPath + "bg-mid.png").c_str());
  texBackgroundNear = loadTexture((levelPath + "bg-near.png").c_str());
  texFloor = loadTexture((levelPath + "floor.png").c_str());
  texRiver = loadTexture((levelPath + "river.png").c_str());
  texPlatform = loadTexture((levelPath + "complete_platform.png").c_str());
  texObstacleBottom = loadTexture((levelPath + "obstacle_bottom.png").c_str());
  texWinGame = loadTexture("assets/images/global/win_game.png");
  texGameOver = loadTexture("assets/images/global/game_over.png");
  texDisplayGrappler = loadTexture("assets/images/global/display_grappler.png");
}

void renderInstructions_2D()
{
  glClear(GL_COLOR_BUFFER_BIT);
  float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
  float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
  drawTexturedRect(0, 0, winW, winH, texInstructionsBackground, false,
                   false);
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

void renderLevelSelect_2D()
{
  glClear(GL_COLOR_BUFFER_BIT);
  float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
  float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
  drawTexturedRect(0, 0, winW, winH, texLevelSelectBackground, false,
                   false);
  drawText(50, 80, "Escolha a fase (2D)");
  for (int i = 0; i < NUM_LEVELS; i++)
  {
    drawButton(levelButtons[i]);
  }
  drawButton(levelSelectBackButton);
  glutSwapBuffers();
}

void drawGameHUD_2D()
{
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

void renderInstructions_3D()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Necessário o DEPTH para 3D

  float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
  float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
  drawTexturedRect(0, 0, winW, winH, texInstructionsBackground, false, false);

  drawText(50, 80, "INSTRUCOES DO MUNDO 3D: ");
  drawText(50, 120, "- Use 'W', 'A', 'S', 'D' para movimentacao no plano horizontal.");
  drawText(50, 150, "- Use o mouse para controlar a camera.");
  drawText(50, 180, "- O gancho (grappling hook) e lancado com o botao esquerdo do mouse.");
  drawText(50, 210, "- Se prender a objetos o puxara na direcao, mas a fisica 3D e mais complexa!");
  drawText(50, 240, "- O objetivo e encontrar a saida no espaco 3D.");

  drawButton(instrBackButton);
  glutSwapBuffers();
}

void renderLevelSelect_3D()
{
  glClear(GL_COLOR_BUFFER_BIT);
  float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
  float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
  drawTexturedRect(0, 0, winW, winH, texLevelSelectBackground, false, false);

  drawText(50, 80, "Escolha a fase 3D");
  for (int i = 0; i < NUM_LEVELS; i++)
  {
    drawButton(levelButtons[i]);
  }
  drawButton(levelSelectBackButton);
  glutSwapBuffers();
}

void drawGameHUD_3D()
{
  drawGameHUD_2D();
}

// ----------------------------------------------------------------------------------------------------

/**
 * Agora a função de display é uma máquina de estados mais complexa para controlar qual o 
 * tipo de jogo que está sendo jogado
 */
void display()
{
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

  switch (currentState)
  {
  case STATE_MODE_SELECT:
    renderMenu(modeSelectButtons, NUM_MODE_SELECT_BUTTONS);
    break;
  case STATE_MENU_2D:
    // Renderiza o sub-menu 2D de 3 botões
    renderMenu(menu2DButtons, NUM_SUB_MENU_BUTTONS);
    break;
  case STATE_MENU_3D:
    // Renderiza o sub-menu 3D de 3 botões
    renderMenu(menu3DButtons, NUM_SUB_MENU_BUTTONS);
    break;
  case STATE_INSTRUCTIONS_2D:
    renderInstructions_2D();
    break;
  case STATE_INSTRUCTIONS_3D:
    renderInstructions_3D();
    break;
  case STATE_LEVEL_SELECT_2D:
    renderLevelSelect_2D();
    break;
  case STATE_LEVEL_SELECT_3D:
    renderLevelSelect_3D();
    break;
  case STATE_GAME_2D:
    gameDisplay();
    drawGameHUD_2D();
    glutSwapBuffers();
    break;
  case STATE_GAME_3D:
    gameDisplay_3D();
    drawGameHUD_3D();
    glutSwapBuffers();
    break;
  }
}

/**
 * Atualiza a posição de TODOS os botões
 */
void updateAllButtonPositions(int w, int h)
{
  float buttonWidth = 220.0f;
  float buttonHeight = 50.0f;
  float spacing = 20.0f;
  float margin = 20.0f;
  float startX = (w - buttonWidth) / 2.0f;

  float totalModeSelectH =
      (NUM_MODE_SELECT_BUTTONS * buttonHeight) + ((NUM_MODE_SELECT_BUTTONS - 1) * spacing);
  float startModeSelectY = (h - totalModeSelectH) / 2.0f;
  for (int i = 0; i < NUM_MODE_SELECT_BUTTONS; ++i)
  {
    modeSelectButtons[i].x = startX;
    modeSelectButtons[i].y = startModeSelectY + i * (buttonHeight + spacing);
  }

  float totalSubMenuH =
      (NUM_SUB_MENU_BUTTONS * buttonHeight) + ((NUM_SUB_MENU_BUTTONS - 1) * spacing);
  float startSubMenuY = (h - totalSubMenuH) / 2.0f;
  for (int i = 0; i < NUM_SUB_MENU_BUTTONS; i++)
  {
    // Ambos os menus 2D e 3D podem compartilhar as mesmas coordenadas pois nunca são exibidos ao mesmo tempo.
    menu2DButtons[i].x = startX;
    menu2DButtons[i].y = startSubMenuY + i * (buttonHeight + spacing);
    menu3DButtons[i].x = startX;
    menu3DButtons[i].y = startSubMenuY + i * (buttonHeight + spacing);
  }

  for (int i = 0; i < NUM_LEVELS; i++)
  {
    levelButtons[i].x = startX;
    levelButtons[i].y = startSubMenuY + i * (buttonHeight + spacing);
    levelButtons[i].enabled = (i + 1 <= maxLevelUnlocked);
  }

  instrBackButton.x = margin;
  instrBackButton.y = h - buttonHeight - margin;

  levelSelectBackButton.x = margin;
  levelSelectBackButton.y = h - buttonHeight - margin;

  gameBackButton.x = margin;
  gameBackButton.y = margin;
}

/**
 * Reshape adaptado para as duas versões de jogo
 */
void reshape(int w, int h)
{
  if (h == 0)
    h = 1;

  if (currentState == STATE_GAME_2D)
  {
    gameReshape(w, h);
  }
  else if (currentState == STATE_GAME_3D)
  {
    gameReshape_3D(w, h);
  }
  else
  {
    // Todos os menus são 2D
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
void mouseMotion(int x, int y)
{
  if (currentState == STATE_GAME_2D)
  {
    gameMouseMotion(x, y);
    gameBackButton.hovered =
        (x >= gameBackButton.x && x <= gameBackButton.x + gameBackButton.w &&
         y >= gameBackButton.y && y <= gameBackButton.y + gameBackButton.h);
  }
  else if (currentState == STATE_GAME_3D)
  {
    gameMouseMotion_3D(x, y);
    gameBackButton.hovered =
        (x >= gameBackButton.x && x <= gameBackButton.x + gameBackButton.w &&
         y >= gameBackButton.y && y <= gameBackButton.y + gameBackButton.h);
  }
  else
  {
    // Lógica de hover para todos os menus
    if (currentState == STATE_MODE_SELECT)
    {
      for (int i = 0; i < NUM_MODE_SELECT_BUTTONS; i++)
      {
        modeSelectButtons[i].hovered =
            (x >= modeSelectButtons[i].x &&
             x <= modeSelectButtons[i].x + modeSelectButtons[i].w &&
             y >= modeSelectButtons[i].y && y <= modeSelectButtons[i].y + modeSelectButtons[i].h);
      }
    }
    else if (currentState == STATE_MENU_2D)
    {
      for (int i = 0; i < NUM_SUB_MENU_BUTTONS; i++)
      {
        menu2DButtons[i].hovered =
            (x >= menu2DButtons[i].x &&
             x <= menu2DButtons[i].x + menu2DButtons[i].w &&
             y >= menu2DButtons[i].y && y <= menu2DButtons[i].y + menu2DButtons[i].h);
      }
    }
    else if (currentState == STATE_MENU_3D)
    {
      for (int i = 0; i < NUM_SUB_MENU_BUTTONS; i++)
      {
        menu3DButtons[i].hovered =
            (x >= menu3DButtons[i].x &&
             x <= menu3DButtons[i].x + menu3DButtons[i].w &&
             y >= menu3DButtons[i].y && y <= menu3DButtons[i].y + menu3DButtons[i].h);
      }
    }
    else if (currentState == STATE_INSTRUCTIONS_2D || currentState == STATE_INSTRUCTIONS_3D)
    {
      instrBackButton.hovered = (x >= instrBackButton.x &&
                                 x <= instrBackButton.x + instrBackButton.w &&
                                 y >= instrBackButton.y &&
                                 y <= instrBackButton.y + instrBackButton.h);
    }
    else if (currentState == STATE_LEVEL_SELECT_2D || currentState == STATE_LEVEL_SELECT_3D)
    {
      for (int i = 0; i < NUM_LEVELS; i++)
      {
        if (levelButtons[i].enabled)
        {
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
 * Mouse click adaptado para a nova máquina de estados
 */
void mouseClick(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON)
  {

    if (currentState == STATE_GAME_2D)
    {
      if (state == GLUT_DOWN && gameBackButton.hovered)
      {
        currentState = STATE_LEVEL_SELECT_2D;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
      }
      else
      {
        gameMouseClick(button, state);
      }
    }
    else if (currentState == STATE_GAME_3D)
    {
      if (state == GLUT_DOWN && gameBackButton.hovered)
      {
        currentState = STATE_LEVEL_SELECT_3D;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
      }
      else
      {
        gameMouseClick_3D(button, state);
      }
    }
    else if (state == GLUT_DOWN)
    {

      switch (currentState)
      {

      case STATE_MODE_SELECT:
        selectedMenuOption = handleMenuInput(x, y, modeSelectButtons, NUM_MODE_SELECT_BUTTONS);
        switch (selectedMenuOption)
        {
        case 1:
          currentState = STATE_MENU_2D;
          break;
        case 2:
          currentState = STATE_MENU_3D; 
          break;
        case 3: // "Sair"
          glutLeaveMainLoop();
          break;
        }
        break;

      case STATE_MENU_2D:
        selectedMenuOption = handleMenuInput(x, y, menu2DButtons, NUM_SUB_MENU_BUTTONS);
        switch (selectedMenuOption)
        {
        case 1:
          currentState = STATE_LEVEL_SELECT_2D;
          break;
        case 2:
          currentState = STATE_INSTRUCTIONS_2D;
          break;
        case 3:
          currentState = STATE_MODE_SELECT;
          break;
        }
        break;

      case STATE_MENU_3D:
        selectedMenuOption = handleMenuInput(x, y, menu3DButtons, NUM_SUB_MENU_BUTTONS);
        switch (selectedMenuOption)
        {
        case 1:
          currentState = STATE_LEVEL_SELECT_3D;
          break;
        case 2:
          currentState = STATE_INSTRUCTIONS_3D;
          break;
        case 3:
          currentState = STATE_MODE_SELECT;
          break;
        }
        break;

      case STATE_INSTRUCTIONS_2D:
        if (instrBackButton.hovered)
        {
          currentState = STATE_MENU_2D;
        }
        break;

      case STATE_INSTRUCTIONS_3D:
        if (instrBackButton.hovered)
        {
          currentState = STATE_MENU_3D;
        }
        break;

      case STATE_LEVEL_SELECT_2D:
        for (int i = 0; i < NUM_LEVELS; i++)
        {
          if (levelButtons[i].hovered && levelButtons[i].enabled)
          {
            activeLevel = i + 1;
            currentState = STATE_GAME_2D;
            gameStartLevel(activeLevel);
            loadGameTextures();
            glutTimerFunc(16, timer, 0);
            break;
          }
        }
        if (levelSelectBackButton.hovered)
        {
          currentState = STATE_MENU_2D;
        }
        break;

      case STATE_LEVEL_SELECT_3D:
        for (int i = 0; i < NUM_LEVELS; i++)
        {
          if (levelButtons[i].hovered && levelButtons[i].enabled)
          {
            activeLevel_3D = i + 1;
            currentState = STATE_GAME_3D;
            gameStartLevel_3D(activeLevel_3D);
            loadGameTextures_3D();
            glutTimerFunc(16, timer, 0);
            break;
          }
        }
        if (levelSelectBackButton.hovered)
        {
          currentState = STATE_MENU_3D;
        }
        break;
      }

      if (currentState != STATE_GAME_2D && currentState != STATE_GAME_3D)
      {
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
      }
    }
  }
  glutPostRedisplay();
}

// ------------------------------------------------------------------------------------------------------------------------

void keyboardDown(unsigned char key, int x, int y)
{
  if (currentState == STATE_GAME_2D)
  {
    GameAction action = gameKeyDown(key, x, y);
    if (action == GAME_ACTION_EXIT_TO_MENU)
    {
      currentState = STATE_LEVEL_SELECT_2D;
      reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
      glutPostRedisplay();
    }
  }
  else if (currentState == STATE_GAME_3D)
  {
    GameAction action = gameKeyDown_3D(key, x, y);
    if (action == GAME_ACTION_EXIT_TO_MENU)
    {
      currentState = STATE_LEVEL_SELECT_3D;
      reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
      glutPostRedisplay();
    }
  }
}

void keyboardUp(unsigned char key, int x, int y)
{
  if (currentState == STATE_GAME_2D)
  {
    gameKeyUp(key, x, y);
  }
  else if (currentState == STATE_GAME_3D)
  {
    gameKeyUp_3D(key, x, y);
  }
}

void specialDown(int key, int x, int y)
{
  if (currentState == STATE_GAME_2D)
  {
    gameSpecialDown(key, x, y);
  }
  else if (currentState == STATE_GAME_3D)
  {
    gameSpecialDown_3D(key, x, y);
  }
}

void specialUp(int key, int x, int y)
{
  if (currentState == STATE_GAME_2D)
  {
    gameSpecialUp(key, x, y);
  }
  else if (currentState == STATE_GAME_3D)
  {
    gameSpecialUp_3D(key, x, y);
  }
}

/**
 * Timer adaptado para 2D e 3D
 */
void timer(int value)
{
  // Se não estivermos em nenhum estado de jogo, pare o timer
  if (currentState != STATE_GAME_2D && currentState != STATE_GAME_3D)
  {
    return;
  }

  GameAction action;
  GameState originalState = currentState; // Guarda o estado atual (2D ou 3D)

  // Chama a função de update correta
  if (currentState == STATE_GAME_2D)
  {
    action = gameUpdate();
  }
  else
  {
    action = gameUpdate_3D();
  }

  // Lida com a ação retornada
  if (action == GAME_ACTION_LEVEL_WON)
  {
    if (activeLevel == maxLevelUnlocked && maxLevelUnlocked < NUM_LEVELS)
    {
      maxLevelUnlocked++;
    }
    // Volta para o menu de seleção de nível correto
    currentState = (originalState == STATE_GAME_2D) ? STATE_LEVEL_SELECT_2D : STATE_LEVEL_SELECT_3D;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  }
  else if (action == GAME_ACTION_LEVEL_LOST)
  {
    // Volta para o menu de seleção de nível correto
    currentState = (originalState == STATE_GAME_2D) ? STATE_LEVEL_SELECT_2D : STATE_LEVEL_SELECT_3D;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  }
  else
  {
    // Se o jogo continua, agenda o próximo timer
    glutTimerFunc(16, timer, 0);
  }

  glutPostRedisplay();
}

void init()
{
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
  gameStartLevel(activeLevel); // Pré-carrega o nível 1 2D

  // Carrega as texturas de menu
  texMenuBackground = loadTexture("assets/images/menu/background_menu.png");
  texLevelSelectBackground =
      loadTexture("assets/images/menu/background_level_select.png");
  texInstructionsBackground =
      loadTexture("assets/images/menu/background_instructions.png");

  // Carrega as texturas dos botões
  texButtonNormal = loadTexture("assets/images/menu/button_normal.png");
  texButtonHover = loadTexture("assets/images/menu/button_hover.png");
  texButtonDisabled = loadTexture("assets/images/menu/button_disabled.png");

  // Carrega as texturas do jogo 2D (pré-carregamento)
  loadGameTextures();

}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Gancho");

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