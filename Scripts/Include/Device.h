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


// Flag to specify whether Validation layers should be enabled or not
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// Structure to  store the indices of the Queue family
struct QueueFamilyIndices {
	// Graphics family
	std::optional<uint32_t> graphicsFamily;

	// Presentation family
	std::optional<uint32_t> presentFamily;

	// Compute family
	std::optional<uint32_t> computeFamily;

	// Function to check whether graphicsFamily has a value
	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
	}
};


// Device extensions to be enabled
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Validation layers to be enabled
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

// Structure to store the swap chain support properties
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

// Class to represent physical and logical device
class Device
{
	
public:
	// Handle to store the graphics card suitable to the application
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	// Handle to store the logical device to interface with the physical device
	VkDevice logicalDevice;

	// Handle to graphics queue
	VkQueue graphicsQueue;

	// Handle to presentation queue
	VkQueue presentQueue;

	// Handle to compute queue
	VkQueue computeQueue;

	// Instance of Surface
	VkSurfaceKHR surface;

	// Constructor
	Device(VkInstance *instance, GLFWwindow *window);

	// Destructor
	~Device();

	// Cleanup function
	void Cleanup(VkInstance *instance);

	// Function to pick the physical GPU
	void pickPhysicalDevice(VkInstance *instance);

	// Function to create the logical device
	void createLogicalDevice();

	// Function to check whether the program is suitable in the selected device
	bool isDeviceSuitable(VkPhysicalDevice device);

	// Function to find the memory type
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	
	// Function to find the queue families
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device = nullptr);

	// Function to check the support for an extension
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	// Function to verify the support to the swap chain
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device = nullptr);

	// Function to create the surface
	void createSurface(VkInstance *instance, GLFWwindow *window);
};