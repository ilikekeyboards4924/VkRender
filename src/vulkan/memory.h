#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "vertex.h"
#include "pipeline.h"

// REMOVE THIS CLASS!
class MemoryManager {
public:
	MemoryManager(VkDevice device, PipelineContext& pipelineContext, uint32_t memoryTypeIndex);
	~MemoryManager();
	
	void createVertexBuffer(VkDevice device, std::vector<Vertex> vertices, uint32_t queueFamilyIndex);
	void createUniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
	void updateUniformBuffers(uint32_t imageIndex, VkExtent2D extent, uint32_t testNumber);

	void createDescriptorPool(VkDevice device);
	void createDescriptorSets(VkDevice device, PipelineContext& pipelineContext);

	VkBuffer getVertexBuffer() const { return m_vertexBuffer; };
	std::vector<VkBuffer> getUniformBuffers() const { return m_uniformBuffers; };

	std::vector<VkDescriptorSet> getDescriptorSets() const { return m_descriptorSets; };


	uint32_t testNumber = 0; // delete this
private:
	void allocateMemory(VkDevice device, VkDeviceMemory& memory, uint32_t memoryTypeIndex, uint64_t allocationSize);

	VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory m_vertexBufferMemory = VK_NULL_HANDLE;

	std::vector<VkBuffer> m_uniformBuffers;
	std::vector<void*> m_uniformBufferDataPointers;
	VkDeviceMemory m_uniformBufferMemory;

	VkDescriptorPool m_descriptorPool; // this should probably be somewhere else
	std::vector<VkDescriptorSet> m_descriptorSets; // this too

	// there should probably be somewhere global
	// where both CommandContext and MemoryManager are able to access a FRAMES_IN_FLIGHT number
	// this will do for now
	uint32_t FRAMES_IN_FLIGHT = 2;

	VkDevice m_device = VK_NULL_HANDLE; // use for destruction
};