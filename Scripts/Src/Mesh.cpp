#include "Mesh.h"
#include "CollisionEngine\Collider.h"
#include "UI_Design.h"
#include <fstream>
#include <string>

std::vector<VkDescriptorSet> Mesh::CreateObjectDescriptorSets(VkDescriptorSetLayout descriptorSetLayout,
	VkDescriptorPool descriptorPool,
	std::vector<Buffer*>  uniformBuffers, std::vector<Buffer*>  lightingBuffers,
	TextureImage * opacityTexture)
{
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSetLayout> layouts(swapChainCount,
		descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainCount);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(swapChainCount);
	if (vkAllocateDescriptorSets(device->logicalDevice, &allocInfo, descriptorSets.data())
		!= VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < swapChainCount; i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i]->buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorBufferInfo lightingBufferInfo = {};
		lightingBufferInfo.buffer = lightingBuffers[i]->buffer;
		lightingBufferInfo.offset = 0;
		lightingBufferInfo.range = sizeof(LightingConstants);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = opacityTexture->textureImage->imageView;
		imageInfo.sampler = opacityTexture->textureSampler;

		std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &lightingBufferInfo;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = descriptorSets[i];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device->logicalDevice, static_cast<uint32_t>
			(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
	return descriptorSets;
}
Mesh::Mesh()
{

}

Mesh::Mesh(const char * filename, Vec3 position, Device *device,CommandPool *commandPool,int swapChainCount)
{
	this->device = device;
	this->commandPool = commandPool;
	this->swapChainCount = swapChainCount;
	this->position = position;
	this->isStatic = false;
	ParseObjFile(filename);
	ConstructAABBMesh();

	collider->Translate(position * -1);

	// Create Vertex Buffer
	createVertexBuffer();

	// Create the instance buffer
	InstanceBuffer = new Buffer(device, pow(10, 6) * sizeof(InstanceData), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	// Create Index Buffer
	createIndexBuffer();

	createAABBVertexBuffer();

	createAABBIndexBuffer();

	// Create texture image for opacity map
	opacityImage = new TextureImage(device, commandPool, OPACITY_MAP);

	aabbOpacityImage = new TextureImage(device, commandPool, OPACITY_MAP);

	// Create the Uniform Buffers
	createUniformBuffers();

}

Mesh::~Mesh()
{
}

void Mesh::ParseObjFile(const char * filename)
{
	std::vector<Vec3> positions;
	std::vector<Vec3> normals;
	std::vector<Vec3> texCoords;
	std::ifstream inputFile;
	inputFile.open(filename);
	if (!inputFile.is_open())
	{
		throw std::runtime_error("failed to open obj file!");
	}


	std::string tempString = "";
	int i;
	while (true)
	{
		inputFile >> tempString;
		if (tempString == "mtllib")
		{
			std::string materialFilename;
			inputFile >> materialFilename;
			LoadMaterial(materialFilename.c_str());
		}
		if (tempString == "v")
		{
			float x, y, z;
			inputFile >> x >> y >> z;
			Vec3 v;
			v.x = x;
			v.y = y;
			v.z = z;

			positions.push_back(v);
		}
		else if (tempString == "vn")
		{
			float x, y, z;
			inputFile >> x >> y >> z;
			Vec3 v;
			v.x = x;
			v.y = y;
			v.z = z;

			normals.push_back(v);
		}
		else if (tempString == "vt")
		{
			float x, y, z;
			inputFile >> x >> y >> z;
			Vec3 v;
			v.x = x;
			v.y = 1 - y;
			v.z = z;
			texCoords.push_back(v);
		}

		else if (tempString == "f")
		{
			for (int j = 0; j < 3; j++)
			{
				int position_index, tex_index, normal_index;
				inputFile >> tempString;
				std::string tstring = tempString;
				position_index = atoi(strtok(&tempString[0], "/"));
				tex_index = atoi(strtok(NULL, "/"));
				normal_index = atoi(strtok(NULL, "/"));
				Vertex v;
				v.position = positions[position_index - 1];
				Vec3 color;
				color.x = 1.0f;
				color.y = 1.0f;
				color.z = 1.0f;

				v.color = color;
				v.tex = texCoords[tex_index - 1];
				v.normal = normals[normal_index - 1];

				vertices.push_back(v);
			}
			int vertex_index = vertices.size();
			indices.push_back(vertex_index - 3);
			indices.push_back(vertex_index - 2);
			indices.push_back(vertex_index - 1);

		}
		else if (inputFile.eof())
			break;
	}
}

void Mesh::ConstructAABBMesh()
{
	static int count;
	std::vector<Vec3> positions;
	for (auto vertex : vertices)
	{
		positions.push_back(vertex.position);
	}
	collider = new Collider("Object" +count,positions,6);

	AxisAlignedBoundingBox aabb = *collider->GetAABB();

	std::vector<Vec3> aabbPositions = aabb.GetOctreeVertices();//  aabb.GetVertices();
	std::vector<Vec3> aabbTexCoords = aabb.GetTexCoords();

	//for (auto index : aabb.GetIndices())
	for (auto index : aabb.GetOctreeIndices())
	{
		Vertex v;
		v.position = aabbPositions[index.positionIndex];
		v.tex = aabbTexCoords[index.texCoordIndex];
		aabbVertices.push_back(v);
		int vertex_index = aabbVertices.size();
		aabbIndices.push_back(vertex_index - 1);
	}
	count++;
}

void Mesh::LoadMaterial(const char * filename)
{
	std::ifstream inputFile;
	inputFile.open(filename);
	if (inputFile.is_open())
	{
		std::string tempString = "";
		while (true)
		{
			inputFile >> tempString;
			if (tempString == "Ns")
			{
				//inputFile >> lightingConstants.lightSpecularExponent;
			}
			else if (tempString == "Ka")
			{
				float r, g, b;
				inputFile >> r >> g >> b;
				//lightingConstants.lightAmbient = Vec4{ r, g, b, 1.0 };
			}
			else if (tempString == "Ks")
			{
				float r, g, b;
				inputFile >> r >> g >> b;
				//lightingConstants.lightSpecular = Vec4{ r, g, b, 1.0 };
			}
			else if (tempString == "Kd")
			{
				float r, g, b;
				inputFile >> r >> g >> b;
				//lightingConstants.lightDiffuse = Vec4{ r, g, b, 1.0 };
			}
			else if (inputFile.eof())
				break;
		}
	}
	// Set the intensities of the light
	//lightingConstants.ambientIntensity = 0;
	//lightingConstants.specularIntensity = 0;
	//lightingConstants.diffuseIntensity = 0.3;

	// Set the position of the light
	//lightingConstants.lightPosition = Vec4{ 0.0f, -200.0f, 260.0f, 1.0f };
}


// Function to create Index Buffer
void Mesh::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	IndexBuffer = new Buffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	IndexBuffer->SetDataUsingStageBuffer(device, indices.data(), bufferSize, commandPool);
}

// Function to create Vertex Buffer
void Mesh::createVertexBuffer() {

	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VertexBuffer = new Buffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VertexBuffer->SetDataUsingStageBuffer(device, vertices.data(), bufferSize, commandPool);
}

// Function to create Index Buffer
void Mesh::createAABBIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(aabbIndices[0]) * aabbIndices.size();

	AABBIndexBuffer = new Buffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	AABBIndexBuffer->SetDataUsingStageBuffer(device, aabbIndices.data(), bufferSize, commandPool);
}

