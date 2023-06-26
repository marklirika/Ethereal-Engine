#include "ethereal_game_obj.h"

namespace ethereal {

    EtherealGameObject EtherealGameObject::makeLightPoint(float intensity, float radius, glm::vec3 color) {
        EtherealGameObject gameObj = EtherealGameObject::createGameObject();
        
        gameObj.lightPoint = std::make_unique<PointLightComponent>();
        gameObj.lightPoint->lightIntensity = intensity;
        gameObj.transform.scale.x = radius;
        gameObj.color = color;
        return gameObj;
    }
}
