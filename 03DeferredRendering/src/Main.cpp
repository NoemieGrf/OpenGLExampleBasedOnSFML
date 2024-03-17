
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
#include "GlobalData/Quad.h"
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

    sf::Window window(sf::VideoMode(WIDTH, HEIGHT, 32), "DeferredRendering", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setFramerateLimit(60);

    /* Glad load all openGL functions */
    ::gladLoadGL();

    /* OpenGL setting */
    ::glEnable(GL_DEPTH_TEST);
    ::glViewport(0, 0, WIDTH, HEIGHT);
    ::glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

#pragma region [Plane VBO & VAO]

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

#pragma endregion

#pragma region [Box VBO & VAO]

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

#pragma endregion

#pragma region [Quad VBO & VAO]

    /* [VBO] Create vertex buffer for quad */
    uint quadVertexBuffer;
    ::glGenBuffers(1, &quadVertexBuffer);
    ::glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
    ::glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gQuadVertices.size(), gQuadVertices.data(), GL_STATIC_DRAW);

    /* [VAO] Generate vertex array for quad and bind it */
    uint quadVertexArray;
    ::glGenVertexArrays(1, &quadVertexArray);
    ::glBindVertexArray(quadVertexArray);
    /* Open vertex attrib */
    ::glEnableVertexAttribArray(0);
    ::glEnableVertexAttribArray(1);
    ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    ::glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

#pragma endregion

    /* Camera */
    Camera camera(glm::vec3(-5.0f, 4.0f, -5.0f), glm::radians(-30.0f), glm::radians(45.0f), glm::vec3(0, 1.0f, 0));

    /* Shader */
    Shader shaderGeometryPass("glsl/geometryPass.vert", "glsl/geometryPass.frag");
    Shader deferredLightingPass("glsl/deferredPass.vert", "glsl/deferredPass.frag");
    Shader lightShader("glsl/light.vert", "glsl/light.frag");

#pragma region [GBuffer]

    /* Create GBuffer */
    unsigned int gBuffer;
    ::glGenFramebuffers(1, &gBuffer);
    ::glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    /* Position GBuffer */
    unsigned int gPosition;
    ::glGenTextures(1, &gPosition);
    ::glBindTexture(GL_TEXTURE_2D, gPosition);
    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    /* Normal GBuffer */
    unsigned int gNormal;
    ::glGenTextures(1, &gNormal);
    ::glBindTexture(GL_TEXTURE_2D, gNormal);
    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    /* Color & Specular GBuffer */
    unsigned int gAlbedoSpec;
    ::glGenTextures(1, &gAlbedoSpec);
    ::glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    /* Set attachments */
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    ::glDrawBuffers(3, attachments);

    /* Depth buffer */
    unsigned int rboDepth;
    ::glGenRenderbuffers(1, &rboDepth);
    ::glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    ::glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    /* Unbind frame buffer */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion

    /* Load texture */
    uint boxTexture = Util::LoadImage("./resource/box.jpg", GL_RGBA, GL_RGBA, 0);
    uint planeTexture = Util::LoadImage("./resource/floor.jpg", GL_RGBA, GL_RGBA, 1);

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

        ::glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

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
            deferredLightingPass.SetUniformVec3("lights[" + std::to_string(i) + "].Color", gMultiLightPosition[i]);

            const float linear = 0.1f;
            const float quadratic = 1.0f;
            deferredLightingPass.SetUniformFloat("lights[" + std::to_string(i) + "].Linear", linear);
            deferredLightingPass.SetUniformFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        }

        ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#pragma endregion

        ::glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);

        ::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        ::glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        ::glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
            lightShader.SetUniformVec3("lightColor", gMultiLightPosition[i]);

            // 5. Draw call
            ::glDrawArrays(GL_TRIANGLES, 0, 36);
        }

#pragma endregion 

#pragma endregion

        /* Swap buffer */
        window.display();
    }
}