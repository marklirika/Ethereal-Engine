#include "application.h"

#include "../utility/KeybordInput.h"
#include "../render/render systems/ethereal_render_system.h"
#include "../render/render systems/lightpoint_render_system.h"

#include "../memory/ethereal_buffer.h"

#define GLM_FORCE_RADIANCE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> 
#include <chrono>
#include <stdexcept>;
#include <array>
#include <string>
#include <iostream>
#include <memory>

namespace ethereal {

	Application::Application() {
		//setting descriptors global pool
		globalPool = 
			EtherealDescriptorPool::Builder(etherealDevice)
			.setMaxSets(EtherealSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EtherealSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	Application::~Application() {}

	void Application::run() {
		std::vector<std::unique_ptr<EtherealBuffer>> uboBuffers(EtherealSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<EtherealBuffer>(
				etherealDevice,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = EtherealDescriptorSetLayout::Builder(etherealDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(EtherealSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			EtherealDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
		}

		EtherealRenderSystem etherealRenderSystem{ etherealDevice, etherealRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		LightPointRenderSystem lightPointRenderSystem{ etherealDevice, etherealRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        EtherealCamera camera{};
        camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        auto viewerObject = EtherealGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
        Keybord cameraController{};
        auto startFrameTime = std::chrono::high_resolution_clock::now();

		while (!etherealWindow.shouldClose()) {
			glfwPollEvents();

            auto endFrameTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(startFrameTime - endFrameTime).count();
            startFrameTime = endFrameTime;
             
            cameraController.moveInPlaneXZ(etherealWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = etherealRenderer.getAspectRation();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
			if (auto commandBuffer = etherealRenderer.beginFrame()) {
				int frameIndex = etherealRenderer.getFrameIndex();
				FrameInfo frameInfo { 
					frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex],	gameObjects	
				};

				//update
				GlobalUBO ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				lightPointRenderSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render
				etherealRenderer.beginSwapChainRenderPass(commandBuffer);
                etherealRenderSystem.renderGameObjects(frameInfo);
				lightPointRenderSystem.render(frameInfo);
				etherealRenderer.endSwapChainRenderPass(commandBuffer);
				etherealRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(etherealDevice.device());
	}

	void Application::loadGameObjects() {
        std::shared_ptr<EtherealModel> etherealModel = EtherealModel::createModelFromFile(etherealDevice, "models/smooth_vase.obj");
		auto flatVase = EtherealGameObject::createGameObject();
		flatVase.model = etherealModel;
		flatVase.transform.translation = { -.5f, .5f, 0.f };
		flatVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		etherealModel = EtherealModel::createModelFromFile(etherealDevice, "models/flat_vase.obj");
		auto smoothVase = EtherealGameObject::createGameObject();
		smoothVase.model = etherealModel;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f} 
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto lightPointObj = EtherealGameObject::makeLightPoint(0.8f);
				lightPointObj.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4{ 1.f },
				(i * glm::two_pi<float>()) / lightColors.size(), 
				{ 0.f, -1.f, 0.f });

			lightPointObj.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(lightPointObj.getId(), std::move(lightPointObj));
		}
	}
}
