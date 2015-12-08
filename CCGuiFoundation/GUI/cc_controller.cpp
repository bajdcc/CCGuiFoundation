#include "stdafx.h"
#include "cc_controller.h"
#include "cc_resource.h"

using namespace cc::presentation::windows::helper;
using namespace cc::presentation::windows::resource::procedure;

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			namespace controller
			{
				Controller::Controller(HINSTANCE _hInstance) : hInstance(_hInstance)
					, windowClass(_T("CCGuiFoundation_Display"), false, false, WndProc, _hInstance)
					, shadowClass(_T("CCGuiFoundation_Shadow"), true, false, WndProc, _hInstance)
					, ctrlClass(_T("CCGuiFoundation_Host"), false, false, CtrlProc, _hInstance)
					, mainWindowHandle(NULL)
				{
					ctrlWindow = CreateWindowEx(WS_EX_CONTROLPARENT, ctrlClass.GetName(), NULL, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

					callbackService = adoptRef(new CallbackService());
					resourceService = adoptRef(new ResourceService());
					asyncService = adoptRef(new AsyncService());
					clipboardService = adoptRef(new ClipboardService());
					imageService = adoptRef(new ImageService());
					screenService = adoptRef(new ScreenService(GetHWNDFromWindow));
					inputService = adoptRef(new InputService(MouseProc));
					dialogService = adoptRef(new DialogService(GetHWNDFromWindow));

					clipboardService->SetOwnerHandle(ctrlWindow);
					inputService->SetOwnerHandle(ctrlWindow);
				}

				Controller::~Controller()
				{
					inputService->StopTimer();
					inputService->StopHookMouse();
					clipboardService->SetOwnerHandle(NULL);
					::DestroyWindow(ctrlWindow);
				}

				PassRefPtr<Form> Controller::GetFormFromHandle(HWND hwnd)
				{
					auto found = Windows.find(hwnd);
					if (found == Windows.end())
						return nullptr;
					return found->second;
				}

				bool Controller::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& result)
				{
					bool skipDefaultProcedure = false;
					auto found = Windows.find(hwnd);
					if (found != Windows.end())
					{
						RefPtr<Form> window = found->second;
						skipDefaultProcedure = window->HandleMessage(hwnd, uMsg, wParam, lParam, result);
						switch (uMsg)
						{
						case WM_CLOSE:
							if (!skipDefaultProcedure)
							{
								ShowWindow(window->GetWindowHandle(), SW_HIDE);
								if (window != mainWindow)
								{
									skipDefaultProcedure = true;
								}
							}
							break;
						case WM_DESTROY:
							DestroyWindow(window);
							break;
						}
					}

					if (hwnd == mainWindowHandle && uMsg == WM_DESTROY)
					{
						for (auto & window : Windows)
						{
							if (window.second->IsVisible())
							{
								window.second->Hide();
							}
						}
						while (!Windows.empty())
						{
							DestroyWindow(Windows.begin()->second);
						}
						PostQuitMessage(0);
					}
					return skipDefaultProcedure;
				}

				//=======================================================================

				PassRefPtr<IWindow> Controller::CreatWindow(bool shadow /*= false*/)
				{
					RefPtr<Form> window = adoptRef(new Form(ctrlWindow, shadow ? shadowClass.GetName() : windowClass.GetName(), hInstance));
					Windows.insert(make_pair(window->GetWindowHandle(), window));
					callbackService->InvokeWindowCreated(window);
					window->SetCursor(resourceService->GetDefaultSystemCursor());
					return window;
				}

				void Controller::DestroyWindow(PassRefPtr<IWindow> window)
				{
					if (!window)
						return;
					RefPtr<Form> windowsForm = window;
					windowsForm->InvokeDestroying();
					auto handle = windowsForm->GetWindowHandle();
					auto found = Windows.find(handle);
					if (found != Windows.end())
					{
						callbackService->InvokeWindowDestroyed(windowsForm);
						Windows.erase(found);
					}
				}

				PassRefPtr<IWindow> Controller::GetMainWindow()
				{
					return mainWindow;
				}

				void Controller::Run(PassRefPtr<IWindow> window)
				{
					mainWindow = window;
					mainWindowHandle = GetForm(mainWindow)->GetWindowHandle();
					mainWindow->Show();
					WinMessageLoop winMsgLoop;
					winMsgLoop.Run();
				}

				PassRefPtr<IWindow> Controller::GetWindow(CPoint location)
				{
					POINT p;
					p.x = (int)location.x;
					p.y = (int)location.y;
					HWND handle = WindowFromPoint(p);
					auto found = Windows.find(handle);
					if (found == Windows.end())
					{
						return nullptr;
					}
					else
					{
						return found->second;
					}
				}

				//=======================================================================

				PassRefPtr<ICallbackService> Controller::GetCallbackService()
				{
					return callbackService;
				}

				PassRefPtr<IResourceService> Controller::GetResourceService()
				{
					return resourceService;
				}

				PassRefPtr<IAsyncService> Controller::GetAsyncService()
				{
					return asyncService;
				}

				PassRefPtr<IClipboardService> Controller::GetClipboardService()
				{
					return clipboardService;
				}

				PassRefPtr<IImageService> Controller::GetImageService()
				{
					return imageService;
				}

				PassRefPtr<IScreenService> Controller::GetScreenService()
				{
					return screenService;
				}

				PassRefPtr<IWindowService> Controller::GetWindowService()
				{
					return this;
				}

				PassRefPtr<IInputService> Controller::GetInputService()
				{
					return inputService;
				}

				PassRefPtr<IDialogService> Controller::GetDialogService()
				{
					return dialogService;
				}

				//=======================================================================

				void Controller::InvokeMouseHook(WPARAM message, CPoint location)
				{
					callbackService->InvokeMouseHook(message, location);
				}

				void Controller::InvokeGlobalTimer()
				{
					callbackService->InvokeGlobalTimer();
				}

				void Controller::InvokeClipboardUpdated()
				{
					callbackService->InvokeClipboardUpdated();
				}
			}
		}
	}
}