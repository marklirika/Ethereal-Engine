#pragma once
#include "../render/ethereal_model.h"

#include <memory>
#include <unordered_map>
#include "../ECS/components/components.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ethereal {

    class EtherealGameObject {
    public:
        using id_t = uint32_t; // +
        using Map = std::unordered_map<id_t, EtherealGameObject>; // +
        // xueta
        static EtherealGameObject createGameObject() {
            static id_t currentId = 0;
            return EtherealGameObject(currentId++);
        } // +

        EtherealGameObject(const EtherealGameObject&) = delete;
        EtherealGameObject& operator=(const EtherealGameObject) = delete;
        EtherealGameObject(EtherealGameObject&&) = default;
        EtherealGameObject& operator=(EtherealGameObject&&) = default;

        id_t getId() { return id; }
        //xueta
        static EtherealGameObject makeLightPoint(
            float intensity = 10.f, 
            float radius = 0.1f, 
            glm::vec3 color = glm::vec3(1.f));// PointLight render system function

        glm::vec3 color{}; // PointLightComponent color
        TransformComponent transform{}; // TransformComponent of entity +

        std::shared_ptr<EtherealModel> model{}; //MeshComponent of entity
        std::unique_ptr<PointLightComponent> lightPoint = nullptr; //LightComponent of enum MyEnum

    private:
        EtherealGameObject(id_t id) : id(id) {}
        id_t id; // +
    };
}
