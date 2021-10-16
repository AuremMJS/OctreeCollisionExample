#pragma once

#include "Device.h"

class Swapchain
{
public:

	// Handle to Swap chain
	VkSwapchainKHR swapChain;

	// Handles to swap chain images
	std::vector<VkImage> swapChainImages;

	// Swap chain image format
	VkFormat swapChainImageFormat;

	// Swap chain extent
	VkExtent2D swapChainExtent;

	// Swap chain image views
	std::vector<VkImageView> swapChainImageViews;

	Swapchain(Device *device, GLFWwindow *window);
	~Swapchain();

	void Cleanup(Device *device);
	
	void createSwapChain(Device *device, GLFWwindow *window);
	void createImageViews(Device *device);
	VkImageView createImageView(Device *device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkExtent2D chooseSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR& capabilities);
};

