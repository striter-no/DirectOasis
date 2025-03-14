#pragma once

#include "init.hpp"

struct DirectionalLight {
    glm::vec3 direction = glm::vec3{0.f};
    glm::vec3 color = glm::vec3{1.f};
};