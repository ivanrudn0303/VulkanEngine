#ifndef VKOBJECT_H
#define VKOBJECT_H

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include "glm/glm.hpp"

class ImageView
{
#ifdef _DEBUG
	bool Ready;
#endif
	VkImageView ObjectId;
	VkDevice Owner;
public:
	ImageView();
int	Init(VkDevice, VkImage*, VkFormat);
	~ImageView();
};

class Image
{
#ifdef _DEBUG
	bool Ready;
#endif
	VkImage ObjectId;
	VkDevice Owner;
	VkImageLayout Layout;
	glm::uvec2 Resolution;
public:
	Image();
uint32_t	Init(VkDevice, glm::uvec2, VkFormat, VkImageUsageFlags);
	~Image();
};
#endif