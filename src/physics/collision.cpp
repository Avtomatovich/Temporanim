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

    // construct bounding box
    updateBox(shape.ctm);
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
    radius = 0.5f * height.x;

    if (type == PrimitiveType::PRIMITIVE_MESH) {
        // NOTE: ignore rotation to maintain axis alignment
        glm::mat4 T = glm::translate(glm::mat4{1.f}, center);
        glm::mat4 S = glm::scale(glm::mat4{1.f}, height);
        
        // convert from object space to world space
        box = Box{
            T * S * glm::vec4{min, 1.f},
            T * S * glm::vec4{max, 1.f}
        };
    }
}

std::optional<Contact> Collision::detect(const Collision& that) const {
    // // sphere-sphere
    // if (this->type == PrimitiveType::PRIMITIVE_SPHERE && that.type == PrimitiveType::PRIMITIVE_SPHERE ||
    //     that.type == PrimitiveType::PRIMITIVE_SPHERE && this->type == PrimitiveType::PRIMITIVE_SPHERE) {
    //     return glm::distance(center, that.center) < (radius + that.radius);
    // }

    // box-box
    if (this->type == PrimitiveType::PRIMITIVE_MESH && that.type == PrimitiveType::PRIMITIVE_MESH) {
        return boxBox(this->getBox(), that.getBox());
    }
    if (that.type == PrimitiveType::PRIMITIVE_MESH && this->type == PrimitiveType::PRIMITIVE_MESH) {
        return boxBox(that.getBox(), this->getBox());
    }

    // cube-box
    if (this->type == PrimitiveType::PRIMITIVE_CUBE && that.type == PrimitiveType::PRIMITIVE_MESH) {
        // this = cube, that = box
        return cubeBox(*this, that.getBox());
    }
    if (that.type == PrimitiveType::PRIMITIVE_CUBE && this->type == PrimitiveType::PRIMITIVE_MESH) {
        // that = cube, this = box
        return cubeBox(that, this->getBox());
    }

    return std::nullopt;
}

std::optional<Contact> Collision::cubeBox(const Collision& cube, const Box& box) const {
    // NOTE: assume cube is axis-aligned
    Box cubeBox {
        cube.center - cube.height * 0.5f,
        cube.center + cube.height * 0.5f
    };

    return boxBox(cubeBox, box);
}

std::optional<Contact> Collision::boxBox(const Box& b0, const Box& b1) const {
    Contact contact;

    // Init collision axis
    int axis = 0;

    // Init overlap to max
    float init, overlap = std::numeric_limits<float>::max();

    // For each bounding box axis
    for (int i = 0; i < 3; ++i) {
        // Return null if no overlap
        if (b0.max[i] < b1.min[i] || b1.max[i] < b0.min[i]) {
            return std::nullopt;
        }

        // Set initial overlap value
        init = overlap;

        // Get minimum overlap value
        overlap = fmin(overlap, fmin(b0.max[i], b1.max[i]) - fmax(b0.min[i], b1.min[i]));

        // If overlap changed, set axis value
        if (init != overlap) axis = i;
    }

    // Set contact point to overlap midpoints
    contact.p = (glm::min(b0.max, b1.max) + glm::max(b0.min, b1.min)) * 0.5f;

    // Set normal direction to collision axis
    contact.n[axis] = b0.max[axis] < b1.max[axis] ? -1.f : 1.f;

    return contact;
}
