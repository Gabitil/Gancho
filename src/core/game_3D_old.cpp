// #include "game_3D.h"
// #include <GL/freeglut.h>
// #include <stdio.h>
// // Necessário para M_PI e funções sin/cos
// #define _USE_MATH_DEFINES
// #include <cmath>

// // =================================================================================
// // ESTRUTURAS E VARIÁVEIS GLOBAIS (apenas para este módulo 3D)
// // =================================================================================

// // Estrutura simples para vetores 3D
// struct Vector3D {
//     float x, y, z;
// };

// // Estrutura para o nosso jogador 3D
// struct Player3D {
//     Vector3D pos;   // Posição (centro da caixa)
//     Vector3D vel;   // Velocidade
//     Vector3D size;  // Tamanho (largura, altura, profundidade)
// };

// // [2D-LOGIC] Estrutura movida do 2D para armazenar parâmetros de física
// struct LevelParameters {
//     // [2D-LOGIC] float gravity; (Usaremos a gravidade 3D)
//     // [2D-LOGIC]
//     float playerMass;
//     // [2D-LOGIC]
//     float playerWalkAccel;
//     // [2D-LOGIC]
//     float maxWalkSpeed;
//     // [2D-LOGIC]
//     float maxPlayerSpeed;
//     // [2D-LOGIC] float dampingFactor; (Para o gancho)
//     // [2D-LOGIC] float hookSpeed; (Para o gancho)
//     // [2D-LOGIC] float maxPullStrengthPhysics; (Para o gancho)
//     // [2D-LOGIC]
//     float vectorVisualScale;
//     // [2D-LOGIC] int maxShots; (Para o gancho)

//     // NOVO (3D): Adicionado para simular o atrito do "chão" (plataforma 0 do 2D)
//     float floorFrictionCoefficient;
// };
// // [2D-LOGIC]
// LevelParameters levelParameters_3D;


// // --- Variáveis de Estado do Jogo 3D ---
// Player3D player3D;
// const float WORLD_SIZE = 100.0f; // Tamanho do nosso "chão" (de -100 a +100)
// const float FLOOR_Y = 0.0f;      // Posição Y do chão

// // --- Variáveis de Controle da Câmera ---
// float cameraYaw = 0.0f;     // Rotação horizontal (controlada pelo mouse X)
// float cameraPitch = 20.0f;  // Rotação vertical (controlada pelo mouse Y)
// float cameraDistance = 15.0f; // Distância da câmera ao jogador

// // NOVO: Variáveis para controle do mouse relativo
// static int windowWidth = 800;  // Valor padrão
// static int windowHeight = 600; // Valor padrão
// bool mouseInitialized = false;

// // --- Variáveis de Input ---
// bool keyW = false, keyA = false, keyS = false, keyD = false;
// bool isGrounded_3D = true;

// // Variável para controlar se o cursor está livre (fora do modo FPS)
// bool isMouseFree = false;

// // --- IDs das Display Lists (Requisito Técnico) ---
// GLuint floorListID = 0;

// // --- Parâmetros de Física ---
// // const float PLAYER_MOVE_SPEED = 10.0f; // [REMOVIDO] Substituído pela lógica 2D (levelParameters_3D)
// const float GRAVITY = -20.0f;
// const float MOUSE_SENSITIVITY = 0.2f;

// // [2D-LOGIC] Constante para visualização do vetor velocidade
// // [2D-LOGIC]
// const float VELOCITY_VISUAL_SCALE = 7.0f;

// // --- [2D-LOGIC] Variáveis de Força e Aceleração (Adaptadas do 2D) ---
// // [2D-LOGIC]
// float forceNormal_3D = 0;
// // [2D-LOGIC]
// Vector3D forceFriction_3D = {0, 0, 0};
// // [2D-LOGIC]
// Vector3D forceTension_3D = {0, 0, 0}; // (Para o gancho, no futuro)
// // [2D-LOGIC]
// float lastVelocityMag_3D = 0.0f;
// // [2D-LOGIC]
// float currentAcceleration_3D = 0.0f;


