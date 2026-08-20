#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class PipelineContext {
public:
	PipelineContext(VkDevice device, VkFormat format, VkExtent2D extent);
	~PipelineContext();

	VkPipeline getPipeline() const { return m_graphicsPipeline; };
	VkPipelineLayout getPipelineLayout() const { return m_pipelineLayout; };

	VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; };
private:
	std::vector<char> readShaderFile(const std::string& filename);
	VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& codeBytes);
	void createGraphicsPipeline(VkDevice device, VkFormat format, VkExtent2D extent);

	VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

	VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

	VkShaderModule m_vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule m_fragmentShaderModule = VK_NULL_HANDLE;

	VkDevice m_device = VK_NULL_HANDLE; // use for destruction
};