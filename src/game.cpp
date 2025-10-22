#include "game.h"
#include <GL/freeglut.h>
#include <stdio.h>  // Para sprintf
#include <math.h>   // Para sqrt
#include <string>   // Para std::string

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

// --- Constantes de Física (Lentas) ---
const float GRAVITY = -0.1f;        // Aceleração da gravidade (m/s^2 ou pixels/frame^2)
const float PLAYER_WALK_ACCEL = 0.1f; // Aceleração ao andar
const float MAX_WALK_SPEED = 3.0f;    // Velocidade máxima de caminhada
const float PULL_STRENGTH = 0.2f;   // Aceleração da puxada
const float DAMPING_FACTOR = 0.99f; // <<< ADICIONE ESTA LINHA (Amortecimento do balanço)

// --- Constantes do Gancho ---
const float HOOK_SPEED = 25.0f;
const float HOOK_MAX_RANGE = 700.0f;
const float MAX_PULL_STRENGTH_PHYSICS = 0.4f; // Aceleração MÁXIMA que o gancho pode aplicar

// --- Constantes do Mundo e Visualização ---
const float PLAYER_HEIGHT = 40.0f;
const float VIEW_WIDTH = 800.0f;
const float VIEW_HEIGHT = 600.0f;
const float WORLD_WIDTH = 3000.0f;
const float WORLD_HEIGHT = 600.0f;

// --- NOVO: Lógica de Escala de Vetores ---
// 1. O vetor de mira (visual) terá no máximo 5x a altura do personagem
const float MAX_VISUAL_AIM_LENGTH = 5.0f * PLAYER_HEIGHT; // 5 * 40 = 200 pixels
// 2. O valor de "Força" exibido (abstrato) será no máximo 100
const float MAX_AIM_FORCE_DISPLAY = 100.0f;
// 3. A escala global de vetores: 
//    Uma aceleração de 'MAX_PULL_STRENGTH_PHYSICS' (ex: 0.4)
//    deve ser desenhada com 'MAX_VISUAL_AIM_LENGTH' (ex: 200) pixels.
//    Portanto, a escala é 200 / 0.4 = 500.
const float VECTOR_VISUAL_SCALE = MAX_VISUAL_AIM_LENGTH / MAX_PULL_STRENGTH_PHYSICS;


// --- Estruturas Modulares ---
struct GameObject {
    float x, y, w, h;
    float r, g, b;
    float velocityX, velocityY;
};

struct Platform {
    float x, y, w, h;
    float r, g, b;
    bool isHookable;
    float frictionCoefficient; // (mu)
};

// --- Variáveis Globais do Jogo ---
GameObject player;
GameObject door;
Platform platforms[10];
int numPlatforms = 0;
Platform* collidingPlatform = NULL;

int CURRENT_LEVEL = 1;
bool isGrounded = false;

// Estado do Gancho
bool isHooked = false;
bool isHookFiring = false;
float hookPointX = 0, hookPointY = 0;
float hookProjectileX = 0, hookProjectileY = 0;
float hookProjectileVelX = 0, hookProjectileVelY = 0;
float hookCurrentRange = 0;
float ropeLength = 0;
float currentPullForce = 0.0f; // A força (aceleração) real do gancho

// Mira
float mouseGameX = 0, mouseGameY = 0;
float aimDisplayForce = 0; // A força (0-100) para mostrar na tela

// Câmera
float cameraLeft = 0, cameraBottom = 0;

// Input de Movimento
bool isPressingLeft = false;
bool isPressingRight = false;

// Vetores para visualização (valores REAIS de aceleração)
float forceNormal = 0;
float forceFriction = 0;
float forceTensionX = 0;
float forceTensionY = 0;

// --- Funções Internas do Jogo ---

void drawRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void game_drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawVector(float startX, float startY, float vX_accel, float vY_accel, 
                float r, float g, float b, const char* label) {
    // Converte aceleração em tamanho visual
    float vX_visual = vX_accel * VECTOR_VISUAL_SCALE;
    float vY_visual = vY_accel * VECTOR_VISUAL_SCALE;

    float endX = startX + vX_visual;
    float endY = startY + vY_visual;

    glColor3f(r, g, b);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(startX, startY);
    glVertex2f(endX, endY);
    glEnd();
    glLineWidth(1.0f);

    float angle = atan2(vY_visual, vX_visual);
    if (vX_visual == 0 && vY_visual == 0) angle = 0; // Evita NaN
    float size = 10.0f;
    glBegin(GL_TRIANGLES);
    glVertex2f(endX, endY);
    glVertex2f(endX - size * cos(angle - 0.5f), endY - size * sin(angle - 0.5f));
    glVertex2f(endX - size * cos(angle + 0.5f), endY - size * sin(angle + 0.5f));
    glEnd();

    game_drawText(endX + 5, endY + 5, label);
}

bool isPointInside(float x, float y, float objX, float objY, float objW, float objH) {
    return (x >= objX && x <= objX + objW &&
            y >= objY && y <= objY + objH);
}

// --- Implementação da Interface (game.h) ---

void game_init() {
    numPlatforms = 2;

    // Plataforma 0: Chão
    platforms[0].x = 0;
    platforms[0].y = 0;
    platforms[0].w = WORLD_WIDTH;
    platforms[0].h = 40;
    platforms[0].r = 0.2f; platforms[0].g = 0.6f; platforms[0].b = 0.2f;
    platforms[0].isHookable = true;
    platforms[0].frictionCoefficient = 0.8f; // mu ALTO (para parar rápido)

    // Plataforma 1: Teto
    platforms[1].x = 500;
    platforms[1].y = 400;
    platforms[1].w = 300;
    platforms[1].h = 40;
    platforms[1].r = 0.4f; platforms[1].g = 0.4f; platforms[1].b = 0.4f;
    platforms[1].isHookable = true;
    platforms[1].frictionCoefficient = 0.1f; // mu BAIXO
}

void game_start_level(int level) {
    player.x = 50;
    player.y = platforms[0].h;
    player.w = 40;
    player.h = PLAYER_HEIGHT;
    player.r = 0.9f; player.g = 0.1f; player.b = 0.1f;
    player.velocityX = 0;
    player.velocityY = 0;

    door.x = WORLD_WIDTH - 200;
    door.y = platforms[0].h;
    door.w = 30;
    door.h = 80;
    door.r = 0.5f; door.g = 0.3f; door.b = 0.0f;

    isGrounded = true;
    isHooked = false;
    isHookFiring = false;
    collidingPlatform = &platforms[0];
    isPressingLeft = false;
    isPressingRight = false;
}

