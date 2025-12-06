#include "scene.h"
#include "primitive/cone.h"
#include "primitive/cube.h"
#include "primitive/cylinder.h"
#include "primitive/sphere.h"
#include "utils/uniloader.h"
#include "utils/debug.h"

using namespace Debug;
using namespace UniLoader;

Scene::Scene(const RenderData& metaData,
             float aspectRatio,
             float near, float far,
             int param1, int param2) :
    m_global(metaData.globalData),
    m_shapes(metaData.shapes),
    m_lights(metaData.lights)
{
    m_cam = Camera {
        metaData.cameraData.pos,
        metaData.cameraData.look,
        metaData.cameraData.up,
        aspectRatio,
        metaData.cameraData.heightAngle,
        near,
        far
    };

    for (RenderShapeData& shape : m_shapes) {
        const std::string& meshfile = shape.primitive.meshfile;

        // Add model to model map if not present
        if (!m_modelMap.contains(meshfile) && !meshfile.empty()) {
            m_modelMap.emplace(meshfile, Model{meshfile, metaData.animData.at(meshfile)});
        }

        // If shape is mesh
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_MESH) {
            // Add to model's mesh map
            m_modelMap.at(meshfile).addMesh(shape);
        } else {
            // Add primitive to geom map if not present
            if (!m_primMap.contains(getGeomKey(shape))) {
                addPrim(shape, param1, param2);
            }
        }

        // Add texture map to slot 0 if used
        if (shape.primitive.material.textureMap.isUsed) {
            const std::string& filename = shape.primitive.material.textureMap.filename;

            if (!m_texMap.contains(filename)) {
                m_texMap.emplace(filename, Texture{filename, 0});
            }
        }

        // Add normal map to slot 1 if used
        if (shape.primitive.material.bumpMap.isUsed) {
            const std::string& filename = shape.primitive.material.bumpMap.filename;

            if (!m_texMap.contains(filename)) {
                m_texMap.emplace(filename, Texture{filename, 1});
            }
        }
    }
}

void Scene::clean() {
    for (auto& prim : m_primMap) prim.second.clean();

    for (auto& model : m_modelMap) model.second.clean();

    for (auto& tex : m_texMap) tex.second.clean();
}

bool Scene::draw(GLuint shader) {
    glErrorCheck();

    passGlobalVars(shader, m_global);
    passCamVars(shader, m_cam);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (i >= m_lights.size()) {
            passLightVars(shader, SceneLightData{}, i, true);
        } else {
            passLightVars(shader, m_lights.at(i), i, false);
        }
    }

    for (const auto& shape : m_shapes) {
        const Geometry& geom = getGeom(shape);

        // Activate diffuse map slot if available
        if (shape.primitive.material.textureMap.isUsed) {
            const Texture& texture = m_texMap.at(shape.primitive.material.textureMap.filename);

            glActiveTexture(GL_TEXTURE0 + texture.getSlot());
            glBindTexture(GL_TEXTURE_2D, texture.getId());

            passTextureVars(shader, texture);
        }

        // Activate normal map slot if available
        if (shape.primitive.material.bumpMap.isUsed && m_normalMapToggled) {
            const Texture& texture = m_texMap.at(shape.primitive.material.bumpMap.filename);

            glActiveTexture(GL_TEXTURE0 + texture.getSlot());
            glBindTexture(GL_TEXTURE_2D, texture.getId());

            passTextureVars(shader, texture);
        }

        // pass ctms
        passShapeVars(shader, shape);

        if (m_modelMap.contains(shape.primitive.meshfile)) {
            Model& model = m_modelMap.at(shape.primitive.meshfile);

            if (model.hasAnim()) passBoneVars(shader, model);
        }

        geom.draw();

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glErrorCheck();

    return true;
}

void Scene::retessellate(int param1, int param2) {
    for (auto& prim : m_primMap) {
        prim.second.updateTesselParams(param1, param2);
    }
}

void Scene::updateAnim(float dt) {
    for (auto& model : m_modelMap) model.second.updateAnim(dt);
}

void Scene::toggleAnimPlayback() {
    for (auto& model : m_modelMap) model.second.playAnim();
}

void Scene::toggleAnimSwap(bool toNext) {
    for (auto& model : m_modelMap) {
        model.second.swapAnim(toNext);
    }
}

void Scene::toggleNormalMap() {
    m_normalMapToggled = !m_normalMapToggled;
}

void Scene::addPrim(const RenderShapeData& shape, int param1, int param2) {
    int key = getGeomKey(shape);
    switch(shape.primitive.type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            m_primMap.emplace(key, Geometry{std::make_unique<Cube>(param1)});
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            m_primMap.emplace(key, Geometry{std::make_unique<Cone>(param1, param2)});
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_primMap.emplace(key, Geometry{std::make_unique<Cylinder>(param1, param2)});
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            m_primMap.emplace(key, Geometry{std::make_unique<Sphere>(param1, param2)});
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            break;
    }
}

const Geometry& Scene::getGeom(const RenderShapeData& shape) {
    const std::string& meshfile = shape.primitive.meshfile;
    int key = getGeomKey(shape);

    return meshfile.empty() ? m_primMap.at(key) :
               m_modelMap.at(meshfile).getGeom(key);
}

int Scene::getGeomKey(const RenderShapeData& shape) {
    PrimitiveType type = shape.primitive.type;
    return type == PrimitiveType::PRIMITIVE_MESH ? shape.id :
               static_cast<int>(type);
}
