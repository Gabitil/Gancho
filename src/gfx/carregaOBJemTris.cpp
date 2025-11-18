#include <iostream>
#include <vector>
#include <string>
#include "tiny_obj_loader.h"
#include "mesh_utils.h"

GLuint vboModelo = 0;
std::vector<tri> modelo;

void carregaOBJemTris(const std::string& caminho, std::vector<tri>& out) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    std::string baseDir = "models/"; // ajustar conforme necessário

    bool ret = tinyobj::LoadObj(
        &attrib, &shapes, &materials,
        &warn, &err,
        caminho.c_str(),
        baseDir.c_str()
    );


    if (!warn.empty()) std::cout << "Warn: " << warn << "\n";
    if (!err.empty())  std::cout << "Err: "  << err  << "\n";

    if (!ret) {
        std::cerr << "Falha ao carregar OBJ!\n";
        exit(1);
    }

    // Para cada shape (submesh)
    for (const auto& shape : shapes) {
        // O OBJ já vem triangulado pelo tinyobjloader
        size_t numFaces = shape.mesh.indices.size() / 3;

        for (size_t f = 0; f < numFaces; f++) {
            tri T;

            for (int v = 0; v < 3; v++) {
                tinyobj::index_t idx = shape.mesh.indices[3*f + v];

                // POSIÇÃO
                T.v[v].pos[0] = attrib.vertices[3 * idx.vertex_index + 0];
                T.v[v].pos[1] = attrib.vertices[3 * idx.vertex_index + 1];
                T.v[v].pos[2] = attrib.vertices[3 * idx.vertex_index + 2];

                // NORMAL
                if (idx.normal_index >= 0) {
                    T.v[v].normal[0] = attrib.normals[3 * idx.normal_index + 0];
                    T.v[v].normal[1] = attrib.normals[3 * idx.normal_index + 1];
                    T.v[v].normal[2] = attrib.normals[3 * idx.normal_index + 2];
                } else {
                    // normal padrão
                    T.v[v].normal[0] = 0;
                    T.v[v].normal[1] = 1;
                    T.v[v].normal[2] = 0;
                }

                // TEXTURA
                if (idx.texcoord_index >= 0) {
                    T.v[v].tex[0] = attrib.texcoords[2 * idx.texcoord_index + 0];
                    T.v[v].tex[1] = attrib.texcoords[2 * idx.texcoord_index + 1];
                } else {
                    T.v[v].tex[0] = 0;
                    T.v[v].tex[1] = 0;
                }

                // COR (pode mudar depois)
                T.v[v].cor[0] = 1.0f;
                T.v[v].cor[1] = 1.0f;
                T.v[v].cor[2] = 1.0f;
                T.v[v].cor[3] = 1.0f;
            }

            out.push_back(T);
        }
    }

    std::cout << "Modelo carregado: " << out.size() << " triângulos.\n";
}