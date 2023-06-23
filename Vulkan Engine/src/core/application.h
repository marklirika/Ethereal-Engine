#pragma once

#include "ethereal_device.h" 
#include "../render/ethereal_renderer.h"
#include "ethereal_window.h"
#include "../render/ethereal_camera.h"
#include "../ECS/ethereal_game_obj.h"
#include "../memory/ethereal_descriptors.h"

//std
#include <memory>
#include <vector>

namespace ethereal {

	class Application {
		
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application &operator=(const Application&) = delete;

		void run();

		void sierpinski(
			std::vector<EtherealModel::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);

	private:
		void loadGameObjects();

		EtherealWindow etherealWindow{ WIDTH, HEIGHT, "Ethereal Vulkan Engine" };
		EtherealDevice etherealDevice{ etherealWindow };
		EtherealRenderer etherealRenderer{ etherealWindow, etherealDevice };

		std::unique_ptr<EtherealDescriptorPool> globalPool{};
		EtherealGameObject::Map gameObjects;
	};
}