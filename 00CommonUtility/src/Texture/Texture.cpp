#include "Texture.h"

#include <string>
#include <iostream>
#include <sfml/Graphics/Image.hpp>
#include "../Glad/Glad.h"

Texture::Texture(const std::string& path, int slot)
	: _slot(slot)
{
	/* Create texture handle and active it's slot */
	::glGenTextures(1, &_handle);
	::glActiveTexture(GL_TEXTURE0 + slot); // 激活指定车位
	::glBindTexture(GL_TEXTURE_2D, _handle);

	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Load image */
	sf::Image image;
	image.loadFromFile(path);
	image.flipVertically();

	int width = image.getSize().x;
	int height = image.getSize().y;
	const unsigned char* data = image.getPixelsPtr();

	if (data)
	{
		/* Pass data from CPU to GPU */
		::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		::glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture:" << path << std::endl;
	}
}

Texture::~Texture()
{
	::glDeleteTextures(1, &_handle);
}

int Texture::GetSlot() const
{
	return _slot;
}
