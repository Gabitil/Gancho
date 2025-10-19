#include <stdio.h>
#include <GL/freeglut.h>
#include "menu.h"

// Constantes para facilitar a manutenção
const int NUM_BUTTONS = 3;
const int INITIAL_WIN_WIDTH = 800;
const int INITIAL_WIN_HEIGHT = 600;

enum GameState {
    STATE_MENU,
    STATE_INSTRUCTIONS
};

// NOVO: Variável para guardar o estado atual
GameState currentState = STATE_MENU;

// NOVO: Botão específico para a tela de instruções
Button backButton;

Button buttons[NUM_BUTTONS];
MenuOption selected = NONE;

// // NOVA FUNÇÃO: Recalcula a posição dos botões para centralizá-los
// void updateAllButtonPositions(int w, int h) {
//     float buttonW = 220.0f;
//     float buttonH = 50.0f;
//     float spacing = 20.0f; // Espaço entre os botões

//     // Calcula a posição X para centralizar horizontalmente
//     float startX = (w - buttonW) / 2.0f;

//     // Calcula a altura total do bloco de botões para centralizar verticalmente
//     float totalBlockHeight = (NUM_BUTTONS * buttonH) + ((NUM_BUTTONS - 1) * spacing);
//     float startY = (h - totalBlockHeight) / 2.0f;

//     for (int i = 0; i < NUM_BUTTONS; ++i) {
//         buttons[i].x = startX;
//         buttons[i].y = startY + i * (buttonH + spacing);
//         buttons[i].w = buttonW;
//         buttons[i].h = buttonH;
//     }
// }

// Substitua sua função updateAllButtonPositions por esta:
void updateAllButtonPositions(int w, int h) {
    // --- Lógica para os botões do menu (sem alterações) ---
    float buttonW = 220.0f;
    float buttonH = 50.0f;
    float spacing = 20.0f;
    float startX = (w - buttonW) / 2.0f;
    float totalBlockHeight = (NUM_BUTTONS * buttonH) + ((NUM_BUTTONS - 1) * spacing);
    float startY = (h - totalBlockHeight) / 2.0f;
    for (int i = 0; i < NUM_BUTTONS; ++i) {
        buttons[i].x = startX;
        buttons[i].y = startY + i * (buttonH + spacing);
        buttons[i].w = buttonW;
        buttons[i].h = buttonH;
    }

    // --- NOVO: Lógica para o botão "Voltar" ---
    // Posiciona no canto inferior esquerdo com uma margem de 20px
    float margin = 20.0f;
    backButton.x = margin;
    backButton.y = h - backButton.h - margin;
}

// NOVA FUNÇÃO AUXILIAR: Desenha uma linha de texto na tela
void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// NOVA FUNÇÃO: Renderiza a tela de instruções
void renderInstructions() {
    glClear(GL_COLOR_BUFFER_BIT); // Limpa a tela

    // Define a cor do texto das instruções
    glColor3f(0.1f, 0.1f, 0.1f); // Preto/Cinza escuro

    // --- AQUI VOCÊ COLOCARÁ SEU TEXTO ---
    // Exemplo de como adicionar texto:
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    drawText(50, 80, "COMO JOGAR");
    drawText(50, 120, "1. Regra numero um.");
    drawText(50, 150, "2. Regra numero dois explicando o objetivo.");
    drawText(50, 180, "3. Descreva os controles aqui.");
    drawText(50, 240, "Boa sorte!");
    // ------------------------------------

    // Desenha o botão "Voltar"
    drawButton(backButton);

    glutSwapBuffers();
}

// void display() {
//     renderMenu(buttons, NUM_BUTTONS);
// }

// Substitua sua função display() por esta:
void display() {
    // MODIFICADO: Verifica o estado atual para decidir o que renderizar
    switch (currentState) {
        case STATE_MENU:
            renderMenu(buttons, NUM_BUTTONS);
            break;
        case STATE_INSTRUCTIONS:
            renderInstructions();
            break;
    }
}

// void mouseMotion(int x, int y) {
//     // A CONVERSÃO DE 'y' FOI REMOVIDA DAQUI
//     // int height = glutGet(GLUT_WINDOW_HEIGHT);
//     // y = height - y;

//     for (int i = 0; i < NUM_BUTTONS; i++) {
//         buttons[i].hovered = (x >= buttons[i].x && x <= buttons[i].x + buttons[i].w &&
//                               y >= buttons[i].y && y <= buttons[i].y + buttons[i].h);
//     }
//     glutPostRedisplay(); // Redesenha a tela para mostrar a mudança de cor
// }

