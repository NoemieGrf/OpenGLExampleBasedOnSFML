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
#include "GlobalData/Quad.h"
#include "FrameBuffer/GBuffer.h"

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

    sf::Window window(sf::VideoMode(WIDTH, HEIGHT, 32), "DeferredRendering", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setFramerateLimit(60);

    /* Glad load all openGL functions */
    ::gladLoadGL();

    /* OpenGL setting */
    ::glEnable(GL_DEPTH_TEST);
    ::glViewport(0, 0, WIDTH, HEIGHT);
    ::glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

    /* Objects VAO & VBO */
    LayoutVertexData<VertexLayout::PositionNormalTexcoord> planeVertexData(sizeof(float) * gPlaneVertices.size(), gPlaneVertices.data());
    LayoutVertexData<VertexLayout::PositionNormalTexcoord> cubeVertexData(sizeof(float) * gCubeVertices.size(), gCubeVertices.data());
    LayoutVertexData<VertexLayout::PositionTexcoord> quadVertexData(sizeof(float) * gQuadVertices.size(), gQuadVertices.data());

    /* Camera */
    Camera camera(glm::vec3(-5.0f, 4.0f, -5.0f), glm::radians(-30.0f), glm::radians(45.0f), glm::vec3(0, 1.0f, 0));

    /* Shader */
    Shader shaderGeometryPass("glsl/geometryPass.vert", "glsl/geometryPass.frag");
    Shader deferredLightingPass("glsl/deferredPass.vert", "glsl/deferredPass.frag");
    Shader lightShader("glsl/light.vert", "glsl/light.frag");

    /* Material */
    Material planeMaterial{ 0.9f, 0.1f };
    Material cubeMaterial{ 0.8f, 0.5f };

    /* GBuffer */
    GBuffer gBuffer(WIDTH, HEIGHT, 0, 1, 2);

    /* Load texture */
    Texture cubeTexture("./resource/box.jpg", 0);
    Texture planeTexture("./resource/floor.jpg", 1);

    /* Geometry pass objects */
    std::vector<Object> geometryPassObejcts;
    geometryPassObejcts.emplace_back(&planeVertexData, &shaderGeometryPass, &planeTexture, &planeMaterial);
    for (auto i = 0; i < gCubePositions.size(); i++)
    {
        Object cubeObj(&cubeVertexData, &shaderGeometryPass, &cubeTexture, &cubeMaterial);
        cubeObj.SetPosition(gCubePositions[i]);

        geometryPassObejcts.push_back(cubeObj);
    }

    /* Light pass screen quad object */
    Object lightPassQuad(&quadVertexData, &deferredLightingPass);

    /* Forward pass objects */
    std::vector<Object> geometryPassObejcts;

    

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

        /* Prepare MVP */
        glm::mat4 model(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(FOV), W_H_ratio, 0.1f, 100.0f);

#pragma region [GEOMETRY PASS] Render geometry/color into GBuffer

        ::glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.GetFrameBufferHandle());

        ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region [Draw Cube]
        
        // 1. Bind vertex attribute
        ::glBindVertexArray(cubeVertexArray);

        // 2. Bind vertex array
        ::glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);

        // 3. Bind shader program
        shaderGeometryPass.Bind();

        // 4. Upload shader parameters
        shaderGeometryPass.SetUniformMat4("projection", projection);
        shaderGeometryPass.SetUniformMat4("view", view);
        shaderGeometryPass.SetUniformMat4("model", glm::mat4(1.0f));
        shaderGeometryPass.SetUniformInt("tex", 0);

        // 5. Draw call
        for (auto i = 0; i < gCubePositions.size(); i++)
        {
            model = glm::translate(glm::mat4(1.0f), gCubePositions[i]);
            shaderGeometryPass.SetUniformMat4("model", model);

            // 5. Draw call
            ::glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
#pragma endregion

#pragma region [Draw Plane]

        // 1. Bind vertex attribute
        ::glBindVertexArray(planeVertexArray);

        // 2. Bind vertex array
        ::glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer);

        // 3. Bind shader program
        shaderGeometryPass.Bind();

        // 4. Upload shader parameters
        shaderGeometryPass.SetUniformMat4("projection", projection);
        shaderGeometryPass.SetUniformMat4("view", view);
        shaderGeometryPass.SetUniformMat4("model", glm::mat4(1.0f));
        shaderGeometryPass.SetUniformInt("tex", 1);

        // 5. Draw call
        ::glDrawArrays(GL_TRIANGLES, 0, 6);

#pragma endregion

        ::glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion
         
#pragma region [LIGHT PASS] Render lighting

        ::glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ::glActiveTexture(GL_TEXTURE0 + 2);
        ::glBindTexture(GL_TEXTURE_2D, gPosition);

        ::glActiveTexture(GL_TEXTURE0 + 3);
        ::glBindTexture(GL_TEXTURE_2D, gNormal);

        ::glActiveTexture(GL_TEXTURE0 + 4);
        ::glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

#pragma region [Draw Quad]

        // 1. Bind vertex attribute
        ::glBindVertexArray(quadVertexArray);

        // 2. Bind vertex array
        ::glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);

        // 3. Bind shader program
        deferredLightingPass.Bind();

        // 4. Upload shader parameters
        deferredLightingPass.SetUniformInt("gPosition", 2);
        deferredLightingPass.SetUniformInt("gNormal", 3);
        deferredLightingPass.SetUniformInt("gAlbedoSpec", 4);

        for (unsigned int i = 0; i < gMultiLightPosition.size(); i++)
        {
            deferredLightingPass.SetUniformVec3("lights[" + std::to_string(i) + "].Position", gMultiLightPosition[i]);
            deferredLightingPass.SetUniformVec3("lights[" + std::to_string(i) + "].Color", gMultiLightColor[i]);

            const float linear = 0.5f;
            const float quadratic = 0.3f;
            deferredLightingPass.SetUniformFloat("lights[" + std::to_string(i) + "].Linear", linear);
            deferredLightingPass.SetUniformFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        }

        ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#pragma endregion

        ::glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);

        ::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        ::glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        ::glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion

#pragma region [FORWARD PASS] Render light cube

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

        for (unsigned int i = 0; i < gMultiLightPosition.size(); i++)
        {
            model = glm::translate(glm::mat4(1.0f), gMultiLightPosition[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.SetUniformMat4("model", model);
            lightShader.SetUniformVec3("lightColor", gMultiLightColor[i]);

            // 5. Draw call
            ::glDrawArrays(GL_TRIANGLES, 0, 36);
        }

#pragma endregion 

#pragma endregion

        /* Swap buffer */
        window.display();
    }

    return 0;
}