#include "scene.h"
#include "primitive/cone.h"
#include "primitive/cube.h"
#include "primitive/cylinder.h"
#include "primitive/sphere.h"
#include "utils/uniloader.h"
#include "utils/debug.h"

using namespace Debug;
using namespace UniLoader;

Scene::Scene(const RenderData& metaData,
             float aspectRatio,
             float near, float far,
             int param1, int param2) :
    m_global(metaData.globalData),
    m_shapes(metaData.shapes),
    m_lights(metaData.lights)
{
    m_cam = Camera {
        metaData.cameraData.pos,
        metaData.cameraData.look,
        metaData.cameraData.up,
        aspectRatio,
        metaData.cameraData.heightAngle,
        near,
        far
    };

    for (int i = 0; i < m_shapes.size(); ++i) {
        RenderShapeData& shape = m_shapes[i];

        const std::string& meshfile = shape.primitive.meshfile;

        // Add animator to animator map if not present
        if (!m_animMap.contains(meshfile) && !meshfile.empty()) {
            const AnimData& animData = metaData.animData.at(meshfile);
            // Only add if animations are present
            if (!animData.animations.empty()) m_animMap.emplace(meshfile, animData);
        }

        // Add primitive to geom map if not present and not mesh
        if (shape.primitive.type != PrimitiveType::PRIMITIVE_MESH) {
            if (!m_primMap.contains(getGeomKey(shape))) {
                addPrim(shape, param1, param2);
            }
        }

        // Init mesh and texture data
        initModelAndTex(shape);

        // Init physics data
        initPhys(shape, i);
    }

    // Init index of first projectile instance in shape list
    m_projectileIdx = m_shapes.size();
}

void Scene::initModelAndTex(const RenderShapeData& shape) {
    const std::string& meshfile = shape.primitive.meshfile;

    // Add model to model map if not present
    if (!m_modelMap.contains(meshfile) && !meshfile.empty()) {
        m_modelMap.emplace(meshfile, Model{meshfile});
    }

    // Add to mesh to model
    m_modelMap.at(meshfile).addMesh(shape);

    // Add texture map to slot 0 if used
    if (shape.primitive.material.textureMap.isUsed) {
        const std::string& filename = shape.primitive.material.textureMap.filename;

        if (!m_texMap.contains(filename)) {
            m_texMap.emplace(filename, Texture{filename, 0});
        }
    }

    // Add normal map to slot 1 if used
    if (shape.primitive.material.bumpMap.isUsed) {
        const std::string& filename = shape.primitive.material.bumpMap.filename;

        if (!m_texMap.contains(filename)) {
            m_texMap.emplace(filename, Texture{filename, 1});
        }
    }
}

void Scene::initPhys(const RenderShapeData& shape, int i) {
    // Add collision instance to collision map
    m_collMap.emplace(i, Collision{shape});

    // Add rigid body to phys map if dynamic
    if (shape.primitive.isDynamic) {
        // Use default mass of 1.f
        m_physMap.emplace(i, RigidBody{shape.primitive.type,
                                       1.f,
                                       shape.ctm,
                                       m_collMap.at(i).getBox()});
    }
}

bool Scene::draw(GLuint shader) {
    glErrorCheck();

    passGlobalVars(shader, m_global);
    passCamVars(shader, m_cam);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (i >= m_lights.size()) {
            passLightVars(shader, SceneLightData{}, i, true);
        } else {
            passLightVars(shader, m_lights[i], i, false);
        }
    }

    for (int i = 0; i < m_shapes.size(); ++i) {
        // Fetch current shape
        const RenderShapeData& shape = m_shapes[i];

        // Activate diffuse map slot if available
        if (shape.primitive.material.textureMap.isUsed) {
            const Texture& texture = m_texMap.at(shape.primitive.material.textureMap.filename);

            glActiveTexture(GL_TEXTURE0 + texture.getSlot());
            glBindTexture(GL_TEXTURE_2D, texture.getId());

            passTextureVars(shader, texture);
        }

        // Activate normal map slot if available
        if (shape.primitive.material.bumpMap.isUsed && m_normalMapToggled) {
            const Texture& texture = m_texMap.at(shape.primitive.material.bumpMap.filename);

            glActiveTexture(GL_TEXTURE0 + texture.getSlot());
            glBindTexture(GL_TEXTURE_2D, texture.getId());

            passTextureVars(shader, texture);
        }

        passShapeVars(shader, shape);

        // Fetch animation if present
        if (m_animMap.contains(shape.primitive.meshfile)) {
            passBoneVars(shader, m_animMap.at(shape.primitive.meshfile));
        }

        // Fetch physics state if dynamic
        if (m_physMap.contains(i)) {
            passPhysVars(shader, m_physMap.at(i));
        }

        getGeom(shape).draw();

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glErrorCheck();

    return true;
}

void Scene::addPrim(const RenderShapeData& shape, int param1, int param2) {
    int key = getGeomKey(shape);
    switch(shape.primitive.type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            m_primMap.emplace(key, std::make_unique<Cube>(param1));
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            m_primMap.emplace(key, std::make_unique<Cone>(param1, param2));
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_primMap.emplace(key, std::make_unique<Cylinder>(param1, param2));
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            m_primMap.emplace(key, std::make_unique<Sphere>(param1, param2));
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            break;
    }
}

