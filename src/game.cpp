#include "game.h"
#include <GL/freeglut.h>
#include <stdio.h>  // Para sprintf
#include <math.h>   // Para sqrt

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

// --- Constantes de Física e Mundo ---
const float GRAVITY = -0.3f;
const float FRICTION = 0.90f;
const float DAMPING_FACTOR = 0.995f; // Amortecimento do balanço
const float PULL_STRENGTH = 0.5f;    // <<< FORÇA DE PUXADA (RE-ADICIONADA)

// Visão da Câmera
const float VIEW_WIDTH = 800.0f;
const float VIEW_HEIGHT = 600.0f;

// Tamanho da Fase
const float WORLD_WIDTH = 3000.0f;
const float WORLD_HEIGHT = 600.0f;

// --- Estruturas de Jogo ---
struct GameObject {
    float x, y, w, h;
    float r, g, b;
    float velocityX, velocityY;
};

// --- Variáveis Globais do Jogo ---
GameObject player;
GameObject door;
GameObject floorObj;
GameObject ceiling; // O teto ancorável

int CURRENT_LEVEL = 1;
bool isGrounded = false;

// Estado do Gancho
bool isHooked = false;
float hookPointX = 0;
float hookPointY = 0;
float ropeLength = 0; // Comprimento da corda

// Mira
float mouseGameX = 0;
float mouseGameY = 0;
float aimMagnitude = 0;

// Câmera
float cameraLeft = 0;
float cameraBottom = 0;

// --- Funções Internas do Jogo ---

void drawRect(const GameObject& obj) {
    glColor3f(obj.r, obj.g, obj.b);
    glBegin(GL_QUADS);
    glVertex2f(obj.x, obj.y);
    glVertex2f(obj.x + obj.w, obj.y);
    glVertex2f(obj.x + obj.w, obj.y + obj.h);
    glVertex2f(obj.x, obj.y + obj.h);
    glEnd();
}

void game_drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Checa se um ponto (x,y) está dentro de um objeto
bool isPointInside(float x, float y, const GameObject& obj) {
    return (x >= obj.x && x <= obj.x + obj.w &&
            y >= obj.y && y <= obj.y + obj.h);
}

// --- Implementação da Interface (game.h) ---

void game_init() {
    // Chão (tamanho do mundo)
    floorObj.x = 0;
    floorObj.y = 0;
    floorObj.w = WORLD_WIDTH;
    floorObj.h = 40;
    floorObj.r = 0.2f; floorObj.g = 0.6f; floorObj.b = 0.2f;

    // Teto (um bloco no meio do caminho)
    ceiling.x = 500;
    ceiling.y = 400;
    ceiling.w = 300;
    ceiling.h = 40;
    ceiling.r = 0.4f; ceiling.g = 0.4f; ceiling.b = 0.4f; // Cinza
}

void game_start_level(int level) {
    CURRENT_LEVEL = level;

    // Configura o jogador
    player.x = 50;
    player.y = floorObj.h;
    player.w = 40;
    player.h = 40;
    player.r = 0.9f; player.g = 0.1f; player.b = 0.1f;
    player.velocityX = 0;
    player.velocityY = 0;

    // Configura a porta
    door.x = WORLD_WIDTH - 200;
    door.y = floorObj.h;
    door.w = 30;
    door.h = 80;
    door.r = 0.5f; door.g = 0.3f; door.b = 0.0f;

    // Reseta estados
    isGrounded = true;
    isHooked = false;
    ropeLength = 0;
}

void game_reshape(int w, int h) {
    glViewport(0, 0, w, h); // Apenas define o viewport
}

GameAction game_update() {
    // --- 1. Aplicar Forças Externas (Gravidade) ---
    if (!isGrounded) {
        player.velocityY += GRAVITY;
    }

    // --- 2. LÓGICA DO GANCHO (PUXADA) ---
    // Esta é a força de puxada (o "lançamento" / DMT)
    if (isHooked) {
        float playerCenterX = player.x + player.w / 2;
        float playerCenterY = player.y + player.h / 2;

        float vecToHookX = hookPointX - playerCenterX;
        float vecToHookY = hookPointY - playerCenterY;
        float currentDist = sqrt(vecToHookX * vecToHookX + vecToHookY * vecToHookY);

        if (currentDist > 0.01f) {
            // Vetor normalizado (direção da puxada)
            float normX = vecToHookX / currentDist;
            float normY = vecToHookY / currentDist;

            // Adiciona aceleração na direção do gancho
            player.velocityX += normX * PULL_STRENGTH;
            player.velocityY += normY * PULL_STRENGTH;
        }
    }

    // --- 3. Atualizar Posição (Baseado na velocidade) ---
    player.x += player.velocityX;
    player.y += player.velocityY;

    // --- 4. Resolver Restrições e Colisões ---
    isGrounded = false; // Assume que está no ar

    // Colisão com o chão
    if (player.y < floorObj.h) {
        player.y = floorObj.h;
        player.velocityY = 0;
        isGrounded = true;
        player.velocityX *= FRICTION; 
        if (isHooked) isHooked = false; // Solta o gancho se bater no chão
    }

    // Colisão com o teto (ancorável)
    if (player.x + player.w > ceiling.x && player.x < ceiling.x + ceiling.w &&
        player.y + player.h > ceiling.y && player.y < ceiling.y) {
        player.y = ceiling.y - player.h;
        player.velocityY = 0;
        if (isHooked) isHooked = false; // Solta o gancho se bater no teto
    }

    // Limites do Mundo
    if (player.x < 0) { player.x = 0; player.velocityX = 0; }
    if (player.x + player.w > WORLD_WIDTH) { player.x = WORLD_WIDTH - player.w; player.velocityX = 0; }

    // --- 5. LÓGICA DO PÊNDULO (RESTRIÇÃO DA CORDA) ---
    // Esta parte vem DEPOIS de atualizar a posição
    if (isHooked) {
        float playerCenterX = player.x + player.w / 2;
        float playerCenterY = player.y + player.h / 2;

        float vecToHookX = hookPointX - playerCenterX;
        float vecToHookY = hookPointY - playerCenterY;
        float currentDist = sqrt(vecToHookX * vecToHookX + vecToHookY * vecToHookY);

        // Se a corda estiver esticada
        if (currentDist > ropeLength && currentDist > 0.01f) {
            float stretch = currentDist - ropeLength;
            float normX = vecToHookX / currentDist;
            float normY = vecToHookY / currentDist;

            // Corrige a posição para ficar na ponta da corda
            player.x += normX * stretch;
            player.y += normY * stretch;

            // Remove a componente da velocidade que estica a corda
            // (para criar o "balanço" em vez de um "quicar" elástico)
            float dotProd = player.velocityX * normX + player.velocityY * normY;
            if (dotProd > 0) {
                player.velocityX -= normX * dotProd;
                player.velocityY -= normY * dotProd;
            }
        }
        
        // Aplica amortecimento (damping) para o balanço diminuir com o tempo
        player.velocityX *= DAMPING_FACTOR;
        player.velocityY *= DAMPING_FACTOR;
        
        isGrounded = false; // Garante que não estamos no chão se estivermos pendurados
    }

    // --- 6. Checar Vitória ---
    if (isPointInside(player.x + player.w / 2, player.y + player.h / 2, door)) {
        return GAME_ACTION_LEVEL_WON;
    }

    return GAME_ACTION_CONTINUE;
}

