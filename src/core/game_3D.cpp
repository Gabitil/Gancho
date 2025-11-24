// Header correspondente primeiro
#include "game_3D.h" 

// Bibliotecas padrão
#include <stdio.h>
#include <cmath>
#include <vector>
#include <algorithm>

// Utilitários
#include "mesh_utils.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =================================================================================
// ESTRUTURAS INTERNAS
// =================================================================================

struct Vector3D { float x, y, z; };

struct Player3D {
    Vector3D pos;
    Vector3D vel;
    Vector3D size;
};

struct Platform3D {
    float x, y, z;
    float w, h, d;
    float r, g, b; // Cor
    float friction;
    bool isHookable;
};

struct WindZone3D {
    float x, y, z;
    float w, h, d;
    Vector3D force;
};

struct BreakableWall3D {
    float x, y, z;
    float w, h, d;
    float strength;
    bool isBroken;
};

struct SpikeZone3D {
    float x, y, z;
    float w, h, d;
};

struct LevelParameters3D {
    float playerMass;
    float playerWalkAccel;
    float maxWalkSpeed;
    float maxPlayerSpeed;
    float vectorVisualScale;
};

// =================================================================================
// VARIÁVEIS GLOBAIS
// =================================================================================

// Estados do Jogo
bool isGameOver_3D = false;
bool isGrounded_3D = false;
static bool isGameVictory_3D = false; // Adicionado static para evitar conflito
static int victoryTimer_3D = 0;

// VBO Global
GLuint cubeVBO = 0; // Armazena o ID do VBO do cubo unitário

// Jogador e Física
Player3D player3D;
LevelParameters3D levelParams_3D;
const float GRAVITY_3D = -20.0f;

// Objetos do Mundo
std::vector<Platform3D> platforms3D;
std::vector<WindZone3D> windZones3D;
std::vector<BreakableWall3D> walls3D;
std::vector<SpikeZone3D> spikes3D;

// Variáveis de Controle de Janela e Mouse
static int windowWidth = 800;
static int windowHeight = 600;
bool isMouseFree = false;
bool mouseInitialized = false;

// Câmera
float cameraYaw = 0.0f;
float cameraPitch = 20.0f;
float cameraDistance = 25.0f;

// Input Teclado
bool keyW = false;
bool keyA = false;
bool keyS = false;
bool keyD = false;

// Vetores de Debug
Vector3D forceFriction_3D = {0, 0, 0};

// =================================================================================
// FUNÇÕES AUXILIARES: GEOMETRIA
// =================================================================================

// Helper para preencher a struct vert definida em mesh_utils.h
vert makeVert(float x, float y, float z, float r, float g, float b, float u, float v, float nx, float ny, float nz) {
    vert ver;
    ver.pos[0] = x; ver.pos[1] = y; ver.pos[2] = z;
    ver.cor[0] = r; ver.cor[1] = g; ver.cor[2] = b; ver.cor[3] = 1.0f;
    ver.tex[0] = u; ver.tex[1] = v;
    ver.normal[0] = nx; ver.normal[1] = ny; ver.normal[2] = nz;
    return ver;
}