// Substitua sua função mouseMotion() por esta:
void mouseMotion(int x, int y) {
    // MODIFICADO: Lógica de hover depende do estado atual
    if (currentState == STATE_MENU) {
        for (int i = 0; i < NUM_BUTTONS; i++) {
            buttons[i].hovered = (x >= buttons[i].x && x <= buttons[i].x + buttons[i].w &&
                                  y >= buttons[i].y && y <= buttons[i].y + buttons[i].h);
        }
    } else if (currentState == STATE_INSTRUCTIONS) {
        // Na tela de instruções, só precisamos checar o botão "Voltar"
        backButton.hovered = (x >= backButton.x && x <= backButton.x + backButton.w &&
                              y >= backButton.y && y <= backButton.y + backButton.h);
    }
    glutPostRedisplay();
}

// void mouseClick(int button, int state, int x, int y) {
//     if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
//         selected = handleMenuInput(x, y, buttons, NUM_BUTTONS);
//         switch (selected) {
//             case START_GAME:
//                 printf("Iniciar jogo\n");
//                 // Aqui você colocaria a lógica para iniciar o jogo
//                 break;
//             case HOW_TO_PLAY:
//                 printf("Como jogar\n");
//                 // Aqui você abriria a tela de instruções
//                 break;
//             case EXIT_GAME:
//                 printf("Saindo...\n");
//                 glutLeaveMainLoop(); // Fecha o programa de forma limpa
//                 break;
//             case NONE:
//                 // Não faz nada se clicou fora
//                 break;
//         }
//     }
// }

// Substitua sua função mouseClick() por esta:
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // MODIFICADO: A ação do clique depende do estado atual
        if (currentState == STATE_MENU) {
            selected = handleMenuInput(x, y, buttons, NUM_BUTTONS);
            switch (selected) {
                case START_GAME:
                    printf("Iniciar jogo\n");
                    break;
                case HOW_TO_PLAY:
                    printf("Abrindo instrucoes...\n");
                    currentState = STATE_INSTRUCTIONS; // MUDA PARA A TELA DE INSTRUÇÕES
                    break;
                case EXIT_GAME:
                    printf("Saindo...\n");
                    glutLeaveMainLoop();
                    break;
                default: break;
            }
        } else if (currentState == STATE_INSTRUCTIONS) {
            // Se estamos nas instruções, checa se o clique foi no botão "Voltar"
            if (backButton.hovered) {
                printf("Voltando para o menu...\n");
                currentState = STATE_MENU; // MUDA DE VOLTA PARA O MENU
            }
        }
        glutPostRedisplay(); // Pede para redesenhar a tela após a mudança de estado
    }
}

// void init() {
//     glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Fundo cinza claro

//     // Define apenas os dados que não mudam
//     buttons[0].label = "Iniciar Jogo";
//     buttons[0].hovered = false;

//     buttons[1].label = "Como Jogar";
//     buttons[1].hovered = false;

//     buttons[2].label = "Sair";
//     buttons[2].hovered = false;

//     // Chama a função para calcular a posição inicial dos botões
//     updateAllButtonPositions(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
// }

// Substitua sua função init() por esta:
void init() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Fundo cinza claro

    // Botões do menu principal
    buttons[0].label = "Iniciar Jogo";
    buttons[0].hovered = false;
    buttons[1].label = "Como Jogar";
    buttons[1].hovered = false;
    buttons[2].label = "Sair";
    buttons[2].hovered = false;

    // NOVO: Inicializa o botão de voltar
    backButton.label = "Voltar para o Menu";
    backButton.hovered = false;
    backButton.w = 220; // Mesma largura dos outros
    backButton.h = 50;  // Mesma altura

    // Calcula a posição inicial de todos os botões
    updateAllButtonPositions(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
}

// void reshape(int w, int h) {
//     // Garante que a altura nunca seja 0
//     if (h == 0) h = 1;

//     glViewport(0, 0, w, h);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     // Usa as novas dimensões para configurar o sistema de coordenadas
//     gluOrtho2D(0, w, h, 0);
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();

//     // ATUALIZA A POSIÇÃO dos botões sempre que a janela for redimensionada
//     updateAllButtonPositions(w, h);
// }

// Substitua sua função reshape() por esta:
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // MODIFICADO: Chama a nova função que atualiza TODOS os botões
    updateAllButtonPositions(w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
    glutInitWindowPosition(100, 100); // Posição inicial da janela
    glutCreateWindow("Menu - OpenGL");

    init();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouseMotion); // Para hover sem clicar
    glutMouseFunc(mouseClick);

    glutMainLoop();
    return 0;
}