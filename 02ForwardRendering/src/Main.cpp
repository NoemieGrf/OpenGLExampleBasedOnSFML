
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sfml/Window.hpp>
#include <sfml/Graphics/Image.hpp>

#include "Glad/Glad.h"
#include "Shader/Shader.h"
#include "Camera/Camera.h"
#include "GlobalData/Plane.h"
#include "GlobalData/Cube.h"
#include "GlobalData/Light.h"
#include "Utility/Util.hpp"

using uint = unsigned int;

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

    /* Camera & Shader */
    Camera camera(glm::vec3(-5.0f, 4.0f, -5.0f), glm::radians(-30.0f), glm::radians(45.0f), glm::vec3(0, 1.0f, 0));
    Shader generalShader("glsl/general.vert", "glsl/general.frag");
    Shader lightShader("glsl/light.vert", "glsl/light.frag");

    /* render loop */
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
        glm::mat4 model(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(FOV), W_H_ratio, 0.1f, 100.0f);

#pragma region [Draw Light]

        // 1. Bind vertex attribute
        ::glBindVertexArray(cubeVertexArray);

        // 2. Bind vertex array
        ::glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);

        // 3. Bind shader program
        lightShader.Bind();

        // 4. Upload shader parameters
        lightShader.SetUniformMat4("view", view);
        lightShader.SetUniformMat4("projection", projection);
        model = glm::translate(glm::mat4(1.0f), gSingleLightPosition);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.SetUniformMat4("model", model);

        // 5. Draw call
        ::glDrawArrays(GL_TRIANGLES, 0, 36);

#pragma endregion 

#pragma region [Draw Plane]

        {
            // 1. Bind vertex attribute
            ::glBindVertexArray(planeVertexArray);

            // 2. Bind vertex array
            ::glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer);

            // 3. Bind shader program
            generalShader.Bind();

            // 4. Upload shader parameters
            generalShader.SetUniformInt("tex", 1);
            generalShader.SetUniformVec3("light.position", gSingleLightPosition);
            generalShader.SetUniformVec3("viewPos", camera.GetPosition());

            /* Light properties */
            glm::vec3 lightColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
            glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
            glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
            generalShader.SetUniformVec3("light.ambient", ambientColor);
            generalShader.SetUniformVec3("light.diffuse", diffuseColor);
            generalShader.SetUniformVec3("light.specular", 0.6f, 0.6f, 0.6f);

            /* Material properties */
            generalShader.SetUniformVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
            generalShader.SetUniformVec3("material.specular", 0.6f, 0.6f, 0.6f);
            generalShader.SetUniformFloat("material.shininess", 16.0f);

            /* MVP */
            generalShader.SetUniformMat4("model", glm::mat4(1.0f));
            generalShader.SetUniformMat4("view", view);
            generalShader.SetUniformMat4("projection", projection);

            // 5. Draw call
            ::glDrawArrays(GL_TRIANGLES, 0, 6);
        }

#pragma endregion

#pragma region [Draw Cube]

        {
            // 1. Bind vertex attribute
            ::glBindVertexArray(cubeVertexArray);

            // 2. Bind vertex array
            ::glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);

            // 3. Bind shader program
            generalShader.Bind();

            // 4. Upload shader parameters
            generalShader.SetUniformInt("tex", 0);
            generalShader.SetUniformVec3("light.position", gSingleLightPosition);
            generalShader.SetUniformVec3("viewPos", camera.GetPosition());

            /* Light properties */
            glm::vec3 lightColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
            glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
            glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
            generalShader.SetUniformVec3("light.ambient", ambientColor);
            generalShader.SetUniformVec3("light.diffuse", diffuseColor);
            generalShader.SetUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);

            /* Material properties */
            generalShader.SetUniformVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
            generalShader.SetUniformVec3("material.specular", 1.0f, 1.0f, 1.0f); // specular lighting doesn't have full effect on this object's material
            generalShader.SetUniformFloat("material.shininess", 32.0f);

            /* VP */
            generalShader.SetUniformMat4("projection", projection);
            generalShader.SetUniformMat4("view", view);

            for (auto i = 0; i < gCubePositions.size(); i++)
            {
                model = glm::translate(glm::mat4(1.0f), gCubePositions[i]);
                generalShader.SetUniformMat4("model", model);

                // 5. Draw call
                ::glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

#pragma endregion    

        window.display();
    }

    ::glDeleteBuffers(1, &cubeVertexBuffer);
    ::glDeleteBuffers(1, &planeVertexBuffer);

    ::glDeleteVertexArrays(1, &planeVertexArray);
    ::glDeleteVertexArrays(1, &cubeVertexArray);

    ::glDeleteTextures(1, &boxTexture);
    ::glDeleteTextures(1, &planeTexture);

    return 0;
}