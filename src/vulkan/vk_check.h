#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <string>

inline void VK_CHECK(VkResult result, const char* msg) {
	if (result != VK_SUCCESS) {
		throw std::runtime_error(std::string(msg) + " (VkResult = " + std::to_string(result) + ")");
	}
}