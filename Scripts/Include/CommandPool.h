#pragma once
#include "Device.h"

// Class to represent a command
class CommandPool
{
public:
	// Command Pool
	VkCommandPool commandPool;

	// Constructor
	CommandPool(Device *device);

	// Destructor
	~CommandPool();

	// Function to create the command pool
	void createCommandPool(Device *device);

	// Function to begin single time commands
	VkCommandBuffer beginSingleTimeCommands(Device *device);
	
	// Function to end single time commands
	void endSingleTimeCommands(Device *device, VkCommandBuffer commandBuffer);
	
	// Cleanup functions
	void Cleanup(Device *device);
};

