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
#include "GlobalData/Light.h"
#include "Object/Object.h"
#include "Vertex/VertexData.hpp"
#include "Texture/Texture.h"
#include "Material/Material.h"

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

    /* Objects VAO & VBO */
    LayoutVertexData<VertexLayout::PositionNormalTexcoord> planeVertexData(sizeof(float) * gPlaneVertices.size(), gPlaneVertices.data());
    LayoutVertexData<VertexLayout::PositionNormalTexcoord> cubeVertexData(sizeof(float) * gCubeVertices.size(), gCubeVertices.data());

    /* Load texture */
    Texture cubeTexture("./resource/box.jpg", 0);
    Texture planeTexture("./resource/floor.jpg", 1);

    /* Shader */
    Shader generalShader("glsl/general.vert", "glsl/general.frag");
    Shader lightShader("glsl/light.vert", "glsl/light.frag");

    /* Material */
    Material planeMaterial{ 0.9f, 0.1f };
    Material cubeMaterial{ 0.8f, 0.5f };

    /* Camera */
    Camera camera(glm::vec3(-5.0f, 4.0f, -5.0f), glm::radians(-30.0f), glm::radians(45.0f), glm::vec3(0, 1.0f, 0));

    /* Light shading objects */
    std::vector<Object> lightShadingOpaqueObjects;
    {
        // Add palne
        lightShadingOpaqueObjects.emplace_back(&planeVertexData, &generalShader, &planeTexture, &planeMaterial);

        // Add cubes
        for (auto i = 0; i < gCubePositions.size(); i++)
        {
            Object cubeObj(&cubeVertexData, &generalShader, &cubeTexture, &cubeMaterial);
            cubeObj.SetPosition(gCubePositions[i]);

            lightShadingOpaqueObjects.push_back(cubeObj);
        }
    }

    /* Unlit shading objects */
    std::vector<Object> unlitShadingOpaqueObjects;
    {
        Object cubeObj(&cubeVertexData, &lightShader);
        cubeObj.SetPosition(gSingleLightPosition);
        cubeObj.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
        unlitShadingOpaqueObjects.push_back(cubeObj);
    }
    
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

        /* Prepare VP */
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(FOV), W_H_ratio, 0.1f, 100.0f);

#pragma region [Forward Pass]

        /* Draw light shading objects */
        for (auto& renderObject : lightShadingOpaqueObjects)
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
            
            renderObject.GetShader()->SetUniformVec3("light.color", glm::vec3{ 1.0f, 1.0f, 1.0f });
            renderObject.GetShader()->SetUniformVec3("light.position", gSingleLightPosition);
            renderObject.GetShader()->SetUniformFloat("material.roughness", renderObject.GetMaterial()->roughness);
            renderObject.GetShader()->SetUniformFloat("material.matellic", renderObject.GetMaterial()->metallic);

            // 5. Draw call
            ::glDrawArrays(GL_TRIANGLES, 0, renderObject.GetVertexData()->GetTriangleNum());
        }

        /* Draw unlit shading objects */
        for (auto& renderObject : unlitShadingOpaqueObjects)
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

            // 5. Draw call
            ::glDrawArrays(GL_TRIANGLES, 0, renderObject.GetVertexData()->GetTriangleNum());
        }

#pragma endregion

        window.display();
    }

    return 0;
}