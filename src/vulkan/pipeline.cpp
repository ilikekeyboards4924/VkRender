#include "pipeline.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "vertex.h" // sketchy

PipelineContext::PipelineContext(VkDevice device, VkFormat format, VkExtent2D extent) {
	m_device = device;

	createGraphicsPipeline(device, format, extent);
}

std::vector<char> PipelineContext::readShaderFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file");
	}

	std::vector<char> buffer(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

	file.close();

	std::cout << "read file \"" << filename << "\" of size: " << buffer.size() << std::endl;

	if (buffer.size() % 4 != 0) {
		throw std::runtime_error("shader file size is not a multiple of 4");
	}

	return buffer;
}

VkShaderModule PipelineContext::createShaderModule(VkDevice device, const std::vector<char>& codeBytes) {
	VkShaderModuleCreateInfo shaderModuleInfo{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = codeBytes.size(),
		.pCode = reinterpret_cast<const uint32_t*>(codeBytes.data()) // copied from vulkan docs
	};

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module");
	} else {
		std::cout << "shader module created" << std::endl;
	}

	return shaderModule;
}

void PipelineContext::createGraphicsPipeline(VkDevice device, VkFormat format, VkExtent2D extent) {
	VkPipelineRenderingCreateInfo renderingInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &format,
	};

	m_vertexShaderModule = createShaderModule(device, readShaderFile("shader/vertex.vert.spv"));
	VkPipelineShaderStageCreateInfo vertexShaderStageInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = m_vertexShaderModule, // shader code in vector of bytes
		.pName = "main", // entry point of the shader program
	};
	m_fragmentShaderModule = createShaderModule(device, readShaderFile("shader/fragment.frag.spv"));
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = m_fragmentShaderModule, // shader code in vector of bytes
		.pName = "main", // entry point of the shader program
	};
	VkPipelineShaderStageCreateInfo shaderStages[2] = { vertexShaderStageInfo, fragmentShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertexBindingDescription,
		.vertexAttributeDescriptionCount = 3,
		.pVertexAttributeDescriptions = vertexAttributeDescriptions.data(),
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};

	VkViewport viewport{ 
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(extent.width), 
		.height = static_cast<float>(extent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	VkRect2D scissorRectangle{ .extent = extent };
	VkPipelineViewportStateCreateInfo viewportStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissorRectangle,
	};

	VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.lineWidth = 1.0f, // vulkan gets mad if you dont have this, even if its "polygon fill" mode
	};

	VkPipelineMultisampleStateCreateInfo multisampleStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT, // don't know, copied from vulkan docs
		.sampleShadingEnable = VK_FALSE,
	};

	VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	};

	VkPipelineColorBlendStateCreateInfo colorBlendStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY, // don't know, copied from vulkan docs
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachmentState,
	};

	// normally viewport and scissor rectangles are baked into pipeline at creation, but this tells the pipeline that these two things have dynamic states
	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data(),
	};

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
	};
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = 1,
		.pBindings = &descriptorSetLayoutBinding,
	};
	if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout");
	}

	VkPipelineLayoutCreateInfo layoutInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1, // descriptor set layout count
		.pSetLayouts = &m_descriptorSetLayout,
		.pushConstantRangeCount = 0, // push constant range count
	};
	if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout");
	} else {
		std::cout << "pipeline layout created" << std::endl;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &renderingInfo,
		.stageCount = 2, // vertex and fragment
		.pStages = shaderStages, // array of shaders
		.pVertexInputState = &vertexInputStateInfo,
		.pInputAssemblyState = &inputAssemblyStateInfo,
		.pViewportState = &viewportStateInfo,
		.pRasterizationState = &rasterizationStateInfo,
		.pMultisampleState = &multisampleStateInfo,
		.pColorBlendState = &colorBlendStateInfo,
		.pDynamicState = &dynamicStateInfo,
		.layout = m_pipelineLayout,
		.renderPass = nullptr, // not needed since dynamic rendering
	};

	if (vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline");
	} else {
		std::cout << "graphics pipeline created" << std::endl;
	}
}

PipelineContext::~PipelineContext() {
	vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	vkDestroyShaderModule(m_device, m_vertexShaderModule, nullptr);
	vkDestroyShaderModule(m_device, m_fragmentShaderModule, nullptr);
	vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
}