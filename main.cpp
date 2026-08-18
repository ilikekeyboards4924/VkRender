#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>

#include "src/vulkan/instance.h"
#include "src/vulkan/device.h"

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "VkRender", nullptr, nullptr);

	InstanceContext instanceContext;
	DeviceContext deviceContext(instanceContext.getInstance());

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}