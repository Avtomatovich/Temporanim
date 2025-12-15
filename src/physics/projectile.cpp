#include "projectile.h"

Projectile::Projectile(const RenderData& metaData)
    : m_shapes(metaData.shapes)
{
    gen = std::default_random_engine(rd());
    idx = std::uniform_int_distribution<int>{0, static_cast<int>(m_shapes.size() - 1)};
}

const std::vector<RenderShapeData>& Projectile::getShapes() const {
    return m_shapes;
}

RenderShapeData Projectile::spawn() {
    return m_shapes.at(idx(gen));
}
