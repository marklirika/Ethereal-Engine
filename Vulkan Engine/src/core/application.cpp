#include "application.h"
#include "ECS/systems/mesh_render_system.h"
#include "ECS/systems/point_light_render_system.h"
#include "memory/ethereal_buffer.h"
#include "utility/KeybordInput.h"

//glm
#define GLM_FORCE_RADIANCE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> 

//std
#include <chrono>
#include <stdexcept>
#include <array>
#include <string>
#include <iostream>
#include <memory>

#include "Frog's Empire/terrain/terrain.h"

namespace ethereal {

	Application::Application() {
		//setting descriptors global pool
		globalPool = 
			EtherealDescriptorPool::Builder(etherealDevice)
			.setMaxSets(EtherealSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EtherealSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadMeshes();
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

		//setting descriptors global set layout
		auto globalSetLayout = EtherealDescriptorSetLayout::Builder(etherealDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build(); 

		//setting descriptors global sets
		std::vector<VkDescriptorSet> globalDescriptorSets(EtherealSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			EtherealDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
		}

		//setting render systems
		MeshRenderSystem etherealRenderSystem{ etherealDevice, etherealRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		PointLightRenderSystem lightPointRenderSystem{ etherealDevice, etherealRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        
		//setting camera
		EtherealCamera camera{};
        camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));

		//setting viewer
		auto viewerObject = scene.createEntity("viewerObject");
		auto& viewerTransform = viewerObject.getComponent<TransformComponent>();;;
		viewerTransform.translation.z = -2.5f;
			
		//setting camera controller
		CameraController cameraController{};

		//setting frame time
        auto startFrameTime = std::chrono::high_resolution_clock::now();

		//Main Loop
		while (!etherealWindow.shouldClose()) {
			glfwPollEvents();

            auto endFrameTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(startFrameTime - endFrameTime).count();
            startFrameTime = endFrameTime;
             
            cameraController.moveInPlaneXZ(etherealWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerTransform.translation, viewerTransform.rotation);

            float aspect = etherealRenderer.getAspectRation();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);
			
			if (auto commandBuffer = etherealRenderer.beginFrame()) {
				std::size_t size = scene.getRegistry().size();
				int frameIndex = etherealRenderer.getFrameIndex();
				FrameInfo frameInfo { 
					frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], scene	
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
                etherealRenderSystem.renderMesh(frameInfo);
				lightPointRenderSystem.render(frameInfo);
				etherealRenderer.endSwapChainRenderPass(commandBuffer);
				etherealRenderer.endFrame();
			}
		}

		auto result = vkDeviceWaitIdle(etherealDevice.device());
		if(!result){
			throw std::runtime_error("failed to wait device idle");
		}
	}

	void Application::loadMeshes() {
		//terrain
		std::shared_ptr<EtherealModel> terrainModel = Frogs_Empire::Terrain::generateTerrain(this->etherealDevice, { 1024, 1024 }, { 1, 1 });
		auto terrain = scene.createEntity("terrain");
		auto& terrainMesh = terrain.addComponent<MeshComponent>(terrainModel);
		auto& terrainTransform = terrain.getComponent<TransformComponent>();
		terrainTransform.scale = { 0.01f, 0.01f, 0.01f };
		terrainTransform.rotation += glm::radians(90.0f);
		//frog + light below
		std::shared_ptr<EtherealModel> etherealModel = EtherealModel::createModelFromFile(etherealDevice, "models/frog_1.obj");		
		auto frog = scene.createEntity("frog");
		auto& frogMesh = frog.addComponent<MeshComponent>(etherealModel);
		auto& frogTransfrom = frog.getComponent<TransformComponent>();
		frogTransfrom.translation = { 0.f, 0.f, 0.f };
		frogTransfrom.scale = { 0.01f, 0.01f, 0.01f };
		frogTransfrom.rotation += glm::radians(90.0f);

		std::vector<glm::vec3> lightColors {
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f} 
		};

		//for (int i = 0; i < lightColors.size(); i++) {
		//	const std::string name = "point light " + std::to_string(i);
		//	auto pointLightEntity = scene.createEntity(name);
		//	auto& pointLight = pointLightEntity.addComponent<PointLightComponent>(1.f, lightColors[i]);
		//	auto rotateLight = glm::rotate(
		//		glm::mat4{ 1.f },
		//		(i * glm::two_pi<float>()) / lightColors.size(), 
		//		{ 0.f, -1.f, 0.f });

		//	pointLightEntity.getComponent<TransformComponent>().translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
		//}
		auto sun = scene.createEntity("Sun");
		sun.addComponent<PointLightComponent>(100000.f, glm::vec3(1.f, 0.f, 0.f));
		sun.getComponent<TransformComponent>().translation = { 500.f, -1000.f, 0.f };
		auto sun_2 = scene.createEntity("Sun2");
		sun_2.addComponent<PointLightComponent>(100000.f, glm::vec3(0.f, 0.f, 1.f));
		sun_2.getComponent<TransformComponent>().translation = { -500.f, -1000.f, 0.f };
		std::size_t size = scene.getRegistry().size();
		std::cout << "Size of registry: " << size << std::endl;
	}
}
