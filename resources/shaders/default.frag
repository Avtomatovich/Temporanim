#version 330 core
in vec3 fs_Pos;
in vec3 fs_Normal;
out vec4 fragColor;

uniform float k_a;
uniform float k_d;
uniform float k_s;
uniform float shininess;
uniform vec4 cAmbient;
uniform vec4 cDiffuse;
uniform vec4 cSpecular;
uniform vec4 cameraPos;

uniform int size;
uniform int lightType[8];
uniform vec4 lightPos[8];
uniform vec4 lightDirections[8];
uniform vec4 lightColors[8];
uniform vec3 function[8];
uniform float lightAngle[8];      // already radians!
uniform float lightPenumbra[8];   // already radians!

void main() {
    vec3 N = normalize(fs_Normal);
    vec3 V = normalize(cameraPos.xyz - fs_Pos);

    // Base ambient
    vec3 ambientResult = k_a * cAmbient.rgb;
    vec3 diffuseResult = vec3(0);
    vec3 specularResult = vec3(0);

    for (int i = 0; i < size; i++) {
        vec3 L;
        float attenuation = 1.0;

        // -------------------------------
        // DIRECTIONAL LIGHT
        // -------------------------------
        if (lightType[i] == 1) {
            L = normalize(-lightDirections[i].xyz);
            attenuation = 1.0;
        }
        // -------------------------------
        // POINT or SPOT LIGHT
        // -------------------------------
        else {
            vec3 lightVector = lightPos[i].xyz - fs_Pos;
            float dist = length(lightVector);
            L = normalize(lightVector);

            // Standard attenuation: a + b*d + c*d^2
            float a = function[i].x;
            float b = function[i].y;
            float c = function[i].z;
            attenuation = 1.0 / max(a + b*dist + c*dist*dist, 0.0001);

            // -------------------------------
            // SPOT LIGHT EXTRA INTENSITY FALL-OFF
            // -------------------------------
            if (lightType[i] == 2) {
                vec3 D = normalize(lightDirections[i].xyz);
                // theta = cos(angle from light center)
                float theta = dot(-L, D);

                // Important: viewer JSON gives:
                //   angle    = OUTER cone
                //   penumbra = INNER bright cone
                //
                // Convert to cosine:
                float outerCone = cos(lightAngle[i]);
                float innerCone = cos(lightPenumbra[i]);

                // Smooth falloff (0 outside outerCone, 1 inside innerCone)
                float t = clamp((theta - outerCone) / (innerCone - outerCone), 0.0, 1.0);
                attenuation *= t;
            }
        }

        // -------------------------------
        // PHONG: DIFFUSE
        // -------------------------------
        float NdotL = max(dot(N, L), 0.0);
        diffuseResult += attenuation * k_d * cDiffuse.rgb * lightColors[i].rgb * NdotL;

        // -------------------------------
        // PHONG: SPECULAR
        // Avoid pow(0,0) when shininess = 0
        // -------------------------------
        if (NdotL > 0.0 && shininess > 0.0) {
            vec3 H = normalize(L + V);
            float spec = pow(max(dot(N, H), 0.0), shininess);
            specularResult += attenuation * k_s * cSpecular.rgb * lightColors[i].rgb * spec;
        }
    }

    fragColor = vec4(ambientResult + diffuseResult + specularResult, 1.0);
}
