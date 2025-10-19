#include <stdio.h>
#include <GL/freeglut.h>
#include "menu.h"

// Constantes para facilitar a manutenção
const int NUM_BUTTONS = 3;
const int INITIAL_WIN_WIDTH = 800;
const int INITIAL_WIN_HEIGHT = 600;

Button buttons[NUM_BUTTONS];
MenuOption selected = NONE;

// NOVA FUNÇÃO: Recalcula a posição dos botões para centralizá-los
void updateButtonPositions(int w, int h) {
    float buttonW = 220.0f;
    float buttonH = 50.0f;
    float spacing = 20.0f; // Espaço entre os botões

    // Calcula a posição X para centralizar horizontalmente
    float startX = (w - buttonW) / 2.0f;

    // Calcula a altura total do bloco de botões para centralizar verticalmente
    float totalBlockHeight = (NUM_BUTTONS * buttonH) + ((NUM_BUTTONS - 1) * spacing);
    float startY = (h - totalBlockHeight) / 2.0f;

    for (int i = 0; i < NUM_BUTTONS; ++i) {
        buttons[i].x = startX;
        buttons[i].y = startY + i * (buttonH + spacing);
        buttons[i].w = buttonW;
        buttons[i].h = buttonH;
    }
}

void display() {
    renderMenu(buttons, NUM_BUTTONS);
}

void mouseMotion(int x, int y) {
    // A CONVERSÃO DE 'y' FOI REMOVIDA DAQUI
    // int height = glutGet(GLUT_WINDOW_HEIGHT);
    // y = height - y;

    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].hovered = (x >= buttons[i].x && x <= buttons[i].x + buttons[i].w &&
                              y >= buttons[i].y && y <= buttons[i].y + buttons[i].h);
    }
    glutPostRedisplay(); // Redesenha a tela para mostrar a mudança de cor
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        selected = handleMenuInput(x, y, buttons, NUM_BUTTONS);
        switch (selected) {
            case START_GAME:
                printf("Iniciar jogo\n");
                // Aqui você colocaria a lógica para iniciar o jogo
                break;
            case HOW_TO_PLAY:
                printf("Como jogar\n");
                // Aqui você abriria a tela de instruções
                break;
            case EXIT_GAME:
                printf("Saindo...\n");
                glutLeaveMainLoop(); // Fecha o programa de forma limpa
                break;
            case NONE:
                // Não faz nada se clicou fora
                break;
        }
    }
}

void init() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Fundo cinza claro

    // Define apenas os dados que não mudam
    buttons[0].label = "Iniciar Jogo";
    buttons[0].hovered = false;

    buttons[1].label = "Como Jogar";
    buttons[1].hovered = false;

    buttons[2].label = "Sair";
    buttons[2].hovered = false;

    // Chama a função para calcular a posição inicial dos botões
    updateButtonPositions(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
}

void reshape(int w, int h) {
    // Garante que a altura nunca seja 0
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Usa as novas dimensões para configurar o sistema de coordenadas
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ATUALIZA A POSIÇÃO dos botões sempre que a janela for redimensionada
    updateButtonPositions(w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
    glutInitWindowPosition(100, 100); // Posição inicial da janela
    glutCreateWindow("Menu Responsivo - OpenGL");

    init();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouseMotion); // Para hover sem clicar
    glutMouseFunc(mouseClick);

    glutMainLoop();
    return 0;
}