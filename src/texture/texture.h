#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

class Texture
{
public:
    Texture(const std::string& filename,
            unsigned int slot);

    const GLuint getId() const;

    const unsigned int getSlot() const;

    void clean();

private:
    GLuint m_texId;
    unsigned int m_slot;
};

#endif // TEXTURE_H
