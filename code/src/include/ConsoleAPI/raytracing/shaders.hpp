#pragma once

#define MAX_DIST 99999.f

#include "camera.hpp"
#include "figures.hpp"
#include "intersects.hpp"
#include "light.hpp"

#include <unordered_map>
#include <utils/vector.hpp>
#include <functional>
#include <memory>

struct Object {
    
    std::shared_ptr<Figure> figure;
    Object(std::shared_ptr<Figure> figure) 
        : figure(figure) {}
    Object() : figure(nullptr) {} // Инициализация указателя
};

class Shader {
        Camera camera;
        
        std::vector<Object> objects;
        std::function<void(Ray&, Camera&, std::vector<Object>&)> body;

        std::unordered_map<std::string, glm::vec2> vec2_uniforms;
        std::unordered_map<std::string, glm::vec3> vec3_uniforms;
        std::unordered_map<std::string, glm::vec4> vec4_uniforms;
        std::unordered_map<std::string, glm::mat2> mat2_uniforms;


    public:
        DirectionalLight light;

        Camera &getCamera() { return camera; }
        Object& addObject(Object object) { 
            objects.push_back(object);
            return objects.back();
        }

        void setUniform(std::string name, glm::vec2 value){ vec2_uniforms[name] = value; }
        void setUniform(std::string name, glm::vec3 value){ vec3_uniforms[name] = value; }
        void setUniform(std::string name, glm::vec4 value){ vec4_uniforms[name] = value; }
        void setUniform(std::string name, glm::mat2 value){ mat2_uniforms[name] = value; }

        void setMainBody(
            std::function<void(Ray&, Camera&, std::vector<Object>&)> body
        ){ this->body = body; }

        void proceed(
            float x,
            float y,
            float aspectRatio,
            Ray &ray
        ){
            float normalizedX = (2.0f * x - 1.0f) * aspectRatio;
            float normalizedY = 1.0f - 2.0f * y;

            glm::vec3 no_cam_dir = glm::vec3(1.0, normalizedX, normalizedY);

            ray.origin = vec3_uniforms["cam_pos"];
            ray.direction = glm::normalize(no_cam_dir);
            
            auto zx = glm::vec2{ray.direction.z, ray.direction.x};
            zx = zx * mat2_uniforms["ux_mouse"];

            auto xy = glm::vec2{zx.y, ray.direction.y};
            xy = xy * mat2_uniforms["uy_mouse"];

            ray.direction = glm::vec3(xy.x, xy.y, zx.x);
            body(ray, camera, objects);
        }

        Shader(){
            setMainBody([&](
                Ray& ray, Camera& camera, std::vector<Object>& objects
            ){
                ray.minIt = glm::vec2{MAX_DIST};
                for (const auto& object : objects) {
                    object.figure->intersect(ray);
                }

                float diffuse = glm::max(glm::dot(glm::normalize(light.direction), ray.intersectNormal), 0.01f) * 0.5f;
                float specular = glm::max(0.0f, glm::dot(glm::reflect(ray.direction, ray.intersectNormal), light.direction));
                
                float lightIntensity = diffuse + glm::pow(specular, 32);

                if (ray.minIt.x == MAX_DIST) ray.color = glm::vec3(0.3f, 0.4f, 0.7f);
                else ray.color = ray.intersectMaterial.color * glm::min(lightIntensity, 1.f);
            });
        }
        ~Shader(){}
};
// glm::vec3 direction = forward * glm::vec3(0.0f, 0.0f, -1.0f) + right * normalizedX + up * normalizedY;
// ray.t = std::numeric_limits<float>::infinity();