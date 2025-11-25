#ifndef GAME_STRUCTS_H // Guarda de inclusão (impede erros de loop)
#define GAME_STRUCTS_H

#include <GL/freeglut.h> // Necessário para GLuint
#include <vector>        // Necessário se você for declarar vetores aqui, se não, pode remover
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =========================================================
// 1. ESTRUTURAS GRÁFICAS (Renderização / VBOs)
// =========================================================


// Um vértice completo no formato: pos(3) + cor(4) + tex(2) + normal(3)
struct vert {
    GLfloat pos[3];    // x, y, z
    GLfloat cor[4];    // r, g, b, a
    GLfloat tex[2];    // s, t
    GLfloat normal[3]; // nx, ny, nz
};

struct tri {
    vert v[3];
};

struct VBO_Info {
    GLuint vboID;
    int triCount;
};

// =========================================================
// 2. ESTRUTURAS DE LÓGICA DO JOGO (3D)
// =========================================================

struct GameObject_3D {
    float x, y, z;
    float w, h, d;
    float r, g, b;
    float velocityX, velocityY, velocityZ;
};

struct Platform_3D {
    float x, y, z;
    float w, h, d;
    float r, g, b;
    bool isHookable;
    float frictionCoefficient;
};

struct WindZone_3D {
    float x, y, z;
    float w, h, d;
    float accelX, accelY, accelZ;
};

struct BreakableWall_3D {
    float x, y, z;
    float w, h, d;
    float r, g, b;
    float strength;
    bool isBroken;
};

struct SpikeZone_3D {
    float x, y, z;
    float w, h, d;
    float r, g, b;
};

struct RiverZone_3D { // Supondo que exista, baseado no seu código anterior
    float x, y, z;
    float w, h, d;
    // Adicione outras propriedades se necessário
};

// =========================================================
// 3. ENUMS E ESTADOS
// =========================================================

enum GameAction {
    GAME_ACTION_CONTINUE,
    GAME_ACTION_EXIT_TO_MENU,
    GAME_ACTION_LEVEL_WON,
    GAME_ACTION_LEVEL_LOST
};

// =========================================================
// 4.  ESTRUTURAS DE LÓGICA DO JOGO (2D)
// =========================================================

/**
 * Todos os elementos do jogo (correntes de vento, espinhos, jogador, etc) são
 * modularizados para garantir que sempre terão os mesmos comportamentos e sigam
 * uma estrutura padrão. Os valores passíveis de mudanças são variáveis
 * controladas pela lógica do game ou pela fase em questão
 */
struct GameObject {
  float x, y, w, h;
  float r, g, b;
  float velocityX, velocityY;
};

struct Platform {
  float x, y, w, h;
  float r, g, b;
  bool isHookable;
  float frictionCoefficient;  // Coeficiente de atrito, responsável pela
                              // aceleração ou desaceleração do personagem
};

struct WindZone {
  float x, y, w, h;
  float accelX, accelY;  // Aceleração que o vento aplica ao jogador
};

struct BreakableWall {
  float x, y, w, h;
  float r, g, b;
  float strength;  // Resistência da parede
  bool isBroken;
};

struct SpikeZone {
  float x, y, w, h;
  float r, g, b;
};

// =========================================================
// 5. MATEMÁTICA E VETORES
// =========================================================

struct Vector_3D {
    float x, y, z;

    // Construtor padrão (inicia com 0)
    Vector_3D() : x(0), y(0), z(0) {}

    // Construtor com valores
    Vector_3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    // --- Operadores para facilitar a Física ---

    // Soma de vetores (v1 + v2)
    Vector_3D operator+(const Vector_3D& other) const {
        return Vector_3D(x + other.x, y + other.y, z + other.z);
    }

    // Subtração de vetores (v1 - v2)
    Vector_3D operator-(const Vector_3D& other) const {
        return Vector_3D(x - other.x, y - other.y, z - other.z);
    }

    // Multiplicação por escalar (v1 * 2.0f) - Útil para F = m * a
    Vector_3D operator*(float scalar) const {
        return Vector_3D(x * scalar, y * scalar, z * scalar);
    }

    // Retorna a magnitude (tamanho) do vetor
    float magnitude() const {
        return sqrt(x*x + y*y + z*z);
    }

    // Normaliza o vetor (retorna vetor unitário com mesma direção)
    Vector_3D normalized() const {
        float mag = magnitude();
        if (mag > 0.0001f) {
            return Vector_3D(x / mag, y / mag, z / mag);
        }
        return Vector_3D(0, 0, 0);
    }
};

#endif // Fim do GAME_STRUCTS_H