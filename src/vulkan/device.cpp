#include "device.h"
#include <vector>
#include <iostream>
#include <stdexcept>

DeviceContext::DeviceContext(VkInstance instance) {
	pickPhysicalDevice(instance);
	pickQueueFamily();
	pickMemoryType();
	createDevice();
}

void DeviceContext::pickPhysicalDevice(VkInstance instance) {
	// get a list of physical devices
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

	for (auto physicalDevice : physicalDevices) {
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			m_physicalDevice = physicalDevice;
			std::cout << "found discrete gpu" << std::endl;
			return;
		}
	}
}

void DeviceContext::pickQueueFamily() {
	// get a list of queue families
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

	for (uint32_t i = 0; i < queueFamilies.size(); i++) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			m_graphicsFamilyIndex = i;
			return;
		}
	}
}

void DeviceContext::pickMemoryType() {
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		VkMemoryType memoryType = memoryProperties.memoryTypes[i];

		if ((memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && (memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
			std::cout << "found memory that is visible to host and also local to gpu" << std::endl;
			m_memoryTypeIndex = i;
			return;
		}
	}

	throw std::runtime_error("unable to find memory that is visible to host and also local to gpu");
}

void DeviceContext::createDevice() {
	float queuePriority = 1.0f; // required even when only one queue
	VkDeviceQueueCreateInfo queueInfo{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = m_graphicsFamilyIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority,
	};

	const char* deviceExtensions = { "VK_KHR_swapchain" };
	VkPhysicalDeviceFeatures deviceFeatures = { VK_KHR_dynamic_rendering };
	VkPhysicalDeviceSynchronization2Features synchronization2Features{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
		.synchronization2 = VK_TRUE,
	};
	VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
		.pNext = &synchronization2Features,
		.dynamicRendering = VK_TRUE,
	};


	VkDeviceCreateInfo deviceInfo{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &dynamicRenderingFeatures,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queueInfo,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = &deviceExtensions,
		.pEnabledFeatures = &deviceFeatures,
	};

	if (vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &m_device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create device");
	} else {
		std::cout << "logical device created" << std::endl;
	}

	// once we have a logical device, retrieve the graphics queue
	vkGetDeviceQueue(m_device, m_graphicsFamilyIndex, 0, &m_graphicsQueue);
}

DeviceContext::~DeviceContext() {
	vkDestroyDevice(m_device, nullptr);
}