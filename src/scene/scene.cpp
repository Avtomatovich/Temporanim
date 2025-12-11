#include "scene.h"
#include "primitive/cone.h"
#include "primitive/cube.h"
#include "primitive/cylinder.h"
#include "primitive/sphere.h"
#include "utils/uniloader.h"
#include "utils/debug.h"
#include <algorithm>

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

        // Add collision instance to collision map
        m_collMap.emplace(i, shape);

        // Add model to model map if not present
        if (!m_modelMap.contains(meshfile) && !meshfile.empty()) {
            m_modelMap.emplace(meshfile, meshfile);
        }

        // Add animator to animator map if not present
        if (!m_animMap.contains(meshfile) && !meshfile.empty()) {
            const AnimData& animData = metaData.animData.at(meshfile);
            // Only add if animations are present
            if (!animData.animations.empty()) m_animMap.emplace(meshfile, animData);
        }

        // If shape is mesh
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_MESH) {
            // Add to model's mesh map
            m_modelMap.at(meshfile).addMesh(shape);
        } else {
            // Add primitive to geom map if not present
            if (!m_primMap.contains(getGeomKey(shape))) {
                addPrim(shape, param1, param2);
            }
        }

        // Add rigid body to phys map if dynamic
        if (shape.primitive.isDynamic) {
            // Use default mass of 1.f
            m_physMap.emplace(i, RigidBody{shape.primitive.type,
                                           1.f,
                                           shape.ctm,
                                           m_collMap.at(i).getBox()});
        }

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
    
    loadProjectileTemplate(FruitType::APPLE);
    loadProjectileTemplate(FruitType::TOMATO);
    loadProjectileTemplate(FruitType::CABBAGE);
    loadProjectileTemplate(FruitType::CARROT);
    loadProjectileTemplate(FruitType::ONION);
    loadProjectileTemplate(FruitType::SWEET_POTATO);
    
    std::string meshfile = findMeshfile("paladin");
    if (meshfile.empty()) {
        std::cerr << "Missing paladin model" << std::endl;
    } else {
        // for each shape
        for (int i = 0; i < m_shapes.size(); ++i) {
            // if mesh is part of model
            if (meshfile == m_shapes[i].primitive.meshfile) {
                // fetch and scale down mesh AABB
                m_collMap.at(i).scaleBox(0.8f);
            }
        }        
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

    // draw projectiles
    for (const auto& proj : m_projectiles) {
        const RenderShapeData& renderData = proj.getRenderData();

        // textures
        if (renderData.primitive.material.textureMap.isUsed) {
            const Texture& texture = m_texMap.at(renderData.primitive.material.textureMap.filename);
            glActiveTexture(GL_TEXTURE0 + texture.getSlot());
            glBindTexture(GL_TEXTURE_2D, texture.getId());
            passTextureVars(shader, texture);
        }

        if (renderData.primitive.material.bumpMap.isUsed && m_normalMapToggled) {
            const Texture& texture = m_texMap.at(renderData.primitive.material.bumpMap.filename);
            glActiveTexture(GL_TEXTURE0 + texture.getSlot());
            glBindTexture(GL_TEXTURE_2D, texture.getId());
            passTextureVars(shader, texture);
        }

        passShapeVars(shader, renderData);
        passPhysVars(shader, proj.getRigidBody());

        getGeom(renderData).draw();

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
    for (auto& [_, anim] : m_animMap) anim.swap(toNext, true);
}

void Scene::toggleNormalMap() {
    m_normalMapToggled = !m_normalMapToggled;
}

void Scene::loadProjectileTemplate(FruitType type) {
    std::string meshfile = Projectile::getMeshFileForType(type);

    // Skip if already loaded
    if (m_projectileTemplates.contains(meshfile)) return;

    // Find the template shape - try exact match first
    for (const auto& shape : m_shapes) {
        if (shape.primitive.meshfile == meshfile) {
            m_projectileTemplates[meshfile] = shape;
            return;
        }
    }
    
    // Extract fruit directory name (e.g., "apple" from "models/apple/scene.gltf")
    // Find the directory name before the filename
    size_t lastSlash = meshfile.find_last_of("/");
    if (lastSlash != std::string::npos) {
        size_t secondLastSlash = meshfile.find_last_of("/", lastSlash - 1);
        std::string fruitDir = meshfile.substr(secondLastSlash + 1, lastSlash - secondLastSlash - 1);
        // fruitDir should be "apple", "potato", "cabbage", etc.
        
        // Only match dynamic meshes that contain this fruit directory name
        for (const auto& shape : m_shapes) {
            if (shape.primitive.type == PrimitiveType::PRIMITIVE_MESH && 
                shape.primitive.isDynamic &&
                shape.primitive.meshfile.find(fruitDir) != std::string::npos) {
                m_projectileTemplates[meshfile] = shape;
                std::cout << "Loaded projectile template: " << meshfile 
                          << " from " << shape.primitive.meshfile << std::endl;
                return;
            }
        }
    }

    std::cerr << "Warning: Projectile template not found for " << meshfile << std::endl;
}

void Scene::throwProjectile(const glm::vec3& spawnPos, const glm::vec3& velocity) {
    // Pick a random fruit type - use enum count instead of hardcoded 6
    int numTypes = 6;  // TOMATO, CABBAGE, CARROT, APPLE, ONION, SWEET_POTATO
    FruitType type = static_cast<FruitType>(rand() % numTypes);
    
    std::cout << "Selected fruit type: " << static_cast<int>(type) << std::endl;
    
    std::string meshfile = Projectile::getMeshFileForType(type);
    std::cout << "Meshfile: " << meshfile << std::endl;

    if (!m_projectileTemplates.contains(meshfile)) {
        std::cerr << "No template for projectile type: " << meshfile << std::endl;
        std::cerr << "Available templates: ";
        for (const auto& [key, _] : m_projectileTemplates) {
            std::cerr << key << " ";
        }
        std::cerr << std::endl;
        return;
    }

    // Create projectile using template
    m_projectiles.emplace_back(type, spawnPos, velocity, m_projectileTemplates.at(meshfile));
    
    //  initial spin  (rotation)
    if (m_torqueEnabled) {
        glm::vec3 throwDir = glm::normalize(velocity);
        
        // random axis for dramatic effect
        glm::vec3 randomAxis = glm::normalize(glm::vec3(
            (rand() % 200 - 100) / 100.0f,
            (rand() % 200 - 100) / 100.0f,
            (rand() % 200 - 100) / 100.0f
        ));
        // perpendicular to throw direction
        glm::vec3 spinAxis = glm::normalize(randomAxis - glm::dot(randomAxis, throwDir) * throwDir);
        
        float spinSpeed = glm::length(velocity) * 4.0f * (0.7f + (rand() % 60) / 100.0f);
        spinSpeed = glm::clamp(spinSpeed, 8.0f, 30.0f);
        
        m_projectiles.back().getRigidBody().setAngularMomentum(spinAxis * spinSpeed * 0.3f);
    }
    std::cout << "Threw projectile! Total projectiles: " << m_projectiles.size() << std::endl;
}

void Scene::updateProjectiles(float dt) {
    // Update each projectile's physics
    for (auto& proj : m_projectiles) {
        // Clear forces
        proj.getRigidBody().clearForces();

        // gravity (similar to sphere bouncing)
        if (m_gravityEnabled) {
            proj.getRigidBody().applyForce();
        }

        // physics
        proj.getRigidBody().integrate(dt);

        proj.updateRenderData();

        // check collision with ground (bounce like sphere)..?
        proj.getRigidBody().handleForces();
    }

    // Remove projectiles that should be deleted
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                       [](const Projectile& p) { return p.shouldRemove(); }),
        m_projectiles.end()
        );
}

