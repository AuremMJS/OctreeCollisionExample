#pragma once

#include "Vertex.h"
#include "Device.h"

static class BindingAttributeDescriptionHelper
{
public:
	static std::vector<VkVertexInputBindingDescription> getBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};