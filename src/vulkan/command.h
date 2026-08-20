#pragma once
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "memory.h"
#include <vulkan/vulkan.h>
#include <vector>

class CommandContext {
public:
	CommandContext(VkDevice device, uint32_t queueFamilyIndex, uint32_t swapchainImagesCount);
	~CommandContext();

	void drawFrame(DeviceContext& deviceContext, SwapchainContext& swapchainContext, PipelineContext& pipelineContext, MemoryManager& memoryManager);
private:
	void createCommandPool(VkDevice device, uint32_t queueFamilyIndex);
	void createCommandBuffers(VkDevice device);
	void createSynchronizationObjects(VkDevice device, uint32_t swapchainImagesCount);
	void transitionImageLayout(
		VkImage image,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkAccessFlags2 srcAccessBitmask,
		VkAccessFlags2 dstAccessBitmask,
		VkPipelineStageFlags2 srcStageBitmask,
		VkPipelineStageFlags2 dstStageBitmask
	);
	void recordDrawCommands(PipelineContext& pipelineContext, SwapchainContext& swapchainContext, uint32_t imageIndex, VkBuffer vertexBuffer, std::vector<VkDescriptorSet> descriptorSets);
	void recordCommandBuffer(PipelineContext& pipelineContext, SwapchainContext& swapchainContext, uint32_t imageIndex, VkBuffer vertexBuffer, std::vector<VkDescriptorSet> descriptorSets);

	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;

	std::vector<VkSemaphore> m_renderFinishedSemaphores; // signal once finished rendering to image view
	std::vector<VkSemaphore> m_presentFinishedSemaphores; // signal once previous frame presentation has finished
	std::vector<VkFence> m_frameFences; // signal once command buffer commands are all finished for this frame

	uint32_t FRAMES_IN_FLIGHT = 2; // 2 for double buffer
	uint32_t frameIndex = 0; // current frame being worked on. not necessarily the current image being rendered to.

	VkDevice m_device = VK_NULL_HANDLE; // use for destruction
};