#include "ImGuiHelper.h"

// ImGUI Includes
#include "ImGUI\imgui.h"
#include "ImGUI\imgui_impl_glfw.h"
#include "ImGUI\imgui_impl_vulkan.h"


ImGuiHelper::ImGuiHelper(VkInstance *instance, GLFWwindow *window, Device *device, Swapchain *swapChain, CommandPool *commandPool)
{
	InitImGUI(instance, window, device, swapChain, commandPool);
}


ImGuiHelper::~ImGuiHelper()
{
}


void ImGuiHelper::InitImGUI(VkInstance *instance, GLFWwindow *window, Device *device, Swapchain *swapChain, CommandPool *commandPool)
{
	// Setup ImGui Context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForVulkan(window, true); // It lets ImGUI to interact with GLFW
												// For example, DearImGUI will have access to the keyboard/mouse events
												// But still run user registered callbacks

	CreateImGUIDescriptorPool(device);
	createImGUIRenderPass(device, swapChain->swapChainImageFormat);

	// InitInfo - Bridge between engine and ImGUI
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = *instance; // instance
	init_info.PhysicalDevice = device->physicalDevice; // physical device
	init_info.Device = device->logicalDevice; // logical device

	QueueFamilyIndices queueFamilyIndices = device->findQueueFamilies();
	init_info.QueueFamily = queueFamilyIndices.graphicsFamily.value(); // graphics queue family
	init_info.Queue = device->graphicsQueue; // graphics queue

	init_info.PipelineCache = VK_NULL_HANDLE; // Pipeline Cache is not used here

	init_info.DescriptorPool = imGUIDescriptorPool; // imGUI Descriptor Pool
	init_info.Allocator = nullptr; // Not used - nullptr
	init_info.MinImageCount = imGUI_MinImageCount; // ImGUI swap chain min image count
	init_info.ImageCount = swapChain->swapChainImageViews.size(); // Swap chain images count
	init_info.CheckVkResultFn = check_vk_result; // error handling function for ImGUI

	// Initialize
	ImGui_ImplVulkan_Init(&init_info, imGUIRenderPass);

	// Upload Fonts
	VkCommandBuffer command_buffer = commandPool->beginSingleTimeCommands(device);
	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
	commandPool->endSingleTimeCommands(device, command_buffer);

	imGuiCommandBuffers.resize(swapChain->swapChainImages.size());
	createimGuiCommandPool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	createimGuiCommandBuffers(device, static_cast<uint32_t>(imGuiCommandBuffers.size()));
	CreateImGuiFrameBuffer(device, swapChain);
}

