#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "vertex.h"

class MemoryManager {
public:
	MemoryManager(VkDevice device);
	~MemoryManager();
	
	void allocateVertexBufferMemory(VkDevice device, std::vector<Vertex> vertices, uint32_t queueFamilyIndex, uint32_t memoryTypeIndex);

	VkBuffer getVertexBuffer() const { return m_vertexBuffer; };
	VkBuffer getUniformBuffer() const { return m_uniformBuffer; };
private:
	VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
	VkBuffer m_uniformBuffer = VK_NULL_HANDLE;

	VkDeviceMemory m_memory = VK_NULL_HANDLE;

	VkDevice m_device = VK_NULL_HANDLE; // use for destruction
};