#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

#include "../device.h"

class Model {
public:
	struct Vertex {
		glm::vec2 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};

	Model(DeviceContext& deviceContext, const std::vector<Vertex>& vertices);
	~Model();

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);
private:
	void createVertexBuffers(const std::vector<Vertex>& vertices);

	VkBuffer m_vertexBuffer;
	VkDeviceMemory m_vertexBufferMemory;
	uint32_t m_vertexCount;

	DeviceContext& m_deviceContext;
};