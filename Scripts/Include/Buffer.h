#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define GLFW_INCLUDE_VULKAN  // Vulkan Header from LunarG SDK, which provides Vulkan functions, structures and enumerations, is included automatically
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION

#include <GLFW/glfw3.h> // Include GLFW to render the objects in a window
#include <iostream> // Included for reporting errors
#include <stdexcept> // Included for reporting errors
#include <functional> // Used for Lambda functions in resource management
#include <cstdlib> // Provides EXIT_SUCCESS and EXIT_FAILURE macros
#include <optional> // Provides optional template
#include <set>  // Provides set
#include <cstdint> // Necessary for UINT32_MAX
#include <algorithm> // Necessary for min and max functions
#include <fstream> // Provides functions to read/write a file
#include <string> // String related function
#include <array>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Device.h"
#include "CommandPool.h"

// Class to represent any Vulkan Buffer
class Buffer
{
public:
	// Vulkan Buffer
	VkBuffer buffer;

	// Memory of the buffer
	VkDeviceMemory memory;

	// Constructor
	Buffer(Device *device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	
	// Destructor
	~Buffer();

	// Cleanup function
	void Cleanup(Device *device);

	// Function to set the data in the buffer
	void SetData(Device *device, void *value, VkDeviceSize bufferSize);

	void SetDataUsingStageBuffer(Device * device, void * value, VkDeviceSize bufferSize, CommandPool *commandPool);

	// Function to copy from one buffer to another
	void copyBuffer(Device *device, CommandPool *commandPool, Buffer *srcBuffer,VkDeviceSize size);
};

