#ifndef CC_CONTROLLER
#define CC_CONTROLLER

#include "stdafx.h"
#include "cc_interface.h"
#include "cc_presentation.h"
#include "cc_service.h"
#include "cc_form.h"

using namespace cc::interfaces::windows;
using namespace cc::presentation::windows::service;

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			namespace controller
			{
				class Controller : public IController, public virtual IWindowService
				{
				public:
					Controller(HINSTANCE _hInstance);
					~Controller();

					PassRefPtr<Form>					GetFormFromHandle(HWND hwnd);
					bool								HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & result);
					PassRefPtr<IWindow>					CreatWindow(bool shadow = false);
					void								DestroyWindow(PassRefPtr<IWindow> window);
					PassRefPtr<IWindow>					GetMainWindow();
					void								Run(PassRefPtr<IWindow> window);
					PassRefPtr<IWindow>					GetWindow(CPoint location);

					PassRefPtr<ICallbackService>		GetCallbackService();
					PassRefPtr<IResourceService>		GetResourceService();
					PassRefPtr<IAsyncService>			GetAsyncService();
					PassRefPtr<IClipboardService>		GetClipboardService();
					PassRefPtr<IImageService>			GetImageService();
					PassRefPtr<IScreenService>			GetScreenService();
					PassRefPtr<IWindowService>			GetWindowService();
					PassRefPtr<IInputService>			GetInputService();
					PassRefPtr<IDialogService>			GetDialogService();

					void								InvokeMouseHook(WPARAM message, CPoint location);					
					void								InvokeGlobalTimer();
					void								InvokeClipboardUpdated();

				protected:
					WinClass							windowClass;
					WinClass							shadowClass;
					WinClass							ctrlClass;
					HINSTANCE							hInstance;
					HWND								ctrlWindow;
					map<HWND, RefPtr<Form>>				Windows;
					RefPtr<IWindow>						mainWindow;
					HWND								mainWindowHandle;

					RefPtr<CallbackService>				callbackService;
					RefPtr<ResourceService>				resourceService;
					RefPtr<AsyncService>				asyncService;
					RefPtr<ClipboardService>			clipboardService;
					RefPtr<ImageService>				imageService;
					RefPtr<ScreenService>				screenService;
					RefPtr<InputService>				inputService;
					RefPtr<DialogService>				dialogService;
				};
			}
		}
	}
}


#endif