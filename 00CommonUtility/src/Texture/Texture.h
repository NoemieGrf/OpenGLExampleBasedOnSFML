#pragma once

#include <string>

class Texture
{
public:
	Texture(const std::string& path, int slot);
	~Texture();

public:
	int GetSlot() const;

private:
	unsigned int _handle;
	int _slot;
};