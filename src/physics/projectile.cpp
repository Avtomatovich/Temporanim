#include "projectile.h"

Projectile::Projectile() {
    // tomato
    SceneMaterial tomato_mtl;
    ScenePrimitive tomato {PrimitiveType::PRIMITIVE_SPHERE};
    projectiles.push_back(tomato);

    // cabbage
    ScenePrimitive cabbage {PrimitiveType::PRIMITIVE_SPHERE};
    projectiles.push_back(cabbage);

    // carrot
    ScenePrimitive carrot {PrimitiveType::PRIMITIVE_CONE};
    projectiles.push_back(carrot);

    // apple
    ScenePrimitive apple {PrimitiveType::PRIMITIVE_SPHERE};
    projectiles.push_back(apple);

    // onion
    ScenePrimitive onion {PrimitiveType::PRIMITIVE_SPHERE};
    projectiles.push_back(onion);
}
