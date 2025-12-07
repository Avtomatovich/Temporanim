#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <unordered_map>
#include "utils/sceneparser.h"
#include "geometry.h"

class Model
{
public:
    Model(const std::string& meshfile);

    const Geometry& getGeom(int key) const;

    void addMesh(RenderShapeData& shape);

    void clean();

private:
    const std::string m_meshfile;
    std::unordered_map<int, Geometry> m_meshMap;
};

#endif // MODEL_H
