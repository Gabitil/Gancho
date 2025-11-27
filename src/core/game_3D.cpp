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
const float MOUSE_SENSITIVITY_3D = 0.1f; // Velocidade de rotação da câmera (sensibilidade do mouse)
const float MAX_CHARGE_TIME_3D = 2.0f;   // Tempo máximo em segundos para carregar a força total
const float MAX_PITCH_3D = 89.0f;        // Limite vertical superior da câmera (Pitch)
const float MIN_PITCH_3D = 5.0f;         // Limite vertical inferior da câmera (Pitch)

// ==================================================================================
// ## 2. OBJETOS DO JOGO E PARÂMETROS DE NÍVEL
// ==================================================================================

GameObject_3D player_3D;
GameObject_3D door_3D;
Platform_3D *collidingPlatform_3D = NULL;

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

struct LevelParameters_3D
{
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
    float maxHookLength;
    float aimConvergenceDist;
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
float cameraYaw_3D = -90.0f; // Rotação horizontal (eixo Y)
float cameraPitch_3D = 0.0f; // Rotação vertical (eixo X)
float aimPitch_3D = 20.0f;   // NOVO: Ângulo real da mira

// Limites
const float CAMERA_STOP_PITCH = 30.0f; // A câmera para de subir aqui
const float MAX_AIM_PITCH = 85.0f;     // A mira continua até aqui

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

// Variáveis de Câmera (Modo 3ª Pessoa)
float cameraDistance = 30.0f; // Distância da câmera ao jogador

// Variáveis de Display List (Chão Grade)
GLuint floorListID = 0;
const float WORLD_SIZE = 100.0f; // Tamanho visual da grade
const float FLOOR_Y = 0.0f;

int lastMouseX_3D = 0;
int lastMouseY_3D = 0;

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
// FUNÇÕES DE CRIAÇÃO DO MUNDO
// =================================================================================

/**
 * Cria os VBOs para toda a geometria estática do nível 3D.
 */
void createLevelVBOs()
{
    // 1. Limpeza de VBOs antigos (libera a memória da GPU)
    for (auto &info : platformVBOs)
    {
        if (info.vboID != 0)
            glDeleteBuffers(1, &info.vboID);
    }
    platformVBOs.clear();

    for (auto &info : wallVBOs)
    {
        if (info.vboID != 0)
            glDeleteBuffers(1, &info.vboID);
    }
    wallVBOs.clear();

    for (auto &info : spikeVBOs)
    {
        if (info.vboID != 0)
            glDeleteBuffers(1, &info.vboID);
    }
    spikeVBOs.clear();

    // 2. Processar Plataformas
    for (const auto &p : platforms_3D)
    {
        int triCount = 0;
        // Gera os triângulos na CPU
        tri *platformTris = createBoxTris(p.x, p.y, p.z, p.w, p.h, p.d, p.r, p.g, p.b, triCount);

        // Envia para a GPU e guarda o ID
        VBO_Info info;
        info.vboID = criaVBODeTris(triCount, platformTris);
        info.triCount = triCount;
        platformVBOs.push_back(info);

        // Limpa a memória da CPU, pois já está na placa de vídeo
        free(platformTris);
    }

    // 3. Processar Paredes Quebráveis
    for (const auto &w : breakableWalls_3D)
    {
        if (w.isBroken)
            continue; // Não desenha se já quebrou

        int triCount = 0;
        // Paredes com cor fixa (ex: marrom/laranja) ou usando w.r, w.g, w.b se sua struct tiver cor
        // Assumindo que sua struct BreakableWall_3D tem cor:
        tri *wallTris = createBoxTris(w.x, w.y, w.z, w.w, w.h, w.d, w.r, w.g, w.b, triCount);

        VBO_Info info;
        info.vboID = criaVBODeTris(triCount, wallTris);
        info.triCount = triCount;
        wallVBOs.push_back(info);

        free(wallTris);
    }

    // 4. Processar Espinhos
    for (const auto &s : spikeZones_3D)
    {
        int triCount = 0;
        // Espinhos vermelhos
        tri *spikeTris = createBoxTris(s.x, s.y, s.z, s.w, s.h, s.d, 1.0f, 0.0f, 0.0f, triCount);

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

/**
 * Cria as Display Lists para os objetos estáticos do mundo (grade do chão).
 */
/**
 * Cria a grade do chão (Visualização Antiga - Corrigida)
 */
void createWorldDisplayLists()
{
    if (floorListID != 0)
        glDeleteLists(floorListID, 1);

    floorListID = glGenLists(1);
    glNewList(floorListID, GL_COMPILE);
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);

    // CORREÇÃO: Chão Branco (1.0, 1.0, 1.0) como pedido
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);
    // Usa WORLD_SIZE que já está definido nas suas globais
    for (float i = -WORLD_SIZE; i <= WORLD_SIZE; i += 2.0f)
    {
        glVertex3f(i, FLOOR_Y, -WORLD_SIZE);
        glVertex3f(i, FLOOR_Y, WORLD_SIZE);
    }
    for (float i = -WORLD_SIZE; i <= WORLD_SIZE; i += 2.0f)
    {
        glVertex3f(-WORLD_SIZE, FLOOR_Y, i);
        glVertex3f(WORLD_SIZE, FLOOR_Y, i);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEndList();
}

// =================================================================================
// TELAS E INTERFACE (Modularizadas)
// =================================================================================

/**
 * Desenha a tela de Vitória (Sobreposição Verde)
 */
void drawVictoryScreen(int w, int h)
{
    // Fundo Verde Transparente
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Cor: R, G, B, Alpha (0.0, 0.8, 0.0, 0.5)
    drawRect(0, 0, w, h, 0.0f, 0.8f, 0.0f, 0.5f);

    glDisable(GL_BLEND);

    // Texto Centralizado
    glColor3f(1.0f, 1.0f, 1.0f); // Branco
    drawTextCentered(w / 2.0f, h / 2.0f - 20, "NIVEL COMPLETADO!", GLUT_BITMAP_TIMES_ROMAN_24);
    drawTextCentered(w / 2.0f, h / 2.0f + 20, "Aguarde...", GLUT_BITMAP_HELVETICA_18);
}

/**
 * Desenha a tela de Game Over (Sobreposição Vermelha)
 */
void drawGameOverScreen(int w, int h)
{
    // Fundo Vermelho Transparente
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Cor: R, G, B, Alpha (0.8, 0.0, 0.0, 0.5)
    drawRect(0, 0, w, h, 0.8f, 0.0f, 0.0f, 0.5f);

    glDisable(GL_BLEND);

    // Texto Centralizado
    glColor3f(1.0f, 1.0f, 1.0f); // Branco
    drawTextCentered(w / 2.0f, h / 2.0f - 20, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    drawTextCentered(w / 2.0f, h / 2.0f + 20, "Reiniciando...", GLUT_BITMAP_HELVETICA_18);
}

void drawPhysicsDebugHUD_3D()
{
    // Configura projeção 2D para UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, w, h, 0); // 0,0 no topo esquerdo

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // --- Coleta de Dados ---
    std::vector<std::string> infoLines;
    char buffer[100];

    // 1. Velocidade (Módulo 3D)
    float vMag = sqrt(player_3D.velocityX * player_3D.velocityX +
                      player_3D.velocityY * player_3D.velocityY +
                      player_3D.velocityZ * player_3D.velocityZ);
    sprintf(buffer, "v: %.1f m/s", vMag);
    infoLines.push_back(buffer);

    // 2. Aceleração (Calculada no Update)
    sprintf(buffer, "a: %.2f m/s^2", currentAcceleration_3D);
    infoLines.push_back(buffer);

    // 3. Peso (P = m * g)
    float weightForce = levelParameters_3D.gravity * levelParameters_3D.playerMass;
    sprintf(buffer, "P: %.2f N", weightForce);
    infoLines.push_back(buffer);

    // 4. Normal e Atrito (Se no chão)
    if (isGrounded_3D)
    {
        // Normal (considerando apenas a componente Y que calculamos)
        sprintf(buffer, "N: %.2f N", forceNormalY_3D);
        infoLines.push_back(buffer);

        // Atrito (Módulo do vetor de atrito XZ)
        float fricMag = sqrt(forceFrictionX_3D * forceFrictionX_3D + forceFrictionZ_3D * forceFrictionZ_3D);
        if (fricMag > 0.001f)
        {
            sprintf(buffer, "Fat: %.2f N", fricMag);
            infoLines.push_back(buffer);
        }
    }

    // 5. Tensão (Se no gancho)
    if (isHooked_3D)
    {
        // Calcula a magnitude real do vetor de tensão atual (Pitágoras 3D)
        float tMag = sqrt(forceTensionX_3D * forceTensionX_3D +
                          forceTensionY_3D * forceTensionY_3D +
                          forceTensionZ_3D * forceTensionZ_3D);

        sprintf(buffer, "T: %.2f N", tMag);
        infoLines.push_back(buffer);
    }

    // 6. Coeficientes de Atrito das Plataformas Visíveis
    // No 3D "visível" é mais complexo, vamos listar apenas a plataforma que estamos colidindo
    if (collidingPlatform_3D)
    {
        sprintf(buffer, "mu_chao: %.2f", collidingPlatform_3D->frictionCoefficient);
        infoLines.push_back(buffer);
    }

    // --- Renderização do Bloco ---
    float lineHeight = 15.0f;
    float margin = 10.0f;
    float padding = 20.0f;

    // Calcula largura máxima
    float maxWidth = 0;
    for (const auto &line : infoLines)
    {
        // getTextWidth deve estar disponível via utils.h
        maxWidth = std::max(maxWidth, (float)getTextWidth(line.c_str(), GLUT_BITMAP_9_BY_15));
    }

    float blockWidth = maxWidth + (2 * padding) + 10.0f;
    float blockHeight = (infoLines.size() * lineHeight) + (2 * padding) + 5.0f;
    float blockX = w - margin - blockWidth;
    float blockY = margin;

    // Fundo
    if (texDisplayGrappler != 0)
    {
        glColor3f(1, 1, 1);
        drawTexturedRect(blockX, blockY, blockWidth, blockHeight, texDisplayGrappler, false, false);
    }
    else
    {
        drawRect(blockX, blockY, blockWidth, blockHeight, 0.0f, 0.0f, 0.0f, 0.7f); // Preto transparente
    }

    // Texto
    float currentY = blockY + padding + 10;
    for (const auto &line : infoLines)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(blockX + padding, currentY, line.c_str(), GLUT_BITMAP_9_BY_15);
        currentY += lineHeight;
    }

    // Restaura
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void drawShotCounterHUD_3D()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, w, h, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    char shotText[50];
    // Usando levelParameters_3D.maxShots (se existir na struct, senão use um valor fixo ou adicione)
    // Assumindo que 'shotsRemaining_3D' é global
    int maxShots = 5; // Valor padrão ou levelParameters_3D.maxShots
    sprintf(shotText, "Disparos: %d / %d", shotsRemaining_3D, maxShots);

    float textWidth = getTextWidth(shotText, GLUT_BITMAP_9_BY_15);
    float padding = 18.0f;
    float blockWidth = textWidth + (2 * padding) + 20.0f;
    float blockHeight = 6.0f + (2 * padding) + 6.0f;
    float blockX = w / 2.0f - blockWidth / 2.0f;
    float blockY = 10.0f;

    if (texDisplayGrappler != 0)
    {
        glColor3f(1, 1, 1);
        drawTexturedRect(blockX, blockY, blockWidth, blockHeight, texDisplayGrappler, false, false);
    }
    else
    {
        drawRect(blockX, blockY, blockWidth, blockHeight, 0.0f, 0.0f, 0.0f, 0.7f);
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    drawTextCentered(w / 2.0f, blockY + padding + 10, shotText, GLUT_BITMAP_9_BY_15);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/**
 * Desenha a barra de carregamento do gancho
 */
void drawChargeBar(int w, int h)
{
    if (!isChargingHook_3D)
        return;

    float barCX = w / 2.0f;
    float barCY = h - 100.0f;
    float barW = 200.0f;
    float barH = 20.0f;

    // Desenho da barra (Fundo e Preenchimento)
    glColor3f(0.2f, 0.2f, 0.2f);
    drawRect(barCX - barW / 2, barCY, barW, barH, 0.2f, 0.2f, 0.2f);

    float fillW = barW * chargePercentage_3D;
    glColor3f(1.0f, 1.0f - chargePercentage_3D, 0.0f); // Amarelo -> Vermelho
    drawRect(barCX - barW / 2, barCY, fillW, barH, 1.0f, 1.0f - chargePercentage_3D, 0.0f);

    // Texto com o Valor em Newtons (NOVO)
    char forceText[50];
    sprintf(forceText, "Forca: %.1f N", currentChargeForce_3D);

    glColor3f(1.0f, 1.0f, 1.0f);
    // Desenha logo acima da barra (-20px no Y)
    drawTextCentered(barCX, barCY - 20.0f, forceText, GLUT_BITMAP_HELVETICA_18);
}

/**
 * Desenha toda a interface 2D (HUD, Barra, Telas Finais).
 * Substitui a drawHUD anterior.
 */
void drawHUD()
{
    // Configura projeção 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, w, h, 0); // Y invertido (0 no topo) para bater com as coordenadas do mouse/UI

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Desabilita efeitos 3D para desenhar a UI chapada
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // --- Lógica de Seleção de Tela ---
    if (isGameVictory_3D)
    {
        drawVictoryScreen(w, h);
    }
    else if (isGameOver_3D)
    {
        drawGameOverScreen(w, h);
    }
    else
    {
        // Jogo Rodando:
        drawChargeBar(w, h);

        // --- NOVAS CHAMADAS ---
        drawPhysicsDebugHUD_3D();
        drawShotCounterHUD_3D();

        // --- MIRA (CROSSHAIR) ---
        float centerX = w / 2.0f;
        float centerY = h / 2.0f;

        // [CORREÇÃO PRECISA DA MIRA]

        // 1. Converter diferença de ângulo para Radianos
        float pitchDiffDeg = aimPitch_3D - cameraPitch_3D;
        float pitchDiffRad = pitchDiffDeg * M_PI / 180.0f;

        // 2. Pegar o FOV (Field of View) que definimos no gameReshape (45 graus)
        float fovDeg = 45.0f;
        float fovRad = (fovDeg / 2.0f) * M_PI / 180.0f;

        // 3. Fórmula Mágica da Projeção:
        // Isso converte o ângulo 3D perfeitamente para pixels na tela 2D
        // A tangente (tan) corrige a distorção da perspectiva.
        float screenHalfHeight = h / 2.0f;
        float offsetY = screenHalfHeight * (tan(pitchDiffRad) / tan(fovRad));

        // 4. Aplica ao centro (Atenção ao sinal!)
        // Se a mira estiver invertida visualmente (subindo quando devia descer), troque o '-' por '+'
        float crossY = centerY + offsetY;
        float crossX = centerX;

        // Desenha a cruz (Verde)
        glColor3f(0.0f, 1.0f, 0.0f);
        glLineWidth(2.0f);
        float size = 10.0f;
        glBegin(GL_LINES);
        glVertex2f(crossX - size, crossY);
        glVertex2f(crossX + size, crossY);
        glVertex2f(crossX, crossY - size);
        glVertex2f(crossX, crossY + size);
        glEnd();
    }

    // Restaura matrizes e estados 3D
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/**
 * Desenha os vetores de debug (Normal, Atrito, Peso, Velocidade, Tensão).
 * Equivalente visual ao jogo 2D.
 */
void drawDebugVectors()
{
    // 1. Definir pontos de origem
    // Centro de Massa (para Peso, Velocidade, Tensão)
    float cx = player_3D.x + player_3D.w / 2.0f;
    float cy = player_3D.y + player_3D.h / 2.0f;
    float cz = player_3D.z + player_3D.d / 2.0f;
    Vector_3D center = {cx, cy, cz};

    // Pés (para Normal e Atrito)
    Vector_3D feet = {cx, cy - player_3D.h / 2.0f, cz};

    // Escala para Forças (Newton -> Tamanho Visual)
    float scaleForce = levelParameters_3D.vectorVisualScale;

    // ---------------------------------------------------
    // A. VETOR PESO (P) - Roxo/Magenta
    // Sempre presente. P = m * g (Aponta para baixo)
    // ---------------------------------------------------
    float weightVal = levelParameters_3D.playerMass * levelParameters_3D.gravity;
    Vector_3D vecP = {0, -weightVal, 0};

    // Cria um ponto deslocado apenas para desenhar o peso
    Vector_3D weightPos = {center.x + 2.0f, center.y, center.z};
    drawVector_3D(center, vecP, scaleForce, 1.0f, 0.0f, 1.0f, "P");

    // ---------------------------------------------------
    // B. VETOR VELOCIDADE (V) - Verde
    // Usa uma escala diferente (cinemática vs dinâmica)
    // ---------------------------------------------------
    float vMag = sqrt(player_3D.velocityX * player_3D.velocityX +
                      player_3D.velocityY * player_3D.velocityY +
                      player_3D.velocityZ * player_3D.velocityZ);

    if (vMag > 0.1f)
    {
        // Se quiser usar VELOCITY_VISUAL_SCALE_3D (7.0f) definido no header:
        float scaleVel = 0.5f; // Ou use: VELOCITY_VISUAL_SCALE_3D * 0.1f;

        Vector_3D vecV = {player_3D.velocityX, player_3D.velocityY, player_3D.velocityZ};
        drawVector_3D(center, vecV, scaleVel, 0.0f, 1.0f, 0.0f, "V");
    }

    // ---------------------------------------------------
    // C. FORÇAS DE CONTATO (Se estiver no chão)
    // ---------------------------------------------------
    if (isGrounded_3D)
    {
        // Normal (N) - Azul Ciano
        // Aponta para cima perpendicular ao chão
        Vector_3D vecN = {0, forceNormalY_3D, 0};
        drawVector_3D(feet, vecN, scaleForce, 0.0f, 1.0f, 1.0f, "N");

        // Atrito (Fat) - Vermelho
        // Aponta contra o movimento
        float fricMag = sqrt(forceFrictionX_3D * forceFrictionX_3D + forceFrictionZ_3D * forceFrictionZ_3D);
        if (fricMag > 0.1f)
        {
            // Desenhamos um pouquinho acima do pé (+0.1 em Y) para a linha não sumir dentro da grade do chão
            Vector_3D feetRaised = {feet.x, feet.y + 0.1f, feet.z};
            Vector_3D vecFat = {forceFrictionX_3D, 0, forceFrictionZ_3D};
            drawVector_3D(feetRaised, vecFat, scaleForce, 1.0f, 0.0f, 0.0f, "Fat");
        }
    }

    // ---------------------------------------------------
    // D. TENSÃO (T) - Azul
    // Se estiver pendurado no gancho
    // ---------------------------------------------------
    if (isHooked_3D)
    {
        // 1. Define a origem do vetor na MÃO/ARMA (Mesma altura usada no gameDisplay)
        // O centro.y é o meio do corpo. A corda sai um pouco acima (+0.4h)
        float ropeOriginY = center.y + (player_3D.h * 0.4f);
        
        Vector_3D ropeStartPos = {center.x, ropeOriginY, center.z};

        // 2. Vetor de Força
        Vector_3D vecT = {forceTensionX_3D, forceTensionY_3D, forceTensionZ_3D};

        // 3. Desenha o vetor SOBRE a corda
        // O vetor vai começar na mão e seguir a linha da corda.
        // O tamanho da seta azul representará a intensidade da força.
        drawVector_3D(ropeStartPos, vecT, scaleForce, 0.2f, 0.2f, 1.0f, "T");
    }

    // ---------------------------------------------------
    // E. MIRA/LANÇAMENTO (Se estiver mirando/carregando)
    // ---------------------------------------------------
    if (isChargingHook_3D && !isHooked_3D)
    {
        // Calcula vetor de lançamento baseado na carga atual
        // Direção da câmera (recalculada aqui ou pega globais se tiver)
        float yawRad = cameraYaw_3D * M_PI / 180.0f;
        float pitchRad = cameraPitch_3D * M_PI / 180.0f;

        float dirX = -sin(yawRad) * cos(pitchRad);
        float dirY = -sin(pitchRad);
        float dirZ = -cos(yawRad) * cos(pitchRad);

        // Vetor Força de Lançamento
        float forceVal = currentChargeForce_3D; // Valor calculado no update
        Vector_3D vecAim = {dirX * forceVal, dirY * forceVal, dirZ * forceVal};

        // Desenha em Branco/Cinza saindo do player
        drawVector_3D(center, vecAim, scaleForce, 0.8f, 0.8f, 0.8f, "F_imp");
    }
}

// =================================================================================
// IMPLEMENTAÇÃO DA API game_3D.h
// =================================================================================

/**
 * Chamada quando o nível 3D é iniciado.
 * Estrutura adaptada para o uso de VBOs e múltiplos níveis.
 */
void gameStartLevel_3D(int level)
{
    printf("gameStartLevel_3D() chamado para o nivel %d.\n", level);

    // --- Configuração do OpenGL ---
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // AJUSTE: CÉU AZUL CLARO
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

    // --- Inicialização do Player ---
    player_3D.velocityX = 0.0f;
    player_3D.velocityY = 0.0f;
    player_3D.velocityZ = 0.0f;
    player_3D.w = 1.0f;
    player_3D.h = 2.0f;
    player_3D.d = 1.0f;

    // --- Resetar Variáveis de Controle ---
    isPressingForward_3D = false;
    isPressingBackward_3D = false;
    isPressingLeft_3D = false;
    isPressingRight_3D = false;

    isGrounded_3D = true;
    isHooked_3D = false;
    isHookFiring_3D = false;
    isChargingHook_3D = false;
    requestHookRelease_3D = false;

    // --- Configuração da Câmera ---
    cameraYaw_3D = 0.0f;
    cameraPitch_3D = 20.0f;
    aimPitch_3D = 20.0f;
    cameraDistance = 20.0f;

    // Resetar Física e Estados
    lastVelocityMag_3D = 0.0f;
    currentAcceleration_3D = 0.0f;
    forceNormalX_3D = 0;
    forceNormalY_3D = 0;
    forceNormalZ_3D = 0;
    forceFrictionX_3D = 0;
    forceFrictionY_3D = 0;
    forceFrictionZ_3D = 0;
    forceTensionX_3D = 0;
    forceTensionY_3D = 0;
    forceTensionZ_3D = 0;

    isGameOver_3D = false;
    isGameVictory_3D = false;

    // Limpar vetores antigos
    platforms_3D.clear();
    breakableWalls_3D.clear();
    windZones_3D.clear();
    spikeZones_3D.clear();

    // --- Setup Mouse ---
    glutSetCursor(GLUT_CURSOR_NONE);
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    gameReshape_3D(w, h);
    glutWarpPointer(w / 2, h / 2);
    mouseInitialized_3D = false;

    // --- Carregamento do Nível ---
    switch (level)
    {
    case 1:
        // Parâmetros Físicos Equilibrados
        levelParameters_3D.playerMass = 10.0f;
        levelParameters_3D.playerWalkAccel = 60.0f;
        levelParameters_3D.maxWalkSpeed = 15.0f;
        levelParameters_3D.maxPlayerSpeed = 40.0f; // Aumentado para permitir balanços rápidos
        levelParameters_3D.maxHookLength = 100.0f; // Corda mais longa para grandes vãos
        levelParameters_3D.vectorVisualScale = 0.04f;
        levelParameters_3D.gravity = 25.0f; // Gravidade um pouco mais forte para "sentir" o peso
        levelParameters_3D.hookSpeed = 150.0f;
        levelParameters_3D.maxShots = 15; // Mais tiros para testes
        levelParameters_3D.maxPullStrengthPhysics = 150.0f;
        levelParameters_3D.dampingFactor = 0.995f; // Pouca resistência no ar
        levelParameters_3D.aimConvergenceDist = 40.0f;

        // 1. O CHÃO BASE (Lava segura / Floor)
        platforms_3D.push_back({-WORLD_WIDTH_3D / 2, -5.0f, -WORLD_DEPTH_3D / 2, WORLD_WIDTH_3D, 5.0f, WORLD_DEPTH_3D, 0.3f, 0.3f, 0.3f, true, 0.8f});

        // 2. PAREDE DE TESTE DE MIRA (Atrás do player)
        platforms_3D.push_back({-10.0f, 0.0f, -20.0f, 20.0f, 15.0f, 5.0f, 0.6f, 0.6f, 0.7f, true, 0.5f});

        // 3. O CAMINHO DE PILARES (Parkour Básico)
        // Pilar 1
        platforms_3D.push_back({0.0f, 0.0f, 15.0f, 5.0f, 4.0f, 5.0f, 0.5f, 0.5f, 0.8f, true, 0.5f});
        // Pilar 2 (Mais alto)
        platforms_3D.push_back({0.0f, 4.0f, 35.0f, 5.0f, 8.0f, 5.0f, 0.5f, 0.5f, 0.8f, true, 0.5f});
        // Pilar 3 (Longe, requer pulo + gancho ou dash)
        platforms_3D.push_back({0.0f, 6.0f, 65.0f, 8.0f, 2.0f, 8.0f, 0.5f, 0.5f, 0.8f, true, 0.5f});

        // 4. O "CANYON" (Desafio de Gancho)
        // Paredes laterais altas para se pendurar e cruzar o vão
        platforms_3D.push_back({-25.0f, 0.0f, 80.0f, 5.0f, 40.0f, 60.0f, 0.4f, 0.4f, 0.4f, true, 0.3f}); // Esq
        platforms_3D.push_back({25.0f, 0.0f, 80.0f, 5.0f, 40.0f, 60.0f, 0.4f, 0.4f, 0.4f, true, 0.3f});  // Dir

        // Teto alto conectando as paredes (para balançar igual Tarzan)
        platforms_3D.push_back({-25.0f, 35.0f, 80.0f, 55.0f, 2.0f, 60.0f, 0.7f, 0.7f, 0.7f, true, 0.3f});

        // 5. ZONA DE PERIGO (Espinhos no chão do Canyon)
        spikeZones_3D.push_back({-20.0f, 0.1f, 80.0f, 45.0f, 1.0f, 60.0f});

        // 6. PAREDE QUEBRÁVEL (Obstáculo Final)
        // Bloqueia a saída do Canyon. Precisa vir com velocidade do balanço.
        platforms_3D.push_back({-10.0f, 6.0f, 150.0f, 20.0f, 2.0f, 20.0f, 0.5f, 0.5f, 0.8f, true, 0.5f});        // Plataforma de pouso
        breakableWalls_3D.push_back({-5.0f, 8.0f, 160.0f, 10.0f, 10.0f, 2.0f, 0.8f, 0.4f, 0.1f, 400.0f, false}); // Parede Frágil

        // 7. A PORTA (Objetivo)
        door_3D.x = 0.0f;
        door_3D.y = 8.0f;   // Em cima da plataforma final
        door_3D.z = 180.0f; // Atrás da parede quebrável
        door_3D.w = 4.0f;
        door_3D.h = 6.0f;
        door_3D.d = 4.0f;

        // Posição Inicial
        player_3D.x = 0.0f;
        player_3D.y = 2.0f;
        player_3D.z = 0.0f;
        break;

    case 2:
        // ... (Mantenha seu nível 2 ou copie a lógica acima para testes diferentes)
        levelParameters_3D.playerMass = 8.0f;
        levelParameters_3D.playerWalkAccel = 70.0f;
        levelParameters_3D.maxHookLength = 80.0f;
        levelParameters_3D.maxShots = 6;
        platforms_3D.push_back({-WORLD_WIDTH_3D / 2, -1.0f, -WORLD_DEPTH_3D / 2, WORLD_WIDTH_3D, 1.0f, WORLD_DEPTH_3D, 0.2f, 0.2f, 0.5f, true, 0.7f});
        player_3D.x = 0.0f;
        player_3D.y = 3.0f;
        player_3D.z = 0.0f;
        break;

    default:
        gameStartLevel_3D(1);
        return;
    }

    // --- CORREÇÃO DO MOUSE E ESTADOS FINAIS ---
    
    // 1. Força o mouse a ficar preso (Resolvendo o problema de ter que apertar 'M')
    isMouseFree_3D = false; 

    // 2. Configura cursor
    glutSetCursor(GLUT_CURSOR_NONE);
    
    // 3. Setup de Tela e Ponteiro
    gameReshape_3D(w, h);
    
    // 4. Força o ponteiro para o centro IMEDIATAMENTE
    glutWarpPointer(w / 2, h / 2);
    
    // 5. Reinicia a flag de "primeiro movimento" para evitar solavancos
    mouseInitialized_3D = false;

    // Recarregar munição e definir plataforma inicial
    shotsRemaining_3D = levelParameters_3D.maxShots;
    if (!platforms_3D.empty())
        collidingPlatform_3D = &platforms_3D[0];
    else
        collidingPlatform_3D = NULL;

    // Gera a geometria
    createWorldDisplayLists(); 
    createLevelVBOs();
}

GameAction gameUpdate_3D()
{
    // 1. Vitória/Derrota
    if (isGameVictory_3D)
    {
        gameVictoryTimer_3D--;
        if (gameVictoryTimer_3D <= 0)
        {
            isGameVictory_3D = false;
            return GAME_ACTION_LEVEL_WON;
        }
        return GAME_ACTION_CONTINUE;
    }
    if (isGameOver_3D)
    {
        gameOverTimer_3D--;
        if (gameOverTimer_3D <= 0)
        {
            isGameOver_3D = false;
            return GAME_ACTION_LEVEL_LOST;
        }
        return GAME_ACTION_CONTINUE;
    }

    // 2. Delta Time
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (float)(currentTime - lastTime_3D) / 1000.0f;
    lastTime_3D = currentTime;
    if (deltaTime > 0.1f)
        deltaTime = 0.1f;

    // 3. Variáveis Locais
    float prevPositionPlayerX = player_3D.x;
    float prevPositionPlayerY = player_3D.y;
    float prevPositionPlayerZ = player_3D.z;

    float pCx = player_3D.x + player_3D.w / 2.0f;
    float pCy = player_3D.y + player_3D.h / 2.0f;
    float pCz = player_3D.z + player_3D.d / 2.0f;

    forceNormalX_3D = 0;
    forceNormalY_3D = 0;
    forceNormalZ_3D = 0;
    forceFrictionX_3D = 0;
    forceFrictionY_3D = 0;
    forceFrictionZ_3D = 0;
    forceTensionX_3D = 0;
    forceTensionY_3D = 0;
    forceTensionZ_3D = 0;

    float accelX = 0, accelY = -levelParameters_3D.gravity, accelZ = 0;

    // 4. Vento
    for (const auto &wind : windZones_3D)
    {
        if (isPointInsideBox(pCx, pCy, pCz, wind.x, wind.y, wind.z, wind.w, wind.h, wind.d))
        {
            accelX += wind.accelX;
            accelY += wind.accelY;
            accelZ += wind.accelZ;
        }
    }

    // =========================================================================
    // --- LÓGICA DO GANCHO ---
    // =========================================================================

    // A. CARREGAMENTO
    if (isChargingHook_3D)
    {
        float chargeTime = (glutGet(GLUT_ELAPSED_TIME) - chargeStartTime_3D) / 1000.0f;
        chargePercentage_3D = chargeTime / MAX_CHARGE_TIME_3D;
        if (chargePercentage_3D > 1.0f)
            chargePercentage_3D = 1.0f;
        currentChargeForce_3D = chargePercentage_3D * MAX_AIM_FORCE_DISPLAY_3D;
    }

    // B. AÇÃO DE DISPARO (Botão Solto)
    if (requestHookRelease_3D)
    {
        requestHookRelease_3D = false; // Consome o evento imediatamente

        // Se estava PRESO, solta (Prioridade 1)
        if (isHooked_3D)
        {
            isHooked_3D = false;
            isGrounded_3D = false;
            printf("UPDATE: Soltou do gancho.\n");
        }
        // Tenta disparar (Prioridade 2)
        else
        {
            // A validação de munição deve ser feita AQUI
            if (shotsRemaining_3D > 0)
            {
                // Zera qualquer estado residual
                isHookFiring_3D = true;
                isChargingHook_3D = false; // Garante que parou de carregar

                shotsRemaining_3D--;

                printf("UPDATE: DISPARO CONFIRMADO! Forca: %.1f. Restam: %d\n", currentChargeForce_3D, shotsRemaining_3D);

                // --- CONFIGURAÇÃO DO PROJÉTIL ---
                float fireYawRad, firePitchRad;

                if (isMouseFree_3D)
                {
                    // ... (Mantenha sua lógica de mouse livre aqui se quiser, ou simplifique) ...
                    // Para simplificar e garantir precisão, no modo livre geralmente usamos a mesma lógica:
                    fireYawRad = cameraYaw_3D * M_PI / 180.0f;
                    firePitchRad = aimPitch_3D * M_PI / 180.0f;
                }
                else
                {
                    fireYawRad = cameraYaw_3D * M_PI / 180.0f;
                    firePitchRad = aimPitch_3D * M_PI / 180.0f;
                }

                // 2. Vetor de Direção da MIRA (Para onde o olho está olhando)
                // Nota: Usamos aimPitch_3D para determinar a inclinação do raio
                float dirX = -sin(fireYawRad) * cos(firePitchRad);
                float dirY = -sin(firePitchRad);
                float dirZ = -cos(fireYawRad) * cos(firePitchRad);

                // 3. Posição de Nascimento do Gancho (Mão/Peito do Player)
                float spawnDistance = 1.5f; // Ajustado para não nascer dentro de paredes
                hookProjectileX_3D = pCx + (dirX * spawnDistance);
                hookProjectileY_3D = pCy + (player_3D.h * 0.6f) + (dirY * spawnDistance); // 0.6f sobe um pouco a saída
                hookProjectileZ_3D = pCz + (dirZ * spawnDistance);

                // 4. CÁLCULO DE CONVERGÊNCIA (A Mágica)
                // Precisamos saber onde está a câmera REAL para projetar o raio através da mira (crosshair)

                // 1. ONDE ESTÁ O OLHO (Câmera)?
                // Recalcula posição exata para garantir sincronia
                float camRadPitch = cameraPitch_3D * M_PI / 180.0f;
                float camRadYaw = cameraYaw_3D * M_PI / 180.0f;

                float camX = pCx + cameraDistance * cos(camRadPitch) * sin(camRadYaw);
                float camY = pCy + (player_3D.h / 2.0f) + cameraDistance * sin(camRadPitch);
                float camZ = pCz + cameraDistance * cos(camRadPitch) * cos(camRadYaw);

                // 2. PARA ONDE O OLHO ESTÁ OLHANDO? (Direção da Mira)
                float aimRadPitch = aimPitch_3D * M_PI / 180.0f;
                float aimRadYaw = cameraYaw_3D * M_PI / 180.0f; // Assume Yaw travado com a câmera

                float dirViewX = -sin(aimRadYaw) * cos(aimRadPitch);
                float dirViewY = -sin(aimRadPitch);
                float dirViewZ = -cos(aimRadYaw) * cos(aimRadPitch);

                // 3. ONDE É O PONTO DE IMPACTO IDEAL? (A Reta Vertical/Plano de Convergência)
                // Aqui usamos o seu novo parâmetro. O alvo é fixado nesta distância exata.
                float dist = levelParameters_3D.aimConvergenceDist;
                float targetX = camX + (dirViewX * dist);
                float targetY = camY + (dirViewY * dist);
                float targetZ = camZ + (dirViewZ * dist);

                // 4. ONDE O TIRO NASCE? (Peito do Player)
                float spawnDist = 1.5f;
                hookProjectileX_3D = pCx + (dirViewX * spawnDist);
                hookProjectileY_3D = pCy + (player_3D.h * 0.6f) + (dirViewY * spawnDist); // Leve ajuste de altura
                hookProjectileZ_3D = pCz + (dirViewZ * spawnDist);

                // 5. CALCULA VELOCIDADE (Do Peito -> Ponto de Impacto Ideal)
                float vecX = targetX - hookProjectileX_3D;
                float vecY = targetY - hookProjectileY_3D;
                float vecZ = targetZ - hookProjectileZ_3D;

                // Normaliza e aplica velocidade
                float len = sqrt(vecX * vecX + vecY * vecY + vecZ * vecZ);
                hookProjectileVelX_3D = (vecX / len) * levelParameters_3D.hookSpeed;
                hookProjectileVelY_3D = (vecY / len) * levelParameters_3D.hookSpeed;
                hookProjectileVelZ_3D = (vecZ / len) * levelParameters_3D.hookSpeed;
                // -----------------------------------------------------------

                // Define Força Física
                float forceFactor = currentChargeForce_3D / MAX_AIM_FORCE_DISPLAY_3D;
                currentPullForce_3D = forceFactor * levelParameters_3D.maxPullStrengthPhysics;
            }
            else
            {
                printf("UPDATE: Falha no disparo (Sem municao ou estado invalido).\n");
            }
        }
    }

    // C. PROJÉTIL VOANDO
    if (isHookFiring_3D)
    {
        float prevHX = hookProjectileX_3D;
        float prevHY = hookProjectileY_3D;
        float prevHZ = hookProjectileZ_3D;

        // Move o projétil
        hookProjectileX_3D += hookProjectileVelX_3D * deltaTime;
        hookProjectileY_3D += hookProjectileVelY_3D * deltaTime;
        hookProjectileZ_3D += hookProjectileVelZ_3D * deltaTime;

        // --- DEBUG DE MOVIMENTO ---
        // Isso vai floodar o terminal um pouco, mas é necessário para entender o erro.
        // Se Vel for 0 ou Pos não mudar, saberemos o motivo.
        // printf("VOANDO: Pos(%.2f, %.2f, %.2f) Vel(%.2f, %.2f) dt: %.4f\n",
        //        hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D,
        //        hookProjectileVelX_3D, hookProjectileVelY_3D, deltaTime);

        // 1. Verifica Limite Máximo (Corda Max)
        float distOriginSq = pow(hookProjectileX_3D - pCx, 2) +
                             pow(hookProjectileY_3D - pCy, 2) +
                             pow(hookProjectileZ_3D - pCz, 2);

        if (distOriginSq > levelParameters_3D.maxHookLength * levelParameters_3D.maxHookLength)
        {
            isHookFiring_3D = false;
            printf("FIM DE CURSO: Gancho nao alcancou nada.\n");
        }
        else
        {
            // 2. Verifica Colisão com Plataformas
            // 2. Verifica Colisão com Plataformas
            for (auto &p : platforms_3D)
            {
                // [FIX ROBUSTO] Ignora colisão com qualquer plataforma que o jogador esteja TOCANDO ou MUITO PERTO.
                // Expandimos a caixa do jogador em 1.0f para este teste. Isso cobre o chão onde pisamos.
                if (checkAABBCollision(player_3D.x - 0.5f, player_3D.y - 0.5f, player_3D.z - 0.5f,
                                       player_3D.w + 1.0f, player_3D.h + 1.0f, player_3D.d + 1.0f,
                                       p.x, p.y, p.z, p.w, p.h, p.d))
                {
                    continue; // É o chão ou uma parede que estamos encostados. Ignora.
                }

                if (p.isHookable && p.frictionCoefficient > 0.001f)
                {
                    float hX, hY, hZ;
                    // Passamos a posição ANTERIOR e a ATUAL para verificar o trajeto (Raycast)
                    // Nota: Certifique-se que sua função lineBoxIntersection em utils trata o segmento,
                    // ou se for apenas ponto, a velocidade alta + spawn distance ajudará a não bugar.
                    if (lineBoxIntersection(prevHX, prevHY, prevHZ,
                                            hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D,
                                            p, hX, hY, hZ))
                    {
                        // Código de colisão confirmada (igual ao seu original)
                        isHookFiring_3D = false;
                        isHooked_3D = true;
                        hookPointX_3D = hX;
                        hookPointY_3D = hY;
                        hookPointZ_3D = hZ;
                        float distSq = pow(hX - pCx, 2) + pow(hY - pCy, 2) + pow(hZ - pCz, 2);
                        ropeLength_3D = sqrt(distSq);
                        isGrounded_3D = false;
                        printf("COLISAO: Gancho PRENDEU em %.2f, %.2f, %.2f!\n", hX, hY, hZ);
                        break;
                    }
                }
            }
        }
    }
    // D. FÍSICA PRESO
    if (isHooked_3D)
    {
        // 1. Vetor do Player até o Gancho
        float vecX = hookPointX_3D - pCx;
        float vecY = hookPointY_3D - pCy;
        float vecZ = hookPointZ_3D - pCz;
        float currDist = sqrt(vecX * vecX + vecY * vecY + vecZ * vecZ);

        if (currDist > 0.1f)
        {
            // Direção normalizada (Aponta para onde o gancho prendeu)
            float normX = vecX / currDist;
            float normY = vecY / currDist;
            float normZ = vecZ / currDist;

            // --- A. APLICAÇÃO DA FORÇA DE PUXADA ---
            // Adiciona aceleração na direção do gancho baseada na carga
            accelX += normX * currentPullForce_3D;
            accelY += normY * currentPullForce_3D;
            accelZ += normZ * currentPullForce_3D;

            // Atualiza vetores de debug
            forceTensionX_3D = normX * currentPullForce_3D;
            forceTensionY_3D = normY * currentPullForce_3D;
            forceTensionZ_3D = normZ * currentPullForce_3D;

            // --- B. MECÂNICA DE GUINCHO (A Mágica) ---
            // Se a força puxou o jogador para perto, ENCOLHEMOS a corda.
            // Isso impede que o jogador caia de volta para a distância antiga.
            if (currDist < ropeLength_3D)
            {
                ropeLength_3D = currDist;
            }

            // --- C. RESTRIÇÃO DE CORDA (Só impede de esticar) ---
            // Se tentarmos ir para LONGE (maior que a corda), travamos.
            if (currDist > ropeLength_3D)
            {
                float stretch = currDist - ropeLength_3D;

                // 1. Corrige Posição (Traz de volta para o raio da corda)
                player_3D.x += normX * stretch;
                player_3D.y += normY * stretch;
                player_3D.z += normZ * stretch;

                // 2. Corrige Velocidade (Anula velocidade de afastamento)
                // Projeta a velocidade na direção da corda
                float velAlongRope = player_3D.velocityX * normX +
                                     player_3D.velocityY * normY +
                                     player_3D.velocityZ * normZ;

                // Se a velocidade for negativa (estamos nos afastando do ponto de gancho), zeramos ela
                // (Nota: Como norm aponta P -> G, velocidade negativa = afastando)
                if (velAlongRope < 0)
                {
                    player_3D.velocityX -= normX * velAlongRope;
                    player_3D.velocityY -= normY * velAlongRope;
                    player_3D.velocityZ -= normZ * velAlongRope;
                }
            }

            // --- D. DESGRUDA DO CHÃO ---
            // Se a força Y for positiva e forte, tira o status de "Grounded"
            // para evitar que o atrito do chão freie o puxão inicial
            if (currentPullForce_3D > 10.0f && normY > 0.2f)
            {
                isGrounded_3D = false;
                collidingPlatform_3D = NULL;
            }
        }

        // Amortecimento leve no ar para não balançar para sempre
        float airDamping = 0.99f;
        player_3D.velocityX *= airDamping;
        player_3D.velocityY *= airDamping;
        player_3D.velocityZ *= airDamping;
    }
    // 6. Movimento no Chão
    if (isGrounded_3D)
    {
        forceNormalY_3D = levelParameters_3D.gravity * levelParameters_3D.playerMass;
        float yawRad = cameraYaw_3D * M_PI / 180.0f;
        float fwdX = -sin(yawRad);
        float fwdZ = -cos(yawRad);
        float rightX = cos(yawRad);
        float rightZ = -sin(yawRad);
        float walkForce = levelParameters_3D.playerWalkAccel;
        bool isMoving = false;

        if (isPressingForward_3D)
        {
            accelX += fwdX * walkForce;
            accelZ += fwdZ * walkForce;
            isMoving = true;
        }
        if (isPressingBackward_3D)
        {
            accelX -= fwdX * walkForce;
            accelZ -= fwdZ * walkForce;
            isMoving = true;
        }
        if (isPressingLeft_3D)
        {
            accelX -= rightX * walkForce;
            accelZ -= rightZ * walkForce;
            isMoving = true;
        }
        if (isPressingRight_3D)
        {
            accelX += rightX * walkForce;
            accelZ += rightZ * walkForce;
            isMoving = true;
        }

        float frictionCoef = 0.8f;
        if (collidingPlatform_3D)
            frictionCoef = collidingPlatform_3D->frictionCoefficient;
        float damping = 5.0f * frictionCoef;
        if (!isMoving)
            damping *= 2.0f;

        player_3D.velocityX -= player_3D.velocityX * damping * deltaTime;
        player_3D.velocityZ -= player_3D.velocityZ * damping * deltaTime;
        forceFrictionX_3D = -player_3D.velocityX * damping;
        forceFrictionZ_3D = -player_3D.velocityZ * damping;
    }

    // 8. Integração
    // =========================================================================
    // 8. MOVIMENTO FÍSICO COM COLISÃO ROBUSTA (EIXO POR EIXO)
    // =========================================================================

    // Atualiza velocidades primeiro
    player_3D.velocityX += accelX * deltaTime;
    player_3D.velocityY += accelY * deltaTime;
    player_3D.velocityZ += accelZ * deltaTime;

    // Limites de velocidade (Drag/Max Speed)
    if (isGrounded_3D)
    {
        float velXZ = sqrt(player_3D.velocityX * player_3D.velocityX + player_3D.velocityZ * player_3D.velocityZ);
        if (velXZ > levelParameters_3D.maxWalkSpeed)
        {
            float s = levelParameters_3D.maxWalkSpeed / velXZ;
            player_3D.velocityX *= s;
            player_3D.velocityZ *= s;
        }
    }
    // Limite global (exceto se estiver sendo puxado pelo gancho muito forte, opcional)
    float totalSpeed = sqrt(player_3D.velocityX * player_3D.velocityX + player_3D.velocityY * player_3D.velocityY + player_3D.velocityZ * player_3D.velocityZ);
    if (totalSpeed > levelParameters_3D.maxPlayerSpeed)
    {
        float s = levelParameters_3D.maxPlayerSpeed / totalSpeed;
        player_3D.velocityX *= s;
        player_3D.velocityY *= s;
        player_3D.velocityZ *= s;
    }

    // Pequeno offset para evitar "z-fighting" na colisão
    float epsilon = 0.01f;

    // -----------------------------------------------------------------------
    // PASSO A: MOVIMENTO EIXO X (Paredes Laterais)
    // -----------------------------------------------------------------------
    player_3D.x += player_3D.velocityX * deltaTime;

    // Verifica Paredes Quebráveis (X)
    for (auto &wall : breakableWalls_3D)
    {
        if (wall.isBroken)
            continue;
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               wall.x, wall.y, wall.z, wall.w, wall.h, wall.d))
        {
            // Tenta quebrar
            float impactForce = levelParameters_3D.playerMass * totalSpeed; // Simplificado
            if (impactForce >= wall.strength)
            {
                wall.isBroken = true;
                player_3D.velocityX *= 0.6f; // Perde velocidade ao quebrar
                // Não paramos o player, deixamos ele atravessar (satisfatório)

                createLevelVBOs();
            }
            else
            {
                // Colisão Sólida
                if (player_3D.velocityX > 0)
                    player_3D.x = wall.x - player_3D.w - epsilon; // Vinha da esquerda
                else if (player_3D.velocityX < 0)
                    player_3D.x = wall.x + wall.w + epsilon; // Vinha da direita
                player_3D.velocityX = 0;
            }
        }
    }

    // Verifica Plataformas (X)
    for (const auto &p : platforms_3D)
    {
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               p.x, p.y, p.z, p.w, p.h, p.d))
        {
            if (player_3D.velocityX > 0)
                player_3D.x = p.x - player_3D.w - epsilon;
            else if (player_3D.velocityX < 0)
                player_3D.x = p.x + p.w + epsilon;
            player_3D.velocityX = 0;
        }
    }

    // -----------------------------------------------------------------------
    // PASSO B: MOVIMENTO EIXO Z (Frente/Trás)
    // -----------------------------------------------------------------------
    player_3D.z += player_3D.velocityZ * deltaTime;

    // Verifica Paredes Quebráveis (Z)
    for (auto &wall : breakableWalls_3D)
    {
        if (wall.isBroken)
            continue;
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               wall.x, wall.y, wall.z, wall.w, wall.h, wall.d))
        {
            float impactForce = levelParameters_3D.playerMass * totalSpeed;
            if (impactForce >= wall.strength)
            {
                wall.isBroken = true;
                player_3D.velocityZ *= 0.6f;
            }
            else
            {
                if (player_3D.velocityZ > 0)
                    player_3D.z = wall.z - player_3D.d - epsilon;
                else if (player_3D.velocityZ < 0)
                    player_3D.z = wall.z + wall.d + epsilon;
                player_3D.velocityZ = 0;
            }
        }
    }

    // Verifica Plataformas (Z)
    for (const auto &p : platforms_3D)
    {
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               p.x, p.y, p.z, p.w, p.h, p.d))
        {
            if (player_3D.velocityZ > 0)
                player_3D.z = p.z - player_3D.d - epsilon;
            else if (player_3D.velocityZ < 0)
                player_3D.z = p.z + p.d + epsilon;
            player_3D.velocityZ = 0;
        }
    }

    // -----------------------------------------------------------------------
    // PASSO C: MOVIMENTO EIXO Y (Chão/Teto)
    // -----------------------------------------------------------------------
    player_3D.y += player_3D.velocityY * deltaTime;

    isGrounded_3D = false;
    collidingPlatform_3D = NULL;

    // Verifica Plataformas (Y)
    for (auto &p : platforms_3D)
    {
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               p.x, p.y, p.z, p.w, p.h, p.d))
        {
            // Colisão vindo de CIMA (Caindo no chão)
            if (player_3D.velocityY <= 0)
            {
                player_3D.y = p.y + p.h; // Posiciona exatamente em cima
                player_3D.velocityY = 0;
                isGrounded_3D = true;
                collidingPlatform_3D = &p;
            }
            // Colisão vindo de BAIXO (Batendo cabeça)
            else if (player_3D.velocityY > 0)
            {
                player_3D.y = p.y - player_3D.h - epsilon;
                player_3D.velocityY = 0;
            }
        }
    }

    // Verifica Paredes Quebráveis (Y) - (Caso pule em cima ou bata a cabeça)
    for (auto &wall : breakableWalls_3D)
    {
        if (wall.isBroken)
            continue;
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               wall.x, wall.y, wall.z, wall.w, wall.h, wall.d))
        {
            // Geralmente não quebramos parede pulando na cabeça, então tratamos como sólido
            if (player_3D.velocityY <= 0)
            {
                player_3D.y = wall.y + wall.h;
                player_3D.velocityY = 0;
                isGrounded_3D = true;
            }
            else
            {
                player_3D.y = wall.y - player_3D.h - epsilon;
                player_3D.velocityY = 0;
            }
        }
    }

    // -----------------------------------------------------------------------
    // 9. VERIFICAÇÕES FINAIS (Morte / Vitória)
    // -----------------------------------------------------------------------

    // Queda no abismo
    if (player_3D.y < DEATH_BOUNDARY_Y_3D)
    {
        isGameOver_3D = true;
        gameOverTimer_3D = 180;
        return GAME_ACTION_CONTINUE;
    }

    // Espinhos (Apenas check, sem física sólida para simplificar, ou adicione no loop acima se quiser sólido)
    for (const auto &spike : spikeZones_3D)
    {
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               spike.x, spike.y, spike.z, spike.w, spike.h, spike.d))
        {
            isGameOver_3D = true;
            gameOverTimer_3D = 180;
            return GAME_ACTION_CONTINUE;
        }
    }

    // Porta (Vitória)
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                           door_3D.x, door_3D.y, door_3D.z, door_3D.w, door_3D.h, door_3D.d))
    {
        isGameVictory_3D = true;
        gameVictoryTimer_3D = 180;
        return GAME_ACTION_CONTINUE;
    }

    return GAME_ACTION_CONTINUE;
}

