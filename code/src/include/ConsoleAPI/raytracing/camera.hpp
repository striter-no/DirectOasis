#pragma once

#include "init.hpp"

class Camera {
    public:

        glm::vec3 position;
        glm::vec3 rotation;
        float fov;

        glm::vec3 getViewVector() const {
            glm::vec3 direction(
                cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x)),
                sin(glm::radians(rotation.y)),
                cos(glm::radians(rotation.y)) * sin(glm::radians(rotation.x))
            );
            return -direction;
        }

        void moveForward(float dt){
            position += getViewVector() * dt;
        }

        void moveRight(float dt){
            position += glm::cross(getViewVector(), glm::vec3(0.0f, 1.0f, 0.0f)) * dt;
        }

        void moveUp(float dt){
            position += glm::vec3(0.0f, 1.0f, 0.0f) * dt;
        }

        void rotate(float dx, float dy, float dz){
            rotation.x += dx;
            rotation.y += dy;
            rotation.z += dz;
        }

        Camera(
            glm::vec3 position,
            glm::vec3 rotation
        ): position(position), rotation(rotation)  {}

        Camera(){}
        ~Camera(){}
};