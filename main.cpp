#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>

#include "src/vulkan/instance.h"
#include "src/vulkan/device.h"

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "VkRender", nullptr, nullptr);

	InstanceContext instanceContext;
	DeviceContext deviceContext(instanceContext.getInstance());
	
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(instanceContext.getInstance(), window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create surface");
	} else {
		std::cout << "surface created" << std::endl;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	vkDestroySurfaceKHR(instanceContext.getInstance(), surface, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}