void ImGuiHelper::createimGuiCommandPool(Device *device, VkCommandPoolCreateFlags flags) {
	QueueFamilyIndices indices = device->findQueueFamilies();
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	commandPoolCreateInfo.flags = flags;

	if (vkCreateCommandPool(device->logicalDevice, &commandPoolCreateInfo, nullptr, &imGuiCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("Could not create graphics command pool");
	}
}

void ImGuiHelper::createimGuiCommandBuffers(Device *device, uint32_t commandBufferCount) {
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandPool = imGuiCommandPool;
	commandBufferAllocateInfo.commandBufferCount = commandBufferCount;
	vkAllocateCommandBuffers(device->logicalDevice, &commandBufferAllocateInfo, 
		imGuiCommandBuffers.data());
}

// Function to create render pass
// Render Pass - Information on no of color and depth buffers, no of samples to use and how the contents should be handled
void ImGuiHelper::createImGUIRenderPass(Device *device, VkFormat swapChainImageFormat) {
	// Color buffer attachment information
	VkAttachmentDescription colorAttachment = {};
	// format of colour buffer attachment
	colorAttachment.format = swapChainImageFormat;
	// Number of samples
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	// Specify what to do with the data in the attachment before rendering
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	// Specify what to do with the data in the attachment after rendering
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Specify what to do with the stencil data in the attachment before rendering
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// Specify what to do with the stencil data in the attachment after rendering
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Specify layout the image will have before the render pass
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	// Specify layout the image should transition to after render pass
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Attachment Reference for subpass
	VkAttachmentReference colorAttachmentRef = {};
	// Specify the attachment to refer
	colorAttachmentRef.attachment = 0;
	// Specify the layout to transition to when this subpass is started
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Subpass desciption
	VkSubpassDescription subpass = {};
	// Specify where the subpass has to be executed
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	// Specify the number of color attachment references
	subpass.colorAttachmentCount = 1;
	// Specify the pointer to the color attachment reference
	subpass.pColorAttachments = &colorAttachmentRef;

	// Subpass dependency to make the render pass wait for the color attachment output bit stage
	VkSubpassDependency dependency = {};
	// Specify the dependency and dependent subpasses
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	// Specify the operation to wait for. i.e, wait for the swap chain to read the image
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	// Specify the operation that should wait
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// Render pass create info
	VkRenderPassCreateInfo renderPassInfo = {};
	// Type of information stored in the structure
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	// No of attachments in the render pass
	renderPassInfo.attachmentCount = 1;
	// Pointer to the attachment
	renderPassInfo.pAttachments = &colorAttachment;
	// No of subpasses
	renderPassInfo.subpassCount = 1;
	// Pointer to the subpass
	renderPassInfo.pSubpasses = &subpass;
	// No of subpass dependencies
	renderPassInfo.dependencyCount = 1;
	// Pointer to the subpass dependency
	renderPassInfo.pDependencies = &dependency;

	// Create render pass
	// 1st Parameter - GPU
	// 2nd Parameter - Render pass create info
	// 3rd Parameter - Custom Allocator
	// 4th Parameter - Pointer to the created render pass
	if (vkCreateRenderPass(device->logicalDevice, &renderPassInfo, nullptr, &imGUIRenderPass) 
		!= VK_SUCCESS) {
		// Throw runtime error exception as render pass creation failed
		throw std::runtime_error("failed to create ImGUI render pass!");
	}
}

void ImGuiHelper::CreateImGuiFrameBuffer(Device *device, Swapchain *swapChain)
{
	VkImageView attachment[1];
	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.renderPass = imGUIRenderPass;
	info.attachmentCount = 1;
	info.pAttachments = attachment;
	info.width = swapChain->swapChainExtent.width;
	info.height = swapChain->swapChainExtent.height;
	info.layers = 1;
	imGUIFramebuffers.resize(swapChain->swapChainImageViews.size());
	for (uint32_t i = 0; i < swapChain->swapChainImageViews.size(); i++)
	{
		attachment[0] = swapChain->swapChainImageViews[i];
		vkCreateFramebuffer(device->logicalDevice, &info, nullptr, &imGUIFramebuffers[i]);
	}
}

void ImGuiHelper::CreateImGUIDescriptorPool(Device *device)
{
	// Create Descriptor Pool
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	vkCreateDescriptorPool(device->logicalDevice, &pool_info, nullptr, &imGUIDescriptorPool);
}


void ImGuiHelper::CleanupImGUI(Device *device)
{
	// Resources to destroy on swapchain recreation
	for (auto framebuffer : imGUIFramebuffers) {
		vkDestroyFramebuffer(device->logicalDevice, framebuffer, nullptr);
	}

	vkDestroyRenderPass(device->logicalDevice, imGUIRenderPass, nullptr);

	vkFreeCommandBuffers(device->logicalDevice, imGuiCommandPool, static_cast<uint32_t>(imGuiCommandBuffers.size()), imGuiCommandBuffers.data());
	vkDestroyCommandPool(device->logicalDevice, imGuiCommandPool, nullptr);

	// Resources to destroy when the program ends
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	vkDestroyDescriptorPool(device->logicalDevice, imGUIDescriptorPool, nullptr);
}

void ImGuiHelper::DrawImGUI(Device *device, void (*drawFunc)())
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Rendering Biological Iridicence");
	(*drawFunc)();
	ImGui::End();
	ImGui::Render();
}

void ImGuiHelper::RenderImGUI(Device *device, VkExtent2D swapChainExtent,  uint32_t imageIndex)
{

	VkResult err;

	{
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(imGuiCommandBuffers[imageIndex], &info);
		check_vk_result(err);
	}

	{
		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = imGUIRenderPass;
		info.framebuffer = imGUIFramebuffers[imageIndex];
		info.renderArea.extent.width = swapChainExtent.width;
		info.renderArea.extent.height = swapChainExtent.height;
		info.clearValueCount = 1;
		info.pClearValues = &clearColor;
		vkCmdBeginRenderPass(imGuiCommandBuffers[imageIndex], &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	// Record Imgui Draw Data and draw funcs into command buffer
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), imGuiCommandBuffers[imageIndex]);

	// Submit command buffer
	vkCmdEndRenderPass(imGuiCommandBuffers[imageIndex]);
	err = vkEndCommandBuffer(imGuiCommandBuffers[imageIndex]);
	check_vk_result(err);

}

void ImGuiHelper::UpdateMinImageCount()
{
	ImGui_ImplVulkan_SetMinImageCount(imGUI_MinImageCount);
}

void ImGuiHelper::check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}