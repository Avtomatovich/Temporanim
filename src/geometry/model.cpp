#include "model.h"

Model::Model(const std::string& meshfile,
             const AnimData& animData) :
    m_meshfile(meshfile),
    m_animator(animData)
{}

const Geometry& Model::getGeom(int key) const {
    return m_meshMap.at(key);
}

const std::vector<glm::mat4>& Model::getSkinMats() const {
    return m_animator.getSkinMats();
}

void Model::addMesh(RenderShapeData& shape) {
    if (!m_meshMap.contains(shape.id)) {
        m_meshMap.emplace(shape.id,
                          Geometry{std::make_unique<Mesh>(shape.vertexData,
                                                          shape.indexes)});
    }
}

bool Model::hasAnim() const {
    return m_animator.hasAnim();
}

void Model::playAnim() {
    m_animator.play();
}

void Model::updateAnim(float deltaTime) {
    m_animator.update(deltaTime);
}

void Model::swapAnim(bool isNext) {
    isNext ? m_animator.swapForward() : m_animator.swapBackward();
}

void Model::clean() {
    for (auto& mesh : m_meshMap) {
        mesh.second.clean();
    }
}
