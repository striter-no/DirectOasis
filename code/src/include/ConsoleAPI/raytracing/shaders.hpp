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

#include <MathLib/Random.hpp>

struct Object {
    
    std::shared_ptr<Figure> figure;
    Object(std::shared_ptr<Figure> figure) 
        : figure(figure) {}
    Object() : figure(nullptr) {} // Инициализация указателя
};

enum GRAPHICS_TYPE {
    RAY_TRACING,
    RAY_CASTING
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
            zx = zx * mat2_uniforms["uy_mouse"];

            auto xy = glm::vec2{zx.y, ray.direction.y};
            xy = xy * mat2_uniforms["ux_mouse"];

            ray.direction = glm::vec3(xy.x, xy.y, zx.x);
            body(ray, camera, objects);
        }

        Shader(GRAPHICS_TYPE gtype){
            auto ray_casting = [&](
                Ray& ray, Camera& camera, std::vector<Object>& objects
            ){
                auto getSky = [](glm::vec3 rd, glm::vec3 light){
                    glm::vec3 col = glm::vec3(0.3, 0.6, 1.0);
                    glm::vec3 sun = glm::vec3(0.95, 0.9, 1.0);
                    sun *= glm::max(0.001, glm::pow(glm::dot(rd, light), 256.0));
                    // col *= glm::max(0.001f, glm::dot(light, glm::vec3(0.0, 0.0, 1.0)));
                    return glm::clamp(sun + col, 0.0f, 1.0f);
                };

                auto castRay = [&](Ray &lray){
                    lray.minIt = glm::vec2{MAX_DIST};
                    for (const auto& object : objects) {
                        object.figure->intersect(lray);
                    }
                    if (lray.minIt.x == MAX_DIST) {
                        return glm::vec3{-1.f};
                    }

                    float diffuse = glm::max(glm::dot(glm::normalize(light.direction), lray.intersectNormal), 0.01f);// * 0.7f;
                    float specular = glm::max(0.0f, glm::dot(glm::reflect(lray.direction, lray.intersectNormal), light.direction));
                    
                    auto lightSum = glm::mix(diffuse, (float)glm::pow(specular, 32), 0.5f);
                    lray.origin += lray.direction * (lray.minIt.x - 0.001f);

                    return lray.intersectColor * glm::clamp(lightSum, 0.f, 1.f);
                };

                auto col = castRay(ray);
                if (col.x == -1.f) {ray.color = getSky(ray.direction, light.direction); return;}

                Ray lray = ray;
                lray.direction = light.direction;
                if (castRay(lray).x != -1.f) {
                    col *= 0.5f;
                }
                
                ray.color = col;
            };

            auto ray_tracing = [&](
                Ray& ray, Camera& camera, std::vector<Object>& objects
            ){
                auto castRay = [&](Ray &lray){
                    lray.minIt = glm::vec2{MAX_DIST};
                    for (const auto& object : objects) {
                        object.figure->intersect(lray);
                    }
                    if (lray.minIt.x == MAX_DIST) return glm::vec3{-1.f};

                    if (lray.intersectMaterial.transparency > 0.f) {
                        lray.origin += lray.direction * (lray.minIt.x + 0.001f);
                        lray.direction = glm::refract(lray.direction, lray.intersectNormal, 1.f / (1.f + lray.intersectMaterial.transparency));
                        return lray.intersectColor;
                    }
                    lray.origin += lray.direction * (lray.minIt.x - 0.001f);

                    glm::vec3 rand = {eml::uniform(-1, 1), eml::uniform(-1, 1), eml::uniform(-1, 1)};
                    glm::vec3 spec = glm::reflect(lray.direction, lray.intersectNormal);
                    glm::vec3 diff = glm::normalize(rand * glm::dot(rand, lray.intersectNormal));

                    lray.direction = glm::mix(diff, spec, lray.intersectMaterial.metallic);
                    return lray.intersectColor;
                };
                glm::vec3 col = glm::vec3(1.f);
                for (int i = 0; i < 8; i++){
                    auto sec = castRay(ray);
                    if (sec.x == -1.f) {ray.color = col * glm::vec3(0.3f, 0.4f, 0.7f); return;}
                    col *= sec;
                }
                
                ray.color = col;
            };

            switch(gtype){
                case RAY_TRACING:
                    body = ray_tracing;
                    break;
                case RAY_CASTING:
                    body = ray_casting;
                    break;
            }
        }
        Shader(){}
        ~Shader(){}
};