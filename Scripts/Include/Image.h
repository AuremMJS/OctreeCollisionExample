#pragma once
#include "Device.h"
#include "CommandPool.h"
#include "Buffer.h"

// Class representing an image
class Image
{
public:
	// Texture Image
	VkImage image;

	// Memory for Texture Image
	VkDeviceMemory memory;

	// Texture Image View
	VkImageView imageView;

	// Constructor
	Image(Device *device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags, VkImageType type);
	
	// Destructor
	~Image();

	// Functions to create the image
	void createImage(Device *device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageType type);
	void createImageView(Device *device, VkFormat format, VkImageAspectFlags aspectFlags);
	
	// Function to change the image layout
	void transitionImageLayout(Device *device, CommandPool *commandPool, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	
	// Function to copy data from buffer to image
	void copyBufferToImage(Device *device, CommandPool *commandPool, Buffer *buffer, uint32_t width, uint32_t height);
	
	// Cleanup function
	void Cleanup(Device *device);
};

