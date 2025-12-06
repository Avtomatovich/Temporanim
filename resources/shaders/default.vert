#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

out vec3 fs_Pos;
out vec3 fs_Normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
    vec4 worldPos = modelMatrix * vec4(inPos, 1.0);
    fs_Pos = worldPos.xyz;

    // Correct normal transform
    mat3 normalMat = transpose(inverse(mat3(modelMatrix)));
    fs_Normal = normalize(normalMat * inNormal);

    gl_Position = projMatrix * viewMatrix * worldPos;
}
