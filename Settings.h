#ifndef SETTINGS_H
#define SETTINGS_H

#define VK_USE_PLATFORM_WIN32_KHR
#include "glm/glm.hpp"
#include <vulkan/vulkan.h>

struct Settings
{
	glm::uvec2 Resolution;
	float Filtering;
	bool PostProcessing;
	Settings(const char*);
	Settings();
};
#endif // !SETTINGS_H