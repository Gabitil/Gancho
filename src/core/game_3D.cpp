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
        // A força de tração é currentPullForce * massa (na sua lógica antiga multiplicava por massa no display)
        // Se currentPullForce_3D já for força (Newton), tire a multiplicação.
        // Assumindo que no 3D definimos currentPullForce como aceleração ou força pura.
        // No 2D você fazia: currentPullForce * playerMass. Vamos manter a consistência.
        float tensionN = currentChargeForce_3D; // No update 3D, currentPullForce é derivado da carga
        sprintf(buffer, "T: %.2f N", tensionN);
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
    float barCY = h - 60.0f;
    float barW = 200.0f;
    float barH = 15.0f;

    // Fundo Cinza
    drawRect(barCX - barW / 2, barCY, barW, barH, 0.3f, 0.3f, 0.3f);

    // Preenchimento (Gradiente de Amarelo para Vermelho visualmente via lógica simples)
    // Amarelo (1,1,0) -> Vermelho (1,0,0) diminuindo o Green
    float fillW = barW * chargePercentage_3D;
    drawRect(barCX - barW / 2, barCY, fillW, barH, 1.0f, 1.0f - chargePercentage_3D, 0.0f);

    // Borda Branca (Opcional, para acabamento)
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(barCX - barW / 2, barCY);
    glVertex2f(barCX + barW / 2, barCY);
    glVertex2f(barCX + barW / 2, barCY + barH);
    glVertex2f(barCX - barW / 2, barCY + barH);
    glEnd();
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

        // Cálculo do Deslocamento da Mira
        // Se aimPitch > cameraPitch, a mira está "acima" da visão da câmera.
        // Na tela 2D (0 no topo), "acima" significa diminuir Y.
        // Multiplicador 10.0f define quantos pixels sobe por grau de diferença.
        float pitchDiff = aimPitch_3D - cameraPitch_3D;
        float offsetY = pitchDiff * 15.0f;

        float crossX = centerX;
        float crossY = centerY - offsetY;

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
 * Desenha os vetores de debug (Normal, Atrito, Peso, Velocidade).
 */
/**
 * Desenha os vetores de debug (Normal, Atrito, Peso, Velocidade).
 */
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
    // D. TENSÃO (T) - Amarelo
    // Se estiver pendurado no gancho
    // ---------------------------------------------------
    if (isHooked_3D)
    {
        Vector_3D vecT = {forceTensionX_3D, forceTensionY_3D, forceTensionZ_3D};
        drawVector_3D(center, vecT, scaleForce, 1.0f, 1.0f, 0.0f, "T");
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
        float dirY = sin(pitchRad);
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

    // AJUSTE 1: CÉU AZUL
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

    // --- Configuração da Câmera ---
    cameraYaw_3D = 0.0f;
    cameraPitch_3D = 20.0f;
    aimPitch_3D = 20.0f;

    // AJUSTE 2: Câmera mais afastada (De 25.0f para 40.0f)
    cameraDistance = 25.0f;

    // Resetar Física
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

    isGrounded_3D = true;
    isHooked_3D = false;
    isHookFiring_3D = false;
    isChargingHook_3D = false;

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
        levelParameters_3D.playerMass = 10.0f;
        levelParameters_3D.playerWalkAccel = 60.0f;
        levelParameters_3D.maxWalkSpeed = 15.0f;
        levelParameters_3D.maxPlayerSpeed = 30.0f;

        // AJUSTE 3: Escala visual (100N = 2x Altura do Player -> 0.04f)
        levelParameters_3D.vectorVisualScale = 0.04f;

        levelParameters_3D.gravity = 20.0f;

        platforms_3D.push_back({-WORLD_WIDTH_3D / 2, -1.0f, -WORLD_DEPTH_3D / 2, WORLD_WIDTH_3D, 1.0f, WORLD_DEPTH_3D, 0.4f, 0.4f, 0.4f, true, 0.8f});
        platforms_3D.push_back({-20.0f, 5.0f, 10.0f, 10.0f, 1.0f, 20.0f, 0.4f, 0.4f, 0.4f, true, 0.2f});
        breakableWalls_3D.push_back({10.0f, 1.0f, 0.0f, 1.0f, 5.0f, 10.0f, 0.6f, 0.3f, 0.0f, 50.0f, false});

        door_3D.x = 40.0f;
        door_3D.y = 1.0f;
        door_3D.z = 0.0f;
        door_3D.w = 1.0f;
        door_3D.h = 4.0f;
        door_3D.d = 1.0f;

        player_3D.x = 0.0f;
        player_3D.y = 3.0f;
        player_3D.z = 0.0f;
        break;

    case 2:
        levelParameters_3D.playerMass = 8.0f;
        levelParameters_3D.playerWalkAccel = 70.0f;
        // Mantém a escala visual consistente entre fases
        levelParameters_3D.vectorVisualScale = 0.04f;

        platforms_3D.push_back({-WORLD_WIDTH_3D / 2, -1.0f, -WORLD_DEPTH_3D / 2, WORLD_WIDTH_3D, 1.0f, WORLD_DEPTH_3D, 0.2f, 0.2f, 0.5f, true, 0.7f});

        player_3D.x = 0.0f;
        player_3D.y = 3.0f;
        player_3D.z = 0.0f;
        break;

    default:
        gameStartLevel_3D(1);
        return;
    }

    if (!platforms_3D.empty())
        collidingPlatform_3D = &platforms_3D[0];
    else
        collidingPlatform_3D = NULL;

    createWorldDisplayLists();
    createLevelVBOs();
}

