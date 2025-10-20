#include <stdio.h>
#include <GL/freeglut.h>
#include "menu.h" // Interface do Menu
#include "game.h" // Interface do Jogo

// --- Constantes Globais ---
const int NUM_MENU_BUTTONS = 3;
const int INITIAL_WIN_WIDTH = 800;
const int INITIAL_WIN_HEIGHT = 600;
const int NUM_LEVELS = 4;

// --- Estados do Jogo ---
enum GameState {
    STATE_MENU,
    STATE_INSTRUCTIONS,
    STATE_LEVEL_SELECT,
    STATE_GAME
};
GameState currentState = STATE_MENU;

// --- Variáveis Globais de UI ---
Button menuButtons[NUM_MENU_BUTTONS];
Button instrBackButton;
Button levelButtons[NUM_LEVELS];
Button levelSelectBackButton;
Button gameBackButton; // Botão "Voltar" de dentro do jogo

MenuOption selectedMenuOption = NONE;

// Variável de progressão
int maxLevelUnlocked = 1; // Começa com a Fase 1 destravada

// Variável global para saber qual fase está ativa
int activeLevel = 1;

// --- Protótipos de Callbacks ---
void display();
void reshape(int w, int h);
void mouseMotion(int x, int y);
void mouseClick(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void timer(int value); // <--- Adicione esta linha

// --- Funções de Renderização de Telas ---

void renderInstructions() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawText(50, 80, "COMO JOGAR");
    drawText(50, 120, "Use WASD ou as Setas para mover.");
    drawText(50, 150, "Aponte com o mouse.");
    drawText(50, 180, "Chegue ate a porta para vencer.");
    drawText(50, 210, "Pressione 'Q' no jogo para sair (ou use o botao 'Voltar').");
    
    drawButton(instrBackButton);
    glutSwapBuffers();
}

void renderLevelSelect() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Título
    glColor3f(0.1f, 0.1f, 0.1f);
    drawText(50, 80, "SELECIONE A FASE");

    // Desenha os botões de fase (habilitados ou não)
    for(int i = 0; i < NUM_LEVELS; i++) {
        drawButton(levelButtons[i]);
    }
    drawButton(levelSelectBackButton);
    
    glutSwapBuffers();
}

