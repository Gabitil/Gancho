#ifndef MENU_H
#define MENU_H

#include <GL/freeglut.h>

// Enum para as opções do menu principal
enum MenuOption {
    NONE,
    START_GAME,
    HOW_TO_PLAY,
    EXIT_GAME
};

// Estrutura do Botão, com estado 'enabled'
struct Button {
    float x, y, w, h;
    const char* label;
    bool hovered;
    bool enabled; // Para o sistema de destravar fases
};

// Funções públicas do módulo de menu
void drawButton(const Button& btn);
void renderMenu(Button buttons[], int count);
MenuOption handleMenuInput(int x, int y, Button buttons[], int count);

// Protótipo da função de desenhar texto
void drawText(float x, float y, const char* text);

#endif