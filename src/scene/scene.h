#ifndef SCENE_H
#define SCENE_H

#include <unordered_map>
#include "camera/camera.h"
#include "geometry/model.h"
#include "geometry/geometry.h"
#include "texture/texture.h"
#include "utils/sceneparser.h"

class Scene
{
public:
    Scene();

    Scene(const RenderData& metaData,
          float aspectRatio,
          float near, float far,
          int param1, int param2);

    void clean();

    bool draw(GLuint shader);

    inline void moveCam(glm::vec3 pos, glm::vec3 look, glm::vec3 up) {
        m_cam.setView(pos, look, up);
    }

    inline void resize(int w, int h) {
        m_cam.setAspectRatio(w * 1.f / h);
    }

    inline void updateProjection(float near, float far) {
        m_cam.perspective(near, far);
    }

    void retessellate(int param1, int param2);

    void updateAnim(float dt);

    void toggleAnimPlayback();

    void toggleAnimSwap(bool isNext);

    void toggleNormalMap();

private:
    SceneGlobalData m_global;
    Camera m_cam;
    std::vector<RenderShapeData> m_shapes;
    std::vector<SceneLightData> m_lights;

    std::unordered_map<int, Geometry> m_primMap;
    std::unordered_map<std::string, Texture> m_texMap;
    std::unordered_map<std::string, Model> m_modelMap;

    void addPrim(const RenderShapeData& shape, int param1, int param2);
    const Geometry& getGeom(const RenderShapeData& shape);
    int getGeomKey(const RenderShapeData& shape);

    bool m_normalMapToggled = true;
};

#endif // SCENE_H
