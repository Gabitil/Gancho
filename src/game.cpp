/**
 * Por padrão de projeto, todas as variáveis usadas são explicitamente do tipo float. Isso é 
 * feito para garantir a segurança e a linearidade dos resultados calculados.
 */

#include "game.h"
#include <GL/freeglut.h>
#include "utils.h"
#include <stdio.h>
// #include <math.h>
#include <string>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

// ----------------------------------------------------------------------------------------------------------------

/**
 * Para padronização dos cálculos e dos valores de vetores e deslocamentos, todas as constantes
 * usadas são fixas. Constantes padrões da física.
 */
const float GRAVITY = -0.1f; // Aceleração vertical (negativa = para baixo)
const float PLAYER_MASS = 10.0f; // Massa do jogador (para cálculos de força)
const float PLAYER_WALK_ACCEL = 0.1f; // Aceleração horizontal aplicada ao caminhar
const float MAX_WALK_SPEED = 3.0f; // Velocidade máxima permitida enquanto "no chão"
const float DAMPING_FACTOR = 0.99f; // Fator de amortecimento aplicado quando pendurado

/**
 * Constantes relacionadas ao gancho
 */
const float HOOK_SPEED = 25.0f;
const float MAX_PULL_STRENGTH_PHYSICS = 0.4f;

/**
 * Limitador de velocidade do personagem para garantir melhor visibilidade ao jogador dos 
 * vetores e das ações ao decorrer do game
 */
const float MAX_PLAYER_SPEED = 30.0f;

/**
 * Constantes usadas na definição de mundo e para visualização (câmera, espaço para desenhos, etc)
 */
const float PLAYER_HEIGHT = 40.0f;
const float VIEW_WIDTH = 800.0f;
const float VIEW_HEIGHT = 600.0f;
const float WORLD_WIDTH = 3000.0f;
const float WORLD_HEIGHT = 600.0f;
const float DEATH_BOUNDARY_Y = -100.0f; // Constante usada para verificar se o personagem "caiu", limitador para sua queda

/**
 * Os vetores seguem uma escala padrão para que a coerência da visualização seja válida ao jogador. 
 * Para isso, o valor máximo do vetor de lançamento do "grappling hook" será de 100N e o seu tamanho será de 
 * 5x o tamanho do personagem. A partir daí todos os outros vetores serão calculados a fim de manter a proporção de 
 * tamanho e ajudar a dimensionar, para o jogador, a força real de cada força aplicada.
 */
const float MAX_VISUAL_AIM_LENGTH = 5.0f * PLAYER_HEIGHT;
const float MAX_AIM_FORCE_DISPLAY = 100.0f;
const float VECTOR_VISUAL_SCALE = MAX_VISUAL_AIM_LENGTH / MAX_PULL_STRENGTH_PHYSICS;
const float VELOCITY_VISUAL_SCALE = 7.0f;
const float WIND_VECTOR_VISUAL_SCALE = 500.0f;

// ----------------------------------------------------------------------------------------------------------------

/**
 * Definição das variáveis que serão usadas no game.
 * Aqui também definimos o limite de quantidade de cada elemento. Essas 
 * quantidades serão usadas de forma variada de acordo com a fase.
 */
GameObject player;
GameObject door;
Platform platforms[10];
int numPlatforms = 0;
Platform* collidingPlatform = NULL;

WindZone windZones[10];
int numWindZones = 0;
BreakableWall breakableWalls[10];
int numBreakableWalls = 0;
SpikeZone spikeZones[10];
int numSpikeZones = 0;

int CURRENT_LEVEL = 1;
bool isGrounded = false;
bool isHooked = false;
bool isHookFiring = false;
float hookPointX = 0, hookPointY = 0;
float hookProjectileX = 0, hookProjectileY = 0;
float hookProjectileVelX = 0, hookProjectileVelY = 0;
float ropeLength = 0;
float currentPullForce = 0.0f;
float mouseGameX = 0, mouseGameY = 0;
float aimDisplayForce = 0;
float cameraLeft = 0, cameraBottom = 0;
bool isPressingLeft = false;
bool isPressingRight = false;
float forceNormal = 0;
float forceFriction = 0;
float forceTensionX = 0;
float forceTensionY = 0;

