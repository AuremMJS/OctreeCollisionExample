#pragma once
#include "Device.h"
#include "CommandPool.h"
#include "Swapchain.h"

// Class to handle Dear ImGUI Elements
class ImGuiHelper
{
public:
	// Descriptor Pool to create descriptor sets
	VkDescriptorPool imGUIDescriptorPool;

	// Minimum image count
	int imGUI_MinImageCount = 2;

	// ImGUI Render pass
	VkRenderPass imGUIRenderPass;

	// ImGUI command Pool
	VkCommandPool imGuiCommandPool;

	// ImGUI Command Buffers
	std::vector<VkCommandBuffer> imGuiCommandBuffers;

	// ImGUI Frame Buffers
	std::vector<VkFramebuffer> imGUIFramebuffers;

	// Constructor
	ImGuiHelper(VkInstance *instance, GLFWwindow *window, Device *device, Swapchain *swapChain, CommandPool *commandPool);
	
	// Destructor
	~ImGuiHelper();

	// Init Function
	void InitImGUI(VkInstance *instance, GLFWwindow *window, Device *device, Swapchain *swapChain,CommandPool *commandPool);
	
	// Function to create command pool
	void createimGuiCommandPool(Device *device,  VkCommandPoolCreateFlags flags);
	
	// Function to create command buffers
	void createimGuiCommandBuffers(Device *device, uint32_t commandBufferCount);
	
	// Function to create render pass
	void createImGUIRenderPass(Device *device, VkFormat swapChainImageFormat);
	
	// Function to create frame buffers
	void CreateImGuiFrameBuffer(Device *device, Swapchain *swapChain);

	// Function to create descriptor pool
	void CreateImGUIDescriptorPool(Device *device);

	// Cleanup function
	void CleanupImGUI(Device *device);

	// Function to draw GUI elements
	void DrawImGUI(Device *device, void(*drawFunc)());

	// Function to render the GUI elements
	void RenderImGUI(Device *device, VkExtent2D swapChainExtent, uint32_t imageIndex);
	
	// Function to update min image count
	void UpdateMinImageCount();

	// Debug Function
	static void check_vk_result(VkResult err);
};

