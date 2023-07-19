#include "collision.h"

namespace ethereal {

    glm::vec3 CollisionComponent::calculateMeshCenter(const std::vector<EtherealModel::Vertex>& vertices) {
        for (const auto& vertex : vertices) {
            center += vertex.position;
        }
        
        center /= static_cast<float>(vertices.size());

        return center;
    }

    float CollisionComponent::calculateMeshRadius(const std::vector<EtherealModel::Vertex>& vertices) {
        for (const auto& vertex : vertices) {
            float distance = glm::length(vertex.position - center);
            radius = std::max(radius, distance);
        }

        return radius;
    }
}  // namespace ethereal