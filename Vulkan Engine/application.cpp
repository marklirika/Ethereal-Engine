#include "application.h"
#include "ethereal_render_system.h"

#define GLM_FORCE_RADIANCE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> 


#include <stdexcept>;
#include <array>
#include <string>
#include <iostream>
#include <memory>

namespace ethereal {

	Application::Application() {
		loadGameObjects();
	}

	Application::~Application() {}

	void Application::run() {
		EtherealRenderSystem etherealRenderSystem{ etherealDevice, etherealRenderer.getSwapChainRenderPass() };
		while (!etherealWindow.shouldClose()) {
			glfwPollEvents();
			
			if (auto commandBuffer = etherealRenderer.beginFrame()) {
				etherealRenderer.beginSwapChainRenderPass(commandBuffer);
				etherealRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				etherealRenderer.endSwapChainRenderPass(commandBuffer);
				etherealRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(etherealDevice.device());
	}

	//sierpinski(vertices, 5, glm::vec2(-0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.0f, -0.5f));
	void Application::loadGameObjects() {
		std::vector<EtherealModel::Vertex> vertices = {
			{{ 0.5f, 0.5f } , {1.0f, 0.0f, 0.0f}},
			{{ 0.0f, -0.5f }, {0.0f, 1.0f, 0.0f}},
			{{ -0.5f, 0.5f },{0.0f, 0.0f, 1.0f}}
		};
		auto etherealModel = std::make_shared<EtherealModel>(etherealDevice, vertices);

		auto triangle = EtherealGameObject::createGameObject();
		triangle.model = etherealModel;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform2D.translation.x = .0f;	
		triangle.transform2D.scale = { 1.f, 1.f };
		triangle.transform2D.rotation = .25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}

	void Application::sierpinski(
		std::vector<EtherealModel::Vertex>& vertices,
		int depth, 
		glm::vec2 left,
		glm::vec2 right,
		glm::vec2 top) {
		if (depth <= 0) {
			EtherealModel::Vertex a = { {top}, {1.0f, 0.0f, 0.0f} };
			vertices.push_back(EtherealModel::Vertex(top, { 1.0f, 0.0f, 0.0f }));
			vertices.push_back(EtherealModel::Vertex(right, { 0.0f, 1.0f, 0.0f }));
			vertices.push_back(EtherealModel::Vertex(left, { 0.0f, 0.0f, 1.0f }));
		}
		else {
			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftRight = 0.5f * (left + right);
			sierpinski(vertices, depth - 1, left, leftRight, leftTop);
			sierpinski(vertices, depth - 1, leftRight, right, rightTop);
			sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	}
}
