#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
#include "Scene.h"


int main()
{
	{
		Scene sc;
		sc.init("file");
	}
//	while (true);
	system("Pause");
	return 0;
}