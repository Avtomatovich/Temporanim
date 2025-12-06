#version 330 core

const int POINT = 0;
const int DIR = 1;
const int SPOT = 2;

struct Light {
    int type;

    vec4 color;
    vec3 function;

    vec3 pos;
    vec3 dir;

    float penumbra;
    float angle;
};
uniform Light lights[8];

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
    float blend;
};
uniform Material material;

in vec3 worldPos;
in vec3 worldNorm;
in vec2 UV;
in vec3 worldTang;
in vec3 worldBitang;

out vec4 fragColor;

uniform float ka;
uniform float kd;
uniform float ks;
uniform vec3 camPos;

uniform sampler2D tex;
uniform sampler2D normTex;

uniform bool hasNormMap;

float attenuate(vec3 c, float d) {
    return min(1.0, 1.0 / (c.x + d*c.y + d*d*c.z));
}

float angle(vec3 a, vec3 b) {
    return acos(dot(a, b) / (length(a) * length(b)));
}

vec4 lerper(vec4 a, vec4 b, float t) {
    return a + t * (b - a);
}

vec3 updateNorm(vec3 norm) {
    // compute TBN to convert from tangent space (normalize tangents + bitangents!!!)
    mat3 TBN = mat3(normalize(worldTang), normalize(worldBitang), norm);

    // sample normal map, scale [0, 1] -> [-1, 1], convert tangent -> world space
    return normalize(TBN * (texture(normTex, UV).rgb * 2.0 - 1.0));
}

vec4 point(Light light) {
    vec4 illum = vec4(0.0);

    // normalize normal
    vec3 norm = normalize(worldNorm);

    // update normal
    if (hasNormMap) norm = updateNorm(norm);

    // compute attenuation
    float f_att = attenuate(light.function, distance(light.pos, worldPos));

    // dot product of surface normal and surface-to-light direction
    float N_L = dot(norm, normalize(light.pos - worldPos));
    vec4 diffuse = kd * material.diffuse;

    // blend diffuse color with texture color
    diffuse = lerper(diffuse, texture(tex, UV), material.blend);

    illum += f_att * diffuse * light.color * clamp(N_L, 0, 1);

    // dot product of reflected light and surface-to-camera direction
    float R_E = clamp(dot(normalize(reflect(worldPos - light.pos, norm)), normalize(camPos - worldPos)), 0, 1);
    float specular = pow(R_E, material.shininess);

    // if pow is undefined, reset specular term
    if (R_E == 0.0 && material.shininess <= 0.0) {
        specular = 0.0;
    }

    illum += f_att * ks * light.color * material.specular * specular;

    return illum;
}

vec4 dir(Light light) {
    vec4 illum = vec4(0.0);

    // normalize normal
    vec3 norm = normalize(worldNorm);

    // update normal
    if (hasNormMap) norm = updateNorm(norm);
    
    // dot product of surface normal and surface-to-light direction
    float N_L = dot(norm, normalize(-light.dir));
    vec4 diffuse = kd * material.diffuse;

    // blend diffuse color with texture color
    diffuse = lerper(diffuse, texture(tex, UV), material.blend);

    illum += diffuse * light.color * clamp(N_L, 0, 1);

    // dot product of reflected light and surface-to-camera direction
    float R_E = clamp(dot(normalize(reflect(light.dir, norm)), normalize(camPos - worldPos)), 0, 1);
    float specular = pow(R_E, material.shininess);

    // if pow is undefined, reset specular term
    if (R_E == 0.0 && material.shininess <= 0.0) {
        specular = 0.0;
    }

    illum += ks * light.color * material.specular * specular;

    return illum;
}

vec4 spot(Light light) {
    vec4 illum = vec4(0.0);

    float currAngle = angle(normalize(light.dir), normalize(worldPos - light.pos));

    if (currAngle > light.angle) return illum;

    illum = point(light);

    float innerAngle = light.angle - light.penumbra;

    if (innerAngle < currAngle && currAngle < light.angle) {
        float ratio = (currAngle - innerAngle) / (light.angle - innerAngle);
        float falloff = -2.0 * ratio*ratio*ratio + 3.0 * ratio*ratio;

        illum *= 1.0 - falloff;
    }

    return illum;
}

void main() {
    fragColor = ka * material.ambient;

    for (int i = 0; i < 8; ++i) {
        Light light = lights[i];

        if (light.type == POINT) {
            fragColor += point(light);
        } else if (light.type == DIR) {
            fragColor += dir(light);
        } else if (light.type == SPOT) {
            fragColor += spot(light);
        }
    }

    fragColor = clamp(fragColor, 0.0, 1.0);
    // fragColor = vec4(abs(normalize(worldNorm)), 1.0);
}
