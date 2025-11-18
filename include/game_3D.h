#ifndef GAME_3D_H
#define GAME_3D_H

// Importa a enumeração de ações do game.h original
#include "game.h" 

void gameInit_3D();
void gameStartLevel_3D(int level);
GameAction gameUpdate_3D();
void gameDisplay_3D();
void gameReshape_3D(int w, int h);
void gameMouseMotion_3D(int x, int y);
void gameMouseClick_3D(int button, int state);
GameAction gameKeyDown_3D(unsigned char key, int x, int y);
void gameKeyUp_3D(unsigned char key, int x, int y);
void gameSpecialDown_3D(int key, int x, int y);
void gameSpecialUp_3D(int key, int x, int y);
void loadGameTextures_3D();

#endif