// // =================================================================================
// // FUNÇÕES AUXILIARES
// // =================================================================================

// /**
//  * Função auxiliar para desenhar texto em 3D (para os vetores)
//  */
// void drawText3D(float x, float y, float z, const char* text) {
//     glRasterPos3f(x, y, z);
//     while (*text) {
//         glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *text);
//         text++;
//     }
// }

// /**
//  * Função auxiliar para desenhar vetores 3D (Adaptada da lógica 2D)
//  */
// void drawVector3D(Vector3D start, Vector3D vector, float scale, float r, float g, float b, const char* label) {
//     // Cálculo do ponto final bruto
//     Vector3D end;
    
//     bool isForce = (label[0] != 'V'); 
//     float finalScale = scale;
    
//     Vector3D vecToDraw = vector;

//     if (isForce) {
//         if (levelParameters_3D.playerMass <= 0.001f) return;
//         vecToDraw.x /= levelParameters_3D.playerMass;
//         vecToDraw.y /= levelParameters_3D.playerMass;
//         vecToDraw.z /= levelParameters_3D.playerMass;
//     }

//     // --- CORREÇÃO: LIMITADOR DE TAMANHO (CLAMPING) ---
//     // Tamanho máximo reduzido para 3 vezes a altura do personagem
//     float maxLen = 3.0f * player3D.size.y; 
//     float currentLen = sqrt(vecToDraw.x*vecToDraw.x + vecToDraw.y*vecToDraw.y + vecToDraw.z*vecToDraw.z) * finalScale;

//     if (currentLen > maxLen && currentLen > 0.001f) {
//         // Reduz a escala para caber no tamanho máximo mantendo a proporção
//         finalScale = finalScale * (maxLen / currentLen);
//     }

//     // Recalcula o tamanho atual para verificação de desenho mínimo
//     currentLen = sqrt(vecToDraw.x*vecToDraw.x + vecToDraw.y*vecToDraw.y + vecToDraw.z*vecToDraw.z) * finalScale;

//     end.x = start.x + vecToDraw.x * finalScale;
//     end.y = start.y + vecToDraw.y * finalScale;
//     end.z = start.z + vecToDraw.z * finalScale;

//     // Se o vetor for muito pequeno (quase zero), não desenha para não poluir
//     if (currentLen < 0.1f) return;

//     // 1. Desenha a Linha do Vetor
//     glDisable(GL_LIGHTING);
//     glLineWidth(3.0f); 
//     glColor3f(r, g, b);

//     glBegin(GL_LINES);
//         glVertex3f(start.x, start.y, start.z);
//         glVertex3f(end.x, end.y, end.z);
//     glEnd();

//     // Desenha uma pequena "ponta"
//     glPushMatrix();
//         glTranslatef(end.x, end.y, end.z);
//         glutSolidCube(0.05f); 
//     glPopMatrix();

//     // 2. Desenha o Texto (Label)
//     glDisable(GL_DEPTH_TEST); // Garante que o texto fique "na frente"
//     glColor3f(1.0f, 1.0f, 1.0f); 
    
//     glRasterPos3f(end.x, end.y + 0.2f, end.z);
    
//     const char* c = label;
//     while (*c) {
//         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
//         c++;
//     }
    
//     glEnable(GL_DEPTH_TEST);
//     glEnable(GL_LIGHTING);
//     glLineWidth(1.0f);
// }


// /**
//  * Cria as Display Lists para os objetos estáticos do mundo (chão).
//  */
// void createWorldDisplayLists() {
//     // 1. Deletar listas antigas se existirem
//     if (floorListID != 0) glDeleteLists(floorListID, 1);

//     // 2. Criar a Display List do Chão (ALTERADO)
//     floorListID = glGenLists(1);
//     glNewList(floorListID, GL_COMPILE);
//         // Desabilita iluminação para a grade (para a cor ser pura)
//         glDisable(GL_LIGHTING);
//         glLineWidth(1.0f);
//         glColor3f(0.4f, 0.4f, 0.4f); // Cor da grade (cinza escuro)

