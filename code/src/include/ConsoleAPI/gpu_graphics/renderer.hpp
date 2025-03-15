#pragma once

#include "gl_shader.hpp"
#include <functional>

class Renderer {

        Shader shader;

    public:

        void draw(std::function<void()> update){
            shader.use();
                update();
                glBegin(GL_QUADS);
                    glVertex2f(-1.0f, -1.0f);
                    glVertex2f(-1.0f, 1.0f);
                    glVertex2f(1.0f, 1.0f);
                    glVertex2f(1.0f, -1.0f);
                glEnd();
            shader.stop();
        }

        Renderer(Shader &shader): shader(shader)  {}

        Renderer(){}
        ~Renderer(){}
};