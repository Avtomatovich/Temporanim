#include "model.h"

Model::Model(const std::string& meshfile) :
    m_meshfile(meshfile)
{}

void Model::clean() {
    for (auto& mesh : m_meshMap) {
        mesh.second.clean();
    }
}

const Geometry& Model::getGeom(int key) const {
    return m_meshMap.at(key);
}

const Box& Model::getBox() const {
    return m_box;
}

void Model::addMesh(RenderShapeData& shape) {
    if (!m_meshMap.contains(shape.id)) {
        m_meshMap.emplace(shape.id,
                          Geometry{std::make_unique<Mesh>(shape.vertexData,
                                                          shape.indexes)});
    }
}

void Model::buildBox(const std::vector<RenderShapeData>& shapes,
              const std::unordered_map<int, Collision>& collMap)
{
    m_box.min = glm::vec3{std::numeric_limits<float>::min()};
    m_box.max = glm::vec3{std::numeric_limits<float>::max()};

    // for each shape
    for (int i = 0; i < shapes.size(); ++i) {
        // if mesh is part of model
        if (m_meshfile == shapes[i].primitive.meshfile) {
            // fetch mesh AABB
            const Box& meshBox = collMap.at(i).getBox();
            // store min and max per dim
            for (int j = 0; j < 3; ++j) {
                m_box.min[j] = std::min(m_box.min[j], meshBox.min[j]);
                m_box.max[j] = std::max(m_box.max[j], meshBox.max[j]);
            }
        }
    }
}
