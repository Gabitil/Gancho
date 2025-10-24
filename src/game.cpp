#include "game.h"
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm> // Para std::min

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

// --- Constantes de Física ---
// Valores que definem forças e parâmetros físicos gerais do personagem.
const float GRAVITY = -0.1f;              // Aceleração vertical (negativa = para baixo)
const float PLAYER_WALK_ACCEL = 0.1f;     // Aceleração horizontal aplicada ao caminhar
const float MAX_WALK_SPEED = 3.0f;        // Velocidade máxima permitida enquanto "no chão"
const float DAMPING_FACTOR = 0.99f;       // Fator de amortecimento aplicado quando pendurado

// --- Constantes do Gancho ---
// Parâmetros que controlam o comportamento do gancho/arpão.
const float HOOK_SPEED = 25.0f;             // Velocidade do projétil do gancho
const float MAX_PULL_STRENGTH_PHYSICS = 0.4f;   // Força máxima que o gancho aplica ao jogador

// --- Limite de Velocidade e Escala Visual ---
const float MAX_PLAYER_SPEED = 30.0f; // Limita velocidade total (evita valores numéricos muito grandes)

// --- Constantes do Mundo e Visualização ---
const float PLAYER_HEIGHT = 40.0f;
const float VIEW_WIDTH = 800.0f;  // Largura da câmera / janela de visualização em unidades do jogo
const float VIEW_HEIGHT = 600.0f; // Altura da câmera
const float WORLD_WIDTH = 3000.0f; // Dimensão horizontal total do mundo
const float WORLD_HEIGHT = 600.0f; // Dimensão vertical (não muito usada neste código além de limites)

// --- Lógica de Escala de Vetores ---
// Esses fatores são usados para converter grandezas físicas (força, velocidade)
// em representações visuais (comprimentos de vetores desenhados na tela).
const float MAX_VISUAL_AIM_LENGTH = 5.0f * PLAYER_HEIGHT; // comprimento máximo visível do vetor de mira
const float MAX_AIM_FORCE_DISPLAY = 100.0f;               // valor numérico mostrado na HUD como "força"
const float VECTOR_VISUAL_SCALE = MAX_VISUAL_AIM_LENGTH / MAX_PULL_STRENGTH_PHYSICS; // escala para desenhar vetores de força
const float VELOCITY_VISUAL_SCALE = 7.0f; // escala para desenhar vetor de velocidade

// --- Estruturas Modulares ---
// GameObject representa objetos com posição, tamanho, cor e velocidade.
struct GameObject {
    float x, y, w, h;            // posição (x,y) e dimensão (w=largura, h=altura)
    float r, g, b;               // cor para desenho (RGB entre 0 e 1)
    float velocityX, velocityY; // velocidades atuais nas direções X e Y
};

// Plataformas possuem propriedades adicionais como atrito e se podem ser fixadas pelo gancho.
struct Platform {
    float x, y, w, h;            // retângulo da plataforma
    float r, g, b;               // cor
    bool isHookable;             // se o gancho pode "fixar" nessa plataforma
    float frictionCoefficient;   // coeficiente de atrito para física
};

// --- Variáveis Globais do Jogo ---
// Objetos do jogo e estado global. Em um projeto maior você preferiria
// encapsular em classes/structs em vez de variáveis globais.
GameObject player;
GameObject door;
Platform platforms[10];
int numPlatforms = 0;
Platform* collidingPlatform = NULL; // ponteiro para plataforma com a qual o jogador está em contato
int CURRENT_LEVEL = 1;
bool isGrounded = false;      // indica se o jogador está no chão (ou em cima de uma plataforma)
bool isHooked = false;        // se o gancho está fixado em um ponto
bool isHookFiring = false;    // se o projétil do gancho está em voo
float hookPointX = 0, hookPointY = 0; // ponto onde o gancho está preso (se isHooked)
float hookProjectileX = 0, hookProjectileY = 0; // posição do projétil enquanto voa
float hookProjectileVelX = 0, hookProjectileVelY = 0; // velocidade do projétil
float ropeLength = 0; // comprimento do "cordão" entre jogador e ponto do gancho
float currentPullForce = 0.0f; // força atual aplicada pelo gancho (física)
float mouseGameX = 0, mouseGameY = 0; // posição do mouse convertida para coordenadas do mundo
float aimDisplayForce = 0; // valor calculado para exibir a força de mira no HUD
float cameraLeft = 0, cameraBottom = 0; // canto inferior-esquerdo da câmera/viewport no mundo
bool isPressingLeft = false; // input: pressionando esquerda
bool isPressingRight = false; // input: pressionando direita
float forceNormal = 0;       // força normal (reação do solo)
float forceFriction = 0;     // força de atrito
float forceTensionX = 0;     // componente X da tensão do gancho (para debug)
float forceTensionY = 0;     // componente Y da tensão do gancho (para debug)

