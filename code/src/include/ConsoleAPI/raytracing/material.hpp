#pragma once

#include "init.hpp"

struct Material {
    float roughness; // Матовая способность
    float metallic; // Зеркальность
    float emissive; // Отходящий свет
    float transparency; // Прозрачность

    Material(
        float roughness,
        float metallic,
        float emissive,
        float transparency
    ): roughness(roughness), metallic(metallic), emissive(emissive), transparency(transparency) {}

    Material() {}
};