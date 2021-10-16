#include "TextureImage.h"



TextureImage::TextureImage(Device *device, CommandPool *commandPool, const char* filename)
{
	createTextureImage(device, commandPool, filename);
	createTextureSampler(device);
}


TextureImage::~TextureImage()
{
}

// Function to create texture image
void TextureImage::createTextureImage(Device *device, CommandPool *commandPool, const char* filename) {

	int texWidth, texHeight, texChannels;

	std::ifstream inputFile;
	inputFile.open(filename);
	if (!inputFile.is_open())
	{
		throw std::runtime_error("failed to open texture file!");
	}


	std::string tempString = "";

	//Read the header
	int tempInt = 0;

	inputFile >> tempString;

	// TO DO: Reading Comments

	// Read the height and width of the texture
	//int texWidth, texHeight;
	inputFile >> texWidth >> texHeight;

	// Read max colours
	inputFile >> tempString;
	int size = texWidth * texHeight;

	inputFile.seekg(0, inputFile.end);
	int length = inputFile.tellg();
	inputFile.seekg(0, inputFile.beg);

	inputFile >> tempString;
	inputFile >> tempString;
	inputFile >> tempString;
	inputFile >> tempString;

	unsigned char* fileContents = new unsigned char[length];

	int j = 0;
	inputFile.read((char*)fileContents, length);

	//// Array of texels
	//uint32_t* texels = new uint32_t[size];
	unsigned char* texels = new unsigned char[size * 4];
	unsigned char maxr = 0;
	VkDeviceSize imageSize = texWidth * texHeight * 4;
	int i = 0;
	while (i < size)
	{
		unsigned char r, g, b;

		r = fileContents[i * 3 + 1];
		g = fileContents[i * 3 + 2];
		b = fileContents[i * 3 + 3];

		texels[i * 4] = r;
		texels[i * 4 + 1] = g;
		texels[i * 4 + 2] = b;
		texels[i * 4 + 3] = 255;
		i++;
	}

	inputFile.close();
	if (!texels) {
		throw std::runtime_error("failed to load texture image!");
	}

	Buffer *stagingBuffer = new Buffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer->SetData(device, texels, static_cast<size_t>(imageSize));
	
	textureImage = new Image(device, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT,VK_IMAGE_TYPE_2D);
	
	textureImage->transitionImageLayout(device, commandPool, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	textureImage->copyBufferToImage(device, commandPool,stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	textureImage->transitionImageLayout(device, commandPool, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	stagingBuffer->Cleanup(device);
}

// Function to create texture sampler
void TextureImage::createTextureSampler(Device *device) {

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device->logicalDevice, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void TextureImage::Cleanup(Device *device)
{
	vkDestroySampler(device->logicalDevice, textureSampler, nullptr);
	textureImage->Cleanup(device);
	
}