// mesh_utils.h

#ifndef MESH_UTILS_H
#define MESH_UTILS_H

// Tipos padrão necessários para o glew.h (GLint64, etc)
#include <cstddef>
#include <cstdint>

#include <GL/glew.h>

// Um vértice completo no formato: pos(3) + cor(4) + tex(2) + normal(3)
struct vert {
    GLfloat pos[3];    // x, y, z
    GLfloat cor[4];    // r, g, b, a
    GLfloat tex[2];    // s, t
    GLfloat normal[3]; // nx, ny, nz
};

// Um triângulo = 3 vértices
struct tri {
    vert v[3];
};

/// Cria um VBO a partir de um array de triângulos.
/// Retorna o id do buffer (GLuint).
GLuint criaVBODeTris(int quantosTris, const tri* triangulos);

/// Desenha triângulos diretamente da RAM (vertex array "puro", sem VBO).
void desenhaTrisComVertexArrays(int quantosTris, const tri* triangulos);

/// Desenha triângulos usando VBO já criado.
void desenhaTrisComVBO(GLuint vbo, int quantosTris);

#endif
