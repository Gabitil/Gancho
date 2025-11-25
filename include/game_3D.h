#ifndef GAME_3D_H
#define GAME_3D_H

// 1. Tipos Padrão
#include <cstddef>
#include <cstdint>

// 2. GLEW (Sempre antes de FreeGLUT/GL.h)
#include <GL/glew.h>

// 3. FreeGLUT
#include <GL/freeglut.h>

// 4. Headers do Projeto
#include "game.h"
#include "mesh_utils.h"
#include "utils.h"
#include "game_structs.h"

// Bibliotecas padrão
#include <stdio.h>
#include <cmath>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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