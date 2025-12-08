#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <assimp/matrix4x4.inl>
#include <glm/glm.hpp>

namespace Transform
{
    glm::mat4 translate(const glm::vec3& v);
    glm::mat4 rotate(float t, const glm::vec3& v);

    glm::mat4 toGlmMat(const aiMatrix4x4& mat);

    glm::mat3 computeCubeInertia(float M, const glm::vec3& dim);
    glm::mat3 computeSphereInertia(float M, float r);
    glm::mat3 computeCylinderInertia(float M, float r, float h);
    glm::mat3 computeConeInertia(float M, float r, float h);
}

#endif // TRANSFORM_H
