#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace ethereal {

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation{};

        glm::mat3 normalMatrix();
        glm::mat4 mat4();
    };

} // namespace ethereal