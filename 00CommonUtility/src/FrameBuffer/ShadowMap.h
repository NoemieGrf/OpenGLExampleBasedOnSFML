#pragma once

#include "FrameBuffer.h"

class ShadowMap : public FrameBuffer
{
public:
	ShadowMap(unsigned int width, unsigned int height, int textureSlot);
	~ShadowMap() override;

private:
	int _textureSlot;
	unsigned int _textureHandle;
};

