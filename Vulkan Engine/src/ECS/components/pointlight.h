#pragma once

namespace ethereal {

    struct PointLightComponent {
        float lightIntensity = 1.0f;
        glm::vec3 color{}; 
    };

}   // namespace ethereal