// Cria um cubo unitário (1x1x1) centrado na origem
std::vector<tri> createUnitCubeTris() {
    std::vector<tri> tris;
    float d = 0.5f; // Metade do tamanho
    float r = 1, g = 1, b = 1; // Cor base branca (modularemos no desenho)

    // Frente (+Z)
    tris.push_back({ makeVert(-d,-d, d, r,g,b, 0,0, 0,0,1), makeVert( d,-d, d, r,g,b, 1,0, 0,0,1), makeVert( d, d, d, r,g,b, 1,1, 0,0,1) });
    tris.push_back({ makeVert(-d,-d, d, r,g,b, 0,0, 0,0,1), makeVert( d, d, d, r,g,b, 1,1, 0,0,1), makeVert(-d, d, d, r,g,b, 0,1, 0,0,1) });

    // Trás (-Z)
    tris.push_back({ makeVert( d,-d,-d, r,g,b, 0,0, 0,0,-1), makeVert(-d,-d,-d, r,g,b, 1,0, 0,0,-1), makeVert(-d, d,-d, r,g,b, 1,1, 0,0,-1) });
    tris.push_back({ makeVert( d,-d,-d, r,g,b, 0,0, 0,0,-1), makeVert(-d, d,-d, r,g,b, 1,1, 0,0,-1), makeVert( d, d,-d, r,g,b, 0,1, 0,0,-1) });

    // Esquerda (-X)
    tris.push_back({ makeVert(-d,-d,-d, r,g,b, 0,0, -1,0,0), makeVert(-d,-d, d, r,g,b, 1,0, -1,0,0), makeVert(-d, d, d, r,g,b, 1,1, -1,0,0) });
    tris.push_back({ makeVert(-d,-d,-d, r,g,b, 0,0, -1,0,0), makeVert(-d, d, d, r,g,b, 1,1, -1,0,0), makeVert(-d, d,-d, r,g,b, 0,1, -1,0,0) });

    // Direita (+X)
    tris.push_back({ makeVert( d,-d, d, r,g,b, 0,0, 1,0,0), makeVert( d,-d,-d, r,g,b, 1,0, 1,0,0), makeVert( d, d,-d, r,g,b, 1,1, 1,0,0) });
    tris.push_back({ makeVert( d,-d, d, r,g,b, 0,0, 1,0,0), makeVert( d, d,-d, r,g,b, 1,1, 1,0,0), makeVert( d, d, d, r,g,b, 0,1, 1,0,0) });

    // Cima (+Y)
    tris.push_back({ makeVert(-d, d, d, r,g,b, 0,0, 0,1,0), makeVert( d, d, d, r,g,b, 1,0, 0,1,0), makeVert( d, d,-d, r,g,b, 1,1, 0,1,0) });
    tris.push_back({ makeVert(-d, d, d, r,g,b, 0,0, 0,1,0), makeVert( d, d,-d, r,g,b, 1,1, 0,1,0), makeVert(-d, d,-d, r,g,b, 0,1, 0,1,0) });

    // Baixo (-Y)
    tris.push_back({ makeVert(-d,-d,-d, r,g,b, 0,0, 0,-1,0), makeVert( d,-d,-d, r,g,b, 1,0, 0,-1,0), makeVert( d,-d, d, r,g,b, 1,1, 0,-1,0) });
    tris.push_back({ makeVert(-d,-d,-d, r,g,b, 0,0, 0,-1,0), makeVert( d,-d, d, r,g,b, 1,1, 0,-1,0), makeVert(-d,-d, d, r,g,b, 0,1, 0,-1,0) });

    return tris;
}

// Função para desenhar o cubo instanciado usando o VBO
void drawCubeInstance(float x, float y, float z, float w, float h, float d, float r, float g, float b, float a) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(w, h, d); // Escala o cubo unitário para o tamanho desejado
    
    // Se alpha for menor que 1, habilita blend
    if (a < 0.99f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); // Não escreve no Z-buffer se for transparente
    }

    glColor4f(r, g, b, a);
    
    // Chama a função do mesh_utils para desenhar
    // 12 triângulos no cubo (2 por face * 6 faces)
    desenhaTrisComVBO(cubeVBO, 12); 

    if (a < 0.99f) {
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
    glPopMatrix();
}

// Desenha vetores de debug
void drawVector3D(Vector3D start, Vector3D vector, float scale, float r, float g, float b, const char* label) {
    if (fabs(vector.x) + fabs(vector.y) + fabs(vector.z) < 0.01f) return;
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glColor3f(r, g, b);
    glBegin(GL_LINES);
    glVertex3f(start.x, start.y, start.z);
    glVertex3f(start.x + vector.x * scale, start.y + vector.y * scale, start.z + vector.z * scale);
    glEnd();
    glEnable(GL_LIGHTING);
}

// Função simples de colisão AABB (Caixa vs Caixa)
bool checkCollisionAABB(float x1, float y1, float z1, float w1, float h1, float d1,
                        float x2, float y2, float z2, float w2, float h2, float d2) {
    return (x1 - w1/2 < x2 + w2/2 && x1 + w1/2 > x2 - w2/2) &&
           (y1 - h1/2 < y2 + h2/2 && y1 + h1/2 > y2 - h2/2) &&
           (z1 - d1/2 < z2 + d2/2 && z1 + d1/2 > z2 - d2/2);
}

// =================================================================================
// FUNÇÕES DO JOGO (EXPOSTAS NO HEADER)
// =================================================================================

