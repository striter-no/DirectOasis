#pragma once

#include "init.hpp"
#include "material.hpp"

struct Ray {
    glm::vec2 minIt;
    glm::vec2 it;
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 color;

    glm::vec3 intersectPoint;
    glm::vec3 intersectNormal;
    glm::vec3 intersectColor;
    Material  intersectMaterial;
};

struct Figure {
    virtual ~Figure() = default;
    virtual void intersect(Ray& ray) const = 0;
};

struct Sphere: public Figure {
    Material material;
    glm::vec3 color;

    glm::vec3 center; 
    float radius;

    void intersect(Ray& ray) const;
    
    Sphere(glm::vec3 center, float radius, Material material, glm::vec3 color): color(color), material(material), center(center), radius(radius) {}
    Sphere() {}
};

struct Box: public Figure {
    Material material;
    glm::vec3 color;

    glm::vec3 position;
    glm::vec3 size;

    void intersect(Ray& ray) const;

    Box(glm::vec3 position, glm::vec3 size, Material material, glm::vec3 color): color(color), material(material), position(position), size(size) {}
    Box() {}
};

struct Plane: public Figure {
    Material material;
    glm::vec3 color;

    glm::vec3 normal;

    void intersect(Ray& ray) const;

    Plane(glm::vec3 normal, Material material, glm::vec3 color): color(color), material(material), normal(normal) {}
    Plane() {}
};

struct Triangle: public Figure {
    Material material;
    glm::vec3 color;

    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;

    void intersect(Ray& ray) const;

    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,  Material material, glm::vec3 color): color(color), material(material), v0(v0), v1(v1), v2(v2) {}
    Triangle() {}
};