#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class CommandContext {
public:
	CommandContext(VkDevice device, uint32_t queueFamilyIndex, uint32_t swapchainImagesCount);
	~CommandContext();

	void drawFrame(VkDevice device);
private:
	void createCommandPool(VkDevice device, uint32_t queueFamilyIndex);
	void createCommandBuffers(VkDevice device);
	void createSynchronizationObjects(VkDevice device, uint32_t swapchainImagesCount);

	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;

	std::vector<VkSemaphore> m_renderFinishedSemaphores; // signal once finished rendering to image view
	std::vector<VkSemaphore> m_presentFinishedSemaphores; // signal once previous frame presentation has finished
	std::vector<VkFence> m_frameFences; // signal once command buffer commands are all finished for this frame

	uint32_t FRAMES_IN_FLIGHT = 2; // 2 for double buffer
	uint32_t frameIndex = 0; // current frame being worked on. not necessarily the current image being rendered to.

	VkDevice m_device = VK_NULL_HANDLE; // use for destruction
};