// Function to create Vertex Buffer
void Mesh::createAABBVertexBuffer() {

	VkDeviceSize bufferSize = sizeof(aabbVertices[0]) * aabbVertices.size();

	AABBVertexBuffer = new Buffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	AABBVertexBuffer->SetDataUsingStageBuffer(device, aabbVertices.data(), bufferSize, commandPool);
}

void Mesh::Draw(VkCommandBuffer commandBuffer, Pipeline graphicsPipeline, int currentImage)
{
	VkBuffer vertexBuffers[] = { VertexBuffer->buffer };
	VkBuffer instanceBuffers[] = { InstanceBuffer->buffer };
	VkDeviceSize offsets[] = { 0 };

	// Bind Vertex Buffer
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	// Bind Instance Buffer
	vkCmdBindVertexBuffers(commandBuffer, 1, 1, instanceBuffers, offsets);

	// Bind Index Buffer
	vkCmdBindIndexBuffer(commandBuffer, IndexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);

	// Bind descriptor sets
	graphicsPipeline.BindDescriptorSets(commandBuffer, descriptorSets[currentImage]);

	// Draw the meshes
	vkCmdDrawIndexed(commandBuffer,
		static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void Mesh::DrawAABB(VkCommandBuffer commandBuffer, Pipeline graphicsPipeline, int currentImage)
{
	VkBuffer vertexBuffers[] = { AABBVertexBuffer->buffer };
	VkDeviceSize offsets[] = { 0 };

	// Bind Vertex Buffer
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	// Bind Index Buffer
	vkCmdBindIndexBuffer(commandBuffer, AABBIndexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);

	// Bind descriptor sets
	graphicsPipeline.BindDescriptorSets(commandBuffer, aabbDescriptorSets[currentImage]);

	// Draw the meshes
	vkCmdDrawIndexed(commandBuffer,
		static_cast<uint32_t>(aabbIndices.size()), 1,
		0, 0, 0);

}

void Mesh::Cleanup()
{
	// Destroy the index buffer
	IndexBuffer->Cleanup(device);

	// Destroy the vertex buffer
	VertexBuffer->Cleanup(device);

	// Destroy instance buffer
	InstanceBuffer->Cleanup(device);

	AABBIndexBuffer->Cleanup(device);

	AABBVertexBuffer->Cleanup(device);

	// Destroy the texture images
	opacityImage->Cleanup(device);

	aabbOpacityImage->Cleanup(device);
}

void Mesh::CleanupUniformBuffers()
{
	// Cleanup the uniform buffer and lighting constants buffer
	for (size_t i = 0; i < swapChainCount; i++) {

		uniformBuffers[i]->Cleanup(device);

		lightingBuffers[i]->Cleanup(device);

		aabbUniformBuffers[i]->Cleanup(device);

		aabbLightingBuffers[i]->Cleanup(device);
	}
}

void Mesh::SetStatic(bool isStatic)
{
	this->isStatic = isStatic;
}

// Function to create descriptor sets for each Vk Buffer
void Mesh::createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout,
	VkDescriptorPool descriptorPool) {
	
	descriptorSets = CreateObjectDescriptorSets(descriptorSetLayout, descriptorPool,
		uniformBuffers, lightingBuffers, opacityImage);

	aabbDescriptorSets = CreateObjectDescriptorSets(descriptorSetLayout, descriptorPool,
		aabbUniformBuffers, aabbLightingBuffers, aabbOpacityImage);
}

// Function to update uniform buffer values
void Mesh::updateUniformBuffer(uint32_t currentImage, Window window, Swapchain *swapChain) {

	UniformBufferObject ubo = {};

	if (!isStatic && window.ShouldUpdate())
	{
		glm::vec3 translateVec = window.GetTranslateValues();
		position = position - Vec3(translateVec.x, translateVec.y, translateVec.z);
		collider->Translate(Vec3(translateVec.x, translateVec.y, translateVec.z));
		ubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(UIDesign::uiParams.scale)) *
			glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, position.z)) *
			window.GetRotationMatrix();
	}
	else
	{ 
	ubo.model = glm::scale(glm::mat4(1.0f),glm::vec3(UIDesign::uiParams.scale)) *
			glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, position.z)) *
			window.GetRotationMatrix();
	}
	ubo.view = glm::lookAt(glm::vec3(0.0f, 2.0f, 100.0f), 
		 glm::vec3(0.0f, 0.0f, 40.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.proj = glm::perspective(glm::radians(45.0f),
		swapChain->swapChainExtent.width / (float)swapChain->swapChainExtent.height, 0.1f, 1000.0f);
	ubo.proj[1][1] *= -1;

	uniformBuffers[currentImage]->SetData(device, &ubo, sizeof(ubo));
	//ubo.model = ubo.model * glm::inverse(window.GetRotationMatrix());
	aabbUniformBuffers[currentImage]->SetData(device, &ubo, sizeof(ubo));
}

// Function to update lighting constant values
void Mesh::updateLightingConstants(uint32_t currentImage, Window window, Swapchain *swapChain) {
	LightingConstants lightConstants = lightingConstants;

	Matrix4 rot = window.GetLightRotationMatrix();
	//lightConstants.lightPosition = rot * Vec4{ 0.0, 10.0, 100.0, 1.0 };// glm::vec4(85.0f, 2.0f, 100.0f, 1.0)* rot;//;

	lightConstants.isCollided = collider->IsCollidedWithAny();
	lightConstants.useOpacityMap = false;
	lightConstants.showAABB = UIDesign::uiParams.renderAABB;
	lightingBuffers[currentImage]->SetData(device, &lightConstants, sizeof(lightConstants));

	lightConstants.useOpacityMap = true;
	aabbLightingBuffers[currentImage]->SetData(device, &lightConstants, sizeof(lightConstants));
}

// Function to create Uniform Buffers
void Mesh::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(swapChainCount);
	aabbUniformBuffers.resize(swapChainCount);

	VkDeviceSize lightingBufferSize = sizeof(LightingConstants);

	lightingBuffers.resize(swapChainCount);
	aabbLightingBuffers.resize(swapChainCount);

	for (size_t i = 0; i < swapChainCount; i++) {
		uniformBuffers[i] = new Buffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		lightingBuffers[i] = new Buffer(device, lightingBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		aabbUniformBuffers[i] = new Buffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		aabbLightingBuffers[i] = new Buffer(device, lightingBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}