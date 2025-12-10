#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include "modelparser.h"

namespace fs = std::filesystem;

namespace ModelParser {

    static glm::mat4 toGlmMat(const aiMatrix4x4& mat) {
        return glm::mat4{
            mat.a1, mat.b1, mat.c1, mat.d1,
            mat.a2, mat.b2, mat.c2, mat.d2,
            mat.a3, mat.b3, mat.c3, mat.d3,
            mat.a4, mat.b4, mat.c4, mat.d4
        };
    }

    void updateAnim(const aiScene* scene, AnimData& animData) {
        if (!scene->HasAnimations()) return;

        // for each animation
        for (int i = 0; i < scene->mNumAnimations; ++i) {
            // Fetch animation data
            aiAnimation* src = scene->mAnimations[i];
            // Init animation struct
            Animation anim{
                static_cast<float>(src->mDuration),
                static_cast<float>(src->mTicksPerSecond)
            };

            // for each animation node
            for (int j = 0; j < src->mNumChannels; ++j) {
                // Fetch animation node
                aiNodeAnim* nodeAnim = src->mChannels[j];
                // Init bone animation struct
                BoneAnim boneAnim{ nodeAnim->mNodeName.C_Str() };

                int nPos = nodeAnim->mNumPositionKeys;
                int nRot = nodeAnim->mNumRotationKeys;
                int nScale = nodeAnim->mNumScalingKeys;
                int nKeys = std::max(nPos, std::max(nRot, nScale));

                for (int k = 0; k < nKeys; ++k) {
                    if (k < nPos) {
                        aiVectorKey pos = nodeAnim->mPositionKeys[k];
                        boneAnim.positions.push_back({
                            .time = static_cast<float>(pos.mTime),
                            .type = TransformationType::TRANSFORMATION_TRANSLATE,
                            .pos = glm::vec3{ pos.mValue.x, pos.mValue.y, pos.mValue.z }
                        });
                    }

                    if (k < nRot) {
                        aiQuatKey rot = nodeAnim->mRotationKeys[k];
                        boneAnim.rotations.push_back({
                            .time = static_cast<float>(rot.mTime),
                            .type = TransformationType::TRANSFORMATION_ROTATE,
                            .rot = glm::quat{ rot.mValue.w, rot.mValue.x, rot.mValue.y, rot.mValue.z }
                        });
                    }

                    if (k < nScale) {
                        aiVectorKey scale = nodeAnim->mScalingKeys[k];
                        boneAnim.scalings.push_back({
                            .time = static_cast<float>(scale.mTime),
                            .type = TransformationType::TRANSFORMATION_SCALE,
                            .scale = glm::vec3{ scale.mValue.x, scale.mValue.y, scale.mValue.z }
                        });
                    }
                }

                anim.boneAnims.push_back(boneAnim);
            }

            animData.animations.push_back(anim);
        }
    }

    void updateBoneHierarchy(const aiScene* scene, AnimData& animData) {
        auto& skeleton = animData.skeleton;
        const auto& boneToIdx = animData.boneToIdx;

        for (Bone& bone : skeleton) {
            // Fetch node that matches bone
            aiNode* boneNode = scene->mRootNode->FindNode(bone.name.c_str());
            if (!boneNode) continue;

            // Fetch parent node
            aiNode* parent = boneNode->mParent;
            if (!parent) continue;

            // Fetch parent node name
            const std::string& parentName = parent->mName.C_Str();

            // Update bone struct parent index
            if (boneToIdx.contains(parentName)) {
                skeleton.at(boneToIdx.at(bone.name)).parent = boneToIdx.at(parentName);
            }

            // Iterate through node children
            for (int i = 0; i < boneNode->mNumChildren; ++i) {
                // Fetch child node
                aiNode* child = boneNode->mChildren[i];
                if (!child) continue;

                // Fetch child node name
                const std::string& childName = child->mName.C_Str();

                // Update bone struct child indexes
                if (boneToIdx.contains(childName)) {
                    skeleton.at(boneToIdx.at(bone.name)).children.push_back(boneToIdx.at(childName));
                }
            }
        }
    }