const Geometry& Scene::getGeom(const RenderShapeData& shape) {
    int key = getGeomKey(shape);

    return m_modelMap.contains(shape.primitive.meshfile) ?
               m_modelMap.at(shape.primitive.meshfile).getGeom(key) :
               m_primMap.at(key);
}

int Scene::getGeomKey(const RenderShapeData& shape) {
    return shape.primitive.type == PrimitiveType::PRIMITIVE_MESH ?
               shape.id :
               static_cast<int>(shape.primitive.type);
}

void Scene::updatePhys(float dt) {
    if (!m_gravityEnabled && !m_torqueEnabled && !m_collisionsEnabled) {
        for (auto& [_, rb] : m_physMap) rb.reset();
        return;
    }

    for (auto& [_, rb] : m_physMap) rb.clearForces();

    //  gravity
    if (m_gravityEnabled) {
        for (auto& [_, rb] : m_physMap) rb.applyForce();
    } else {
        for (auto& [_, rb] : m_physMap) rb.reset();
    }

    // torque
    if (m_torqueEnabled) {}

    for (auto& [_, rb] : m_physMap) rb.integrate(dt);

    // collision
    if (m_collisionsEnabled) {
        // update dynamic AABBs
        for (auto& [rid, rb] : m_physMap) m_collMap.at(rid).updateBox(rb.getCtm());

        // for each dynamic object
        for (auto& [rid, rb] : m_physMap) {
            Collision& affector = m_collMap.at(rid);
            // check each collision object (static + dynamic)
            for (const auto& [cid, affectee] : m_collMap) {
                // skip self and previously collided dynamics
                if (cid == rid || (m_physMap.contains(cid) && cid <= rid)) continue;              

                // if collision detected
                if (affector.detect(affectee)) {
                    std::cout << "collision detected" << std::endl;

                    // determine reaction forces
                    rb.applyReaction();

                    // determine affectee's reaction forces if affectee is dynamic
                    if (m_physMap.contains(cid)) m_physMap.at(cid).applyReaction();
                }
            }
        }
    }
}

void Scene::loadProjectiles(const Projectile& projectiles) {
    m_projectiles = std::make_unique<Projectile>(projectiles);

    if (!m_projectiles) throw std::runtime_error("Projectile instance is null");

    // Fetch list of projectile shapes
    const auto& shapes = m_projectiles->getShapes();
    // Init model and texture instances for each
    for (const auto& shape : shapes) initModelAndTex(shape);
}

void Scene::spawn() {
    if (!m_projectiles) return;

    // TODO: Despawn shape if count exceeds capping value
    // if (m_numProjectiles > MAX_PROJECTILES) despawn();

    // Fetch random shape from list of projectile shapes
    RenderShapeData shape = m_projectiles->spawn();

    // Throw exception if projectile is not marked as dynamic
    if (!shape.primitive.isDynamic) throw std::runtime_error("Projectile not dynamic");

    // Translate shape to camera location
    glm::vec3 camPos = m_cam.getPos();
    shape.ctm[3] = glm::vec4{camPos.x, camPos.y - 1, camPos.z, 1.f};
    shape.ctmInv = glm::inverse(shape.ctm);

    // Init key for maps
    int i = m_shapes.size();

    // Init related list and map entries for shape
    initPhys(shape, i);

    // Add projectile shape to shapes list
    m_shapes.push_back(shape);

    // Increment projectile count
    m_numProjectiles++;

    // Apply impulse to rigid body
    m_physMap.at(i).applyImpulse(strength * m_cam.getLook());
}

void Scene::despawn() {
    if (m_numProjectiles <= 0) return;

    // TODO: update m_physMap

    // Shift all projectiles in collision map one step back
    for (int i = m_projectileIdx + 1; i < m_shapes.size(); ++i) {
        m_collMap.emplace(i - 1, m_collMap.at(i));
    }

    // Remove final entry in collision maps
    m_collMap.erase(m_shapes.size() - 1);

    // Remove first projectile instance from shape list
    m_shapes.erase(m_shapes.begin() + m_projectileIdx);

    // Decrement projectile count
    m_numProjectiles--;
}

void Scene::clean() {
    for (auto& [_, prim] : m_primMap) prim.clean();
    for (auto& [_, model] : m_modelMap) model.clean();
    for (auto& [_, tex] : m_texMap) tex.clean();
}

void Scene::retessellate(int param1, int param2) {
    for (auto& [_, prim] : m_primMap) prim.updateParams(param1, param2);
}

void Scene::updateAnim(float dt) {
    for (auto& [_, anim] : m_animMap) anim.update(dt);
}

void Scene::playAnim() {
    for (auto& [_, anim] : m_animMap) anim.play();
}

void Scene::swapAnim(bool toNext) {
    for (auto& [_, anim] : m_animMap) anim.swap(toNext);
}

void Scene::toggleNormalMap() {
    m_normalMapToggled = !m_normalMapToggled;
}
