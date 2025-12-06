#include <stdexcept>
#include <iostream>
#include "uniloader.h"
#include "geometry/model.h"

namespace UniLoader
{

    void passGlobalVars(GLuint shader, const SceneGlobalData& global) {
        GLint ka = glGetUniformLocation(shader, "ka");
        GLint kd = glGetUniformLocation(shader, "kd");
        GLint ks = glGetUniformLocation(shader, "ks");

        if (ka == -1 || kd == -1 || ks == -1) {
            throw std::invalid_argument("Missing global uniform variables");
        }

        glUniform1f(ka, global.ka);
        glUniform1f(kd, global.kd);
        glUniform1f(ks, global.ks);
    }


    void passCamVars(GLuint shader, const Camera& cam) {
        GLint view = glGetUniformLocation(shader, "view");
        GLint proj = glGetUniformLocation(shader, "proj");
        GLint camPos = glGetUniformLocation(shader, "camPos");

        if (view == -1 || proj == -1 || camPos == -1) {
            throw std::invalid_argument("Missing camera uniform variables");
        }

        glUniformMatrix4fv(view, 1, GL_FALSE, &cam.getView()[0][0]);
        glUniformMatrix4fv(proj, 1, GL_FALSE, &cam.getProj()[0][0]);
        glUniform3fv(camPos, 1, &cam.getPos()[0]);
    }


    void passLightVars(GLuint shader, const SceneLightData& light, int idx, bool clean) {
        std::string i = std::to_string(idx);

        GLint type = glGetUniformLocation(shader, ("lights[" + i + "].type").c_str());

        if (type == -1) {
            throw std::invalid_argument("Missing light type uniform variable");
        }

        GLint color = glGetUniformLocation(shader, ("lights[" + i + "].color").c_str());
        GLint function = glGetUniformLocation(shader, ("lights[" + i + "].function").c_str());

        if (color == -1 || function == -1) {
            throw std::invalid_argument("Missing light color and function uniform variables");
        }

        GLint pos = glGetUniformLocation(shader, ("lights[" + i + "].pos").c_str());
        GLint dir = glGetUniformLocation(shader, ("lights[" + i + "].dir").c_str());

        if (pos == -1 || dir == -1) {
            throw std::invalid_argument("Missing light pos and dir uniform variables");
        }

        GLint penumbra = glGetUniformLocation(shader, ("lights[" + i + "].penumbra").c_str());
        GLint angle = glGetUniformLocation(shader, ("lights[" + i + "].angle").c_str());

        if (penumbra == -1 || angle == -1) {
            throw std::invalid_argument("Missing light penumbra and angle uniform variables");
        }

        if (clean) {
            glUniform1i(type, 0);

            glUniform4f(color, 0.f, 0.f, 0.f, 0.f);
            glUniform3f(function, 0.f, 0.f, 0.f);

            glUniform3f(pos, 0.f, 0.f, 0.f);
            glUniform3f(dir, 0.f, 0.f, 0.f);

            glUniform1f(penumbra, 0.f);
            glUniform1f(angle, 0.f);

            return;
        }

        glUniform1i(type, static_cast<int>(light.type));

        glUniform4fv(color, 1, &light.color[0]);
        glUniform3fv(function, 1, &light.function[0]);

        glUniform3fv(pos, 1, &light.pos[0]);
        glUniform3fv(dir, 1, &light.dir[0]);

        glUniform1f(penumbra, light.penumbra);
        glUniform1f(angle, light.angle);
    }


    void passShapeVars(GLuint shader, const RenderShapeData& shape) {
        GLint model = glGetUniformLocation(shader, "model");
        GLint modelInvT = glGetUniformLocation(shader, "modelInvT");

        if (model == -1 || modelInvT == -1) {
            throw std::invalid_argument("Missing model matrix uniform variables");
        }

        glUniformMatrix4fv(model, 1, GL_FALSE, &shape.ctm[0][0]);
        glUniformMatrix3fv(modelInvT, 1, GL_TRUE, &shape.ctmInv[0][0]);

        // pass material vars
        GLint ambient = glGetUniformLocation(shader, "material.ambient");
        GLint diffuse = glGetUniformLocation(shader, "material.diffuse");
        GLint specular = glGetUniformLocation(shader, "material.specular");
        GLint shininess = glGetUniformLocation(shader, "material.shininess");

        if (ambient == -1 || diffuse == -1 || specular == -1 || shininess == -1) {
            throw std::invalid_argument("Missing material uniform variables");
        }

        glUniform4fv(ambient, 1, &shape.primitive.material.cAmbient[0]);
        glUniform4fv(diffuse, 1, &shape.primitive.material.cDiffuse[0]);
        glUniform4fv(specular, 1, &shape.primitive.material.cSpecular[0]);
        glUniform1f(shininess, shape.primitive.material.shininess);

        // pass texture-related vars
        GLint blend = glGetUniformLocation(shader, "material.blend");
        GLint repeatU = glGetUniformLocation(shader, "repeatU");
        GLint repeatV = glGetUniformLocation(shader, "repeatV");

        if (blend == -1 || repeatU == -1 || repeatV == -1) {
            throw std::invalid_argument("Missing texture-related material uniform variables");
        }

        glUniform1f(blend, shape.primitive.material.blend);
        glUniform1f(repeatU, shape.primitive.material.textureMap.repeatU);
        glUniform1f(repeatV, shape.primitive.material.textureMap.repeatV);

        // pass bone bool as false
        GLint hasBonesID = glGetUniformLocation(shader, "hasBones");

        if (hasBonesID == -1) {
            throw std::invalid_argument("Missing bone boolean uniform variable");
        }

        glUniform1i(hasBonesID, false);
    }

    void passTextureVars(GLuint shader, const Texture& texture) {
        // pass normal map bool
        GLint hasNormMapID = glGetUniformLocation(shader, "hasNormMap");

        if (hasNormMapID == -1) {
            throw std::invalid_argument("Missing normal map boolean uniform variable");
        }

        glUniform1i(hasNormMapID, texture.getSlot() == 1);

        // pass texture sampler2D var
        std::string var = texture.getSlot() == 1 ? "normTex" : "tex";

        GLint texID = glGetUniformLocation(shader, var.c_str());

        if (texID == -1) {
            std::cerr << "Missing texture var name: " << var << std::endl;
            throw std::invalid_argument("Missing texture sampler2D uniform variable");
        }

        glUniform1i(texID, texture.getSlot());
    }

    void passBoneVars(GLuint shader, const Model& model) {
        const auto& skinMats = model.getSkinMats();

        // pass bone bool as true if skinning matrices exist
        if (!skinMats.empty()) {
            GLint hasBonesID = glGetUniformLocation(shader, "hasBones");

            if (hasBonesID == -1) {
                throw std::invalid_argument("Missing bone boolean uniform variable");
            }

            glUniform1i(hasBonesID, true);
        }

        for (int i = 0; i < skinMats.size(); ++i) {
            if (i >= MAX_BONES) break;

            const glm::mat4& skinMat = skinMats[i];
            
            GLint skinMatID = glGetUniformLocation(shader, ("skinMats[" + std::to_string(i) + "]").c_str());
            if (skinMatID != -1) glUniformMatrix4fv(skinMatID, 1, GL_FALSE, &skinMat[0][0]);

            GLint skinMatT = glGetUniformLocation(shader, ("skinMatsT[" + std::to_string(i) + "]").c_str());
            if (skinMatT != -1) glUniformMatrix3fv(skinMatT, 1, GL_TRUE, &glm::mat3{glm::inverse(skinMat)}[0][0]);
        }
    }

}
