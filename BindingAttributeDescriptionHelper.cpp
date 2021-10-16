#include "BindingAttributeDescriptionHelper.h"

// Get Binding Description for the vertex
std::vector<VkVertexInputBindingDescription> BindingAttributeDescriptionHelper::getBindingDescription() {
	std::vector<VkVertexInputBindingDescription> bindingDescription = {};

	bindingDescription.resize(2);

	bindingDescription[0].binding = 0;
	bindingDescription[0].stride = sizeof(Vertex);
	bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	bindingDescription[1].binding = 1;
	bindingDescription[1].stride = sizeof(Vec3);
	bindingDescription[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	return bindingDescription;
}

// Get the attribute descriptions
std::vector<VkVertexInputAttributeDescription> BindingAttributeDescriptionHelper::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	attributeDescriptions.resize(7);
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, position);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, tex);

	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(Vertex, normal);

	attributeDescriptions[4].binding = 1;
	attributeDescriptions[4].location = 4;
	attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[4].offset = offsetof(InstanceData, instancePosition);

	attributeDescriptions[5].binding = 1;
	attributeDescriptions[5].location = 5;
	attributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[5].offset = offsetof(InstanceData, rotation);

	attributeDescriptions[6].binding = 1;
	attributeDescriptions[6].location = 6;
	attributeDescriptions[6].format = VK_FORMAT_R32_SFLOAT;
	attributeDescriptions[6].offset = offsetof(InstanceData, scale);
	return attributeDescriptions;
}