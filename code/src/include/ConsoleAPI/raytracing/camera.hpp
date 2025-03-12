#pragma once

#include "init.hpp"

class Camera {
    public:

        glm::vec3 position;
        glm::vec3 rotation;
        float fov;

        glm::vec3 getViewVector() const {
            glm::vec3 direction;
            float yaw = glm::radians(rotation.y);
            float pitch = glm::radians(rotation.x);
            direction.x = sin(yaw) * cos(pitch);  // Исправлено для оси X
            direction.y = sin(pitch);             // Тангаж (вверх/вниз)
            direction.z = -cos(yaw) * cos(pitch); // Отрицательная ось Z по умолчанию
            return glm::normalize(direction);
        }

        glm::mat4 getViewMatrix() const {
            glm::mat4 view = glm::mat4(1.0f);
            // Порядок поворотов: Yaw (Y), Pitch (X), Roll (Z)
            view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
            view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
            view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll
            view = glm::translate(view, -position);
            return view;
        }

        void rotateFromMouse(float dx, float dy, float sensitivity = 0.1f) {
            // Рыскание (вращение по горизонтали)
            rotation.y -= dx * sensitivity;
            
            // Тангаж (вращение по вертикали) с ограничением
            rotation.x = glm::clamp(
                rotation.x + dy * sensitivity,
                -89.0f,  // Минимальный угол (смотрим вниз)
                89.0f   // Максимальный угол (смотрим вверх)
            );
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