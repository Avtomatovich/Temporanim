#include "projectile.h"

Projectile::Projectile(const RenderData& metaData)
    : m_shapes(metaData.shapes)
{
    idx = std::uniform_int_distribution<int>{0, static_cast<int>(m_shapes.size() - 1)};
}

RenderShapeData Projectile::spawn() {
    return m_shapes.at(idx(gen));
}
