#ifndef UNILOADER_H
#define UNILOADER_H

#include <GL/glew.h>
#include "geometry/model.h"
#include "sceneparser.h"
#include "camera/camera.h"
#include "texture/texture.h"

namespace UniLoader
{
    void passGlobalVars(GLuint shader, const SceneGlobalData& global);

    void passCamVars(GLuint shader, const Camera& cam);

    void passLightVars(GLuint shader, const SceneLightData& light, int idx, bool clean);

    void passShapeVars(GLuint shader, const RenderShapeData& shape);

    void passTextureVars(GLuint shader, const Texture& texture);

    void passBoneVars(GLuint shader, const Model& model);
}

#endif // UNILOADER_H