void gameInit_3D() {
    // Inicializações únicas se necessário
}

void gameStartLevel_3D(int level) {
    printf("gameStartLevel_3D() chamado para o nivel %d.\n", level);

    // Inicializa VBO se ainda não foi criado
    if (cubeVBO == 0) {
        std::vector<tri> cubeTris = createUnitCubeTris();
        cubeVBO = criaVBODeTris((int)cubeTris.size(), cubeTris.data());
        printf("VBO Criado. ID: %d\n", cubeVBO);
    }

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    gameReshape_3D(w, h); // Força a projeção perspectiva

    // Limpa listas
    platforms3D.clear();
    windZones3D.clear();
    walls3D.clear();
    spikes3D.clear();

    // Reseta Player
    player3D.pos = {0.0f, 5.0f, 10.0f};
    player3D.vel = {0.0f, 0.0f, 0.0f};
    player3D.size = {1.5f, 3.0f, 1.5f};

    // Configurações do Nível
    levelParams_3D.playerMass = 10.0f;
    levelParams_3D.playerWalkAccel = 80.0f; // Aumentei para resposta mais rápida
    levelParams_3D.maxWalkSpeed = 15.0f;
    levelParams_3D.vectorVisualScale = 0.5f;

    isGameOver_3D = false;
    isGameVictory_3D = false;
    isGrounded_3D = false;
    
    // Configura Mouse
    glutSetCursor(GLUT_CURSOR_NONE);
    if(windowWidth > 0 && windowHeight > 0)
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
    mouseInitialized = false;
    isMouseFree = false;

    // --- CONSTRUÇÃO DO NÍVEL 1 ---
    if (level == 1) {
        // Plataforma Inicial (Chão Cinza)
        platforms3D.push_back({0, -2, 10, 20, 4, 20, 0.6f, 0.6f, 0.6f, 0.8f, true});

        // Corredor Longo
        platforms3D.push_back({0, -2, -20, 10, 4, 40, 0.7f, 0.7f, 0.7f, 0.8f, true});

        // Parede Quebrável (Vermelha)
        walls3D.push_back({0, 2, -15, 10, 6, 2, 40.0f, false});

        // Ponte de Gelo (Azul claro, atrito baixo 0.05)
        platforms3D.push_back({0, -2, -60, 10, 4, 40, 0.6f, 0.8f, 1.0f, 0.05f, true});

        // Fosso de Espinhos (Vermelho vivo)
        spikes3D.push_back({0, -6, -85, 20, 2, 10});

        // Plataforma de Aterrissagem (Roxa)
        platforms3D.push_back({0, -2, -100, 15, 4, 15, 0.6f, 0.4f, 0.8f, 0.8f, true});

        // Elevador de Vento (Semi-transparente)
        windZones3D.push_back({0, 10, -115, 10, 30, 10, {0, 55.0f, 0}});

        // Plataforma Final Alta (Verde)
        platforms3D.push_back({0, 30, -130, 20, 2, 20, 0.2f, 0.8f, 0.2f, 0.8f, true});
    }
}

