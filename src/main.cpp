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
// void specialKeys(int key, int x, int y);
void timer(int value); // <--- Adicione esta linha

// --- Funções de Renderização de Telas ---

// *** TEXTO DE INSTRUÇÕES ATUALIZADO ***
void renderInstructions() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawText(50, 80, "COMO JOGAR");
    drawText(50, 120, "1. Mire no teto (a linha ficara verde).");
    drawText(50, 150, "2. Pressione e SEGURE o mouse para prender o gancho.");
    drawText(50, 180, "3. SOLTE o mouse para se soltar.");
    drawText(50, 210, "4. Use o balanco para chegar ate a porta.");
    drawText(50, 240, "Pressione 'Q' ou use o botao 'Voltar' para sair da fase.");
    
    drawButton(instrBackButton);
    glutSwapBuffers();
}

void renderLevelSelect() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawText(50, 80, "SELECIONE A FASE");
    for(int i = 0; i < NUM_LEVELS; i++) {
        drawButton(levelButtons[i]);
    }
    drawButton(levelSelectBackButton);
    glutSwapBuffers();
}

// Desenha o HUD (botão "Voltar") sobre a tela do jogo
void drawGameHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Desenha o botão "Voltar"
    drawButton(gameBackButton);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// --- Funções de Callback do GLUT ---

void display() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Fundo padrão

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
            game_display(); // 1. Desenha o jogo (com câmera)
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
    float startX = (w - btnW) / 2.0f;

    // Botões do Menu Principal
    float totalMenuH = (NUM_MENU_BUTTONS * btnH) + ((NUM_MENU_BUTTONS - 1) * spacing);
    float startMenuY = (h - totalMenuH) / 2.0f;
    for (int i = 0; i < NUM_MENU_BUTTONS; ++i) {
        menuButtons[i].x = startX;
        menuButtons[i].y = startMenuY + i * (btnH + spacing);
    }

    instrBackButton.x = margin;
    instrBackButton.y = h - btnH - margin;

    // Botões de Seleção de Fase
    float totalLevelH = (NUM_LEVELS * btnH) + ((NUM_LEVELS - 1) * spacing);
    float startLevelY = (h - totalLevelH) / 2.0f;
    for(int i = 0; i < NUM_LEVELS; i++) {
        levelButtons[i].x = startX;
        levelButtons[i].y = startLevelY + i * (btnH + spacing);
        levelButtons[i].enabled = (i + 1 <= maxLevelUnlocked); // Atualiza destravamento
    }
    
    levelSelectBackButton.x = margin;
    levelSelectBackButton.y = h - btnH - margin;

    // Botão Voltar (No Jogo)
    gameBackButton.x = margin;
    gameBackButton.y = margin;
}

// Callback de Redimensionamento
void reshape(int w, int h) {
    if (h == 0) h = 1;

    if (currentState == STATE_GAME) {
        game_reshape(w, h);
    } else {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, w, h, 0); // Projeção 2D (0,0 top-left)
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    
    updateAllButtonPositions(w, h);
}

// Callback de movimento do mouse (hover E mira)
void mouseMotion(int x, int y) {
    if (currentState == STATE_GAME) {
        game_mouse_motion(x, y); // Passa o mouse para o jogo (mira)
        gameBackButton.hovered = (x >= gameBackButton.x && x <= gameBackButton.x + gameBackButton.w &&
                                  y >= gameBackButton.y && y <= gameBackButton.y + gameBackButton.h);
    } 
    else {
        // Lógica de hover dos menus
        if (currentState == STATE_MENU) {
            for (int i = 0; i < NUM_MENU_BUTTONS; i++) {
                menuButtons[i].hovered = (x >= menuButtons[i].x && x <= menuButtons[i].x + menuButtons[i].w &&
                                          y >= menuButtons[i].y && y <= menuButtons[i].y + menuButtons[i].h);
            }
        } else if (currentState == STATE_INSTRUCTIONS) {
            instrBackButton.hovered = (x >= instrBackButton.x && x <= instrBackButton.x + instrBackButton.w &&
                                       y >= instrBackButton.y && y <= instrBackButton.y + instrBackButton.h);
        } else if (currentState == STATE_LEVEL_SELECT) {
            for (int i = 0; i < NUM_LEVELS; i++) {
                if (levelButtons[i].enabled) {
                    levelButtons[i].hovered = (x >= levelButtons[i].x && x <= levelButtons[i].x + levelButtons[i].w &&
                                               y >= levelButtons[i].y && y <= levelButtons[i].y + levelButtons[i].h);
                }
            }
            levelSelectBackButton.hovered = (x >= levelSelectBackButton.x && x <= levelSelectBackButton.x + levelSelectBackButton.w &&
                                              y >= levelSelectBackButton.y && y <= levelSelectBackButton.y + levelSelectBackButton.h);
        }
    }
    
    glutPostRedisplay();
}

