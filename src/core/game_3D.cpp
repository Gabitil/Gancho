// #include "game.h"

// #include <GL/freeglut.h>
// #include <stdio.h>

// #include "utils.h"
// // #include <math.h>
// #include <algorithm>
// #include <string>
// #include <vector>

// #ifdef _WIN32
// #define _CRT_SECURE_NO_WARNINGS
// #endif

// // ------------------------------------------------------------------------------------------------

// void gameDisplay_3D() {
//     // Implementação inicial para teste
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Incluir DEPTH_BUFFER_BIT é crucial
//     // Lógica de projeção 3D (glMatrixMode(GL_PROJECTION), gluPerspective, etc.)
// }

#include "game_3D.h"
#include <GL/freeglut.h> // Inclui para o básico
#include <stdio.h>

/**
 * ARQUIVO DE STUB (ESQUELETO) PARA O JOGO 3D
 *
 * Estas funções estão vazias e existem apenas para
 * satisfazer o linker. A lógica 3D real será
 * implementada aqui no futuro.
 */

void gameInit_3D() {
    // Lógica de inicialização 3D (pode ficar vazia por enquanto)
    printf("gameInit_3D() chamado.\n");
}

void gameStartLevel_3D(int level) {
    // Lógica para carregar um nível 3D (vazio por enquanto)
     printf("gameStartLevel_3D() chamado para o nivel %d.\n", level);
}

GameAction gameUpdate_3D() {
    // Lógica de física 3D (retorna CONTINUE por enquanto)
    return GAME_ACTION_CONTINUE;
}

void gameDisplay_3D() {
    // Lógica de desenho 3D (apenas limpa a tela por enquanto)
    glClearColor(0.1f, 0.1f, 0.3f, 1.0f); // Um fundo azul escuro
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Você precisará configurar uma câmera 3D aqui eventualmente
}

void gameReshape_3D(int w, int h) {
    // Lógica de reshape 3D (configura a projeção de perspectiva)
    if (h == 0) h = 1;
    float ratio = 1.0f * w / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 1000.0f); // Projeção 3D
    glMatrixMode(GL_MODELVIEW);
}

void gameMouseMotion_3D(int x, int y) {
    // Lógica de movimento do mouse 3D (controle de câmera)
}

void gameMouseClick_3D(int button, int state) {
    // Lógica de clique 3D (disparar gancho 3D)
}

GameAction gameKeyDown_3D(unsigned char key, int x, int y) {
    // Lógica de teclado 3D
    if (key == 'q' || key == 'Q') {
        return GAME_ACTION_EXIT_TO_MENU;
    }
    return GAME_ACTION_CONTINUE;
}

void gameKeyUp_3D(unsigned char key, int x, int y) {
    // Lógica de teclado 3D
}

void gameSpecialDown_3D(int key, int x, int y) {
    // Lógica de teclado 3D
}

void gameSpecialUp_3D(int key, int x, int y) {
    // Lógica de teclado 3D
}

void loadGameTextures_3D() {
    // Lógica para carregar texturas 3D
    printf("loadGameTextures_3D() chamado.\n");
}