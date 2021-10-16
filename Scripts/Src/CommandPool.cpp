#include "CommandPool.h"



CommandPool::CommandPool(Device *device)
{
	createCommandPool(device);
}


CommandPool::~CommandPool()
{
}

// Function to create command pool
// Command Pool - Manage the memory used to store the buffers. Command buffers are allocated from Command Pools
void CommandPool::createCommandPool(Device *device) {
	// Query the queue family indices
	QueueFamilyIndices queueFamilyIndices = device->findQueueFamilies();

	// Command pool create info
	VkCommandPoolCreateInfo poolInfo = {};
	// Type of information stored in the structure
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	// Specify the graphics queue family index as the commands are for drawing
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	// Set the flag to zero as command buffers will be recorded only at the program start
	poolInfo.flags = 0; // Optional

	// Create command pool
	// 1st Parameter - GPU
	// 2nd Parameter - command pool create info
	// 3rd Parameter - Custom allocator
	// 4th Parameter - pointer to created command pool
	if (vkCreateCommandPool(device->logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		// Throw runtime error exception as command pool creation failed
		throw std::runtime_error("failed to create command pool!");
	}
}

// Function to start single time commands in command buffer
VkCommandBuffer CommandPool::beginSingleTimeCommands(Device *device) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device->logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

// Function to end single time commands in command buffer
void CommandPool::endSingleTimeCommands(Device *device,VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->graphicsQueue);

	vkFreeCommandBuffers(device->logicalDevice, commandPool, 1, &commandBuffer);
}

void CommandPool::Cleanup(Device * device)
{
	vkDestroyCommandPool(device->logicalDevice, commandPool, nullptr);
}
