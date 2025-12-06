#pragma once

#include "scenedata.h"
#include <array>
#include <optional>
#include <unordered_map>
#include <glm/gtc/quaternion.hpp>

#define MAX_LIGHTS 8
#define MAX_BONES 128
#define MAX_WEIGHTS 4

// Struct which contains vertex data for indexed mesh rendering
struct Vertex {
    Vertex() {
        boneIDs.fill(-1);
        weights.fill(0.f);
    }

    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) :
        pos(p), norm(n), uv(t)
    {
        boneIDs.fill(-1);
        weights.fill(0.f);
    }

    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 u,
           glm::vec3 t, glm::vec3 b) :
        pos(p), norm(n), uv(u), tang(t), bitang(b)
    {
        boneIDs.fill(-1);
        weights.fill(0.f);
    }

    glm::vec3 pos{0.f};
    glm::vec3 norm{0.f};
    glm::vec2 uv{0.f};

    glm::vec3 tang{0.f};
    glm::vec3 bitang{0.f};

    std::array<int, MAX_WEIGHTS> boneIDs;
    std::array<float, MAX_WEIGHTS> weights;
};

// Struct which contains bone information
struct Bone {
    std::string name;
    glm::mat4 offset{1.f};
    
    int parent = -1;
    std::vector<int> children;
};

// Struct which contains data for a single keyframe
struct Keyframe {
    float time;
    TransformationType type;
    std::optional<glm::vec3> pos;
    std::optional<glm::quat> rot;
    std::optional<glm::vec3> scale;
};

// Struct which contains data for transforming a bone
struct BoneAnim {
    std::string boneName;
    std::vector<Keyframe> positions;
    std::vector<Keyframe> rotations;
    std::vector<Keyframe> scalings;
};

// Struct which contains data for a single animation sequence
struct Animation {
    float duration, ticksPerSec;
    std::vector<BoneAnim> boneAnims;
};

// Struct which contains all the data needed to animate a scene
struct AnimData {
    std::vector<Bone> skeleton;
    std::unordered_map<std::string, int> boneToIdx;
    std::vector<Animation> animations;
};

// Struct which contains data for a single primitive, to be used for rendering
struct RenderShapeData {
    ScenePrimitive primitive;
    glm::mat4 ctm; // the cumulative transformation matrix
    glm::mat3 ctmInv; // the 3x3 inverse of the cumulative transformation matrix

    int id; // mesh id
    std::vector<Vertex> vertexData; // mesh vertex data
    std::vector<unsigned int> indexes; // mesh indexes
};

// Struct which contains all the data needed to render a scene
struct RenderData {
    SceneGlobalData globalData;
    SceneCameraData cameraData;

    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;

    std::unordered_map<std::string, AnimData> animData;
};

class SceneParser {
public:
    // Parse the scene and store the results in renderData.
    // @param filepath    The path of the scene file to load.
    // @param renderData  On return, this will contain the metadata of the loaded scene.
    // @return            A boolean value indicating whether the parse was successful.
    static bool parse(std::string filepath, RenderData &renderData);
};
