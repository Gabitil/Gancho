#ifndef MENU_H
#define MENU_H

#include <GL/freeglut.h>

enum MenuOption {
    NONE,
    START_GAME,
    HOW_TO_PLAY,
    EXIT_GAME // Mudei de BACK_TO_MENU para EXIT_GAME para corresponder ao label "Sair"
};

struct Button {
    float x, y, w, h;
    const char* label;
    bool hovered;
};

void renderMenu(Button buttons[], int count);
MenuOption handleMenuInput(int x, int y, Button buttons[], int count);

#endif