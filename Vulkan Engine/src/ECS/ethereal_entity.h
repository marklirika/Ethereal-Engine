#pragma once
#include "ethereal_scene.h"
#include "entt.h"

namespace ethereal {
	class Entity
	{
	public:	
		Entity() = default;
		Entity(entt::entity handle, Scene& scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)	{
			return scene._registry.emplace<T>(entityHandle, std::forward<Args>(args)...);
		}		

		template<typename T>
		T& getComponent()	{
			return scene._registry.get<T>(entityHandle);
		}
		
		template<typename T>
		bool hasComponent()	{
			return scene._registry.all_of<T>(entityHandle);
		}

		template<typename T>
		void removeComponent()	{
			scene._registry.remove<T>(entityHandle);
		} 

		operator bool() const { return entityHandle != entt::null; }
	private:
		entt::entity entityHandle{ entt::null };
		Scene& scene;
	};

} // namespace ethereal