void Scene::updatePhys(float dt) {
    if (!m_gravityEnabled && !m_torqueEnabled && !m_collisionEnabled) {
        for (auto& [_, rb] : m_physMap) rb.reset();
        updateProjectiles(dt);
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
    if (m_torqueEnabled) {
        // for (auto& [id, rb] : m_physMap) {
        //     glm::vec3 axis = glm::normalize(glm::vec3{
        //         std::sin(id * 1.23f),
        //         std::cos(id * 4.56f),
        //         std::sin(id * 7.89f)
        //     });
        //     rb.torque += axis * 3.0f;
        // }
    }

    for (auto& [_, rb] : m_physMap) rb.integrate(dt);

    // Update projectiles (always call this, not just when physics disabled)
    updateProjectiles(dt);

    // collision
    if (m_collisionEnabled) {
        // update dynamic AABBs
        for (auto& [rid, rb] : m_physMap) m_collMap.at(rid).updateBox(rb.getCtm());

        // for each dynamic object
        for (auto& [rid, rb] : m_physMap) {
            // store const ref of affector collision
            Collision& affector = m_collMap.at(rid);
            // check each collision object (static + dynamic)
            for (const auto& [cid, affectee] : m_collMap) {
                // skip self and previously collided dynamics
                if (cid == rid || (m_physMap.contains(cid) && cid <= rid)) continue;

                // if collision detected
                if (affector.detect(affectee)) {
                    std::cout << "collision detected" << std::endl;

                    rb.handleForces();

                    // determine affectee's reaction forces if affectee is dynamic
                    if (m_physMap.contains(cid)) m_physMap.at(cid).handleForces();
                }
            }
        }
    }
}

std::string Scene::findMeshfile(const std::string& query) {
    std::string filename, meshfile;
    // convert query to lowercase
    for (const char& c : query) filename += std::tolower(c);

    for (const RenderShapeData& shape: m_shapes) {
        // convert meshfile to lowercase
        for (const char& c : shape.primitive.meshfile) {
            meshfile += std::tolower(c);
        }

        // compare
        if (meshfile.find(filename) != std::string::npos) {
            return shape.primitive.meshfile;
        }

        // clear lowercase meshfile string for next iteration
        meshfile.clear();
    }

    return "";
}
