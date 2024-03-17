#pragma once

#include <string>
#include <iostream>
#include <sfml/Graphics/Image.hpp>
#include "../Glad/Glad.h"

class Util
{
public:
	Util() = delete;
		
public:
	static unsigned int LoadImage(const std::string& path, int internalFormat, GLenum format, int textureSlot)
	{
		/* Create texture handle and active it's slot */
		unsigned int texBuffer;
		::glGenTextures(1, &texBuffer);
		::glActiveTexture(GL_TEXTURE0 + textureSlot); // 激活指定车位
		::glBindTexture(GL_TEXTURE_2D, texBuffer);

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
			::glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			::glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture:" << path << std::endl;
		}

		return texBuffer;
	}
};