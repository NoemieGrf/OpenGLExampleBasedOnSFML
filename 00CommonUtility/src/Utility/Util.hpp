#pragma once

#include <string>
#include <iostream>
#include <sfml/Graphics/Image.hpp>
#include "../Glad/Glad.h"

class Util
{
public:
	inline static unsigned int LoadImage(const std::string& path, int internalFormat, GLenum format, int textureSlot)
	{
		unsigned int texBuffer;
		::glGenTextures(1, &texBuffer);

		::glActiveTexture(GL_TEXTURE0 + textureSlot);
		::glBindTexture(GL_TEXTURE_2D, texBuffer);

		sf::Image image;
		image.loadFromFile(path);
		image.flipVertically();

		int width = image.getSize().x;
		int height = image.getSize().y;
		const unsigned char* data = image.getPixelsPtr();

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture:" << path << std::endl;
		}

		return texBuffer;
	}
};