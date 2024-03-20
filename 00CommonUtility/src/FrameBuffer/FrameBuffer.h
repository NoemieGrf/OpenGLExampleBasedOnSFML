#pragma once

class FrameBuffer
{
public:
	virtual ~FrameBuffer() = default;

public:
	auto GetFrameBufferHandle() -> unsigned int;

protected:
	unsigned int _frameBufferHandle;
};