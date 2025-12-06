#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>
#include <iostream>

static void dfsScene(SceneNode *node, glm::mat4 parentCTM, RenderData &renderData)
{
    if (!node) return;

    glm::mat4 local = glm::mat4(1.f);
    for (SceneTransformation *t : node->transformations) {
        switch (t->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            local *= glm::translate(t->translate);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            local *= glm::rotate(t->angle, t->rotate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            local *= glm::scale(t->scale);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            local *= t->matrix;
            break;
        }
    }

    glm::mat4 ctm = parentCTM * local;

    for (ScenePrimitive *p : node->primitives) {
        RenderShapeData rs;
        rs.primitive = *p;
        rs.ctm = ctm;
        renderData.shapes.push_back(rs);
    }

    for (SceneLight *L : node->lights) {
        SceneLightData ld;
        ld.id       = L->id;
        ld.type     = L->type;
        ld.color    = L->color;
        ld.function = L->function;
        ld.angle    = L->angle;
        ld.penumbra = L->penumbra;

        if (L->type == LightType::LIGHT_POINT ||
            L->type == LightType::LIGHT_SPOT) {
            ld.pos = ctm * glm::vec4(0,0,0,1);
        }

        if (L->type == LightType::LIGHT_DIRECTIONAL ||
            L->type == LightType::LIGHT_SPOT)
        {
            glm::mat3 R = glm::mat3(ctm);
            glm::vec3 worldDir = glm::normalize(R * glm::vec3(L->dir));
            ld.dir = glm::vec4(worldDir, 0.f);
        }

        renderData.lights.push_back(ld);
    }

    for (SceneNode *child : node->children) {
        dfsScene(child, ctm, renderData);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData)
{
    ScenefileReader reader(filepath);
    if (!reader.readJSON()) return false;

    renderData.shapes.clear();
    renderData.lights.clear();

    renderData.globalData = reader.getGlobalData();
    renderData.cameraData = reader.getCameraData();

    dfsScene(reader.getRootNode(), glm::mat4(1.f), renderData);
    return true;
}
