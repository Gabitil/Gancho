#include <stdio.h>
#include <GL/freeglut.h>
#include "menu.h"
#include "game.h" // <<< NOVO: Inclui a interface do jogo

// Constantes para facilitar a manutenção
const int NUM_BUTTONS = 3;
const int INITIAL_WIN_WIDTH = 800;
const int INITIAL_WIN_HEIGHT = 600;

// MODIFICADO: Adiciona o estado de jogo
enum GameState {
    STATE_MENU,
    STATE_INSTRUCTIONS,
    STATE_GAME
};

// NOVO: Variável para guardar o estado atual
GameState currentState = STATE_MENU;

// NOVO: Botão específico para a tela de instruções
Button backButton;

Button buttons[NUM_BUTTONS];
MenuOption selected = NONE;

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

// // Substitua sua função display() por esta:
// void display() {
//     // MODIFICADO: Verifica o estado atual para decidir o que renderizar
//     switch (currentState) {
//         case STATE_MENU:
//             renderMenu(buttons, NUM_BUTTONS);
//             break;
//         case STATE_INSTRUCTIONS:
//             renderInstructions();
//             break;
//     }
// }

// MODIFICADO: A função display agora chama a renderização do jogo
void display() {
    switch (currentState) {
        case STATE_MENU:
            renderMenu(buttons, NUM_BUTTONS);
            break;
        case STATE_INSTRUCTIONS:
            renderInstructions();
            break;
        case STATE_GAME:
            game_display(); // <<< NOVO: Chama a função de display do jogo
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

// // Substitua sua função reshape() por esta:
// void reshape(int w, int h) {
//     if (h == 0) h = 1;
//     glViewport(0, 0, w, h);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluOrtho2D(0, w, h, 0);
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();

//     // MODIFICADO: Chama a nova função que atualiza TODOS os botões
//     updateAllButtonPositions(w, h);
// }

void reshape(int w, int h) {
    switch (currentState) {
        case STATE_MENU:
        case STATE_INSTRUCTIONS:
            if (h == 0) h = 1;
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, w, h, 0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            updateAllButtonPositions(w, h);
            break;
        case STATE_GAME:
            game_reshape(w, h); // <<< NOVO: Chama a função de reshape do jogo
            break;
    }
}

// Substitua sua função mouseClick() por esta:
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // MODIFICADO: A ação do clique depende do estado atual
        if (currentState == STATE_MENU) {
            selected = handleMenuInput(x, y, buttons, NUM_BUTTONS);
            switch (selected) {
                case START_GAME:
                    printf("Iniciando jogo...\n");
                    currentState = STATE_GAME;
                    game_start(); // <<< NOVO: Prepara e inicia o jogo
                    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)); // Força reconfiguração da projeção
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

// NOVO: Função de teclado principal que delega para o jogo
void keyboard(unsigned char key, int x, int y) {
    if (currentState == STATE_GAME) {
        GameAction action = game_keyboard(key, x, y);
        if (action == GAME_ACTION_EXIT_TO_MENU) {
            printf("Voltando para o menu...\n");
            currentState = STATE_MENU;
            // Reconfigura a projeção para o menu
            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }
    }
}

// NOVO: Função de timer principal que delega para o jogo
void timer(int value) {
    // if (currentState == STATE_GAME) {
        game_timer(value);
        glutTimerFunc(16, timer, 0); // Re-agenda o timer se ainda estivermos no jogo
    // }
}

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

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
    glutInitWindowPosition(100, 100); // Posição inicial da janela
    glutCreateWindow("Menu - OpenGL");

    init();
    game_init(); // <<< NOVO: Chama a inicialização única do jogo

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouseMotion);
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyboard); // <<< NOVO: Registra a função de teclado
    glutTimerFunc(0, timer, 0); // <<< NOVO: Registra a função de timer global

    glutMainLoop();
    return 0;
}