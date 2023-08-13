#include "application.h"

//custom
#include "memory/ethereal_buffer.h"
#include "resources/ethereal_texture.h"
#include "render/render systems/forward_render_system.h"
#include "render/render systems/deffered_render_system.h"
#include "render/render systems/point_light_render_system.h"
#include "ECS/systems/audio_system.h"
#include "utility/KeybordInput.h"
#include "utility/utils.h"

//glm
#define GLM_FORCE_RADIANCE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> 
#include <glm/gtx/euler_angles.hpp>

//std
#include <chrono>
#include <stdexcept>
#include <array>
#include <string>
#include <iostream>
#include <memory>

//Gameplay features
#include "Frog's Empire/terrain/terrain.h"
#include "Frog's Empire/unit_gen/unit_gen_system.h"

namespace ethereal {

	Application::Application() {
		//setting descriptors global pool
		globalPool = 
			EtherealDescriptorPool::Builder(etherealDevice)
			.setMaxSets(EtherealSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EtherealSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadEntities();
	}

	Application::~Application() {}

	void Application::run() {
		//creating ubo buffers
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
			EtherealDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		//setting systems
		ForwardRenderSystem forwardRenderSystem{ etherealDevice, etherealRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		//DefferedRenderSystem defferedRenderSystem{};
		PointLightRenderSystem lightPointRenderSystem{ etherealDevice, etherealRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		UnitGenSystem unitGenSystem{ etherealDevice };

		//setting camera
		EtherealCamera camera{};
        camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));

		//setting viewer
		auto viewerObject = scene.createEntity("viewerObject");
		auto& viewerTransform = viewerObject.getComponent<TransformComponent>();;;
		viewerTransform.translation.z = -2.5f;
			
		//setting camera controller
		CameraController cameraController{};

		//setting timer for calculation of frame time
		eHelp::timer timer;
		timer.start();

		//Main Loop
		while (!etherealWindow.shouldClose()) {
			glfwPollEvents();

            auto endFrameTime = std::chrono::high_resolution_clock::now();
			float frameTime = timer.dt();
			timer.reset();
             
            cameraController.moveInPlaneXZ(etherealWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerTransform.translation, viewerTransform.rotation);

            float aspect = etherealRenderer.getAspectRation();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);
			
			if (auto commandBuffer = etherealRenderer.beginFrame()) {
				std::size_t size = scene.getRegistry().size();
				int frameIndex = etherealRenderer.getFrameIndex();
				//index, time, offscreenCmd, composeCmd, offscreenFrm, globalDS, cameran, scene
				OffscreenFrameBuffer testFrm;
				FrameInfo frameInfo { 
					frameIndex, 
					frameTime, 
					VK_NULL_HANDLE, 
					commandBuffer, 
					testFrm, 
					globalDescriptorSets[frameIndex], 
					camera, 
					scene
				};

				//update
				GlobalUBO ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				lightPointRenderSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
				unitGenSystem.generate(frameInfo);
				AudioSystem::update(frameInfo);
				//render
				etherealRenderer.beginSwapChainRenderPass(commandBuffer);
				forwardRenderSystem.render(frameInfo);
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

	void Application::loadEntities() {
		
		////sound system
		//auto musicPlayer = scene.createEntity("music_player");
		//auto& audio = musicPlayer.addComponent<AudioComponent>();
		//audio.type = AudioComponent::AudioType::MUSIC_TRACK;
		//AudioSystem::init();
		//AudioSystem::load(audio, "audio/music/boat.mp3");
		//AudioSystem::play(audio);

		////terrain
		//std::shared_ptr<EtherealModel> terrainModel = 
		//	Frogs_Empire::Terrain::generateTerrain(this->etherealDevice, { 1024, 1024 }, { 1, 1 });
		//auto terrain = scene.createEntity("terrain");
		//auto& terrainMesh = terrain.addComponent<MeshComponent>(terrainModel);
		//auto& terrainTransform = terrain.getComponent<TransformComponent>();
		//terrainTransform.scale = { 0.5f,0.5f,0.5f };
		//terrainTransform.translation = { -100.f, 0.f, -100.f };
		//terrainTransform.rotation += glm::radians(90.0f);

		//frog + light below
		std::shared_ptr<EtherealModel> etherealModel = EtherealModel::createModelFromFile(etherealDevice, "models/frog_1.obj");		
		auto frog = scene.createEntity("frog");
		auto& frogMesh = frog.addComponent<MeshComponent>(etherealModel);
		std::shared_ptr<EtherealTexture> carbonTexture =
			std::make_shared<EtherealTexture>(etherealDevice, "textures/carbon.jpg");
		auto& frogTexture = frog.addComponent<TextureComponent>();
		frogTexture.texture = carbonTexture;
		auto& frogTransfrom = frog.getComponent<TransformComponent>();
		frogTransfrom.translation = { 0.f, 0.f, 0.f };
		frogTransfrom.scale = { 0.5, 0.5, 0.5 };
		frogTransfrom.rotation += glm::radians(90.0f);

		//frog_2 + light below
		auto frog_2 = scene.createEntity("frog");
		auto& frog_2Mesh = frog_2.addComponent<MeshComponent>(etherealModel);
		std::shared_ptr<EtherealTexture> paperTexture =
			std::make_shared<EtherealTexture>(etherealDevice, "textures/paper_yellow.jpg");
		auto& frog_2Texture = frog_2.addComponent<TextureComponent>();
		frog_2Texture.texture = paperTexture;
		auto& frog_2Transfrom = frog_2.getComponent<TransformComponent>();
		frog_2Transfrom.translation = { 10.f, 0.f, 0.f };
		frog_2Transfrom.scale = { 0.5, 0.5, 0.5 };
		frog_2Transfrom.rotation += glm::radians(90.0f);

		glm::vec3 currentPosition(frogTransfrom.translation);
		glm::vec3 currentRotation(frogTransfrom.rotation); // ������������, ��� ���� �������� � ��������

		// ������� ����������, ���� ����� ����������� ������
		glm::vec3 targetPosition(20.0f, -10.0f, 10.0f);

		// �������� ������� ���� ��� ����������� ������� �� ������� ����������
		glm::mat4 viewMatrix = glm::lookAt(currentPosition, targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));

		// ���������� ����� ������ �� ������� ����
		glm::vec3 newRotation = glm::eulerAngles(glm::quat_cast(viewMatrix));

		frogTransfrom.rotation = newRotation;

		// �������������� ����� � �������
		newRotation = glm::degrees(newRotation);
		//std::vector<glm::vec3> lightColors {
		//	{1.f, .1f, .1f},
		//	{.1f, .1f, 1.f},
		//	{.1f, 1.f, .1f},
		//	{1.f, 1.f, .1f},
		//	{.1f, 1.f, 1.f},
		//	{1.f, 1.f, 1.f} 
		//};

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
		sun.addComponent<PointLightComponent>(50000000.f, glm::vec3(1.f, 1.f, 1.f));
		sun.getComponent<TransformComponent>().translation = { 0.f, -5000.f, 0.f };

		std::size_t size = scene.getRegistry().size();
		std::cout << "Size of registry: " << size << std::endl;
	}
}
