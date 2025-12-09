#include "collision.h"
#include <algorithm>

Collision::Collision(const RenderShapeData &shape)
    : type(shape.primitive.type)
{
    center = shape.ctm[3];
    height = {
        glm::length(glm::vec3{shape.ctm[0]}),
        glm::length(glm::vec3{shape.ctm[1]}),
        glm::length(glm::vec3{shape.ctm[2]})
    };
    radius = height[0] * 0.5f;

    if (type == PrimitiveType::PRIMITIVE_MESH) {
        auto x_cmp = [](const auto& a, const auto& b) {
            return a.pos.x < b.pos.x;
        };
        auto y_cmp = [](const auto& a, const auto& b) {
            return a.pos.y < b.pos.y;
        };
        auto z_cmp = [](const auto& a, const auto& b) {
            return a.pos.z < b.pos.z;
        };

        float x_min = std::min_element(shape.vertexData.begin(), shape.vertexData.end(), x_cmp)->pos.x;
        float x_max = std::max_element(shape.vertexData.begin(), shape.vertexData.end(), x_cmp)->pos.x;

        float y_min = std::min_element(shape.vertexData.begin(), shape.vertexData.end(), y_cmp)->pos.y;
        float y_max = std::max_element(shape.vertexData.begin(), shape.vertexData.end(), y_cmp)->pos.y;

        float z_min = std::min_element(shape.vertexData.begin(), shape.vertexData.end(), z_cmp)->pos.z;
        float z_max = std::max_element(shape.vertexData.begin(), shape.vertexData.end(), z_cmp)->pos.z;

        box = Box{
            shape.ctm * glm::vec4{x_min, y_min, z_min, 1.f},
            shape.ctm * glm::vec4{x_max, y_max, z_max, 1.f}
        };
    }
}

bool Collision::detect(const Collision& collider) {
    // sphere-sphere
    if (type == PrimitiveType::PRIMITIVE_SPHERE && collider.type == PrimitiveType::PRIMITIVE_SPHERE) {
        if (glm::distance(center, collider.center) < (radius + collider.radius)) {
            return true;
        }
    }

    // sphere-box
    if (type == PrimitiveType::PRIMITIVE_SPHERE && collider.type == PrimitiveType::PRIMITIVE_MESH) {
        return sphereBox(*this, collider);
    }

    // box-sphere
    if (type == PrimitiveType::PRIMITIVE_MESH && collider.type == PrimitiveType::PRIMITIVE_SPHERE) {
        return sphereBox(collider, *this);
    }

    return false;
}

bool Collision::sphereBox(const Collision& sphere, const Collision& box) {
    // TODO: sphere-box


    return false;
}
