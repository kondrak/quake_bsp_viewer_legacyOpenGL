#ifndef TEXTURE_INCLUDED
#define TEXTURE_INCLUDED

#include "renderer/OpenGLHeaders.hpp"

/*
 *  Basic texture
 */

class Texture
{
public:
    Texture(const char *filename);
    ~Texture();
    int Width()      { return m_width; }
    int Height()     { return m_height; }
    int Components() { return m_components; }
    GLuint Load();

private:
    int    m_width;
    int    m_height;
    int    m_components;
    GLuint m_texId;
    unsigned char *m_textureData;
};

#endif