    void updateTexture(aiMaterial* mtl, aiTextureType type, RenderShapeData& shape) {
        const std::string& meshfile = shape.primitive.meshfile;
        aiString filename;
        SceneFileMap& map = type == aiTextureType_DIFFUSE ?
                               shape.primitive.material.textureMap :
                               shape.primitive.material.bumpMap;

        // Fetch meshfile path (assume meshfile has its own dir)
        fs::path meshpath = fs::path(meshfile);

        // If file type is obj, and texture type is normal, set to height map
        if (meshpath.extension().string() == ".obj") {
            if (type == aiTextureType_NORMALS) type = aiTextureType_HEIGHT;
        }

        if (aiGetMaterialTexture(mtl, type, 0, &filename) == AI_SUCCESS) {
            std::cout << "Fetched texture file name: " << filename.C_Str() << std::endl;

            // Set blend to 1 if it equals zero
            if (shape.primitive.material.blend == 0.f) shape.primitive.material.blend = 1.f;

            // Get meshfile parent path
            fs::path parent = meshpath.parent_path();

            // Build possible texture file path
            fs::path filepath {parent / fs::path(filename.C_Str())};

            // Mark texture as used and assign absolute path if present
            if (fs::exists(filepath)) {
                map.isUsed = true;
                map.filename = filepath.string();
                return;
            }

            // Recurse over files in meshfile dir
            for (const auto& dir : fs::recursive_directory_iterator{parent}) {
                // Check if dir is regular file and matches filename
                if (dir.is_regular_file() && dir.path().filename().string() == filename.C_Str()) {
                    // Mark texture as used, assign absolute path, break
                    map.isUsed = true;
                    map.filename = dir.path().string();
                    break;
                }
            }

            // Throw exception if file is not found
            if (map.filename.empty()) {
                std::cerr << "Missing texture file: " << filename.C_Str() << std::endl;
                throw std::runtime_error("Failed to find texture file in mesh dir");
            }

        } else std::cerr << "Failed to load texture file path from mesh file: " << meshfile << std::endl;

        std::cout << std::endl;
    }

    void updateMaterial(aiMaterial* mtl, RenderShapeData& shape) {
        // Init material vars
        aiColor4D ambient, diffuse, specular;
        float shininess, blend;

        if (aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient) == AI_SUCCESS) {
            shape.primitive.material.cAmbient = {ambient.r, ambient.g, ambient.b, ambient.a};
            // std::cout << "ambient " << ambient.r << " " << ambient.g << " " << ambient.b << " " << ambient.a << std::endl;
        } else std::cerr << "Failed to load ambient color from mesh data" << std::endl;

        if (aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS) {
            shape.primitive.material.cDiffuse = {diffuse.r, diffuse.g, diffuse.b, diffuse.a};
            // std::cout << "diffuse " << diffuse.r << " " << diffuse.g << " " << diffuse.b << " " << diffuse.a << std::endl;
        } else std::cerr << "Failed to load diffuse color from mesh data" << std::endl;

