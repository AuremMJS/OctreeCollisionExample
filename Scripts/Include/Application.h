#pragma once
#define _CRT_SECURE_NO_WARNINGS

// Include Files
#include "Pipeline.h"
#include "Window.h"
#include "UI_Design.h"
#include "Constants.h"
#include "Swapchain.h"
#include "Buffer.h"
#include "CommandPool.h"
#include "TextureImage.h"
#include "ImGuiHelper.h"
#include "ImGUI\imgui.h"
#include "MathHelper.h"
#include "BindingAttributeDescriptionHelper.h"
#include "CollisionEngine\Collider.h"
#include <map>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include "Mesh.h"

// Maximum no of frames processed concurrently
const int MAX_FRAMES_IN_FLIGHT = 2;

struct DescriptorSetLayoutInfo
{
	uint32_t Binding;
	VkDescriptorType Type;
	VkShaderStageFlagBits Stage;
};

// Spectral Paramters
struct SpectralPipelineParameters
{
	Vec3 FourierCoefficients;
	int textureWidth;
	int textureHeight;
	float maxSpectralValue;
	float minWavelength;
	float maxWavelength;
	int noOfSpectralValues;
	float dominantWavelength;
	float saturation;
	float gaussianMinWidth;
	float gaussianMaxWidth;
	float hybridThreshold;
	float filmDensity;
	float airDensity;
	float filmIOR;
	float airIOR;
	float interferencePower;
	float interferenceConstant;
};


// Iridescent Colours for different angle of incidence
struct IridescentColors
{
	Vec4 colors[90];
};

// Pixel data of the image generated in the compute shader
struct Pixel {
	float r, g, b, a;
};

class Application
{
private:

	// Mesh loaded
	Mesh ball;

	Mesh ball2;

	Window window;

	// Vulkan Instance - Connection between the application and the Vulkan Library
	VkInstance instance;

	// Handle to manage the Debug Callback
	VkDebugUtilsMessengerEXT debugMessenger;

	// Device
	Device *device;

	// Handle to Swap chain
	Swapchain *swapChain;

	// Handle to ImGUI elements
	ImGuiHelper *imGui;
	
	// Render pass
	VkRenderPass renderPass;

	// Descriptor Set Layout
	VkDescriptorSetLayout descriptorSetLayout;

	// Pipeline layout - uniforms
	VkPipelineLayout pipelineLayout;

	// Graphics pipeline
	//VkPipeline graphicsPipeline;
	Pipeline graphicsPipeline;
	
	// Swap chain frame buffers
	std::vector<VkFramebuffer> swapChainFramebuffers;

	// Command Pool
	CommandPool *commandPool;

	// Descriptor Pool to create descriptor sets
	VkDescriptorPool descriptorPool;

	// Depth Image
	Image *depthImage;
	
	// Command Buffers
	std::vector<VkCommandBuffer> commandBuffers;

	// Semaphores to signal image is acquired for rendering
	std::vector<VkSemaphore> imageAvailableSemaphores;

	// Semaphores to signal rendering is complete and presentation can start
	std::vector<VkSemaphore> renderFinishedSemaphores;

	// Fences to prevent from more than Max frames getting submitted
	std::vector<VkFence> inFlightFences;

	// Fences to prevent images in flight getting rendered
	std::vector<VkFence> imagesInFlight;

	// Index of current frame
	size_t currentFrame = 0;

	// Flag to indicate whether frame buffer is resized due to window resizing
	bool framebufferResized = false;


public:
	
	// Constructor
	Application();
	
	// Destructor
	~Application();

	// run functions
	void run();

	// Function to initialize the Vulkan API
	void initVulkan();

	// Main Loop functions that is run in every frame
	void mainLoop();

	// Cleanup function to destroy all elements
	void cleanup();
	
	// Function to create Vulkan Instance
	void createInstance();

	// Function to setup debug messenger
	void setupDebugMessenger();

	// To create render pass
	void createRenderPass();

	// To create main graphics pipeline
	void createGraphicsPipeline();

	// Creating frame buffers
	void createFramebuffers();

	// Function to create depth resources
	void createDepthResources();

	// Function to find the format of depth image
	VkFormat findDepthFormat();

	// Function to find the format supported for a config
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	// Functions to create descriptor sets
	VkDescriptorPool createDescriptorPool(int descriptorCount, std::initializer_list<VkDescriptorType> descriptorTypes);
	void createDescriptorSetLayout();

	VkDescriptorSetLayout CreateDescriptorSetLayout(std::vector<DescriptorSetLayoutInfo> descriptorSetLayouts);

	// Function to create command buffers
	void createCommandBuffers();

	// Function to create synchronisation objects
	void createSyncObjects();

	// Function to check whether validation layers are supported
	bool checkValidationLayerSupport();

	// Function to fetch the extensions
	std::vector<const char*> getRequiredExtensions();

	// Function to populate the debug messenger info
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	// Callback function for debug message
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	// Function to submit a command buffer to queue
	void drawFrame();

	// Function to recreate the swap chain and related elements
	void recreateSwapChain();

	// Function to destroy swap chain and related elements
	void cleanupSwapChain();

	// Function to create a compute pipeline
	void createComputePipeline();

	//function to create descriptor sets for compute pipeline to  provide the details about descriptor bindings in every shader
	void createComputeDescriptorSet();

	// function to create descriptor set layout for compute pipeline to  provide the details about descriptor bindings in every shader
	void createComputeDescriptorSetLayout();

	// Functions to create compute command buffers
	void createComputeCommandPool();
	void createComputeCommandBuffers();

	// Function to create buffers used in compute pipeline
	void createComputeBuffers();

	// Function to run the compute command buffer
	void runComputeCommandBuffer();

	// Function to fetch the iridescent colours from the generated iridescent spectra
	void FetchIridescentColoursFromSpectra();

	// Function to calculate the fourier matrix
	void FindFourierMatrix();

	// Function to read the colour matching functions file
	void ReadColorMatchingXMLFile(const char* filename);

	// Function to compute fourier coefficients
	void FindFourierCoefficients(Vec3 colorInXYZ);

	// Function to get the XYZ colour from a spectra
	Vec3 GetXYZColorFromSpectra(std::vector<double> spectra);

	// Function to read the chromaticity coordinates file
	void ReadChromaticityXMLFile(const char* filename);

	// Conversion functions
	Vec3 ConvertRGBtoXYZ(Vec3 colorInRGB);
	Vec3 ConvertXYZtoRGB(Vec3 colorInXYZ);

	// Function to calculate saturation and dominant wavelength
	Vec3 CalculateSaturationAndDominantWavelength(Vec3 colorInXYZ);

	// Function to get the hue value
	double getHueFromXYZ(Vec3 colorInXYZ);

	// Function to update the spectral parameters
	void UpdateSpectralParameters();

	// Function to init the quad 
	void InitQuad();

	// Function to create the quad pipeline
	void createQuadGraphicsPipeline();

	// Function to create quad descriptor sets for each Vk Buffer
	void createQuadDescriptorSets();

	// Function to create quad Index Buffer
	void createQuadIndexBuffer();

	// Function to create quad Vertex Buffer
	void createQuadVertexBuffer();
	
	// function to create quad descriptor set layout to  provide the details about descriptor bindings in every shader
	void createQuadDescriptorSetLayout();

	void OnWindowResized();
};

