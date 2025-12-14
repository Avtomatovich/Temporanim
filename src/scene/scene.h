#ifndef SCENE_H
#define SCENE_H

#include <unordered_map>
#include "animation/animator.h"
#include "camera/camera.h"
#include "geometry/model.h"
#include "geometry/geometry.h"
#include "physics/collision.h"
#include "physics/projectile.h"
#include "physics/rigidbody.h"
#include "texture/texture.h"
#include "utils/sceneparser.h"

class Scene
{
public:
    Scene() {}

    Scene(const RenderData& metaData,
          float aspectRatio,
          float near, float far,
          int param1, int param2);

    bool draw(GLuint shader);

    void clean();

    // tessellation funcs
    void retessellate(int param1, int param2);

    // camera funcs
    inline void moveCam(const glm::vec3& pos,
                        const glm::vec3& look,
                        const glm::vec3& up) { m_cam.setView(pos, look, up); }
    inline void resize(int w, int h) { m_cam.setAspectRatio(w * 1.f / h); }
    inline void updateProj(float near, float far) { m_cam.perspective(near, far); }

    // anim funcs
    void updateAnim(float dt);
    void playAnim();
    void swapAnim(bool isNext);

    // normal map func
    void toggleNormalMap();

    // phys funcs
    void updatePhys(float dt);

    inline void enableGravity(bool toggle) { m_gravityEnabled = toggle; }
    inline void enableRotation(bool toggle) { m_torqueEnabled = toggle; }
    inline void enableCollisions(bool toggle) { m_collisionsEnabled = toggle; }

    // projectile funcs
    void loadProjectiles(const Projectile& projectiles);

    void spawn();

    void despawn();

private:
    SceneGlobalData m_global;
    Camera m_cam;
    std::vector<RenderShapeData> m_shapes;
    std::vector<SceneLightData> m_lights;

    std::unordered_map<int, Geometry> m_primMap;
    std::unordered_map<std::string, Texture> m_texMap;
    std::unordered_map<std::string, Model> m_modelMap;
    std::unordered_map<std::string, Animator> m_animMap;
    std::unordered_map<int, RigidBody> m_physMap;
    std::unordered_map<int, Collision> m_collMap;

    std::unique_ptr<Projectile> m_projectiles;

    void initModelAndTex(const RenderShapeData& shape);
    void initPhys(const RenderShapeData& shape, int i);

    void addPrim(const RenderShapeData& shape, int param1, int param2);
    const Geometry& getGeom(const RenderShapeData& shape);
    int getGeomKey(const RenderShapeData& shape);

    bool m_normalMapToggled = true;

    bool m_gravityEnabled = false;
    bool m_torqueEnabled = false;
    bool m_collisionsEnabled = false;

    int m_projectileIdx;
    int m_numProjectiles = 0;

    constexpr static float strength = 2.f;

    // std::string findMeshfile(const std::string& query);
};

#endif // SCENE_H
