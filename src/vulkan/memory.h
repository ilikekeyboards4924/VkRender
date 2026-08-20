#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "vertex.h"

class MemoryManager {
public:
	MemoryManager(VkDevice device);
	~MemoryManager();
	
	void allocateVertexBufferMemory(VkDevice device, VkBuffer& vertexBuffer, std::vector<Vertex> vertices, uint32_t queueFamilyIndex, uint32_t memoryTypeIndex);
private:
	VkDeviceMemory m_memory = VK_NULL_HANDLE;

	VkDevice m_device = VK_NULL_HANDLE; // use for destruction
};