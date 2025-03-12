#pragma once

#include "camera.hpp"
#include "material.hpp"
#include "figures.hpp"
#include "intersects.hpp"
#include "light.hpp"

#include <utils/vector.hpp>
#include <functional>
#include <memory>

struct Object {
    Material material;
    std::shared_ptr<Figure> figure;
    Object(Material material, std::shared_ptr<Figure> figure) 
        : material(material), figure(figure) {}
    Object() : figure(nullptr) {} // Инициализация указателя
};

class Shader {
        Camera camera;
        
        std::vector<Object> objects;
        std::function<bool(Ray&, Camera&, std::vector<Object>&)> body;

    public:
        DirectionalLight light;

        Camera &getCamera() { return camera; }
        void addObject(Object object) { objects.push_back(object); }

        void setMainBody(
            std::function<bool(Ray&, Camera&, std::vector<Object>&)> body
        ){ this->body = body; }

        bool proceed(
            float x,
            float y,
            float aspectRatio,
            Ray &ray
        ){
            float normalizedX = (2.0f * x - 1.0f) * aspectRatio;
            float normalizedY = 1.0f - 2.0f * y;

            glm::vec3 forward = camera.getViewVector();
            glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 up = glm::normalize(glm::cross(right, forward));

            // Рассчитываем направление луча
            glm::vec3 direction = 
                forward * glm::vec3(0.0f, 0.0f, -1.0f) + 
                right * normalizedX + 
                up * normalizedY;

            glm::vec3 no_cam_dir = glm::vec3(normalizedX, normalizedY, -1.0f);

            ray.origin = camera.position; 
            ray.direction = glm::normalize(no_cam_dir);
            ray.t = std::numeric_limits<float>::infinity();

            return body(ray, camera, objects);
        }

        Shader(){
            setMainBody([&](
                Ray& ray, Camera& camera, std::vector<Object>& objects
            ){
                for (const auto& object : objects) {
                    if (object.figure->intersect(ray)) {
                        float diffuseIntensity = glm::max(glm::dot(ray.intersectNormal, light.direction), 0.1f);
                        ray.color.r = object.material.color.r * diffuseIntensity;
                        ray.color.g = object.material.color.g * diffuseIntensity;
                        ray.color.b = object.material.color.b * diffuseIntensity;
                        return true;
                    }
                }

                return false;
            });
        }
        ~Shader(){}
};