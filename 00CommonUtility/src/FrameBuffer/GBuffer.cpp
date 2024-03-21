#include <iostream>
#include "GBuffer.h"
#include "../Glad/Glad.h"

GBuffer::GBuffer(unsigned int width, unsigned int height, int posTexSlot, int normalTexSlot, int albedoTexSlot)
    : _posTexSlot(posTexSlot)
    , _normalTexSlot(normalTexSlot)
    , _albedoTexSlot(albedoTexSlot)
{
    /* Create GBuffer */
    ::glGenFramebuffers(1, &_frameBufferHandle);
    ::glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferHandle);

    /* Position GBuffer */
    ::glGenTextures(1, &_posTexHandle);
    ::glActiveTexture(GL_TEXTURE0 + _posTexSlot);
    ::glBindTexture(GL_TEXTURE_2D, _posTexHandle);
    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Normal GBuffer */
    ::glGenTextures(1, &_normalTexHandle);
    ::glActiveTexture(GL_TEXTURE0 + _normalTexSlot);
    ::glBindTexture(GL_TEXTURE_2D, _normalTexHandle);
    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Color & Specular GBuffer */
    ::glGenTextures(1, &_albedoTexHandle);
    ::glActiveTexture(GL_TEXTURE0 + _albedoTexSlot);
    ::glBindTexture(GL_TEXTURE_2D, _albedoTexHandle);
    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Depth buffer */
    ::glGenRenderbuffers(1, &_depthBufHandle);
    ::glBindRenderbuffer(GL_RENDERBUFFER, _depthBufHandle);
    ::glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    /* Set attachments */
    ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _posTexHandle, 0);
    ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normalTexHandle, 0);
    ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _albedoTexHandle, 0);
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    ::glDrawBuffers(3, attachments);
    ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBufHandle);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    /* Unbind frame buffer */
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GBuffer::~GBuffer()
{
    ::glDeleteRenderbuffers(1, &_depthBufHandle);
    ::glDeleteTextures(1, &_posTexHandle);
    ::glDeleteTextures(1, &_normalTexHandle);
    ::glDeleteTextures(1, &_albedoTexHandle);
    ::glDeleteFramebuffers(1, &_frameBufferHandle);
    FrameBuffer::~FrameBuffer();
}

auto GBuffer::GetPositionTextureSlot() const -> int
{
    return _posTexSlot;
}

auto GBuffer::GetNormalTextureSlot() const -> int
{
    return _normalTexSlot;
}

auto GBuffer::GetAlbedoTextureSlot() const -> int
{
    return _albedoTexSlot;
}