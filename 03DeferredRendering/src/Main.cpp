
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad/glad.hpp"
#include <sfml/Window.hpp>
#include <sfml/Graphics/Image.hpp>
#include <iostream>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Data.hpp"

void renderQuad();

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

    /* Camera & Shader */
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::radians(0.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));
    Shader shaderGeometryPass("glsl/geometryPass.vert", "glsl/geometryPass.frag");
    Shader deferredLightingPass("glsl/deferredPass.vert", "glsl/deferredPass.frag");

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

    /* Set GBuffer sampler 2d */
    deferredLightingPass.Bind();
    deferredLightingPass.SetUniformInt("gPosition", 0);
    deferredLightingPass.SetUniformInt("gNormal", 1);
    deferredLightingPass.SetUniformInt("gAlbedoSpec", 2);

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

        sf::Time elapsed = clock.getElapsedTime();

        /*
         * ********* GEOMETRY PASS **********
         * Render geometry/color into GBuffer
         * **********************************
         */
        ::glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderGeometryPass.Bind();
        shaderGeometryPass.SetUniformMat4("projection", projection);
        shaderGeometryPass.SetUniformMat4("view", view);

        for (unsigned int i = 0; i < 10; i++)
        {
            /* Set Model Matrix */
            model = glm::translate(glm::mat4(1.0f), gCubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle) * elapsed.asSeconds(), glm::vec3(1.0f, 0.3f, 0.5f));
            shaderGeometryPass.SetUniformMat4("model", model);

            /* Draw Call */
            ::glBindVertexArray(boxVertexArrayObject);
            ::glBindBuffer(GL_ARRAY_BUFFER, boxVertexBufferObject);
            ::glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        ::glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /*
         * ********** LIGHT PASS ************
         * Render lighting
         * **********************************
         */
        ::glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        deferredLightingPass.Bind();

        ::glActiveTexture(GL_TEXTURE0);
        ::glBindTexture(GL_TEXTURE_2D, gPosition);
        ::glActiveTexture(GL_TEXTURE1);
        ::glBindTexture(GL_TEXTURE_2D, gNormal);
        ::glActiveTexture(GL_TEXTURE2);
        ::glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

        deferredLightingPass.SetUniformVec3("viewPos", camera.GetPosition());

        for (unsigned int i = 0; i < 5; i++)
        {
            deferredLightingPass.SetUniformVec3("lights[" + std::to_string(i) + "].Position", gLightPos[i]);
            deferredLightingPass.SetUniformVec3("lights[" + std::to_string(i) + "].Color", gLightColor[i]);
            
            const float linear = 0.1f;
            const float quadratic = 1.0f;
            deferredLightingPass.SetUniformFloat("lights[" + std::to_string(i) + "].Linear", linear);
            deferredLightingPass.SetUniformFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        }

        renderQuad();

        ::glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        ::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        ::glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        ::glBindFramebuffer(GL_FRAMEBUFFER, 0);


        /* Swap buffer */
        window.display();
    }
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}