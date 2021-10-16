#include "Pipeline.h"
#include "BindingAttributeDescriptionHelper.h"

void Pipeline::CreatePipeline(Device *device, Swapchain *swapChain, VkRenderPass renderPass,
	VkDescriptorSetLayout descriptorSetLayout, 
	std::unordered_map<VkShaderStageFlagBits, std::string> shaderMap,
	AdditionalPipelineParams additionalParams, OptionalPipelineParams optionalParams,
	bool isComputePipeline) 
{
	this->isComputePipeline = isComputePipeline;
	this->device = device;
	
	CreateShaderStages(shaderMap);

	if (!isComputePipeline)
	{
		// Information of format of the vertex data passed to the vertex shader
		CreateVertexInputInfo();

		CreateInputAssemblyInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		CreateViewportStateInfo(swapChain);

		CreateRasterizerInfo(additionalParams.CullMode, optionalParams);

		CreateMultisampleInfo();

		CreateDepthStencilInfo(additionalParams.StencilTestEnable);

		CreateColorBlendInfo(additionalParams.ColorBlendEnable);

		CreateDynamicStatesInfo();
	}

	CreatePipelineLayoutInfo(descriptorSetLayout);

	if (!isComputePipeline)
	{
		CreateGraphicsPipeline(renderPass, additionalParams.SubPass);
	}
	else
	{
		CreateComputePipeline();
	}
	DestroyShaderModules();
}

void Pipeline::DestroyPipeline()
{
	// Destroy the graphics pipeline
	vkDestroyPipeline(device->logicalDevice, pipeline, nullptr);

	// Destroy the pipeline layout
	vkDestroyPipelineLayout(device->logicalDevice, pipelineLayout, nullptr);
}