//         glBegin(GL_LINES);
//         // Desenha linhas ao longo do eixo Z (variando X)
//         for (float i = -WORLD_SIZE; i <= WORLD_SIZE; i += 1.0f) { // Espaçamento de 1 unidade
//             glVertex3f(i, FLOOR_Y, -WORLD_SIZE);
//             glVertex3f(i, FLOOR_Y,  WORLD_SIZE);
//         }
//         // Desenha linhas ao longo do eixo X (variando Z)
//         for (float i = -WORLD_SIZE; i <= WORLD_SIZE; i += 1.0f) {
//             glVertex3f(-WORLD_SIZE, FLOOR_Y, i);
//             glVertex3f( WORLD_SIZE, FLOOR_Y, i);
//         }
//         glEnd();
//         // Reabilita a iluminação para o resto da cena
//         glEnable(GL_LIGHTING);
//     glEndList();
// }

// // =================================================================================
// // IMPLEMENTAÇÃO DA API game_3D.h
// // =================================================================================

// /**
//  * Chamada quando o nível 3D é iniciado.
//  */
// void gameStartLevel_3D(int level) {
//     printf("gameStartLevel_3D() chamado para o nivel %d.\n", level);

//     // --- Configuração do OpenGL ---
//     glEnable(GL_LIGHTING);
//     glEnable(GL_LIGHT0);
//     glEnable(GL_COLOR_MATERIAL);
//     glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//     glEnable(GL_DEPTH_TEST);
//     glDepthFunc(GL_LEQUAL);
//     glClearColor(0.1f, 0.1f, 0.3f, 1.0f);

//     // --- Inicialização do Jogo ---
//     player3D.pos = {0.0f, 3.0f, 0.0f};
//     player3D.vel = {0.0f, 0.0f, 0.0f};
//     player3D.size = {1.0f, 2.0f, 1.0f};

//     keyW = keyA = keyS = keyD = false;
//     cameraYaw = 0.0f;
//     cameraPitch = 20.0f;

//     // --- Parâmetros de Física (Ajustados) ---
//     levelParameters_3D.playerMass = 10.0f;
    
//     // CORREÇÃO: Aceleração aumentada para 60.0f para vencer o atrito (que é aprox 16.0f)
//     levelParameters_3D.playerWalkAccel = 60.0f; 
    
//     levelParameters_3D.maxWalkSpeed = 15.0f; // Um pouco mais rápido para 3D
//     levelParameters_3D.maxPlayerSpeed = 40.0f;
//     levelParameters_3D.floorFrictionCoefficient = 0.8f; 
//     levelParameters_3D.vectorVisualScale = 0.5f; // Escala visual ajustada
    
//     // Resetar variáveis
//     lastVelocityMag_3D = 0.0f;
//     currentAcceleration_3D = 0.0f;
//     forceNormal_3D = 0;
//     forceFriction_3D = {0,0,0};
//     forceTension_3D = {0,0,0};
//     isGrounded_3D = true;

//     // Mouse setup
//     glutSetCursor(GLUT_CURSOR_NONE);
//     int w = glutGet(GLUT_WINDOW_WIDTH);
//     int h = glutGet(GLUT_WINDOW_HEIGHT);
//     gameReshape_3D(w, h);
//     glutWarpPointer(windowWidth / 2, windowHeight / 2);
//     mouseInitialized = false;

//     createWorldDisplayLists();
// }

// /**
//  * Loop principal de física e lógica do jogo.
//  */
// GameAction gameUpdate_3D() {
//     // 1. Calcular DeltaTime
//     static int lastTime = 0;
//     if (lastTime == 0) lastTime = glutGet(GLUT_ELAPSED_TIME);
//     int currentTime = glutGet(GLUT_ELAPSED_TIME);
//     float dt = (float)(currentTime - lastTime) / 1000.0f;
//     lastTime = currentTime;

//     // Evitar dt muito grande em travamentos (max 0.1s)
//     if (dt > 0.1f) dt = 0.1f;

