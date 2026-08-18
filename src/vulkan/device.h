#pragma once
#include <vulkan/vulkan.h>

class DeviceContext {
public:
	DeviceContext(VkInstance instance);
	~DeviceContext();

	VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; };
	VkDevice getDevice() const { return m_device; };
private:
	void pickPhysicalDevice(VkInstance instance);
	void pickQueueFamily();
	void createDevice();

	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;


	uint32_t m_graphicsFamilyIndex;
};