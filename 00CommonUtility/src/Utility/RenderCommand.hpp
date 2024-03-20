#pragma once

#include <string>
#include <iostream>
#include <sfml/Graphics/Image.hpp>
#include "../Glad/Glad.h"
#include "../Object/Object.hpp"
#include "../Shader/Shader.h"

class RenderCommand
{
public:
	RenderCommand() = delete;

public:
	template<VertexLayout Layout>
	static void DrawPrepare(Object<Layout>& object, Shader& shader)
	{
		// 1. Bind vertex attribute
		::glBindVertexArray(object.GetVertexArrayHandle());

		// 2. Bind vertex array
		::glBindBuffer(GL_ARRAY_BUFFER, object.GetVertexBufferHandle());

		// 3. Bind shader program
		::glUseProgram(shader.GetHandle());
	}

	template<VertexLayout Layout>
	static void DrawObject(Object<Layout>& object)
	{
		::glDrawArrays(GL_TRIANGLES, 0, object.GetTriangleNum());
	}
};