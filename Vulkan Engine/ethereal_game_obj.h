#pragma once
#include "ethereal_model.h"

#include <memory>


namespace ethereal {

    struct transform2DComponent {
        glm::vec2 translation{};
        glm::vec2 scale{ 1.f, 1.f };
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);

            glm::mat2 rotationMatrix{ {c, s}, {-s, c} };
            glm::mat2 scaleMatrix{ {scale.x, .0f}, {.0f, scale.y} };

            return rotationMatrix * scaleMatrix;
         }
    };


    class EtherealGameObject {
    public:
        using id_t = uint32_t;

        static EtherealGameObject createGameObject() {
            static id_t currentId = 0;
            return EtherealGameObject(currentId++);
        }

        EtherealGameObject(const EtherealGameObject&) = delete;
        EtherealGameObject& operator=(const EtherealGameObject) = delete;
        EtherealGameObject(EtherealGameObject&&) = default;
        EtherealGameObject& operator=(EtherealGameObject&&) = default;

        id_t getId() { return id; }

        std::shared_ptr<EtherealModel> model{};
        glm::vec3 color{};
        transform2DComponent transform2D;

    private:
        EtherealGameObject(id_t id) : id(id) {}
        id_t id;
    };
}
