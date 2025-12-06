#include "texture.h"
#include <qimage.h>
#include <stdexcept>

Texture::Texture(const std::string& filename,
                 unsigned int slot) :
    m_slot(slot)
{
    // Gen texture ID
    glGenTextures(1, &m_texId);

    // Activate texture unit
    glActiveTexture(GL_TEXTURE0 + m_slot);

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, m_texId);

    // Load image data
    QImage data = QImage(QString(filename.c_str()));

    // Throw exception if bad data
    if (data.isNull()) {
        throw std::runtime_error("Error loading texture: \"" + filename + "\"");
    }

    // Format image to fit OpenGL
    data = data.convertToFormat(QImage::Format_RGBA8888).flipped(Qt::Vertical);

    // Store image data in texture unit
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 data.width(),
                 data.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 data.bits());

    // Set texture parameters
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

const GLuint Texture::getId() const {
    return m_texId;
}

const unsigned int Texture::getSlot() const {
    return m_slot;
}

void Texture::clean() {
    glDeleteTextures(1, &m_texId);
}
