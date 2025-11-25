// Header correspondente primeiro
#include "game_3D.h" 
#include "game.h" 

// ==================================================================================
// ## 1. CONSTANTES GLOBAIS
// ==================================================================================

/**
 * Constantes usadas na definição de mundo e para visualização.
 */
const float PLAYER_HEIGHT_3D = 80.0f;
const float VIEW_WIDTH_3D = 800.0f;
const float VIEW_HEIGHT_3D = 600.0f;

// O mundo agora é um volume
const float WORLD_WIDTH_3D = 3000.0f;
const float WORLD_HEIGHT_3D = 600.0f;
const float WORLD_DEPTH_3D = 1000.0f; 

const float DEATH_BOUNDARY_Y_3D = -100.0f; 

/**
 * Constantes visuais e de input para o modo FPS e Carregamento.
 */
const float MAX_VISUAL_AIM_LENGTH_3D = 5.0f * PLAYER_HEIGHT_3D;
const float MAX_AIM_FORCE_DISPLAY_3D = 100.0f;
const float VELOCITY_VISUAL_SCALE_3D = 7.0f;
const float WIND_VECTOR_VISUAL_SCALE_3D = 500.0f;

// Novos constantes para o sistema FPS/Carregamento
const float MOUSE_SENSITIVITY_3D = 0.1f;    // Velocidade de rotação da câmera (sensibilidade do mouse)
const float MAX_CHARGE_TIME_3D = 2.0f;      // Tempo máximo em segundos para carregar a força total
const float MAX_PITCH_3D = 89.0f;           // Limite vertical superior da câmera (Pitch)
const float MIN_PITCH_3D = -89.0f;          // Limite vertical inferior da câmera (Pitch)

// ==================================================================================
// ## 2. OBJETOS DO JOGO E PARÂMETROS DE NÍVEL
// ==================================================================================

GameObject_3D player_3D;
GameObject_3D door_3D;
Platform_3D* collidingPlatform_3D = NULL;

// Vetores para armazenar as definições de objetos 3D do nível
std::vector<Platform_3D> platforms_3D;
std::vector<BreakableWall_3D> breakableWalls_3D;
std::vector<WindZone_3D> windZones_3D;
std::vector<SpikeZone_3D> spikeZones_3D;

// Vetores para armazenar os VBOs (Dados carregados na GPU)
std::vector<VBO_Info> platformVBOs;
std::vector<VBO_Info> wallVBOs;
std::vector<VBO_Info> spikeVBOs;

// Constante para a profundidade do mundo (Z-dimension)
const float WORLD_DEPTH = 50.0f;
const float Z_CENTER = 0.0f;

struct LevelParameters_3D {
    float gravity; 
    float playerMass;
    float playerWalkAccel;
    float maxWalkSpeed;
    float maxPlayerSpeed;
    float dampingFactor;
    float hookSpeed;
    float maxPullStrengthPhysics;
    float vectorVisualScale;
    int maxShots;
};

LevelParameters_3D levelParameters_3D;

// ==================================================================================
// ## 3. CÂMERA E CONTROLES DE MOVIMENTO FPS
// ==================================================================================

/**
 * Posição absoluta da Câmera (Geralmente a posição do player + um offset de altura).
 */
float cameraPosX_3D = 0.0f; 
float cameraPosY_3D = 0.0f; 
float cameraPosZ_3D = 200.0f; 

/**
 * Ângulos de Euler (Yaw/Pitch) para controle da mira (Mouse Look).
 */
float cameraYaw_3D = -90.0f;  // Rotação horizontal (eixo Y)
float cameraPitch_3D = 0.0f;  // Rotação vertical (eixo X)

/**
 * Vetor unitário que define a direção para onde o player está olhando (Direção do Tiro).
 * É calculado a partir de cameraYaw_3D e cameraPitch_3D.
 */
float cameraFrontX_3D = 0.0f;
float cameraFrontY_3D = 0.0f;
float cameraFrontZ_3D = -1.0f;

/**
 * Controles de Movimento (Input).
 */
bool isPressingLeft_3D = false;
bool isPressingRight_3D = false;
bool isPressingForward_3D = false; 
bool isPressingBackward_3D = false; 

// A rotação simples isFacingRight_3D é menos útil, o Yaw define a direção.
// float playerRotationAngle_3D = 0.0f; // Mantido se o modelo do player for desenhado
bool isFacingRight_3D = true; 

bool isMouseFree_3D = false;      // Se true, o mouse aparece e não gira a câmera (Menu/Pause)
bool mouseInitialized_3D = false; // Controle interno para evitar pulo no primeiro frame

// ==================================================================================
// ## 4. ESTADO DO GANCHO E SISTEMA DE CARREGAMENTO (CHARGE)
// ==================================================================================

// Estados principais
bool isGrounded_3D = false; 
bool isHooked_3D = false;   
bool isHookFiring_3D = false;

// Estado de carregamento do gancho (NOVO)
bool isChargingHook_3D = false;     // Verdadeiro enquanto o botão de tiro está pressionado
float chargeStartTime_3D = 0.0f;    // Tempo em ms/s que o carregamento começou
float currentChargeForce_3D = 0.0f; // Força de lançamento atual (0 a MAX_AIM_FORCE_DISPLAY_3D)
float chargePercentage_3D = 0.0f;   // Percentual de carga (0.0 a 1.0) para UI

// Variável para sinalizar a soltura da corda (Segundo clique, se isHooked_3D for true) (NOVO)
bool requestHookRelease_3D = false;

// Ponto onde o gancho prendeu (Coordenadas 3D)
float hookPointX_3D = 0;
float hookPointY_3D = 0;
float hookPointZ_3D = 0; 

// Posição do projétil do gancho (Coordenadas 3D)
float hookProjectileX_3D = 0;
float hookProjectileY_3D = 0;
float hookProjectileZ_3D = 0; 

// Velocidade do projétil (Coordenadas 3D)
float hookProjectileVelX_3D = 0;
float hookProjectileVelY_3D = 0;
float hookProjectileVelZ_3D = 0; 

// Posição de onde sai o gancho (Geralmente um pouco deslocado da câmera) (NOVO)
float hookOriginX_3D = 0.0f; 
float hookOriginY_3D = 0.0f; 
float hookOriginZ_3D = 0.0f;

float ropeLength_3D = 0;
float currentPullForce_3D = 0.0f;

