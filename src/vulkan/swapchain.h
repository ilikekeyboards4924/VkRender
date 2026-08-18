#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class SwapchainContext {
public:
	SwapchainContext(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	~SwapchainContext();

	VkExtent2D getExtent() const { return m_surfaceCapabilities.currentExtent; };
private:
	void pickSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	void pickSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	void createSwapchain(VkDevice device, VkSurfaceKHR surface, uint32_t queueFamilyIndex);

	VkSurfaceFormatKHR m_surfaceFormat;
	VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

	std::vector<VkImage> m_swapchainImages;
};