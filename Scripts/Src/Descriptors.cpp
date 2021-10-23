#include "Application.h"

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