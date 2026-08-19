#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>

#include "src/vulkan/instance.h"
#include "src/vulkan/device.h"
#include "src/vulkan/swapchain.h"
#include "src/vulkan/pipeline.h"
#include "src/vulkan/command.h"

#include "src/vulkan/vertex.h"

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "VkRender", nullptr, nullptr);
	
	InstanceContext instanceContext;
	DeviceContext deviceContext(instanceContext.getInstance());

	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(instanceContext.getInstance(), window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create surface");
	}
	else {
		std::cout << "surface created" << std::endl;
	}

	{
		SwapchainContext swapchainContext(deviceContext.getDevice(), deviceContext.getPhysicalDevice(), surface, deviceContext.getGraphicsFamilyIndex());
		PipelineContext pipelineContext(deviceContext.getDevice(), swapchainContext.getFormat(), swapchainContext.getExtent());
		CommandContext commandContext(deviceContext.getDevice(), deviceContext.getGraphicsFamilyIndex(), swapchainContext.getSwapchainImages().size());

		uint32_t queueFamilyIndex = deviceContext.getGraphicsFamilyIndex();
		VkBufferCreateInfo bufferInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = vertices.size() * sizeof(Vertex),
			.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &queueFamilyIndex,
		};

		VkMemoryAllocateInfo memoryAllocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = 1024 * 1024, // megabyte
			.memoryTypeIndex = deviceContext.getMemoryTypeIndex(),
		};

		VkDeviceMemory deviceMemory;
		if (vkAllocateMemory(deviceContext.getDevice(), &memoryAllocateInfo, nullptr, &deviceMemory) != VK_SUCCESS) {
			throw std::runtime_error("failure");
		}

		VkBuffer vertexBuffer;
		if (vkCreateBuffer(deviceContext.getDevice(), &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failure");
		}
		if (vkBindBufferMemory(deviceContext.getDevice(), vertexBuffer, deviceMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failure");
		}

		void* data;
		if (vkMapMemory(deviceContext.getDevice(), deviceMemory, 0, bufferInfo.size, 0, &data)) {
			throw std::runtime_error("failure");
		}

		std::memcpy(data, vertices.data(), (size_t)bufferInfo.size);

		vkUnmapMemory(deviceContext.getDevice(), deviceMemory);

		while (!glfwWindowShouldClose(window)) {
			commandContext.drawFrame(deviceContext.getDevice(), swapchainContext, pipelineContext.getPipeline(), deviceContext.getGraphicsQueue(), vertexBuffer);

			glfwPollEvents();
		}

		vkDeviceWaitIdle(deviceContext.getDevice());
	}

	vkDestroySurfaceKHR(instanceContext.getInstance(), surface, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}