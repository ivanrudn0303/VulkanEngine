#include "WindowHandler.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

WindowHandler::WindowHandler(): Ready(false)
{
}

int WindowHandler::Init(const Settings *sett, const char* name)
{
	if (Ready)
		return 0;
	Name = name;
	Hinstance = GetModuleHandle(NULL);
	WNDCLASSEX wndcls = {};
	wndcls.cbSize = sizeof(wndcls);
	wndcls.lpfnWndProc = WindowProc;
	wndcls.hInstance = Hinstance;
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndcls.lpszClassName = "AnyNameofClass";

	if (!RegisterClassEx(&wndcls))
		return WRONG_WINDOW_CREATING;
	Hwnd = CreateWindowEx(0, "AnyNameofClass", Name.c_str(), WS_POPUP, 0, 0,
		sett->Resolution.x, sett->Resolution.y, nullptr, nullptr, Hinstance, nullptr);
	if (Hwnd == nullptr)
		return WRONG_WINDOW_CREATING;
	Ready = true;
	return 0;
}

int WindowHandler::Show(int mode)
{
	if (!Ready)
		return WRONG_WINDOW_NOT_READY;
	ShowWindow(Hwnd, mode);
	return 0;
}

const HWND& WindowHandler::GetHwnd() const
{
	return Hwnd;
}

const std::string & WindowHandler::GetName() const
{
	return Name;
}

const HINSTANCE& WindowHandler::GetHinstance() const
{
	return Hinstance;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}