GameAction gameUpdate_3D() {
    if (isGameOver_3D) return GAME_ACTION_CONTINUE;

    // Delta Time fixo para estabilidade
    float dt = 0.016f; 

    // Input e Direção da Câmera
    float yawRad = cameraYaw * M_PI / 180.0f;
    Vector3D forward = {-(float)sin(yawRad), 0, -(float)cos(yawRad)};
    Vector3D right = {-(float)cos(yawRad), 0, (float)sin(yawRad)}; // Cross product simplificado Y-up

    Vector3D accel = {0, GRAVITY_3D, 0};
    float moveForce = isGrounded_3D ? levelParams_3D.playerWalkAccel : levelParams_3D.playerWalkAccel * 0.3f;

    if (keyW) { accel.x += forward.x * moveForce; accel.z += forward.z * moveForce; }
    if (keyS) { accel.x -= forward.x * moveForce; accel.z -= forward.z * moveForce; }
    if (keyA) { accel.x -= right.x * moveForce;   accel.z -= right.z * moveForce; }
    if (keyD) { accel.x += right.x * moveForce;   accel.z += right.z * moveForce; }

    // Vento
    for (const auto& wind : windZones3D) {
        if (checkCollisionAABB(player3D.pos.x, player3D.pos.y, player3D.pos.z, player3D.size.x, player3D.size.y, player3D.size.z,
                               wind.x, wind.y, wind.z, wind.w, wind.h, wind.d)) {
            accel.x += wind.force.x;
            accel.y += wind.force.y;
            accel.z += wind.force.z;
        }
    }

    // Integração (Euler)
    player3D.vel.x += accel.x * dt;
    player3D.vel.y += accel.y * dt;
    player3D.vel.z += accel.z * dt;

    // Atrito (Damping) horizontal
    float friction = 0.92f; // Padrão ar
    
    // Detectar colisões com plataformas para chão e atrito
    isGrounded_3D = false;
    forceFriction_3D = {0,0,0};
    
    // Predição de posição Y para colisão
    float nextY = player3D.pos.y + player3D.vel.y * dt;

    for (const auto& plat : platforms3D) {
        if (checkCollisionAABB(player3D.pos.x, nextY, player3D.pos.z, player3D.size.x, player3D.size.y, player3D.size.z,
                               plat.x, plat.y, plat.z, plat.w, plat.h, plat.d)) {
            // Se estava caindo e bateu no topo
            if (player3D.vel.y <= 0 && player3D.pos.y > plat.y) {
                player3D.pos.y = plat.y + plat.h/2 + player3D.size.y/2;
                player3D.vel.y = 0;
                isGrounded_3D = true;
                
                // Aplica o atrito da plataforma (ex: 0.05 para gelo é muito escorregadio, 0.8 para chão para rápido)
                // Formula simplificada de atrito: vel *= (1.0 - friction)
                friction = 1.0f - (plat.friction * 0.1f); // Ajuste empírico
                if (friction < 0) friction = 0;
            }
        }
    }

    // Aplica atrito em X e Z
    player3D.vel.x *= friction;
    player3D.vel.z *= friction;

    // Paredes Quebráveis
    float nextX = player3D.pos.x + player3D.vel.x * dt;
    float nextZ = player3D.pos.z + player3D.vel.z * dt;

    for (auto& wall : walls3D) {
        if (!wall.isBroken) {
            if (checkCollisionAABB(nextX, player3D.pos.y, nextZ, player3D.size.x, player3D.size.y, player3D.size.z,
                                   wall.x, wall.y, wall.z, wall.w, wall.h, wall.d)) {
                
                float speed = sqrt(player3D.vel.x*player3D.vel.x + player3D.vel.z*player3D.vel.z);
                float impact = speed * levelParams_3D.playerMass;

                if (impact > wall.strength) {
                    wall.isBroken = true;
                    player3D.vel.x *= 0.5f;
                    player3D.vel.z *= 0.5f;
                } else {
                    // Colisão sólida
                    player3D.vel.x = 0;
                    player3D.vel.z = 0;
                    return GAME_ACTION_CONTINUE; // Impede movimento
                }
            }
        }
    }

    // Espinhos
    for (const auto& spike : spikes3D) {
        if (checkCollisionAABB(player3D.pos.x, player3D.pos.y, player3D.pos.z, player3D.size.x, player3D.size.y, player3D.size.z,
                               spike.x, spike.y, spike.z, spike.w, spike.h, spike.d)) {
            printf("MORREU NOS ESPINHOS!\n");
            player3D.pos = {0, 5, 10}; // Respawn
            player3D.vel = {0,0,0};
        }
    }

    // Atualiza Posição Final
    if(!isGrounded_3D) player3D.pos.y += player3D.vel.y * dt;
    player3D.pos.x += player3D.vel.x * dt;
    player3D.pos.z += player3D.vel.z * dt;

    // Morte por queda
    if (player3D.pos.y < -50.0f) {
        player3D.pos = {0, 5, 10};
        player3D.vel = {0,0,0};
    }

    return GAME_ACTION_CONTINUE;
}