void Pipeline::BindPipelineToCommandBuffer(VkCommandBuffer commandBuffer)
{
	VkPipelineBindPoint pipelineBindPoint = !isComputePipeline ?
		VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;

	// Bind the pipeline
	// 1st Parameter - command buffer 
	// 2nd Parameter - whether the pipeline object is graphics pipeline or compute pipeline
	// 3rd Parameter - pipeline
	vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

void Pipeline::BindDescriptorSets(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet)
{
	VkPipelineBindPoint pipelineBindPoint = !isComputePipeline ?
		VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;

	vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint,
		pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	
}

void Pipeline::CreateShaderStages(std::unordered_map<VkShaderStageFlagBits,std::string> shaderFileMap)
{
	noOfShaders = shaderFileMap.size();
	shaderStages = new VkPipelineShaderStageCreateInfo[noOfShaders];
	shaderModules = new VkShaderModule[noOfShaders];
	int i = 0;
	for (auto shaderFile : shaderFileMap)
	{
		// Fetch the byte code of shader
		auto shaderCode = readFile(shaderFile.second);

		// Create Vertex shader module
		VkShaderModule shaderModule = createShaderModule(shaderCode);

		VkPipelineShaderStageCreateInfo shaderStageInfo = {};
		// Type of information stored in the structure
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		// Specify the vertex shader stage in the pipeline
		shaderStageInfo.stage = shaderFile.first;
		// Specify the vertex shader module
		shaderStageInfo.module = shaderModule;
		// Specify the entry point
		shaderStageInfo.pName = "main";

		shaderStages[i] = shaderStageInfo;
		shaderModules[i] = shaderModule;
		i++;
	}
}

void Pipeline::CreateVertexInputInfo()
{
	// Binding description
	bindingDescription = BindingAttributeDescriptionHelper::getBindingDescription();

	// Attribute description
	attributeDescriptions = BindingAttributeDescriptionHelper::getAttributeDescriptions();

	// Information of format of the vertex data passed to the vertex shader
	vertexInputInfo = {};
	// Type of information stored in the structure
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	// Details for loading vertex data
	vertexInputInfo.vertexBindingDescriptionCount =
		static_cast<uint32_t>(bindingDescription.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
	vertexInputInfo.vertexAttributeDescriptionCount =
		static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
}

// Information of kind of geometry drawn
void Pipeline::CreateInputAssemblyInfo(VkPrimitiveTopology topology)
{
	// Type of information stored in the structure
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	// Specify kind of geometry drawn
	inputAssembly.topology = topology;
	// Specify that it is not possible to break lines and triangles by using special index
	inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void Pipeline::CreateViewportStateInfo(Swapchain *swapChain)
{
	viewport = CreateViewport(swapChain);

	scissor = CreateScissor(swapChain);

	// Type of information stored in the structure
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	// No of viewports
	viewportState.viewportCount = 1;
	// Specify the pointer to the viewports
	viewportState.pViewports = &viewport;
	// No of scissors
	viewportState.scissorCount = 1;
	// Specify the pointer to the scissors
	viewportState.pScissors = &scissor;
}

VkViewport Pipeline::CreateViewport(Swapchain *swapChain)
{
	// Viewport information
	// Viewport - region of framebuffer where the output will be rendereds
	VkViewport viewport = {};
	// starting x of viewport
	viewport.x = 0.0f;
	// starting y of viewport
	viewport.y = 0.0f;
	// width of viewport
	viewport.width = (float)swapChain->swapChainExtent.width;
	// height of viewport
	viewport.height = (float)swapChain->swapChainExtent.height;
	// min depth of viewport
	viewport.minDepth = 0.0f;
	// max depth of viewport
	viewport.maxDepth = 1.0f;

	return viewport;
}

VkRect2D Pipeline::CreateScissor(Swapchain *swapChain)
{
	// Scissor information
	// Scissor - a specification of the pixels that will be stored
	VkRect2D scissor = {};
	// Scissor offset
	scissor.offset = { 0, 0 };
	// Scissor extent
	scissor.extent = swapChain->swapChainExtent;

	return scissor;
}

void Pipeline::CreateRasterizerInfo(VkCullModeFlagBits cullMode, OptionalPipelineParams optionalParams)
{
	// Rasterization State create info
	// Rasterizer - turns the geometry into fragments

	// Type of information stored in the structure
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	// Specify whether the fragments beyond the near and far planes has to be clamped or discarded
	rasterizer.depthClampEnable = VK_FALSE;
	// Specify whether the geometry should pass through the rasterizer stage
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	// Specify how fragments are generated
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	// Specify the thickness of the lines
	rasterizer.lineWidth = 1.0f;
	// Specify the type of culling to use
	rasterizer.cullMode = cullMode;
	// Specify the vertex order
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	// Should the rasterizer alter the depth values by adding a constant value or biasing them
	rasterizer.depthBiasEnable = optionalParams.DepthBiasEnable;// VK_FALSE;
	rasterizer.depthBiasConstantFactor = optionalParams.DepthBiasConstantFactor;// 0.0f; // Optional
	rasterizer.depthBiasClamp = optionalParams.DepthBiasClamp;// 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = optionalParams.DepthBiasSlopeFactor;// 0.0f; // Optional
}

void Pipeline::CreateMultisampleInfo()
{
	// Information to configure multisampling to perform anti-aliasing
	
	// Type of information stored in the structure
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional
}

void Pipeline::CreateDepthStencilInfo(VkBool32 stencilTestEnable)
{
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = stencilTestEnable;
}

void Pipeline::CreateColorBlendInfo(VkBool32 colorBlendEnable)
{
	// Colour blending configuration per attached framebuffer
	// Colour blending - way to combine with colour already in framebuffer
	colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = colorBlendEnable;
	if (colorBlendEnable)
	{
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	}
	// Global colour blending setting information
	colorBlending = {};
	// Type of information stored in the structure
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
}

void Pipeline::CreateDynamicStatesInfo()
{
	// Dynamic states which can be changed without recreating the pipeline
	VkDynamicState dynamicStates[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_LINE_WIDTH
	};


	// Dynamic state create info
	
	// Type of information stored in the structure
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	// No of dynamic states
	dynamicState.dynamicStateCount = 2;
	// Specify the array of dynamic states
	dynamicState.pDynamicStates = dynamicStates;
}

void Pipeline::CreatePipelineLayoutInfo(VkDescriptorSetLayout descriptorSetLayout)
{
	// Pipeline layout create info
	// Pipeline layout - Uniform values/Push constants specified during pipeline creation
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	// Type of information stored in the structure
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	// Number of uniforms
	pipelineLayoutInfo.setLayoutCount = 1;

	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	// Number of push constants
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	// Create the pipeline layout
	if (vkCreatePipelineLayout(device->logicalDevice, &pipelineLayoutInfo, nullptr,
		&pipelineLayout) != VK_SUCCESS) {
		// Throw runtime error exception as pipeline layout creation failed
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void Pipeline::CreateGraphicsPipeline(VkRenderPass renderPass, int subpass)
{
	// Graphics pipeline create info
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	// Type of information stored in the structure
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	// No of shader stages
	pipelineInfo.stageCount = noOfShaders;
	// Pointer to shader stages
	pipelineInfo.pStages = shaderStages;
	// Specify the vertex input state
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	// Specify the input assembly state
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	// Specify the viewport state
	pipelineInfo.pViewportState = &viewportState;
	// Specify the rasterization state
	pipelineInfo.pRasterizationState = &rasterizer;
	// Specify the Multisampling state
	pipelineInfo.pMultisampleState = &multisampling;
	// Specify Depth stencil state
	pipelineInfo.pDepthStencilState = &depthStencil;
	// Specify the color blend state
	pipelineInfo.pColorBlendState = &colorBlending;
	// Specify the dynamic state
	pipelineInfo.pDynamicState = nullptr; // Optional
	// Specify the pipeline layout
	pipelineInfo.layout = pipelineLayout;
	// Specify the render pass
	pipelineInfo.renderPass = renderPass;
	// Specify the index of the render pass where this graphics pipeline will be used
	pipelineInfo.subpass = subpass;
	// Specify the base pipeline to derive from
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	// Specify the index of base pipeline to derive from
	pipelineInfo.basePipelineIndex = -1; // Optional

	// Create the graphics pipeline
	// 1st Parameter - GPU
	// 2nd Parameter - Pipeline cache to store and reuse data for pipeline creation
	// 3rd Parameter - Pipeline create info
	// 4th Parameter - Custom allocator
	// 5th Parameter - Pointer to the created graphics pipeline
	if (vkCreateGraphicsPipelines(device->logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo,
		nullptr, &pipeline) != VK_SUCCESS) {
		// Throw runtime error exception as graphics pipeline creation failed
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

// Function to read a file
std::vector<char> Pipeline::readFile(const std::string& filename) {
	// Open the file as a binary and start reading at the end of file
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	// Check whether file opening failed
	if (!file.is_open()) {
		// Throw runtime error exception if file opening failed
		throw std::runtime_error("failed to open file!");
	}

	// Find the file size
	size_t fileSize = (size_t)file.tellg();

	// Initialize a buffer with file size
	std::vector<char> buffer(fileSize);

	// Seek to the beginning of the file
	file.seekg(0);

	// Read all the bytes
	file.read(buffer.data(), fileSize);

	// Close the file
	file.close();

	// Return the buffer
	return buffer;
}

// Function to create shader module
// This function takes byte code of the shader as paramter and returns the shader module
VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
	// Create info for shader module
	VkShaderModuleCreateInfo createInfo = {};
	// Type of information stored in the structure
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	// Size of byte code
	createInfo.codeSize = code.size();
	// Assign the byte code. Cast it uint32_t pointer from char pointer
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	// Create the shader module
	// 1st Parameter - GPU
	// 2nd Parameter - Create Info
	// 3rd Parameter - optional custom allocator
	// 4th Parameter - pointer to shader module
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device->logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		// Throw runtime error exception as the shader module creation failed
		throw std::runtime_error("failed to create shader module!");
	}

	// Return shader module
	return shaderModule;
}

void Pipeline::DestroyShaderModules()
{
	for (int i = 0; i < noOfShaders; i++)
	{
		vkDestroyShaderModule(device->logicalDevice, shaderModules[i], nullptr);
	}
}

void Pipeline::CreateComputePipeline()
{
	// Compute pipeline create info
	VkComputePipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stage = shaderStages[0];
	pipelineCreateInfo.layout = pipelineLayout;

	// Create the compute pipeline
	// 1st Parameter - GPU
	// 2nd Parameter - Pipeline cache to store and reuse data for pipeline creation
	// 3rd Parameter - Pipeline create info
	// 4th Parameter - Custom allocator
	// 5th Parameter - Pointer to the created compute pipeline
	if (vkCreateComputePipelines(device->logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline) != VK_SUCCESS) {
		// Throw runtime error exception as compute pipeline creation failed
		throw std::runtime_error("failed to create compute pipeline!");
	}
}