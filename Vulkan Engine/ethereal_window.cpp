#include "ethereal_window.h"

namespace ethereal {

	EtherealWindow::EtherealWindow(int width, int height, std::string windowName) : width{ width }, height{ height }, windowName{ windowName } {
		EtherealWindow::initWindow();
	}

	EtherealWindow::~EtherealWindow() {
		glfwDestroyWindow(window);  
		glfwTerminate();
	}

	void EtherealWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this); 
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void EtherealWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR * surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	} 
	void EtherealWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto etherealWindow = reinterpret_cast<EtherealWindow*> (glfwGetWindowUserPointer(window));
		
		etherealWindow->framebufferResized = true;
		etherealWindow->width = width;
		etherealWindow->height = height;
	}
}