// aimDisplayForce_3D agora é redundante com currentChargeForce_3D. Mantenho apenas para consistência.
float aimDisplayForce_3D = 0; 

// ==================================================================================
// ## 5. VETORES FÍSICOS (Cinemática Vetorial 3D)
// ==================================================================================

// Forças Físicas (Adicionado Z para todos os vetores)
// Para o objetivo educacional, é importante manter os componentes para visualização.

float forceNormalX_3D = 0; 
float forceNormalY_3D = 0; 
float forceNormalZ_3D = 0; 

float forceFrictionX_3D = 0; 
float forceFrictionY_3D = 0; 
float forceFrictionZ_3D = 0; 

float forceTensionX_3D = 0;
float forceTensionY_3D = 0;
float forceTensionZ_3D = 0; 

// Força Resultante (Para visualizar a aceleração total) (NOVO)
float forceResultantX_3D = 0;
float forceResultantY_3D = 0;
float forceResultantZ_3D = 0;

// ==================================================================================
// ## 6. ACELERAÇÃO, ESTADO E RENDERIZAÇÃO
// ==================================================================================

float lastVelocityMag_3D = 0.0f;
float currentAcceleration_3D = 0.0f;

int shotsRemaining_3D = 0;
bool isGameOver_3D = false;
int gameOverTimer_3D = 0;
bool isGameVictory_3D = false;
int gameVictoryTimer_3D = 0;

float runAnimationTimer_3D = 0.0f;
int currentRunFrame_3D = 0;

static int lastTime_3D = 0;

GLuint platformListID_3D = 0;
GLuint doorListID_3D = 0;
GLuint spikeListID_3D = 0;
GLuint windZoneListID_3D = 0;

// =================================================================================
// FUNÇÕES
// =================================================================================

/**
 * Cria os VBOs para toda a geometria estática do nível 3D.
 */
void createLevelVBOs() {
    // 1. Limpeza de VBOs antigos (libera a memória da GPU)
    for (auto& info : platformVBOs) { if (info.vboID != 0) glDeleteBuffers(1, &info.vboID); }
    platformVBOs.clear();
    
    for (auto& info : wallVBOs) { if (info.vboID != 0) glDeleteBuffers(1, &info.vboID); }
    wallVBOs.clear();
    
    for (auto& info : spikeVBOs) { if (info.vboID != 0) glDeleteBuffers(1, &info.vboID); }
    spikeVBOs.clear();

    // 2. Processar Plataformas
    for (const auto& p : platforms_3D) {
        int triCount = 0;
        // Gera os triângulos na CPU
        tri* platformTris = createBoxTris(p.x, p.y, p.z, p.w, p.h, p.d, p.r, p.g, p.b, triCount);

        // Envia para a GPU e guarda o ID
        VBO_Info info;
        info.vboID = criaVBODeTris(triCount, platformTris);
        info.triCount = triCount;
        platformVBOs.push_back(info);

        // Limpa a memória da CPU, pois já está na placa de vídeo
        free(platformTris); 
    }
    
    // 3. Processar Paredes Quebráveis
    for (const auto& w : breakableWalls_3D) {
        if (w.isBroken) continue; // Não desenha se já quebrou
        
        int triCount = 0;
        // Paredes com cor fixa (ex: marrom/laranja) ou usando w.r, w.g, w.b se sua struct tiver cor
        // Assumindo que sua struct BreakableWall_3D tem cor:
        tri* wallTris = createBoxTris(w.x, w.y, w.z, w.w, w.h, w.d, w.r, w.g, w.b, triCount); 

        VBO_Info info;
        info.vboID = criaVBODeTris(triCount, wallTris);
        info.triCount = triCount;
        wallVBOs.push_back(info);

        free(wallTris);
    }

    // 4. Processar Espinhos
    for (const auto& s : spikeZones_3D) {
        int triCount = 0;
        // Espinhos vermelhos
        tri* spikeTris = createBoxTris(s.x, s.y, s.z, s.w, s.h, s.d, 1.0f, 0.0f, 0.0f, triCount);

        VBO_Info info;
        info.vboID = criaVBODeTris(triCount, spikeTris);
        info.triCount = triCount;
        spikeVBOs.push_back(info);

        free(spikeTris);
    }
    
    // 5. Processar Porta (Opcional, se a porta for um cubo)
    // Se você tiver uma variável global VBO_Info doorVBO_Info:
    /*
    if (doorVBO_Info.vboID != 0) glDeleteBuffers(1, &doorVBO_Info.vboID);
    int dCount = 0;
    tri* doorTris = createBoxTris(door_3D.x, door_3D.y, door_3D.z, door_3D.w, door_3D.h, door_3D.d, 0.0f, 1.0f, 0.0f, dCount);
    doorVBO_Info.vboID = criaVBODeTris(dCount, doorTris);
    doorVBO_Info.triCount = dCount;
    free(doorTris);
    */
}

// =================================================================================
// IMPLEMENTAÇÃO DA API game_3D.h
// =================================================================================

/**
 * Chamada quando o nível 3D é iniciado.
 * Estrutura adaptada para o uso de VBOs e múltiplos níveis.
 */
