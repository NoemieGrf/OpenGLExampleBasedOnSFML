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
#include "Utility/Util.hpp"

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

	/* [VBO] Create vertex buffer for plane */
	uint planeVertexBuffer;	// this is a handle
	::glGenBuffers(1, &planeVertexBuffer);
	::glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer); 
	::glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gPlaneVertices.size(), gPlaneVertices.data(), GL_STATIC_DRAW);

	/* [VAO] Generate vertex array for plane and bind it */
	uint planeVertexArray;
	::glGenVertexArrays(1, &planeVertexArray);
	::glBindVertexArray(planeVertexArray);
	/* Open vertex attrib */
	::glEnableVertexAttribArray(0);
	::glEnableVertexAttribArray(1);
	::glEnableVertexAttribArray(2);
	::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	/* [VBO] Create vertex buffer for cube */
	uint cubeVertexBuffer;
	::glGenBuffers(1, &cubeVertexBuffer);
	::glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
	::glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gCubeVertices.size(), gCubeVertices.data(), GL_STATIC_DRAW);
	
	/* [VAO] Generate vertex array for plane and bind it */
	uint cubeVertexArray;
	::glGenVertexArrays(1, &cubeVertexArray);
	::glBindVertexArray(cubeVertexArray);
	/* Open vertex attrib */
	::glEnableVertexAttribArray(0);
	::glEnableVertexAttribArray(1);
	::glEnableVertexAttribArray(2);
	::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	/* Load texture */
	uint boxTexture = Util::LoadImage("./resource/box.jpg", GL_RGBA, GL_RGBA, 0);
	uint planeTexture = Util::LoadImage("./resource/floor.jpg", GL_RGBA, GL_RGBA, 1);

	/* Camera */
	Camera camera(glm::vec3(-5.0f, 4.0f, -5.0f), glm::radians(-30.0f), glm::radians(45.0f), glm::vec3(0, 1.0f, 0));

	/* Shader */
	Shader shader("glsl/general.vert", "glsl/general.frag");
	
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

		/* Prepare MVP */
		glm::mat4 model(1.0f);	// model针对每个模型，画之前重新算
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(FOV), W_H_ratio, 0.1f, 100.0f);

#pragma region [Draw Plane]

		// 1. Bind vertex attribute
		::glBindVertexArray(planeVertexArray);

		// 2. Bind vertex array
		::glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer);

		// 3. Bind shader program
		shader.Bind();

		// 4. Upload shader parameters
		shader.SetUniformMat4("model", model);
		shader.SetUniformMat4("view", view);
		shader.SetUniformMat4("projection", projection);
		shader.SetUniformInt("tex", 1);

		// 5. Draw call
		::glDrawArrays(GL_TRIANGLES, 0, 6);

#pragma endregion

#pragma region [Draw Cube]
		
		// 1. Bind vertex attribute
		::glBindVertexArray(cubeVertexArray);
		
		// 2. Bind vertex array
		::glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);

		// 3. Bind shader program
		shader.Bind();

		// 4. Upload shader parameters
		shader.SetUniformMat4("view", view);
		shader.SetUniformMat4("projection", projection);
		shader.SetUniformInt("tex", 0);

		for (auto i = 0; i < gCubePositions.size(); i++)
		{
			model = glm::translate(glm::mat4(1.0f), gCubePositions[i]);
			shader.SetUniformMat4("model", model);

			// 5. Draw call
			::glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
#pragma endregion

		/* Swap back buffer */
		window.display();
	}

	::glDeleteBuffers(1, &cubeVertexBuffer);
	::glDeleteBuffers(1, &planeVertexBuffer);
	::glDeleteTextures(1, &boxTexture);
	::glDeleteTextures(1, &planeTexture);
	::glDeleteVertexArrays(1, &planeVertexArray);
	::glDeleteVertexArrays(1, &cubeVertexArray);

	return 0;
}


