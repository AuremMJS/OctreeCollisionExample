#include "Application.h"
#include "UI_Design.h"
#include "Debug.h"
#include "CollisionEngine\CollisionEngine.h"
// Constructor
Application::Application()
{

}

// Destructor
Application::~Application()
{
}

// Run function
void Application::run() {

	// Initialize a Window
	window.initWindow();

	// Initializes Vulkan resources
	initVulkan();

	// Main Loop where every frame is rendered
	mainLoop();

	// Cleanup and deallocate allocate resources
	cleanup();
}

// Function to initialize the Vulkan objects
void Application::initVulkan() {

	// Create the Instance
	// An  instance is the connection between the application and Vulkan library
	createInstance();

	// Setup the Debug Messenger
	setupDebugMessenger();

	// Create Device
	device = new Device(&instance, window.GetGLFWWindow());

	// Create Command Pool
	commandPool = new CommandPool(device);

	// Create Swap chain
	swapChain = new Swapchain(device, window.GetGLFWWindow());

	// Create render pass
	createRenderPass();

	// Create Descriptor Set layout
	createDescriptorSetLayout();

	// Create the graphics pipeline
	createGraphicsPipeline();

	// Init ImGUI
	imGui = new ImGuiHelper(&instance, window.GetGLFWWindow(), device, swapChain, commandPool);

	// Create the depth resources
	createDepthResources();

	// Create Frame Buffers
	createFramebuffers();

	// Parse the Object file
	ball = Mesh(MODEL,Vec3(-40,0,0), device,commandPool,swapChain->swapChainImages.size());

	ball2 = Mesh(MODEL, Vec3(40, 0, 0), device, commandPool, swapChain->swapChainImages.size());

	// Create the Descriptor Pool to create descriptor sets
	descriptorPool = createDescriptorPool(swapChain->swapChainImages.size() * 4, {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
		});

	// Create descriptor sets
	ball.createDescriptorSets(descriptorSetLayout, descriptorPool);

	ball2.createDescriptorSets(descriptorSetLayout, descriptorPool);
	ball2.SetStatic(true);

	// Create Command Buffers
	createCommandBuffers();

	// Create the semaphores and fences
	createSyncObjects();
}

// Function in which every frame is rendered
// Iterates until window is closed
void Application::mainLoop() {

	// Event loop to keep the application running until there is an error or window is closed
	while (!glfwWindowShouldClose(window.GetGLFWWindow())) {
		CollisionEngine::GetInstance()->SetActive(UIDesign::uiParams.isCollisionEnabled);

		CollisionEngine::GetInstance()->CollisionLoop();

		// Checks for events like Window close by the user
		glfwPollEvents();

		// Update ImGUI Min Image Count if resized
		if (framebufferResized)
		{
			imGui->UpdateMinImageCount();
		}

		// draw the frame
		drawFrame();
	}

	// Wait for the logical device to complete operations
	vkDeviceWaitIdle(device->logicalDevice);
}

// Function to destroy all Vulkan objects and free allocated resources
void Application::cleanup() {

	// Cleanup swap chain
	cleanupSwapChain();

	ball.Cleanup();
	ball2.Cleanup();
	// Destroy the descriptor sets
	vkDestroyDescriptorSetLayout(device->logicalDevice, descriptorSetLayout, nullptr);

	// Destroy the semaphores and fences
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		// Destroy the render finished semaphore
		vkDestroySemaphore(device->logicalDevice, renderFinishedSemaphores[i], nullptr);

		// Destroy the image available semaphore
		vkDestroySemaphore(device->logicalDevice, imageAvailableSemaphores[i], nullptr);

		// Destroy the fence
		vkDestroyFence(device->logicalDevice, inFlightFences[i], nullptr);
	}

	// Destroy command pool
	commandPool->Cleanup(device);

	// Check whether validation layers are enabled
	if (enableValidationLayers) {
		// Destroy the Debug Utils Messenger Extension
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	// Cleanup the device
	device->Cleanup(&instance);

	// Destroy the Vulkan instance
	// 1st Parameter - instance to be destoyed
	// 2nd Parameter - optional allocator callback
	vkDestroyInstance(instance, nullptr);

	// Destroy and cleanup the GLFW Window
	glfwDestroyWindow(window.GetGLFWWindow());

	// Terminate GLFW
	glfwTerminate();
}

