#include "command.h"
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

void CommandContext::drawFrame(VkDevice device) {
	if (vkWaitForFences(device, 1, &m_frameFences[frameIndex], VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
		throw std::runtime_error("failed to wait for frame fence");
	}
	vkResetFences(device, 1, &m_frameFences[frameIndex]);
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