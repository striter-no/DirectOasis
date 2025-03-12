#pragma once

#include "init.hpp"

struct Material {
    glm::vec3 color; // Цвет материала (RGB)
    float roughness; // Матовая способность
    float metallic; // Зеркальность
    float emissive; // Отходящий свет
    float transparency; // Прозрачность

    Material(
        glm::vec3 color,
        float roughness = 0.f,
        float metallic = 0.f,
        float emissive = 0.f,
        float transparency = 0.f
    ): color(color), roughness(roughness), metallic(metallic), emissive(emissive), transparency(transparency) {}

    Material() {}
};