#include "ShadowMap.h"
#include "../Glad/Glad.h"

ShadowMap::ShadowMap(unsigned int width, unsigned int height, int textureSlot)
    : _textureSlot(textureSlot)
{
    /* Create frame buffer */
    ::glGenFramebuffers(1, &_frameBufferHandle);
    
    /* Create cube map */
    ::glGenTextures(1, &_textureHandle);
    ::glActiveTexture(GL_TEXTURE0 + textureSlot);
    ::glBindTexture(GL_TEXTURE_CUBE_MAP, _textureHandle);

    for (unsigned int i = 0; i < 6; ++i)
        ::glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    /* Attach to frame buffer */
    ::glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferHandle);
    ::glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _textureHandle, 0);
    ::glDrawBuffer(GL_NONE);
    ::glReadBuffer(GL_NONE);
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap()
{
    ::glDeleteTextures(1, &_frameBufferHandle);
    FrameBuffer::~FrameBuffer();
}