//     // --- Cálculo de Aceleração para HUD ---
//     float currentVelocityMag = sqrt(player3D.vel.x * player3D.vel.x + 
//                                     player3D.vel.y * player3D.vel.y + 
//                                     player3D.vel.z * player3D.vel.z);
//     if (dt > 0.0001f) {
//         currentAcceleration_3D = (currentVelocityMag - lastVelocityMag_3D) / dt;
//     }
//     lastVelocityMag_3D = currentVelocityMag;

//     // Resetar forças visuais
//     forceNormal_3D = 0;
//     forceFriction_3D = {0,0,0};

//     // 2. Direção da Câmera
//     float yawRad = cameraYaw * M_PI / 180.0f;
//     Vector3D forward = {-(float)sin(yawRad), 0, -(float)cos(yawRad)};
//     Vector3D right = {-forward.z, 0, forward.x};

//     // 3. Calcular Aceleração de Entrada (Input)
//     Vector3D inputAccel = {0, 0, 0};
    
//     if (isGrounded_3D) {
//         if (keyW) { inputAccel.x += forward.x; inputAccel.z += forward.z; }
//         if (keyS) { inputAccel.x -= forward.x; inputAccel.z -= forward.z; }
//         // A/D corrigidos (Esquerda/Direita)
//         if (keyA) { inputAccel.x -= right.x;   inputAccel.z -= right.z; } 
//         if (keyD) { inputAccel.x += right.x;   inputAccel.z += right.z; }

//         // Normalizar vetor de entrada para não andar mais rápido na diagonal
//         float inputMag = sqrt(inputAccel.x*inputAccel.x + inputAccel.z*inputAccel.z);
//         if (inputMag > 0.01f) {
//             inputAccel.x /= inputMag;
//             inputAccel.z /= inputMag;
//             inputAccel.x *= levelParameters_3D.playerWalkAccel;
//             inputAccel.z *= levelParameters_3D.playerWalkAccel;
//         }
//     }

//     // 4. Integrar Aceleração de Input na Velocidade (Tentativa de movimento)
//     player3D.vel.x += inputAccel.x * dt;
//     player3D.vel.z += inputAccel.z * dt;

//     // 5. Aplicar Atrito (Lógica do 2D adaptada)
//     if (isGrounded_3D) {
//         // Normal = Massa * Gravidade (em módulo)
//         forceNormal_3D = -GRAVITY * levelParameters_3D.playerMass; // Ex: 20 * 10 = 200N
        
//         // Força de atrito máxima disponível = mu * N
//         float maxFrictionForce = levelParameters_3D.floorFrictionCoefficient * forceNormal_3D;
        
//         // Desaceleração causada pelo atrito = Força / Massa
//         float frictionDecel = maxFrictionForce / levelParameters_3D.playerMass; // = mu * g

//         // Velocidade atual no plano XZ
//         float speedXZ = sqrt(player3D.vel.x*player3D.vel.x + player3D.vel.z*player3D.vel.z);

//         if (speedXZ > 0.001f) {
//             // Se a velocidade é muito baixa e o atrito é forte o suficiente para parar tudo num frame:
//             if (speedXZ < frictionDecel * dt) {
//                 // Pára completamente (Atrito estático/parada final)
//                 // Mas só pára se não houver input forte tentando mover
//                 float inputLen = sqrt(inputAccel.x*inputAccel.x + inputAccel.z*inputAccel.z);
//                 if (inputLen < frictionDecel) { 
//                      player3D.vel.x = 0;
//                      player3D.vel.z = 0;
//                 }
//             } else {
//                 // Atrito Cinético: Aplica força contrária à velocidade
//                 float factorX = player3D.vel.x / speedXZ;
//                 float factorZ = player3D.vel.z / speedXZ;
                
//                 player3D.vel.x -= factorX * frictionDecel * dt;
//                 player3D.vel.z -= factorZ * frictionDecel * dt;

