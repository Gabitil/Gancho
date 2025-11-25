// mesh_utils.h

#ifndef MESH_UTILS_H
#define MESH_UTILS_H

// Tipos padrão necessários para o glew.h (GLint64, etc)
#include <cstddef>
#include <cstdint>

#include <GL/glew.h>
#include "game_structs.h"

/// Cria um VBO a partir de um array de triângulos.
/// Retorna o id do buffer (GLuint).
GLuint criaVBODeTris(int quantosTris, const tri* triangulos);

/// Desenha triângulos diretamente da RAM (vertex array "puro", sem VBO).
void desenhaTrisComVertexArrays(int quantosTris, const tri* triangulos);

/// Desenha triângulos usando VBO já criado.
void desenhaTrisComVBO(GLuint vbo, int quantosTris);

tri* createBoxTris(float x, float y, float z, float w, float h, float d, float r, float g, float b, int& outTriCount);

#endif
