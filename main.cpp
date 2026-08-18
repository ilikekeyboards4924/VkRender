#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "VkRender", nullptr, nullptr);

	uint32_t extensionCount;
	glfwGetRequiredInstanceExtensions(&extensionCount);

	std::cout << "loaded " << extensionCount << " vulkan extensions" << std::endl;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}