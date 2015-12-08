#include <tchar.h>
#include <windows.h>
#include "../CCGuiFoundation/GUI/cc_include.h"
#if defined(_DEBUG)
#pragma comment(lib,"../Debug/CCGuiFoundation.lib")
#else
#pragma comment(lib,"../Release/CCGuiFoundation.lib")
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupCCGuiFoundation();
}
