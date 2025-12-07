#include "sceneparser.h"
#include "scenefilereader.h"
#include "modelparser.h"
#include <glm/gtx/transform.hpp>

void buildRenderData(RenderData& renderData, SceneNode* node, glm::mat4 ctm) {
    if (node == nullptr) return;

    for (SceneTransformation* transform : node->transformations) {
        switch (transform->type) {
            case TransformationType::TRANSFORMATION_TRANSLATE:
                ctm *= glm::translate(transform->translate);
                break;
            case TransformationType::TRANSFORMATION_SCALE:
                ctm *= glm::scale(transform->scale);
                break;
            case TransformationType::TRANSFORMATION_ROTATE:
                ctm *= glm::rotate(transform->angle, transform->rotate);
                break;
            case TransformationType::TRANSFORMATION_MATRIX:
                ctm *= transform->matrix;
                break;
        }
    }

    if (node->children.empty()) {
        for (ScenePrimitive* primitive : node->primitives) {
            if (primitive->type == PrimitiveType::PRIMITIVE_MESH) {
                ModelParser::meshParse(renderData, primitive, ctm);
            } else {
                renderData.shapes.push_back(RenderShapeData{*primitive, ctm, glm::inverse(ctm)});
            }
        }

        for (SceneLight* light : node->lights) {
            SceneLightData lightData{light->id, light->type, light->color};

            switch (light->type) {
                case LightType::LIGHT_POINT:
                    // attenuation, position
                    lightData.function = light->function;
                    lightData.pos = ctm * glm::vec4{0.f, 0.f, 0.f, 1.f};

                    lightData.dir = glm::vec4{0.f};
                    lightData.penumbra = 0.f;
                    lightData.angle = 0.f;
                    break;
                case LightType::LIGHT_DIRECTIONAL:
                    // direction
                    lightData.dir = ctm * light->dir;

                    lightData.function = glm::vec3{0.f};
                    lightData.pos = glm::vec4{0.f};
                    lightData.penumbra = 0.f;
                    lightData.angle = 0.f;
                    break;
                case LightType::LIGHT_SPOT:
                    // attenuation, position, direction, penumbra, angle
                    lightData.function = light->function;
                    lightData.pos = ctm * glm::vec4{0.f, 0.f, 0.f, 1.f};
                    lightData.dir = ctm * light->dir;
                    lightData.penumbra = light->penumbra;
                    lightData.angle = light->angle;
                    break;
            }

            renderData.lights.push_back(lightData);
        }

        return;
    }

    for (SceneNode* child : node->children) {
        buildRenderData(renderData, child, ctm);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    renderData.lights.clear();
    renderData.shapes.clear();
    renderData.animData.clear();

    buildRenderData(renderData, fileReader.getRootNode(), glm::mat4{1.f});

    return true;
}