// Variáveis para cálculo de aceleração (derivada da magnitude da velocidade)
float lastVelocityMag = 0.0f;
float currentAcceleration = 0.0f;

// --- Funções Auxiliares Internas ---

// Desenha um retângulo preenchido (usado para jogador, plataformas, porta e fundo do HUD)
void drawRect(float x, float y, float w, float h, float r, float g, float b) {
    glEnable(GL_BLEND); // Habilita blending para transparência
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, 0.8f); // ALTERADO: Usa glColor4f para adicionar canal alfa (transparência)
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + w, y); glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();
    glDisable(GL_BLEND);
}

// Desenha texto bitmap na posição (x,y) usando uma fonte GLUT
void game_drawText(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) {
        glutBitmapCharacter(font, *c);
    }
}

// Verifica se um ponto (x,y) está dentro do retângulo de um objeto
bool isPointInside(float x, float y, float objX, float objY, float objW, float objH) {
    return (x >= objX && x <= objX + objW &&
            y >= objY && y <= objY + objH);
}

// Retorna largura (em pixels) do texto usando glutBitmapWidth caractere-a-caractere
int getTextWidth(const char* text, void* font) {
    int width = 0;
    for (const char* c = text; *c; ++c) {
        width += glutBitmapWidth(font, *c);
    }
    return width;
}

// Interseção entre linha (x1,y1)-(x2,y2) e retângulo da plataforma p.
// Retorna true se a linha intersecta o retângulo; hitX/hitY contém o ponto de impacto.
bool lineRectIntersection(float x1, float y1, float x2, float y2, const Platform& p, float& hitX, float& hitY) {
    float t0 = 0.0f, t1 = 1.0f;
    float dx = x2 - x1, dy = y2 - y1;
    auto clipTest = [&](float p_val, float q_val, float& t_0, float& t_1) {
        if (fabs(p_val) < 1e-6) { return q_val >= 0; }
        float r = q_val / p_val;
        if (p_val < 0) {
            if (r > t_1) return false;
            if (r > t_0) t_0 = r;
        } else {
            if (r < t_0) return false;
            if (r < t_1) t_1 = r;
        }
        return true;
    };
    if (!clipTest(-dx, x1 - p.x, t0, t1)) return false;
    if (!clipTest(dx, p.x + p.w - x1, t0, t1)) return false;
    if (!clipTest(-dy, y1 - p.y, t0, t1)) return false;
    if (!clipTest(dy, p.y + p.h - y1, t0, t1)) return false;
    hitX = x1 + t0 * dx;
    hitY = y1 + t0 * dy;
    return true;
}

// "Corta" uma linha para que seu ponto final (endX,endY) fique dentro da viewport
void clipLineToView(float startX, float startY, float& endX, float& endY) {
    float viewRight = cameraLeft + VIEW_WIDTH;
    float viewTop = cameraBottom + VIEW_HEIGHT;
    float dx = endX - startX;
    float dy = endY - startY;
    if (dx == 0 && dy == 0) return;
    float t = 1.0;
    if (endX < cameraLeft)  t = std::min(t, (cameraLeft - startX) / dx);
    if (endX > viewRight)   t = std::min(t, (viewRight - startX) / dx);
    if (endY < cameraBottom)t = std::min(t, (cameraBottom - startY) / dy);
    if (endY > viewTop)     t = std::min(t, (viewTop - startY) / dy);
    endX = startX + t * dx;
    endY = startY + t * dy;
}

