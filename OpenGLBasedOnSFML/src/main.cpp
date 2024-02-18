#include <iostream>
#include <string>

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

	// 初始化Window窗口
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	const unsigned int WIDTH = 800;
	const unsigned int HEIGHT = 600;
	const float W_H_ratio = static_cast<float>(WIDTH) / HEIGHT;
	const float FOV = 45.0f;
	const sf::String TITLE = "Modern OpenGL";
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT, 32), TITLE, sf::Style::Titlebar | sf::Style::Close, settings);

	window.setActive();
	window.setMouseCursorVisible(false);
	window.setMouseCursorGrabbed(true);

	::gladLoadGL();

	::glEnable(GL_DEPTH_TEST);

	// camera
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::radians(0.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));

	glViewport(0, 0, 800, 600); // lower left

	// shader
	Shader* testShader = new Shader("glsl/vertexSource.vert", "glsl/fragmentSource.frag");
	
	
#pragma region VAO&VBO
	/* 产生VAO，绑定 */
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);		// 第一个参数是生成多少个VAO，第二个参数是生成的ID返还到哪里
	glBindVertexArray(VAO);

	/* 产生VBO，绑定 */
	unsigned int VBO;
	glGenBuffers(1, &VBO);			// 第一个参数是生成多少个VBO，第二个参数是生成的ID返还到哪里
	glBindBuffer(GL_ARRAY_BUFFER, VBO);		// 第一个参数是要绑定Array Buffer，第二个参数是绑定哪个VBO

	/* 接下来将用户数据绑定到buffer */
	// 第一个参数是绑定到谁，第二个参数数据多少Bytes，第三个参数是具体的数组，第四个参数是显卡如何处理
	// GL_STATIC_DRAW 这样的处理方式数据几乎不会变化
	// GL_DYNAMIC_DRAW 这样处理方式的数据一般经常变动
	// GL_STREAM_DRAW 这种情况下数据每次绘制都会变
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/* 链接顶点属性 */
	// 第一个参数从第几个开始绘制， 第二个参数尺寸是多少， 第三个参数是什么类型
	// 第四个参数是要不要归一化到±1之间，第五个参数为每笔数据之间的间隔是多少，第六个参数为第一笔数据的偏移量是多少
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	/* 打开定点属性 */
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
#pragma endregion

	// texture
	unsigned int texture_buffer_0 = Util::loadImage("./resource/container.jpg", GL_RGB, GL_RGB, 0);
	unsigned int texture_buffer_1 = Util::loadImage("./resource/face.png", GL_RGBA, GL_RGBA, 3);
	
#pragma region MVP matrices
	
	glm::mat4 model(1.0f);
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection(1.0f);
	projection = glm::perspective(glm::radians(FOV), W_H_ratio, 0.1f, 100.0f);
	
#pragma endregion 
	
	
	// 这个While循环是SFML的固定模式用于做事件处理
	sf::Clock clock;
	float mouse_x = 0;
	float mouse_y = 0;
	bool mouse_first = true;
	while (window.isOpen())
	{
		// 内层While循环用于处理事件响应
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
				camera.ProcessMouseMovement(x_offset, y_offset);
				view = camera.getViewMatrix();
				mouse_x = x;
				mouse_y = y;
			}
		}

		// 清屏
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 处理
		sf::Time elapsed = clock.getElapsedTime();
		float greenValue = (sin(elapsed.asSeconds()) / 2.0f) + 0.5f;
		
		// 绘制图形
		for (unsigned int i = 0; i < 10; i++)
		{
			
			// Set Model Matrix
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle) * elapsed.asSeconds(), glm::vec3(1.0f, 0.3f, 0.5f));
			// Set View Matrix
			// Set Projection Matrix
			
			// Set Material -> Shader Program
			testShader->use();
			// Set Material -> Textures
			// Set Material -> Uniforms
			testShader->setInt("ourTexture", 0);
			testShader->setInt("ourFace", 3);
			testShader->setFloat("fragIn", greenValue);
			testShader->setMat4fv("model", model);
			testShader->setMat4fv("view", view);
			testShader->setMat4fv("projection", projection);

			

			// Draw Call
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		window.display();
	}
	// 释放资源

	delete testShader;

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	return EXIT_SUCCESS;
}


