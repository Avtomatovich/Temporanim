#ifndef BOX_H
#define BOX_H

#include <glm/glm.hpp>

struct Box {
    glm::vec3 min{0.f};
    glm::vec3 max{0.f};

    glm::vec3 side() const {
        return max - min;
    }
};

#endif // BOX_H
