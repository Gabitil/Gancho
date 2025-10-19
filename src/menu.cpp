#include "menu.h"
#include <cstring> // Incluir para glutBitmapLength

void drawButton(const Button& btn) {
    // Cor do botão
    if (btn.hovered)
        glColor3f(0.4f, 0.4f, 0.4f); // Um cinza um pouco mais claro para destaque
    else
        glColor3f(0.2f, 0.2f, 0.2f); // Cinza escuro como cor padrão

    // Desenha o retângulo do botão
    glBegin(GL_QUADS);
    glVertex2f(btn.x, btn.y);
    glVertex2f(btn.x + btn.w, btn.y);
    glVertex2f(btn.x + btn.w, btn.y + btn.h);
    glVertex2f(btn.x, btn.y + btn.h);
    glEnd();

    // Cor do texto
    glColor3f(1.0f, 1.0f, 1.0f); // texto branco

    // Calcula a posição para centralizar o texto
    int textWidth = 0;
    for (const char* c = btn.label; *c; ++c) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    float textX = btn.x + (btn.w - textWidth) / 2.0f;
    float textY = btn.y + (btn.h / 2.0f) + 6.0f; // Ajuste fino para centralização vertical

    glRasterPos2f(textX, textY);
    for (const char* c = btn.label; *c; ++c)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

void renderMenu(Button buttons[], int count) {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < count; i++) {
        drawButton(buttons[i]);
    }

    glutSwapBuffers();
}

MenuOption handleMenuInput(int x, int y, Button buttons[], int count) {
    // A CONVERSÃO DE 'y' FOI REMOVIDA DAQUI
    // int height = glutGet(GLUT_WINDOW_HEIGHT);
    // y = height - y; // <- Esta linha foi o problema do hover invertido

    for (int i = 0; i < count; i++) {
        if (x >= buttons[i].x && x <= buttons[i].x + buttons[i].w &&
            y >= buttons[i].y && y <= buttons[i].y + buttons[i].h) {
            return (MenuOption)(i + 1); // START_GAME = 1, HOW_TO_PLAY = 2...
        }
    }
    return NONE;
}