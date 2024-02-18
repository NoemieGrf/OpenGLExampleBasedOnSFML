#include <iostream>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sfml/Window.hpp>
#include <sfml/Graphics/Image.hpp>

#include "glad/glad.h"

#include "Shader.h"
#include "Camera.h"
#include "Data.h"
#include "Util.hpp"

int main()
{
	/* Init SFML window */
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	const unsigned int WIDTH = 800;
	const unsigned int HEIGHT = 600;
	const float W_H_ratio = static_cast<float>(WIDTH) / HEIGHT;
	const float FOV = 45.0f;

	sf::Window window(sf::VideoMode(WIDTH, HEIGHT, 32), "Modern OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);

	window.setActive();
	window.setMouseCursorVisible(false);
	window.setMouseCursorGrabbed(true);

	/* Glad load all openGL functions */
	::gladLoadGL();

	/* OpenGL setting */
	::glEnable(GL_DEPTH_TEST);
	::glViewport(0, 0, WIDTH, HEIGHT);
	::glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	/* Camera & Shader */
	std::unique_ptr<Camera> pCamera = std::make_unique<Camera>(
		glm::vec3(0.0f, 0.0f, 3.0f), glm::radians(0.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));

	std::unique_ptr<Shader> pShader = std::make_unique<Shader>(
		"glsl/vertexSource.vert", "glsl/fragmentSource.frag");
	
	/* Generate vertex array and bind it */
	unsigned int vertexArrayObject;
	::glGenVertexArrays(1, &vertexArrayObject);
	::glBindVertexArray(vertexArrayObject);

	/* Generate vertex buffer and bind it */
	unsigned int vertexBufferObject;
	::glGenBuffers(1, &vertexBufferObject);
	::glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	/* Pass vertex buffer data form cpu to gpu */
	::glBufferData(GL_ARRAY_BUFFER, sizeof(gVertices), gVertices, GL_STATIC_DRAW);

	/* Open vertex attrib */
	::glEnableVertexAttribArray(0);
	::glEnableVertexAttribArray(1);
	::glEnableVertexAttribArray(2);
	::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	/* Load texture */
	unsigned int textureBuffer0 = Util::LoadImage("./resource/container.jpg", GL_RGB, GL_RGB, 0);
	unsigned int textureBuffer1 = Util::LoadImage("./resource/face.png", GL_RGBA, GL_RGBA, 3);
	
	/* Prepare MVP */
	glm::mat4 model(1.0f);
	glm::mat4 view = pCamera->getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(FOV), W_H_ratio, 0.1f, 100.0f);

	sf::Clock clock;
	float mouse_x = 0;
	float mouse_y = 0;
	bool mouse_first = true;

	while (window.isOpen())
	{
		/* Window poll event */
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseMoved)
			{
				if (mouse_first)
				{
					mouse_x = event.mouseMove.x;
					mouse_y = event.mouseMove.y;
					mouse_first = false;
				}

				auto [x, y] = event.mouseMove;
				float x_offset = mouse_x - x;
				float y_offset = y - mouse_y;

				std::cout << x << " " << y << " " << x_offset << " " << y_offset << std::endl;

				pCamera->ProcessMouseMovement(x_offset, y_offset);
				view = pCamera->getViewMatrix();

				mouse_x = x;
				mouse_y = y;
			}
		}

		/* Clear the screen */
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sf::Time elapsed = clock.getElapsedTime();
		float greenValue = (sin(elapsed.asSeconds()) / 2.0f) + 0.5f;
		
		/* Draw shapes */
		for (unsigned int i = 0; i < 10; i++)
		{
			/* Set Model Matrix */
			model = glm::translate(glm::mat4(1.0f), gCubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle) * elapsed.asSeconds(), glm::vec3(1.0f, 0.3f, 0.5f));
			
			/* Set Material */
			pShader->use();
			pShader->setInt("ourTexture", 0);
			pShader->setInt("ourFace", 3);
			pShader->setFloat("fragIn", greenValue);
			pShader->setMat4fv("model", model);
			pShader->setMat4fv("view", view);
			pShader->setMat4fv("projection", projection);

			/* Draw Call */
			::glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		window.display();
	}

	::glDeleteBuffers(1, &vertexBufferObject);
	::glDeleteVertexArrays(1, &vertexArrayObject);

	return EXIT_SUCCESS;
}


