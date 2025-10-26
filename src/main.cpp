/**
 * Para fazermos a main, pesquisamos os melhores padrões de projeto para organizar a melhor forma de estruturar 
 * as funções de callback. Com isso, dividimos o projeto em estados (uma máquina de estados definida pelo "GameState").
 * As funções terão as suas atividades subdivididas e a main cuidará de tudo que não for totalmente relacionado ao jogo. 
 * As funções de tratamento de teclado, mouse e clique são separadas para garantir a modularização do projeto. 
 */

#include <stdio.h>
#include <GL/freeglut.h>
#include "menu.h" 
#include "game.h"
#include "utils.h"

// ------------------------------------------------------------------------------------------------------------------

const int NUM_MENU_BUTTONS = 3;
const int INITIAL_WIN_WIDTH = 800;
const int INITIAL_WIN_HEIGHT = 600;
const int NUM_LEVELS = 3;

enum GameState {
    STATE_MENU,
    STATE_INSTRUCTIONS,
    STATE_LEVEL_SELECT,
    STATE_GAME
};

GameState currentState = STATE_MENU;

Button menuButtons[NUM_MENU_BUTTONS]; // Botões do menu principal
Button instrBackButton; // Boão de voltar contido na tela de instruções
Button levelButtons[NUM_LEVELS]; // Botões para selecionar qual o level o player quer jogar
Button levelSelectBackButton; // Botão de "voltar" contido na seleção de levels
Button gameBackButton; // Botão de "voltar" contido dentro do próprio jogo

MenuOption selectedMenuOption = NONE;
int maxLevelUnlocked = 1;
int activeLevel = 1;

// ------------------------------------------------------------------------------------------------------------------

void display();
void reshape(int w, int h);
void mouseMotion(int x, int y);
void mouseClick(int button, int state, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void timer(int value);

// ------------------------------------------------------------------------------------------------------------------

void renderInstructions() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawText(50, 80, "INSTRUÇÕES DE COMO JOGAR: ");
    drawText(50, 120, "- Use 'A' e 'D' ou setas (<- ->) para andar no chao. Não e possivel pular");
    drawText(50, 150, "- Mire com o mouse para onde quer disparar o gancho");
    drawText(50, 180, "- Clique e segure para disparar o gancho e, ao ser preso, solte para desprende-lo");
    drawText(50, 210, "- Observe os valores no canto superior direito para se guiar melhor");
    drawText(50, 240, "- Haverao diversos obstaculos. Dentre eles:");
    drawText(100, 270, "- Plataformas para andar por cima e/ou se prender com o gancho.");
    drawText(100, 300, "- Correntes de vento que podem mudar a direcao do seu lancamento");
    drawText(100, 330, "- Espinhos fatais (aconselhamos a nao toca-los)");
    drawText(50, 390, "Seu objetivo e chegar ate a porta final =)");
    drawText(50, 420, "Economize disparos! Boa sorte!");
    
    drawButton(instrBackButton);
    glutSwapBuffers();
}

void renderLevelSelect() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawText(50, 80, "Escolha a fase");
    for(int i = 0; i < NUM_LEVELS; i++) {
        drawButton(levelButtons[i]);
    }
    drawButton(levelSelectBackButton);
    glutSwapBuffers();
}

void drawGameHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            drawButton(gameBackButton);
            glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/**
 * Essa função é responsável por verificar o estado do jogo e, de acordo com o valor da 
 * constante global, chama as funções de renderização (desenho dos elementos na tela)
 */
void display() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Cor de fundo padrão

    switch (currentState) {
        case STATE_MENU: renderMenu(menuButtons, NUM_MENU_BUTTONS); break;
        case STATE_INSTRUCTIONS: renderInstructions(); break;
        case STATE_LEVEL_SELECT: renderLevelSelect(); break;
        case STATE_GAME:
            gameDisplay();
            drawGameHUD();
            glutSwapBuffers();
            break;
    }
}

/**
 * Essa função tem um comportamento similar ao "reshape". Ela atualiza a posição de todos os 
 * botões na tela de acordo com o o dimensionamento da janela do freeGlut
 */
void updateAllButtonPositions(int w, int h) {

    /**
     * Parâmetros padrões de configuração para os botões (margem, espaçamento etc)
     */
    float buttonWidth = 220.0f;
    float buttonHeight = 50.0f;
    float spacing = 20.0f;
    float margin = 20.0f;
    float startX = (w - buttonWidth) / 2.0f;

    float totalMenuH = (NUM_MENU_BUTTONS * buttonHeight) + ((NUM_MENU_BUTTONS - 1) * spacing);
    float startMenuY = (h - totalMenuH) / 2.0f;
    for (int i = 0; i < NUM_MENU_BUTTONS; ++i) {
        menuButtons[i].x = startX;
        menuButtons[i].y = startMenuY + i * (buttonHeight + spacing);
    }

    instrBackButton.x = margin;
    instrBackButton.y = h - buttonHeight - margin;

    float totalLevelH = (NUM_LEVELS * buttonHeight) + ((NUM_LEVELS - 1) * spacing);
    float startLevelY = (h - totalLevelH) / 2.0f;
    for(int i = 0; i < NUM_LEVELS; i++) {
        levelButtons[i].x = startX;
        levelButtons[i].y = startLevelY + i * (buttonHeight + spacing);
        levelButtons[i].enabled = (i + 1 <= maxLevelUnlocked);
    }
    
    levelSelectBackButton.x = margin;
    levelSelectBackButton.y = h - buttonHeight - margin;

    gameBackButton.x = margin;
    gameBackButton.y = margin;
}

