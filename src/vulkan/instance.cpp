#include "instance.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

InstanceContext::InstanceContext() {
	createInstance();
}

void InstanceContext::createInstance() {
	VkApplicationInfo appInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "VkRender",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3,
	};

	// what did i forget to do? i forgot to get the required instance extensions from glfw
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	const char* enabledLayerNames = { "VK_LAYER_KHRONOS_validation" };

	VkInstanceCreateInfo instanceInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = 1,
		.ppEnabledLayerNames = &enabledLayerNames,
		.enabledExtensionCount = glfwExtensionCount,
		.ppEnabledExtensionNames = glfwExtensions,
	};

	if (vkCreateInstance(&instanceInfo, nullptr, &m_instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance");
	} else {
		std::cout << "vulkan instance created" << std::endl;
	}
}

InstanceContext::~InstanceContext() {
	// if you never call vkDestroy on anything, you never get a validation layer error in the first place!
	vkDestroyInstance(m_instance, nullptr);
}