// Callback de clique do mouse (menus E jogo)
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        
        if (currentState == STATE_GAME) {
            // Checa o botão "Voltar" (HUD) PRIMEIRO
            if (state == GLUT_DOWN && gameBackButton.hovered) {
                currentState = STATE_LEVEL_SELECT; 
                reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            } else {
                // Se não foi o botão "Voltar", passa o clique para o jogo
                game_mouse_click(button, state);
            }
        } 
        // Lógica de clique dos menus (só no clique "para baixo")
        else if (state == GLUT_DOWN) {
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
                for (int i = 0; i < NUM_LEVELS; i++) {
                    if (levelButtons[i].hovered && levelButtons[i].enabled) {
                        activeLevel = i + 1;
                        currentState = STATE_GAME;
                        game_start_level(activeLevel);
                        glutTimerFunc(16, timer, 0); // Inicia o loop de jogo
                        break;
                    }
                }
                if (levelSelectBackButton.hovered) {
                    currentState = STATE_MENU;
                }
            }
            
            if (currentState != STATE_GAME) {
                reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            }
        }
    }
    glutPostRedisplay();
}

// Callback de teclas normais
void keyboard(unsigned char key, int x, int y) {
    if (currentState == STATE_GAME) {
        GameAction action = game_keyboard(key, x, y); // Passa 'q' para o jogo
        if (action == GAME_ACTION_EXIT_TO_MENU) {
            currentState = STATE_LEVEL_SELECT;
            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            glutPostRedisplay();
        }
    }
}

// Callback de Timer (loop principal do jogo)
void timer(int value) {
    if (currentState != STATE_GAME) {
        return; // Para o loop se sairmos do jogo
    }

    GameAction action = game_update(); // Roda a física

    if (action == GAME_ACTION_LEVEL_WON) {
        if (activeLevel == maxLevelUnlocked && maxLevelUnlocked < NUM_LEVELS) {
            maxLevelUnlocked++; // Destrava a próxima fase!
        }
        currentState = STATE_LEVEL_SELECT;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    } else if (action == GAME_ACTION_LEVEL_LOST) {
        currentState = STATE_LEVEL_SELECT;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    } else {
        // Jogo continua
        glutTimerFunc(16, timer, 0); // Re-agenda (aprox 60 FPS)
    }
    
    glutPostRedisplay();
}

// --- Função de Inicialização ---
void init() {
    // Botões do Menu Principal
    menuButtons[0] = {0, 0, 220, 50, "Iniciar Jogo", false, true};
    menuButtons[1] = {0, 0, 220, 50, "Como Jogar", false, true};
    menuButtons[2] = {0, 0, 220, 50, "Sair", false, true};

    instrBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

    // Botões de Seleção de Fase
    levelButtons[0] = {0, 0, 220, 50, "Fase 1", false, false};
    levelButtons[1] = {0, 0, 220, 50, "Fase 2", false, false};
    levelButtons[2] = {0, 0, 220, 50, "Fase 3", false, false};
    levelButtons[3] = {0, 0, 220, 50, "Fase 4", false, false};
    
    levelSelectBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

    // Botão Voltar (No Jogo)
    gameBackButton = {0, 0, 100, 35, "Voltar", false, true};
    
    updateAllButtonPositions(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
    game_init(); // Inicializa o módulo do jogo
}

// --- Main ---
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Jogo Gancho Vetorial - Protótipo");

    init(); 

    // Registra Callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseClick);        // Para cliques (menus e jogo)
    glutPassiveMotionFunc(mouseMotion); // Para hover (menus)
    glutMotionFunc(mouseMotion);      // Para arrastar (mira do jogo)
    glutKeyboardFunc(keyboard);
    // glutSpecialFunc não é mais usado

    glutMainLoop();
    return 0;
}