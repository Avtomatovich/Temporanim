#include "collision.h"
#include <algorithm>

Collision::Collision(const RenderShapeData& shape)
    : type(shape.primitive.type)
{
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

        // retain object space min, max
        min = {x_min, y_min, z_min};
        max = {x_max, y_max, z_max};
    }

    updateBox(shape.ctm);
}

const PrimitiveType Collision::getType() const {
    return type;
}

const Box& Collision::getBox() const {
    return box;
}

void Collision::scaleBox(float factor) {
    glm::vec3 center = (box.max + box.min) * 0.5f;
    glm::vec3 extents = (box.max - box.min) * 0.5f * factor;
    box.min = center - extents;
    box.max = center + extents;
}

void Collision::updateBox(const glm::mat4& ctm) {
    center = ctm[3];
    height = {
        glm::length(glm::vec3{ctm[0]}),
        glm::length(glm::vec3{ctm[1]}),
        glm::length(glm::vec3{ctm[2]})
    };
    // scale unit radius of 0.5f by ctm factor
    radius = height[0] * 0.5f;

    if (type == PrimitiveType::PRIMITIVE_MESH) {
        // NOTE: ignore rotation to maintain axis alignment
        glm::mat4 TS = glm::scale(glm::translate(glm::mat4{1.f}, center), height);
        
        // convert from object space to world space
        box = Box{
            TS * glm::vec4{min, 1.f},
            TS * glm::vec4{max, 1.f}
        };
    }
}

bool Collision::detect(const Collision& that) const {
    // sphere-sphere
    if (this->type == PrimitiveType::PRIMITIVE_SPHERE && that.type == PrimitiveType::PRIMITIVE_SPHERE ||
        that.type == PrimitiveType::PRIMITIVE_SPHERE && this->type == PrimitiveType::PRIMITIVE_SPHERE) {
        return glm::distance(center, that.center) < (radius + that.radius);
    }

    // box-box
    if (this->type == PrimitiveType::PRIMITIVE_MESH && that.type == PrimitiveType::PRIMITIVE_MESH) {
        return boxBox(this->getBox(), that.getBox());
    }
    if (that.type == PrimitiveType::PRIMITIVE_MESH && this->type == PrimitiveType::PRIMITIVE_MESH) {
        return boxBox(that.getBox(), this->getBox());
    }

    // cube-box
    if (this->type == PrimitiveType::PRIMITIVE_CUBE && that.type == PrimitiveType::PRIMITIVE_MESH) {
        return cubeBox(*this, that.getBox());
    }
    if (this->type == PrimitiveType::PRIMITIVE_MESH && that.type == PrimitiveType::PRIMITIVE_CUBE) {
        return cubeBox(that, this->getBox());
    }

    return false;
}

bool Collision::cubeBox(const Collision& cube, const Box& box) const {
    // NOTE: assume cube is axis-aligned
    Box cubeBox;

    for (int i = 0; i < 3; ++i) {
        cubeBox.min[i] = cube.center[i] - cube.height[i] / 2;
        cubeBox.max[i] = cube.center[i] + cube.height[i] / 2;
    }

    return boxBox(cubeBox, box);
}

bool Collision::boxBox(const Box& b0, const Box& b1) const {
    for (int i = 0; i < 3; ++i) {
        if (b0.max[i] < b1.min[i] || b1.max[i] < b0.min[i]) {
            return false;
        }
    }

    return true;
}
