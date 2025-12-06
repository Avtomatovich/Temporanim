#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <unordered_map>
#include "utils/sceneparser.h"
#include "geometry.h"
#include "animation/animator.h"

class Model
{
public:
    Model(const std::string& meshfile,
          const AnimData& animData);

    const Geometry& getGeom(int key) const;

    const std::vector<glm::mat4>& getSkinMats() const;

    void addMesh(RenderShapeData& shape);

    bool hasAnim() const;

    void playAnim();

    void updateAnim(float deltaTime);

    void swapAnim(bool isNext);

    void clean();

private:
    const std::string m_meshfile;
    std::unordered_map<int, Geometry> m_meshMap;
    Animator m_animator;
};

#endif // MODEL_H