void gameStartLevel_3D(int level) {
    printf("gameStartLevel_3D() chamado para o nivel %d.\n", level);

    // --- Configuração do OpenGL ---
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.1f, 0.1f, 0.3f, 1.0f); // Fundo azul escuro

    // --- Inicialização do Player (GameObject_3D) ---
    // Reseta velocidades
    player_3D.velocityX = 0.0f;
    player_3D.velocityY = 0.0f;
    player_3D.velocityZ = 0.0f;
    
    // Define dimensões do jogador (Caixa de colisão)
    player_3D.w = 1.0f; // Largura
    player_3D.h = 2.0f; // Altura
    player_3D.d = 1.0f; // Profundidade

    // --- Resetar Controles e Câmera ---
    isPressingForward_3D = false;
    isPressingBackward_3D = false;
    isPressingLeft_3D = false;
    isPressingRight_3D = false;

    cameraYaw_3D = -90.0f; // Olhando para -Z (Frente padrão)
    cameraPitch_3D = 0.0f; // Horizonte

    // --- Resetar variáveis de estado ---
    lastVelocityMag_3D = 0.0f;
    currentAcceleration_3D = 0.0f;
    
    // Forças
    forceNormalX_3D = 0; forceNormalY_3D = 0; forceNormalZ_3D = 0;
    forceFrictionX_3D = 0; forceFrictionY_3D = 0; forceFrictionZ_3D = 0;
    forceTensionX_3D = 0; forceTensionY_3D = 0; forceTensionZ_3D = 0;
    
    isGrounded_3D = true;
    isHooked_3D = false;
    isHookFiring_3D = false;
    isChargingHook_3D = false;
    
    // Limpa as geometrias do nível anterior da memória RAM
    platforms_3D.clear();
    breakableWalls_3D.clear();
    windZones_3D.clear();
    spikeZones_3D.clear();
    // NOTA: A limpeza dos VBOs da GPU (glDeleteBuffers) será feita 
    // automaticamente no início da função createLevelVBOs().

    // --- Mouse setup ---
    // (Certifique-se que você tem acesso às funções glut aqui)
    glutSetCursor(GLUT_CURSOR_NONE);
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    gameReshape_3D(w, h); // Se essa função existir, ok. Senão, o próprio GLUT cuida do reshape.
    
    // Centraliza o mouse para o modo FPS
    // (Assumindo que windowWidth/Height são globais ou pegos via glutGet)
    glutWarpPointer(w / 2, h / 2); 
    
    // Variável local ou global para evitar "pulo" da câmera no primeiro frame
    // mouseInitialized = false; 

    // --- Definição do Nível ---

    // Chão Padrão (Usado em vários cases) - Ajuste conforme necessidade
    float floorY = 0.0f; 

    switch (level) {
        case 1:
            // --- Parâmetros de Física ---
            levelParameters_3D.playerMass = 10.0f;
            levelParameters_3D.playerWalkAccel = 60.0f;
            levelParameters_3D.maxWalkSpeed = 15.0f;
            levelParameters_3D.maxPlayerSpeed = 30.0f; // Adicionado valor padrão
            levelParameters_3D.vectorVisualScale = 0.5f;
            // floorFrictionCoefficient removido pois agora é propriedade de CADA plataforma (Platform_3D)

            // --- Carga de Geometria 3D Nível 1 ---

            // 1. Chão Base (Plataforma grande cinza)
            // x, y, z, w, h, d, r, g, b, isHookable, friction
            platforms_3D.push_back({
                -WORLD_WIDTH_3D/2, -1.0f, -WORLD_DEPTH_3D/2,  // Posição (x, y, z)
                WORLD_WIDTH_3D, 1.0f, WORLD_DEPTH_3D,         // Tamanho (w, h, d)
                0.4f, 0.4f, 0.4f,                             // Cor (Cinza)
                true, 0.8f                                    // Hookable, Atrito
            });

            // 2. Plataforma Suspensa 1
            platforms_3D.push_back({
                -20.0f, 5.0f, 10.0f, 
                10.0f, 1.0f, 20.0f, 
                0.4f, 0.4f, 0.4f, 
                true, 0.2f // Baixo atrito
            });
            
            // 3. Parede Quebrável
            // x, y, z, w, h, d, r, g, b, strength, isBroken
            breakableWalls_3D.push_back({
                10.0f, 1.0f, 0.0f, 
                1.0f, 5.0f, 10.0f, 
                0.6f, 0.3f, 0.0f, // Cor marrom
                50.0f, false      // Força, estado
            });

            // 4. Zona de Vento
            // x, y, z, w, h, d, accelX, accelY, accelZ
            windZones_3D.push_back({
                -30.0f, 1.0f, 10.0f, 
                5.0f, 5.0f, 5.0f, 
                0.0f, 0.0f, 5.0f // Empurra em Z
            });

            // 5. Porta (GameObject_3D)
            door_3D.x = 40.0f; door_3D.y = 1.0f; door_3D.z = 0.0f;
            door_3D.w = 1.0f;  door_3D.h = 4.0f; door_3D.d = 1.0f;
            door_3D.r = 0.5f;  door_3D.g = 1.0f; door_3D.b = 0.5f; // Verde

            // 6. Posição Inicial do Jogador
            player_3D.x = 0.0f;
            player_3D.y = 3.0f;
            player_3D.z = 0.0f;

            break;

        case 2:
            // --- Parâmetros de Física ---
            levelParameters_3D.playerMass = 8.0f;
            levelParameters_3D.playerWalkAccel = 70.0f;
            levelParameters_3D.maxWalkSpeed = 18.0f;

            // --- Carga de Geometria 3D Nível 2 ---
            
            // 1. Chão (Cor azulada, menor atrito global se fosse o caso, mas aqui definimos por plataforma)
            platforms_3D.push_back({
                -WORLD_WIDTH_3D/2, -1.0f, -WORLD_DEPTH_3D/2,
                WORLD_WIDTH_3D, 1.0f, WORLD_DEPTH_3D,
                0.2f, 0.2f, 0.5f, 
                true, 0.7f
            });

            // 2. Plataforma Suspensa 2
            platforms_3D.push_back({
                20.0f, 8.0f, 0.0f, 
                8.0f, 1.0f, 8.0f, 
                0.5f, 0.5f, 0.5f, 
                true, 0.1f
            });
            
            // 3. Zona de Espinhos (SpikeZone_3D)
            // x, y, z, w, h, d, r, g, b
            spikeZones_3D.push_back({
                5.0f, 0.0f, -5.0f, 
                10.0f, 1.0f, 10.0f, 
                1.0f, 0.0f, 0.0f // Vermelho
            });

            // 4. Porta
            door_3D.x = 50.0f; door_3D.y = 1.0f; door_3D.z = 0.0f;
            door_3D.w = 1.0f;  door_3D.h = 4.0f; door_3D.d = 1.0f;
            door_3D.r = 0.5f;  door_3D.g = 1.0f; door_3D.b = 0.5f;

            // 5. Player
            player_3D.x = 0.0f; player_3D.y = 3.0f; player_3D.z = 0.0f;
            break;

        default:
            gameStartLevel_3D(1); // Recursão para fallback
            return;
    }

    // --- Ações Finais ---
    // Encontrar plataforma inicial (colisão simples)
    // Se o player começa no chão, podemos setar collidingPlatform_3D para o chão (índice 0)
    if (!platforms_3D.empty()) {
        collidingPlatform_3D = &platforms_3D[0];
    } else {
        collidingPlatform_3D = NULL;
    }

    // NOVO: Chama a rotina que cria os VBOs para toda a geometria estática do nível
    createLevelVBOs();
}

/**
 * Loop principal de física e lógica do jogo.
 */
