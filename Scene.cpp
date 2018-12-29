#include "Scene.h"


Scene::Scene():Ready(false)
{}

int Scene::init(const char *file)
{
	int res = MainWindow.Init(&Setting, "Proga");
	if (res)
		return res;
	MainWindow.Show(SW_SHOWNORMAL);
	res = Resource.init(&Setting, &MainWindow);
	if (res)
		return res;
	return 0;
}