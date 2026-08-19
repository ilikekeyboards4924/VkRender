#include "command.h"
#include "swapchain.h"
#include <vector>
#include <iostream>
#include <stdexcept>

CommandContext::CommandContext(VkDevice device, uint32_t queueFamilyIndex, uint32_t swapchainImagesCount) {
	m_device = device;

	createCommandPool(device, queueFamilyIndex);
	createCommandBuffers(device);
	createSynchronizationObjects(device, swapchainImagesCount);
}

void CommandContext::createCommandPool(VkDevice device, uint32_t queueFamilyIndex) {
	VkCommandPoolCreateInfo commandPoolInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queueFamilyIndex,
	};
	
	vkCreateCommandPool(device, &commandPoolInfo, nullptr, &m_commandPool);
}

void CommandContext::createCommandBuffers(VkDevice device) {
	VkCommandBufferAllocateInfo commandBufferInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = m_commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = FRAMES_IN_FLIGHT,
	};
	
	m_commandBuffers.resize(FRAMES_IN_FLIGHT);

	if (vkAllocateCommandBuffers(device, &commandBufferInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers");
	} else {
		std::cout << "command buffers allocated" << std::endl;
	}
}

void CommandContext::createSynchronizationObjects(VkDevice device, uint32_t swapchainImagesCount) {
	VkSemaphoreCreateInfo semaphoreInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, };
	VkFenceCreateInfo fenceInfo{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT, };
	
	m_renderFinishedSemaphores.resize(swapchainImagesCount);
	for (uint32_t i = 0; i < swapchainImagesCount; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render finished semaphore");
		} else {
			std::cout << "render finished semaphore created" << std::endl;
		}
	}

	m_presentFinishedSemaphores.resize(FRAMES_IN_FLIGHT);
	m_frameFences.resize(FRAMES_IN_FLIGHT);
	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_presentFinishedSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create present finished semaphore");
		} else {
			std::cout << "present finished semaphore created" << std::endl;
		}

		if (vkCreateFence(device, &fenceInfo, nullptr, &m_frameFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create frame fence");
		} else {
			std::cout << "frame fence created" << std::endl;
		}
	}
}

void CommandContext::transitionImageLayout(
	VkImage image,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkAccessFlags2 srcAccessBitmask,
	VkAccessFlags2 dstAccessBitmask,
	VkPipelineStageFlags2 srcStageBitmask,
	VkPipelineStageFlags2 dstStageBitmask
) {
	VkImageMemoryBarrier2 barrier{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = srcStageBitmask,
		.srcAccessMask = srcAccessBitmask,
		.dstStageMask = dstStageBitmask,
		.dstAccessMask = dstAccessBitmask,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};

	VkDependencyInfo dependencyInfo{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.dependencyFlags = {}, // don't know, copied from vulkan docs
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier,
	};

	vkCmdPipelineBarrier2(m_commandBuffers[frameIndex], &dependencyInfo);
}

void CommandContext::recordCommandBuffer(VkPipeline pipeline, VkImage image, VkImageView imageView, VkExtent2D extent, VkBuffer vertexBuffer) {
	VkCommandBufferBeginInfo commandBufferBeginInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, };
	vkBeginCommandBuffer(m_commandBuffers[frameIndex], &commandBufferBeginInfo);

	// transition image from undefined layout to color attachment optimal layout (prepare for drawing)
	transitionImageLayout(
		image,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		{}, // according to docs, empty because: "(no need to wait for previous operations)"
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
	);

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	VkRenderingAttachmentInfo renderingAttachmentInfo{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = imageView,
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = clearColor, // clear to black screen
	};

	VkRenderingInfo renderingInfo{
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.renderArea = { .offset = {0, 0}, .extent = extent },
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &renderingAttachmentInfo,
	};

	vkCmdBeginRendering(m_commandBuffers[frameIndex], &renderingInfo);
	vkCmdBindPipeline(m_commandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	const VkViewport viewport = { 0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f };
	vkCmdSetViewport(m_commandBuffers[frameIndex], 0, 1, &viewport);
	const VkRect2D scissorRectangle = { VkOffset2D(0, 0), extent };
	vkCmdSetScissor(m_commandBuffers[frameIndex], 0, 1, &scissorRectangle);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(m_commandBuffers[frameIndex], 0, 1, &vertexBuffer, &offset);

	vkCmdDraw(m_commandBuffers[frameIndex], 3, 1, 0, 0);

	vkCmdEndRendering(m_commandBuffers[frameIndex]);

	// transition image from color attachment optimal layout, to present src layout (prepare for presentation)
	transitionImageLayout(
		image,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		{},
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
	);

	vkEndCommandBuffer(m_commandBuffers[frameIndex]);
}

// ran into issues earlier, because i was passing swapchainContext by value, instead of by reference
void CommandContext::drawFrame(VkDevice device, SwapchainContext& swapchainContext, VkPipeline pipeline, VkQueue queue, VkBuffer vertexBuffer) {
	//std::cout << "DRAW FRAME FUNCTION CHECKPOINT : ONE" << std::endl;

	if (vkWaitForFences(device, 1, &m_frameFences[frameIndex], VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
		throw std::runtime_error("failed to wait for frame fence");
	}
	vkResetFences(device, 1, &m_frameFences[frameIndex]);

	uint32_t imageIndex = 0;
	if (vkAcquireNextImageKHR(device, swapchainContext.getSwapchain(), UINT64_MAX, m_presentFinishedSemaphores[frameIndex], nullptr, &imageIndex) != VK_SUCCESS) {
		throw std::runtime_error("failed to acquire next image");
	}

	//std::cout << "DRAW FRAME FUNCTION CHECKPOINT : TWO" << std::endl;

	recordCommandBuffer(pipeline, swapchainContext.getSwapchainImages()[imageIndex], swapchainContext.getSwapchainImageViews()[imageIndex], swapchainContext.getExtent(), vertexBuffer);

	VkPipelineStageFlags pipelineStageFlags = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // i mean.... this sounds right?
	VkSubmitInfo submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_presentFinishedSemaphores[frameIndex],
		.pWaitDstStageMask = &pipelineStageFlags, // is this correct? no idea
		.commandBufferCount = 1,
		.pCommandBuffers = &m_commandBuffers[frameIndex],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &m_renderFinishedSemaphores[imageIndex],
	};

	vkQueueSubmit(queue, 1, &submitInfo, m_frameFences[frameIndex]);

	const VkSwapchainKHR swapchain = swapchainContext.getSwapchain();
	VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_renderFinishedSemaphores[imageIndex],
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &imageIndex,
	};

	//std::cout << "DRAW FRAME FUNCTION CHECKPOINT : THREE" << std::endl;

	if (vkQueuePresentKHR(queue, &presentInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to present to queue");
	}

	//std::cout << "DRAW FRAME FUNCTION CHECKPOINT : FOUR" << std::endl;

	frameIndex = (frameIndex + 1) % 2;
}

CommandContext::~CommandContext() {
	vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	for (uint32_t i = 0; i < m_renderFinishedSemaphores.size(); i++) {
		vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
	}
	for (uint32_t i = 0; i < m_presentFinishedSemaphores.size(); i++) {
		vkDestroySemaphore(m_device, m_presentFinishedSemaphores[i], nullptr);
	}
	for (uint32_t i = 0; i < m_frameFences.size(); i++) {
		vkDestroyFence(m_device, m_frameFences[i], nullptr);
	}
}