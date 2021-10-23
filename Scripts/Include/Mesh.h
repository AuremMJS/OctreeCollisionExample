#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "vertex.h"
#include "Buffer.h"
#include "Pipeline.h"
#include "TextureImage.h"
#include "Window.h"
#include "CollisionEngine\Collider.h"
#include <vector>

// Uniforms for model, view, projection transformations
struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

// Uniform for Lighting Constants
struct LightingConstants
{
	Vec4 lightPosition;
	Vec4 lightAmbient;
	Vec4 lightDiffuse;
	Vec4 lightSpecular;
	float ambientIntensity;
	float specularIntensity;
	float diffuseIntensity;
	float lightSpecularExponent;
	float ambientEnabled = 1;
	float specularEnabled = 1;
	float DiffuseEnabled = 1;
	float textureEnabled = 1;
	float transparency;
	float useNormalMap;
	float useOpacityMap;
};

class Mesh
{
private:
	bool isStatic;
	Vec3 position;
	Device *device;
	Collider* collider;
	CommandPool *commandPool;
	// Vertices of the mesh
	std::vector<Vertex> aabbVertices;

	// Indices of the faces of the mesh
	std::vector<int> aabbIndices;

	std::vector<VkDescriptorSet> CreateObjectDescriptorSets(VkDescriptorSetLayout descriptorSetLayout,
		VkDescriptorPool descriptorPool,
		std::vector<Buffer*>uniformBuffers, std::vector<Buffer*> lightingBuffers,
		TextureImage *opacityTexture);

public:
	// Vertices of the mesh
	std::vector<Vertex> vertices;

	// Indices of the faces of the mesh
	std::vector<int> indices;

	// Lighting Constants of the mesh
	LightingConstants lightingConstants;

	// Vertex Buffer
	Buffer *VertexBuffer;

	// Instance Buffer to generate many copies of the mesh
	Buffer *InstanceBuffer;

	// Index Buffer
	Buffer *IndexBuffer;

	// Index Buffer
	Buffer *AABBIndexBuffer;

	// Vertex Buffer
	Buffer *AABBVertexBuffer;


	// Texture Image
	TextureImage *opacityImage;

	TextureImage *aabbOpacityImage;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;

	// Descriptor sets to bind each VkBuffer to the uniform buffer descriptor
	std::vector<VkDescriptorSet> descriptorSets;

	// Descriptor sets to bind each VkBuffer to the uniform buffer descriptor
	std::vector<VkDescriptorSet> aabbDescriptorSets;

	// Uniform Buffers
	std::vector<Buffer*> uniformBuffers;

	// Lighting Buffers
	std::vector<Buffer*> lightingBuffers;

	// Uniform Buffers
	std::vector<Buffer*> aabbUniformBuffers;

	// Lighting Buffers
	std::vector<Buffer*> aabbLightingBuffers;

	int swapChainCount;

	Mesh();
	Mesh(const char* filename, Vec3 position, Device *device, CommandPool *commandPool,int swapChainCount);
	~Mesh();
	// Function to parse a obj file
	void ParseObjFile(const char* filename);

	// Function to construct AABB mesh
	void ConstructAABBMesh();

	// Function to load properties from a MTL file
	void LoadMaterial(const char* filename);

	// Function to create index buffer
	void createIndexBuffer();

	// Function to create vertex buffer
	void createVertexBuffer();

	void createAABBIndexBuffer();

	void createAABBVertexBuffer();

	void createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout,
		VkDescriptorPool descriptorPool);

	void createDescriptorSetLayout();

	void createUniformBuffers();

	void Draw(VkCommandBuffer commandBuffer, Pipeline graphicsPipeline, int currentImage);

	void DrawAABB(VkCommandBuffer commandBuffer, Pipeline graphicsPipeline, int currentImage);

	void updateUniformBuffer(uint32_t currentImage, Window window, Swapchain *swapChain);

	void updateLightingConstants(uint32_t currentImage, Window window, Swapchain *swapChain);

	void Cleanup();

	void CleanupUniformBuffers();

	void SetStatic(bool isStatic);
};