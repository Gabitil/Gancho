#include "menu.h"
#include <cstring> // Para strlen, ou podemos usar o método do glutBitmapWidth

// Função auxiliar para desenhar texto
void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Desenha um único botão
void drawButton(const Button& btn) {
    // Cor do botão baseada no estado
    if (!btn.enabled) {
        glColor3f(0.5f, 0.5f, 0.5f); // Cinza para botão desabilitado
    } else if (btn.hovered) {
        glColor3f(0.4f, 0.4f, 0.4f); // Cor de hover
    } else {
        glColor3f(0.2f, 0.2f, 0.2f); // Cor padrão
    }

    // Desenha o retângulo do botão
    glBegin(GL_QUADS);
    glVertex2f(btn.x, btn.y);
    glVertex2f(btn.x + btn.w, btn.y);
    glVertex2f(btn.x + btn.w, btn.y + btn.h);
    glVertex2f(btn.x, btn.y + btn.h);
    glEnd();

    // Cor do texto baseada no estado
    if (!btn.enabled) {
        glColor3f(0.8f, 0.8f, 0.8f); // Cinza claro para texto desabilitado
    } else {
        glColor3f(1.0f, 1.0f, 1.0f); // Texto branco
    }

    // Calcula a posição para centralizar o texto
    int textWidth = 0;
    for (const char* c = btn.label; *c; ++c) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    float textX = btn.x + (btn.w - textWidth) / 2.0f;
    float textY = btn.y + (btn.h / 2.0f) + 6.0f; // Ajuste fino

    glRasterPos2f(textX, textY);
    for (const char* c = btn.label; *c; ++c)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

// Renderiza o menu principal
void renderMenu(Button buttons[], int count) {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < count; i++) {
        drawButton(buttons[i]);
    }

    glutSwapBuffers();
}

// Verifica qual botão do menu foi clicado
MenuOption handleMenuInput(int x, int y, Button buttons[], int count) {
    for (int i = 0; i < count; i++) {
        // Só retorna a opção se o botão estiver habilitado e com hover
        if (buttons[i].enabled &&
            x >= buttons[i].x && x <= buttons[i].x + buttons[i].w &&
            y >= buttons[i].y && y <= buttons[i].y + buttons[i].h) {
            return (MenuOption)(i + 1); // START_GAME = 1, HOW_TO_PLAY = 2...
        }
    }
    return NONE;
}