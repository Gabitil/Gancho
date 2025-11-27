/**
 * Assim como em "menu.h", as "structs" e as assinaturas das funções também
 * foram definidas no header do game. Para diferenciar das funções da main,
 * todas as funções de estados do game possuem o prefixo "game". Também foi
 * usada a lógica de #ifndef e #define.
 */

#ifndef GAME_H
#define GAME_H

/**
 * Mesmo que o jogo 2D não use GLEW explicitamente, incluímos para evitar conflitos 
 * se este header for chamado antes de outros visto que o jogo 2D foi desenvolvido antes do 3D. 
 * No entanto, tentamos evitar ao máximo essa possibilidade
 * */ 
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "game_structs.h"

// #include "GL/stb_image.h"

void createDisplayLists();
void gameStartLevel();
void gameStartLevel(int level);
GameAction gameUpdate();
void gameDisplay();
void gameReshape(int w, int h);
void gameMouseMotion(int x, int y);
void gameMouseClick(int button, int state);

GameAction gameKeyDown(unsigned char key, int x, int y);
void gameKeyUp(unsigned char key, int x, int y);
void gameSpecialDown(int key, int x, int y);
void gameSpecialUp(int key, int x, int y);

// Texturas do Jogador (animação básica)
extern GLuint texPlayer;
extern GLuint texPlayerRunRight;
extern GLuint texPlayerRunLeft;
extern GLuint texPlayerJump;
extern GLuint texPlayerLose;

// Texturas dos Levels
extern GLuint texBackground;
extern GLuint texBackgroundFar;
extern GLuint texBackgroundMid;
extern GLuint texBackgroundNear;
extern GLuint texFloor;
extern GLuint texRiver;
extern GLuint texPlatform;
extern GLuint texObstacleBottom;
extern GLuint texDoor;

extern GLuint texWinGame;
extern GLuint texGameOver;

extern GLuint texDisplayGrappler;  // Textura do gancho

struct RiverZone {
  float x, y, w, h;
};

// Define a velocidade da animação (em milissegundos por troca de frame)
const float RUN_ANIMATION_SPEED_MS =
    150.0f;  // Troca de textura a cada 150ms (ajuste conforme necessário)

// Variável que armazena o tempo desde a última troca de frame
extern float runAnimationTimer;

// Variável que indica qual frame de corrida está ativo (0 ou 1)
extern int currentRunFrame;

// Variável de controle do level atual
extern int CURRENT_LEVEL;

#endif