        if (aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular) == AI_SUCCESS) {
            shape.primitive.material.cSpecular = {specular.r, specular.g, specular.b, specular.a};
            // std::cout << "specular " << specular.r << " " << specular.g << " " << specular.b << " " << specular.a << std::endl;
        } else std::cerr << "Failed to load specular color from mesh data" << std::endl;

        if (aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &shininess) == AI_SUCCESS) {
            shape.primitive.material.shininess = shininess;
            // std::cout << "shininess " << shininess << std::endl;
        } else std::cerr << "Failed to load shininess from mesh data" << std::endl;

        if (aiGetMaterialFloat(mtl, AI_MATKEY_OPACITY, &blend) == AI_SUCCESS) {
            shape.primitive.material.blend = blend;
            // std::cout << "blend " << blend << std::endl;
        } else std::cerr << "Failed to load diffuse blend factor from mesh data" << std::endl;

        // Update diffuse texture
        updateTexture(mtl, aiTextureType_DIFFUSE, shape);

        // Update normal map
        updateTexture(mtl, aiTextureType_NORMALS, shape);
    }

    void buildMeshData(RenderData& renderData,
                       const ScenePrimitive* primitive,
                       glm::mat4 ctm,
                       const aiScene* scene,
                       aiNode* node)
    {
        if (!node) return;

        // Recurse in postorder traversal
        for (int i = 0; i < node->mNumChildren; ++i) {
            buildMeshData(renderData, primitive, ctm, scene, node->mChildren[i]);
        }

        // Compose transformation
        ctm *= toGlmMat(node->mTransformation);

        // Fetch const ref to shape's meshfile
        const std::string& meshfile = primitive->meshfile;

        // Fetch list of bones mapped to meshfile
        auto& skeleton = renderData.animData[meshfile].skeleton;

        // Fetch map of bone names to bone ids
        auto& boneToIdx = renderData.animData[meshfile].boneToIdx;

        // Iterate through node's mesh indexes
        for (int i = 0; i < node->mNumMeshes; ++i) {
            // Fetch mesh index and instance
            int meshIdx = node->mMeshes[i];
            aiMesh* mesh = scene->mMeshes[meshIdx];

            // Create RenderShapeData instance
            RenderShapeData shape{*primitive, ctm, glm::inverse(ctm), meshIdx};

            // Exit if mesh lacks normals
            if (!mesh->HasNormals()) {
                throw std::runtime_error("Mesh is missing normals");
            }

            // // PARSE INDEXES
            for (int j = 0; j < mesh->mNumFaces; ++j) {
                aiFace face = mesh->mFaces[j];

                for (int k = 0; k < face.mNumIndices; ++k) {
                    shape.indexes.push_back(face.mIndices[k]);
                }
            }

            // // PARSE VERTICES
            for (int j = 0; j < mesh->mNumVertices; ++j) {
                aiVector3D v = mesh->mVertices[j];
                aiVector3D n = mesh->mNormals[j];

                // Fetch uv coords if they exist (other uvs are bump maps, etc)
                aiVector3D uv = mesh->HasTextureCoords(0) ?
                                    mesh->mTextureCoords[0][j] :
                                    aiVector3D{0.f};

                // Fetch tangents if they exist
                aiVector3D t = mesh->HasTangentsAndBitangents() ?
                                      mesh->mTangents[j] :
                                      aiVector3D{0.f};

                // Fetch bitangents if they exist
                aiVector3D b = mesh->HasTangentsAndBitangents() ?
                                      mesh->mBitangents[j] :
                                      aiVector3D{0.f};

                // Insert vertex
                shape.vertexData.push_back({
                    {v.x, v.y, v.z},
                    {n.x, n.y, n.z},
                    {uv.x, uv.y},
                    {t.x, t.y, t.z},
                    {b.x, b.y, b.z}
                });
            }

            // // PARSE MATERIALS
            // NOTE: always true unless removed by flags
            if (scene->HasMaterials()) {
                aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];

                aiString name;
                aiGetMaterialString(mtl, AI_MATKEY_NAME, &name);

                // Update material if not default
                if (std::string(name.C_Str()) != "DefaultMaterial") {
                    updateMaterial(mtl, shape);
                }
            }

            // // PARSE BONES
            if (mesh->HasBones()) {
                for (int j = 0; j < mesh->mNumBones; ++j) {
                    aiBone* bone = mesh->mBones[j];
                    const std::string& name = bone->mName.C_Str();

                    // If bone is not in boneToIdx
                    if (!boneToIdx.contains(name)) {
                        // Map bone name to idx
                        boneToIdx.emplace(name, skeleton.size());

                        // Add bone to skeleton
                        skeleton.push_back({
                            name,
                            toGlmMat(bone->mOffsetMatrix)
                        });
                    }

                    // Iterate over bone weights
                    for (int k = 0; k < bone->mNumWeights; ++k) {
                        // Fetch bone weight and mesh vertex
                        aiVertexWeight weight = bone->mWeights[k];
                        Vertex& vertex = shape.vertexData.at(weight.mVertexId);

                        // For each bone weight in vertex
                        for (int b = 0; b < vertex.boneIDs.max_size(); ++b) {
                            // If bone ID is not set
                            if (vertex.boneIDs[b] < 0) {
                                // Add current bone ID and weight to vertex
                                vertex.boneIDs[b] = boneToIdx.at(name);
                                vertex.weights[b] = weight.mWeight;
                                break;
                            }
                        }
                    }
                }
            }

            renderData.shapes.push_back(shape);
        }
    }

    void meshParse(RenderData& renderData, const ScenePrimitive* primitive, glm::mat4 ctm) {
        Assimp::Importer importer;

        // Omit cameras, lights, and colors
        importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS |
                                                            aiComponent_LIGHTS |
                                                            aiComponent_COLORS);

        const aiScene* scene = importer.ReadFile(primitive->meshfile.c_str(),
                                                 aiProcess_GenNormals |
                                                 aiProcess_RemoveComponent |
                                                 aiProcess_LimitBoneWeights |
                                                 aiProcess_Triangulate |
                                                 aiProcess_JoinIdenticalVertices |
                                                 aiProcess_SortByPType |
                                                 aiProcess_CalcTangentSpace);

        if (!scene) {
            throw std::runtime_error("Error parsing meshfile: " + std::string(importer.GetErrorString()));
        }

        buildMeshData(renderData, primitive, ctm, scene, scene->mRootNode);

        updateBoneHierarchy(scene, renderData.animData[primitive->meshfile]);

        updateAnim(scene, renderData.animData[primitive->meshfile]);
    }

}