/**
 * Loop principal de renderização.
 */
void gameDisplay_3D()
{
    // 1. Limpeza
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // --- CONFIGURAÇÃO DA CÂMERA ---
    float yawRad = cameraYaw_3D * M_PI / 180.0f;
    float pitchRad = cameraPitch_3D * M_PI / 180.0f;

    float cx = player_3D.x + player_3D.w / 2.0f;
    float cy = player_3D.y + player_3D.h / 2.0f;
    float cz = player_3D.z + player_3D.d / 2.0f;

    float camX = cx + cameraDistance * cos(pitchRad) * sin(yawRad);
    float camY = cy + cameraDistance * sin(pitchRad);
    float camZ = cz + cameraDistance * cos(pitchRad) * cos(yawRad);

    float targetY = cy + 3.0f;

    gluLookAt(camX, camY, camZ, cx, targetY, cz, 0.0f, 1.0f, 0.0f);

    // Luz
    GLfloat lightPos[] = {0.0f, 100.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // --- 2. CENÁRIO ---
    glCallList(floorListID);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    int stride = sizeof(vert);

    auto drawList = [&](const std::vector<VBO_Info> &list)
    {
        for (const auto &info : list)
        {
            if (info.vboID == 0)
                continue;
            glBindBuffer(GL_ARRAY_BUFFER, info.vboID);
            glVertexPointer(3, GL_FLOAT, stride, (void *)offsetof(vert, pos));
            glColorPointer(4, GL_FLOAT, stride, (void *)offsetof(vert, cor));
            glNormalPointer(GL_FLOAT, stride, (void *)offsetof(vert, normal));
            glDrawArrays(GL_TRIANGLES, 0, info.triCount * 3);
        }
    };

    drawList(platformVBOs);
    drawList(wallVBOs);
    drawList(spikeVBOs);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    // --- 3. OBJETOS DINÂMICOS ---
    // Porta
    glPushMatrix();
    glTranslatef(door_3D.x, door_3D.y, door_3D.z);
    glTranslatef(door_3D.w / 2, door_3D.h / 2, door_3D.d / 2);
    glScalef(door_3D.w, door_3D.h, door_3D.d);
    glColor3f(0.0f, 1.0f, 0.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Player
    glPushMatrix();
    glTranslatef(player_3D.x, player_3D.y, player_3D.z);
    glTranslatef(player_3D.w / 2, player_3D.h / 2, player_3D.d / 2);
    glScalef(player_3D.w, player_3D.h, player_3D.d);
    if (isGameOver_3D)
        glColor3f(0.2f, 0.0f, 0.0f);
    else
        glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Corda
    if (isHooked_3D || isHookFiring_3D)
    {
        // Desabilita luz para garantir que a linha tenha cor sólida
        glDisable(GL_LIGHTING);
        glLineWidth(2.0f);
        glColor3f(0.8f, 0.8f, 0.8f); // Cor cinza claro visível

        // Calcula centro do player para início da linha
        float cx = player_3D.x + player_3D.w / 2.0f;
        float cyShot = player_3D.y + player_3D.h / 2.0f + (player_3D.h * 0.4f);
        float cz = player_3D.z + player_3D.d / 2.0f;

        glBegin(GL_LINES);
        glVertex3f(cx, cyShot, cz); // Início: Player

        // Fim: Projétil voando OU Ponto preso
        if (isHookFiring_3D)
        {
            glVertex3f(hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D);
        }
        else
        {
            glVertex3f(hookPointX_3D, hookPointY_3D, hookPointZ_3D);
        }
        glEnd();

        // --- NOVO: MOSTRAR TENSÃO NA PRÓPRIA CORDA ---
        if (isHooked_3D)
        {
            // 1. Calcula o Ponto Médio da corda
            float midX = (cx + hookPointX_3D) / 2.0f;
            float midY = (cyShot + hookPointY_3D) / 2.0f;
            float midZ = (cz + hookPointZ_3D) / 2.0f;

            // 2. Pega o valor da Tensão (Magnitude do vetor)
            float tMag = sqrt(forceTensionX_3D * forceTensionX_3D +
                              forceTensionY_3D * forceTensionY_3D +
                              forceTensionZ_3D * forceTensionZ_3D);

            // 3. Formata e Desenha
            char tensionStr[32];
            sprintf(tensionStr, "%.1f N", tMag);

            // Usa a mesma cor Azul do vetor (0.2, 0.2, 1.0)
            glColor3f(0.2f, 0.2f, 1.0f);

            // Desenha um pouco acima da linha (+1.0f no Y) para não sobrepor
            drawText3D(midX, midY + 1.0f, midZ, tensionStr);
        }

        // Desenha a ponta (esfera) se estiver voando
        if (isHookFiring_3D)
        {
            glPushMatrix();
            glTranslatef(hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D);
            glColor3f(1.0f, 1.0f, 0.0f); // Amarelo para destacar
            glutSolidSphere(0.3f, 8, 8); // Esfera um pouco maior
            glPopMatrix();
        }

        glEnable(GL_LIGHTING); // Restaura
    }

    // --- VISUALIZAÇÃO DOS ATRITOS (DEBUG) ---
    // Desabilita luz e textura para o texto ficar nítido e com cor sólida
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Cor do texto (Amarelo para destaque ou Preto se o chão for muito claro)
    glColor3f(1.0f, 1.0f, 0.0f);

    char frictionText[32];

    for (const auto &p : platforms_3D)
    {
        // Calcula o centro da plataforma (topo)
        float cx = p.x + p.w / 2.0f;
        float cy = p.y + p.h + 0.5f; // +0.5f para flutuar um pouco acima do chão
        float cz = p.z + p.d / 2.0f;

        // Formata o texto: "u: 0.80"
        sprintf(frictionText, "u: %.2f", p.frictionCoefficient);

        // Desenha
        drawText3D(cx, cy, cz, frictionText);
    }

    // --- VISUALIZAÇÃO DA RESISTÊNCIA (PAREDES) ---
    // Cor Laranja para diferenciar do atrito
    glColor3f(1.0f, 0.5f, 0.0f);

    char strText[32];
    for (const auto &w : breakableWalls_3D)
    {
        if (w.isBroken)
            continue;

        // Calcula o centro do topo da parede
        float cx = w.x + w.w / 2.0f;
        float cy = w.y + w.h + 0.5f;
        float cz = w.z + w.d / 2.0f;

        // Formata: "Str: 400"
        sprintf(strText, "Str: %.0f", w.strength);
        drawText3D(cx, cy, cz, strText);
    }

    // Reabilita iluminação para o resto da cena (se necessário)
    glEnable(GL_LIGHTING);

    // ... (Continua para drawDebugVectors e HUD) ...

    // --- 4. VETORES FÍSICOS (Debug) ---
    drawDebugVectors();

    // --- 5. HUD (Overlay 2D) ---
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, w, h, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // --- LÓGICA DE TELAS E HUD ---
    if (isGameVictory_3D)
    {
        drawVictoryScreen(w, h);
    }
    else if (isGameOver_3D)
    {
        drawGameOverScreen(w, h);
    }
    else
    {
        // Jogo Rodando: Mostra o HUD

        // 1. Barra de Carga
        if (isChargingHook_3D)
        {
            drawChargeBar(w, h); // Assumindo que você tem essa função ou o código inline
        }

        // 2. Painel de Física (Velocidade, Forças) -> AQUI!
        drawPhysicsDebugHUD_3D();

        // 3. Contador de Tiros -> AQUI!
        drawShotCounterHUD_3D();

        // --- 4. MIRA (CROSSHAIR) ---
        float centerX = w / 2.0f;
        float centerY = h / 2.0f;

        float pitchDiff = aimPitch_3D - cameraPitch_3D;

        // --- CORREÇÃO MATEMÁTICA ---
        // 1. Converter para Radianos
        float pitchDiffRad = pitchDiff * M_PI / 180.0f;

        // 2. Usar o mesmo FOV definido no gameReshape (45 graus)
        float fovDeg = 45.0f;
        float fovRad = (fovDeg / 2.0f) * M_PI / 180.0f;

        // 3. Calcular a posição Y usando Tangente (Geometria real da projeção)
        // Se a mira estiver indo para o lado oposto, inverta o sinal antes do (tan...
        float screenHalfHeight = h / 2.0f;
        float crossY = centerY + screenHalfHeight * (tan(pitchDiffRad) / tan(fovRad));

        // Desenha a cruz (Verde)
        glColor3f(0.0f, 1.0f, 0.0f);
        glLineWidth(2.0f);
        float size = 10.0f;
        glBegin(GL_LINES);
        // Linha Horizontal
        glVertex2f(centerX - size, crossY);
        glVertex2f(centerX + size, crossY);
        // Linha Vertical
        glVertex2f(centerX, crossY - size);
        glVertex2f(centerX, crossY + size);
        glEnd();
    }

    // Restaura estado OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/**
 * Chamada quando a janela é redimensionada.
 */
void gameReshape_3D(int w, int h)
{
    // Previne divisão por zero se a janela for minimizada ou muito pequena
    if (h == 0)
        h = 1;

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
    gluPerspective(45.0f, (float)w / h, 0.1f, 2000.0f);

    // 3. Volta para ModelView (Onde movemos objetos e câmera)
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Chamada quando o mouse se move (ativo ou passivo).
 */
void gameMouseMotion_3D(int x, int y)
{

    lastMouseX_3D = x;
    lastMouseY_3D = y;

    if (isMouseFree_3D)
        return;

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    int cx = w / 2, cy = h / 2;

    if (!mouseInitialized_3D)
    {
        // CORREÇÃO: Mesmo sendo o primeiro frame, garantimos que ele está no centro
        // e marcamos como inicializado.
        mouseInitialized_3D = true;
        glutWarpPointer(cx, cy);
        return;
    }

    float dx = (float)(x - cx);
    float dy = (float)(cy - y);

    if (dx == 0 && dy == 0)
        return;

    // 1. Yaw (Horizontal)
    cameraYaw_3D -= dx * MOUSE_SENSITIVITY_3D;

    // 2. Pitch (Vertical) - MIRA PRIMEIRO
    aimPitch_3D -= dy * MOUSE_SENSITIVITY_3D;

    // ---------------------------------------------------------
    // PASSO A: Calcular onde a CÂMERA deve ficar (Limites Físicos)
    // ---------------------------------------------------------

    float targetCameraPitch = aimPitch_3D;

    // Cálculo Dinâmico do Limite Inferior (Não ver através do chão)
    float margin = 2.0f;
    float heightDiff = (FLOOR_Y + margin) - player_3D.y;
    float ratio = heightDiff / cameraDistance;

    if (ratio < -1.0f)
        ratio = -1.0f;
    if (ratio > 1.0f)
        ratio = 1.0f;

    float dynamicMinPitch = asin(ratio) * 180.0f / M_PI;

    // Aplica os limites físicos na CÂMERA ALVO
    if (targetCameraPitch < dynamicMinPitch)
    {
        targetCameraPitch = dynamicMinPitch;
    }
    if (targetCameraPitch > MAX_PITCH_3D)
    {
        targetCameraPitch = MAX_PITCH_3D;
    }

    // ---------------------------------------------------------
    // PASSO B: Trava de Tela (Restringir a MIRA em relação à CÂMERA)
    // ---------------------------------------------------------
    // Agora que sabemos onde a câmera VAI ficar (targetCameraPitch),
    // garantimos que a mira não se afaste demais dela.

    const float PIXELS_PER_DEGREE = 15.0f;
    float maxAngleDiff = ((h / 2.0f) - 20.0f) / PIXELS_PER_DEGREE;

    // Se a mira subiu demais em relação à câmera travada
    if (aimPitch_3D < targetCameraPitch - maxAngleDiff)
    {
        aimPitch_3D = targetCameraPitch - maxAngleDiff;
    }

    // Se a mira desceu demais
    if (aimPitch_3D > targetCameraPitch + maxAngleDiff)
    {
        aimPitch_3D = targetCameraPitch + maxAngleDiff;
    }

    // ---------------------------------------------------------
    // PASSO C: Finalizar
    // ---------------------------------------------------------

    // Atualiza a câmera real com o valor calculado e travado
    cameraPitch_3D = targetCameraPitch;

    // Limites absolutos de segurança para a mira (evita inversão 360)
    if (aimPitch_3D < -89.0f)
        aimPitch_3D = -89.0f;
    if (aimPitch_3D > 89.0f)
        aimPitch_3D = 89.0f;

    glutWarpPointer(cx, cy);
}

/**
 * Chamada quando um botão do mouse é clicado.
 */
void gameMouseClick_3D(int button, int state)
{
    if (isGameOver_3D || isGameVictory_3D)
        return;

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            // Inicia carregamento
            if (!isHooked_3D && !isHookFiring_3D && shotsRemaining_3D > 0)
            {
                isChargingHook_3D = true;
                chargeStartTime_3D = glutGet(GLUT_ELAPSED_TIME);
                currentChargeForce_3D = 0.0f;
                chargePercentage_3D = 0.0f;
                printf("MOUSE DOWN: Carregando...\n");
            }
            else
            {
                printf("MOUSE DOWN: Bloqueado (Preso=%d, Voando=%d, Tiros=%d)\n",
                       isHooked_3D, isHookFiring_3D, shotsRemaining_3D);
            }
        }
        else if (state == GLUT_UP)
        {
            // Finaliza e solicita disparo
            if (isChargingHook_3D)
            {
                isChargingHook_3D = false; // Importante: parar de carregar
                requestHookRelease_3D = true;
                printf("MOUSE UP: Solicitando disparo!\n");
            }
        }
    }
}

/**
 * Chamada quando uma tecla é pressionada.
 */
GameAction gameKeyDown_3D(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    case 'W':
        isPressingForward_3D = true;
        break;
    case 'a':
    case 'A':
        isPressingLeft_3D = true;
        break;
    case 's':
    case 'S':
        isPressingBackward_3D = true;
        break;
    case 'd':
    case 'D':
        isPressingRight_3D = true;
        break;

    // --- ESPAÇO: Soltar ou Cancelar Gancho ---
    case 32:
        // Se estiver PRESO (Hooked) OU VOANDO (Firing/Travado)
        if (isHooked_3D || isHookFiring_3D)
        {
            isHooked_3D = false;
            isHookFiring_3D = false; // Força o cancelamento do estado "Voando"
            isGrounded_3D = false;

            // Limpa vetores visuais
            forceTensionX_3D = 0;
            forceTensionY_3D = 0;
            forceTensionZ_3D = 0;

            printf("COMANDO: Gancho cancelado/solto pelo Espaco!\n");
        }

        if (isChargingHook_3D)
        {
            isChargingHook_3D = false;
            chargePercentage_3D = 0.0f;
            printf("COMANDO: Carregamento cancelado.\n");
        }
        break;

    case 'm':
    case 'M':
        isMouseFree_3D = !isMouseFree_3D;
        if (isMouseFree_3D)
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        else
        {
            glutSetCursor(GLUT_CURSOR_NONE);
            int w = glutGet(GLUT_WINDOW_WIDTH);
            int h = glutGet(GLUT_WINDOW_HEIGHT);
            glutWarpPointer(w / 2, h / 2);
            mouseInitialized_3D = false;
        }
        break;

    case 'q':
    case 'Q':
        isMouseFree_3D = true;
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        return GAME_ACTION_EXIT_TO_MENU;
    }
    return GAME_ACTION_CONTINUE;
}

/**
 * Chamada quando uma tecla é solta.
 */
void gameKeyUp_3D(unsigned char key, int x, int y)
{
    switch (key)
    {
    // Quando solta a tecla, paramos o movimento naquela direção
    case 'w':
    case 'W':
        isPressingForward_3D = false;
        break;
    case 'a':
    case 'A':
        isPressingLeft_3D = false;
        break;
    case 's':
    case 'S':
        isPressingBackward_3D = false;
        break;
    case 'd':
    case 'D':
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
void loadGameTextures_3D()
{
    printf("loadGameTextures_3D() chamado. (Nao ha texturas para carregar)\n");
}