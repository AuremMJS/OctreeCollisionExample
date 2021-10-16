#pragma once
#include "Swapchain.h"

struct AdditionalPipelineParams
{
	VkCullModeFlagBits CullMode;
	VkBool32 StencilTestEnable;
	VkBool32 ColorBlendEnable;
	int SubPass;
};

struct OptionalPipelineParams
{
	VkBool32 DepthBiasEnable;
	int DepthBiasConstantFactor;
	int DepthBiasClamp;
	int DepthBiasSlopeFactor;
};

class Pipeline
{
public:
	void CreatePipeline(Device *device, Swapchain *swapChain, VkRenderPass renderPass,
		VkDescriptorSetLayout descriptorSetLayout,
		std::unordered_map<VkShaderStageFlagBits, std::string> shaderMap,
		AdditionalPipelineParams additionalParams, OptionalPipelineParams optionalParams = OptionalPipelineParams(),
		bool isComputePipeline = false);

	void DestroyPipeline();

	void BindPipelineToCommandBuffer(VkCommandBuffer commandBuffer);

	void BindDescriptorSets(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet);
private:
	bool isComputePipeline;
	Device *device;
	VkPipelineShaderStageCreateInfo *shaderStages;
	VkShaderModule *shaderModules;
	int noOfShaders;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	std::vector<VkVertexInputBindingDescription> bindingDescription;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	VkPipelineViewportStateCreateInfo viewportState;
	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineRasterizationStateCreateInfo rasterizer;
	VkPipelineMultisampleStateCreateInfo multisampling;
	VkPipelineDepthStencilStateCreateInfo depthStencil;
	VkPipelineColorBlendStateCreateInfo colorBlending;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineDynamicStateCreateInfo dynamicState;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	void CreateShaderStages(std::unordered_map<VkShaderStageFlagBits, std::string> shaderMap);
	void CreateVertexInputInfo();
	void CreateInputAssemblyInfo(VkPrimitiveTopology topology);
	void CreateViewportStateInfo(Swapchain *swapChain);
	VkViewport CreateViewport(Swapchain *swapChain);
	VkRect2D CreateScissor(Swapchain *swapChain);
	void CreateRasterizerInfo(VkCullModeFlagBits cullMode, OptionalPipelineParams optionalParams);
	void CreateMultisampleInfo();
	void CreateDepthStencilInfo(VkBool32 stencilTestEnable);
	void CreateColorBlendInfo(VkBool32 colorBlendEnable);
	void CreateDynamicStatesInfo();
	void CreatePipelineLayoutInfo(VkDescriptorSetLayout descriptorSetLayout);
	void CreateGraphicsPipeline(VkRenderPass renderPass, int subpass);
	std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void DestroyShaderModules();
	void CreateComputePipeline();
};