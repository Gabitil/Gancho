#ifndef DECORATIONS_H
#define DECORATIONS_H

#include <vector>

// Estrutura para objetos decorativos
struct DecoObject {
  float x, y, z;
  float scale;
  float rotationY;
};

// Declarações das funções
void loadLevel1Decorations(std::vector<DecoObject>& hexagons_3D,
                          std::vector<DecoObject>& flowers_3D,
                          std::vector<DecoObject>& plants_3D,
                          std::vector<DecoObject>& rocks_3D,
                          std::vector<DecoObject>& pines_3D,
                          std::vector<DecoObject>& trees_3D);

void loadLevel2Decorations(std::vector<DecoObject>& hexagons_3D,
                          std::vector<DecoObject>& flowers_3D,
                          std::vector<DecoObject>& plants_3D,
                          std::vector<DecoObject>& rocks_3D,
                          std::vector<DecoObject>& pines_3D,
                          std::vector<DecoObject>& trees_3D);

void loadLevel3Decorations(std::vector<DecoObject>& hexagons_3D,
                          std::vector<DecoObject>& flowers_3D,
                          std::vector<DecoObject>& plants_3D,
                          std::vector<DecoObject>& rocks_3D,
                          std::vector<DecoObject>& pines_3D,
                          std::vector<DecoObject>& trees_3D);

#endif // DECORATIONS_H
