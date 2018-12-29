#ifndef WINDOWHANDLER_H
#define WINDOWHANDLER_H

#include <Windows.h>
#include "Settings.h"
#include <string>
#include "ListError.h"

class WindowHandler
{
private:
	HWND Hwnd;
	HINSTANCE Hinstance;
	std::string Name;
	bool Ready;
public:
	WindowHandler();
int	Init(const Settings*, const char*);
int Show(int mode);
const HWND& GetHwnd() const;
const std::string& GetName() const;
const HINSTANCE& GetHinstance() const;
};
#endif // !WINDOHANDLE_H