// Function to create an  instance - the connection between the application and the Vulkan library
void Application::createInstance()
{
	// Check whether validation layers requested are supported
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// Structure to specify optional information about the application.
	// This provides useful information to the driver to optimize for the application
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // specifies the type of the information stored in the structure
	appInfo.pApplicationName = "Hello Triangle"; // Name of the application
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // Version of the application
	appInfo.pEngineName = "No Engine"; // Name of the engine used to create the engine
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // Version of engine used to create the engine
	appInfo.apiVersion = VK_API_VERSION_1_0; // Highest version of Vulkan that the application must use

	// Structure to specify which global extensions and validation layers should be used in the application
	// This is applied to the entire program and not to specific device
	VkInstanceCreateInfo createInfo = {};
	// Type of inforamtion stored in the structure
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	// Specify the application info
	createInfo.pApplicationInfo = &appInfo;

	// Retrieve the number of supported extensions and store in extensionCount
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// An array to hold the supported extension details
	std::vector<VkExtensionProperties> extensions(extensionCount);

	// Query the supported extension details
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// Print the supported extension details
	std::cout << "available extensions:" << std::endl;

	// Loop through the extensions and print
	for (const auto& extension : extensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}

	// Stores the reference to the extensions
	auto glfwExtensions = getRequiredExtensions();

	// Specifies the no of extensions and the extensions to use in the application
	createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
	createInfo.ppEnabledExtensionNames = glfwExtensions.data();

	// Stores the Debug Create Info
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	// Check whether Validation layers are enabled
	if (enableValidationLayers)
	{
		// Specifies the no of validation layers to use in the application
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());

		// Specifies the validation layers to use in the application
		createInfo.ppEnabledLayerNames = validationLayers.data();

		// Populate the Debug Messenger Create Info
		populateDebugMessengerCreateInfo(debugCreateInfo);

		// Set the Debug Messenger to pNext
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		// Set the no of validation layers as zero
		createInfo.enabledLayerCount = 0;

		// Set pNext to null
		createInfo.pNext = nullptr;
	}
	// Creates an Vulkan instance
	// 1st parameter - pointer to creation info
	// 2nd parameter - pointer to custom allocator callbacks
	// 3rd parameter - pointer to Vulkan instance
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {

		// Throw runtime error exception if the Vulkan instance creation fails
		throw std::runtime_error("failed to create instance!");
	}
}

// Function to set up the Debug Messenger
// Debug Messender - provides an explicit callback to print debug message in standard output
void Application::setupDebugMessenger() {

	// Return if the validation layers are disabled
	if (!enableValidationLayers) return;

	// Create and populate the Debug Messenger Create Info
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	// Create the Debug Messenger Extension
	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) 
		!= VK_SUCCESS) {
		// Throw runtime error if the extension creation fails
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

// Function to create render pass
// Render Pass - Information on no of color and depth buffers, no of samples to use and how the contents should be handled
void Application::createRenderPass() {
	// Color buffer attachment information
	VkAttachmentDescription colorAttachment = {};
	// format of colour buffer attachment
	colorAttachment.format = swapChain->swapChainImageFormat;
	// Number of samples
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	// Specify what to do with the data in the attachment before rendering
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// Specify what to do with the data in the attachment after rendering
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Specify what to do with the stencil data in the attachment before rendering
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// Specify what to do with the stencil data in the attachment after rendering
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Specify layout the image will have before the render pass
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Specify layout the image should transition to after render pass
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Attachment Reference for subpass
	VkAttachmentReference colorAttachmentRef = {};
	// Specify the attachment to refer
	colorAttachmentRef.attachment = 0;
	// Specify the layout to transition to when this subpass is started
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Subpass desciption
	// Subpass desciption
	std::array<VkSubpassDescription, 1> subpasses = {};

	// Specify where the subpass has to be executed
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	// Specify the number of color attachment references
	subpasses[0].colorAttachmentCount = 1;
	// Specify the pointer to the color attachment reference
	subpasses[0].pColorAttachments = &colorAttachmentRef;
	subpasses[0].pDepthStencilAttachment = &depthAttachmentRef;

	// Subpass dependency to make the render pass wait for the color attachment output bit stage
	std::array<VkSubpassDependency, 1> dependencies = {};
	// Specify the dependency and dependent subpasses
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	// Specify the operation to wait for. i.e, wait for the swap chain to read the image
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = 0;
	// Specify the operation that should wait
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

	// Render pass create info
	VkRenderPassCreateInfo renderPassInfo = {};
	// Type of information stored in the structure
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	// No of attachments in the render pass
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	// Pointer to the attachment
	renderPassInfo.pAttachments = attachments.data();
	// No of subpasses
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());; //1;
	// Pointer to the subpass
	renderPassInfo.pSubpasses = subpasses.data();// &subpass;
	// No of subpass dependencies
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());;// 1;
	// Pointer to the subpass dependency
	renderPassInfo.pDependencies = dependencies.data();// &dependency;

	// Create render pass
	// 1st Parameter - GPU
	// 2nd Parameter - Render pass create info
	// 3rd Parameter - Custom Allocator
	// 4th Parameter - Pointer to the created render pass
	if (vkCreateRenderPass(device->logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		// Throw runtime error exception as render pass creation failed
		throw std::runtime_error("failed to create render pass!");
	}
}