// Desenha um vetor com ponta de seta.
void drawVector(float startX, float startY, float vX_physics, float vY_physics, float scale,
                  float r, float g, float b, const char* label) {
    
    float vX_visual = vX_physics * scale;
    float vY_visual = vY_physics * scale;
    float endX = startX + vX_visual;
    float endY = startY + vY_visual;
    
    glColor3f(r, g, b); glLineWidth(2.0f);
    glBegin(GL_LINES); glVertex2f(startX, startY); glVertex2f(endX, endY); glEnd();
    glLineWidth(1.0f);
    
    float angle = atan2(vY_visual, vX_visual);
    if (vX_visual == 0 && vY_visual == 0) angle = 0;
    float size = 10.0f;
    
    glBegin(GL_TRIANGLES);
    glVertex2f(endX, endY);
    glVertex2f(endX - size * cos(angle - 0.5f), endY - size * sin(angle - 0.5f));
    glVertex2f(endX - size * cos(angle + 0.5f), endY - size * sin(angle + 0.5f));
    glEnd();
    
    // ALTERADO: Rótulo do vetor agora é desenhado na cena
    game_drawText(endX + 5, endY + 5, label, GLUT_BITMAP_9_BY_15);
}

// --- Funções Principais do Jogo ---

// Inicialização do mundo
void game_init() {
    numPlatforms = 2;
    platforms[0] = {0, 0, WORLD_WIDTH, 40, 0.2f, 0.6f, 0.2f, true, 0.8f};
    platforms[1] = {500, 400, 300, 40, 0.4f, 0.4f, 0.4f, true, 0.1f};
}

// Prepara o nível para iniciar/reiniciar
void game_start_level(int level) {
    player = {50, platforms[0].h, 40, PLAYER_HEIGHT, 0.9f, 0.1f, 0.1f, 0, 0};
    door = {WORLD_WIDTH - 200, platforms[0].h, 30, 80, 0.5f, 0.3f, 0.0f};
    isGrounded = true; isHooked = false; isHookFiring = false;
    collidingPlatform = &platforms[0]; isPressingLeft = false; isPressingRight = false;
    lastVelocityMag = 0.0f;
    currentAcceleration = 0.0f;
}

void game_reshape(int w, int h) { glViewport(0, 0, w, h); }

