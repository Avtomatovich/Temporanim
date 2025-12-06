#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <assimp/matrix4x4.inl>
#include <glm/glm.hpp>

namespace Transform
{
    glm::mat4 translate(const glm::vec3& v);
    glm::mat4 rotate(float t, const glm::vec3& v);

    aiMatrix4x4 getAiMat(const glm::mat4& mat);
    glm::mat4 getGlmMat(const aiMatrix4x4& mat);
}

#endif // TRANSFORM_H
