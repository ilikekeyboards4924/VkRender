#include "memory.h"
#include <vector>
#include <iostream>
#include <stdexcept>

MemoryManager::MemoryManager(VkDevice device) {
	m_device = device;
}

void MemoryManager::allocateVertexBufferMemory(VkDevice device, VkBuffer& vertexBuffer, std::vector<Vertex> vertices, uint32_t queueFamilyIndex, uint32_t memoryTypeIndex) {
	// create buffer
	VkBufferCreateInfo bufferInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = vertices.size() * sizeof(Vertex),
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &queueFamilyIndex,
	};
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer");
	}

	// allocate memory for buffer
	VkMemoryAllocateInfo memoryAllocateInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = 1024ULL * 1024, // megabyte
		.memoryTypeIndex = memoryTypeIndex,
	};
	if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &m_memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate memory");
	}
	if (vkBindBufferMemory(device, vertexBuffer, m_memory, 0) != VK_SUCCESS) {
		throw std::runtime_error("failed to bind buffer to memory");
	}

	// copy vertex data into memory
	void* data;
	if (vkMapMemory(device, m_memory, 0, bufferInfo.size, 0, &data)) {
		throw std::runtime_error("failed to map memory");
	}
	std::memcpy(data, vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(device, m_memory);
}

MemoryManager::~MemoryManager() {
	vkFreeMemory(m_device, m_memory, nullptr);
}