// Desenha o HUD (botão "Voltar") sobre a tela do jogo
void drawGameHUD() {
    // Salva as matrizes 3D/Mundo do jogo
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Configura a projeção 2D (pixels) do menu
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Desenha o botão "Voltar"
    drawButton(gameBackButton);

    // Restaura as matrizes
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// --- Funções de Callback do GLUT ---

// Função principal de display
void display() {
    // Define a cor de limpeza padrão (será usada pelos menus)
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

    switch (currentState) {
        case STATE_MENU:
            renderMenu(menuButtons, NUM_MENU_BUTTONS);
            break;
        case STATE_INSTRUCTIONS:
            renderInstructions();
            break;
        case STATE_LEVEL_SELECT:
            renderLevelSelect();
            break;
        case STATE_GAME:
            game_display(); // 1. Desenha o jogo
            drawGameHUD();  // 2. Desenha o HUD 2D por cima
            glutSwapBuffers(); // 3. Troca o buffer
            break;
    }
}

// Atualiza a posição de TODOS os botões
void updateAllButtonPositions(int w, int h) {
    float btnW = 220.0f;
    float btnH = 50.0f;
    float spacing = 20.0f;
    float margin = 20.0f;

    // Posição centralizada para blocos de botões
    float startX = (w - btnW) / 2.0f;

    // Botões do Menu Principal
    float totalMenuH = (NUM_MENU_BUTTONS * btnH) + ((NUM_MENU_BUTTONS - 1) * spacing);
    float startMenuY = (h - totalMenuH) / 2.0f;
    for (int i = 0; i < NUM_MENU_BUTTONS; ++i) {
        menuButtons[i].x = startX;
        menuButtons[i].y = startMenuY + i * (btnH + spacing);
    }

    // Botão Voltar (Instruções)
    instrBackButton.x = margin;
    instrBackButton.y = h - btnH - margin;

    // Botões de Seleção de Fase
    float totalLevelH = (NUM_LEVELS * btnH) + ((NUM_LEVELS - 1) * spacing);
    float startLevelY = (h - totalLevelH) / 2.0f;
    for(int i = 0; i < NUM_LEVELS; i++) {
        levelButtons[i].x = startX;
        levelButtons[i].y = startLevelY + i * (btnH + spacing);
        levelButtons[i].enabled = (i + 1 <= maxLevelUnlocked); // Atualiza o destravamento
    }
    
    // Botão Voltar (Seleção de Fase)
    levelSelectBackButton.x = margin;
    levelSelectBackButton.y = h - btnH - margin;

    // Botão Voltar (No Jogo) - Canto superior esquerdo
    gameBackButton.x = margin;
    gameBackButton.y = margin;
}

// Callback de Redimensionamento
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    // O sistema de coordenadas depende do estado!
    if (currentState == STATE_GAME) {
        game_reshape(w, h); // O jogo usa sua própria projeção
    } else {
        // Menus usam projeção 2D simples (0,0 top-left)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, w, h, 0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    
    // Atualiza a posição de todos os botões (menus)
    updateAllButtonPositions(w, h);
}

// Callback de movimento do mouse (hover)
void mouseMotion(int x, int y) {
    bool needsRedraw = false;

    if (currentState == STATE_MENU) {
        for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
            // AQUI ESTAVA O ERRO (usava 'buttons[i]')
            // CORRIGIDO PARA 'menuButtons[i]':
            menuButtons[i].hovered = (x >= menuButtons[i].x && x <= menuButtons[i].x + menuButtons[i].w &&
                                      y >= menuButtons[i].y && y <= menuButtons[i].y + menuButtons[i].h);
        }
        needsRedraw = true;
    } else if (currentState == STATE_INSTRUCTIONS) {
        instrBackButton.hovered = (x >= instrBackButton.x && x <= instrBackButton.x + instrBackButton.w &&
                                   y >= instrBackButton.y && y <= instrBackButton.y + instrBackButton.h);
        needsRedraw = true;
    } else if (currentState == STATE_LEVEL_SELECT) {
        for (int i = 0; i < NUM_LEVELS; i++) {
            if (levelButtons[i].enabled) {
                levelButtons[i].hovered = (x >= levelButtons[i].x && x <= levelButtons[i].x + levelButtons[i].w &&
                                           y >= levelButtons[i].y && y <= levelButtons[i].y + levelButtons[i].h);
            }
        }
        levelSelectBackButton.hovered = (x >= levelSelectBackButton.x && x <= levelSelectBackButton.x + levelSelectBackButton.w &&
                                          y >= levelSelectBackButton.y && y <= levelSelectBackButton.y + levelSelectBackButton.h);
        needsRedraw = true;
    } else if (currentState == STATE_GAME) {
        game_mouse_motion(x, y); // Passa o mouse para o jogo (mira)
        gameBackButton.hovered = (x >= gameBackButton.x && x <= gameBackButton.x + gameBackButton.w &&
                                  y >= gameBackButton.y && y <= gameBackButton.y + gameBackButton.h);
        needsRedraw = true;
    }

    if (needsRedraw) {
        glutPostRedisplay();
    }
}

