#pragma once
#include "Image.h"
#include "Buffer.h"
class TextureImage
{
public:
	Image *textureImage;
	VkSampler textureSampler;
	TextureImage(Device *device, CommandPool *commandPool, const char* filename);
	~TextureImage();

	void createTextureImage(Device *device, CommandPool *commandPool, const char* filename);
	void createTextureSampler(Device *device);
	void Cleanup(Device *device);
};

