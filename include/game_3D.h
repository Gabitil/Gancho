#ifndef GAME_3D_H
#define GAME_3D_H

// Tipos Padrão
#include <cstddef>
#include <cstdint>

// GLEW
#include <GL/glew.h>

// FreeGLUT
#include <GL/freeglut.h>

// Headers ".h"
#include "game.h"
#include "game_structs.h"
#include "mesh_utils.h"
#include "player_anims.h"
#include "utils.h"

// Bibliotecas padrão
#include <stdio.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void gameInit_3D();
void gameStartLevel_3D(int level);
void loadEnvironmentModels(int level);
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
void carregaOBJemTris(const std::string& caminho, std::vector<tri>& out);
extern bool isGrappling_3D;
extern bool isMouseFree_3D;
extern int activeLevel_3D;

#endif