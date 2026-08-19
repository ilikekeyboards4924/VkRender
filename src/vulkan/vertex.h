#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
	glm::vec2 position; // two dimensional for now
	glm::vec3 color;

	glm::vec2 offset; // additional value to test if i know how to use vulkan
};

extern VkVertexInputBindingDescription vertexBindingDescription;
extern VkVertexInputAttributeDescription positionAttributeDescription;
extern VkVertexInputAttributeDescription colorAttributeDescription;
extern VkVertexInputAttributeDescription offsetAttributeDescription;

extern std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;

extern std::vector<Vertex> vertices;