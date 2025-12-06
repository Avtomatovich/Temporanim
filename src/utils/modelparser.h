#ifndef MODELPARSER_H
#define MODELPARSER_H

#include <assimp/scene.h>
#include "utils/sceneparser.h"

namespace ModelParser {    

    void updateAnim(const aiScene* scene, AnimData& animData);

    void updateBoneHierarchy(const aiScene* scene, AnimData& animData);

    void updateTexture(aiMaterial* mtl, RenderShapeData& shape);

    void updateMaterial(aiMaterial* mtl, RenderShapeData& shape);

    void buildMeshData(RenderData& renderData,
                       const ScenePrimitive* primitive,
                       glm::mat4 ctm,
                       const aiScene* scene,
                       aiNode* node);

    void meshParse(RenderData& renderData, const ScenePrimitive* primitive, glm::mat4 ctm);

}

#endif // MODELPARSER_H