void game_display() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Céu
    glClear(GL_COLOR_BUFFER_BIT);

    // --- Câmera (Segue o Jogador) ---
    float cameraX = player.x + player.w / 2 - (VIEW_WIDTH / 2);
    if (cameraX < 0) cameraX = 0;
    if (cameraX + VIEW_WIDTH > WORLD_WIDTH) cameraX = WORLD_WIDTH - VIEW_WIDTH;
    cameraLeft = cameraX;
    cameraBottom = 0; 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(cameraLeft, cameraLeft + VIEW_WIDTH, cameraBottom, cameraBottom + VIEW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // --- Desenho do Mundo ---
    drawRect(floorObj);
    drawRect(ceiling);
    drawRect(door);
    drawRect(player);

    // --- Desenho da Mira (Sempre Ativo) ---
    float playerCenterX = player.x + player.w / 2;
    float playerCenterY = player.y + player.h / 2;

    float vecAimX = mouseGameX - playerCenterX;
    float vecAimY = mouseGameY - playerCenterY;
    aimMagnitude = sqrt(vecAimX * vecAimX + vecAimY * vecAimY);

    // Cor da mira (verde se pode prender, vermelha se não)
    if (isPointInside(mouseGameX, mouseGameY, ceiling)) {
        glColor3f(0.0f, 1.0f, 0.0f); // Verde
    } else {
        glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
    }
    glBegin(GL_LINES);
    glVertex2f(playerCenterX, playerCenterY);
    glVertex2f(mouseGameX, mouseGameY);
    glEnd();
    
    // Texto da magnitude (no meio da linha de mira)
    char magText[50];
    sprintf(magText, "Forca: %.0f", aimMagnitude);
    glColor3f(1.0f, 1.0f, 1.0f);
    game_drawText(playerCenterX + vecAimX / 2, playerCenterY + vecAimY / 2, magText);

    // --- Desenho da Corda (Se Estiver Preso) ---
    if (isHooked) {
        glColor3f(0.8f, 0.8f, 0.8f); // Cor da corda (cinza)
        glBegin(GL_LINES);
        glVertex2f(playerCenterX, playerCenterY);
        glVertex2f(hookPointX, hookPointY);
        glEnd();
    }
}

GameAction game_keyboard(unsigned char key, int x, int y) {
    if (key == 'q' || key == 'Q') {
        return GAME_ACTION_EXIT_TO_MENU;
    }
    return GAME_ACTION_CONTINUE;
}

void game_mouse_motion(int x, int y) {
    // Converte coordenadas de pixel para coordenadas de mundo (jogo)
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    if (winW == 0) winW = 1;
    if (winH == 0) winH = 1;

    mouseGameX = cameraLeft + (float)x / winW * VIEW_WIDTH;
    mouseGameY = cameraBottom + (float)(winH - y) / winH * VIEW_HEIGHT;
}

// Lógica de clique (Segurar/Soltar)
void game_mouse_click(int button, int state) {
    if (button == GLUT_LEFT_BUTTON) {
        
        // Pressionou o botão: Tenta prender
        if (state == GLUT_DOWN) {
            if (!isHooked) {
                // Verifica se a mira está em cima do teto
                if (isPointInside(mouseGameX, mouseGameY, ceiling)) {
                    isHooked = true;
                    hookPointX = mouseGameX;
                    hookPointY = mouseGameY;
                    
                    // Define o comprimento da corda
                    float dx = hookPointX - (player.x + player.w / 2);
                    float dy = hookPointY - (player.y + player.h / 2);
                    ropeLength = sqrt(dx*dx + dy*dy);
                    
                    isGrounded = false; // Sai do chão
                }
            }
        }
        // Soltou o botão: Solta o gancho
        else if (state == GLUT_UP) {
            if (isHooked) {
                isHooked = false;
            }
        }
    }
}