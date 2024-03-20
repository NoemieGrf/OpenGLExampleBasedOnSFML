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
#include "Utility/RenderCommand.hpp"
#include "Object/Object.hpp"
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
	Object<VertexLayout::PositionNormalTexcoord> plane(sizeof(float) * gPlaneVertices.size(), gPlaneVertices.data());
	Object<VertexLayout::PositionNormalTexcoord> cube(sizeof(float) * gCubeVertices.size(), gCubeVertices.data());

	/* Load texture */
	Texture boxTexture("./resource/box.jpg", 0);
	Texture planeTexture("./resource/floor.jpg", 1);

	/* Shader */
	Shader shader("glsl/general.vert", "glsl/general.frag");

	/* Camera */
	Camera camera(glm::vec3(-5.0f, 4.0f, -5.0f), glm::radians(-30.0f), glm::radians(45.0f), glm::vec3(0, 1.0f, 0));
	
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

		/* Draw plane */
		RenderCommand::DrawPrepare(plane, shader);

		shader.SetUniformMat4("model", glm::mat4(1.0f));
		shader.SetUniformMat4("view", view);
		shader.SetUniformMat4("projection", projection);
		shader.SetUniformInt("tex", planeTexture.GetSlot());

		RenderCommand::DrawObject(plane);

		/* Draw box */
		RenderCommand::DrawPrepare(cube, shader);

		shader.SetUniformMat4("view", view);
		shader.SetUniformMat4("projection", projection);
		shader.SetUniformInt("tex", boxTexture.GetSlot());

		for (auto i = 0; i < gCubePositions.size(); i++)
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), gCubePositions[i]);
			shader.SetUniformMat4("model", model);

			RenderCommand::DrawObject(cube);
		}

#pragma endregion

		/* Swap back buffer */
		window.display();
	}

	return 0;
}


