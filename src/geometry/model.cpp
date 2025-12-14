#include "model.h"

Model::Model(const std::string& meshfile) :
    m_meshfile(meshfile)
{}

const std::string& Model::getMeshfile() const {
    return m_meshfile;
}

const Geometry& Model::getGeom(int key) const {
    return m_meshMap.at(key);
}

void Model::addMesh(RenderShapeData& shape) {
    if (!m_meshMap.contains(shape.id)) {
        m_meshMap.emplace(shape.id,
                          Geometry{std::make_unique<Mesh>(shape.vertexData,
                                                          shape.indexes)});
    }
}

void Model::clean() {
    for (auto& mesh : m_meshMap) {
        mesh.second.clean();
    }
}