/**
 * Variáveis para controle da aceleração do personagem. 
 * Usada nos cálculos usando a velocidade. No sistema escolhido, não é uma aceleração
 * que define a velocidade, mas sim a velocidade fixada que define uma aceleração.
 * Desse modo fica mais simples de coordenar todos os movimentos e fazer os cálculos do game.
 */
float lastVelocityMag = 0.0f;
float currentAcceleration = 0.0f;

// ----------------------------------------------------------------------------------------------------------------

/**
 * Função para inicializar a quantidade de elementos de mundo, suas posições e outros parâmetros.
 * TODO - Variar a quantidade de elementos e as suas posições de acordo com as fases
 */
void gameInit() {
    // Plataformas
    numPlatforms = 2;
    platforms[0] = {0, 0, WORLD_WIDTH, 40, 0.2f, 0.6f, 0.2f, true, 0.8f};
    platforms[1] = {500, 400, 300, 40, 0.4f, 0.4f, 0.4f, true, 0.1f}; 
    
    // Correntes de Vento
    numWindZones = 1;
    windZones[0] = {800, 40, 100, 300, 0.0f, 0.05f};
    
    // Paredes Quebráveis
    numBreakableWalls = 1;
    breakableWalls[0] = {1200, 40, 40, 150, 0.6f, 0.4f, 0.2f, 20.0f, false};
    
    // Espinhos
    numSpikeZones = 1;
    spikeZones[0] = {1500, 40, 200, 20, 1.0f, 0.0f, 0.0f};
}

// Prepara o nível para iniciar/reiniciar
void gameStartLevel(int level) {
    player = {50, platforms[0].h, 40, PLAYER_HEIGHT, 0.9f, 0.1f, 0.1f, 0, 0};
    door = {WORLD_WIDTH - 200, platforms[0].h, 30, 80, 0.5f, 0.3f, 0.0f};
    isGrounded = true; isHooked = false; isHookFiring = false;
    collidingPlatform = &platforms[0]; isPressingLeft = false; isPressingRight = false;
    lastVelocityMag = 0.0f;
    currentAcceleration = 0.0f;

    // Reseta todas as paredes quebráveis
    for (int i = 0; i < numBreakableWalls; i++) {
        breakableWalls[i].isBroken = false;
    }
}

void gameReshape(int w, int h) { glViewport(0, 0, w, h); }

