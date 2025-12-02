#include <iostream>
#include <string>
#include <vector>

#include "mesh_utils.h"
#include "tiny_obj_loader.h"

GLuint vboModelo = 0;
std::vector<tri> modelo;

/**
 * @brief Carrega um arquivo OBJ e converte suas faces em triângulos (estrutura
 * tri).
 *
 * Esta função usa tinyobjloader para ler um arquivo .obj especificado por
 * 'caminho' e preenche o vetor de saída 'out' com triângulos (tipo tri), um por
 * face triangulada. Ela processa posições, normais, coordenadas de textura e
 * atribui uma cor padrão por vértice.
 *
 * Comportamento detalhado:
 * - Invoca tinyobj::LoadObj para popular tinyobj::attrib_t, shapes e materials.
 * - Usa "models/" como diretório base para resolução de materiais/texturas
 * (pode ser ajustado).
 * - Para cada shape, percorre os índices agrupados em conjuntos de 3
 * (triângulos) e constrói um tri:
 *   - Preenche v[].pos a partir de attrib.vertices (x,y,z).
 *   - Se houver normal_index válido (>= 0), preenche v[].normal a partir de
 * attrib.normals; caso contrário atribui a normal padrão (0, 1, 0).
 *   - Se houver texcoord_index válido (>= 0), preenche v[].tex a partir de
 * attrib.texcoords (u,v); caso contrário usa (0,0).
 *   - Define v[].cor como branca (1.0f, 1.0f, 1.0f, 1.0f).
 * - Adiciona cada tri construído ao vetor 'out' (usando push_back).
 * - Exibe avisos (warn) e erros (err) via std::cout / std::cerr.
 * - Em caso de falha no carregamento (ret == false) chama exit(1), terminando o
 * processo.
 * - Ao final imprime o número total de triângulos carregados.
 *
 * Parâmetros:
 * @param caminho  Caminho do arquivo .obj a ser carregado (std::string).
 * @param out      Vetor de saída (std::vector<tri>&) que receberá os
 * triângulos. Observação: a função acrescenta elementos a 'out'; ela NÃO limpa
 * 'out' antes.
 *
 * Efeitos colaterais / observações:
 * - A função usa saída padrão para mensagens e encerra o processo em erro
 * crítico.
 * - Pressupõe a existência da struct 'tri' com campos v[].pos, v[].normal,
 * v[].tex, v[].cor.
 * - Índices em tinyobj podem ser negativos quando não há dado correspondente; a
 * função já trata normais/texcoords ausentes com valores padrão.
 * - Para melhor desempenho em grandes modelos, recomenda-se reservar espaço em
 * 'out' antes da chamada (out.reserve(...)).
 *
 * Recomendações:
 * - Ajustar baseDir conforme necessário para que materiais/texture paths sejam
 * resolvidos.
 * - Validar a existência do arquivo externamente se não desejar que a função
 * finalize o programa.
 */
void carregaOBJemTris(const std::string& caminho, std::vector<tri>& out) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn, err;

  // Obtém o diretório do arquivo OBJ para buscar texturas relativas
  size_t lastSlash = caminho.find_last_of("/\\");
  std::string objDir =
      (lastSlash != std::string::npos) ? caminho.substr(0, lastSlash + 1) : "";

  // Se for um modelo global, tenta buscar em assets/images/
  // Se for um modelo de level, tenta buscar em assets/images/levelX/
  std::string baseDir = "models/";
  if (objDir.find("global") != std::string::npos) {
    baseDir = "assets/images/";
  } else if (objDir.find("level1") != std::string::npos) {
    baseDir = "assets/images/level1/";
  } else if (objDir.find("level2") != std::string::npos) {
    baseDir = "assets/images/level2/";
  } else if (objDir.find("level3") != std::string::npos) {
    baseDir = "assets/images/level3/";
  }

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              caminho.c_str(), baseDir.c_str());

  if (!warn.empty()) std::cout << "Warn: " << warn << "\n";
  if (!err.empty()) std::cout << "Err: " << err << "\n";

  if (!ret) {
    std::cerr << "Falha ao carregar OBJ: " << caminho << "\n";
    exit(1);
  }

  // Para cada shape (submesh)
  for (const auto& shape : shapes) {
    // O OBJ já vem triangulado pelo tinyobjloader
    size_t numFaces = shape.mesh.indices.size() / 3;

    for (size_t f = 0; f < numFaces; f++) {
      tri T;

      for (int v = 0; v < 3; v++) {
        tinyobj::index_t idx = shape.mesh.indices[3 * f + v];

        // POSIÇÃO
        T.v[v].pos[0] = attrib.vertices[3 * idx.vertex_index + 0];
        T.v[v].pos[1] = attrib.vertices[3 * idx.vertex_index + 1];
        T.v[v].pos[2] = attrib.vertices[3 * idx.vertex_index + 2];

        // NORMAL (Invertida para corrigir modelos com normais invertidas)
        if (idx.normal_index >= 0) {
          T.v[v].normal[0] = -attrib.normals[3 * idx.normal_index + 0];
          T.v[v].normal[1] = -attrib.normals[3 * idx.normal_index + 1];
          T.v[v].normal[2] = -attrib.normals[3 * idx.normal_index + 2];
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

  std::cout << "Modelo carregado: " << out.size()
            << " triângulos (baseDir: " << baseDir << ").\n";
}