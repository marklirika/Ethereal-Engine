#pragma once

#include "ethereal_scene.h"
#include "ethereal_entity.h"
#include "ECS/ethereal_components.h"

namespace ethereal {

	Scene::Scene() {

	}

	Scene::~Scene() { }

	Entity Scene::createEntity(const std::string& name)	{

		Entity entity{ _registry.create(), *this };
		auto& tag = entity.addComponent<TagComponent>();
		tag.name = name.empty() ? "Unnamed Entity" : name;
		entity.addComponent<TransformComponent>();

		return entity;
	}
	void Scene::removeEntity(Entity entity) {
		_registry.destroy(entity.getEntityHandle());
	}

}