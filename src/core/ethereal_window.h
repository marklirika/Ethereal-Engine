#pragma once
//GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//std
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

		VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

		bool wasWindowResized() { return framebufferResized; }

		void resetWindowResizedFlag() { framebufferResized = false; }

		GLFWwindow* getGLFWwindow() const { return window; }

	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;
	};
}