GameAction gameUpdate() {
    float prevPlayerX = player.x;
    float prevPlayerY = player.y;
    float prevVelX = player.velocityX;
    float prevVelY = player.velocityY;

    forceNormal = 0; forceFriction = 0; forceTensionX = 0; forceTensionY = 0;
    float accelX = 0, accelY = GRAVITY;

    // Aplicar Força do Vento (como aceleração)
    for (int i = 0; i < numWindZones; i++) {
        WindZone* wz = &windZones[i];
        // NOVO: Apenas aplica vento se o jogador estiver dentro da zona
        if (isPointInside(player.x + player.w / 2, player.y + player.h / 2, wz->x, wz->y, wz->w, wz->h)) {
            accelX += wz->accelX;
            accelY += wz->accelY;
        }
    }

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

    // --- Colisões (com novos obstáculos) ---
    
    // Checar Limites do Mundo (Morte)
    if (player.x < -player.w || player.x > WORLD_WIDTH || player.y < DEATH_BOUNDARY_Y) {
        return GAME_ACTION_LEVEL_LOST;
    }

    // Checar Colisão com Espinhos (Morte)
    for (int i = 0; i < numSpikeZones; i++) {
        SpikeZone* sp = &spikeZones[i];
        if (player.x < sp->x + sp->w && player.x + player.w > sp->x &&
            player.y < sp->y + sp->h && player.y + player.h > sp->y)
        {
            return GAME_ACTION_LEVEL_LOST;
        }
    }

    // Colisão com Paredes Quebráveis
    for (int i = 0; i < numBreakableWalls; i++) {
        BreakableWall* bw = &breakableWalls[i];
        if (bw->isBroken) continue;

        // Verifica colisão AABB
        if (player.x < bw->x + bw->w && player.x + player.w > bw->x &&
            player.y < bw->y + bw->h && player.y + player.h > bw->y)
        {
            // Calcula velocidade de impacto
            float impactVelX = player.velocityX;
            float impactVelY = player.velocityY;
            float impactSpeed = sqrt(impactVelX * impactVelX + impactVelY * impactVelY);
            
            // Calcula força de impacto como uma "força de parada"
            // Supondo um "delta-t" pequeno para calcular a aceleração necessária para parar
            // Força = Massa * Aceleração
            // Aceleração = (Velocidade Final - Velocidade Inicial) / delta_t
            // Força de Impacto = PLAYER_MASS * impactSpeed / TEMPO_DE_COLISAO (simplificado)
            // Aqui, vamos simplificar para Forca de Impacto ~ PLAYER_MASS * impactSpeed
            float impactForce = PLAYER_MASS * impactSpeed;

            if (impactForce >= bw->strength) { // Quebra a parede se a força for suficiente
                bw->isBroken = true;
                // Reduz um pouco a velocidade do jogador ao quebrar
                player.velocityX *= 0.7f; 
                player.velocityY *= 0.7f;
            } else { // Não quebrou, então o jogador colide e para
                // Reverte a posição para antes da colisão
                player.x = prevPlayerX;
                player.y = prevPlayerY;
                
                // Zera a velocidade na direção da colisão
                // (mais complexo para direções mistas, aqui simplificamos)
                if (player.x + player.w > bw->x && prevPlayerX + player.w <= bw->x) { // Colisão pela esquerda
                    player.velocityX = 0;
                    player.x = bw->x - player.w;
                } else if (player.x < bw->x + bw->w && prevPlayerX >= bw->x + bw->w) { // Colisão pela direita
                    player.velocityX = 0;
                    player.x = bw->x + bw->w;
                }
                if (player.y + player.h > bw->y && prevPlayerY + player.h <= bw->y) { // Colisão por baixo
                    player.velocityY = 0;
                    player.y = bw->y - player.h;
                } else if (player.y < bw->y + bw->h && prevPlayerY >= bw->y + bw->h) { // Colisão por cima
                    player.velocityY = 0;
                    player.y = bw->y + bw->h;
                }
            }
        }
    }

    // Colisão com Plataformas
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
            // Gancho não prende em atrito 0
            if (p->isHookable && p->frictionCoefficient > 0.0f && lineRectIntersection(prevHookX, prevHookY, hookProjectileX, hookProjectileY, *p, hitX, hitY)) {
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

// HUD de física aprimorado
void drawPhysicsDebugHUD() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    float line_height = 15.0f;
    float margin = 10.0f;
    float padding = 8.0f;

    std::vector<std::string> info_lines;
    char buffer[100];
    
    float velMag = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    sprintf(buffer, "v: %.1f m/s", velMag); info_lines.push_back(buffer);
    
    sprintf(buffer, "a: %.2f m/s^2", currentAcceleration); info_lines.push_back(buffer);

    sprintf(buffer, "P: %.2f N", GRAVITY * PLAYER_MASS); info_lines.push_back(buffer); // NOVO: Peso exibido com massa
    
    if (isGrounded) {
        sprintf(buffer, "N: %.2f N", forceNormal * PLAYER_MASS); info_lines.push_back(buffer); // NOVO: Normal exibida com massa
        if (fabs(forceFriction) > 0.001f) {
            sprintf(buffer, "Fat: %.2f N", fabs(forceFriction) * PLAYER_MASS); info_lines.push_back(buffer); // NOVO: Atrito exibido com massa
        }
    }
    if (isHooked) {
        sprintf(buffer, "T: %.2f N", currentPullForce * PLAYER_MASS); info_lines.push_back(buffer); // NOVO: Tensão exibida com massa
    }
    
    bool mu_header_added = false;
    for (int i = 0; i < numPlatforms; ++i) {
        Platform* p = &platforms[i];
        if (p->x < cameraLeft + VIEW_WIDTH && p->x + p->w > cameraLeft) {
            sprintf(buffer, "mu_%d: %.2f", i + 1, p->frictionCoefficient);
            info_lines.push_back(buffer);
            mu_header_added = true;
        }
    }
    
    float max_width = 0;
    for (const auto& line : info_lines) {
        max_width = std::max(max_width, (float)getTextWidth(line.c_str(), GLUT_BITMAP_9_BY_15));
    }

    float block_width = max_width + 2 * padding;
    float block_height = (info_lines.size() * line_height) + (padding * 2) - (line_height - 10);
    float block_x = glutGet(GLUT_WINDOW_WIDTH) - margin - block_width;
    float block_y = margin;

    drawRect(block_x, block_y, block_width, block_height, 0.0f, 0.0f, 0.0f, 0.7f);

    float current_y = block_y + padding + 10;
    for (const auto& line : info_lines) {
        float text_x = block_x + padding;
        
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(text_x, current_y, line.c_str(), GLUT_BITMAP_9_BY_15);
        current_y += line_height;
    }

    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}


void gameDisplay() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    cameraLeft = player.x + player.w / 2 - VIEW_WIDTH / 2;
    if (cameraLeft < 0) cameraLeft = 0;
    if (cameraLeft + VIEW_WIDTH > WORLD_WIDTH) cameraLeft = WORLD_WIDTH - VIEW_WIDTH;
    cameraBottom = 0;

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(cameraLeft, cameraLeft + VIEW_WIDTH, cameraBottom, cameraBottom + VIEW_HEIGHT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // --- Desenha Mundo e Obstáculos ---
    for (int i = 0; i < numPlatforms; i++) {
        Platform* p = &platforms[i];
        glPushMatrix();
        glTranslatef(p->x, p->y, 0.0f);
        drawRect(0, 0, p->w, p->h, p->r, p->g, p->b);
        glColor3f(1.0, 1.0, 1.0);
        drawText(20, 15, ("mu_" + std::to_string(i + 1)).c_str());
        glPopMatrix();
    }
    
    // Desenha Zonas de Vento
    for (int i = 0; i < numWindZones; i++) {
        WindZone* wz = &windZones[i];
        glPushMatrix();
        glTranslatef(wz->x, wz->y, 0.0f);
        drawRect(0, 0, wz->w, wz->h, 1.0f, 1.0f, 1.0f, 0.2f); // Retângulo branco semi-transparente

        // NOVO: Desenha o vetor da corrente de vento
        float windMag = sqrt(wz->accelX * wz->accelX + wz->accelY * wz->accelY);
        if (windMag > 0.001f) {
            char windLabel[50];
            sprintf(windLabel, "Corrente de Vento: %.2f N", windMag * PLAYER_MASS); // Exibe força do vento
            drawVector(wz->w / 2, wz->h / 2, wz->accelX, wz->accelY, WIND_VECTOR_VISUAL_SCALE, 0.0f, 0.8f, 1.0f, windLabel);
        }
        glPopMatrix();
    }
    
    // Desenha Paredes Quebráveis (se não estiverem quebradas)
    for (int i = 0; i < numBreakableWalls; i++) {
        BreakableWall* bw = &breakableWalls[i];
        if (!bw->isBroken) {
            glPushMatrix();
            glTranslatef(bw->x, bw->y, 0.0f);
            drawRect(0, 0, bw->w, bw->h, bw->r, bw->g, bw->b);
            
            // NOVO: Exibe a força necessária para quebrar
            glColor3f(1.0f, 1.0f, 1.0f); // Texto branco
            char strengthText[50];
            sprintf(strengthText, "Quebrar: %.0f N", bw->strength);
            
            // Centraliza o texto na parede
            int textWidth = getTextWidth(strengthText, GLUT_BITMAP_9_BY_15);
            drawText(bw->w / 2 - textWidth / 2, bw->h / 2, strengthText, GLUT_BITMAP_9_BY_15);
            glPopMatrix();
        }
    }
    
    // Desenha Espinhos
    for (int i = 0; i < numSpikeZones; i++) {
        SpikeZone* sp = &spikeZones[i];
        // drawSpikes já usa glPushMatrix/popMatrix internamente
        drawSpikes(sp->x, sp->y, sp->w, sp->h, sp->r, sp->g, sp->b);
    }
    
    // Desenha a porta (objetivo)
    glPushMatrix();
    glTranslatef(door.x, door.y, 0.0f);
    drawRect(0, 0, door.w, door.h, door.r, door.g, door.b);
    glPopMatrix();
    
    float pCX = player.x + player.w / 2, pCY = player.y + player.h / 2; // centro do jogador

    // Desenha o jogador e seus vetores
    glPushMatrix();
    glTranslatef(pCX, pCY, 0.0f);
    
    drawRect(-player.w / 2, -player.h / 2, player.w, player.h, player.r, player.g, player.b);

    // Rótulos dos vetores re-adicionados
    drawVector(0, 0, 0, GRAVITY, VECTOR_VISUAL_SCALE, 0.2f, 0.2f, 1.0f, "P"); // Peso
    float velMag = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    if (velMag > 0.01) {
        drawVector(0, 0, player.velocityX, player.velocityY, VELOCITY_VISUAL_SCALE, 1.0f, 0.5f, 0.0f, "v"); // Velocidade
    }
    if (isHooked) {
        drawVector(0, 0, forceTensionX, forceTensionY, VECTOR_VISUAL_SCALE, 1.0f, 0.0f, 1.0f, "T"); // Tensão
    }
    if (isGrounded) {
        drawVector(0, -player.h/2, 0, forceNormal, VECTOR_VISUAL_SCALE, 0.0f, 1.0f, 1.0f, "N"); // Normal
        if (fabs(forceFriction) > 0.001f) {
            drawVector(0, -player.h/2 + 5, forceFriction, 0, VECTOR_VISUAL_SCALE, 1.0f, 0.0f, 0.0f, "Fat"); // Atrito
        }
    }
    
    glPopMatrix();

    // Cálculo e desenho do vetor de mira
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
        drawVector(pCX, pCY, aimPhysicsX, aimPhysicsY, VECTOR_VISUAL_SCALE, 1.0f, 1.0f, 1.0f, magText); // Mira
    }

    // Desenha linha entre jogador e gancho (se disparado ou preso)
    if (isHookFiring || isHooked) {
        float endX = isHookFiring ? hookProjectileX : hookPointX;
        float endY = isHookFiring ? hookProjectileY : hookPointY;
        if (endX < cameraLeft || endX > cameraLeft + VIEW_WIDTH || endY < cameraBottom || endY > cameraBottom + VIEW_HEIGHT) {
            clipLineToView(pCX, pCY, endX, endY, VIEW_WIDTH, VIEW_HEIGHT, cameraLeft, cameraBottom);
        }
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES); glVertex2f(pCX, pCY); glVertex2f(endX, endY); glEnd();
    }
    
    drawPhysicsDebugHUD();
}