// A função central de atualização do jogo (física, colisões, etc.)
GameAction game_update() {
    forceNormal = 0; forceFriction = 0; forceTensionX = 0; forceTensionY = 0;
    float accelX = 0, accelY = GRAVITY;

    if (isHooked) {
        float playerCenterX = player.x + player.w / 2, playerCenterY = player.y + player.h / 2;
        float vecToHookX = hookPointX - playerCenterX, vecToHookY = hookPointY - playerCenterY;
        float dist = sqrt(vecToHookX * vecToHookX + vecToHookY * vecToHookY);
        if (dist > 0.01f) {
            float normX = vecToHookX / dist, normY = vecToHookY / dist;
            accelX += normX * currentPullForce; accelY += normY * currentPullForce;
            forceTensionX = normX * currentPullForce; forceTensionY = normY * currentPullForce;
        }
    }

    if (isGrounded) {
        if (isPressingLeft) accelX -= PLAYER_WALK_ACCEL;
        if (isPressingRight) accelX += PLAYER_WALK_ACCEL;
    }

    if (isGrounded && collidingPlatform) {
        forceNormal = -GRAVITY;
        float frictionAccel = collidingPlatform->frictionCoefficient * forceNormal;
        if (fabs(player.velocityX + accelX) < frictionAccel) {
            player.velocityX = 0; accelX = 0; forceFriction = 0;
        } else {
            if (player.velocityX > 0) { accelX -= frictionAccel; forceFriction = -frictionAccel; }
            else if (player.velocityX < 0) { accelX += frictionAccel; forceFriction = frictionAccel; }
        }
    }

    player.velocityX += accelX; player.velocityY += accelY;

    if (isGrounded && fabs(player.velocityX) > MAX_WALK_SPEED) {
        player.velocityX = (player.velocityX > 0) ? MAX_WALK_SPEED : -MAX_WALK_SPEED;
    }

    float speed = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    if (speed > MAX_PLAYER_SPEED) {
        player.velocityX = (player.velocityX / speed) * MAX_PLAYER_SPEED;
        player.velocityY = (player.velocityY / speed) * MAX_PLAYER_SPEED;
    }

    player.x += player.velocityX; player.y += player.velocityY;

    isGrounded = false; collidingPlatform = NULL;
    for (int i = 0; i < numPlatforms; i++) {
        Platform* p = &platforms[i];
        if (player.x < p->x + p->w && player.x + player.w > p->x && player.y < p->y + p->h && player.y + player.h > p->y) {
            if (player.velocityY > 0 && (player.y + player.h) > p->y && player.y < p->y) {
                player.y = p->y - player.h; player.velocityY = 0; if (isHooked) isHooked = false;
            } else if (player.velocityY <= 0 && player.y < (p->y + p->h) && (player.y + player.h) > (p->y + p->h)) {
                player.y = p->y + p->h; isGrounded = true; collidingPlatform = p;
                forceNormal = -GRAVITY - (player.velocityY * 0.5f);
                player.velocityY = 0; if (isHooked) isHooked = false;
            }
        }
    }

    if (player.x < 0) { player.x = 0; player.velocityX = 0; }
    if (player.x + player.w > WORLD_WIDTH) { player.x = WORLD_WIDTH - player.w; player.velocityX = 0; }

    if (isHooked) {
        float pCX = player.x + player.w / 2, pCY = player.y + player.h / 2;
        float vHX = hookPointX - pCX, vHY = hookPointY - pCY;
        float cD = sqrt(vHX * vHX + vHY * vHY);
        if (cD > ropeLength && cD > 0.01f) {
            float s = cD - ropeLength, nX = vHX / cD, nY = vHY / cD;
            player.x += nX * s; player.y += nY * s;
            float dP = player.velocityX * nX + player.velocityY * nY;
            if (dP > 0) { player.velocityX -= nX * dP; player.velocityY -= nY * dP; }
        }
        player.velocityX *= DAMPING_FACTOR; player.velocityY *= DAMPING_FACTOR;
        isGrounded = false;
    }

    if (isHookFiring) {
        float prevHookX = hookProjectileX, prevHookY = hookProjectileY;
        hookProjectileX += hookProjectileVelX; hookProjectileY += hookProjectileVelY;
        
        bool hit = false;
        for (int i = 0; i < numPlatforms; i++) {
            Platform* p = &platforms[i];
            float hitX, hitY;
            if (p->isHookable && lineRectIntersection(prevHookX, prevHookY, hookProjectileX, hookProjectileY, *p, hitX, hitY)) {
                isHookFiring = false; isHooked = true;
                hookPointX = hitX; hookPointY = hitY;
                float dx = hookPointX - (player.x + player.w / 2), dy = hookPointY - (player.y + player.h / 2);
                ropeLength = sqrt(dx * dx + dy * dy);
                isGrounded = false;
                hit = true;
                break;
            }
        }

        if (!hit) {
            if (hookProjectileX < cameraLeft || 
                hookProjectileX > cameraLeft + VIEW_WIDTH ||
                hookProjectileY < cameraBottom ||
                hookProjectileY > cameraBottom + VIEW_HEIGHT)
            {
                isHookFiring = false;
            }
        }
    }
    
    float currentVelocityMag = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    currentAcceleration = currentVelocityMag - lastVelocityMag;
    lastVelocityMag = currentVelocityMag;

    if (isPointInside(player.x + player.w / 2, player.y + player.h / 2, door.x, door.y, door.w, door.h)) {
        return GAME_ACTION_LEVEL_WON;
    }
    return GAME_ACTION_CONTINUE;
}

