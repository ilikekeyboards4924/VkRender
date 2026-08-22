#include "model.h"
#include "../device.h"
#include "../vk_check.h"
#include <vector>
#include <assert.h>

Model::Model(DeviceContext& deviceContext, const std::vector<Vertex>& vertices) : m_deviceContext(deviceContext) {
	createVertexBuffers(vertices);
}

void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
	// assert at least one triangle
	m_vertexCount = static_cast<uint32_t>(vertices.size());
	assert(m_vertexCount >= 3 && "vertex count must be at least 3");

	// create vertex buffer
	VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
	uint32_t queueFamilyIndex = m_deviceContext.getGraphicsFamilyIndex();
	VkBufferCreateInfo bufferInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = bufferSize,
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &queueFamilyIndex,
	};
	VK_CHECK(vkCreateBuffer(m_deviceContext.getDevice(), &bufferInfo, nullptr, &m_vertexBuffer), "failed to create vertex buffer");

	// allocate memory and bind vertex buffer to allocated memory
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(m_deviceContext.getDevice(), m_vertexBuffer, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memoryRequirements.size,
		.memoryTypeIndex = m_deviceContext.getMemoryTypeIndex(),
	};
	VK_CHECK(vkAllocateMemory(m_deviceContext.getDevice(), &allocateInfo, nullptr, &m_vertexBufferMemory), "failed to allocate memory");
	
	VK_CHECK(vkBindBufferMemory(m_deviceContext.getDevice(), m_vertexBuffer, m_vertexBufferMemory, 0), "failed to bind vertex buffer to memory");

	// copy vertices data into vertex buffer
	void* data;
	VK_CHECK(vkMapMemory(m_deviceContext.getDevice(), m_vertexBufferMemory, 0, bufferSize, 0, &data), "failed to map memory");
	std::memcpy(data, vertices.data(), bufferSize);
	vkUnmapMemory(m_deviceContext.getDevice(), m_vertexBufferMemory);
}

void Model::bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = { m_vertexBuffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void Model::draw(VkCommandBuffer commandBuffer) {
	vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = 0;

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = 8;
	return attributeDescriptions;
}

Model::~Model() {
	vkDestroyBuffer(m_deviceContext.getDevice(), m_vertexBuffer, nullptr);
	vkFreeMemory(m_deviceContext.getDevice(), m_vertexBufferMemory, nullptr);
}