GameAction gameUpdate_3D() {
    // --- 1. Controle de Vitória e Derrota ---
    if (isGameVictory_3D) {
        gameVictoryTimer_3D--;
        if (gameVictoryTimer_3D <= 0) {
            isGameVictory_3D = false;
            return GAME_ACTION_LEVEL_WON;
        }
        return GAME_ACTION_CONTINUE;
    }

    if (isGameOver_3D) {
        gameOverTimer_3D--;
        if (gameOverTimer_3D <= 0) {
            isGameOver_3D = false;
            return GAME_ACTION_LEVEL_LOST;
        }
        return GAME_ACTION_CONTINUE;
    }

    // --- 2. Cálculo do Delta Time (Essencial para Física) ---
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (float)(currentTime - lastTime_3D) / 1000.0f;
    lastTime_3D = currentTime;
    
    // Proteção contra "pulos" de tempo muito grandes (lag)
    if (deltaTime > 0.1f) deltaTime = 0.1f; 

    // --- 3. Inicialização de Forças ---
    float prevPositionPlayerX = player_3D.x;
    float prevPositionPlayerY = player_3D.y;
    float prevPositionPlayerZ = player_3D.z;

    forceNormalX_3D = 0; forceNormalY_3D = 0; forceNormalZ_3D = 0;
    forceFrictionX_3D = 0; forceFrictionY_3D = 0; forceFrictionZ_3D = 0;
    forceTensionX_3D = 0; forceTensionY_3D = 0; forceTensionZ_3D = 0;

    // Gravidade aplica aceleração em Y negativo
    float accelX = 0, accelY = -levelParameters_3D.gravity, accelZ = 0;


    // --- 4. Zonas de Vento (3D) ---
    float pCx = player_3D.x + player_3D.w / 2.0f;
    float pCy = player_3D.y + player_3D.h / 2.0f;
    float pCz = player_3D.z + player_3D.d / 2.0f;

    for (const auto& wind : windZones_3D) {
        if (isPointInsideBox(pCx, pCy, pCz, wind.x, wind.y, wind.z, wind.w, wind.h, wind.d)) {
            accelX += wind.accelX;
            accelY += wind.accelY;
            accelZ += wind.accelZ;
        }
    }


    // --- 5. Física do Gancho (Hook Physics) ---
    if (isHooked_3D) {
        float vecToHookX = hookPointX_3D - pCx;
        float vecToHookY = hookPointY_3D - pCy;
        float vecToHookZ = hookPointZ_3D - pCz;
        
        float dist = sqrt(vecToHookX*vecToHookX + vecToHookY*vecToHookY + vecToHookZ*vecToHookZ);
        
        if (dist > 0.01f) {
            // Normaliza o vetor direção
            float normX = vecToHookX / dist;
            float normY = vecToHookY / dist;
            float normZ = vecToHookZ / dist;

            // Aplica a força de puxada na direção do gancho
            accelX += normX * currentPullForce_3D;
            accelY += normY * currentPullForce_3D;
            accelZ += normZ * currentPullForce_3D;

            // Salva para visualização
            forceTensionX_3D = normX * currentPullForce_3D;
            forceTensionY_3D = normY * currentPullForce_3D;
            forceTensionZ_3D = normZ * currentPullForce_3D;
        }
    }


    // --- 6. Movimentação do Personagem (WASD relativo à Câmera) ---
    if (isGrounded_3D) {
        // Precisamos saber para onde a câmera aponta no plano horizontal (XZ)
        // para mover o personagem corretamente
        float fwdX = cameraFrontX_3D;
        float fwdZ = cameraFrontZ_3D;
        
        // Normaliza no plano XZ (evita andar devagar se olhar pra baixo/cima)
        float len = sqrt(fwdX*fwdX + fwdZ*fwdZ);
        if (len > 0.01f) { fwdX /= len; fwdZ /= len; }

        // Vetor "Direita" da câmera (Produto vetorial com Y-UP)
        // Right = Front x Up(0,1,0) => (z, 0, -x)
        float rightX = -fwdZ;
        float rightZ = fwdX;

        float walkForce = levelParameters_3D.playerWalkAccel;

        if (isPressingForward_3D) {
            accelX += fwdX * walkForce;
            accelZ += fwdZ * walkForce;
        }
        if (isPressingBackward_3D) {
            accelX -= fwdX * walkForce;
            accelZ -= fwdZ * walkForce;
        }
        if (isPressingLeft_3D) {
            accelX -= rightX * walkForce; // Esquerda
            accelZ -= rightZ * walkForce;
        }
        if (isPressingRight_3D) {
            accelX += rightX * walkForce; // Direita
            accelZ += rightZ * walkForce;
        }

        // Atrito com a plataforma
        if (collidingPlatform_3D) {
            // Normal no chão plano é a gravidade invertida
            float fNormal = levelParameters_3D.gravity * levelParameters_3D.playerMass; // F = m*a
            
            // Visualização (apenas magnitude em Y)
            forceNormalY_3D = fNormal;

            float frictionCoef = collidingPlatform_3D->frictionCoefficient;
            // A força de atrito máxima disponível: Fat = u * N
            float maxFrictionForce = frictionCoef * fNormal;
            // Aceleração de atrito disponível: a_atrito = Fat / m
            float frictionAccelMax = maxFrictionForce / levelParameters_3D.playerMass;

            // Velocidade atual no plano XZ
            float velXZ = sqrt(player_3D.velocityX*player_3D.velocityX + player_3D.velocityZ*player_3D.velocityZ);
            
            if (velXZ > 0.001f) {
                // Direção oposta à velocidade
                float dirX = -(player_3D.velocityX / velXZ);
                float dirZ = -(player_3D.velocityZ / velXZ);

                // Se a velocidade for muito baixa, paramos completamente (evita tremedeira)
                if (velXZ < frictionAccelMax * deltaTime) {
                    player_3D.velocityX = 0;
                    player_3D.velocityZ = 0;
                    accelX = 0; // Cancela inputs se estiver parando
                    accelZ = 0;
                } else {
                    // Aplica desaceleração
                    accelX += dirX * frictionAccelMax;
                    accelZ += dirZ * frictionAccelMax;
                    
                    // Visualização
                    forceFrictionX_3D = dirX * maxFrictionForce;
                    forceFrictionZ_3D = dirZ * maxFrictionForce;
                }
            }
        }
    }

    // --- 7. Lógica de Carregamento e Disparo (Input Mouse) ---
    // (Verificação da flag isChargingHook_3D que é setada no MouseClickFunc)
    if (isChargingHook_3D) {
        float chargeTime = (glutGet(GLUT_ELAPSED_TIME) - chargeStartTime_3D) / 1000.0f;
        chargePercentage_3D = chargeTime / MAX_CHARGE_TIME_3D;
        if (chargePercentage_3D > 1.0f) chargePercentage_3D = 1.0f;
        
        currentChargeForce_3D = chargePercentage_3D * MAX_AIM_FORCE_DISPLAY_3D;
    }

    // Se houve soltura do gancho (requestHookRelease_3D setado no MouseFunc)
    if (requestHookRelease_3D) {
        if (isHooked_3D) {
            // Solta a corda
            isHooked_3D = false;
        } else if (isChargingHook_3D) {
            // Dispara!
            isChargingHook_3D = false;
            isHookFiring_3D = true;
            
            // Origem: Câmera ou Arma
            hookProjectileX_3D = cameraPosX_3D;
            hookProjectileY_3D = cameraPosY_3D;
            hookProjectileZ_3D = cameraPosZ_3D;
            
            // Direção: Onde a câmera está olhando
            float speed = levelParameters_3D.hookSpeed;
            hookProjectileVelX_3D = cameraFrontX_3D * speed;
            hookProjectileVelY_3D = cameraFrontY_3D * speed;
            hookProjectileVelZ_3D = cameraFrontZ_3D * speed;
            
            // A força de puxada será baseada na carga
            currentPullForce_3D = currentChargeForce_3D;
        }
        requestHookRelease_3D = false; // Reseta flag
    }


    // --- 8. Integração de Euler (Física Básica) ---
    // v = v0 + a * t
    player_3D.velocityX += accelX * deltaTime;
    player_3D.velocityY += accelY * deltaTime;
    player_3D.velocityZ += accelZ * deltaTime;

    // Limite de Velocidade no chão (apenas XZ)
    if (isGrounded_3D) {
        float velXZ = sqrt(player_3D.velocityX*player_3D.velocityX + player_3D.velocityZ*player_3D.velocityZ);
        if (velXZ > levelParameters_3D.maxWalkSpeed) {
            float scale = levelParameters_3D.maxWalkSpeed / velXZ;
            player_3D.velocityX *= scale;
            player_3D.velocityZ *= scale;
        }
    }

    // Limite Global de velocidade (Segurança para visualização)
    float totalSpeed = sqrt(player_3D.velocityX*player_3D.velocityX + 
                            player_3D.velocityY*player_3D.velocityY + 
                            player_3D.velocityZ*player_3D.velocityZ);
    if (totalSpeed > levelParameters_3D.maxPlayerSpeed) {
        float scale = levelParameters_3D.maxPlayerSpeed / totalSpeed;
        player_3D.velocityX *= scale;
        player_3D.velocityY *= scale;
        player_3D.velocityZ *= scale;
    }

    // x = x0 + v * t
    player_3D.x += player_3D.velocityX * deltaTime;
    player_3D.y += player_3D.velocityY * deltaTime;
    player_3D.z += player_3D.velocityZ * deltaTime;


    // --- 9. Colisões com Limites de Mundo ---
    if (player_3D.y < DEATH_BOUNDARY_Y_3D) {
        isGameOver_3D = true;
        gameOverTimer_3D = 180;
        return GAME_ACTION_CONTINUE;
    }
    // Opcional: Limites X e Z do mundo
    if (player_3D.x < -WORLD_WIDTH_3D || player_3D.x > WORLD_WIDTH_3D ||
        player_3D.z < -WORLD_DEPTH_3D || player_3D.z > WORLD_DEPTH_3D) {
        // isGameOver_3D = true; // Se quiser matar ao sair do mapa lateralmente
    }


    // --- 10. Colisões com Zonas de Perigo (Box vs Box) ---
    // Espinhos
    for (const auto& spike : spikeZones_3D) {
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               spike.x, spike.y, spike.z, spike.w, spike.h, spike.d)) {
            isGameOver_3D = true;
            gameOverTimer_3D = 180;
            return GAME_ACTION_CONTINUE;
        }
    }

    // --- 11. Colisões com Paredes Quebráveis ---
    for (auto& wall : breakableWalls_3D) {
        if (wall.isBroken) continue;
        
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               wall.x, wall.y, wall.z, wall.w, wall.h, wall.d)) {
            
            float impactSpeed = totalSpeed; // Simplificado
            float impactForce = levelParameters_3D.playerMass * impactSpeed;

            if (impactForce >= wall.strength) {
                wall.isBroken = true;
                player_3D.velocityX *= 0.7f; // Perde energia
                player_3D.velocityY *= 0.7f;
                player_3D.velocityZ *= 0.7f;
            } else {
                // Colisão simples AABB resolve voltando para posição anterior
                player_3D.x = prevPositionPlayerX;
                player_3D.y = prevPositionPlayerY;
                player_3D.z = prevPositionPlayerZ;
                player_3D.velocityX = 0;
                // player_3D.velocityY = 0; // Geralmente mantemos Y se for parede vertical
                player_3D.velocityZ = 0;
            }
        }
    }


    // --- 12. Colisão com Plataformas (Chão/Teto) ---
    isGrounded_3D = false;
    collidingPlatform_3D = NULL;

    for (auto& p : platforms_3D) {
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               p.x, p.y, p.z, p.w, p.h, p.d)) {
            
            // Detecta colisão vindo de CIMA (Pouso)
            // Estava acima antes? E agora colidiu? E estava caindo?
            if (player_3D.velocityY <= 0 && 
                prevPositionPlayerY >= p.y + p.h - 0.1f) { // Tolerância
                
                player_3D.y = p.y + p.h; // Corrige posição para o topo
                player_3D.velocityY = 0;
                isGrounded_3D = true;
                collidingPlatform_3D = &p;
                
                if (isHooked_3D) isHooked_3D = false; // Solta o gancho ao pousar?
            }
            // Detecta colisão vindo de BAIXO (Bateu cabeça)
            else if (player_3D.velocityY > 0 && 
                     prevPositionPlayerY + player_3D.h <= p.y + 0.1f) {
                
                player_3D.y = p.y - player_3D.h;
                player_3D.velocityY = 0;
            }
            // Colisão lateral (simples)
            else {
                 // Para evitar atravessar paredes laterais de plataformas, 
                 // idealmente resetaríamos X/Z para prevPosition
                 // player_3D.x = prevPositionPlayerX;
                 // player_3D.z = prevPositionPlayerZ;
            }
        }
    }


    // --- 13. Restrição de Corda (Hook Physics - Constraints) ---
    if (isHooked_3D) {
        float vecX = hookPointX_3D - pCx;
        float vecY = hookPointY_3D - pCy;
        float vecZ = hookPointZ_3D - pCz;
        float currDist = sqrt(vecX*vecX + vecY*vecY + vecZ*vecZ);

        if (currDist > ropeLength_3D && currDist > 0.01f) {
            float stretch = currDist - ropeLength_3D;
            float nX = vecX / currDist;
            float nY = vecY / currDist;
            float nZ = vecZ / currDist;

            // Corrige posição (Constraint posicional)
            player_3D.x += nX * stretch;
            player_3D.y += nY * stretch;
            player_3D.z += nZ * stretch;

            // Corrige velocidade (Constraint de velocidade - remove componente na direção da corda)
            // Dot Product (Vel . Normal)
            float velAlongRope = player_3D.velocityX * nX + player_3D.velocityY * nY + player_3D.velocityZ * nZ;
            
            if (velAlongRope < 0) { // Se está se afastando
                player_3D.velocityX -= nX * velAlongRope;
                player_3D.velocityY -= nY * velAlongRope;
                player_3D.velocityZ -= nZ * velAlongRope;
            }
        }
        
        // Amortecimento no balanço
        player_3D.velocityX *= levelParameters_3D.dampingFactor;
        player_3D.velocityY *= levelParameters_3D.dampingFactor;
        player_3D.velocityZ *= levelParameters_3D.dampingFactor;
    }


    // --- 14. Lógica do Projétil do Gancho (Hook Moving) ---
    if (isHookFiring_3D) {
        float prevHX = hookProjectileX_3D;
        float prevHY = hookProjectileY_3D;
        float prevHZ = hookProjectileZ_3D;

        hookProjectileX_3D += hookProjectileVelX_3D;
        hookProjectileY_3D += hookProjectileVelY_3D;
        hookProjectileZ_3D += hookProjectileVelZ_3D;

        bool hit = false;
        for (auto& p : platforms_3D) {
            float hX, hY, hZ;
            if (p.isHookable && 
                lineBoxIntersection(prevHX, prevHY, prevHZ, 
                                    hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D, 
                                    p, hX, hY, hZ)) {
                
                isHookFiring_3D = false;
                isHooked_3D = true;
                hookPointX_3D = hX;
                hookPointY_3D = hY;
                hookPointZ_3D = hZ;

                // Calcula comprimento da corda
                float dx = hookPointX_3D - (player_3D.x + player_3D.w/2);
                float dy = hookPointY_3D - (player_3D.y + player_3D.h/2);
                float dz = hookPointZ_3D - (player_3D.z + player_3D.d/2);
                ropeLength_3D = sqrt(dx*dx + dy*dy + dz*dz);
                
                isGrounded_3D = false;
                hit = true;
                break;
            }
        }

        // Limite de distância visual
        if (!hit) {
            float distOrigin = sqrt(pow(hookProjectileX_3D - cameraPosX_3D, 2) + 
                                    pow(hookProjectileY_3D - cameraPosY_3D, 2) +
                                    pow(hookProjectileZ_3D - cameraPosZ_3D, 2));
                                    
            if (distOrigin > VIEW_WIDTH_3D) { // Usando View Width como limite arbitrário
                isHookFiring_3D = false;
            }
        }
    }


    // --- 15. Verificação de Vitória ---
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                           door_3D.x, door_3D.y, door_3D.z, door_3D.w, door_3D.h, door_3D.d)) {
        isGameVictory_3D = true;
        gameVictoryTimer_3D = 180;
        return GAME_ACTION_CONTINUE;
    }

    return GAME_ACTION_CONTINUE;
}