// ALTERADO: HUD de física aprimorado
void drawPhysicsDebugHUD() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    float line_height = 15.0f; // Altura da linha para GLUT_BITMAP_9_BY_15
    float margin = 10.0f;
    float padding = 5.0f;

    std::vector<std::string> info_lines;
    char buffer[100];
    
    float velMag = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    sprintf(buffer, "v: %.1f m/s", velMag); info_lines.push_back(buffer);
    
    // ALTERADO: Aceleração agora é sempre exibida
    sprintf(buffer, "a: %.2f m/s^2", currentAcceleration); info_lines.push_back(buffer);

    sprintf(buffer, "P: %.2f N", GRAVITY); info_lines.push_back(buffer);
    
    if (isGrounded) {
        sprintf(buffer, "N: %.2f N", forceNormal); info_lines.push_back(buffer);
        if (fabs(forceFriction) > 0.001f) {
            sprintf(buffer, "Fat: %.2f N", fabs(forceFriction)); info_lines.push_back(buffer);
        }
    }
    if (isHooked) {
        sprintf(buffer, "T: %.2f N", currentPullForce); info_lines.push_back(buffer);
    }
    
    // ALTERADO: Adiciona 'mu' apenas para plataformas visíveis na tela
    for (int i = 0; i < numPlatforms; ++i) {
        Platform* p = &platforms[i];
        // Verifica se a plataforma está visível horizontalmente na câmera
        if (p->x < cameraLeft + VIEW_WIDTH && p->x + p->w > cameraLeft) {
            sprintf(buffer, "mu_%d: %.2f", i + 1, p->frictionCoefficient);
            info_lines.push_back(buffer);
        }
    }
    
    float max_width = 0;
    for (const auto& line : info_lines) {
        max_width = std::max(max_width, (float)getTextWidth(line.c_str(), GLUT_BITMAP_9_BY_15));
    }

    float block_width = max_width + 2 * padding;
    float block_height = info_lines.size() * line_height + padding; // Ajuste na altura do bloco
    float block_x = glutGet(GLUT_WINDOW_WIDTH) - margin - block_width;
    float block_y = margin;

    // Desenha o fundo preto semi-transparente
    drawRect(block_x, block_y, block_width, block_height, 0.0f, 0.0f, 0.0f);

    float current_y = block_y + padding + 10; // Posição Y inicial para o texto
    for (const auto& line : info_lines) {
        // ALTERADO: Alinha todo o texto à esquerda dentro do bloco para consistência
        float text_x = block_x + padding;
        
        glColor3f(1.0f, 1.0f, 1.0f);
        game_drawText(text_x, current_y, line.c_str(), GLUT_BITMAP_9_BY_15);
        current_y += line_height;
    }

    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}


void game_display() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    cameraLeft = player.x + player.w / 2 - VIEW_WIDTH / 2;
    if (cameraLeft < 0) cameraLeft = 0;
    if (cameraLeft + VIEW_WIDTH > WORLD_WIDTH) cameraLeft = WORLD_WIDTH - VIEW_WIDTH;
    cameraBottom = 0;

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(cameraLeft, cameraLeft + VIEW_WIDTH, cameraBottom, cameraBottom + VIEW_HEIGHT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    for (int i = 0; i < numPlatforms; i++) {
        Platform* p = &platforms[i];
        drawRect(p->x, p->y, p->w, p->h, p->r, p->g, p->b);
        char muText[20]; sprintf(muText, "mu_%d", i + 1); glColor3f(1.0, 1.0, 1.0);
        game_drawText(p->x + 20, p->y + 15, muText);
    }
    drawRect(door.x, door.y, door.w, door.h, door.r, door.g, door.b);
    
    float pCX = player.x + player.w / 2, pCY = player.y + player.h / 2;

    glPushMatrix();
    glTranslatef(pCX, pCY, 0.0f);
    
    drawRect(-player.w / 2, -player.h / 2, player.w, player.h, player.r, player.g, player.b);

    // ALTERADO: Adicionados rótulos aos vetores
    drawVector(0, 0, 0, GRAVITY, VECTOR_VISUAL_SCALE, 0.2f, 0.2f, 1.0f, "P");
    float velMag = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    if (velMag > 0.01) {
        drawVector(0, 0, player.velocityX, player.velocityY, VELOCITY_VISUAL_SCALE, 1.0f, 0.5f, 0.0f, "v");
    }
    if (isHooked) {
        drawVector(0, 0, forceTensionX, forceTensionY, VECTOR_VISUAL_SCALE, 1.0f, 0.0f, 1.0f, "T");
    }
    if (isGrounded) {
        drawVector(0, -player.h/2, 0, forceNormal, VECTOR_VISUAL_SCALE, 0.0f, 1.0f, 1.0f, "N");
        if (fabs(forceFriction) > 0.001f) {
            drawVector(0, -player.h/2 + 5, forceFriction, 0, VECTOR_VISUAL_SCALE, 1.0f, 0.0f, 0.0f, "Fat");
        }
    }
    
    glPopMatrix();

    float vAX = mouseGameX - pCX, vAY = mouseGameY - pCY;
    float rMD = sqrt(vAX * vAX + vAY * vAY);
    float cMD = fmin(rMD, MAX_VISUAL_AIM_LENGTH);
    aimDisplayForce = (cMD / MAX_VISUAL_AIM_LENGTH) * MAX_AIM_FORCE_DISPLAY;
    float vANX = 0, vANY = 0; if (rMD > 0.01f) { vANX = vAX / rMD; vANY = vAY / rMD; }

    float forcePercent = cMD / MAX_VISUAL_AIM_LENGTH;
    float aimPhysicsX = vANX * forcePercent * MAX_PULL_STRENGTH_PHYSICS;
    float aimPhysicsY = vANY * forcePercent * MAX_PULL_STRENGTH_PHYSICS;
    
    if (!isHooked && !isHookFiring) {
        char magText[50];
        sprintf(magText, "Forca: %.0f", aimDisplayForce);
        drawVector(pCX, pCY, aimPhysicsX, aimPhysicsY, VECTOR_VISUAL_SCALE, 1.0f, 1.0f, 1.0f, magText);
    }

    if (isHookFiring || isHooked) {
        float endX = isHookFiring ? hookProjectileX : hookPointX;
        float endY = isHookFiring ? hookProjectileY : hookPointY;
        if (endX < cameraLeft || endX > cameraLeft + VIEW_WIDTH || endY < cameraBottom || endY > cameraBottom + VIEW_HEIGHT) {
            clipLineToView(pCX, pCY, endX, endY);
        }
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES); glVertex2f(pCX, pCY); glVertex2f(endX, endY); glEnd();
    }
    
    drawPhysicsDebugHUD();
}

