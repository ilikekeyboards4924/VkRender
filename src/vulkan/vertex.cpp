#include "vertex.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <stdexcept>

Vertex vert;

VkVertexInputBindingDescription vertexBindingDescription{
	.binding = 0,
	.stride = 28, // probably better way to do this than hovering over struct and waiting for vs to tell me its size
	.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
};

VkVertexInputAttributeDescription positionAttributeDescription{
	.location = 2,
	.binding = 0,
	.format = VK_FORMAT_R32G32_SFLOAT,
	.offset = 0,
};

VkVertexInputAttributeDescription colorAttributeDescription{
	.location = 3,
	.binding = 0,
	.format = VK_FORMAT_R32G32B32_SFLOAT,
	.offset = 8,
};

VkVertexInputAttributeDescription offsetAttributeDescription{
	.location = 4,
	.binding = 0,
	.format = VK_FORMAT_R32G32_SFLOAT,
	.offset = 20,
};

std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = { positionAttributeDescription, colorAttributeDescription, offsetAttributeDescription };


std::vector<Vertex> vertices = {
	{
		glm::vec2(-1.0f, -1.0f),
		glm::vec3(1.0f, 0.7f, 0.3f),
		glm::vec2(0.5f, 0.7f),
	},
	{
		glm::vec2(1.0f, -1.0f),
		glm::vec3(0.4f, 0.2f, 0.8f),
		glm::vec2(-0.5f, 0.3f),
	},
	{
		glm::vec2(0.0f, 1.0f),
		glm::vec3(0.3f, 0.7f, 1.0f),
		glm::vec2(-0.5f, -0.3f),
	},
};