//                 // Guarda vetor para visualização (aponta contra o movimento)
//                 forceFriction_3D.x = -factorX * maxFrictionForce;
//                 forceFriction_3D.z = -factorZ * maxFrictionForce;
//             }
//         }
//     }

//     // 6. Aplicar Gravidade
//     player3D.vel.y += GRAVITY * dt;

//     // 7. Limitar Velocidade Máxima (Walk Speed) no XZ
//     if (isGrounded_3D) {
//         float speedXZ = sqrt(player3D.vel.x*player3D.vel.x + player3D.vel.z*player3D.vel.z);
//         if (speedXZ > levelParameters_3D.maxWalkSpeed) {
//             float ratio = levelParameters_3D.maxWalkSpeed / speedXZ;
//             player3D.vel.x *= ratio;
//             player3D.vel.z *= ratio;
//         }
//     }

//     // 8. Atualizar Posição
//     player3D.pos.x += player3D.vel.x * dt;
//     player3D.pos.y += player3D.vel.y * dt;
//     player3D.pos.z += player3D.vel.z * dt;

//     // 9. Colisão com o chão
//     float playerBottom = player3D.pos.y - (player3D.size.y / 2.0f);
//     if (playerBottom <= FLOOR_Y) {
//         player3D.pos.y = FLOOR_Y + (player3D.size.y / 2.0f);
//         player3D.vel.y = 0;
//         isGrounded_3D = true;
//     } else {
//         isGrounded_3D = false;
//     }

//     // Limites do mundo
//     if (player3D.pos.x > WORLD_SIZE) player3D.pos.x = WORLD_SIZE;
//     if (player3D.pos.x < -WORLD_SIZE) player3D.pos.x = -WORLD_SIZE;
//     if (player3D.pos.z > WORLD_SIZE) player3D.pos.z = WORLD_SIZE;
//     if (player3D.pos.z < -WORLD_SIZE) player3D.pos.z = -WORLD_SIZE;

//     return GAME_ACTION_CONTINUE;
// }

// /**
//  * Loop principal de renderização.
//  */
// void gameDisplay_3D() {
//     // 1. Limpar buffers
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     // 2. Configurar Matriz ModelView
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();

//     // 3. Posicionar a Luz
//     GLfloat light_pos[] = {0.0f, 100.0f, 0.0f, 1.0f};
//     glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

//     // 4. Calcular Posição da Câmera
//     float yawRad = cameraYaw * M_PI / 180.0f;
//     float pitchRad = cameraPitch * M_PI / 180.0f;

//     float offsetX = cameraDistance * cos(pitchRad) * sin(yawRad);
//     float offsetY = cameraDistance * sin(pitchRad);
//     float offsetZ = cameraDistance * cos(pitchRad) * cos(yawRad);

//     float camX = player3D.pos.x + offsetX;
//     float camY = player3D.pos.y + offsetY;
//     float camZ = player3D.pos.z + offsetZ;
    
//     gluLookAt(camX, camY, camZ,
//               player3D.pos.x, player3D.pos.y, player3D.pos.z,
//               0.0f, 1.0f, 0.0f);

//     // 5. Desenhar o Céu (Não mais uma lista, apenas limpa a cor)
//     // O glClearColor já define o fundo azul escuro.

//     // 6. Desenhar o Chão (Grade)
//     // A iluminação foi desabilitada dentro da Display List
//     glCallList(floorListID);

//     // 7. Desenhar o Jogador
//     glEnable(GL_LIGHTING); // Garante que a iluminação esteja ligada para o jogador
//     glPushMatrix();
//         glTranslatef(player3D.pos.x, player3D.pos.y, player3D.pos.z);
//         glScalef(player3D.size.x, player3D.size.y, player3D.size.z);
//         glColor3f(1.0f, 0.0f, 0.0f);
//         glutSolidCube(1.0);
//     glPopMatrix();


//     // 8. [2D-LOGIC] Desenhar Vetores Físicos
//     // [2D-LOGIC]
//     Vector3D playerCenter = player3D.pos; // Posição já é o centro