/**
 * Para adaptar o reshape, separamos entre o reshape do game ou o do menu (visto que ele só tem botões)
 */
void reshape(int w, int h) {
    if (h == 0) h = 1;

    if (currentState == STATE_GAME) {
        gameReshape(w, h);
    } else {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, w, h, 0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    
    updateAllButtonPositions(w, h);
}

/**
 * O callback que será passado para a máquina de estados do freeGlut é o da main, no entanto 
 * o jogo cadastra e controla a sua própria função de controle do mouse. A main 
 * apenas controla questões de visualização dos botões (hover e ativação)
 */
void mouseMotion(int x, int y) {
    if (currentState == STATE_GAME) {
        gameMouseMotion(x, y);
        gameBackButton.hovered = (x >= gameBackButton.x && x <= gameBackButton.x + gameBackButton.w &&
                                  y >= gameBackButton.y && y <= gameBackButton.y + gameBackButton.h);
    } 
    else {

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

/**
 * Assim como a callback de movumento do mouse tem o seu controle dividido entre a main e as callback do game, 
 * a callback de controle de clique do mouse também tem o seu controle separado. Assim como explicado em sala, 
 * a função de mouse é chamada sempre que o usuário pressiona (GLUT_DOWN) ou solta (GLUT_UP) o botão do mouse. 
 * As callbacks do game controlam apenas as questões de jogabilidade e a main é responsável por controlar os botões
 * de voltar e outras coisas na tela (ainda que esteja em modo de jogo).  
 */
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        
        if (currentState == STATE_GAME) {
            if (state == GLUT_DOWN && gameBackButton.hovered) {
                currentState = STATE_LEVEL_SELECT; 
                reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            } else {
                gameMouseClick(button, state);
            }
        } 
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
                        gameStartLevel(activeLevel);
                        glutTimerFunc(16, timer, 0);
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

// ------------------------------------------------------------------------------------------------------------------------

void keyboardDown(unsigned char key, int x, int y) {
    if (currentState == STATE_GAME) {
        GameAction action = gameKeyDown(key, x, y);
        if (action == GAME_ACTION_EXIT_TO_MENU) {
            currentState = STATE_LEVEL_SELECT;
            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            glutPostRedisplay();
        }
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    if (currentState == STATE_GAME) {
        gameKeyUp(key, x, y);
    }
}

void specialDown(int key, int x, int y) {
    if (currentState == STATE_GAME) {
        gameSpecialDown(key, x, y);
    }
}

void specialUp(int key, int x, int y) {
    if (currentState == STATE_GAME) {
        gameSpecialUp(key, x, y);
    }
}

/**
 * Controla toda a continuidade do game. É responsável por chamar as funções de atualização
 * do jogo e verificar os estados retornados; com isso, essa função trata os desbloqueios de fase e 
 * os estados que irão proceder após algum retorno do game
 */
void timer(int value) {
    if (currentState != STATE_GAME) {
        return;
    }

    GameAction action = gameUpdate();

    if (action == GAME_ACTION_LEVEL_WON) {
        if (activeLevel == maxLevelUnlocked && maxLevelUnlocked < NUM_LEVELS) {
            maxLevelUnlocked++;
        }
        currentState = STATE_LEVEL_SELECT;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    } else if (action == GAME_ACTION_LEVEL_LOST) {
        currentState = STATE_LEVEL_SELECT;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    } else {
        glutTimerFunc(16, timer, 0); // Faz o reagendamento da função em 60 FPS
    }
    
    glutPostRedisplay();
}

void init() {
    // Botões do Menu Principal
    menuButtons[0] = {0, 0, 220, 50, "Iniciar Jogo", false, true};
    menuButtons[1] = {0, 0, 220, 50, "Como Jogar", false, true};
    menuButtons[2] = {0, 0, 220, 50, "Sair", false, true};

    // Botão Voltar (na tela de instruções)
    instrBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

    // Botões de Seleção de Fase
    levelButtons[0] = {0, 0, 220, 50, "Fase 1", false, false};
    levelButtons[1] = {0, 0, 220, 50, "Fase 2", false, false};
    levelButtons[2] = {0, 0, 220, 50, "Fase 3", false, false};
    
    // Botão Voltar (na seleção de fases)
    levelSelectBackButton = {0, 0, 220, 50, "Voltar ao Menu", false, true};

    // Botão Voltar (no jogo)
    gameBackButton = {0, 0, 100, 35, "Voltar", false, true};
    
    updateAllButtonPositions(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
    gameInit();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Gancho");

    init(); 

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseClick);
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion);
    
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);

    glutMainLoop();
    return 0;
}