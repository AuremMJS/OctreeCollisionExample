#include "Application.h"


// function to create descriptor set layout to  provide the details about descriptor bindings in every shader
void Application::createQuadDescriptorSetLayout() {
	std::vector<DescriptorSetLayoutInfo> descriptorSetLayouts;
	descriptorSetLayouts.push_back(DescriptorSetLayoutInfo(
		{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }));

	quadDescriptorSetLayout = CreateDescriptorSetLayout(descriptorSetLayouts);
}


// Function to create descriptor sets for each Vk Buffer
void Application::createQuadDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(swapChain->swapChainImages.size(),
		quadDescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = quadDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain->swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	quadDescriptorSets.resize(swapChain->swapChainImages.size());
	if (vkAllocateDescriptorSets(device->logicalDevice, &allocInfo,
		quadDescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < swapChain->swapChainImages.size(); i++) {

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = refImage->textureImage->imageView;
		imageInfo.sampler = refImage->textureSampler;

		std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = quadDescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device->logicalDevice,
			static_cast<uint32_t>(descriptorWrites.size()),
			descriptorWrites.data(), 0, nullptr);
	}
}

// function to create descriptor set layout for compute pipeline to  provide the details about descriptor bindings in every shader
void Application::createComputeDescriptorSetLayout() {

	std::vector<DescriptorSetLayoutInfo> descriptorSetLayouts;
	descriptorSetLayouts.push_back(DescriptorSetLayoutInfo(
		{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT }));
	descriptorSetLayouts.push_back(DescriptorSetLayoutInfo(
		{ 5, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT }));

	computeDescriptorSetLayout = CreateDescriptorSetLayout(descriptorSetLayouts);
}


// Function to create descriptor sets of compute pipeline for each Vk Buffer
void Application::createComputeDescriptorSet() {
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = computeDescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &computeDescriptorSetLayout;

	if (vkAllocateDescriptorSets(device->logicalDevice, &allocInfo, &computeDescriptorSet)
		!= VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	VkDescriptorBufferInfo ComputeBufferInfo = {};
	ComputeBufferInfo.buffer = ComputeBuffers->buffer;
	ComputeBufferInfo.offset = 0;
	ComputeBufferInfo.range = ComputeBufferSize;

	VkDescriptorBufferInfo spectralParametersBufferInfo = {};
	spectralParametersBufferInfo.buffer = SpectralParametersBuffer->buffer;
	spectralParametersBufferInfo.offset = 0;
	spectralParametersBufferInfo.range = sizeof(SpectralPipelineParameters);

	std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = computeDescriptorSet;
	descriptorWrites[0].dstBinding = 4;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &ComputeBufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = computeDescriptorSet;
	descriptorWrites[1].dstBinding = 5;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pBufferInfo = &spectralParametersBufferInfo;

	vkUpdateDescriptorSets(device->logicalDevice,
		static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}


// function to create descriptor set layout to  provide the details about descriptor bindings in every shader
void Application::createDescriptorSetLayout() {

	std::vector<DescriptorSetLayoutInfo> descriptorSetLayouts;
	descriptorSetLayouts.push_back(DescriptorSetLayoutInfo(
		{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT }));
	descriptorSetLayouts.push_back(DescriptorSetLayoutInfo(
		{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT }));
	descriptorSetLayouts.push_back(DescriptorSetLayoutInfo(
		{ 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }));

	descriptorSetLayout = CreateDescriptorSetLayout(descriptorSetLayouts);
}

VkDescriptorSetLayout Application::CreateDescriptorSetLayout(std::vector<DescriptorSetLayoutInfo> descriptorSetLayouts)
{
	VkDescriptorSetLayout _descriptorSetLayout;
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	for (auto descriptorSetLayout : descriptorSetLayouts)
	{
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = descriptorSetLayout.Binding;
		layoutBinding.descriptorType = descriptorSetLayout.Type;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = descriptorSetLayout.Stage;
		layoutBinding.pImmutableSamplers = nullptr; // Optional

		bindings.push_back(layoutBinding);
	}
	
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device->logicalDevice, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	return _descriptorSetLayout;
}

// Function to create descriptor pool to create descriptor sets
VkDescriptorPool Application::createDescriptorPool(int descriptorCount, std::initializer_list<VkDescriptorType> descriptorTypes) {
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorPoolSize> poolSizes = {};
	for (auto descriptorType : descriptorTypes)
	{
		VkDescriptorPoolSize poolSize;
		poolSize.type = descriptorType;
		poolSize.descriptorCount = descriptorCount;
		poolSizes.push_back(poolSize);
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = descriptorCount;

	if (vkCreateDescriptorPool(device->logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
	return descriptorPool;
}