#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <unordered_map>
#include "physics/collision.h"
#include "utils/sceneparser.h"
#include "geometry.h"
#include "physics/box.h"

class Model
{
public:
    Model(const std::string& meshfile);

    void clean();

    const Geometry& getGeom(int key) const;

    const Box& getBox() const;

    void addMesh(RenderShapeData& shape);

    void buildBox(const std::vector<RenderShapeData>& shapes,
                  const std::unordered_map<int, Collision>& collMap);

private:
    const std::string m_meshfile;
    std::unordered_map<int, Geometry> m_meshMap;
    Box m_box;
};

#endif // MODEL_H
