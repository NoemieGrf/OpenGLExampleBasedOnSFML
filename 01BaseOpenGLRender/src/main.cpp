#include <iostream>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sfml/Window.hpp>

#include "Glad/Glad.h"
#include "Shader/Shader.h"
#include "Camera/Camera.h"
#include "GlobalData/Plane.h"
#include "GlobalData/Cube.h"
#include "Object/Object.h"
#include "Vertex/VertexData.hpp"
#include "Texture/Texture.h"

using uint = unsigned int;

int main()
{
	/* Init SFML window */
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	const uint WIDTH = 1600;
	const uint HEIGHT = 900;
	const float W_H_ratio = static_cast<float>(WIDTH) / HEIGHT;
	const float FOV = 50.0f;

	sf::Window window(sf::VideoMode(WIDTH, HEIGHT, 32), "BaseOpenGLRender", sf::Style::Titlebar | sf::Style::Close, settings);

	/* Glad load all openGL functions */
	::gladLoadGL();

	/* OpenGL setting */
	::glEnable(GL_DEPTH_TEST);
	::glViewport(0, 0, WIDTH, HEIGHT);
	::glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

	/* Objects VAO & VBO */
	LayoutVertexData<VertexLayout::PositionNormalTexcoord> planeVertexData(sizeof(float) * gPlaneVertices.size(), gPlaneVertices.data());
	LayoutVertexData<VertexLayout::PositionNormalTexcoord> cubeVertexData(sizeof(float) * gCubeVertices.size(), gCubeVertices.data());

	/* Load texture */
	Texture cubeTexture("./resource/box.jpg", 0);
	Texture planeTexture("./resource/floor.jpg", 1);

	/* Shader */
	Shader shader("glsl/general.vert", "glsl/general.frag");

	/* Camera */
	Camera camera(glm::vec3(-5.0f, 4.0f, -5.0f), glm::radians(-30.0f), glm::radians(45.0f), glm::vec3(0, 1.0f, 0));

	/* Object */
	std::vector<Object> opaqueObjects;
	{
		// Add plane
		opaqueObjects.emplace_back(&planeVertexData, &shader, &planeTexture);

		// Add cubes
		for (auto i = 0; i < gCubePositions.size(); i++)
		{
			Object cubeObj(&cubeVertexData, &shader, &cubeTexture);
			cubeObj.SetPosition(gCubePositions[i]);

			opaqueObjects.push_back(cubeObj);
		}
	}
	
	while (window.isOpen())
	{
		/* Window poll event */
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		/* Clear the screen */
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Prepare VP */
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(FOV), W_H_ratio, 0.1f, 100.0f);

#pragma region [Forward Pass]

		for (auto& renderObject : opaqueObjects)
		{
			// 1. Bind vertex attribute
			::glBindVertexArray(renderObject.GetVertexData()->GetVertexArrayHandle());

			// 2. Bind vertex array
			::glBindBuffer(GL_ARRAY_BUFFER, renderObject.GetVertexData()->GetVertexBufferHandle());

			// 3. Bind shader program
			::glUseProgram(renderObject.GetShader()->GetHandle());

			// 4. Upload uniforms
			renderObject.GetShader()->SetUniformMat4("model", renderObject.GetModelMatrix());
			renderObject.GetShader()->SetUniformMat4("view", view);
			renderObject.GetShader()->SetUniformMat4("projection", projection);
			renderObject.GetShader()->SetUniformInt("tex", renderObject.GetMainTexture()->GetSlot());

			// 5. Draw call
			::glDrawArrays(GL_TRIANGLES, 0, renderObject.GetVertexData()->GetTriangleNum());
		}

#pragma endregion

		/* Swap back buffer */
		window.display();
	}

	return 0;
}


