/**
 * Estávamos tendo problemas com as estruturas (structs) dos dois jogos pois a main
 * precisava, em alguns casos, manipular estruturas parecidas. Além disso, como o jogo 
 * 3D inclui o jogo 2D, aconteciam casos de repetir a mesma estrutura em duas partes distintas.
 * Para unificar esse desenvolvimento, pesquisamos uma forma de resolver e escolhemos modularizar todas 
 * as estruturas usadas nos dois games. Isso permitiu a separação e unificação quando necessário
 */

#ifndef GAME_STRUCTS_H
#define GAME_STRUCTS_H

#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


struct vert {
    GLfloat pos[3]; // x, y, z
    GLfloat cor[4]; // r, g, b, a
    GLfloat tex[2]; // s, t
    GLfloat normal[3]; // nx, ny, nz
};

struct tri {
    vert v[3];
};

struct VBO_Info {
    GLuint vboID;
    int triCount;
};

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
    bool isSolid;  // Se true, bloqueia passagem de todos os lados (como parede)
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

enum GameAction {
    GAME_ACTION_CONTINUE,
    GAME_ACTION_EXIT_TO_MENU,
    GAME_ACTION_LEVEL_WON,
    GAME_ACTION_LEVEL_LOST
};

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

struct WindZone {
  float x, y, w, h;
  float accelX, accelY;
};

struct BreakableWall {
  float x, y, w, h;
  float r, g, b;
  float strength;
  bool isBroken;
};

struct SpikeZone {
  float x, y, w, h;
  float r, g, b;
};

/**
 * Buscamos uma estrutura pronta de desenho de vetores para facilitar os cálculos matemáticos.
 * Isso funciona como uma classe de Java onde existem campos, construtores e funções definidas dentro 
 * da struct
 */
struct Vector_3D {
    float x, y, z;

    // Construtor padrão (inicia com 0)
    Vector_3D() : x(0), y(0), z(0) {}

    // Construtor com valores
    Vector_3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

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

#endif