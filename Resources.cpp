#include "Resources.h"
#include <vector>
#include <cinttypes>
#include <string>


uint32_t FindQueueFamily(const VkPhysicalDevice*, int);
int FindProperGpu(VkInstance*, VkPhysicalDevice*);

Resources::Resources():Ready(false)
{}

Resources::~Resources()
{
	if (!Ready)
		return;
	SwapchainViews.clear();
	vkDestroySwapchainKHR(Device, SwapChain, nullptr);
	vkDestroySurfaceKHR(Instance, Surface, nullptr);
	vkFreeCommandBuffers(Device, CommandPool, CommandBuffer.size(), CommandBuffer.data());
	vkDestroyCommandPool(Device, CommandPool, nullptr);
	vkDestroyDevice(Device, nullptr);
	vkDestroyInstance(Instance, nullptr);
}

int Resources::init(Settings *Sett, WindowHandler* Wnd)
{
	int res;
	if (res = InstanceInit(Wnd))
		return res;
	if (res = DeviceInit())
		return res;
	QueueInit();
	if (res = SurfaceInit(Sett, Wnd))
		return res;
	if (res = SwapchainInit(Sett))
		return res;
	if (res = SwapImagesInit())
		return res;
	if (res = CommandPoolInit())
		return res;
	Ready = true;






	uint32_t siz;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &siz, nullptr);
	std::vector< VkQueueFamilyProperties> Vec(siz);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &siz, Vec.data());
	VkBool32 sup = vkGetPhysicalDeviceWin32PresentationSupportKHR(PhysicalDevice, FamilyIndex);
	sup = false;
	res = vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, FamilyIndex, Surface, &sup);
	Image img;
	img.Init(Device, { 1025, 767 }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT| VK_IMAGE_USAGE_TRANSFER_DST_BIT);

	sup = true;
	res = vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, FamilyIndex, Surface, &sup);

	VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
	cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufAllocInfo.commandPool = CommandPool;
	cmdBufAllocInfo.commandBufferCount = 1;
	cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBuffer.resize(1);
	res = vkAllocateCommandBuffers(Device, &cmdBufAllocInfo, CommandBuffer.data());
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	VkClearColorValue clearColor = { 128.0f / 256.0f, 128.0f / 256.0f, 128.0f / 256.0f, 1.0f };
	VkClearValue clearValue = {};
	clearValue.color = clearColor;
	res = vkBeginCommandBuffer(CommandBuffer[0], &beginInfo);
	VkImageSubresourceRange imageRange = {};
	imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageRange.levelCount = 1;
	imageRange.layerCount = 1;
	vkCmdClearColorImage(CommandBuffer[0], SwapchainImages[0], VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imageRange);
	res = vkEndCommandBuffer(CommandBuffer[0]);
	uint32_t i = 0;
	res = vkAcquireNextImageKHR(Device, SwapChain, UINT64_MAX, NULL, NULL, &i);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = CommandBuffer.data();
	vkQueueSubmit(CommonQueue, 1, &submitInfo, NULL);
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &SwapChain;
	presentInfo.pImageIndices = &i;
	VkResult r;
	presentInfo.pResults = &r;

	res = vkQueuePresentKHR(CommonQueue, &presentInfo);


	return 0;
}

int Resources::InstanceInit(const WindowHandler* wnd)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = wnd->GetName().c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 1, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = 2;
	const char* ext[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
	createInfo.ppEnabledExtensionNames = ext;
#ifdef _DEBUG
	createInfo.enabledLayerCount = 1;
	const char *Layer[] = { "VK_LAYER_LUNARG_api_dump" };
	createInfo.ppEnabledLayerNames = Layer;
#else
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
#endif // _DEBUG
	return vkCreateInstance(&createInfo, nullptr, &Instance);
}

int Resources::QueueInit()
{
	vkGetDeviceQueue(Device, FamilyIndex, 0, &CommonQueue);
	return 0;
}

int Resources::SurfaceInit(Settings* Sc, WindowHandler* Wnd)
{
	
	VkWin32SurfaceCreateInfoKHR inf = {};
	inf.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	inf.hinstance = Wnd->GetHinstance();
	inf.hwnd = Wnd->GetHwnd();
	return vkCreateWin32SurfaceKHR(Instance, &inf, nullptr, &Surface);
}