void game_reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GameAction game_update() {
    // --- 1. Resetar Forças de Visualização ---
    forceNormal = 0;
    forceFriction = 0;
    forceTensionX = 0;
    forceTensionY = 0;

    // --- 2. Aplicar Acelerações (Forças) ---
    float accelX = 0;
    float accelY = GRAVITY; // Começa com gravidade

    // Aceleração do Gancho (Puxada)
    if (isHooked) {
        float playerCenterX = player.x + player.w / 2;
        float playerCenterY = player.y + player.h / 2;
        float vecToHookX = hookPointX - playerCenterX;
        float vecToHookY = hookPointY - playerCenterY;
        float currentDist = sqrt(vecToHookX * vecToHookX + vecToHookY * vecToHookY);

        if (currentDist > 0.01f) {
            float normX = vecToHookX / currentDist;
            float normY = vecToHookY / currentDist;
            
            // Usa a força definida no momento do disparo
            accelX += normX * currentPullForce;
            accelY += normY * currentPullForce;
            
            // Guarda para visualização
            forceTensionX = normX * currentPullForce;
            forceTensionY = normY * currentPullForce;
        }
    }

    // Aceleração de Andar (Chão)
    if (isGrounded) {
        if (isPressingLeft) accelX -= PLAYER_WALK_ACCEL;
        if (isPressingRight) accelX += PLAYER_WALK_ACCEL;
    }

    // --- 3. Aplicar Fricção (se no chão) ---
    // (Nova física de força de atrito)
    if (isGrounded && collidingPlatform != NULL) {
        forceNormal = -GRAVITY; // Força normal (assume massa=1, N=g)
        float frictionAccel = collidingPlatform->frictionCoefficient * forceNormal;

        // Se o atrito for mais forte que a velocidade + aceleração, pare
        if (fabs(player.velocityX + accelX) < frictionAccel) {
            player.velocityX = 0;
            accelX = 0; // Atrito estático cancela a aceleração
            forceFriction = 0;
        } else {
            // Atrito cinético
            if (player.velocityX > 0) {
                accelX -= frictionAccel;
                forceFriction = -frictionAccel;
            } else if (player.velocityX < 0) {
                accelX += frictionAccel;
                forceFriction = frictionAccel;
            }
        }
    }

    // --- 4. Atualizar Velocidade e Posição ---
    player.velocityX += accelX;
    player.velocityY += accelY;

    // Limitar velocidade de caminhada
    if (isGrounded && fabs(player.velocityX) > MAX_WALK_SPEED) {
        player.velocityX = (player.velocityX > 0) ? MAX_WALK_SPEED : -MAX_WALK_SPEED;
    }

    player.x += player.velocityX;
    player.y += player.velocityY;

    // --- 5. Resolver Colisões (Plataformas e Limites do Mundo) ---
    isGrounded = false;
    collidingPlatform = NULL;

    for (int i = 0; i < numPlatforms; i++) {
        Platform* p = &platforms[i];
        if (player.x < p->x + p->w && player.x + player.w > p->x &&
            player.y < p->y + p->h && player.y + player.h > p->y)
        {
            if (player.velocityY > 0 && (player.y + player.h) > p->y && player.y < p->y) {
                player.y = p->y - player.h;
                player.velocityY = 0;
                if (isHooked) isHooked = false; 
            }
            else if (player.velocityY <= 0 && player.y < (p->y + p->h) && (player.y + player.h) > (p->y + p->h)) {
                player.y = p->y + p->h;
                isGrounded = true;
                collidingPlatform = p;
                forceNormal = -GRAVITY - (player.velocityY * 0.5f); // Impacto
                player.velocityY = 0;
                if (isHooked) isHooked = false;
            }
        }
    }

    // RE-ADICIONADO: Limites do Mundo
    if (player.x < 0) {
        player.x = 0;
        player.velocityX = 0;
    }
    if (player.x + player.w > WORLD_WIDTH) {
        player.x = WORLD_WIDTH - player.w;
        player.velocityX = 0;
    }


    // --- 6. LÓGICA DO PÊNDULO (Restrição da Corda) ---
    if (isHooked) {
        float playerCenterX = player.x + player.w / 2;
        float playerCenterY = player.y + player.h / 2;
        float vecToHookX = hookPointX - playerCenterX;
        float vecToHookY = hookPointY - playerCenterY;
        float currentDist = sqrt(vecToHookX * vecToHookX + vecToHookY * vecToHookY);

        if (currentDist > ropeLength && currentDist > 0.01f) {
            float stretch = currentDist - ropeLength;
            float normX = vecToHookX / currentDist;
            float normY = vecToHookY / currentDist;
            player.x += normX * stretch;
            player.y += normY * stretch;
            float dotProd = player.velocityX * normX + player.velocityY * normY;
            if (dotProd > 0) {
                player.velocityX -= normX * dotProd;
                player.velocityY -= normY * dotProd;
            }
        }
        player.velocityX *= DAMPING_FACTOR;
        player.velocityY *= DAMPING_FACTOR;
        isGrounded = false;
    }

    // --- 7. LÓGICA DE DISPARO DO GANCHO ---
    if (isHookFiring) {
        hookProjectileX += hookProjectileVelX;
        hookProjectileY += hookProjectileVelY;
        hookCurrentRange += HOOK_SPEED;
        bool hit = false;

        for (int i = 0; i < numPlatforms; i++) {
            Platform* p = &platforms[i];
            if (p->isHookable && isPointInside(hookProjectileX, hookProjectileY, p->x, p->y, p->w, p->h)) {
                hit = true;
                isHookFiring = false;
                isHooked = true;
                hookPointX = hookProjectileX;
                hookPointY = hookProjectileY;
                float dx = hookPointX - (player.x + player.w / 2);
                float dy = hookPointY - (player.y + player.h / 2);
                ropeLength = sqrt(dx*dx + dy*dy);
                isGrounded = false;
                break;
            }
        }
        if (!hit && hookCurrentRange > HOOK_MAX_RANGE) {
            isHookFiring = false; // Miss
        }
    }

    // --- 8. Checar Vitória ---
    if (isPointInside(player.x, player.y, door.x, door.y, door.w, door.h)) {
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

    // --- Desenho do Mundo (Modular) ---
    for (int i = 0; i < numPlatforms; i++) {
        Platform* p = &platforms[i];
        drawRect(p->x, p->y, p->w, p->h, p->r, p->g, p->b);
        if (p->frictionCoefficient > 0) {
            char muText[20];
            sprintf(muText, "mu: %.2f", p->frictionCoefficient);
            glColor3f(1.0, 1.0, 1.0);
            game_drawText(p->x + 20, p->y + 15, muText);
        }
    }
    drawRect(door.x, door.y, door.w, door.h, door.r, door.g, door.b);
    drawRect(player.x, player.y, player.w, player.h, player.r, player.g, player.b);

    // --- Desenho da Mira (com CLAMP) ---
    float playerCenterX = player.x + player.w / 2;
    float playerCenterY = player.y + player.h / 2;
    float vecAimX = mouseGameX - playerCenterX;
    float vecAimY = mouseGameY - playerCenterY;
    float rawMouseDist = sqrt(vecAimX * vecAimX + vecAimY * vecAimY);
    
    float clampedMouseDist = fmin(rawMouseDist, MAX_VISUAL_AIM_LENGTH);
    aimDisplayForce = (clampedMouseDist / MAX_VISUAL_AIM_LENGTH) * MAX_AIM_FORCE_DISPLAY;

    float vecAimNormX = 0, vecAimNormY = 0;
    if (rawMouseDist > 0.01f) {
        vecAimNormX = vecAimX / rawMouseDist;
        vecAimNormY = vecAimY / rawMouseDist;
    }

    // Posição final do vetor de mira visual (travado)
    float aimVisualEndX = playerCenterX + vecAimNormX * clampedMouseDist;
    float aimVisualEndY = playerCenterY + vecAimNormY * clampedMouseDist;

    // Linha de mira (pontilhada)
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineStipple(1, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glVertex2f(playerCenterX, playerCenterY);
    glVertex2f(aimVisualEndX, aimVisualEndY);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    
    char magText[50];
    sprintf(magText, "Forca: %.1f", aimDisplayForce);
    glColor3f(1.0f, 1.0f, 1.0f);
    game_drawText(aimVisualEndX + 5, aimVisualEndY + 5, magText);

    // --- Desenho da Corda (Disparando ou Presa) ---
    if (isHookFiring) {
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES);
        glVertex2f(playerCenterX, playerCenterY);
        glVertex2f(hookProjectileX, hookProjectileY);
        glEnd();
    }
    if (isHooked) {
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES);
        glVertex2f(playerCenterX, playerCenterY);
        glVertex2f(hookPointX, hookPointY);
        glEnd();
    }

    // --- DESENHO DOS VETORES DE FÍSICA ---
    char vecLabel[50];

    // 1. Vetor Peso (Gravidade)
    sprintf(vecLabel, "Peso: %.2f", GRAVITY);
    drawVector(playerCenterX, playerCenterY, 0, GRAVITY, 
               0.2f, 0.2f, 1.0f, vecLabel);

    // 2. Vetor Velocidade (em m/s, não aceleração, então escalamos por 10)
    if (fabs(player.velocityX) > 0.01 || fabs(player.velocityY) > 0.01) {
        float velMag = sqrt(player.velocityX*player.velocityX + player.velocityY*player.velocityY);
        sprintf(vecLabel, "Vel: %.1f", velMag);
        drawVector(playerCenterX, playerCenterY, player.velocityX * 10.0f, player.velocityY * 10.0f, 
                   1.0f, 0.5f, 0.0f, vecLabel);
    }

    // 3. Vetor Tensão (Gancho)
    if (isHooked) {
        sprintf(vecLabel, "Tensao: %.2f", currentPullForce);
        drawVector(playerCenterX, playerCenterY, forceTensionX, forceTensionY, 
                   1.0f, 0.0f, 1.0f, vecLabel);
    }

    // 4. Vetores de Contato (Chão)
    if (isGrounded) {
        // Vetor Normal
        sprintf(vecLabel, "Normal: %.2f", forceNormal);
        drawVector(playerCenterX, player.y, 0, forceNormal, 
                   0.0f, 1.0f, 1.0f, vecLabel);
        
        // Vetor Atrito
        if (fabs(forceFriction) > 0.001f) {
            sprintf(vecLabel, "Atrito: %.2f", forceFriction);
            drawVector(playerCenterX, player.y + 10, forceFriction, 0,
                       1.0f, 0.0f, 0.0f, vecLabel);
        }
    }
}

