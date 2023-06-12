#pragma once
#include "ethereal_model.h"

#include <memory>
#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>

namespace ethereal {

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation{};

        glm::mat3 normalMatrix();
        glm::mat4 mat4();
    };

    struct LightPointComponent {
        float lightIntensity = 1.0f;
    };

    class EtherealGameObject {
    public:
        using id_t = uint32_t;
        using Map = std::unordered_map<id_t, EtherealGameObject>;

        static EtherealGameObject createGameObject() {
            static id_t currentId = 0;
            return EtherealGameObject(currentId++);
        }

        static EtherealGameObject makeLightPoint(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

        EtherealGameObject(const EtherealGameObject&) = delete;
        EtherealGameObject& operator=(const EtherealGameObject) = delete;
        EtherealGameObject(EtherealGameObject&&) = default;
        EtherealGameObject& operator=(EtherealGameObject&&) = default;

        id_t getId() { return id; }

        glm::vec3 color{};
        TransformComponent transform{};

        std::shared_ptr<EtherealModel> model{};
        std::unique_ptr<LightPointComponent> lightPoint = nullptr;

    private:
        EtherealGameObject(id_t id) : id(id) {}
        id_t id;
    };
}
