#include "game.h"
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

// --- Constantes de Física ---
const float GRAVITY = -0.1f;
const float PLAYER_WALK_ACCEL = 0.1f;
const float MAX_WALK_SPEED = 3.0f;
const float DAMPING_FACTOR = 0.99f;

// --- Constantes do Gancho ---
const float HOOK_SPEED = 25.0f;
const float MAX_PULL_STRENGTH_PHYSICS = 0.4f;

// --- Limite de Velocidade e Escala Visual ---
const float MAX_PLAYER_SPEED = 30.0f;

// --- Constantes do Mundo e Visualização ---
const float PLAYER_HEIGHT = 40.0f;
const float VIEW_WIDTH = 800.0f;
const float VIEW_HEIGHT = 600.0f;
const float WORLD_WIDTH = 3000.0f;
const float WORLD_HEIGHT = 600.0f;

// --- Lógica de Escala de Vetores ---
const float MAX_VISUAL_AIM_LENGTH = 5.0f * PLAYER_HEIGHT;
const float MAX_AIM_FORCE_DISPLAY = 100.0f;
const float VECTOR_VISUAL_SCALE = MAX_VISUAL_AIM_LENGTH / MAX_PULL_STRENGTH_PHYSICS;
const float VELOCITY_VISUAL_SCALE = 7.0f;

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
    float frictionCoefficient;
};

// --- Variáveis Globais do Jogo ---
GameObject player;
GameObject door;
Platform platforms[10];
int numPlatforms = 0;
Platform* collidingPlatform = NULL;
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

// --- Funções Auxiliares Internas ---

void drawRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + w, y); glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();
}

void game_drawText(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) {
        glutBitmapCharacter(font, *c);
    }
}

// READICIONADO: A função que foi removida por engano
bool isPointInside(float x, float y, float objX, float objY, float objW, float objH) {
    return (x >= objX && x <= objX + objW &&
            y >= objY && y <= objY + objH);
}

int getTextWidth(const char* text, void* font) {
    int width = 0;
    for (const char* c = text; *c; ++c) {
        width += glutBitmapWidth(font, *c);
    }
    return width;
}

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

void drawVector(float startX, float startY, float vX_accel, float vY_accel,
                  float r, float g, float b, const char* label) {
    float vX_visual = vX_accel * VECTOR_VISUAL_SCALE;
    float vY_visual = vY_accel * VECTOR_VISUAL_SCALE;
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
    game_drawText(endX + 5, endY + 5, label);
}

// --- Funções Principais do Jogo ---

void game_init() {
    numPlatforms = 2;
    platforms[0] = {0, 0, WORLD_WIDTH, 40, 0.2f, 0.6f, 0.2f, true, 0.8f};
    platforms[1] = {500, 400, 300, 40, 0.4f, 0.4f, 0.4f, true, 0.1f};
}

void game_start_level(int level) {
    player = {50, platforms[0].h, 40, PLAYER_HEIGHT, 0.9f, 0.1f, 0.1f, 0, 0};
    door = {WORLD_WIDTH - 200, platforms[0].h, 30, 80, 0.5f, 0.3f, 0.0f};
    isGrounded = true; isHooked = false; isHookFiring = false;
    collidingPlatform = &platforms[0]; isPressingLeft = false; isPressingRight = false;
}

void game_reshape(int w, int h) { glViewport(0, 0, w, h); }

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
        for (int i = 0; i < numPlatforms; i++) {
            Platform* p = &platforms[i];
            float hitX, hitY;
            if (p->isHookable && lineRectIntersection(prevHookX, prevHookY, hookProjectileX, hookProjectileY, *p, hitX, hitY)) {
                isHookFiring = false; isHooked = true;
                hookPointX = hitX; hookPointY = hitY;
                float dx = hookPointX - (player.x + player.w / 2), dy = hookPointY - (player.y + player.h / 2);
                ropeLength = sqrt(dx * dx + dy * dy);
                isGrounded = false;
                break;
            }
        }
    }

    if (isPointInside(player.x, player.y, door.x, door.y, door.w, door.h)) {
        return GAME_ACTION_LEVEL_WON;
    }
    return GAME_ACTION_CONTINUE;
}