// --- Funções de Input ---

GameAction game_key_down(unsigned char key, int x, int y) {
    switch (key) {
        case 'q':
        case 'Q':
            return GAME_ACTION_EXIT_TO_MENU;
        case 'a':
        case 'A':
            isPressingLeft = true;
            break;
        case 'd':
        case 'D':
            isPressingRight = true;
            break;
    }
    return GAME_ACTION_CONTINUE;
}

void game_key_up(unsigned char key, int x, int y) {
    switch (key) {
        case 'a':
        case 'A':
            isPressingLeft = false;
            break;
        case 'd':
        case 'D':
            isPressingRight = false;
            break;
    }
}

void game_special_down(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            isPressingLeft = true;
            break;
        case GLUT_KEY_RIGHT:
            isPressingRight = true;
            break;
    }
}

void game_special_up(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            isPressingLeft = false;
            break;
        case GLUT_KEY_RIGHT:
            isPressingRight = false;
            break;
    }
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

// Lógica de clique (Disparar / Soltar)
void game_mouse_click(int button, int state) {
    // Ação apenas ao PRESSIONAR
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        
        // Se já está preso, clique solta
        if (isHooked) {
            isHooked = false;
        } 
        // Se o gancho está viajando, clique cancela
        else if (isHookFiring) {
            isHookFiring = false;
        }
        // Se está livre, dispara o gancho
        else {
            isHookFiring = true;
            float playerCenterX = player.x + player.w / 2;
            float playerCenterY = player.y + player.h / 2;
            hookProjectileX = playerCenterX;
            hookProjectileY = playerCenterY;
            
            float vecAimX = mouseGameX - playerCenterX;
            float vecAimY = mouseGameY - playerCenterY;
            float rawMouseDist = sqrt(vecAimX*vecAimX + vecAimY*vecAimY);

            // Calcula a força real (aceleração) baseada no vetor de mira
            float clampedMouseDist = fmin(rawMouseDist, MAX_VISUAL_AIM_LENGTH);
            float forcePercent = clampedMouseDist / MAX_VISUAL_AIM_LENGTH;
            currentPullForce = forcePercent * MAX_PULL_STRENGTH_PHYSICS;
            
            // Define a velocidade do projétil do gancho
            if (rawMouseDist > 0.01f) {
                hookProjectileVelX = (vecAimX / rawMouseDist) * HOOK_SPEED;
                hookProjectileVelY = (vecAimY / rawMouseDist) * HOOK_SPEED;
            } else {
                hookProjectileVelX = 0;
                hookProjectileVelY = HOOK_SPEED; // Dispara para cima se clicar em si mesmo
            }
            hookCurrentRange = 0;
        }
    }
}