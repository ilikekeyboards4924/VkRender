#pragma once
#include <vulkan/vulkan.h>

class DeviceContext {
public:
	DeviceContext(VkInstance instance);
	~DeviceContext();

	VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; };
	VkDevice getDevice() const { return m_device; };

	VkQueue getGraphicsQueue() const { return m_graphicsQueue; };
	uint32_t getGraphicsFamilyIndex() const { return m_graphicsFamilyIndex; };

	uint32_t getMemoryTypeIndex() const { return m_memoryTypeIndex; };
private:
	void pickPhysicalDevice(VkInstance instance);
	void pickQueueFamily();
	void pickMemoryType();
	void createDevice();

	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;

	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	uint32_t m_graphicsFamilyIndex;

	uint32_t m_memoryTypeIndex;
};