#pragma once

#include "init.hpp"

struct Ray {
    float t;
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 color;

    glm::vec3 intersectPoint;
    glm::vec3 intersectNormal;
};

struct Figure {
    virtual bool intersect(Ray& ray) const = 0; // Чисто виртуальный метод
    virtual ~Figure() = default; // Добавьте виртуальный деструктор
};

struct Sphere: public Figure {
    glm::vec3 center; 
    float radius;

    bool intersect(Ray& ray) const;
    
    Sphere(glm::vec3 center, float radius): center(center), radius(radius) {}
    Sphere() {}
};

struct Box: public Figure {
    glm::vec3 position_min;
    glm::vec3 position_max;

    bool intersect(Ray& ray) const;

    Box(glm::vec3 position_min, glm::vec3 position_max): position_min(position_min), position_max(position_max) {}
    Box() {}
};

struct Plane: public Figure {
    glm::vec3 point;
    glm::vec3 normal;

    bool intersect(Ray& ray) const;

    Plane(glm::vec3 point, glm::vec3 normal): point(point), normal(glm::normalize(normal)) {}
    Plane() {}
};

struct Triangle: public Figure {
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;

    bool intersect(Ray& ray) const;

    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2): v0(v0), v1(v1), v2(v2) {}
    Triangle() {}
};