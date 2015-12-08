#ifndef CC_SERVICE
#define CC_SERVICE

#include "stdafx.h"
#include "cc_interface.h"
#include "cc_presentation.h"
#include "cc_resource.h"
#include "cc_thread.h"

using namespace std::chrono;
using namespace cc::threading;
using namespace cc::interfaces::windows;
using namespace cc::presentation::windows::resource;

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			namespace service
			{
				class CallbackService;
				class ResourceService;
				class AsyncService;
				class ClipboardService;
				class ImageService;
				class ScreenService;
				class InputService;
				class DialogService;

				class CallbackService : public ICallbackService
				{
				public:
					CallbackService();

					bool					InstallListener(PassRefPtr<IControllerListener> listener) override;
					bool					UninstallListener(PassRefPtr<IControllerListener> listener) override;

					void					InvokeMouseHook(WPARAM message, CPoint location);
					void					InvokeGlobalTimer();
					void					InvokeClipboardUpdated();
					void					InvokeWindowCreated(PassRefPtr<IWindow> window);
					void					InvokeWindowDestroyed(PassRefPtr<IWindow> window);

				protected:
					set<RawPtr<IControllerListener>> listeners;
				};

				class ClipboardService : public IClipboardService
				{
				public:
					ClipboardService();

					void					SetOwnerHandle(HWND handle);
					bool					ContainsText() override;
					CString					GetText() override;
					bool					SetText(const CString& value) override;

				protected:
					HWND					ownerHandle;
				};

				class ResourceService : public IResourceService
				{
				public:
					ResourceService();

					PassRefPtr<ICursor>		GetSystemCursor(ICursor::CursorType type) override;
					PassRefPtr<ICursor>		GetDefaultSystemCursor() override;
					Font					GetDefaultFont() override;
					void					SetDefaultFont(const Font& value) override;

				protected:
					vector<RefPtr<Cursor>>		systemCursors;
					Font						defaultFont;
				};

				class AsyncService : public IAsyncService
				{
				protected:
					struct TaskItem
					{
						RefPtr<semaphore>	sem;
						function<void()>	proc;

						TaskItem();
						TaskItem(PassRefPtr<semaphore> _semaphore, const function<void()>& _proc);
						~TaskItem();
					};

					class DelayItem : public IDelay
					{
					public:
						DelayItem(PassRefPtr<AsyncService> _service, const function<void()>& _proc, bool _executeInMainThread, cint milliseconds);
						~DelayItem();

						RawPtr<AsyncService>				service;
						function<void()>					proc;
						ExecuteStatus						status;
						time_point<system_clock>			executeTime;
						bool								executeInMainThread;

						ExecuteStatus						GetStatus() override;
						bool								Delay(cint milliseconds) override;
						bool								Cancel() override;
					};

				public:
					AsyncService();
					~AsyncService();

					void									ExecuteAsyncTasks() override;
					bool									IsInMainThread() override;
					void									InvokeAsync(const function<void()>& proc) override;
					void									InvokeInMainThread(const function<void()>& proc) override;
					bool									InvokeInMainThreadAndWait(const function<void()>& proc, cint milliseconds = -1) override;
					PassRefPtr<IDelay>						DelayExecute(const function<void()>& proc, cint milliseconds) override;
					PassRefPtr<IDelay>						DelayExecuteInMainThread(const function<void()>& proc, cint milliseconds1) override;
				
				protected:
					cint									mainThreadId;
					spin_mutex								taskListLock;
					vector<TaskItem>						taskItems;
					list<RefPtr<DelayItem>>					delayItems;
				};

				class ImageService : public IImageService
				{
				public:
					ImageService();

					PassRefPtr<IImage>						CreateImageFromFile(const CString& path);
					PassRefPtr<IImage>						CreateImageFromMemory(void* buffer, cint length);
					PassRefPtr<IImage>						CreateImageFromHBITMAP(HBITMAP handle);
					PassRefPtr<IImage>						CreateImageFromHICON(HICON handle);
					CComPtr<IWICImagingFactory>				GetImagingFactory();

				protected:
					CComPtr<IWICImagingFactory>				imagingFactory;
				};

				class ScreenService : public IScreenService
				{
					typedef HWND(*HandleRetriver)(PassRefPtr<IWindow>);
				public:
					ScreenService(HandleRetriver _handleRetriver);

					struct MonitorEnumProcData
					{
						ScreenService*		screenService;
						cint				currentScreen;
					};

					static BOOL CALLBACK					MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
					void									RefreshScreenInformation();
					cint									GetScreenCount() override;
					PassRefPtr<IScreen>						GetScreen(cint index) override;
					PassRefPtr<IScreen>						GetScreen(PassRefPtr<IWindow> window) override;
				
				protected:
					vector<RefPtr<Screen>>					screens;
					HandleRetriver							handleRetriver;
				};

				class InputService : public IInputService
				{
				public:
					InputService(HOOKPROC _mouseProc);

					void									SetOwnerHandle(HWND handle);
					void									StartHookMouse() override;
					void									StopHookMouse() override;
					bool									IsHookingMouse() override;
					void									StartTimer() override;
					void									StopTimer() override;
					bool									IsTimerEnabled() override;
					bool									IsKeyPressing(cint code) override;
					bool									IsKeyToggled(cint code) override;
					CString									GetKeyName(cint code) override;
					cint									GetKey(const CString& name) override;
				
				protected:
					CString									GetKeyNameInternal(cint code);
					void									InitializeKeyNames();

					HWND									ownerHandle;
					HHOOK									mouseHook;
					bool									isTimerEnabled;
					HOOKPROC								mouseProc;

					vector<CString>							keyNames;
					map<CString, cint>						keys;
				};

				class DialogService : public IDialogService
				{
					typedef HWND(*HandleRetriver)(PassRefPtr<IWindow>);
				public:
					DialogService(HandleRetriver _handleRetriver);

					MessageBoxButtonsOutput ShowMessageBox(
						PassRefPtr<IWindow> window,
						const CString& text,
						const CString& title,
						MessageBoxButtonsInput buttons,
						MessageBoxDefaultButton defaultButton,
						MessageBoxIcons icon,
						MessageBoxModalOptions modal) override;

					bool ShowColorDialog(
						PassRefPtr<IWindow> window,
						CColor& selection,
						bool selected,
						ColorDialogCustomColorOptions customColorOptions,
						CColor* customColors) override;

					bool ShowFontDialog(
						PassRefPtr<IWindow> window,
						Font& selectionFont,
						CColor& selectionColor,
						bool selected,
						bool showEffect,
						bool forceFontExist) override;

					bool ShowFileDialog(
						PassRefPtr<IWindow> window,
						vector<CString>& selectionFileNames,
						cint& selectionFilterIndex,
						FileDialogTypes dialogType,
						const CString& title,
						const CString& initialFileName,
						const CString& initialDirectory,
						const CString& defaultExtension,
						const CString& filter,
						FileDialogOptions options) override;

				protected:
					HandleRetriver handleRetriver;
				};
			}
		}
	}
}

#endif