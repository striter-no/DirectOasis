#pragma once

#include "init.hpp"
#include "figures.hpp"

void Sphere::intersect(
    Ray& ray
) const {
    auto ro = ray.origin - center;
    auto b = glm::dot(ro, ray.direction);
    auto c = glm::dot(ro, ro) - this->radius * radius;
    auto h = b * b - c;
    if (h < 0.0f) {
        ray.intersectPoint = glm::vec3(-1.0f);
        return;
    }
    h = glm::sqrt(h);
    ray.it = glm::vec2(-b - h, -b + h);

    if (ray.it.x > 0.f && ray.it.x < ray.minIt.x) {
        ray.minIt = ray.it;
        ray.intersectPoint = ro + ray.direction * ray.it.x;
        ray.intersectNormal = glm::normalize(ray.intersectPoint);
        ray.intersectMaterial = this->material;
    }
}

void Box::intersect(
    Ray& ray
) const {
    auto in_box = [&](const glm::vec3 &&ro, const glm::vec3 &rd, const glm::vec3 &rad, glm::vec3 &oN) -> glm::vec2 {
        auto m = 1.0f / rd;
        auto n = m * ro;
        auto k = glm::abs(m) * rad;
        auto t1 = -n - k;
        auto t2 = -n + k;
        float tN = glm::max(glm::max(t1.x, t1.y), t1.z);
        float tF = glm::min(glm::min(t2.x, t2.y), t2.z);
        if (tN > tF || tF < 0.0f){
            return glm::vec2(-1.f);
        }
        
        oN = -glm::sign(rd) * glm::step({t1.y, t1.z, t1.x}, t1) * glm::step({t1.z, t1.x, t1.y}, t1);
        return glm::vec2(tN, tF);
    };
    glm::vec3 n;
    ray.it = in_box(ray.origin - this->position, ray.direction, this->size, n);

    if (ray.it.x > 0.f && ray.it.x < ray.minIt.x) {
        ray.minIt = ray.it;
        ray.intersectNormal = n;
        ray.intersectMaterial = this->material;
    }
}

void Plane::intersect(
    Ray& ray
) const {
    auto in_plain = [&](const glm::vec3 &ro, const glm::vec3 &rd, const glm::vec4 p){
        return -(glm::dot(ro, {p.x, p.y, p.z}) + p.w) / glm::dot(rd, {p.x, p.y, p.z});
    };

    ray.it = glm::vec2{in_plain(ray.origin, ray.direction, glm::vec4(this->normal, 1.f))};

    if (ray.it.x > 0.f && ray.it.x < ray.minIt.x) {
        ray.minIt = ray.it;
        ray.intersectNormal = this->normal;
        ray.intersectMaterial = this->material;
    }
}