/**
 * Loop principal de renderização.
 */
void gameDisplay_3D() {
    // 1. Limpeza de Buffers
    // Substitui glClearColor e glClear do 2D
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Configuração de Câmera (Substitui a lógica de cameraLeft/glOrtho)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Lógica FPS: A câmera segue o player
    cameraPosX_3D = player_3D.x;
    cameraPosY_3D = player_3D.y + (player_3D.h * 0.9f); // Altura dos olhos
    cameraPosZ_3D = player_3D.z; // Player pode estar em Z variado ou fixo

    gluLookAt(cameraPosX_3D, cameraPosY_3D, cameraPosZ_3D,
              cameraPosX_3D + cameraFrontX_3D, cameraPosY_3D + cameraFrontY_3D, cameraPosZ_3D + cameraFrontZ_3D,
              0.0f, 1.0f, 0.0f);

    // Configuração de Luz Básica
    GLfloat light_pos[] = {0.0f, 200.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // 3. Desenho do Mundo (Substitui os loops de platforms, walls, spikes do 2D)
    // Aqui usamos os VBOs que geramos na createLevelVBOs
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    int stride = sizeof(vert);

    auto drawVBOList = [&](const std::vector<VBO_Info>& list) {
        for (const auto& info : list) {
            if (info.vboID == 0) continue;
            glBindBuffer(GL_ARRAY_BUFFER, info.vboID);
            glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(vert, pos));
            glColorPointer(4, GL_FLOAT, stride, (void*)offsetof(vert, cor));
            glNormalPointer(GL_FLOAT, stride, (void*)offsetof(vert, normal));
            glDrawArrays(GL_TRIANGLES, 0, info.triCount * 3);
        }
    };

    drawVBOList(platformVBOs);
    drawVBOList(wallVBOs); // Paredes quebráveis
    drawVBOList(spikeVBOs);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    // 4. Desenho de Objetos Dinâmicos (Porta e Player)
    
    // Porta
    glPushMatrix();
        glTranslatef(door_3D.x, door_3D.y, door_3D.z);
        // Ajuste de pivô se necessário (assumindo canto inferior esquerdo)
        glTranslatef(door_3D.w/2, door_3D.h/2, door_3D.d/2); 
        glScalef(door_3D.w, door_3D.h, door_3D.d);
        glColor3f(0.0f, 1.0f, 0.0f); // Verde (substitui textura)
        glutSolidCube(1.0);
    glPopMatrix();

    // Player (Substitui toda a lógica de sprites/animação por um cubo vermelho)
    if (!isGameOver_3D) {
        glPushMatrix();
            glTranslatef(player_3D.x, player_3D.y, player_3D.z);
            glTranslatef(player_3D.w/2, player_3D.h/2, player_3D.d/2);
            glScalef(player_3D.w, player_3D.h, player_3D.d);
            glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
            glutSolidCube(1.0);
        glPopMatrix();
    }

    // 5. Desenho da Corda (Lógica idêntica ao 2D, adaptada para Z)
    if (isHooked_3D || isHookFiring_3D) {
        glDisable(GL_LIGHTING); 
        glLineWidth(2.0f);
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES);
            // Ponta A: Player (Centro)
            glVertex3f(player_3D.x + player_3D.w/2, 
                       player_3D.y + player_3D.h/2, 
                       player_3D.z + player_3D.d/2);
            
            // Ponta B: Gancho ou Projétil
            if (isHookFiring_3D) {
                glVertex3f(hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D);
            } else {
                glVertex3f(hookPointX_3D, hookPointY_3D, hookPointZ_3D);
            }
        glEnd();
        glEnable(GL_LIGHTING);
    }

    // 6. Desenho dos Vetores Físicos (Lógica Educacional)
    // Usamos Vector_3D e a função drawVector_3D (que desenha linhas e cones)
    
    Vector_3D playerCenter = {
        player_3D.x + player_3D.w/2,
        player_3D.y + player_3D.h/2,
        player_3D.z + player_3D.d/2
    };

    // Vetor Peso (P)
    Vector_3D forceP = {0, levelParameters_3D.gravity * levelParameters_3D.playerMass, 0};
    drawVector_3D(playerCenter, forceP, levelParameters_3D.vectorVisualScale, 0.2f, 0.2f, 1.0f, "P");

    // Vetor Velocidade (V)
    float velMag = sqrt(player_3D.velocityX*player_3D.velocityX + 
                        player_3D.velocityY*player_3D.velocityY + 
                        player_3D.velocityZ*player_3D.velocityZ);
    if (velMag > 0.01f) {
        Vector_3D vel = {player_3D.velocityX, player_3D.velocityY, player_3D.velocityZ};
        drawVector_3D(playerCenter, vel, VELOCITY_VISUAL_SCALE_3D, 1.0f, 0.5f, 0.0f, "V");
    }

    // Vetor Tensão (T) - Se estiver preso
    if (isHooked_3D) {
        Vector_3D tens = {forceTensionX_3D, forceTensionY_3D, forceTensionZ_3D};
        drawVector_3D(playerCenter, tens, levelParameters_3D.vectorVisualScale, 1.0f, 0.0f, 1.0f, "T");
    }

    // Vetor Normal (N) e Atrito (Fat) - Se estiver no chão
    if (isGrounded_3D) {
        // Normal parte do pé do personagem
        Vector_3D feetPos = {playerCenter.x, playerCenter.y - player_3D.h/2, playerCenter.z};
        
        Vector_3D norm = {forceNormalX_3D, forceNormalY_3D, forceNormalZ_3D};
        drawVector_3D(feetPos, norm, levelParameters_3D.vectorVisualScale, 0.0f, 1.0f, 1.0f, "N");

        float fricMag = sqrt(forceFrictionX_3D*forceFrictionX_3D + 
                             forceFrictionY_3D*forceFrictionY_3D + 
                             forceFrictionZ_3D*forceFrictionZ_3D);
        
        if (fricMag > 0.001f) {
            // Atrito desenhado um pouco acima do chão para não sobrepor
            Vector_3D fricPos = {feetPos.x, feetPos.y + 0.2f, feetPos.z};
            Vector_3D fric = {forceFrictionX_3D, forceFrictionY_3D, forceFrictionZ_3D};
            drawVector_3D(fricPos, fric, levelParameters_3D.vectorVisualScale, 1.0f, 0.0f, 0.0f, "Fat");
        }
    }

    // 7. HUD e Interface 2D (Aiming, Game Over, Victory)
    // Mudamos para 2D para desenhar na tela
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // --- Lógica de Mira (Substitui o vetor do mouse 2D) ---
    // Em FPS, a mira é o centro (Crosshair) e a força é uma barra
    
    // Desenha Mira (Cruz no centro)
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex2f(cx - 10, cy); glVertex2f(cx + 10, cy);
        glVertex2f(cx, cy - 10); glVertex2f(cx, cy + 10);
    glEnd();

    // Desenha Barra de Força (Se estiver carregando o gancho)
    // Substitui: Aim Vector visualização
    if (isChargingHook_3D) {
        float barW = 200.0f;
        float barH = 15.0f;
        // Desenha fundo
        glColor3f(0.3f, 0.3f, 0.3f);
        glRectf(cx - barW/2, cy - 40, cx + barW/2, cy - 40 + barH);
        
        // Desenha carga (Amarelo -> Vermelho)
        glColor3f(1.0f, 1.0f - chargePercentage_3D, 0.0f);
        glRectf(cx - barW/2, cy - 40, cx - barW/2 + (barW * chargePercentage_3D), cy - 40 + barH);
        
        // Texto de Força (Opcional, lógica do sprintf mantida)
        /*
        char magText[50];
        sprintf(magText, "Forca: %.0f N", currentChargeForce_3D);
        drawText(cx - 20, cy - 60, magText); // Assumindo drawText 2D
        */
    }

    // Telas de Fim de Jogo
    if (isGameVictory_3D) {
        // Chame sua função de desenho de texto ou tela de vitória aqui
        // drawGameVictoryScreen(); 
    } else if (isGameOver_3D) {
        // drawGameOverScreen();
    }

    // Restaura Matrizes 3D
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
}

