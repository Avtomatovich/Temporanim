#include "mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertexData,
           const std::vector<unsigned int>& indexes) :
    m_vertexData(vertexData),
    m_indexes(indexes)
{}

const std::vector<Vertex>& Mesh::getVertices() const {
    return m_vertexData;
}

const std::vector<unsigned int>& Mesh::getIndexes() const {
    return m_indexes;
}
