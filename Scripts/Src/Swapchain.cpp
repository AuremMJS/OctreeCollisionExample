#include "Swapchain.h"

Swapchain::Swapchain(Device *device, GLFWwindow *window)
{
	createSwapChain(device, window);
	createImageViews(device);
}


Swapchain::~Swapchain()
{
}

void Swapchain::Cleanup(Device *device)
{
	// Destroy the image views
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device->logicalDevice, imageView, nullptr);
	}

	// Destroy the swap chain
	vkDestroySwapchainKHR(device->logicalDevice, swapChain, nullptr);
}


// Function to create Swap chain
// Swap chain - A queue of images waiting to be presented in the screen
void Swapchain::createSwapChain(Device *device, GLFWwindow *window) {
	// Query swap chain support properties
	SwapChainSupportDetails swapChainSupport = device->querySwapChainSupport();

	// Choose surface format
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

	// Choose presentation mode
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

	// Choose swap extent
	VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

	// No of images in swap chain
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	// Check whether the image count exceeds the max image count
	if (swapChainSupport.capabilities.maxImageCount > 0 
		&& imageCount > swapChainSupport.capabilities.maxImageCount) {
		// Set the image count to max image count
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	// Information to create swap chain
	VkSwapchainCreateInfoKHR createInfo = {};

	// Type of imformation stored in the structure
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

	// Assign Surface
	createInfo.surface = device->surface;

	// Assign Minimum Image count
	createInfo.minImageCount = imageCount;

	// Assign image format
	createInfo.imageFormat = surfaceFormat.format;

	// Assign color space
	createInfo.imageColorSpace = surfaceFormat.colorSpace;

	// Assign swap chain extent
	createInfo.imageExtent = extent;

	// Assign the amount of layers each image has
	createInfo.imageArrayLayers = 1;

	// Specify the usage of the image in the swap chain
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Fetch the Queue families
	QueueFamilyIndices indices = device->findQueueFamilies();

	// Store the queue family indices in an array
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), 
		indices.presentFamily.value() };

	// Check whether the graphics queue family and presentation queue family are different
	if (indices.graphicsFamily != indices.presentFamily) {
		// Use concurrent sharing - Images can be used across multiple queue 
		//families without explicit owner transfer
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		// Use exclusive sharing - Image is owned by one queue family at a time and 
		//ownership must be explicitly transferred
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	// Specify the transform to be applied to the image in the swap chain
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	// Specify if the alpha channel should be used for blending with other windows
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	// Set the presentation mode
	createInfo.presentMode = presentMode;

	// Set clipping to true
	createInfo.clipped = VK_TRUE;

	// Specify null as old swap chain
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// Create the swap chain
	// 1st Parameter - GPU
	// 2nd Parameter - Create info
	// 3rd Parameter - optional custom allocator
	// 4th Parameter - pointer to swap chain
	if (vkCreateSwapchainKHR(device->logicalDevice, &createInfo, nullptr, &swapChain) 
		!= VK_SUCCESS) {
		// Swap chain creation failed. Throw runtime error exception
		throw std::runtime_error("failed to create swap chain!");
	}

	// Get the number of swap chain images
	vkGetSwapchainImagesKHR(device->logicalDevice, swapChain, &imageCount, nullptr);

	// Resize the collection of swap chain images
	swapChainImages.resize(imageCount);

	// Fetch the swap chain images
	vkGetSwapchainImagesKHR(device->logicalDevice, swapChain, &imageCount, 
		swapChainImages.data());

	// Store the image format
	swapChainImageFormat = surfaceFormat.format;

	// store the extent
	swapChainExtent = extent;
}

// Function to choose swap extent for the swap chain
VkExtent2D Swapchain::chooseSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR& capabilities) {
	// Check whether the current extent width is not maximum
	if (capabilities.currentExtent.width != UINT32_MAX) {
		// return the current extent
		return capabilities.currentExtent;
	}
	else {
		// Get the current window width and height
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		// Choose the extent that best matches the window within minImageExtent and maxImageExtentBounds
		VkExtent2D actualExtent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}


// Function to choose the presentation mode for the swap chain
VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	// Loop through the available presentation modes
	for (const auto& availablePresentMode : availablePresentModes) {
		// Check if the presentation mode supports triple buffering
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			// Return the presentation mode supporting triple buffering
			return availablePresentMode;
		}
	}

	// Return FIFO presentation mode when no presentation mode supports triple buffering
	return VK_PRESENT_MODE_FIFO_KHR;
}

// Function to choose the surface format for the swap chain
VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	// Loop through the available surface formats
	for (const auto& availableFormat : availableFormats) {
		// Check whether the current iterate of the formats stores in order BGRA with 8 bit unsigned integer and whether SRGB color space is supported
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			// Return the format satifying the condition
			return availableFormat;
		}
	}

	// Return the first format if none of the formats satisfy the conditions
	return availableFormats[0];
}

// Function to create Image Views
// Image View - specifies a way to access the image and part of image to access
void Swapchain::createImageViews(Device *device) {
	swapChainImageViews.resize(swapChainImages.size());

	for (uint32_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = createImageView(device, swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

// Function to create a image view
VkImageView Swapchain::createImageView(Device *device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device->logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}