// --- Funções de Input ---
GameAction game_key_down(unsigned char key, int x, int y) {
    switch (key) {
        case 'q': case 'Q': return GAME_ACTION_EXIT_TO_MENU;
        case 'a': case 'A': isPressingLeft = true; break;
        case 'd': case 'D': isPressingRight = true; break;
    }
    return GAME_ACTION_CONTINUE;
}

void game_key_up(unsigned char key, int x, int y) {
    switch (key) {
        case 'a': case 'A': isPressingLeft = false; break;
        case 'd': case 'D': isPressingRight = false; break;
    }
}

void game_special_down(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: isPressingLeft = true; break;
        case GLUT_KEY_RIGHT: isPressingRight = true; break;
    }
}

void game_special_up(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: isPressingLeft = false; break;
        case GLUT_KEY_RIGHT: isPressingRight = false; break;
    }
}

void game_mouse_motion(int x, int y) {
    int winW = glutGet(GLUT_WINDOW_WIDTH), winH = glutGet(GLUT_WINDOW_HEIGHT);
    if (winW == 0) winW = 1; if (winH == 0) winH = 1;
    mouseGameX = cameraLeft + (float)x / winW * VIEW_WIDTH;
    mouseGameY = cameraBottom + (float)(winH - y) / winH * VIEW_HEIGHT;
}

void game_mouse_click(int button, int state) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            if (!isHooked && !isHookFiring) {
                isHookFiring = true;
                float pCX = player.x + player.w / 2, pCY = player.y + player.h / 2;
                hookProjectileX = pCX; hookProjectileY = pCY;
                
                float vAX = mouseGameX - pCX, vAY = mouseGameY - pCY;
                float rMD = sqrt(vAX * vAX + vAY * vAY);
                float cMD = fmin(rMD, MAX_VISUAL_AIM_LENGTH);
                float fP = cMD / MAX_VISUAL_AIM_LENGTH;
                currentPullForce = fP * MAX_PULL_STRENGTH_PHYSICS;

                if (rMD > 0.01f) {
                    hookProjectileVelX = (vAX / rMD) * HOOK_SPEED;
                    hookProjectileVelY = (vAY / rMD) * HOOK_SPEED;
                } else {
                    hookProjectileVelX = 0; hookProjectileVelY = HOOK_SPEED;
                }
            }
        } 
        else if (state == GLUT_UP) {
            isHooked = false; 
            isHookFiring = false;
        }
    }
}