int Resources::SwapchainInit(Settings* sett)
{
	VkSwapchainCreateInfoKHR inf = {};
	inf.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	inf.surface = Surface;
	inf.queueFamilyIndexCount = 0;
	VkSurfaceCapabilitiesKHR details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &details);
	inf.imageExtent = details.currentExtent;
	inf.clipped = VK_TRUE;
	inf.oldSwapchain = VK_NULL_HANDLE;
	inf.minImageCount = 2;
	inf.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
	inf.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	inf.imageArrayLayers = 1;
	inf.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	inf.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	inf.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	inf.preTransform  = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	inf.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	return vkCreateSwapchainKHR(Device, &inf, nullptr, &SwapChain);
}

int Resources::SwapImagesInit()
{
	uint32_t size;
	int res = vkGetSwapchainImagesKHR(Device, SwapChain, &size, nullptr);
	if (res)
		return res;
	SwapchainImages.resize(size);
	res = vkGetSwapchainImagesKHR(Device, SwapChain, &size, SwapchainImages.data());
	if (res)
		return res;
	SwapchainViews.resize(SwapchainImages.size());
	for (uint32_t i = 0; i < SwapchainImages.size(); ++i)
		(SwapchainViews[i]).Init(Device, &SwapchainImages[i], VK_FORMAT_R8G8B8A8_SRGB);
	return 0;
}

int Resources::CommandPoolInit()
{
	VkCommandPoolCreateInfo inf = {};
	inf.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	inf.pNext = nullptr;
	inf.queueFamilyIndex = FamilyIndex;
	return vkCreateCommandPool(Device, &inf, nullptr, &CommandPool);
}

#define QUEUE_STRUCT_INIT(strc, index) strc.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;\
strc.pNext = nullptr;\
strc.flags = 0;\
strc.queueFamilyIndex = index;\
strc.queueCount = 1;\
strc.pQueuePriorities = array_priority;


int Resources::DeviceInit()
{
	
	if (FindProperGpu(&Instance, &PhysicalDevice))
		return WRONG_NO_GPU;
	float array_priority[] = { 1.0 };
	uint32_t unique = 1;
	VkDeviceQueueCreateInfo Queues[3] = {};
	uint32_t fnd = FindQueueFamily(&PhysicalDevice, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT);
	if (fnd == MAXUINT32)
		return WRONG_GPU_PROPERTIES;
	QUEUE_STRUCT_INIT(Queues[0], fnd);
	FamilyIndex = fnd;
	VkDeviceCreateInfo InfDevice = {};
	InfDevice.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	InfDevice.queueCreateInfoCount = unique;
	InfDevice.pQueueCreateInfos = Queues;
	InfDevice.enabledExtensionCount = 1;
	const char *Extentions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
//	uint32_t size;
//	std::vector<VkExtensionProperties> s;
//	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &size, nullptr);
//	s.resize(size);
//	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &size, s.data());
	InfDevice.ppEnabledExtensionNames = Extentions;
	return vkCreateDevice(PhysicalDevice, &InfDevice, nullptr, &Device);
}

uint32_t FindQueueFamily(const VkPhysicalDevice* PhysDev, int flag)
{
	uint32_t QueueFamilycount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(*PhysDev, &QueueFamilycount, nullptr);
	std::vector <VkQueueFamilyProperties> QueueFamilies(QueueFamilycount);
	vkGetPhysicalDeviceQueueFamilyProperties(*PhysDev, &QueueFamilycount, QueueFamilies.data());
	uint32_t max = MAXUINT32;
	uint32_t ret = max;
	for(uint32_t i = 0; i < QueueFamilycount; ++i)
		if (((QueueFamilies[i].queueFlags & flag)== flag) && (QueueFamilies[i].queueFlags < max))
		{
			max = QueueFamilies[i].queueFlags;
			ret = i;
		}
	return ret;
}

int FindProperGpu(VkInstance* inst, VkPhysicalDevice* dest)
{
	uint32_t gpu_count = 0;
	vkEnumeratePhysicalDevices(*inst, &gpu_count, nullptr);
	std::vector<VkPhysicalDevice> Phys(gpu_count);
	vkEnumeratePhysicalDevices(*inst, &gpu_count, Phys.data());

	VkPhysicalDeviceProperties prop;
	for (uint32_t i = 0; i < gpu_count; ++i)
	{
		vkGetPhysicalDeviceProperties(Phys[i], &prop);
		if (prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			*dest = Phys[i];
			return 0;
		}
	}
	for (uint32_t i = 0; i < gpu_count; ++i)
	{
		vkGetPhysicalDeviceProperties(Phys[i], &prop);
		if (prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			*dest = Phys[i];
			return 0;
		}
	}
	return WRONG_NO_GPU;
}