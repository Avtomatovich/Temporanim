#include "geometry.h"

Geometry::Geometry(std::unique_ptr<Primitive> prim) :
    m_prim(std::move(prim))
{
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    genPrim();
}

Geometry::Geometry(std::unique_ptr<Mesh> mesh) :
    m_mesh(std::move(mesh))
{
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    glGenVertexArrays(1, &m_vao);
    genMesh();
}

void Geometry::updateTesselParams(int param1, int param2) {
    if (!m_prim) return;
    m_prim->updateParams(param1, param2);
    genPrim();
}

void Geometry::draw() const {
    glBindVertexArray(m_vao);

    m_prim ?
        glDrawArrays(GL_TRIANGLES, 0, m_numVertices) :
        glDrawElements(GL_TRIANGLES, m_numIndexes, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Geometry::clean() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

void Geometry::genPrim() {
    const std::vector<float>& primData = m_prim->generateShape();

    // init no of vertices
    m_numVertices = primData.size() / stride;

    // // VBO
    // bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // fill VBO with data
    glBufferData(GL_ARRAY_BUFFER, primData.size() * sizeof(GLfloat), primData.data(), GL_STATIC_DRAW);

    // // VAO
    // bind VAO
    glBindVertexArray(m_vao);

    // set VAO attribs
    // position attrib = 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, // index in shader
                          3, // number of vals in attrib
                          GL_FLOAT, // type of val in attrib
                          GL_FALSE, // do not normalize to [-1, 1] or [0, 1]
                          stride * sizeof(GLfloat), // size (in bytes) between vertices
                          reinterpret_cast<void*>(0 * sizeof(GLfloat))); // offset (in bytes) from vertex start

    // normal attrib = 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // texture attrib = 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));

    // tangent attrib = 3
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), reinterpret_cast<void*>(8 * sizeof(GLfloat)));

    // bitangent attrib = 4
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), reinterpret_cast<void*>(11 * sizeof(GLfloat)));

    // unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // unbind VAO
    glBindVertexArray(0);
}

void Geometry::genMesh() {
    const std::vector<Vertex>& meshData = m_mesh->getVertices();
    const std::vector<unsigned int>& indexes = m_mesh->getIndexes();

    // init no of indexes
    m_numIndexes = indexes.size();

    // bind all (VAO first)
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    // populate VBO
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(Vertex), meshData.data(), GL_STATIC_DRAW);

    // populate EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), indexes.data(), GL_STATIC_DRAW);

    // set VAO attribs
    // position attrib = 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    // normal attrib = 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, norm)));

    // texture attrib = 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    // tangent attrib = 3
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tang)));

    // bitangent attrib = 4
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bitang)));

    // bone ID attrib = 5
    // NOTE: using glVertexAttrib*I*Pointer, not glVertexAttribPointer
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, MAX_WEIGHTS, GL_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, boneIDs)));

    // bone weight attrib = 6
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, MAX_WEIGHTS, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, weights)));

    // unbind all (VAO first)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
