#include "command.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "memory.h"
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
			std::cout << "created render finished semaphore " << i << std::endl;
		}
	}

	m_presentFinishedSemaphores.resize(FRAMES_IN_FLIGHT);
	m_frameFences.resize(FRAMES_IN_FLIGHT);
	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_presentFinishedSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create present finished semaphore");
		} else {
			std::cout << "created present finished semaphore " << i << std::endl;
		}

		if (vkCreateFence(device, &fenceInfo, nullptr, &m_frameFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create frame fence");
		} else {
			std::cout << "created frame fence " << i << std::endl;
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

void CommandContext::recordDrawCommands(PipelineContext& pipelineContext, SwapchainContext& swapchainContext, uint32_t imageIndex, VkBuffer vertexBuffer, std::vector<VkDescriptorSet> descriptorSets) {
	// begin a dynamic rendering pass
	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	VkRenderingAttachmentInfo renderingAttachmentInfo{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = swapchainContext.getSwapchainImageViews()[imageIndex],
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = clearColor, // clear to black screen
	};
	VkRenderingInfo renderingInfo{
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.renderArea = {.offset = {0, 0}, .extent = swapchainContext.getExtent() },
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &renderingAttachmentInfo,
	};
	vkCmdBeginRendering(m_commandBuffers[frameIndex], &renderingInfo);
	
	// bind the pipeline at the graphics bind point (shouldn't it always be the graphics point? what else do you use a pipeline for?)
	vkCmdBindPipeline(m_commandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineContext.getPipeline());

	// set the viewport and scissor, since they were defined as being dynamic during pipeline creation
	const VkViewport viewport = { 0.0f, 0.0f, static_cast<float>(swapchainContext.getExtent().width), static_cast<float>(swapchainContext.getExtent().height), 0.0f, 1.0f };
	vkCmdSetViewport(m_commandBuffers[frameIndex], 0, 1, &viewport);
	const VkRect2D scissorRectangle = { VkOffset2D(0, 0), swapchainContext.getExtent() };
	vkCmdSetScissor(m_commandBuffers[frameIndex], 0, 1, &scissorRectangle);

	// bind the vertex buffer with no memory offset
	uint64_t offset = 0;
	vkCmdBindVertexBuffers(m_commandBuffers[frameIndex], 0, 1, &vertexBuffer, &offset);


	vkCmdBindDescriptorSets(m_commandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineContext.getPipelineLayout(), 0, 1, &descriptorSets[frameIndex], 0, nullptr);


	vkCmdDraw(m_commandBuffers[frameIndex], 3, 1, 0, 0);

	vkCmdEndRendering(m_commandBuffers[frameIndex]);
}

void CommandContext::recordCommandBuffer(PipelineContext& pipelineContext, SwapchainContext& swapchainContext, uint32_t imageIndex, VkBuffer vertexBuffer, std::vector<VkDescriptorSet> descriptorSets) {
	// begin recording commands into command buffer
	VkCommandBufferBeginInfo commandBufferBeginInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, };
	vkBeginCommandBuffer(m_commandBuffers[frameIndex], &commandBufferBeginInfo);

	// transition image from undefined layout to color attachment optimal layout (prepare for drawing)
	transitionImageLayout(
		swapchainContext.getSwapchainImages()[imageIndex],
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		{}, // according to docs, empty because: "(no need to wait for previous operations)"
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
	);

	recordDrawCommands(pipelineContext, swapchainContext, imageIndex, vertexBuffer, descriptorSets);

	// transition image from color attachment optimal layout, to present src layout (prepare for presentation)
	transitionImageLayout(
		swapchainContext.getSwapchainImages()[imageIndex],
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
void CommandContext::drawFrame(DeviceContext& deviceContext, SwapchainContext& swapchainContext, PipelineContext& pipelineContext, MemoryManager& memoryManager) {
	memoryManager.updateUniformBuffers(frameIndex, swapchainContext.getExtent()); // this is not great, i'll fix it later

	// wait for this frame to finish execution of old command buffer
	if (vkWaitForFences(deviceContext.getDevice(), 1, &m_frameFences[frameIndex], VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
		throw std::runtime_error("failed to wait for frame fence");
	}
	if (vkResetFences(deviceContext.getDevice(), 1, &m_frameFences[frameIndex]) != VK_SUCCESS) {
		throw std::runtime_error("failed to reset frame fence");
	}

	// record new command buffer
	uint32_t imageIndex = 0;
	if (vkAcquireNextImageKHR(deviceContext.getDevice(), swapchainContext.getSwapchain(), UINT64_MAX, m_presentFinishedSemaphores[frameIndex], nullptr, &imageIndex) != VK_SUCCESS) {
		throw std::runtime_error("failed to acquire next image");
	}
	recordCommandBuffer(pipelineContext, swapchainContext, imageIndex, memoryManager.getVertexBuffer(), memoryManager.getDescriptorSets());

	// submit render commands to the graphics queue
	VkPipelineStageFlags waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // wait at the color output stage
	VkSubmitInfo submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_presentFinishedSemaphores[frameIndex],
		.pWaitDstStageMask = &waitStages, // wait at the color output stage
		.commandBufferCount = 1,
		.pCommandBuffers = &m_commandBuffers[frameIndex],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &m_renderFinishedSemaphores[imageIndex],
	};
	if (vkQueueSubmit(deviceContext.getGraphicsQueue(), 1, &submitInfo, m_frameFences[frameIndex]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit command buffer to queue");
	}

	// present finished frame
	const VkSwapchainKHR swapchain = swapchainContext.getSwapchain();
	VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_renderFinishedSemaphores[imageIndex],
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &imageIndex,
	};
	if (vkQueuePresentKHR(deviceContext.getGraphicsQueue(), &presentInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to present to queue");
	}

	// next frame
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