// Callback de clique do mouse
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (currentState == STATE_MENU) {
            selectedMenuOption = handleMenuInput(x, y, menuButtons, NUM_MENU_BUTTONS);
            switch (selectedMenuOption) {
                case START_GAME: currentState = STATE_LEVEL_SELECT; break;
                case HOW_TO_PLAY: currentState = STATE_INSTRUCTIONS; break;
                case EXIT_GAME: glutLeaveMainLoop(); break;
                default: break;
            }
        } else if (currentState == STATE_INSTRUCTIONS) {
            if (instrBackButton.hovered) {
                currentState = STATE_MENU;
            }
        } else if (currentState == STATE_LEVEL_SELECT) {
            // Checa botões de fase
            for (int i = 0; i < NUM_LEVELS; i++) {
                if (levelButtons[i].hovered && levelButtons[i].enabled) {
                    activeLevel = i + 1; // Guarda a fase
                    currentState = STATE_GAME;
                    game_start_level(activeLevel);
                    glutTimerFunc(16, timer, 0); // Inicia o loop de jogo
                    break;
                }
            }
            // Checa botão voltar
            if (levelSelectBackButton.hovered) {
                currentState = STATE_MENU;
            }
        } else if (currentState == STATE_GAME) {
            if (gameBackButton.hovered) {
                currentState = STATE_LEVEL_SELECT; // Volta para a seleção de fase
            }
        }
        
        // Força o reshape para ajustar a projeção ao novo estado
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        glutPostRedisplay();
    }
}

// Callback de teclas normais
void keyboard(unsigned char key, int x, int y) {
    if (currentState == STATE_GAME) {
        GameAction action = game_keyboard(key, x, y);
        if (action == GAME_ACTION_EXIT_TO_MENU) {
            currentState = STATE_LEVEL_SELECT;
            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            glutPostRedisplay();
        }
    }
}

// Callback de teclas especiais
void specialKeys(int key, int x, int y) {
    if (currentState == STATE_GAME) {
        game_special_keys(key, x, y);
        // Não precisa de reshape, o jogo continua
    }
}

// Callback de Timer (loop principal do jogo)
void timer(int value) {
    if (currentState != STATE_GAME) {
        return; // Para o loop se sairmos do jogo
    }

    GameAction action = game_update(); // Roda a lógica do jogo

    if (action == GAME_ACTION_LEVEL_WON) {
        // Destrava a próxima fase
        if (activeLevel == maxLevelUnlocked && maxLevelUnlocked < NUM_LEVELS) {
            maxLevelUnlocked++;
        }
        currentState = STATE_LEVEL_SELECT; // Volta para a seleção
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    } else if (action == GAME_ACTION_LEVEL_LOST) {
        currentState = STATE_LEVEL_SELECT; // Volta para a seleção
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    } else {
        // Jogo continua
        glutTimerFunc(16, timer, 0); // Re-agenda o próximo frame
    }
    
    glutPostRedisplay();
}

// --- Função de Inicialização ---
void init() {
    // Botões do Menu Principal
    menuButtons[0] = {0, 0, 220, 50, "Iniciar Jogo", false, true};
    menuButtons[1] = {0, 0, 220, 50, "Como Jogar", false, true};
    menuButtons[2] = {0, 0, 220, 50, "Sair", false, true};

    // Botão Voltar (Instruções)
    instrBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

    // Botões de Seleção de Fase
    levelButtons[0] = {0, 0, 220, 50, "Fase 1", false, false};
    levelButtons[1] = {0, 0, 220, 50, "Fase 2", false, false};
    levelButtons[2] = {0, 0, 220, 50, "Fase 3", false, false};
    levelButtons[3] = {0, 0, 220, 50, "Fase 4", false, false};
    
    // Botão Voltar (Seleção de Fase)
    levelSelectBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

    // Botão Voltar (No Jogo)
    gameBackButton = {0, 0, 100, 35, "Voltar", false, true};
    
    // Calcula as posições iniciais
    updateAllButtonPositions(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);

    // Inicializa o módulo do jogo
    game_init();
}

// --- Main ---
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Protótipo de Jogo com Menu");

    init(); // Inicializa nossos estados e botões

    // Registra Callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseClick);
    glutPassiveMotionFunc(mouseMotion); // Hover e mira do jogo
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys); // Para as setas

    glutMainLoop();
    return 0;
}