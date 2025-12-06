#ifndef MESH_H
#define MESH_H

#include "utils/sceneparser.h"
#include <glm/glm.hpp>

class Mesh
{
public:
    Mesh(std::vector<Vertex>& vertexData,
         const std::vector<unsigned int>& indexes);

    const std::vector<Vertex>& getVertices() const;

    const std::vector<unsigned int>& getIndexes() const;

private:
    std::vector<Vertex>& m_vertexData;
    const std::vector<unsigned int>& m_indexes;
};

#endif // MESH_H
