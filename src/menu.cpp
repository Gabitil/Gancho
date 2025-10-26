#include "menu.h"
#include "utils.h"
#include <cstring> 

// ----------------------------------------------------------------------------------------------------------------

/**
 * Os botões do jogo também serão padronizados. 
 * Para implementar o esquema de "mouse sobre o botão" solicitado nos requisitos técnicos
 * do trabalho, foi usada uma variável de controle dentro da definição da estrutura do botão
 * chamada de "hovered" e ela define se o mouse está ou não sobre o botão. Além disso, para 
 * indicar se o botão está disponível para clique, também há a variável "enabled"
 */
void drawButton(const Button& btn) {
    
    if (!btn.enabled) {
        glColor3f(0.5f, 0.5f, 0.5f); // Cor para indicar desabilitado
    } else if (btn.hovered) {
        glColor3f(0.4f, 0.4f, 0.4f); // Cor para indicar "mouse sobre"
    } else {
        glColor3f(0.2f, 0.2f, 0.2f); // Cor padrão do botão (ativado)
    }

    /**
     * Por padrão, será usado um quadrado para o desenho do botão, ou seja, 
     * ele será feito usando apenas uma primitiva geométrica
     */
    glBegin(GL_QUADS);
    glVertex2f(btn.x, btn.y);
    glVertex2f(btn.x + btn.w, btn.y);
    glVertex2f(btn.x + btn.w, btn.y + btn.h);
    glVertex2f(btn.x, btn.y + btn.h);
    glEnd();

    // Cores do texto do botão
    if (btn.enabled) {
        glColor3f(1.0f, 1.0f, 1.0f); // Cor caso esteja ativado
    } else {
        glColor3f(0.8f, 0.8f, 0.8f); // Cor caso esteja desativado
    }

    /**
     * Como tivemos dificuldade para centralizar o texto no botão, buscamos uma lógica
     * auxiliar para calcular onde o texto deve ficar para que, em qualquer desenho de botão
     * ele fique centralizado corretamente
     */
    int textWidth = 0;
    for (const char* caracter = btn.label; *caracter; ++caracter) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *caracter);
    }
    
    float textX = btn.x + (btn.w - textWidth) / 2.0f;
    float textY = btn.y + (btn.h / 2.0f) + 6.0f;

    drawText(textX, textY, btn.label, GLUT_BITMAP_HELVETICA_18);
}

/**
 * Função responsável por desenhar a quantidade de botões solicitada. Para isso, 
 * ela deve limpar o buffer de cor "GL_COLOR_BUFFER_BIT", desenhar os botões e trocar
 * os buffers de desenho para perpetuar as alterações na cena
 */
void renderMenu(Button buttons[], int count) {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < count; i++) {
        drawButton(buttons[i]);
    }
    glutSwapBuffers();
}

/**
 * Para verificar qual botão do menu foi selecionado, a lógica é parecida com a de 
 * colisão de quadrados (amplamente usada em atividades anteriores e nesse trabalho).
 * Ao escolher, será retornada a opção de menu que foi escolhida
 */
MenuOption handleMenuInput(int x, int y, Button buttons[], int count) {
    for (int i = 0; i < count; i++) {
        if (buttons[i].enabled &&
            x >= buttons[i].x && x <= buttons[i].x + buttons[i].w &&
            y >= buttons[i].y && y <= buttons[i].y + buttons[i].h) {
            return (MenuOption)(i + 1);
        }
    }
    return NONE;
}