// --- Funções de Input ---
GameAction gameKeyDown(unsigned char key, int x, int y) {
    switch (key) {
        case 'q': case 'Q': return GAME_ACTION_EXIT_TO_MENU;
        case 'a': case 'A': isPressingLeft = true; break;
        case 'd': case 'D': isPressingRight = true; break;
    }
    return GAME_ACTION_CONTINUE;
}

void gameKeyUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'a': case 'A': isPressingLeft = false; break;
        case 'd': case 'D': isPressingRight = false; break;
    }
}

void gameSpecialDown(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: isPressingLeft = true; break;
        case GLUT_KEY_RIGHT: isPressingRight = true; break;
    }
}

void gameSpecialUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: isPressingLeft = false; break;
        case GLUT_KEY_RIGHT: isPressingRight = false; break;
    }
}

void gameMouseMotion(int x, int y) {
    int winW = glutGet(GLUT_WINDOW_WIDTH), winH = glutGet(GLUT_WINDOW_HEIGHT);
    if (winW == 0) winW = 1; if (winH == 0) winH = 1;
    mouseGameX = cameraLeft + (float)x / winW * VIEW_WIDTH;
    mouseGameY = cameraBottom + (float)(winH - y) / winH * VIEW_HEIGHT;
}

void gameMouseClick(int button, int state) {
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