GameAction gameUpdate_3D()
{
    // --- 1. Controle de Vitória e Derrota ---
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

    // --- 2. Cálculo do Delta Time ---
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (float)(currentTime - lastTime_3D) / 1000.0f;
    lastTime_3D = currentTime;
    if (deltaTime > 0.1f)
        deltaTime = 0.1f;

    // --- 3. Inicialização de Forças ---
    float prevPositionPlayerX = player_3D.x;
    float prevPositionPlayerY = player_3D.y;
    float prevPositionPlayerZ = player_3D.z;

    // Reseta vetores visuais
    forceNormalX_3D = 0;
    forceNormalY_3D = 0;
    forceNormalZ_3D = 0;
    forceFrictionX_3D = 0;
    forceFrictionY_3D = 0;
    forceFrictionZ_3D = 0;
    forceTensionX_3D = 0;
    forceTensionY_3D = 0;
    forceTensionZ_3D = 0;

    // Gravidade
    float accelX = 0, accelY = -levelParameters_3D.gravity, accelZ = 0;

    float pCx = player_3D.x + player_3D.w / 2.0f;
    float pCy = player_3D.y + player_3D.h / 2.0f;
    float pCz = player_3D.z + player_3D.d / 2.0f;

    // --- 4. Zonas de Vento (Mantido igual) ---
    for (const auto &wind : windZones_3D)
    {
        if (isPointInsideBox(pCx, pCy, pCz, wind.x, wind.y, wind.z, wind.w, wind.h, wind.d))
        {
            accelX += wind.accelX;
            accelY += wind.accelY;
            accelZ += wind.accelZ;
        }
    }

    // --- 5. Física do Gancho (Puxada) (Mantido igual) ---
    if (isHooked_3D)
    {
        float vecToHookX = hookPointX_3D - pCx;
        float vecToHookY = hookPointY_3D - pCy;
        float vecToHookZ = hookPointZ_3D - pCz;
        float dist = sqrt(vecToHookX * vecToHookX + vecToHookY * vecToHookY + vecToHookZ * vecToHookZ);

        if (dist > 0.01f)
        {
            float normX = vecToHookX / dist;
            float normY = vecToHookY / dist;
            float normZ = vecToHookZ / dist;
            accelX += normX * currentPullForce_3D;
            accelY += normY * currentPullForce_3D;
            accelZ += normZ * currentPullForce_3D;
            forceTensionX_3D = normX * currentPullForce_3D;
            forceTensionY_3D = normY * currentPullForce_3D;
            forceTensionZ_3D = normZ * currentPullForce_3D;
        }
    }

    // =========================================================================
    // --- 6. Movimentação e Atrito (CORRIGIDO) ---
    // =========================================================================

    if (isGrounded_3D)
    {
        forceNormalY_3D = levelParameters_3D.gravity * levelParameters_3D.playerMass;

        // 1. Calcula Direção baseada na Câmera
        float yawRad = cameraYaw_3D * M_PI / 180.0f;
        float fwdX = -sin(yawRad);
        float fwdZ = -cos(yawRad);
        float rightX = cos(yawRad);
        float rightZ = -sin(yawRad);

        float walkForce = levelParameters_3D.playerWalkAccel;
        bool isMoving = false;

        // 2. Aplica Força de Movimento
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

        // 3. Atrito (Damping)
        // Se não estiver movendo (ou mesmo se estiver), o atrito tenta parar o player
        float frictionCoef = 0.8f;
        if (collidingPlatform_3D)
            frictionCoef = collidingPlatform_3D->frictionCoefficient;

        // A força de atrito é proporcional à velocidade
        // F_atrito = -V * coeficiente (simplificado para damping linear)
        float damping = 5.0f * frictionCoef;

        // Se soltou as teclas, damping muito maior para parar rápido
        if (!isMoving)
            damping *= 2.0f;

        player_3D.velocityX -= player_3D.velocityX * damping * deltaTime;
        player_3D.velocityZ -= player_3D.velocityZ * damping * deltaTime;

        // Salva vetor para visualização (oposto à velocidade)
        forceFrictionX_3D = -player_3D.velocityX * damping;
        forceFrictionZ_3D = -player_3D.velocityZ * damping;
    }

    // --- 7. Lógica de Carregamento e Disparo (Mantido igual) ---
    if (isChargingHook_3D)
    {
        float chargeTime = (glutGet(GLUT_ELAPSED_TIME) - chargeStartTime_3D) / 1000.0f;
        chargePercentage_3D = chargeTime / MAX_CHARGE_TIME_3D;
        if (chargePercentage_3D > 1.0f)
            chargePercentage_3D = 1.0f;
        currentChargeForce_3D = chargePercentage_3D * MAX_AIM_FORCE_DISPLAY_3D;
    }

    if (requestHookRelease_3D)
    {
        if (isHooked_3D)
        {
            isHooked_3D = false;
        }
        else if (isChargingHook_3D)
        {
            isChargingHook_3D = false;
            isHookFiring_3D = true;
            hookProjectileX_3D = pCx;
            hookProjectileY_3D = pCy;
            hookProjectileZ_3D = pCz;
            float yawRad = cameraYaw_3D * M_PI / 180.0f;
            float pitchRad = aimPitch_3D * M_PI / 180.0f;
            float dirX = -sin(yawRad) * cos(pitchRad);
            float dirY = sin(pitchRad);
            float dirZ = -cos(yawRad) * cos(pitchRad);
            float speed = levelParameters_3D.hookSpeed;
            hookProjectileVelX_3D = dirX * speed;
            hookProjectileVelY_3D = dirY * speed;
            hookProjectileVelZ_3D = dirZ * speed;
            currentPullForce_3D = currentChargeForce_3D;
        }
        requestHookRelease_3D = false;
    }

    // --- 8. Integração de Euler ---
    player_3D.velocityX += accelX * deltaTime;
    player_3D.velocityY += accelY * deltaTime;
    player_3D.velocityZ += accelZ * deltaTime;

    // Limite de Velocidade (XZ)
    if (isGrounded_3D)
    {
        float velXZ = sqrt(player_3D.velocityX * player_3D.velocityX + player_3D.velocityZ * player_3D.velocityZ);
        if (velXZ > levelParameters_3D.maxWalkSpeed)
        {
            float scale = levelParameters_3D.maxWalkSpeed / velXZ;
            player_3D.velocityX *= scale;
            player_3D.velocityZ *= scale;
        }
    }

    player_3D.x += player_3D.velocityX * deltaTime;
    player_3D.y += player_3D.velocityY * deltaTime;
    player_3D.z += player_3D.velocityZ * deltaTime;

    // --- 9. Colisões com Limites e Espinhos (Mantido igual) ---
    if (player_3D.y < DEATH_BOUNDARY_Y_3D)
    {
        isGameOver_3D = true;
        gameOverTimer_3D = 180;
        return GAME_ACTION_CONTINUE;
    }
    for (const auto &spike : spikeZones_3D)
    {
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d, spike.x, spike.y, spike.z, spike.w, spike.h, spike.d))
        {
            isGameOver_3D = true;
            gameOverTimer_3D = 180;
            return GAME_ACTION_CONTINUE;
        }
    }

    // --- 10. Paredes Quebráveis (Mantido igual) ---
    for (auto &wall : breakableWalls_3D)
    {
        if (wall.isBroken)
            continue;
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d, wall.x, wall.y, wall.z, wall.w, wall.h, wall.d))
        {
            float totalSpeed = sqrt(player_3D.velocityX * player_3D.velocityX + player_3D.velocityY * player_3D.velocityY + player_3D.velocityZ * player_3D.velocityZ);
            float impactForce = levelParameters_3D.playerMass * totalSpeed;
            if (impactForce >= wall.strength)
            {
                wall.isBroken = true;
                player_3D.velocityX *= 0.5f;
                player_3D.velocityZ *= 0.5f;
            }
            else
            {
                player_3D.x = prevPositionPlayerX;
                player_3D.z = prevPositionPlayerZ;
                player_3D.velocityX = 0;
                player_3D.velocityZ = 0;
            }
        }
    }

    // --- 11. Colisão com Plataformas (CORRIGIDO) ---
    isGrounded_3D = false;
    collidingPlatform_3D = NULL;
    float prevYApprox = player_3D.y - player_3D.velocityY * deltaTime;

    for (auto &p : platforms_3D)
    {
        if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d,
                               p.x, p.y, p.z, p.w, p.h, p.d))
        {

            // Pouso (Vindo de cima)
            // Verifica se a base do player estava acima do topo da plataforma no frame anterior
            if (prevYApprox >= p.y + p.h - 0.2f && player_3D.velocityY <= 0)
            {
                player_3D.y = p.y + p.h;
                player_3D.velocityY = 0;
                isGrounded_3D = true;
                collidingPlatform_3D = &p;
                if (isHooked_3D)
                    isHooked_3D = false;
            }
            // Cabeçada (Vindo de baixo)
            else if (prevYApprox + player_3D.h <= p.y + 0.2f && player_3D.velocityY > 0)
            {
                player_3D.y = p.y - player_3D.h;
                player_3D.velocityY = 0;
            }
        }
    }

    // --- 12. Restrição de Corda (Mantido igual) ---
    if (isHooked_3D)
    {
        float vecX = hookPointX_3D - pCx;
        float vecY = hookPointY_3D - pCy;
        float vecZ = hookPointZ_3D - pCz;
        float currDist = sqrt(vecX * vecX + vecY * vecY + vecZ * vecZ);
        if (currDist > ropeLength_3D)
        {
            float stretch = currDist - ropeLength_3D;
            float nX = vecX / currDist;
            float nY = vecY / currDist;
            float nZ = vecZ / currDist;
            player_3D.x += nX * stretch;
            player_3D.y += nY * stretch;
            player_3D.z += nZ * stretch;
            float velAlongRope = player_3D.velocityX * nX + player_3D.velocityY * nY + player_3D.velocityZ * nZ;
            if (velAlongRope < 0)
            {
                player_3D.velocityX -= nX * velAlongRope;
                player_3D.velocityY -= nY * velAlongRope;
                player_3D.velocityZ -= nZ * velAlongRope;
            }
        }
        player_3D.velocityX *= levelParameters_3D.dampingFactor;
        player_3D.velocityY *= levelParameters_3D.dampingFactor;
        player_3D.velocityZ *= levelParameters_3D.dampingFactor;
    }

    // --- 13. Projétil do Gancho (Mantido igual) ---
    if (isHookFiring_3D)
    {
        float prevHX = hookProjectileX_3D;
        float prevHY = hookProjectileY_3D;
        float prevHZ = hookProjectileZ_3D;
        hookProjectileX_3D += hookProjectileVelX_3D * deltaTime;
        hookProjectileY_3D += hookProjectileVelY_3D * deltaTime;
        hookProjectileZ_3D += hookProjectileVelZ_3D * deltaTime;
        bool hit = false;
        for (auto &p : platforms_3D)
        {
            float hX, hY, hZ;
            if (p.isHookable && lineBoxIntersection(prevHX, prevHY, prevHZ, hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D, p, hX, hY, hZ))
            {
                isHookFiring_3D = false;
                isHooked_3D = true;
                hookPointX_3D = hX;
                hookPointY_3D = hY;
                hookPointZ_3D = hZ;
                float dx = hookPointX_3D - (player_3D.x + player_3D.w / 2);
                float dy = hookPointY_3D - (player_3D.y + player_3D.h / 2);
                float dz = hookPointZ_3D - (player_3D.z + player_3D.d / 2);
                ropeLength_3D = sqrt(dx * dx + dy * dy + dz * dz);
                isGrounded_3D = false;
                hit = true;
                break;
            }
        }
        float dist = sqrt(pow(hookProjectileX_3D - pCx, 2) + pow(hookProjectileY_3D - pCy, 2) + pow(hookProjectileZ_3D - pCz, 2));
        if (dist > VIEW_WIDTH_3D && !hit)
            isHookFiring_3D = false;
    }

    // --- 14. Vitória (Mantido igual) ---
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w, player_3D.h, player_3D.d, door_3D.x, door_3D.y, door_3D.z, door_3D.w, door_3D.h, door_3D.d))
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
        glDisable(GL_LIGHTING);
        glLineWidth(2.0f);
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES);
        glVertex3f(cx, cy, cz);
        if (isHookFiring_3D)
            glVertex3f(hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D);
        else
            glVertex3f(hookPointX_3D, hookPointY_3D, hookPointZ_3D);
        glEnd();
        glEnable(GL_LIGHTING);
    }

    // --- 4. VETORES FÍSICOS (Debug) ---
    // AQUI ESTAVA FALTANDO A CHAMADA PRINCIPAL!
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

        // Cálculo do Deslocamento
        // aimPitch_3D (Mira) vs cameraPitch_3D (Câmera travada)
        // Exemplo: Mira no céu (-60) - Câmera travada (-20) = -40 (Diferença)
        float pitchDiff = aimPitch_3D - cameraPitch_3D;
        
        // Sensibilidade visual (pixels por grau)
        float pixelsPerDegree = 12.0f; 
        
        // Como Y=0 é no topo da tela:
        // Se pitchDiff é negativo (olhando pra cima), queremos diminuir o Y.
        // Então somamos o número negativo.
        float crossY = centerY + (pitchDiff * pixelsPerDegree); 

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
    if (isMouseFree_3D) return;
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    int cx = w / 2, cy = h / 2;

    if (!mouseInitialized_3D) {
        mouseInitialized_3D = true;
        glutWarpPointer(cx, cy);
        return;
    }

    float dx = (float)(x - cx);
    float dy = (float)(cy - y);

    if (dx == 0 && dy == 0) return;

    // 1. Yaw (Horizontal)
    cameraYaw_3D -= dx * MOUSE_SENSITIVITY_3D;

    // 2. Pitch (Vertical) - MIRA (Aim)
    // Mouse Cima -> Valor Diminui (Negativo = Olhar pro Céu)
    aimPitch_3D -= dy * MOUSE_SENSITIVITY_3D;

    // Trava absoluta da mira (Pode olhar totalmente para cima ou para baixo)
    if (aimPitch_3D < -89.0f) aimPitch_3D = -89.0f; 
    if (aimPitch_3D > 89.0f)  aimPitch_3D = 89.0f;

    // 3. Pitch da CÂMERA (Segue a mira, mas com cinto de segurança)
    cameraPitch_3D = aimPitch_3D;

    // LIMITADOR INFERIOR (Olhar para o Céu -> Câmera desce)
    // Se o ângulo for menor que 5.0f (ou seja, indo para 0 ou negativo),
    // travamos em 5.0f para a câmera ficar sempre um pouco acima da linha da cintura.
    // Isso impede que ela entre no chão.
    if (cameraPitch_3D < MIN_PITCH_3D) { 
       cameraPitch_3D = MIN_PITCH_3D; 
    }

    glutWarpPointer(cx, cy);
}

