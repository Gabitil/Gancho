/**
 * Assim como ensinado pelo professor, todos os scripts desse trabalho "exceto a main"
 * possuem um "header" com as assinaturas das funções e as variáveis que serão usadas. 
 * "Structs" para os elementos e variáveis globais. Também usamos a ideia de #ifndef
 * e #define assim como explicado em sala.
 * Todas as funções estão explicadas detalhadamente no script principal ".cpp"
 */

#ifndef MENU_H
#define MENU_H

#include <GL/freeglut.h>

enum MenuOption {
    NONE,
    START_GAME,
    START_GAME_3D,
    HOW_TO_PLAY,
    HOW_TO_PLAY_3D,
    EXIT_GAME
};

struct Button {
    float x, y, w, h;
    const char* label;
    bool hovered;
    bool enabled;
};

void drawButton(const Button& btn);
void renderMenu(Button buttons[], int count);
MenuOption handleMenuInput(int x, int y, Button buttons[], int count);

#endif