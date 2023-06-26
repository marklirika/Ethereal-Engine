#include "ethereal_entity.h"

namespace ethereal {
	Entity::Entity(entt::entity handle, Scene& scene)
		: entityHandle(handle), scene(scene) {
	}
}