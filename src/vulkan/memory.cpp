#include "memory.h"
#include <vector>
#include <iostream>
#include <stdexcept>

MemoryManager::MemoryManager(VkDevice device, uint32_t memoryTypeIndex) {
	m_device = device;

	allocateMemory(device, memoryTypeIndex, (1024ULL * 1024));
}

void MemoryManager::allocateMemory(VkDevice device, uint32_t memoryTypeIndex, uint64_t allocationSize) {
	// allocate memory for buffers
	VkMemoryAllocateInfo memoryAllocateInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = allocationSize, // how many bytes to allocate
		.memoryTypeIndex = memoryTypeIndex,
	};
	if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &m_memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate memory");
	} else {
		std::cout << "allocated " << allocationSize << " bytes on the gpu" << std::endl;
	}
}

void MemoryManager::createVertexBuffer(VkDevice device, std::vector<Vertex> vertices, uint32_t queueFamilyIndex) {
	// create buffer
	VkBufferCreateInfo bufferInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = vertices.size() * sizeof(Vertex),
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &queueFamilyIndex,
	};
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_vertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer");
	}

	// bind buffer to the allocated memory
	if (vkBindBufferMemory(device, m_vertexBuffer, m_memory, 0) != VK_SUCCESS) {
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
	vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
}