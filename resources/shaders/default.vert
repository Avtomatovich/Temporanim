#version 330 core

layout(location = 0) in vec3 objPos;
layout(location = 1) in vec3 objNorm;
layout(location = 2) in vec2 objUV;
layout(location = 3) in vec3 objTang;
layout(location = 4) in vec3 objBitang;
layout(location = 5) in ivec4 boneIDs;
layout(location = 6) in vec4 weights;

const int MAX_BONES = 128;
const int MAX_WEIGHTS = 4;

out vec3 worldPos;
out vec3 worldNorm;
out vec2 UV;
out vec3 worldTang;
out vec3 worldBitang;

uniform mat4 model;
uniform mat3 modelInvT;

uniform mat4 view;
uniform mat4 proj;

uniform float repeatU;
uniform float repeatV;

uniform mat4 skinMats[MAX_BONES];
uniform mat3 skinMatsT[MAX_BONES];

uniform bool hasBones;

void processBones() {
    vec4 initPos = vec4(0.0);
    vec3 initNorm = vec3(0.0);
    vec3 initTang = vec3(0.0);
    vec3 initBitang = vec3(0.0);
    float weightSum = 0.0;

    // For each bone ID + weight
    for (int i = 0; i < MAX_WEIGHTS; ++i) {
        // Skip invalid bones + weights
        if (boneIDs[i] < 0 || weights[i] <= 0.0) continue;

        // If bone is in bone matrix array range
        if (boneIDs[i] < MAX_BONES) {
            // Add weighted skinning transform to total position
            initPos += skinMats[boneIDs[i]] * vec4(objPos, 1.0) * weights[i];
            // Add weighted skinning transpose to total normal
            initNorm += skinMatsT[boneIDs[i]] * objNorm * weights[i];
            // Add weighted skinning transpose to total tangent
            initTang += skinMatsT[boneIDs[i]] * objTang * weights[i];
            // Add weighted skinning transpose to total bitangent
            initBitang += skinMatsT[boneIDs[i]] * objBitang * weights[i];
            // Add up weights
            weightSum += weights[i];
        } else {
            // Reset vals if out of range
            initPos = vec4(objPos, 1.0);
            initNorm = objNorm;
            initTang = objTang;
            initBitang = objBitang;
            weightSum = 1.0;
            break;
        }
    }

    // Normalize results
    if (weightSum > 0.0) {
        initPos /= weightSum;
        initNorm /= weightSum;
        initTang /= weightSum;
        initBitang /= weightSum;
    }

    worldPos = vec3(model * initPos);

    worldNorm = normalize(modelInvT * initNorm);

    // normal map math
    worldTang = normalize(modelInvT * initTang);

    worldBitang = normalize(modelInvT * initBitang);
}

void main() {
    if (hasBones) processBones();
    else {
        worldPos = vec3(model * vec4(objPos, 1.0));

        worldNorm = normalize(modelInvT * objNorm);

        // normal map math
        worldTang = normalize(modelInvT * objTang);

        worldBitang = normalize(modelInvT * objBitang);
    } 

    UV.x = objUV.x * repeatU;
    UV.y = objUV.y * repeatV;

    gl_Position = proj * view * vec4(worldPos, 1.0);
}
