#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

namespace ethereal {

	class EtherealWindow {
	
	public:
		EtherealWindow(int width, int height, std::string windowName);
		~EtherealWindow();

		EtherealWindow(const EtherealWindow&) = delete;
		EtherealWindow& operator=(const EtherealWindow&) = delete;

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
				
		bool shouldClose() { return glfwWindowShouldClose(window); }

		VkExtent2D getExtent() { return {static_cast<uint32_t>(WIDTH), static_cast<uint32_t>(HEIGHT)}; }

	private:
		void initWindow();

		const int WIDTH;
		const int HEIGHT;

		std::string windowName;
		GLFWwindow* window;
	};
}