void drawGameHUD_Info() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    std::string hud_string = "| ";
    char buffer[100];
    float velMag = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    sprintf(buffer, "v: %.1f m/s | ", velMag); hud_string += buffer;
    sprintf(buffer, "P: %.2f N | ", GRAVITY); hud_string += buffer;
    if (isGrounded) {
        sprintf(buffer, "N: %.2f N | ", forceNormal); hud_string += buffer;
        if (fabs(forceFriction) > 0.001f) {
            sprintf(buffer, "Fat: %.2f N | ", fabs(forceFriction)); hud_string += buffer;
        }
        if (collidingPlatform) {
            int platform_idx = collidingPlatform - platforms + 1;
            sprintf(buffer, "mu_%d: %.2f | ", platform_idx, collidingPlatform->frictionCoefficient);
            hud_string += buffer;
        }
    }
    if (isHooked) {
        sprintf(buffer, "T: %.2f N | ", currentPullForce); hud_string += buffer;
    }
    
    glColor3f(0.0f, 0.0f, 0.0f);
    game_drawText(11.0f, 21.0f, hud_string.c_str(), GLUT_BITMAP_9_BY_15);
    glColor3f(1.0f, 1.0f, 1.0f);
    game_drawText(10.0f, 20.0f, hud_string.c_str(), GLUT_BITMAP_9_BY_15);

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
    drawRect(player.x, player.y, player.w, player.h, player.r, player.g, player.b);

    float pCX = player.x + player.w / 2, pCY = player.y + player.h / 2;
    float vAX = mouseGameX - pCX, vAY = mouseGameY - pCY;
    float rMD = sqrt(vAX * vAX + vAY * vAY);
    float cMD = fmin(rMD, MAX_VISUAL_AIM_LENGTH);
    aimDisplayForce = (cMD / MAX_VISUAL_AIM_LENGTH) * MAX_AIM_FORCE_DISPLAY;
    float vANX = 0, vANY = 0; if (rMD > 0.01f) { vANX = vAX / rMD; vANY = vAY / rMD; }
    float aVEX = pCX + vANX * cMD, aVEY = pCY + vANY * cMD;
    glColor3f(1.0f, 1.0f, 1.0f); glLineStipple(1, 0xAAAA); glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES); glVertex2f(pCX, pCY); glVertex2f(aVEX, aVEY); glEnd();
    glDisable(GL_LINE_STIPPLE);
    char magText[50]; sprintf(magText, "Forca: %.0f", aimDisplayForce); glColor3f(1.0f, 1.0f, 1.0f);
    game_drawText(aVEX + 5, aVEY + 5, magText);

    if (isHookFiring || isHooked) {
        float endX = isHookFiring ? hookProjectileX : hookPointX;
        float endY = isHookFiring ? hookProjectileY : hookPointY;
        if (endX < cameraLeft || endX > cameraLeft + VIEW_WIDTH || endY < cameraBottom || endY > cameraBottom + VIEW_HEIGHT) {
            clipLineToView(pCX, pCY, endX, endY);
        }
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES); glVertex2f(pCX, pCY); glVertex2f(endX, endY); glEnd();
    }
    
    drawVector(pCX, pCY, 0, GRAVITY, 0.2f, 0.2f, 1.0f, "P");
    if (fabs(player.velocityX) > 0.01 || fabs(player.velocityY) > 0.01) {
        float vX_vis = player.velocityX * VELOCITY_VISUAL_SCALE, vY_vis = player.velocityY * VELOCITY_VISUAL_SCALE;
        float visMag = sqrt(vX_vis * vX_vis + vY_vis * vY_vis);
        if (visMag > MAX_VISUAL_AIM_LENGTH) {
            vX_vis = (vX_vis / visMag) * MAX_VISUAL_AIM_LENGTH;
            vY_vis = (vY_vis / visMag) * MAX_VISUAL_AIM_LENGTH;
        }
        drawVector(pCX, pCY, vX_vis / VECTOR_VISUAL_SCALE, vY_vis / VECTOR_VISUAL_SCALE, 1.0f, 0.5f, 0.0f, "v");
    }
    if (isHooked) { drawVector(pCX, pCY, forceTensionX, forceTensionY, 1.0f, 0.0f, 1.0f, "T"); }
    if (isGrounded) {
        drawVector(pCX, player.y, 0, forceNormal, 0.0f, 1.0f, 1.0f, "N");
        if (fabs(forceFriction) > 0.001f) {
            drawVector(pCX, player.y + 10, forceFriction, 0, 1.0f, 0.0f, 0.0f, "Fat");
        }
    }

    drawGameHUD_Info();
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
        } else if (state == GLUT_UP) {
            isHooked = false; isHookFiring = false;
        }
    }
}