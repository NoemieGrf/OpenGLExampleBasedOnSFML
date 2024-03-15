
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sfml/Window.hpp>
#include <sfml/Graphics/Image.hpp>

#include "Glad/Glad.h"
#include "Shader/Shader.h"
#include "Camera/Camera.h"
#include "GlobalData/Data.hpp"
#include "Utility/Util.hpp"

int main()
{
    /* Init SFML window */
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;

    const unsigned int WIDTH = 1600;
    const unsigned int HEIGHT = 900;
    const float W_H_ratio = static_cast<float>(WIDTH) / HEIGHT;
    const float FOV = 45.0f;

    sf::Window window(sf::VideoMode(WIDTH, HEIGHT, 32), "ForwardRendering", sf::Style::Titlebar | sf::Style::Close, settings);

    /* Glad load all openGL functions */
    ::gladLoadGL();

    /* OpenGL setting */
    ::glEnable(GL_DEPTH_TEST);
    ::glViewport(0, 0, WIDTH, HEIGHT);
    ::glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

    /* Camera & Shader */
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::radians(0.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));
    Shader boxShader("glsl/box.vert", "glsl/box.frag");

    /* Generate vertex array and bind it */
    unsigned int boxVertexArrayObject;
    ::glGenVertexArrays(1, &boxVertexArrayObject);
    ::glBindVertexArray(boxVertexArrayObject);

    /* Generate vertex buffer and bind it */
    unsigned int boxVertexBufferObject;
    ::glGenBuffers(1, &boxVertexBufferObject);
    ::glBindBuffer(GL_ARRAY_BUFFER, boxVertexBufferObject);

    /* Pass vertex buffer data form cpu to gpu */
    ::glBufferData(GL_ARRAY_BUFFER, sizeof(gVertices), gVertices, GL_STATIC_DRAW);

    /* Position & normal attribute */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* Prepare MVP */
    glm::mat4 model(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(FOV), W_H_ratio, 0.1f, 100.0f);

    /* render loop */
    sf::Clock clock;
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

        sf::Time elapsed = clock.getElapsedTime();

        boxShader.Bind();
        boxShader.SetUniformVec3("light.position", gLightPos);
        boxShader.SetUniformVec3("viewPos", camera.GetPosition());

        /* Light properties */
        glm::vec3 lightColor = glm::vec3{ 0.6f, 0.6f, 1.0f };
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        boxShader.SetUniformVec3("light.ambient", ambientColor);
        boxShader.SetUniformVec3("light.diffuse", diffuseColor);
        boxShader.SetUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);

        /* Material properties */
        boxShader.SetUniformVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        boxShader.SetUniformVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        boxShader.SetUniformVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
        boxShader.SetUniformFloat("material.shininess", 32.0f);

        /* VP */
        boxShader.SetUniformMat4("projection", projection);
        boxShader.SetUniformMat4("view", view);

        for (unsigned int i = 0; i < 10; i++)
        {
            /* Set Model Matrix */
            model = glm::translate(glm::mat4(1.0f), gCubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle) * elapsed.asSeconds(), glm::vec3(1.0f, 0.3f, 0.5f));
            boxShader.SetUniformMat4("model", model);

            /* Draw Call */
            ::glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        window.display();
    }

    ::glDeleteVertexArrays(1, &boxVertexArrayObject);
    ::glDeleteBuffers(1, &boxVertexBufferObject);

    return 0;
}