/**
 * Chamada quando a janela é redimensionada.
 */
void gameReshape_3D(int w, int h) {
    // Previne divisão por zero se a janela for minimizada ou muito pequena
    if (h == 0) h = 1;

    // Atualiza as variáveis globais (importante para o mouse/UI em outras funções)
    // Certifique-se que windowWidth/Height são globais acessíveis
    // windowWidth = w; 
    // windowHeight = h;

    float ratio = 1.0f * w / h;

    // 1. Configura a Viewport (Área de desenho em pixels)
    glViewport(0, 0, w, h);

    // 2. Configura a Matriz de Projeção (A Lente da Câmera)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // gluPerspective(fovy, aspect, zNear, zFar)
    // fovy: 60.0f -> Um ângulo um pouco mais aberto é melhor para 3ª pessoa (vê mais ao redor).
    // zNear: 0.1f -> Mantemos bem perto para não "cortar" o personagem se a câmera chegar muito perto.
    // zFar: 5000.0f -> Aumentado para cobrir seu WORLD_WIDTH_3D (3000.0).
    gluPerspective(60.0f, ratio, 0.1f, 5000.0f);

    // 3. Volta para ModelView (Onde movemos objetos e câmera)
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Chamada quando o mouse se move (ativo ou passivo).
 */
void gameMouseMotion_3D(int x, int y) {
    // 1. Verifica se o mouse deve controlar a câmera
    // (Assumindo que você tem essa flag global de controle de menu)
    if (isMouseFree_3D) return;

    // 2. Obtém dimensões atuais da janela (mais seguro que variáveis globais antigas)
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    int centerX = w / 2;
    int centerY = h / 2;
    
    // 3. Inicialização (Evita "pulo" da câmera no primeiro frame)
    if (!mouseInitialized_3D) {
        mouseInitialized_3D = true;
        glutWarpPointer(centerX, centerY);
        return;
    }

    // 4. Calcula o deslocamento (Delta)
    // Nota: O eixo Y da tela é invertido (0 é topo), por isso centerY - y
    float xoffset = (float)(x - centerX);
    float yoffset = (float)(centerY - y); 

    // Se não houve movimento, retorna para economizar processamento
    if (xoffset == 0 && yoffset == 0) return;

    // 5. Aplica Sensibilidade
    xoffset *= MOUSE_SENSITIVITY_3D;
    yoffset *= MOUSE_SENSITIVITY_3D;

    // 6. Atualiza os Ângulos (Euler Angles)
    // += é o padrão. Se sentir que está invertido (mouse direita vira esquerda), mude para -=
    cameraYaw_3D   += xoffset;
    cameraPitch_3D += yoffset;

    // 7. Aplica Limites (Constraints)
    // Pitch: Impede que a câmera dê "cambalhota" (Gimbal Lock)
    if (cameraPitch_3D > MAX_PITCH_3D)  cameraPitch_3D = MAX_PITCH_3D;
    if (cameraPitch_3D < MIN_PITCH_3D)  cameraPitch_3D = MIN_PITCH_3D;

    // 8. CRUCIAL: Atualiza o Vetor de Direção (Camera Front)
    // [Image of spherical coordinates to cartesian vector conversion diagram]

    // Converte de Coordenadas Esféricas (Ângulos) para Cartesianas (Vetor X,Y,Z)
    
    float radYaw = cameraYaw_3D * (M_PI / 180.0f);
    float radPitch = cameraPitch_3D * (M_PI / 180.0f);

    Vector_3D direction;
    direction.x = cos(radYaw) * cos(radPitch);
    direction.y = sin(radPitch);
    direction.z = sin(radYaw) * cos(radPitch);

    // Normaliza o vetor (garante que o tamanho seja 1.0)
    Vector_3D front = direction.normalized(); // Usa a função da sua struct Vector_3D

    // Atualiza as variáveis globais que o gameDisplay_3D usa
    cameraFrontX_3D = front.x;
    cameraFrontY_3D = front.y;
    cameraFrontZ_3D = front.z;

    // 9. Trava o mouse no centro novamente (Loop infinito de movimento)
    glutWarpPointer(centerX, centerY);
}

/**
 * Chamada quando um botão do mouse é clicado.
 */
void gameMouseClick_3D(int button, int state) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        printf("Disparar gancho 3D!\n");
        // Lógica do gancho 3D
    }
}

