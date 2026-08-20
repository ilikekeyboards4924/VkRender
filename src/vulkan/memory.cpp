#include "memory.h"
#include "vertex.h"
#include "pipeline.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <stdexcept>

MemoryManager::MemoryManager(VkDevice device, PipelineContext& pipelineContext, uint32_t memoryTypeIndex) {
	m_device = device;

	// why is there only an allocation for vertex buffer memory and not uniform buffer memory?
	//allocateMemory(device, memoryTypeIndex, (1024ULL * 1024));
	allocateMemory(device, m_vertexBufferMemory, memoryTypeIndex, 1024ULL * 1024);
	allocateMemory(device, m_uniformBufferMemory, memoryTypeIndex, 1024ULL * 512);

	// cant call these if the uniform buffer is emtpys
	//createDescriptorPool(device);
	//createDescriptorSets(device, pipelineContext);
}

void MemoryManager::allocateMemory(VkDevice device, VkDeviceMemory& memory, uint32_t memoryTypeIndex, uint64_t allocationSize) {
	// allocate memory for buffers
	VkMemoryAllocateInfo memoryAllocateInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = allocationSize, // how many bytes to allocate
		.memoryTypeIndex = memoryTypeIndex,
	};
	if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS) {
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
		throw std::runtime_error("failed to create vertex buffer");
	}

	// bind buffer to the allocated memory
	if (vkBindBufferMemory(device, m_vertexBuffer, m_vertexBufferMemory, 0) != VK_SUCCESS) {
		throw std::runtime_error("failed to bind vertex buffer to memory");
	}

	// copy vertex data into memory
	void* data;
	if (vkMapMemory(device, m_vertexBufferMemory, 0, bufferInfo.size, 0, &data)) {
		throw std::runtime_error("failed to map memory (vertex buffer)");
	}
	std::memcpy(data, vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(device, m_vertexBufferMemory);
}

void MemoryManager::createUniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
	m_uniformBuffers.resize(FRAMES_IN_FLIGHT);
	m_uniformBufferDataPointers.resize(FRAMES_IN_FLIGHT);

	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
	VkDeviceSize alignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;

	VkDeviceSize uboSize = sizeof(UniformBufferObject);
	VkDeviceSize alignedBufferSize = (uboSize + alignment - 1) & ~(alignment - 1); // bitwise tricks for rounding up

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		VkDeviceSize offset = alignedBufferSize * i; // aligned to the specifications of the physical device

		VkBufferCreateInfo bufferInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = uboSize,
			.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &queueFamilyIndex,
		};

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_uniformBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create uniform buffer");
		} else {
			std::cout << "created uniform buffer " << i << std::endl;
		}

		if (vkBindBufferMemory(device, m_uniformBuffers[i], m_uniformBufferMemory, offset) != VK_SUCCESS) {
			throw std::runtime_error("failed to bind uniform buffer to memory");
		}
	}

	void* uniformMemoryMappedPointer;
	VkDeviceSize totalMemorySize = alignedBufferSize * FRAMES_IN_FLIGHT;
	if (vkMapMemory(device, m_uniformBufferMemory, 0, totalMemorySize, 0, &uniformMemoryMappedPointer) != VK_SUCCESS) {
		throw std::runtime_error("failed to map memory to pointer");
	}

	// get the rest of the pointers based on the first one
	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		char* bytePointer = static_cast<char*>(uniformMemoryMappedPointer);
		m_uniformBufferDataPointers[i] = bytePointer + (alignedBufferSize * i);
	}
}

void MemoryManager::updateUniformBuffers(uint32_t imageIndex, VkExtent2D extent) {
	UniformBufferObject ubo{
		.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(extent.width) / static_cast<float>(extent.height), 0.1f, 10.0f),
	};

	ubo.proj[1][1] *= -1; // glm was originally designed for opengl. in opengl, Y is in the opposite direction that it is in vulkan. flip the projection matrix

	std::memcpy(m_uniformBufferDataPointers[imageIndex], &ubo, sizeof(ubo));
}

void MemoryManager::createDescriptorPool(VkDevice device) {
	VkDescriptorPoolSize poolSize{
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = FRAMES_IN_FLIGHT,
	};

	VkDescriptorPoolCreateInfo poolInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = FRAMES_IN_FLIGHT,
		.poolSizeCount = 1,
		.pPoolSizes = &poolSize,
	};

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool");
	}
}

void MemoryManager::createDescriptorSets(VkDevice device, PipelineContext& pipelineContext) {
	std::vector<VkDescriptorSetLayout> layouts(FRAMES_IN_FLIGHT, pipelineContext.getDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocateInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = m_descriptorPool,
		.descriptorSetCount = static_cast<uint32_t>(layouts.size()),
		.pSetLayouts = layouts.data(),
	};

	m_descriptorSets.resize(FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device, &allocateInfo, m_descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	} else {
		std::cout << "allocated descriptor sets" << std::endl;
	}

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		std::cout << "CHECKPOINT ONE" << std::endl;

		VkDescriptorBufferInfo bufferInfo{
			.buffer = m_uniformBuffers[i],
			.offset = 0,
			.range = sizeof(UniformBufferObject),
		};

		std::cout << "CHECKPOINT ONE POINT FIVE" << std::endl;

		VkWriteDescriptorSet descriptorWrite{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = m_descriptorSets[i],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &bufferInfo,
		};
		std::cout << "CHECKPOINT TWO" << std::endl;

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr); // i think its 0 and nullptr for copy writes since no copying
		std::cout << "CHECKPOINT THREE" << std::endl;
	};
}

MemoryManager::~MemoryManager() {
	vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);
	vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
}