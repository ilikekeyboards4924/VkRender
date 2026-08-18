#pragma once
#include <vulkan/vulkan.h>

class InstanceContext {
public:
	InstanceContext();
	~InstanceContext();

	VkInstance getInstance() const { return m_instance; };
private:
	void createInstance();

	VkInstance m_instance = VK_NULL_HANDLE;
};