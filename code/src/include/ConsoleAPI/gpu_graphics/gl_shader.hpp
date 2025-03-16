#pragma once

#include "headless_gl.hpp"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <utils/files.hpp>
#include <utils/vector.hpp>

class DirectGLShader {

        uint compileShader(uint type, std::string source){
            uint id = glCreateShader(type);
            const char *src = source.c_str();
            glShaderSource(id, 1, &src, NULL);
            glCompileShader(id);

            int result;
            glGetShaderiv(id, GL_COMPILE_STATUS, &result);

            if (!result){
                int length;
                glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
                std::string message;
                message.resize(length);

                glGetShaderInfoLog(id, length, &length, &message[0]);
                throw std::runtime_error("Failed to compile shader (" + std::string(type == GL_FRAGMENT_SHADER ? "fragment": (type == GL_VERTEX_SHADER ? "vertex": "unknown")) + ')' + message);
            }

            return id;
        }

        uint loadShader(std::string shaderName, std::string fragment=""){
            std::string vertexShaderCode = utils::fls::getFile(shaderName+".vert");
            std::string fragmentShaderCode = utils::fls::getFile(fragment.empty() ? shaderName + ".frag": fragment + ".frag");

            uint program = glCreateProgram();
            uint vertexs = compileShader(GL_VERTEX_SHADER, vertexShaderCode);
            uint frags   = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

            glAttachShader(program, vertexs);
            glAttachShader(program, frags);
            glLinkProgram(program);
            glValidateProgram(program);

            glDeleteShader(vertexs);
            glDeleteShader(frags);

            return program;
        }

        uint program = 0;

    public:
        void use(){
            glUseProgram(program);
        }

        void stop(){
            glUseProgram(0);
        }

        void uniform(std::string name, glm::vec2 value){
            int location = glGetUniformLocation(program, name.c_str());
            glUniform2fv(location, 1, &value[0]);
        }

        void uniform(std::string name, glm::vec3 value){
            int location = glGetUniformLocation(program, name.c_str());
            glUniform3fv(location, 1, &value[0]);
        }

        void uniform(std::string name, glm::vec4 value){
            int location = glGetUniformLocation(program, name.c_str());
            glUniform4fv(location, 1, &value[0]);
        }

        void uniform(std::string name, glm::mat2 value){
            int location = glGetUniformLocation(program, name.c_str());
            glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }

        void uniform(std::string name, glm::mat3 value){
            int location = glGetUniformLocation(program, name.c_str());
            glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }

        void uniform(std::string name, glm::mat4 value){
            int location = glGetUniformLocation(program, name.c_str());
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }

        void uniform(std::string name, int value){
            int location = glGetUniformLocation(program, name.c_str());
            glUniform1i(location, value);
        }

        void uniform(std::string name, float value){
            int location = glGetUniformLocation(program, name.c_str());
            glUniform1f(location, value);
        }
        
        DirectGLShader(std::string path){
            program = loadShader(path);
        }

        DirectGLShader(std::string vert, std::string frag){
            program = loadShader(vert, frag);
        }

        DirectGLShader(){}
        ~DirectGLShader(){}
};