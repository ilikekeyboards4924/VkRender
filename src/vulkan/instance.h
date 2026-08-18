#pragma once
#include <vulkan/vulkan.h>

class InstanceContext {
public:
	InstanceContext();
	~InstanceContext();

	VkInstance getInstance() const { return m_instance; };
private:
	VkInstance m_instance = VK_NULL_HANDLE;

	void createInstance();
};