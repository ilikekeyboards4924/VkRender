#include "swapchain.h"
#include <vector>
#include <iostream>
#include <stdexcept>

SwapchainContext::SwapchainContext(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {

}

void SwapchainContext::pickSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
	// get a list of surface formats
	uint32_t surfaceFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data());

	for (auto surfaceFormat : surfaceFormats) {
		// SRGB colorspace and BGRA
		if (surfaceFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR && surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB) {
			m_surfaceFormat = surfaceFormat;
			return;
		}
	}
}

void SwapchainContext::pickSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities) != VK_SUCCESS) {
		throw std::runtime_error("failed to get surface capabilities");
	} else {
		std::cout << "retrieved surface capabilities" << std::endl;
		m_surfaceCapabilities = surfaceCapabilities;
		return;
	}
}

void SwapchainContext::createSwapchain(VkDevice device, VkSurfaceKHR surface, uint32_t queueFamilyIndex) {
	uint32_t queueFamilyIndices = { queueFamilyIndex };

	VkSwapchainCreateInfoKHR swapchainInfo{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surface,
		.minImageCount = 2, // 2 for double buffer
		.imageFormat = m_surfaceFormat.format,
		.imageColorSpace = m_surfaceFormat.colorSpace,
		.imageExtent = m_surfaceCapabilities.currentExtent,
		.imageArrayLayers = 1, // 1 for monitor, 2 for VR
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &queueFamilyIndices,
		.preTransform = m_surfaceCapabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_FIFO_KHR, // vsync double buffer
		.clipped = VK_TRUE, // discard hidden pixels
		.oldSwapchain = nullptr,
	};

	if (vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swapchain");
	} else {
		std::cout << "swapchain created" << std::endl;
	}

	uint32_t swapchainImageCount = 0;
	vkGetSwapchainImagesKHR(device, m_swapchain, &swapchainImageCount, nullptr);
	m_swapchainImages.resize(swapchainImageCount);
	vkGetSwapchainImagesKHR(device, m_swapchain, &swapchainImageCount, m_swapchainImages.data());
}