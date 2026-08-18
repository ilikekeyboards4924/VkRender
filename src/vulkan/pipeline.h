#pragma once
#include <vulkan/vulkan.h>

class PipelineContext {
public:
	PipelineContext(VkDevice device, VkFormat format, VkExtent2D extent);
	~PipelineContext();

	VkPipeline getPipeline() const { return m_graphicsPipeline; };
private:
	void createGraphicsPipeline(VkDevice device, VkFormat format, VkExtent2D extent);

	VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout;
};