#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class SwapchainContext {
public:
	SwapchainContext(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t queueFamilyIndex);
	~SwapchainContext();

	VkSwapchainKHR getSwapchain() const { return m_swapchain; };

	VkExtent2D getExtent() const { return m_surfaceCapabilities.currentExtent; };
	VkFormat getFormat() const { return m_surfaceFormat.format; };

	std::vector<VkImage> getSwapchainImages() const { return m_swapchainImages; };
	std::vector<VkImageView> getSwapchainImageViews() const { return m_swapchainImageViews; };
private:
	void pickSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	void pickSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	void createSwapchain(VkDevice device, VkSurfaceKHR surface, uint32_t queueFamilyIndex);
	void createSwapchainImageViews(VkDevice device);
	
	VkSurfaceFormatKHR m_surfaceFormat;
	VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;

	VkDevice m_device = VK_NULL_HANDLE; // use for destruction
};