void gameDisplay_3D() {
    // Setup de Iluminação
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);

    GLfloat lightPos0[] = { 50.0f, 100.0f, 50.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    GLfloat lightPos1[] = { -50.0f, 20.0f, -50.0f, 1.0f };
    GLfloat ambientColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientColor);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Posicionamento da Câmera
    float yawRad = cameraYaw * M_PI / 180.0f;
    float pitchRad = cameraPitch * M_PI / 180.0f;
    float camX = player3D.pos.x + cameraDistance * cos(pitchRad) * sin(yawRad);
    float camY = player3D.pos.y + cameraDistance * sin(pitchRad);
    float camZ = player3D.pos.z + cameraDistance * cos(pitchRad) * cos(yawRad);

    gluLookAt(camX, camY, camZ, 
              player3D.pos.x, player3D.pos.y, player3D.pos.z, 
              0.0f, 1.0f, 0.0f);

    // Se o VBO não foi criado por algum erro, desenha wireframes para não ficar tela preta
    if (cubeVBO == 0) {
        // Fallback de emergência
        glColor3f(1,0,0);
        glutWireCube(1.0);
    } else {
        // 1. Desenha Plataformas
        for (const auto& p : platforms3D) {
            drawCubeInstance(p.x, p.y, p.z, p.w, p.h, p.d, p.r, p.g, p.b, 1.0f);
        }

        // 2. Desenha Paredes
        for (const auto& w : walls3D) {
            if (!w.isBroken) {
                drawCubeInstance(w.x, w.y, w.z, w.w, w.h, w.d, 0.9f, 0.3f, 0.2f, 1.0f);
            }
        }

        // 3. Desenha Vento
        glDepthMask(GL_FALSE);
        for (const auto& wz : windZones3D) {
            drawCubeInstance(wz.x, wz.y, wz.z, wz.w, wz.h, wz.d, 0.6f, 0.8f, 1.0f, 0.4f);
            drawVector3D({wz.x, wz.y, wz.z}, wz.force, 0.1f, 1.0f, 1.0f, 1.0f, "");
        }
        glDepthMask(GL_TRUE);

        // 4. Desenha Espinhos
        for (const auto& s : spikes3D) {
            drawCubeInstance(s.x, s.y, s.z, s.w, s.h, s.d, 1.0f, 0.0f, 0.0f, 1.0f);
        }

        // 5. Desenha Player
        drawCubeInstance(player3D.pos.x, player3D.pos.y, player3D.pos.z, 
                         player3D.size.x, player3D.size.y, player3D.size.z, 
                         0.2f, 0.8f, 0.2f, 1.0f);
    }

    glutSwapBuffers();
}

void gameReshape_3D(int w, int h) {
    windowWidth = w; 
    windowHeight = h;
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w/h, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void gameMouseMotion_3D(int x, int y) {
    if (isMouseFree) return;
    
    int cx = windowWidth / 2;
    int cy = windowHeight / 2;

    if (!mouseInitialized) {
        glutWarpPointer(cx, cy);
        mouseInitialized = true;
        return;
    }

    float dx = (float)(x - cx);
    float dy = (float)(y - cy);

    if (dx == 0 && dy == 0) return;

    float sensitivity = 0.2f;
    cameraYaw -= dx * sensitivity;
    cameraPitch += dy * sensitivity;

    // Limitar Pitch
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;

    glutWarpPointer(cx, cy);
}

void gameMouseClick_3D(int button, int state) {
    // Implementar clique do gancho 3D futuramente
}

GameAction gameKeyDown_3D(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': case 'W': keyW = true; break;
        case 'a': case 'A': keyA = true; break;
        case 's': case 'S': keyS = true; break;
        case 'd': case 'D': keyD = true; break;
        case 'm': case 'M': 
            isMouseFree = !isMouseFree;
            glutSetCursor(isMouseFree ? GLUT_CURSOR_LEFT_ARROW : GLUT_CURSOR_NONE);
            if(!isMouseFree) glutWarpPointer(windowWidth/2, windowHeight/2);
            break;
        case 'q': case 'Q':
            glDisable(GL_LIGHTING);
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            return GAME_ACTION_EXIT_TO_MENU;
    }
    return GAME_ACTION_CONTINUE;
}

void gameKeyUp_3D(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': case 'W': keyW = false; break;
        case 'a': case 'A': keyA = false; break;
        case 's': case 'S': keyS = false; break;
        case 'd': case 'D': keyD = false; break;
    }
}

void gameSpecialDown_3D(int key, int x, int y) {}
void gameSpecialUp_3D(int key, int x, int y) {}
void loadGameTextures_3D() {}