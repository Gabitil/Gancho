#include "game.h"
#include <GL/freeglut.h>
#include <stdio.h> // Para printf

// --- Constantes do Jogo ---
const float PLAYER_SPEED = 5.0f;
const float GAME_WORLD_WIDTH = 800.0f;
const float GAME_WORLD_HEIGHT = 600.0f;

// --- Estruturas de Jogo ---
struct GameObject {
    float x, y, w, h;
    float r, g, b; // Cor
};

// --- Variáveis Globais do Jogo ---
GameObject player;
GameObject door;
GameObject floorObj; // Renomeado para evitar conflito
int CURRENT_LEVEL = 1;

// Posição do mouse no mundo do jogo
float mouseGameX = 0;
float mouseGameY = 0;

// --- Funções Internas do Jogo ---

// Helper para desenhar retângulos
void drawRect(const GameObject& obj) {
    glColor3f(obj.r, obj.g, obj.b);
    glBegin(GL_QUADS);
    glVertex2f(obj.x, obj.y);
    glVertex2f(obj.x + obj.w, obj.y);
    glVertex2f(obj.x + obj.w, obj.y + obj.h);
    glVertex2f(obj.x, obj.y + obj.h);
    glEnd();
}

// Detecção de colisão AABB (Axis-Aligned Bounding Box)
bool checkCollision(const GameObject& a, const GameObject& b) {
    // Se um retângulo está à esquerda do outro
    if (a.x + a.w < b.x || b.x + b.w < a.x) {
        return false;
    }
    // Se um retângulo está acima do outro
    if (a.y + a.h < b.y || b.y + b.h < a.y) {
        return false;
    }
    // Caso contrário, há colisão
    return true;
}

// Move o jogador e garante que ele não saia dos limites
void movePlayer(float dx, float dy) {
    player.x += dx;
    player.y += dy;

    // Colisão com o chão
    if (player.y < floorObj.y + floorObj.h) {
        player.y = floorObj.y + floorObj.h;
    }
    // Limites da tela (esquerda/direita)
    if (player.x < 0) {
        player.x = 0;
    }
    if (player.x + player.w > GAME_WORLD_WIDTH) {
        player.x = GAME_WORLD_WIDTH - player.w;
    }
    // Limite da tela (topo)
    if (player.y + player.h > GAME_WORLD_HEIGHT) {
        player.y = GAME_WORLD_HEIGHT - player.h;
    }
}


// --- Implementação da Interface (game.h) ---

void game_init() {
    // Inicializa o chão (é o mesmo para todas as fases)
    floorObj.x = 0;
    floorObj.y = 0;
    floorObj.w = GAME_WORLD_WIDTH;
    floorObj.h = 40;
    floorObj.r = 0.2f; floorObj.g = 0.6f; floorObj.b = 0.2f; // Verde
}

void game_start_level(int level) {
    CURRENT_LEVEL = level;
    printf("Iniciando prototipo - Fase %d\n", level);

    // Configura o jogador
    player.x = 50;
    player.y = floorObj.h; // Começa em cima do chão
    player.w = 40;
    player.h = 40;
    player.r = 0.9f; player.g = 0.1f; player.b = 0.1f; // Vermelho

    // Configura a porta
    door.x = 720;
    door.y = floorObj.h; // Em cima do chão
    door.w = 30;
    door.h = 80;
    door.r = 0.5f; door.g = 0.3f; door.b = 0.0f; // Marrom

    // (Futuro: adicione obstáculos baseados no 'level')
    // if (level == 2) { ... }
}

void game_reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Define o mundo do jogo como (0,0) no canto inferior esquerdo
    // e (800, 600) no canto superior direito.
    gluOrtho2D(0.0, GAME_WORLD_WIDTH, 0.0, GAME_WORLD_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

GameAction game_update() {
    // Lógica do jogo (movimento é feito por input)

    // Checar condição de vitória
    if (checkCollision(player, door)) {
        printf("Colidiu com a porta! Venceu!\n");
        return GAME_ACTION_LEVEL_WON;
    }

    return GAME_ACTION_CONTINUE;
}

void game_display() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Cor do céu (azul claro)
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Desenha os objetos
    drawRect(floorObj);
    drawRect(door);
    drawRect(player); // Desenha o jogador

    // Desenha a "mira" (vetor)
    glColor3f(1.0f, 1.0f, 0.0f); // Amarelo
    glBegin(GL_LINES);
    glVertex2f(player.x + player.w / 2, player.y + player.h / 2); // Centro do jogador
    glVertex2f(mouseGameX, mouseGameY); // Posição do mouse
    glEnd();
    
    // NÃO chame glutSwapBuffers() aqui. O main.cpp fará isso.
}

GameAction game_keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'w':
        case 'W':
            movePlayer(0, PLAYER_SPEED);
            break;
        case 's':
        case 'S':
            movePlayer(0, -PLAYER_SPEED);
            break;
        case 'a':
        case 'A':
            movePlayer(-PLAYER_SPEED, 0);
            break;
        case 'd':
        case 'D':
            movePlayer(PLAYER_SPEED, 0);
            break;
        case 'q': // Sair para o menu
        case 'Q':
            return GAME_ACTION_EXIT_TO_MENU;
    }
    return GAME_ACTION_CONTINUE;
}

GameAction game_special_keys(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:
            movePlayer(0, PLAYER_SPEED);
            break;
        case GLUT_KEY_DOWN:
            movePlayer(0, -PLAYER_SPEED);
            break;
        case GLUT_KEY_LEFT:
            movePlayer(-PLAYER_SPEED, 0);
            break;
        case GLUT_KEY_RIGHT:
            movePlayer(PLAYER_SPEED, 0);
            break;
    }
    return GAME_ACTION_CONTINUE;
}

void game_mouse_motion(int x, int y) {
    // Converte as coordenadas do mouse (pixel, 0,0 top-left)
    // para as coordenadas do JOGO (mundo, 0,0 bottom-left)
    
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    if (winW == 0) winW = 1;
    if (winH == 0) winH = 1;

    // (float)x / winW -> Posição X percentual (0.0 a 1.0)
    // (float)(winH - y) / winH -> Posição Y percentual (0.0 a 1.0), invertida
    
    mouseGameX = (float)x / winW * GAME_WORLD_WIDTH;
    mouseGameY = (float)(winH - y) / winH * GAME_WORLD_HEIGHT;
}