//     // [2D-LOGIC] drawVector(0, 0, 0, levelParameters_3D.gravity, ... "P");
//     // Adaptado para 3D: Vetor Peso (Força = m * g)
//     Vector3D forceP = {0, GRAVITY * levelParameters_3D.playerMass, 0};
//     // [2D-LOGIC]
//     drawVector3D(playerCenter, forceP, levelParameters_3D.vectorVisualScale, 0.2f, 0.2f, 1.0f, "P");

//     // [2D-LOGIC] float velMag = sqrt(player.velocityX * player.velocityX + ...
//     float velMag = sqrt(player3D.vel.x*player3D.vel.x + player3D.vel.y*player3D.vel.y + player3D.vel.z*player3D.vel.z);
//     // [2D-LOGIC]
//     if (velMag > 0.01) {
//         // [2D-LOGIC] drawVector(0, 0, player.velocityX, player.velocityY, ... "V");
//         // (player3D.vel já é um Vector3D)
//         // [2D-LOGIC]
//         drawVector3D(playerCenter, player3D.vel, VELOCITY_VISUAL_SCALE, 1.0f, 0.5f, 0.0f, "V");
//     }
    
//     // [2D-LOGIC] if (isHooked) { ... "T" } (Ainda não implementado)

//     // [2D-LOGIC]
//     if (isGrounded_3D) {
//         // [2D-LOGIC] drawVector(0, -player.h / 2, 0, forceNormal, ... "N");
//         // Vetor Normal (N) aponta para cima, partindo da base do jogador
//         Vector3D normalStart = {playerCenter.x, playerCenter.y - (player3D.size.y / 2.0f), playerCenter.z};
//         Vector3D forceN = {0, forceNormal_3D, 0};
//         // [2D-LOGIC]
//         drawVector3D(normalStart, forceN, levelParameters_3D.vectorVisualScale, 0.0f, 1.0f, 1.0f, "N");

//         // [2D-LOGIC] if (fabs(forceFriction) > 0.001f) {
//         float frictionMag = sqrt(forceFriction_3D.x*forceFriction_3D.x + forceFriction_3D.z*forceFriction_3D.z);
//         // [2D-LOGIC]
//         if (frictionMag > 0.001f) {
//             // [2D-LOGIC] drawVector(0, -player.h / 2 + 5, forceFriction, 0, ... "Fat");
//             // Começa um pouco acima da base
//             Vector3D frictionStart = {normalStart.x, normalStart.y + 0.1f, normalStart.z};
//             // A força de atrito (forceFriction_3D) já é um Vector3D
//             // [2D-LOGIC]
//             drawVector3D(frictionStart, forceFriction_3D, levelParameters_3D.vectorVisualScale, 1.0f, 0.0f, 0.0f, "Fat");
//         }
//     }
// }

// /**
//  * Chamada quando a janela é redimensionada.
//  */
// void gameReshape_3D(int w, int h) {
//     // Atualiza as variáveis globais imediatamente
//     windowWidth = w;
//     windowHeight = h;

//     if (h == 0) h = 1;
//     float ratio = 1.0f * w / h;

//     // Configura a Viewport
//     glViewport(0, 0, w, h);

//     // Configura a Matriz de Projeção
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluPerspective(45.0f, ratio, 0.1f, 1000.0f);
    
//     // Volta para ModelView para não afetar renderizações futuras
//     glMatrixMode(GL_MODELVIEW);
// }

// /**
//  * Chamada quando o mouse se move (ativo ou passivo).
//  */
// /**
//  * Chamada quando o mouse se move (ativo ou passivo).
//  */
// void gameMouseMotion_3D(int x, int y) {
//     // Se o mouse estiver livre (modo menu/pausa), não rotaciona a câmera
//     if (isMouseFree) return;

//     // Calcula o centro da tela
//     int centerX = windowWidth / 2;
//     int centerY = windowHeight / 2;
    
//     if (!mouseInitialized) {
//         mouseInitialized = true;
//         if (x == centerX && y == centerY) return;
//     }

