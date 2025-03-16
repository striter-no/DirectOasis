#pragma once

#include "gl_shader.hpp"
#include <functional>

struct Image {
    std::vector<std::vector<glm::vec4>> pixels;
};

class Renderer {

        DirectGLShader shader;
        GLuint texture;
        GLuint fbo;
        
        GLuint VAO, VBO, EBO;
        
        int width;
        int height;
    public:

        Image draw(std::function<void(DirectGLShader&)> update){

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT);
            
            shader.use();
            glBindVertexArray(VAO);
            update(shader);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            shader.stop();

            // Получение данных текстуры (например, в массив)
            std::vector<unsigned char> pixels(width * height * 4);
            glBindTexture(GL_TEXTURE_2D, texture);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

            glBindVertexArray(0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            Image image;
            image.pixels.resize(height);
            for (int y = 0; y < height; y++) {
                image.pixels[y].resize(width);
                for (int x = 0; x < width; x++) {
                    int index = (y * width + x) * 4; // 4 компоненты на пиксель (RGBA)
                    image.pixels[y][x] = glm::vec4(
                        pixels[index] / 255.0f,     // R
                        pixels[index + 1] / 255.0f, // G
                        pixels[index + 2] / 255.0f, // B
                        pixels[index + 3] / 255.0f  // A
                    );
                }
            }

            return image;
        }

        Renderer(DirectGLShader &shader, int width, int height): shader(shader), width(width), height(height) {
            if (width <= 0 || height <= 0) {
                throw std::runtime_error("Width/height must be > 0");
            }

            float vertices[]{
                // Координаты      // UV
                1.0f,  1.0f,     1.0f, 1.0f,
                1.0f, -1.0f,     1.0f, 0.0f,
                -1.0f, -1.0f,     0.0f, 0.0f,
                -1.0f,  1.0f,     0.0f, 1.0f
            };
            unsigned int indices[]{ 0, 1, 3, 1, 2, 3 };
            
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            // Атрибуты вершин
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // Создание текстуры
            
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            
            // Проверка текстуры
            GLenum texErr = glGetError();
            if (texErr != GL_NO_ERROR) {
                throw std::runtime_error("Texture creation failed: " + std::to_string(texErr));
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Создание FBO
            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

            // Проверка FBO
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                const GLubyte* version = glGetString(GL_VERSION);
                throw std::runtime_error("FBO incomplete: " + std::to_string(status) + " OpenGL version: " + std::string(reinterpret_cast<const char*>(version)));
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        Renderer(){}
        ~Renderer(){}
};