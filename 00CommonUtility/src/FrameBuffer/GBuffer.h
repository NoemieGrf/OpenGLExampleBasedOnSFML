#pragma once

#include "FrameBuffer.h"

class GBuffer : public FrameBuffer
{
public:
	GBuffer(unsigned int width, unsigned int height, int posTexSlot, int normalTexSlot, int albedoTexSlot);
	~GBuffer() override;

public:
	int _posTexSlot;
	unsigned int _posTexHandle;

	int _normalTexSlot;
	unsigned int _normalTexHandle;

	int _albedoTexSlot;
	unsigned int _albedoTexHandle;

	unsigned int _depthBufHandle;
};