// Function to create the Graphics pipeline
// Graphics Pipeline - Sequence of operations with vertices & textures as input and pixels to render as output
void Application::createGraphicsPipeline() 
{
	std::unordered_map<VkShaderStageFlagBits, std::string> graphicsShaderMap;
	graphicsShaderMap.emplace(VK_SHADER_STAGE_VERTEX_BIT, "shaders/vert.spv");
	graphicsShaderMap.emplace(VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/frag.spv");

	AdditionalPipelineParams additionalParams = {};
	additionalParams.CullMode = VK_CULL_MODE_NONE;
	additionalParams.StencilTestEnable = VK_FALSE;
	additionalParams.ColorBlendEnable = VK_TRUE;
	additionalParams.SubPass = 0;

	graphicsPipeline.CreatePipeline(device, swapChain, renderPass, descriptorSetLayout,
		graphicsShaderMap,additionalParams);
}

// Function to create Frame buffers
// Frame buffers - Frame buffers represent a group of memory attachments used by a render pass instance
void Application::createFramebuffers() {
	// Resize the collection of frame buffers
	swapChainFramebuffers.resize(swapChain->swapChainImageViews.size());

	// Loop through the image views
	for (size_t i = 0; i < swapChain->swapChainImageViews.size(); i++) {

		// Attachments for current image view
		std::array<VkImageView, 2> attachments = {
swapChain->swapChainImageViews[i],
depthImage->imageView
		};

		// Frame buffer create info
		VkFramebufferCreateInfo framebufferInfo = {};
		// Type of information stored in the structure
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		// Specify the render pass
		framebufferInfo.renderPass = renderPass;
		// Specify the no of attachments for image view
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		// Specify the attachment for the image view
		framebufferInfo.pAttachments = attachments.data();
		// Specify width
		framebufferInfo.width = swapChain->swapChainExtent.width;
		// Specify height
		framebufferInfo.height = swapChain->swapChainExtent.height;
		// Specify the no of layers in the image
		framebufferInfo.layers = 1;

		// Create the frame buffer
		// 1st Parameter - GPU
		// 2nd Parameter - Frame buffer create info
		// 3rd Parameter - Custom Allocator
		// 4th Parameter - Pointer to the created frame buffer
		if (vkCreateFramebuffer(device->logicalDevice, &framebufferInfo, nullptr,
				&swapChainFramebuffers[i]) != VK_SUCCESS) {
			// Throw runtime error exception as framebuffer creation failed
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

// Function to find the suitable depth format
VkFormat Application::findDepthFormat() {
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

// Function to find the supported depth format
VkFormat Application::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device->physicalDevice, format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

// Function to create command buffers
// Command Buffers - All drawing operations are recorded in a command buffer
void Application::createCommandBuffers() {
	// resize the command buffers collection
	commandBuffers.resize(swapChainFramebuffers.size());

	// Command Buffer Allocate Info
	VkCommandBufferAllocateInfo allocInfo = {};
	// Type of information stored in the structure
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	// Specify the command pool
	allocInfo.commandPool = commandPool->commandPool;
	// Specify the command buffer is primary command buffer - can be submitted to 
	//queue for execution, but cannot be called from another other command buffer
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	// Specify the no of buffers to allocate
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	// Allocate command buffers
	// 1st Parameter - GPU
	// 2nd Parameter - Command Buffer allocate info
	// 3rd Parameter - Pointer to Command Buffers
	if (vkAllocateCommandBuffers(device->logicalDevice, &allocInfo, commandBuffers.data()) 
		!= VK_SUCCESS) {
		// Throw runtime error exception as allocation for command buffers failed
		throw std::runtime_error("failed to allocate command buffers!");
	}

	// Loop through the command buffers
	for (size_t i = 0; i < commandBuffers.size(); i++) {
		// Command buffer begin info to start command buffer recording
		VkCommandBufferBeginInfo beginInfo = {};
		// Type of information stored in the structure
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// Specify how the command buffer is used
		beginInfo.flags = 0; // Optional
		// Specify which state to inherit from, in case of secondary command buffer
		beginInfo.pInheritanceInfo = nullptr; // Optional

		// Start record the command buffer
		// 1st Parameter - command buffer to start recording
		// 2nd Parameter - Begin info
		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			// Throw runtime error exception
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		// Render pass begin info to start the render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		// Type of information stored in the structure
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		// Specify the render pass to start
		renderPassInfo.renderPass = renderPass;
		// Specify the swap chain frame buffers
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		// Specify the area where shader loads and stores take place
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->swapChainExtent;
		// Set the clear colour for the background
		// Set the number of clear colours
		// Specify the pointer to the clear colour

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.8f, 0.6f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// Start the render pass
		// 1st Parameter - command buffer to record the commands to
		// 2nd Parameter - render pass begin info
		// 3rd Parameter - whether the drawing commands are executed inline or executed from a secondary command buffers
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind the graphics pipeline
		// 1st Parameter - command buffer 
		// 2nd Parameter - whether the pipeline object is graphics pipeline or compute pipeline
		// 3rd Parameter - graphics pipeline
		//vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		graphicsPipeline.BindPipelineToCommandBuffer(commandBuffers[i]);
		
		VkDeviceSize offsets[] = { 0 };

		ball.Draw(commandBuffers[i],graphicsPipeline,i);

		ball2.Draw(commandBuffers[i], graphicsPipeline, i);

		ball.DrawAABB(commandBuffers[i], graphicsPipeline, i);

		ball2.DrawAABB(commandBuffers[i], graphicsPipeline, i);

		// End the render pass recording
		vkCmdEndRenderPass(commandBuffers[i]);

		// End the command buffer recording
		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			// Throw runtime error exception as command buffer recording cannot be ended
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}


// Function to create semaphores and fences
// Semaphores - A synchronization method where operations are synchronized within or across command queues
// Fences - A synchronization method where the entire application is synchronized with the rendering operation
void Application::createSyncObjects() {

	// Resize the collection of image available semaphores
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

	// Resize the collection of render finished semaphores
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

	// Resize the collection of in-flight fences
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	// Resize the collection of images in-flight fences
	imagesInFlight.resize(swapChain->swapChainImages.size(), VK_NULL_HANDLE);


	// create info for semaphores
	VkSemaphoreCreateInfo semaphoreInfo = {};
	// Type of information stored in the structure
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	// Create info for fences
	VkFenceCreateInfo fenceInfo = {};
	// Type of information stored in the strucuture
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	// Specify the flag to start the fences with signaled state
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	// Create all the semaphores and fences
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		// Create semaphores and fences

		// Create the semaphores
		// 1st Parameter - GPU
		// 2nd Parameter - create info for semaphores
		// 3rd Parameter - Custom allocator
		// 4th Parameter - pointer to the created semaphore

		// Create the fence
		// 1st Parameter - GPU
		// 2nd Parameter - create info for fence
		// 3rd Parameter - Custom allocator
		// 4th Parameter - pointer to the created fence
		if (vkCreateSemaphore(device->logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device->logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device->logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			// Any one or both the semaphore creation failed, so throw runtime error exception
			throw std::runtime_error("failed to create semaphores!");
		}
	}
}

// Function to check whether all the requested validation layers are available
// Validation layer - optional components hooked into Vulkan function calls to add validation check operations
bool Application::checkValidationLayerSupport() {

	// Find the number of available layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// Find the list of all available layers
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// Loop through the requested validation layers
	for (const char* layerName : validationLayers) {

		// flag to set when the requested validation layer is there in the list of available validation layers
		bool layerFound = false;

		// Loop through all the available validation layers
		for (const auto& layerProperties : availableLayers) {

			// Compare the layer name of the requested layer and layer in available layers 
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				// The requested layer is there in the available layers list
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			// The requested layer is not there in the available layers list
			return false;
		}
	}

	return true;
}

// Function that returns the list of extenstions based on whether validation layers are enabled or not
std::vector<const char*> Application::getRequiredExtensions() {

	// Stores the no of extensions
	uint32_t glfwExtensionCount = 0;

	// Stores the reference to the extensions
	const char** glfwExtensions;

	// GLFW function to fetch the extensions required
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// A list of extensions
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// Check whether validation layers are enabled
	if (enableValidationLayers) {
		// Add Debug Utils Extension
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	// Return the extensions
	return extensions;
}

// Function to populate the Debug Messenger Create Info
void Application::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	// Specify the structure type
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	// Specify all types of severities for the callback
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	// Specify the types of messages for the callback to be called
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	// Pointer to the callback function to be called
	createInfo.pfnUserCallback = debugCallback;
}

// Debug callback function
// VKAPI_ATTR and VKAPI_CALL ensures the right signature for Vulkan to call it
// 1st Parameter - Severity of message - Diagnostic, Information, Warning or Error
// 2nd Parameter - Message Type - General, Validation or Performance
// 3rd Parameter - Contains the details of the message with members pMessage,pObjects and objectsCount
// 4th Parameter - Used to pass data
// Return value - Should the Vulkan call that triggered the validation layer message should be aborted
VKAPI_ATTR VkBool32 VKAPI_CALL Application::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	// Print the validation error
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	// Do not abort the Vulkan call that triggered the validation layer message
	return VK_FALSE;
}

//////////////////////////
// Main Loop Functions //
////////////////////////

// Function to draw the frame on the screen
// Acquires the image from the swap chain and executes command buffer and returns the image to swap chain for presentation
void Application::drawFrame() {
	//UIDesign::uiParams.peakWavelength = peakWavelengths[UIDesign::uiParams.IncidencAngleToVerify];
	imGui->DrawImGUI(device, UIDesign::DrawUI);

	// Wait for frame to finish
	vkWaitForFences(device->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	// index of swap chain image
	uint32_t imageIndex;
	// Acquire image from swap chain
	// 1st Parameter - GPU
	// 2nd Parameter - Swap chain
	// 3rd Parameter - timeout in nanoseconds for an image to become available
	// 4th & 5th Parameter - synchronization object to signal when presentation engine used this image
	// 6th Parameter - index of swap chain image
	VkResult result = vkAcquireNextImageKHR(device->logicalDevice, swapChain->swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	// Check whether swap chain has become incompatible with the surface
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		// Recreate swap chain
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		// Throw runtime error exception as acquiring image from swap chain failed
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		// Wait for the fences
		vkWaitForFences(device->logicalDevice, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	// Mark the image as now being in use by this frame
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	// Update the uniform buffer to have the current model view projection matrices
	ball.updateUniformBuffer(imageIndex, window, swapChain);
	ball2.updateUniformBuffer(imageIndex, window, swapChain);

	// Update the uniform buffer to have the current ambient, specular, diffuse values
	ball.updateLightingConstants(imageIndex, window, swapChain);
	ball2.updateLightingConstants(imageIndex, window, swapChain);

	imGui->RenderImGUI(device, swapChain->swapChainExtent, imageIndex);

	std::array<VkCommandBuffer, 2> submitCommandBuffers =
	{
		commandBuffers[imageIndex]
		,
		imGui->imGuiCommandBuffers[imageIndex]
	};

	// Submit info to submit to command buffer
	VkSubmitInfo submitInfo = {};
	// Type of information stored in the structure
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	// Semaphores to wait for
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	// Pipeline stage to wait
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	// Set the no of semaphores
	submitInfo.waitSemaphoreCount = 1;
	// Set the semaphores to wait for
	submitInfo.pWaitSemaphores = waitSemaphores;
	// Set the stages to wait
	submitInfo.pWaitDstStageMask = waitStages;
	// Set the no of command buffers to submit
	submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size()); //1;
	// Set the pointer to command buffers to submit
	submitInfo.pCommandBuffers = submitCommandBuffers.data();// &commandBuffers[imageIndex];
	// Semaphores to signal after command buffer execution
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	// Set the no of semaphores to signal
	submitInfo.signalSemaphoreCount = 1;
	// Set the semaphores to signal
	submitInfo.pSignalSemaphores = signalSemaphores;

	// Reset the fence to unsignaled state
	vkResetFences(device->logicalDevice, 1, &inFlightFences[currentFrame]);

	// Submit the command buffer to graphics queue
	// 1st Parameter - graphics queue
	// 2nd Parameter - No of submit infos
	// 3rd Parameter - pointer to submit infos
	// 4th Parameter - optional fence signaled when command buffer is executed
	if (vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		// Throw runtime error exception as the submission of command buffer to graphics queue failed
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	// Configuring the presentation using present info
	VkPresentInfoKHR presentInfo = {};
	// Type of information stored in the structure
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	// no of semaphores to wait for before presentation
	presentInfo.waitSemaphoreCount = 1;
	// semaphores to wait for before presentation
	presentInfo.pWaitSemaphores = signalSemaphores;
	// Array of swap chains
	VkSwapchainKHR swapChains[] = { swapChain->swapChain };
	// Set the no of swap chains
	presentInfo.swapchainCount = 1;
	// Specify the swap chains
	presentInfo.pSwapchains = swapChains;
	// Specify the index of image for each swap chain	
	presentInfo.pImageIndices = &imageIndex;
	// Result values to check in each swap chain
	presentInfo.pResults = nullptr;

	// Submit a request to present the image to swap chain
	result = vkQueuePresentKHR(device->presentQueue, &presentInfo);

	// Check whether the swap chain has become incompatible with the surface or the surface properties no longer match or frame buffer is resized
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {

		// Set frame buffer resized flag to false
		framebufferResized = false;

		// Recreate swap chain
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		// Throw runtime error exception as request to present the image to swap chain failed
		throw std::runtime_error("failed to present swap chain image!");
	}

	// Update the current frame index
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// Function to recreate swap chain and other components when window is resized
void Application::recreateSwapChain() {

	// Get the width and height of the window to check whether it is minimized
	int width = 0, height = 0;
	glfwGetFramebufferSize(window.GetGLFWWindow(), &width, &height);
	// Check whether the window is minimized
	while (width == 0 || height == 0) {
		// Update the width and height
		glfwGetFramebufferSize(window.GetGLFWWindow(), &width, &height);
		// Wait for the window to be maximized
		glfwWaitEvents();
	}
	// Wait for the device to complete its operations
	vkDeviceWaitIdle(device->logicalDevice);

	// Cleanup existing swap chain and dependent components
	cleanupSwapChain();

	swapChain = new Swapchain(device, window.GetGLFWWindow());

	// Create Render Pass
	createRenderPass();

	// Create Graphics pipeline
	createGraphicsPipeline();

	//createQuadGraphicsPipeline();

	createDepthResources();

	// Create frame buffers
	createFramebuffers();

	// Recreate uniform buffers
	ball.createUniformBuffers();
	ball2.createUniformBuffers();
	// Recreate the descriptor sets

	descriptorPool = createDescriptorPool(swapChain->swapChainImages.size() * 4, {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
		});
	ball.createDescriptorSets(descriptorSetLayout, descriptorPool);
	ball2.createDescriptorSets(descriptorSetLayout, descriptorPool);
	// Create command buffers
	createCommandBuffers();

	//Init ImGUI
	imGui->InitImGUI(&instance, window.GetGLFWWindow(), device, swapChain, commandPool);
}

// Cleanup function to destroy swap chain and related components
void Application::cleanupSwapChain() {

	// Destroy the depth image view, depth image and depth memory
	depthImage->Cleanup(device);

	// Destroy the frame buffers
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(device->logicalDevice, framebuffer, nullptr);
	}

	// Destroy the graphics pipeline
	graphicsPipeline.DestroyPipeline();

	// Destroy the render pass
	vkDestroyRenderPass(device->logicalDevice, renderPass, nullptr);

	swapChain->Cleanup(device);

	ball.CleanupUniformBuffers();
	ball2.CleanupUniformBuffers();
	// Destroy the descriptor pool
	vkDestroyDescriptorPool(device->logicalDevice, descriptorPool, nullptr);

	// Cleanup ImGUI
	imGui->CleanupImGUI(device);
}

// Function to create depth resources for depth buffer
void Application::createDepthResources() {
	VkFormat depthFormat = findDepthFormat();
	depthImage = new Image(device, swapChain->swapChainExtent.width, swapChain->swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_TYPE_2D);
}

void Application::OnWindowResized()
{
	// Set the frame buffer resized flag to true
	framebufferResized = true;
}