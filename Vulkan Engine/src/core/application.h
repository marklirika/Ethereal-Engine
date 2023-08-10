#pragma once

#include "ethereal_device.h" 
#include "render/ethereal_renderer.h"
#include "ethereal_window.h"
#include "resources/ethereal_camera.h"
#include "ECS/ethereal_scene.h"
#include "ECS/ethereal_entity.h"
#include "ECS/ethereal_components.h"
#include "memory/ethereal_descriptors.h"

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

	private:
		EtherealWindow etherealWindow{ WIDTH, HEIGHT, "Ethereal Vulkan Engine" };
		EtherealDevice etherealDevice{ etherealWindow };
		EtherealRenderer etherealRenderer{ etherealWindow, etherealDevice };

		std::unique_ptr<EtherealDescriptorPool> globalPool{};

		void loadEntities();
		Scene scene;
	};
}