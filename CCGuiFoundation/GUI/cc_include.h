#ifndef CC_INCLUDE
#define CC_INCLUDE

#ifdef CCGUI_EXPORTS
#define CCGUI_API __declspec(dllexport)
#else
#define CCGUI_API __declspec(dllimport)
#endif

extern "C" CCGUI_API int SetupCCGuiFoundation();

#endif