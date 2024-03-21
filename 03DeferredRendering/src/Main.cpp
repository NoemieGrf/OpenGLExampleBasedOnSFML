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

template<typename T>
using uptr = std::unique_ptr<T>;

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
    Shader geometryPassShader("glsl/geometryPass.vert", "glsl/geometryPass.frag");
    Shader lightningPassShader("glsl/deferredPass.vert", "glsl/deferredPass.frag");
    Shader forwardLightShader("glsl/light.vert", "glsl/light.frag");

    /* Material */
    Material planeMaterial{ 0.9f, 0.1f };
    Material cubeMaterial{ 0.8f, 0.5f };

    /* GBuffer */
    GBuffer gBuffer(WIDTH, HEIGHT, 0, 1, 2);

    /* Load texture */
    Texture cubeTexture("./resource/box.jpg", 3);
    Texture planeTexture("./resource/floor.jpg", 4);

    /* Geometry pass objects */
    std::vector<uptr<Object>> geometryPassObejcts;
    geometryPassObejcts.push_back(std::make_unique<Object>(&planeVertexData, &geometryPassShader, &planeTexture, &planeMaterial));
    for (auto i = 0; i < gCubePositions.size(); i++)
    {
        uptr<Object> pCube = std::make_unique<Object>(&cubeVertexData, &geometryPassShader, &cubeTexture, &cubeMaterial);
        pCube->SetPosition(gCubePositions[i]);

        geometryPassObejcts.push_back(std::move(pCube));
    }

    /* Light pass screen quad object */
    uptr<Object> pLightPassQuad = std::make_unique<Object>(&quadVertexData, &lightningPassShader);

    /* Forward pass objects */
    std::vector<uptr<Object>> pointLightObjects;
    for (unsigned int i = 0; i < gMultiLightPosition.size(); i++)
    {
        uptr<PointLightObject> pLight = std::make_unique<PointLightObject>(&cubeVertexData, &forwardLightShader);
        pLight->SetPosition(gMultiLightPosition[i]);
        pLight->SetScale(glm::vec3(0.2, 0.2, 0.2));
        pLight->SetColor(gMultiLightColor[i]);
        pLight->SetLinerAttenuation(0.5);
        pLight->SetQuadraticAttenuation(0.3);

        pointLightObjects.push_back(std::move(pLight));
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

        /* Prepare MVP */
        glm::mat4 model(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(FOV), W_H_ratio, 0.1f, 100.0f);

#pragma region [GEOMETRY PASS] Render geometry/color into GBuffer

        ::glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.GetFrameBufferHandle());

        ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto& pRenderObject : geometryPassObejcts)
        {
            // 1. Bind vertex attribute
            ::glBindVertexArray(pRenderObject->GetVertexData()->GetVertexArrayHandle());

            // 2. Bind vertex array
            ::glBindBuffer(GL_ARRAY_BUFFER, pRenderObject->GetVertexData()->GetVertexBufferHandle());

            // 3. Bind shader program
            ::glUseProgram(pRenderObject->GetShader()->GetHandle());

            // 4. Upload uniforms
            pRenderObject->GetShader()->SetUniformMat4("model", pRenderObject->GetModelMatrix());
            pRenderObject->GetShader()->SetUniformMat4("view", view);
            pRenderObject->GetShader()->SetUniformMat4("projection", projection);
            pRenderObject->GetShader()->SetUniformInt("tex", pRenderObject->GetMainTexture()->GetSlot());
            pRenderObject->GetShader()->SetUniformFloat("roughness", pRenderObject->GetMaterial()->roughness);
            pRenderObject->GetShader()->SetUniformFloat("metallic", pRenderObject->GetMaterial()->metallic);

            // 5. Draw call
            ::glDrawArrays(GL_TRIANGLES, 0, pRenderObject->GetVertexData()->GetTriangleNum());
        }

        ::glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion
         
#pragma region [LIGHT PASS] Render lighting

        ::glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            // 1. Bind vertex attribute
            ::glBindVertexArray(pLightPassQuad->GetVertexData()->GetVertexArrayHandle());

            // 2. Bind vertex array
            ::glBindBuffer(GL_ARRAY_BUFFER, pLightPassQuad->GetVertexData()->GetVertexBufferHandle());

            // 3. Bind shader program
            ::glUseProgram(pLightPassQuad->GetShader()->GetHandle());

            // 4. Upload shader parameters
            pLightPassQuad->GetShader()->SetUniformInt("gPosition", gBuffer.GetPositionTextureSlot());
            pLightPassQuad->GetShader()->SetUniformInt("gNormal", gBuffer.GetNormalTextureSlot());
            pLightPassQuad->GetShader()->SetUniformInt("gAlbedoSpec", gBuffer.GetAlbedoTextureSlot());
            pLightPassQuad->GetShader()->SetUniformVec3("viewPos", camera.GetPosition());

            for (unsigned int i = 0; i < pointLightObjects.size(); i++)
            {
                PointLightObject* pPointLight = dynamic_cast<PointLightObject*>(pointLightObjects[i].get());

                pLightPassQuad->GetShader()->SetUniformVec3("lights[" + std::to_string(i) + "].Position", pPointLight->GetPosition());
                pLightPassQuad->GetShader()->SetUniformVec3("lights[" + std::to_string(i) + "].Color", pPointLight->GetColor());
                pLightPassQuad->GetShader()->SetUniformFloat("lights[" + std::to_string(i) + "].Linear", pPointLight->GetLinerAttenuation());
                pLightPassQuad->GetShader()->SetUniformFloat("lights[" + std::to_string(i) + "].Quadratic", pPointLight->GetQuadraticAttenuation());
            }

            // 5. Draw call
            ::glDrawArrays(GL_TRIANGLES, 0, pLightPassQuad->GetVertexData()->GetTriangleNum());

        }

        ::glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.GetFrameBufferHandle());

        ::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        ::glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        ::glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion

#pragma region [FORWARD PASS] Render light cube

        for (auto& pRenderObject : pointLightObjects)
        {
            // 1. Bind vertex attribute
            ::glBindVertexArray(pRenderObject->GetVertexData()->GetVertexArrayHandle());

            // 2. Bind vertex array
            ::glBindBuffer(GL_ARRAY_BUFFER, pRenderObject->GetVertexData()->GetVertexBufferHandle());

            // 3. Bind shader program
            ::glUseProgram(pRenderObject->GetShader()->GetHandle());

            // 4. Upload uniforms
            pRenderObject->GetShader()->SetUniformMat4("model", pRenderObject->GetModelMatrix());
            pRenderObject->GetShader()->SetUniformMat4("view", view);
            pRenderObject->GetShader()->SetUniformMat4("projection", projection);

            PointLightObject* pPointLight = dynamic_cast<PointLightObject*>(pRenderObject.get());
            pRenderObject->GetShader()->SetUniformVec3("lightColor", pPointLight->GetColor());

            // 5. Draw call
            ::glDrawArrays(GL_TRIANGLES, 0, pRenderObject->GetVertexData()->GetTriangleNum());
        }

#pragma endregion

        /* Swap buffer */
        window.display();
    }

    return 0;
}