#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include "utils/sceneparser.h"

class Mesh
{
public:
    Mesh(std::vector<Vertex>& vertexData,
         const std::vector<unsigned int>& indexes);

    const std::vector<Vertex>& getVertices() const;

    const std::vector<unsigned int>& getIndexes() const;

private:
    const std::vector<Vertex>& m_vertexData;
    const std::vector<unsigned int>& m_indexes;
};

#endif // MESH_H
