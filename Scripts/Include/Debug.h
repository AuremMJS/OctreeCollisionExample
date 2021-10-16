#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> // Include GLFW to render the objects in a window

// Function to create VkDebugUtilsMessengerEXT object from Debug Messenger Info structure
// 1st Parameter - Vulkan Instance for the debug messenger to be created
// 2nd Parameter - structure of create info parameters used while creating the Debug Messenger
// 3rd Parameter - optional allocator callback
// 4th Parameter - the Debug Messenger created in this function
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger);

// Function to destroy VkDebugUtilsMessengerEXT object
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator);