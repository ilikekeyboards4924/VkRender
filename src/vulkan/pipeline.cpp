#include "pipeline.h"
#include <vector>
#include <iostream>
#include <stdexcept>

PipelineContext::PipelineContext(VkDevice device, VkExtent2D extent) {
	createGraphicsPipeline(device, extent);
}

void PipelineContext::createGraphicsPipeline(VkDevice device, VkExtent2D extent) {
	VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0, // zero for now, define these later for a vertex buffer
		.pVertexBindingDescriptions = nullptr,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = nullptr,
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};

	VkViewport viewport{ .width = static_cast<float>(extent.width), .height = static_cast<float>(extent.height) };
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

	// don't know, copied from vulkan docs
	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data(),
	};

	VkPipelineLayoutCreateInfo layoutInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0, // don't know, copied from vulkan docs
		.pushConstantRangeCount = 0, // don't know, copied from vulkan docs
	};
	if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout");
	} else {
		std::cout << "pipeline layout created" << std::endl;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2, // vertex and fragment
		.pStages = nullptr, // array of shaders
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

	if (vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) { // this will not work until i have written the SPIR-V file reading logic
		throw std::runtime_error("failed to create graphics pipeline");
	} else {
		std::cout << "graphics pipeline created" << std::endl;
	}
}

PipelineContext::~PipelineContext() {

}