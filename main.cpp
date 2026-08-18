#include <GLFW/glfw3.h>
#include <iostream>

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "VkRender", nullptr, nullptr);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	return 0;
}