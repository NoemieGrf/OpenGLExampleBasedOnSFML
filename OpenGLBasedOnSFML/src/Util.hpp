#pragma once

#include <string>
#include <iostream>
#include <sfml/Graphics/Image.hpp>
#include "glad/glad.h"

class Util
{
public:
	inline static unsigned int loadImage(const std::string& s, int internalFormat, GLenum format, int textureSlot)
	{
		unsigned int texBuffer;
		::glGenTextures(1, &texBuffer);

		::glActiveTexture(GL_TEXTURE0 + textureSlot);
		::glBindTexture(GL_TEXTURE_2D, texBuffer);

		sf::Image image1;
		image1.loadFromFile(s);
		image1.flipVertically();

		int width = image1.getSize().x;
		int height = image1.getSize().y;
		const unsigned char* data = image1.getPixelsPtr();

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		return texBuffer;
	}
};