/**
 * Chamada quando uma tecla é pressionada.
 */
GameAction gameKeyDown_3D(unsigned char key, int x, int y) {
    switch (key) {
        // --- Movimentação (WASD) ---
        case 'w': case 'W': 
            isPressingForward_3D = true; 
            break;
        case 'a': case 'A': 
            isPressingLeft_3D = true; 
            break;
        case 's': case 'S': 
            isPressingBackward_3D = true; 
            break;
        case 'd': case 'D': 
            isPressingRight_3D = true; 
            break;
        
        // --- Lógica para alternar o modo do Mouse ('M' ou TAB/ESC se preferir) ---
        case 'm': case 'M': 
            isMouseFree_3D = !isMouseFree_3D; // Inverte o estado (Variável com sufixo _3D)
            
            if (isMouseFree_3D) {
                glutSetCursor(GLUT_CURSOR_LEFT_ARROW); // Mostra o mouse para clicar em menus
            } else {
                glutSetCursor(GLUT_CURSOR_NONE); // Esconde o mouse para jogar
                
                // Centraliza para evitar "pulo" da câmera ao voltar
                int w = glutGet(GLUT_WINDOW_WIDTH);
                int h = glutGet(GLUT_WINDOW_HEIGHT);
                glutWarpPointer(w / 2, h / 2);
                
                mouseInitialized_3D = false; // Reseta a lógica de inicialização do mouse
            }
            break;

        // --- Sair para o Menu ---
        case 'q': case 'Q':
            // --- LIMPEZA DO ESTADO OPENGL ANTES DE SAIR ---
            // Isso garante que o menu 2D não seja afetado pelas luzes/profundidade do 3D
            glDisable(GL_LIGHTING); 
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_COLOR_MATERIAL);
            
            // Reseta a cor atual para branco puro
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f); 
            
            // Garante que o cursor apareça no menu
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            isMouseFree_3D = true; // Libera o mouse logicamente
            
            return GAME_ACTION_EXIT_TO_MENU;
    }
    return GAME_ACTION_CONTINUE;
}

/**
 * Chamada quando uma tecla é solta.
 */
void gameKeyUp_3D(unsigned char key, int x, int y) {
    switch (key) {
        // Quando solta a tecla, paramos o movimento naquela direção
        case 'w': case 'W': 
            isPressingForward_3D = false; 
            break;
        case 'a': case 'A': 
            isPressingLeft_3D = false; 
            break;
        case 's': case 'S': 
            isPressingBackward_3D = false; 
            break;
        case 'd': case 'D': 
            isPressingRight_3D = false; 
            break;
    }
}

// Funções de Teclas Especiais (não usadas aqui)
void gameSpecialDown_3D(int key, int x, int y) {}
void gameSpecialUp_3D(int key, int x, int y) {}

/**
 * Carrega texturas 3D (vazio por enquanto).
 */
void loadGameTextures_3D() {
    printf("loadGameTextures_3D() chamado. (Nao ha texturas para carregar)\n");
}