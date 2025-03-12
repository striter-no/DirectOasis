#pragma once

#include "init.hpp"
#include "figures.hpp"

bool Sphere::intersect(
    Ray& ray
) const {
    glm::vec3 oc = ray.origin - this->center;
    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - this->radius * this->radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false; // Нет пересечения
    }

    float sqrtDiscriminant = glm::sqrt(discriminant);
    float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
    float t1 = (-b + sqrtDiscriminant) / (2.0f * a);

    ray.t = (t0 < t1) ? t0 : t1;
    
    ray.intersectPoint = ray.origin + ray.direction * ray.t;
    ray.intersectNormal = glm::normalize(ray.intersectPoint - this->center);
    return ray.t >= 0; // Пересечение только если t >= 0
}

bool Plane::intersect(
    Ray& ray
) const {
    float denom = glm::dot(this->normal, ray.direction);
    if (std::abs(denom) < 1e-6f) {
        return false; // Луч параллелен плоскости
    }

    glm::vec3 diff = this->point - ray.origin;
    ray.t = glm::dot(diff, this->normal) / denom;
    
    ray.intersectNormal = this->normal;
    ray.intersectPoint = ray.origin + ray.direction * ray.t;
    return ray.t >= 0.0f;
}

bool Box::intersect(
    Ray& ray
) const {
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; ++i) {
        float invDir = 1.0f / ray.direction[i];
        float tNear = (this->position_min[i] - ray.origin[i]) * invDir;
        float tFar = (this->position_max[i] - ray.origin[i]) * invDir;

        if (tNear > tFar) std::swap(tNear, tFar);
        tMin = std::max(tMin, tNear);
        tMax = std::min(tMax, tFar);

        if (tMin > tMax) return false;
    }

    ray.t = tMin;
    ray.intersectPoint = ray.origin + ray.direction * ray.t;
    ray.intersectNormal = glm::normalize(glm::cross(this->position_max - this->position_min, ray.direction));
    return ray.t >= 0.0f;
}

bool Triangle::intersect(
    Ray& ray
) const {
    const float EPSILON = 1e-6f;
    glm::vec3 edge1 = this->v1 - this->v0;
    glm::vec3 edge2 = this->v2 - this->v0;
    glm::vec3 h = glm::cross(ray.direction, edge2);
    float a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON) return false; // Луч параллелен треугольнику

    float f = 1.0f / a;
    glm::vec3 s = ray.origin - this->v0;
    float u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f) return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(ray.direction, q);

    if (v < 0.0f || u + v > 1.0f) return false;

    ray.t = f * glm::dot(edge2, q);
    ray.intersectPoint = ray.origin + ray.direction * ray.t;
    ray.intersectNormal = glm::normalize(glm::cross(edge1, edge2));
    return ray.t > EPSILON; // Проверяем, что пересечение впереди
}