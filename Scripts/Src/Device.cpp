#include "Device.h"



Device::Device(VkInstance *instance, GLFWwindow *window)
{
	createSurface(instance, window);
	pickPhysicalDevice(instance);
	createLogicalDevice();
}


Device::~Device()
{
}

void Device::Cleanup(VkInstance *instance)
{
	// Destroy the logical device
	vkDestroyDevice(logicalDevice, nullptr);

	// Destroy the surface
	// Using Vulkan function to destroy as GLFW doesn't offer a function to destroy the surface
	vkDestroySurfaceKHR(*instance, surface, nullptr);

}

// Function to find the memory type to create a buffer
uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

// Function to pick the graphics card in the system that supports the features required in the application
void Device::pickPhysicalDevice(VkInstance *instance) {

	// Stores the number of supporting GPUs in the system
	uint32_t deviceCount = 0;

	// Fetches the number of supporting GPUs in the system
	vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

	// Check whether the number of supporting GPUs is zero
	if (deviceCount == 0) {
		// There is no supporting GPU. Throw runtime error exception
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	// Stores the list of all supporting GPUs
	std::vector<VkPhysicalDevice> devices(deviceCount);

	// Fetches the list of all supporting GPUs
	vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

	// Loop through the GPUs
	for (const auto& device : devices) {

		// Check whether this GPU is suitable to application
		if (isDeviceSuitable(device)) {
			// Set the current GPU and break so that the current GPU is not assigned with a different value
			physicalDevice = device;
			break;
		}
	}

	// Check whether there is no suitable GPU found
	if (physicalDevice == VK_NULL_HANDLE) {
		// Throw runtime error exception as there is no suitable GPU
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

// Function to create logical device to interface with physical device
// Logical device - represents logical connections to physical device
void Device::createLogicalDevice() {
	// Fetch the Queue families for the GPU
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	// vector to store the queue create informations
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		// structure to store the queue create information for current queue
		VkDeviceQueueCreateInfo queueCreateInfo = {};

		// Type of information stored in struture
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

		// Queue family index
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();

		// Number of queue families
		queueCreateInfo.queueCount = 1;

		// Set the Queue Priority
		queueCreateInfo.pQueuePriorities = &queuePriority;

		// Push this queue to the vector

		queueCreateInfos.push_back(queueCreateInfo);
	}


	// Set of device features used in the application
	VkPhysicalDeviceFeatures deviceFeatures = {};

	deviceFeatures.samplerAnisotropy = VK_TRUE;

	deviceFeatures.geometryShader = VK_TRUE;

	// Information for creating the logical device
	VkDeviceCreateInfo createInfo = {};

	// Type of information stored in the structure
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	// Set the Queue create information
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	// Set the device features information
	createInfo.pEnabledFeatures = &deviceFeatures;

	// Set the extensions used
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	// Check whether validation layers are enabled
	if (enableValidationLayers) {
		// Set the validation layers in the create information
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		// Set the validation layers as zero
		createInfo.enabledLayerCount = 0;
	}

	// Create the logical device
	// 1st Parameter - the GPU to interface with
	// 2nd Parameter - Queue and usage information
	// 3rd Parameter - optional allocator callback
	// 4th Parameter - a pointer to variable to  store the logical device
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		// Logical device creation failed. So, throw runtime error exception
		throw std::runtime_error("failed to create logical device!");
	}

	// Create the graphics queue
	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);

	// Create the presentaion queue
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);

	// Create the compute queue
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &computeQueue);
}

// Function to check whether the device is suitable for the application
bool Device::isDeviceSuitable(VkPhysicalDevice device) {

	// Stores the properties of a device
	VkPhysicalDeviceProperties deviceProperties;

	// Stores the optional features supported by the device
	VkPhysicalDeviceFeatures deviceFeatures;

	// Fetch the properties of the device
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	// Fetch the features supported by the device
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	// Fetch the queue family indices supported
	QueueFamilyIndices indices = findQueueFamilies(device);

	// Flag to check Device extension support
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	// Flag to check swap chain properties are supported
	bool swapChainAdequate = false;

	// Check whether device extension is supported
	if (extensionsSupported) {

		// Query the swap chain support properties
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);

		// Check whether all swap chain properties are supported
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	// Check whether the device type, device features, queue families, device extensions and swap chain properties satisfy the conditions
	// Return the value evaluated
	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader &&
		indices.isComplete() &&
		extensionsSupported &&
		swapChainAdequate &&
		supportedFeatures.samplerAnisotropy;
}

// Function to  find the suitable Queue families
// Queue family - A family of queues only allow a subset of commands
QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) {

	if (device == nullptr)
		device = physicalDevice;
	// Stores the Queue family indices
	QueueFamilyIndices indices;

	// The no of queue families
	uint32_t queueFamilyCount = 0;

	// Fetch the number of queue families
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	// List of all queue families
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

	// Fetch the list of all queue families
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;

	// Loop through the queue families
	for (const auto& queueFamily : queueFamilies) {
		// Check whether queue family supports VK_QUEUE_GRAPHICS_BIT
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			// Assign the index of the graphics family
			indices.graphicsFamily = i;
		}

		// Check whether queue family supports presentation to the surface
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			// Assign the index of the presentation family
			indices.presentFamily = i;
		}

		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			indices.computeFamily = i;
		}

		// Break if there is a value assigned to indices
		if (indices.isComplete())
			break;
		i++;
	}

	// Return the Queue family indices
	return indices;
}

// Function to check whether the device supports all required extensions
bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {

	// Fetch the number of extensions supported by the device
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	// Fetch the list of extensions supported by the device
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// Set of strings used to represent the unconfirmed required extensions
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		// Remove the current iterate of available extensions from the set
		requiredExtensions.erase(extension.extensionName);
	}

	// If the set is empty, then all the required extensions are available in the device
	return requiredExtensions.empty();
}

// Function to query the swap chain support details
SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) {

	if (device == nullptr)
		device = physicalDevice;
	// Swap chain support properties
	SwapChainSupportDetails details;

	// Query the device surface capabilities
	// 1st Parameter - GPU
	// 2nd Parameter - surface
	// 3rd Parameter - pointer to device surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	// Fetch the number of surface formats supported
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	// Check whether the number of supported surface formats is not zero
	if (formatCount != 0) {
		// Resize the collection to store surface formats
		details.formats.resize(formatCount);

		// Fetch the collection of surface formats
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	// Fetch the number of presentation modes available
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	// Check whether the number of available presentation modes is not zero
	if (presentModeCount != 0) {
		// Resize the collection of presentation modes
		details.presentModes.resize(presentModeCount);

		// Fetch the collection of presentation modes
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	// Return the swap chain support details
	return details;
}

// Function to create surface
// Surface - to present rendered images
void Device::createSurface(VkInstance *instance, GLFWwindow *window) {
	// Create the surface using glfwCreateWindowSurface
	// glfwCreateWindowSurface takes care of platform specific implementation of creating the surface
	// 1st Parameter - Vulkan instance
	// 2nd Parameter - Window
	// 3rd Parameter - Custom allocator
	// 4th Parameter - pointer to the surface
	if (glfwCreateWindowSurface(*instance, window, nullptr, &surface) != VK_SUCCESS) {
		// Surface creation failed. Throw runtime error exception
		throw std::runtime_error("failed to create window surface!");
	}
}

