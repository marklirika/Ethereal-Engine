#include "ethereal_window.h"

namespace ethereal {

	EtherealWindow::EtherealWindow(int width, int height, std::string windowName) : WIDTH{ width }, HEIGHT{ height }, windowName{ windowName } {
		EtherealWindow::initWindow();
	}

	EtherealWindow::~EtherealWindow() {
		glfwDestroyWindow(window);  
		glfwTerminate();
	}

	void EtherealWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);
	}

	void EtherealWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR * surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	} 
}