/**
 * Chamada quando um botão do mouse é clicado.
 */
void gameMouseClick_3D(int button, int state)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        printf("Disparar gancho 3D!\n");
        // Lógica do gancho 3D
    }
}

/**
 * Chamada quando uma tecla é pressionada.
 */
GameAction gameKeyDown_3D(unsigned char key, int x, int y)
{
    switch (key)
    {
    // --- Movimentação (WASD) ---
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

    // --- Lógica para alternar o modo do Mouse ('M' ou TAB/ESC se preferir) ---
    case 'm':
    case 'M':
        isMouseFree_3D = !isMouseFree_3D; // Inverte o estado (Variável com sufixo _3D)

        if (isMouseFree_3D)
        {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW); // Mostra o mouse para clicar em menus
        }
        else
        {
            glutSetCursor(GLUT_CURSOR_NONE); // Esconde o mouse para jogar

            // Centraliza para evitar "pulo" da câmera ao voltar
            int w = glutGet(GLUT_WINDOW_WIDTH);
            int h = glutGet(GLUT_WINDOW_HEIGHT);
            glutWarpPointer(w / 2, h / 2);

            mouseInitialized_3D = false; // Reseta a lógica de inicialização do mouse
        }
        break;

    // --- Sair para o Menu ---
    case 'q':
    case 'Q':
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