#include "VkObject.h"


ImageView::ImageView()
{
#ifdef _DEBUG
	Ready = false;
#endif // _DEBUG
}

int ImageView::Init(VkDevice dev, VkImage *img, VkFormat form)
{
#ifdef _DEBUG
	if (Ready)
		this->~ImageView();
#endif
	Owner = dev;
	VkImageViewCreateInfo inf = {};
	inf.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	inf.format = form;
	inf.viewType = VK_IMAGE_VIEW_TYPE_2D;
	inf.subresourceRange.baseArrayLayer = 0;
	inf.subresourceRange.baseMipLevel = 0;
	inf.subresourceRange.levelCount = 1;
	inf.subresourceRange.layerCount = 1;
	inf.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	inf.image = *img;
#ifdef _DEBUG
	int res = vkCreateImageView(Owner, &inf, nullptr, &ObjectId);
	if (res)
		return res;
	Ready = true;
	return 0;
#else
	return vkCreateImageView(Owner, &inf, nullptr, &ObjectId);
#endif
}

ImageView::~ImageView()
{
#ifdef _DEBUG
	if (Ready)
#endif
	vkDestroyImageView(Owner, ObjectId, nullptr);
}

Image::Image()
{
#ifdef _DEBUG
	Ready = false;
#endif // _DEBUG
}

uint32_t Image::Init(VkDevice dev, glm::uvec2 res, VkFormat form, VkImageUsageFlags usg)
{
	Owner = dev;
	VkImageCreateInfo inf = {};
	inf.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	inf.pNext = nullptr;
	inf.format = form;
	inf.tiling = VK_IMAGE_TILING_OPTIMAL;
	inf.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	inf.usage = usg;
	inf.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	inf.samples = VK_SAMPLE_COUNT_1_BIT;
	inf.flags = 0;
	Layout = VK_IMAGE_LAYOUT_UNDEFINED;
	inf.extent = { res.x, res.y, 1 };
	Resolution = res;
	inf.imageType = VK_IMAGE_TYPE_2D;
	inf.queueFamilyIndexCount = 0;
	inf.pQueueFamilyIndices = nullptr;
	inf.arrayLayers = 1;
	inf.mipLevels = 1;
	VkResult result = vkCreateImage(Owner, &inf, nullptr, &ObjectId);
	if (result)
		return 0;
	VkMemoryRequirements mem;
	vkGetImageMemoryRequirements(Owner, ObjectId, &mem);
#ifdef _DEBUG
	Ready = true;
#endif // _DEBUG
	return uint32_t(mem.size + (mem.size % mem.alignment));
}

Image::~Image()
{
#ifdef _DEBUG
	if (Ready)
#endif
		vkDestroyImage(Owner, ObjectId, nullptr);
}