//     // Calcula o delta (diferença) do centro
//     float deltaX = (float)(x - centerX);
//     float deltaY = (float)(y - centerY);

//     // Se não houve movimento, não faz nada
//     if (deltaX == 0 && deltaY == 0) return;

//     // Aplica o delta à rotação da câmera
//     // CORREÇÃO: Invertido o sinal de += para -= para corrigir a inversão esquerda/direita
//     cameraYaw -= deltaX * MOUSE_SENSITIVITY; 
    
//     // Mantém o Pitch como estava (se estiver invertido também, troque para -=)
//     cameraPitch += deltaY * MOUSE_SENSITIVITY;

//     // Limites da câmera (Pitch)
//     if (cameraPitch > 89.0f) {
//         cameraPitch = 89.0f;
//     }
    
//     // Limite para olhar para baixo
//     float minPitch = (isGrounded_3D) ? 1.0f : -89.0f;
//     if (cameraPitch < minPitch) {
//         cameraPitch = minPitch;
//     }

//     // Força o mouse a voltar para o centro da tela
//     glutWarpPointer(centerX, centerY);
// }
// /**
//  * Chamada quando um botão do mouse é clicado.
//  */
// void gameMouseClick_3D(int button, int state) {
//     if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
//         printf("Disparar gancho 3D!\n");
//         // Lógica do gancho 3D
//     }
// }

// /**
//  * Chamada quando uma tecla é pressionada.
//  */
// GameAction gameKeyDown_3D(unsigned char key, int x, int y) {
//     switch (key) {
//         case 'w': case 'W': keyW = true; break;
//         case 'a': case 'A': keyA = true; break;
//         case 's': case 'S': keyS = true; break;
//         case 'd': case 'D': keyD = true; break;
        
//         // Lógica para alternar o modo do Mouse
//         case 'm': case 'M': 
//             isMouseFree = !isMouseFree; // Inverte o estado
//             if (isMouseFree) {
//                 glutSetCursor(GLUT_CURSOR_LEFT_ARROW); // Mostra o mouse
//             } else {
//                 glutSetCursor(GLUT_CURSOR_NONE); // Esconde o mouse
//                 // Centraliza para evitar "pulo" da câmera ao voltar
//                 glutWarpPointer(windowWidth / 2, windowHeight / 2);
//                 mouseInitialized = false; 
//             }
//             break;

//         case 'q': case 'Q':
//             // --- LIMPEZA DO ESTADO OPENGL ANTES DE SAIR ---
//             // Isso corrige o menu escuro
//             glDisable(GL_LIGHTING); 
//             glDisable(GL_DEPTH_TEST);
//             glDisable(GL_COLOR_MATERIAL);
//             // Reseta a cor atual para branco puro, caso contrário o menu fica tingido
//             glColor4f(1.0f, 1.0f, 1.0f, 1.0f); 
            
//             // Mostra o cursor ao sair
//             glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
//             return GAME_ACTION_EXIT_TO_MENU;
//     }
//     return GAME_ACTION_CONTINUE;
// }

// /**
//  * Chamada quando uma tecla é solta.
//  */
// void gameKeyUp_3D(unsigned char key, int x, int y) {
//     switch (key) {
//         // [2D-LOGIC] case 'a': case 'A': isPressingLeft = false; break;
//         // [2D-LOGIC] case 'd': case 'D': isPressingRight = false; break;
//         case 'w': case 'W': keyW = false; break;
//         case 'a': case 'A': keyA = false; break;
//         case 's': case 'S': keyS = false; break;
//         case 'd': case 'D': keyD = false; break;
//     }
// }

// // Funções de Teclas Especiais (não usadas aqui)
// void gameSpecialDown_3D(int key, int x, int y) {}
// void gameSpecialUp_3D(int key, int x, int y) {}

// /**
//  * Carrega texturas 3D (vazio por enquanto).
//  */
// void loadGameTextures_3D() {
//     printf("loadGameTextures_3D() chamado. (Nao ha texturas para carregar)\n");
// }