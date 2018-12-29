#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Model.h"
#include "Settings.h"
#include "Resources.h"
#include "ListError.h"
#include "WindowHandler.h"

class Scene
{
private:
	bool Ready;
	std::vector<Model> Models;
	Settings Setting;
	Resources Resource;
	WindowHandler MainWindow;
public:
	Scene();
int	init(const char*);
};

#endif // !SCENE_H