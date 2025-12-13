#include "projectile.h"

Projectile::Projectile(const RenderData& metaData)
    : m_shapes(metaData.shapes)
{
    // for (int i = 0; i < m_shapes.size(); ++i) {
    //     RenderShapeData& shape = m_shapes[i];

    //     const std::string& meshfile = shape.primitive.meshfile;

    //     // Add collision instance to collision map
    //     m_collMap.emplace(i, shape);

    //     // Add model to model map if not present
    //     if (!m_modelMap.contains(meshfile) && !meshfile.empty()) {
    //         m_modelMap.emplace(meshfile, meshfile);
    //     }

    //     // If shape is mesh
    //     if (shape.primitive.type == PrimitiveType::PRIMITIVE_MESH) {
    //         // Add to model's mesh map
    //         m_modelMap.at(meshfile).addMesh(shape);
    //     }

    //     // Throw exception if projectile is not marked as dynamic
    //     if (shape.primitive.isDynamic) {
    //         std::runtime_error("Projectile not marked as dynamic");
    //     }

    //     // Add rigid body to phys map with default mass of 1.f
    //     m_physMap.emplace(i, RigidBody{shape.primitive.type,
    //                                    1.f,
    //                                    shape.ctm,
    //                                    m_collMap.at(i).getBox()});

    //     // Add texture map to slot 0 if used
    //     if (shape.primitive.material.textureMap.isUsed) {
    //         const std::string& filename = shape.primitive.material.textureMap.filename;

    //         if (!m_texMap.contains(filename)) {
    //             m_texMap.emplace(filename, Texture{filename, 0});
    //         }
    //     }

    //     // Add normal map to slot 1 if used
    //     if (shape.primitive.material.bumpMap.isUsed) {
    //         const std::string& filename = shape.primitive.material.bumpMap.filename;

    //         if (!m_texMap.contains(filename)) {
    //             m_texMap.emplace(filename, Texture{filename, 1});
    //         }
    //     }
    // }

    idx = std::uniform_int_distribution<int>{0, static_cast<int>(m_shapes.size() - 1)};
}

RenderShapeData Projectile::spawn() {
    return m_shapes.at(idx(gen));
}
