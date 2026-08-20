#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "vertex.h"

class MemoryManager {
public:
	MemoryManager(VkDevice device, uint32_t memoryTypeIndex);
	~MemoryManager();
	
	void createVertexBuffer(VkDevice device, std::vector<Vertex> vertices, uint32_t queueFamilyIndex);

	VkBuffer getVertexBuffer() const { return m_vertexBuffer; };
	std::vector<VkBuffer> getUniformBuffers() const { return m_uniformBuffers; };
private:
	void allocateMemory(VkDevice device, uint32_t memoryTypeIndex, uint64_t allocationSize);

	VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
	std::vector<VkBuffer> m_uniformBuffers;

	VkDeviceMemory m_memory = VK_NULL_HANDLE;

	// there should probably be somewhere global
	// where both CommandContext and MemoryManager are able to access a FRAMES_IN_FLIGHT number
	// this will do for now
	uint32_t FRAMES_IN_FLIGHT = 2;

	VkDevice m_device = VK_NULL_HANDLE; // use for destruction
};