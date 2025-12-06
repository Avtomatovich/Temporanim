#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <GL/glew.h>
#include <memory>
#include "primitive/primitive.h"
#include "mesh.h"

class Geometry
{
public:
    Geometry(std::unique_ptr<Primitive> prim);

    Geometry(std::unique_ptr<Mesh> mesh);

    void updateTesselParams(int param1, int param2);

    void draw() const;

    void clean();

private:
    // primitive data + func
    std::unique_ptr<Primitive> m_prim;
    size_t m_numVertices;
    void genPrim();

    // mesh data + func
    std::unique_ptr<Mesh> m_mesh;
    size_t m_numIndexes;
    void genMesh();

    GLuint m_vbo; // vertex buffer obj
    GLuint m_ebo; // element buffer obj
    GLuint m_vao; // vertex array obj

    // 3 vert + 3 norm + 2 uv + 3 tang + 3 bitang
    int stride = 14;
};

#endif // GEOMETRY_H
