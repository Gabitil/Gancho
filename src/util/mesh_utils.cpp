// mesh_utils.cpp
#include <GL/glew.h>      // PRIMEIRO
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include "mesh_utils.h"
#include <cstdlib>  // malloc, free
#include <cstring>  // memcpy

// Layout: pos(3) + cor(4) + tex(2) + normal(3) = 12 floats por vértice
static const int FLOATS_POS    = 3;
static const int FLOATS_COR    = 4;
static const int FLOATS_TEX    = 2;
static const int FLOATS_NORMAL = 3;
static const int FLOATS_TOTAL  = FLOATS_POS + FLOATS_COR + FLOATS_TEX + FLOATS_NORMAL; // 12

// === Função interna: monta o "arrayzão" intercalado na RAM ===
// retorna um ponteiro que PRECISA de free() depois
static char* inicializaVertexArrayTriangulos(int quantosTris, const tri* triangulos) {
    int quantosFloatsPorVertice = FLOATS_TOTAL; // 12
    int quantosVertices         = quantosTris * 3;

    char* arrayzao = (char*) std::malloc(
        quantosVertices *
        quantosFloatsPorVertice *
        sizeof(GLfloat)
    );

    GLfloat* p = (GLfloat*) arrayzao;

    // para cada triângulo, colocar dados de seus 3 vértices
    for (int i = 0; i < quantosTris; i++, p += quantosFloatsPorVertice * 3) {
        const tri* t = &triangulos[i];

        // vértice 0
        std::memcpy(&p[0],                         t->v[0].pos,    sizeof(GLfloat) * 3);
        std::memcpy(&p[3],                         t->v[0].cor,    sizeof(GLfloat) * 4);
        std::memcpy(&p[7],                         t->v[0].tex,    sizeof(GLfloat) * 2);
        std::memcpy(&p[9],                         t->v[0].normal, sizeof(GLfloat) * 3);

        // vértice 1
        std::memcpy(&p[12],                        t->v[1].pos,    sizeof(GLfloat) * 3);
        std::memcpy(&p[15],                        t->v[1].cor,    sizeof(GLfloat) * 4);
        std::memcpy(&p[19],                        t->v[1].tex,    sizeof(GLfloat) * 2);
        std::memcpy(&p[21],                        t->v[1].normal, sizeof(GLfloat) * 3);

        // vértice 2
        std::memcpy(&p[24],                        t->v[2].pos,    sizeof(GLfloat) * 3);
        std::memcpy(&p[27],                        t->v[2].cor,    sizeof(GLfloat) * 4);
        std::memcpy(&p[31],                        t->v[2].tex,    sizeof(GLfloat) * 2);
        std::memcpy(&p[33],                        t->v[2].normal, sizeof(GLfloat) * 3);
    }

    return arrayzao;
}

// === Função interna: configura ponteiros de atributo pro layout intercalado ===
// Usa VBO já bound em GL_ARRAY_BUFFER
static void configuraPonteirosAtributosIntercalados() {
    GLsizei stride = sizeof(GLfloat) * FLOATS_TOTAL; // 12 floats por vértice

    // Como estamos usando VBO, o último parâmetro é um OFFSET em bytes
    const GLvoid* offsetPos   = (const GLvoid*)(0 * sizeof(GLfloat));  // 0
    const GLvoid* offsetColor = (const GLvoid*)(3 * sizeof(GLfloat));  // depois de 3 floats (pos)
    const GLvoid* offsetTex   = (const GLvoid*)(7 * sizeof(GLfloat));  // 3+4 = 7
    const GLvoid* offsetNorm  = (const GLvoid*)(9 * sizeof(GLfloat));  // 3+4+2 = 9

    glVertexPointer(3, GL_FLOAT, stride, offsetPos);
    glColorPointer (4, GL_FLOAT, stride, offsetColor);
    glTexCoordPointer(2, GL_FLOAT, stride, offsetTex);
    glNormalPointer(GL_FLOAT,  stride, offsetNorm);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
}

// === Implementação pública: desenhar direto da RAM (sem VBO) ===
void desenhaTrisComVertexArrays(int quantosTris, const tri* triangulos) {
    char* arrayzao = inicializaVertexArrayTriangulos(quantosTris, triangulos);
    const GLfloat* p = (const GLfloat*) arrayzao;

    int stride = sizeof(GLfloat) * FLOATS_TOTAL;

    glVertexPointer(3, GL_FLOAT, stride, p + 0);
    glColorPointer (4, GL_FLOAT, stride, p + 3);
    glTexCoordPointer(2, GL_FLOAT, stride, p + 7);
    glNormalPointer( GL_FLOAT, stride, p + 9);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    int quantosVertices = quantosTris * 3;
    glDrawArrays(GL_TRIANGLES, 0, quantosVertices);

    std::free(arrayzao);
}

// === Implementação pública: criar VBO a partir de tris ===
GLuint criaVBODeTris(int quantosTris, const tri* triangulos) {
    GLuint idBuffer = 0;
    glGenBuffers(1, &idBuffer);

    char* arrayzao = inicializaVertexArrayTriangulos(quantosTris, triangulos);

    int stride          = sizeof(GLfloat) * FLOATS_TOTAL;
    int quantosVertices = 3 * quantosTris;
    int tamanhoBuffer   = stride * quantosVertices;

    glBindBuffer(GL_ARRAY_BUFFER, idBuffer);
    glBufferData(GL_ARRAY_BUFFER, tamanhoBuffer, arrayzao, GL_STATIC_DRAW);

    std::free(arrayzao);

    // Desvincula se quiser
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return idBuffer;
}

// === Implementação pública: desenhar usando VBO já criado ===
void desenhaTrisComVBO(GLuint vbo, int quantosTris) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    configuraPonteirosAtributosIntercalados();

    int quantosVertices = quantosTris * 3;
    glDrawArrays(GL_TRIANGLES, 0, quantosVertices);

    // opcional: desligar estados / desbindar
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
