#include "stdafx.h"
#include "cc_service.h"

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			namespace service
			{
				CallbackService::CallbackService()
				{

				}

				bool CallbackService::InstallListener(PassRefPtr<IControllerListener> listener)
				{
					return listeners.insert(listener.get()).second;
				}

				bool CallbackService::UninstallListener(PassRefPtr<IControllerListener> listener)
				{
					return listeners.erase(listener.get()) == 1;
				}

				void CallbackService::InvokeMouseHook(WPARAM message, CPoint location)
				{
					switch (message)
					{
						case WM_LBUTTONDOWN:
						{
							for (auto & listener : listeners)
							{
								listener->LeftButtonDown(location);
							}
						}
							break;
						case WM_LBUTTONUP:
						{
							for (auto & listener : listeners)
							{
								listener->LeftButtonUp(location);
							}
						}
							break;
						case WM_RBUTTONDOWN:
						{
							for (auto & listener : listeners)
							{
								listener->RightButtonDown(location);
							}
						}
							break;
						case WM_RBUTTONUP:
						{
							for (auto & listener : listeners)
							{
								listener->RightButtonUp(location);
							}
						}
							break;
						case WM_MOUSEMOVE:
						{
							for (auto & listener : listeners)
							{
								listener->MouseMoving(location);
							}
						}
							break;
					}
				}

				void CallbackService::InvokeGlobalTimer()
				{
					for (auto & listener : listeners)
					{
						listener->GlobalTimer();
					}
				}

				void CallbackService::InvokeClipboardUpdated()
				{
					for (auto & listener : listeners)
					{
						listener->ClipboardUpdated();
					}
				}

				void CallbackService::InvokeWindowCreated(PassRefPtr<IWindow> window)
				{
					RefPtr<IWindow> _window = window;
					for (auto & listener : listeners)
					{
						listener->WindowCreated(_window);
					}
				}

				void CallbackService::InvokeWindowDestroyed(PassRefPtr<IWindow> window)
				{
					RefPtr<IWindow> _window = window;
					for (auto & listener : listeners)
					{
						listener->WindowDestroying(_window);
					}
				}

				ResourceService::ResourceService()
				{
					{
						systemCursors.resize(ICursor::CursorCount);
						for (size_t i = 0; i < systemCursors.size(); i++)
						{
							systemCursors[i] = adoptRef(new Cursor((ICursor::CursorType)i));
						}
					}
					{
						NONCLIENTMETRICS metrics;
						metrics.cbSize = sizeof(NONCLIENTMETRICS);
						SystemParametersInfo(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
						if (!*metrics.lfMessageFont.lfFaceName)
						{
							metrics.cbSize = sizeof(NONCLIENTMETRICS)-sizeof(metrics.iPaddedBorderWidth);
							SystemParametersInfo(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
						}
						defaultFont.fontFamily = metrics.lfMessageFont.lfFaceName;
						defaultFont.size = metrics.lfMessageFont.lfHeight;
						if (defaultFont.size < 0)
						{
							defaultFont.size = -defaultFont.size;
						}
					}
				}

				PassRefPtr<ICursor> ResourceService::GetSystemCursor(ICursor::CursorType type)
				{
					cint index = (cint)type;
					if (0 <= index && index < (int)systemCursors.size())
					{
						return systemCursors[index];
					}
					else
					{
						return nullptr;
					}
				}

				PassRefPtr<ICursor> ResourceService::GetDefaultSystemCursor()
				{
					return GetSystemCursor(ICursor::Arrow);
				}

				Font ResourceService::GetDefaultFont()
				{
					return defaultFont;
				}

				void ResourceService::SetDefaultFont(const Font& value)
				{
					defaultFont = value;
				}

				AsyncService::AsyncService()
					: mainThreadId(std::hash<std::thread::id>()(std::this_thread::get_id()))
				{
				}

				AsyncService::~AsyncService()
				{
					ATLTRACE("Async Service stopped\n");
				}

				void AsyncService::ExecuteAsyncTasks()
				{
					time_point<system_clock> now = system_clock::now();
					decltype(taskItems) items;
					decltype(delayItems) executableDelayItems;

					{
						lock_guard<spin_mutex> lock(taskListLock);
						items = taskItems;
						taskItems.clear();
						for (auto i = delayItems.begin(); i != delayItems.end();)
						{
							RefPtr<DelayItem> item = *i;
							if (now >= item->executeTime)
							{
								item->status = IDelay::Executing;
								executableDelayItems.push_back(item);
								i = delayItems.erase(i);
							}
							else
							{
								i++;
							}
						}
					}

					for (auto & item : items)
					{
						item.proc();
						if (item.sem)
						{
							item.sem->signal();
						}
					}
					for (auto & item : executableDelayItems)
					{
						if (item->executeInMainThread)
						{
							item->proc();
							item->status = IDelay::Executed;
						}
						else
						{
							InvokeAsync([=]()
							{
								item->proc();
								item->status = IDelay::Executed;
							});
						}
					}
				}

				bool AsyncService::IsInMainThread()
				{
					return std::hash<std::thread::id>()(std::this_thread::get_id()) == mainThreadId;
				}

				void AsyncService::InvokeAsync(const function<void()>& proc)
				{
					thread_pool_lite::queue(proc);
				}

				void AsyncService::InvokeInMainThread(const function<void()>& proc)
				{
					lock_guard<spin_mutex> lock(taskListLock);
					TaskItem item(nullptr, proc);
					taskItems.push_back(item);
				}

				bool AsyncService::InvokeInMainThreadAndWait(const function<void()>& proc, cint milliseconds /*= -1*/)
				{
					RefPtr<semaphore> sem = adoptRef(new semaphore(0));

					{
						lock_guard<spin_mutex> lock(taskListLock);
						TaskItem item(sem, proc);
						taskItems.push_back(item);
					}

					if (milliseconds < 0)
					{
						return sem->wait();
					}
					else
					{
						return sem->wait_for(chrono::milliseconds(milliseconds));
					}
				}

				PassRefPtr<IDelay> AsyncService::DelayExecute(const function<void()>& proc, cint milliseconds)
				{
					RefPtr<DelayItem> delay;
					{
						lock_guard<spin_mutex> lock(taskListLock);
						delay = adoptRef(new DelayItem(this, proc, false, milliseconds));
						delayItems.push_back(delay);
					}
					return delay;
				}

				PassRefPtr<IDelay> AsyncService::DelayExecuteInMainThread(const function<void()>& proc, cint milliseconds)
				{
					RefPtr<DelayItem> delay;
					{
						lock_guard<spin_mutex> lock(taskListLock);
						delay = adoptRef(new DelayItem(this, proc, true, milliseconds));
						delayItems.push_back(delay);
					}
					return delay;
				}

				AsyncService::TaskItem::TaskItem()
				{
				}

				AsyncService::TaskItem::TaskItem(PassRefPtr<semaphore> _semaphore, const function<void()>& _proc)
					: sem(_semaphore)
					, proc(_proc)
				{
				}

				AsyncService::TaskItem::~TaskItem()
				{
				}

				AsyncService::DelayItem::DelayItem(PassRefPtr<AsyncService> _service,
					const function<void()>& _proc, bool _executeInMainThread, cint milliseconds)
					: service(_service)
					, proc(_proc)
					, status(IDelay::Pending)
					, executeTime(system_clock::now() + chrono::milliseconds(milliseconds))
					, executeInMainThread(_executeInMainThread)
				{
				}

				AsyncService::DelayItem::~DelayItem()
				{
				}

				IDelay::ExecuteStatus AsyncService::DelayItem::GetStatus()
				{
					return status;
				}

				bool AsyncService::DelayItem::Delay(cint milliseconds)
				{
					lock_guard<spin_mutex> lock(service->taskListLock);
					if (status == IDelay::Pending)
					{
						executeTime = system_clock::now() + chrono::milliseconds(milliseconds);
						return true;
					}
					return false;
				}

				bool AsyncService::DelayItem::Cancel()
				{
					lock_guard<spin_mutex> lock(service->taskListLock);
					if (status == IDelay::Pending)
					{
						auto & delayItems = service->delayItems;
						for (auto i = delayItems.begin(); i != delayItems.end(); i++)
						{
							if (*i == this)
							{
								delayItems.erase(i);
								status = IDelay::Canceled;
								return true;
							}
						}
					}
					return false;
				}

				ImageService::ImageService()
				{
					CComPtr<IWICImagingFactory> ImagingFactory;
					HRESULT hr = ImagingFactory.CoCreateInstance(
#if defined(WINCODEC_SDK_VERSION2)
						CLSID_WICImagingFactory1,
#else
						CLSID_WICImagingFactory,
#endif
						NULL,
						CLSCTX_INPROC_SERVER);
					if (SUCCEEDED(hr))
					{
						imagingFactory = ImagingFactory;
					}
				}

				PassRefPtr<IImage> ImageService::CreateImageFromFile(const CString& path)
				{
					CComPtr<IWICBitmapDecoder> bitmapDecoder;
					HRESULT hr = imagingFactory->CreateDecoderFromFilename(
						path,
						NULL,
						GENERIC_READ,
						WICDecodeMetadataCacheOnDemand,
						&bitmapDecoder);
					if (SUCCEEDED(hr))
					{
						return adoptRef(new Image(this, bitmapDecoder));
					}
					else
					{
						return nullptr;
					}
				}

				PassRefPtr<IImage> ImageService::CreateImageFromMemory(void* buffer, cint length)
				{
					RefPtr<IImage> result;
					CComPtr<IStream> stream = SHCreateMemStream((const BYTE*)buffer, (int)length);
					if (stream)
					{
						CComPtr<IWICBitmapDecoder> bitmapDecoder;
						HRESULT hr = imagingFactory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnDemand, &bitmapDecoder);
						if (SUCCEEDED(hr))
						{
							result = adoptRef(new Image(this, bitmapDecoder));
						}
					}
					return result;
				}

				PassRefPtr<IImage> ImageService::CreateImageFromHBITMAP(HBITMAP handle)
				{
					CComPtr<IWICBitmap> bitmap;
					HRESULT hr = imagingFactory->CreateBitmapFromHBITMAP(handle, NULL, WICBitmapUseAlpha, &bitmap);
					if (SUCCEEDED(hr))
					{
						return adoptRef(new BitmapImage(this, bitmap, IImage::Bmp));
					}
					else
					{
						return nullptr;
					}
				}

				PassRefPtr<IImage> ImageService::CreateImageFromHICON(HICON handle)
				{
					CComPtr<IWICBitmap> bitmap;
					HRESULT hr = imagingFactory->CreateBitmapFromHICON(handle, &bitmap);
					if (SUCCEEDED(hr))
					{
						return adoptRef(new BitmapImage(this, bitmap, IImage::Icon));
					}
					else
					{
						return nullptr;
					}
				}

				CComPtr<IWICImagingFactory> ImageService::GetImagingFactory()
				{
					return imagingFactory;
				}

				ClipboardService::ClipboardService()
					: ownerHandle(NULL)
				{
				}

				void ClipboardService::SetOwnerHandle(HWND handle)
				{
					HWND oldHandle = ownerHandle;
					ownerHandle = handle;
					if (handle == NULL)
					{
						RemoveClipboardFormatListener(oldHandle);
					}
					else
					{
						AddClipboardFormatListener(ownerHandle);
					}
				}

				bool ClipboardService::ContainsText()
				{
					if (OpenClipboard(ownerHandle))
					{
						UINT format = 0;
						bool contains = false;
						while (format = EnumClipboardFormats(format))
						{
							if (format == CF_TEXT || format == CF_UNICODETEXT)
							{
								contains = true;
								break;
							}
						}
						CloseClipboard();
						return contains;
					}
					return false;
				}

				CString ClipboardService::GetText()
				{
					if (OpenClipboard(ownerHandle))
					{
						CString result;
						HANDLE handle = GetClipboardData(CF_UNICODETEXT);
						if (handle != 0)
						{
							LPCTSTR buffer = (LPCTSTR)GlobalLock(handle);
							result = buffer;
							GlobalUnlock(handle);
						}
						CloseClipboard();
						return result;
					}
					return _T("");
				}

				bool ClipboardService::SetText(const CString& value)
				{
					if (OpenClipboard(ownerHandle))
					{
						EmptyClipboard();
						cint size = (value.GetLength() + 1)*sizeof(TCHAR);
						HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE, size);
						LPCTSTR buffer = (LPCTSTR)GlobalLock(data);
						CopyMemory((void*)buffer, (void*)(LPCTSTR)value, size);
						GlobalUnlock(data);
						SetClipboardData(CF_UNICODETEXT, data);
						CloseClipboard();
						return true;
					}
					return false;
				}

				ScreenService::ScreenService(HandleRetriver _handleRetriver)
					: handleRetriver(_handleRetriver)
				{
				}

				BOOL CALLBACK ScreenService::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
				{
					MonitorEnumProcData* data = (MonitorEnumProcData*)dwData;
					if (data->currentScreen == data->screenService->screens.size())
					{
						data->screenService->screens.push_back(adoptRef(new Screen()));
					}
					data->screenService->screens[data->currentScreen]->SetMonitor(hMonitor);
					data->currentScreen++;
					return TRUE;
				}

				void ScreenService::RefreshScreenInformation()
				{
					for (auto & screen : screens)
					{
						screen->SetMonitor(NULL);
					}
					MonitorEnumProcData data;
					data.screenService = this;
					data.currentScreen = 0;
					EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)(&data));
				}

				cint ScreenService::GetScreenCount()
				{
					RefreshScreenInformation();
					return GetSystemMetrics(SM_CMONITORS);
				}

				PassRefPtr<IScreen> ScreenService::GetScreen(cint index)
				{
					RefreshScreenInformation();
					return screens[index];
				}

				PassRefPtr<IScreen> ScreenService::GetScreen(PassRefPtr<IWindow> window)
				{
					RefreshScreenInformation();
					HWND hwnd = handleRetriver(window);
					if (hwnd)
					{
						HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
						if (monitor != NULL)
						{
							for (auto & screen : screens)
							{
								if (screen->GetMonitor() == monitor)
								{
									return screen;
								}
							}
						}
					}
					return nullptr;
				}

				CString InputService::GetKeyNameInternal(cint code)
				{
					if (code < 8) return _T("?");
					TCHAR name[256] = { 0 };
					cint scanCode = MapVirtualKey((int)code, MAPVK_VK_TO_VSC) << 16;
					switch (code)
					{
						case VK_INSERT:
						case VK_DELETE:
						case VK_HOME:
						case VK_END:
						case VK_PRIOR:
						case VK_NEXT:
						case VK_LEFT:
						case VK_RIGHT:
						case VK_UP:
						case VK_DOWN:
							scanCode |= 1 << 24;
							break;
						case VK_CLEAR:
						case VK_LSHIFT:
						case VK_RSHIFT:
						case VK_LCONTROL:
						case VK_RCONTROL:
						case VK_LMENU:
						case VK_RMENU:
							return _T("?");
					}
					GetKeyNameText((int)scanCode, name, sizeof(name) / sizeof(*name));
					return name[0] ? name : _T("?");
				}

				void InputService::InitializeKeyNames()
				{
					for (size_t i = 0; i < keyNames.size(); i++)
					{
						keyNames[i] = GetKeyNameInternal(i);
						if (keyNames[i] != _T("?"))
						{
							keys[keyNames[i]] = i;
						}
					}
				}

				InputService::InputService(HOOKPROC _mouseProc)
					: ownerHandle(NULL)
					, mouseHook(NULL)
					, isTimerEnabled(false)
					, mouseProc(_mouseProc)
					, keyNames(146)
				{
					InitializeKeyNames();
				}

				void InputService::SetOwnerHandle(HWND handle)
				{
					ownerHandle = handle;
				}

				void InputService::StartHookMouse()
				{
					if (!IsHookingMouse())
					{
						mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, NULL, NULL);
					}
				}

				void InputService::StopHookMouse()
				{
					if (IsHookingMouse())
					{
						UnhookWindowsHookEx(mouseHook);
						mouseHook = NULL;
					}
				}

				bool InputService::IsHookingMouse()
				{
					return mouseHook != NULL;
				}

				void InputService::StartTimer()
				{
					if (!IsTimerEnabled())
					{
						SetTimer(ownerHandle, 1, 16, NULL);
						isTimerEnabled = true;
					}
				}

				void InputService::StopTimer()
				{
					if (IsTimerEnabled())
					{
						KillTimer(ownerHandle, 1);
						isTimerEnabled = false;
					}
				}

				bool InputService::IsTimerEnabled()
				{
					return isTimerEnabled;
				}

				bool InputService::IsKeyPressing(cint code)
				{
					return IsKeyPressing(code);
				}

				bool InputService::IsKeyToggled(cint code)
				{
					return IsKeyToggled(code);
				}

				CString InputService::GetKeyName(cint code)
				{
					if (0 <= code && 0 < keyNames.size())
					{
						return keyNames[code];
					}
					else
					{
						return _T("?");
					}
				}

				cint InputService::GetKey(const CString& name)
				{
					auto found = keys.find(name);
					return found == keys.end() ? -1 : found->second;

				}

				DialogService::DialogService(HandleRetriver _handleRetriver)
					: handleRetriver(_handleRetriver)
				{
				}

				IDialogService::MessageBoxButtonsOutput DialogService::ShowMessageBox(
					PassRefPtr<IWindow> window,
					const CString& text,
					const CString& title,
					MessageBoxButtonsInput buttons,
					MessageBoxDefaultButton defaultButton,
					MessageBoxIcons icon,
					MessageBoxModalOptions modal)
				{
					CString realTitle = title;
					if (title.IsEmpty() && window)
					{
						realTitle = window->GetTitle();
					}
					HWND hWnd = handleRetriver(window);
					LPCTSTR lpText = text;
					LPCTSTR lpCaption = realTitle;
					UINT uType = 0;

#define MAP(A, B) case A: uType|=(B); break
					switch (buttons)
					{
						MAP(DisplayOK, MB_OK);
						MAP(DisplayOKCancel, MB_OKCANCEL);
						MAP(DisplayYesNo, MB_YESNO);
						MAP(DisplayYesNoCancel, MB_YESNOCANCEL);
						MAP(DisplayRetryCancel, MB_RETRYCANCEL);
						MAP(DisplayAbortRetryIgnore, MB_ABORTRETRYIGNORE);
						MAP(DisplayCancelTryAgainContinue, MB_CANCELTRYCONTINUE);
					}
					switch (defaultButton)
					{
						MAP(DefaultFirst, MB_DEFBUTTON1);
						MAP(DefaultSecond, MB_DEFBUTTON2);
						MAP(DefaultThird, MB_DEFBUTTON3);
					}
					switch (icon)
					{
						MAP(IconError, MB_ICONERROR);
						MAP(IconQuestion, MB_ICONQUESTION);
						MAP(IconWarning, MB_ICONWARNING);
						MAP(IconInformation, MB_ICONINFORMATION);
					}
					switch (modal)
					{
						MAP(ModalWindow, MB_APPLMODAL);
						MAP(ModalSystem, MB_SYSTEMMODAL);
						MAP(ModalTask, MB_TASKMODAL);
					}
#undef MAP

					cint result = MessageBox(hWnd, lpText, lpCaption, uType);
					switch (result)
					{
						case IDABORT: return SelectAbort;
						case IDCANCEL: return SelectCancel;
						case IDCONTINUE: return SelectContinue;
						case IDIGNORE: return SelectIgnore;
						case IDNO: return SelectNo;
						case IDOK: return SelectOK;
						case IDRETRY: return SelectRetry;
						case IDTRYAGAIN: return SelectTryAgain;
						case IDYES: return SelectYes;
						default: return SelectOK;
					}
				}

				bool DialogService::ShowColorDialog(PassRefPtr<IWindow> window,
					CColor& selection,
					bool selected,
					ColorDialogCustomColorOptions customColorOptions,
					CColor* customColors)
				{
					CHOOSECOLOR chooseColor;
					ZeroMemory(&chooseColor, sizeof(chooseColor));
					COLORREF customColorsBuffer[16] = { 0 };
					if (customColors)
					{
						for (cint i = 0; i < sizeof(customColorsBuffer) / sizeof(*customColorsBuffer); i++)
						{
							customColorsBuffer[i] = RGB(customColors[i].r, customColors[i].g, customColors[i].b);
						}
					}

					chooseColor.lStructSize = sizeof(chooseColor);
					chooseColor.hwndOwner = handleRetriver(window);
					chooseColor.rgbResult = RGB(selection.r, selection.g, selection.b);
					chooseColor.lpCustColors = customColorsBuffer;
					chooseColor.Flags = CC_ANYCOLOR;

#define MAP(A, B) case A: chooseColor.Flags|=(B); break
					switch (customColorOptions)
					{
						MAP(CustomColorDisabled, CC_PREVENTFULLOPEN);
						MAP(CustomColorOpened, CC_FULLOPEN);
					}
#undef MAP
					if (selected)
					{
						chooseColor.Flags |= CC_RGBINIT;
					}

					BOOL result = ChooseColor(&chooseColor);
					if (result)
					{
						selection = CColor(GetRValue(chooseColor.rgbResult), GetGValue(chooseColor.rgbResult), GetBValue(chooseColor.rgbResult));
						if (customColors)
						{
							for (cint i = 0; i < sizeof(customColorsBuffer) / sizeof(*customColorsBuffer); i++)
							{
								COLORREF color = customColorsBuffer[i];
								customColors[i] = CColor(GetRValue(color), GetGValue(color), GetBValue(color));
							}
						}
					}
					return result != FALSE;
				}

				bool DialogService::ShowFontDialog(PassRefPtr<IWindow> window,
					Font& selectionFont,
					CColor& selectionColor,
					bool selected,
					bool showEffect,
					bool forceFontExist)
				{
					LOGFONT logFont;
					ZeroMemory(&logFont, sizeof(logFont));
					logFont.lfHeight = -(int)selectionFont.size;
					logFont.lfWeight = selectionFont.bold ? FW_BOLD : FW_REGULAR;
					logFont.lfItalic = selectionFont.italic ? TRUE : FALSE;
					logFont.lfUnderline = selectionFont.underline ? TRUE : FALSE;
					logFont.lfStrikeOut = selectionFont.strikeline ? TRUE : FALSE;
					_tcscpy_s(logFont.lfFaceName, selectionFont.fontFamily);

					CHOOSEFONT chooseFont;
					ZeroMemory(&chooseFont, sizeof(chooseFont));
					chooseFont.lStructSize = sizeof(chooseFont);
					chooseFont.hwndOwner = handleRetriver(window);
					chooseFont.lpLogFont = &logFont;
					chooseFont.iPointSize = 0;
					chooseFont.Flags = (showEffect ? CF_EFFECTS : 0) | (forceFontExist ? CF_FORCEFONTEXIST : 0) | (selected ? CF_INITTOLOGFONTSTRUCT : 0);
					chooseFont.rgbColors = RGB(selectionColor.r, selectionColor.g, selectionColor.b);

					BOOL result = ChooseFont(&chooseFont);
					if (result)
					{
						selectionFont.fontFamily = logFont.lfFaceName;
						selectionFont.bold = logFont.lfWeight > FW_REGULAR;
						selectionFont.italic = logFont.lfItalic != FALSE;
						selectionFont.underline = logFont.lfUnderline != FALSE;
						selectionFont.strikeline = logFont.lfStrikeOut != FALSE;
						selectionFont.size = -logFont.lfHeight;

						selectionColor = CColor(GetRValue(chooseFont.rgbColors), GetGValue(chooseFont.rgbColors), GetBValue(chooseFont.rgbColors));
					}
					return result != FALSE;
				}

				bool DialogService::ShowFileDialog(PassRefPtr<IWindow> window,
					vector<CString>& selectionFileNames,
					cint& selectionFilterIndex,
					FileDialogTypes dialogType,
					const CString& title,
					const CString& initialFileName,
					const CString& initialDirectory,
					const CString& defaultExtension,
					const CString& filter,
					FileDialogOptions options)
				{
					vector<TCHAR> fileNamesBuffer(MAX_PATH + 1);

					OPENFILENAME ofn;
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = handleRetriver(window);
					ofn.hInstance = NULL;
					ofn.lpstrCustomFilter = NULL;
					ofn.nMaxCustFilter = 0;
					ofn.nFilterIndex = (int)selectionFilterIndex + 1;
					ofn.lpstrFile = &fileNamesBuffer[0];
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					if (!initialDirectory.IsEmpty())
						ofn.lpstrInitialDir = initialDirectory;
					if (!title.IsEmpty())
						ofn.lpstrTitle = title;
					if (!defaultExtension.IsEmpty())
						ofn.lpstrDefExt = defaultExtension;

					vector<cint> filterSeparators;
					for (cint i = 0; i < filter.GetLength(); i++)
					{
						if (filter[i] == _T('|'))
						{
							filterSeparators.push_back(i);
						}
					}
					if (filterSeparators.size() % 2 == 1)
					{
						filterSeparators.push_back(filter.GetLength());
					}

					vector<TCHAR> filterBuffer(filter.GetLength() + 2);
					cint index = 0;
					for (size_t i = 0; i < filterSeparators.size(); i++)
					{
						cint end = filterSeparators[i];
						_tcsncpy_s(&filterBuffer[index], filterBuffer.size() - index, (LPCTSTR)filter + index, end - index);
						filterBuffer[end] = 0;
						index = end + 1;
					}
					filterBuffer[index] = 0;
					ofn.lpstrFilter = &filterBuffer[0];

					ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES;
					if (options&FileDialogAllowMultipleSelection)	ofn.Flags |= OFN_ALLOWMULTISELECT;
					if (options&FileDialogFileMustExist)			ofn.Flags |= OFN_FILEMUSTEXIST;
					if (!(options&FileDialogShowReadOnlyCheckBox))	ofn.Flags |= OFN_HIDEREADONLY;
					if (!(options&FileDialogDereferenceLinks))		ofn.Flags |= OFN_NODEREFERENCELINKS;
					if (!(options&FileDialogShowNetworkButton))		ofn.Flags |= OFN_NONETWORKBUTTON;
					if (options&FileDialogPromptCreateFile)			ofn.Flags |= OFN_CREATEPROMPT;
					if (options&FileDialogPromptOverwriteFile)		ofn.Flags |= OFN_OVERWRITEPROMPT;
					if (options&FileDialogDirectoryMustExist)		ofn.Flags |= OFN_PATHMUSTEXIST;
					if (!(options&FileDialogAddToRecent))			ofn.Flags |= OFN_DONTADDTORECENT;

					BOOL result = FALSE;
					switch (dialogType)
					{
						case FileDialogOpen:
							result = GetOpenFileName(&ofn);
							break;
						case FileDialogOpenPreview:
							result = GetOpenFileNamePreview(&ofn);
							break;
						case FileDialogSave:
							result = GetSaveFileName(&ofn);
							break;
						case FileDialogSavePreview:
							result = GetSaveFileNamePreview(&ofn);
							break;
					}
					if (result)
					{
						selectionFilterIndex = ofn.nFilterIndex - 1;
						selectionFileNames.clear();
						if (options&FileDialogAllowMultipleSelection)
						{
							LPCTSTR reading = initialFileName;
							CString directory = reading;
							reading += directory.GetLength() + 1;
							while (*reading)
							{
								CString fileName = reading;
								reading += fileName.GetLength() + 1;
								selectionFileNames.push_back(directory + _T("\\") + fileName);
							}
							if (selectionFileNames.empty())
							{
								selectionFileNames.push_back(directory);
							}
						}
						else
						{
							selectionFileNames.push_back(&fileNamesBuffer[0]);
						}
					}

					return result != FALSE;
				}
			}
		}
	}
}
