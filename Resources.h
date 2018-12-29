#ifndef RESOURCES_H
#define RESOURCES_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vector>
#include "Settings.h"
#include "WindowHandler.h"
#include "vulkan/vk_sdk_platform.h"
#include "vulkan/vulkan.h"
#include "ListError.h"
#include "VkObject.h"

struct Resources
{
	VkInstance Instance;
	VkPhysicalDevice PhysicalDevice;
	VkDevice Device;
	VkQueue CommonQueue;
	uint32_t FamilyIndex;
	VkSurfaceKHR Surface;
	VkSwapchainKHR SwapChain;
	std::vector<VkImage> SwapchainImages;
	std::vector<ImageView> SwapchainViews;
	VkCommandPool CommandPool;
	std::vector<VkCommandBuffer> CommandBuffer;
	bool Ready;
	Resources();
	~Resources();
int	init(Settings*, WindowHandler*);
private:
	int DeviceInit();
	int InstanceInit(const WindowHandler*);
	int QueueInit();
	int SurfaceInit(Settings*, WindowHandler*);
	int SwapchainInit(Settings*);
	int SwapImagesInit();
	int CommandPoolInit();
};
#endif // !RESOUCES_H