// // No novo arquivo game_3d.h
// #ifndef GAME_3D_H
// #define GAME_3D_H

// #include "game.h" // Para reusar GameAction e structs básicas, se quiser

// // Declarações mínimas de funções 3D (para compilar)
// void gameStartLevel_3D(int level);
// GameAction gameUpdate_3D();
// void gameDisplay_3D();
// void gameReshape_3D(int w, int h);
// void gameMouseMotion_3D(int x, int y);
// void gameMouseClick_3D(int button, int state);
// GameAction gameKeyDown_3D(unsigned char key, int x, int y);
// void gameKeyUp_3D(unsigned char key, int x, int y);
// void gameSpecialDown_3D(int key, int x, int y);
// void gameSpecialUp_3D(int key, int x, int y);
// void loadGameTextures_3D(); // Função separada para texturas 3D

// #endif

#ifndef GAME_3D_H
#define GAME_3D_H

// Importa a enumeração de ações do game.h original
#include "game.h" 

// --- Funções de Interface do Jogo 3D ---
// (Estas são as funções que main.cpp espera que existam)

void gameInit_3D(); // Você pode precisar desta
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

#endif // GAME_3D_H