#pragma once

#include "Scene.h"
#include "Components/components.h"
#include "ethereal_entity.h"

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

}