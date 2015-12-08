#include "stdafx.h"
#include "cc_form.h"
#include "cc_resource.h"

using namespace cc::presentation::windows::helper;

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			DWORD Form::InternalGetExStyle()
			{
				return (DWORD)GetWindowLongPtr(handle, GWL_EXSTYLE);
			}

			void Form::InternalSetExStyle(DWORD exStyle)
			{
				LONG_PTR result = SetWindowLongPtr(handle, GWL_EXSTYLE, exStyle);
			}

			bool Form::GetExStyle(DWORD exStyle)
			{
				LONG_PTR Long = InternalGetExStyle();
				return (Long & exStyle) != 0;
			}

			void Form::SetExStyle(DWORD exStyle, bool available)
			{
				DWORD Long = InternalGetExStyle();
				if (available)
				{
					Long |= exStyle;
				}
				else
				{
					Long &= ~exStyle;
				}
				InternalSetExStyle((DWORD)Long);
			}

			bool Form::GetStyle(DWORD style)
			{
				LONG_PTR Long = GetWindowLongPtr(handle, GWL_STYLE);
				return (Long & style) != 0;
			}

			void Form::SetStyle(DWORD style, bool available)
			{
				LONG_PTR Long = GetWindowLongPtr(handle, GWL_STYLE);
				if (available)
				{
					Long |= style;
				}
				else
				{
					Long &= ~style;
				}
				SetWindowLongPtr(handle, GWL_STYLE, Long);
			}

			bool Form::GetClassStyle(DWORD style)
			{
				DWORD Long = GetClassLong(handle, GCL_STYLE);
				return (Long & style) != 0;
			}

			void Form::SetClassStyle(DWORD style, bool available)
			{
				DWORD Long = GetClassLong(handle, GCL_STYLE);
				if (available)
				{
					Long |= style;
				}
				else
				{
					Long &= ~style;
				}
				SetClassLong(handle, GCL_STYLE, Long);
			}

			MouseInfo Form::ConvertMouse(WPARAM wParam, LPARAM lParam, bool wheelMessage, bool nonClient)
			{
				MouseInfo info;

				info.nonClient = false;
				if (nonClient)
				{
					switch (wParam)
					{
						case HTMINBUTTON:
						case HTMAXBUTTON:
						case HTCLOSE:
							break;
						default:
							info.nonClient = true;
							break;
					}
				}

				if (wheelMessage)
				{
					info.wheel = GET_WHEEL_DELTA_WPARAM(wParam);
					wParam = GET_KEYSTATE_WPARAM(wParam);
				}
				else
				{
					info.wheel = 0;
				}

				if (nonClient)
				{
					info.ctrl = IsKeyPressing(VK_CONTROL);
					info.shift = IsKeyPressing(VK_SHIFT);
					info.left = IsKeyPressing(MK_LBUTTON);
					info.middle = IsKeyPressing(MK_MBUTTON);
					info.right = IsKeyPressing(MK_RBUTTON);

					POINTS point = MAKEPOINTS(lParam);
					CPoint offset = GetClientBoundsInScreen().TopLeft();
					info.pt.x = point.x - offset.x;
					info.pt.y = point.y - offset.y;
				}
				else
				{
					info.ctrl = (wParam & MK_CONTROL) != 0;
					info.shift = (wParam & MK_SHIFT) != 0;
					info.left = (wParam & MK_LBUTTON) != 0;
					info.middle = (wParam & MK_MBUTTON) != 0;
					info.right = (wParam & MK_RBUTTON) != 0;

					POINTS point = MAKEPOINTS(lParam);

					if (wheelMessage)
					{
						CPoint offset = GetClientBoundsInScreen().TopLeft();
						info.pt.x = point.x - offset.x;
						info.pt.y = point.y - offset.y;
					}
					else
					{
						info.pt.x = point.x;
						info.pt.y = point.y;
					}
				}
				return info;
			}

			KeyInfo Form::ConvertKey(WPARAM wParam, LPARAM lParam)
			{
				KeyInfo info;
				info.code = wParam;
				info.ctrl = IsKeyPressing(VK_CONTROL);
				info.shift = IsKeyPressing(VK_SHIFT);
				info.alt = IsKeyPressing(VK_MENU);
				info.capslock = IsKeyToggled(VK_CAPITAL);
				return info;
			}

			CharInfo Form::ConvertChar(WPARAM wParam)
			{
				CharInfo info;
				info.code = (wchar_t)wParam;
				info.ctrl = IsKeyPressing(VK_CONTROL);
				info.shift = IsKeyPressing(VK_SHIFT);
				info.alt = IsKeyPressing(VK_MENU);
				info.capslock = IsKeyToggled(VK_CAPITAL);
				return info;
			}

			void Form::TrackMouse(bool enable)
			{
				TRACKMOUSEEVENT trackMouseEvent;
				trackMouseEvent.cbSize = sizeof(trackMouseEvent);
				trackMouseEvent.hwndTrack = handle;
				trackMouseEvent.dwFlags = (enable ? 0 : TME_CANCEL) | TME_HOVER | TME_LEAVE;
				trackMouseEvent.dwHoverTime = HOVER_DEFAULT;
				TrackMouseEvent(&trackMouseEvent);
			}

			void Form::UpdateCompositionForContent()
			{
				HIMC imc = ImmGetContext(handle);
				COMPOSITIONFORM cf;
				cf.dwStyle = CFS_POINT;
				cf.ptCurrentPos.x = (int)caretPoint.x;
				cf.ptCurrentPos.y = (int)caretPoint.y;
				ImmSetCompositionWindow(imc, &cf);
				ImmReleaseContext(handle, imc);
			}

			bool Form::HandleMessageInternal(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& result)
			{
				bool transferFocusEvent = false;
				bool nonClient = false;

				switch (uMsg)
				{
					case WM_LBUTTONDOWN:
					case WM_LBUTTONUP:
					case WM_LBUTTONDBLCLK:
					case WM_RBUTTONDOWN:
					case WM_RBUTTONUP:
					case WM_RBUTTONDBLCLK:
					case WM_MBUTTONDOWN:
					case WM_MBUTTONUP:
					case WM_MBUTTONDBLCLK:
						transferFocusEvent = true;
				}
				switch (uMsg)
				{
					// ************************************** moving and sizing
					case WM_MOVING:
					case WM_SIZING:
					{
						LPRECT rawBounds = (LPRECT)lParam;
						CRect bounds(rawBounds);
						for (auto & listener : listeners)
						{
							listener->Moving(bounds, true);
						}
						if (!bounds.EqualRect(rawBounds))
						{
							*rawBounds = bounds;
							result = TRUE;
						}
					}
						break;
					case WM_MOVE:
					case WM_SIZE:
					{
						for (auto & listener : listeners)
						{
							listener->Moved();
						}
					}
						break;
						// ************************************** state
					case WM_ENABLE:
					{
						for (auto & listener : listeners)
						{
							if (wParam == TRUE)
							{
								listener->Enabled();
							}
							else
							{
								listener->Disabled();
							}
						}
					}
						break;
					case WM_SETFOCUS:
					{
						for (auto & listener : listeners)
						{
							listener->GotFocus();
						}
					}
						break;
					case WM_KILLFOCUS:
					{
						for (auto & listener : listeners)
						{
							listener->LostFocus();
						}
					}
						break;
					case WM_ACTIVATE:
					{
						for (auto & listener : listeners)
						{
							if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
							{
								listener->Activated();
							}
							else
							{
								listener->Deactivated();
							}
						}
					}
						break;
					case WM_SHOWWINDOW:
					{
						if (wParam == TRUE)
						{
							for (auto & listener : listeners)
							{
								listener->Opened();
							}
						}
						else
						{
							for (auto & listener : listeners)
							{
								listener->Closed();
							}
						}
					}
						break;
					case WM_CLOSE:
					{
						bool cancel = false;
						for (auto & listener : listeners)
						{
							listener->Closing(cancel);
						}
						return cancel;
					}
						break;
						// ************************************** mouse
					case WM_NCLBUTTONDOWN:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_LBUTTONDOWN:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						for (auto & listener : listeners)
						{
							listener->LeftButtonDown(info);
						}
					}
						break;
					case WM_NCLBUTTONUP:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_LBUTTONUP:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						for (auto & listener : listeners)
						{
							listener->LeftButtonUp(info);
						}
					}
						break;
					case WM_NCLBUTTONDBLCLK:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_LBUTTONDBLCLK:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						for (auto & listener : listeners)
						{
							listener->LeftButtonDoubleClick(info);
						}
					}
						break;
					case WM_NCRBUTTONDOWN:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_RBUTTONDOWN:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						for (auto & listener : listeners)
						{
							listener->RightButtonDown(info);
						}
					}
						break;
					case WM_NCRBUTTONUP:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_RBUTTONUP:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						for (auto & listener : listeners)
						{
							listener->RightButtonUp(info);
						}
					}
						break;
					case WM_NCRBUTTONDBLCLK:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_RBUTTONDBLCLK:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						for (auto & listener : listeners)
						{
							listener->RightButtonDoubleClick(info);
						}
					}
						break;
					case WM_NCMBUTTONDOWN:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_MBUTTONDOWN:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						for (auto & listener : listeners)
						{
							listener->MiddleButtonDown(info);
						}
					}
						break;
					case WM_NCMBUTTONUP:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_MBUTTONUP:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						for (auto & listener : listeners)
						{
							listener->MiddleButtonUp(info);
						}
					}
						break;
					case WM_NCMBUTTONDBLCLK:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_MBUTTONDBLCLK:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						for (auto & listener : listeners)
						{
							listener->MiddleButtonDoubleClick(info);
						}
					}
						break;
					case WM_NCMOUSEMOVE:
						if (!customFrameMode) break;
						nonClient = true;
					case WM_MOUSEMOVE:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, false, nonClient);
						if (info.pt != mouseLast)
						{
							if (!mouseHoving)
							{
								mouseHoving = true;
								for (auto & listener : listeners)
								{
									listener->MouseEntered();
								}
								TrackMouse(true);
							}
							for (auto & listener : listeners)
							{
								listener->MouseMoving(info);
							}
						}
					}
						break;
						// ************************************** wheel
					case WM_MOUSEHWHEEL:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, true, false);
						for (auto & listener : listeners)
						{
							listener->HorizontalWheel(info);
						}
					}
						break;
					case WM_MOUSEWHEEL:
					{
						MouseInfo info = ConvertMouse(wParam, lParam, true, false);
						for (auto & listener : listeners)
						{
							listener->VerticalWheel(info);
						}
					}
						break;
						// ************************************** mouse state
					case WM_NCMOUSELEAVE:
						nonClient = true;
					case WM_MOUSELEAVE:
						if (customFrameMode == nonClient)
						{
							mouseLast.x = -1;
							mouseLast.y = -1;
							mouseHoving = false;
							for (auto & listener : listeners)
							{
								listener->MouseLeaved();
							}
						}
						break;
					case WM_NCMOUSEHOVER:
					case WM_MOUSEHOVER:
					{
						TrackMouse(true);
					}
						break;
						// ************************************** key
					case WM_KEYUP:
					{
						KeyInfo info = ConvertKey(wParam, lParam);
						for (auto & listener : listeners)
						{
							listener->KeyUp(info);
						}
					}
						break;
					case WM_KEYDOWN:
					{
						KeyInfo info = ConvertKey(wParam, lParam);
						for (auto & listener : listeners)
						{
							listener->KeyDown(info);
						}
					}
						break;
					case WM_SYSKEYUP:
					{
						KeyInfo info = ConvertKey(wParam, lParam);
						for (auto & listener : listeners)
						{
							listener->SysKeyUp(info);
						}
					}
						break;
					case WM_SYSKEYDOWN:
					{
						KeyInfo info = ConvertKey(wParam, lParam);
						for (auto & listener : listeners)
						{
							listener->SysKeyDown(info);
						}
					}
						break;
					case WM_CHAR:
					{
						CharInfo info = ConvertChar(wParam);
						for (auto & listener : listeners)
						{
							listener->Char(info);
						}
					}
						break;
						// ************************************** painting
					case WM_PAINT:
					{
						for (auto & listener : listeners)
						{
							listener->Paint();
						}
					}
						break;
					case WM_ERASEBKGND:
						result = 0;
						return true;
					case WM_NCPAINT:
					case WM_SYNCPAINT:
						if (customFrameMode)
						{
							result = 0;
							return true;
						}
						break;
						// ************************************** IME
					case WM_IME_SETCONTEXT:
						if (wParam == TRUE)
						{
							HIMC imc = ImmGetContext(handle);
							ImmAssociateContext(hwnd, imc);
							ImmReleaseContext(handle, imc);
						}
						break;
					case WM_IME_STARTCOMPOSITION:
						UpdateCompositionForContent();
						break;
						// ************************************** hit test
					case WM_NCHITTEST:
					{
						POINTS location = MAKEPOINTS(lParam);
						CPoint windowLocation = GetBounds().TopLeft();
						location.x -= (SHORT)windowLocation.x;
						location.y -= (SHORT)windowLocation.y;
						for (auto & listener : listeners)
						{
							switch (listener->HitTest(CPoint(location.x, location.y)))
							{
								case IWindowListener::BorderNoSizing:
									result = HTBORDER;
									return true;
								case IWindowListener::BorderLeft:
									result = HTLEFT;
									return true;
								case IWindowListener::BorderRight:
									result = HTRIGHT;
									return true;
								case IWindowListener::BorderTop:
									result = HTTOP;
									return true;
								case IWindowListener::BorderBottom:
									result = HTBOTTOM;
									return true;
								case IWindowListener::BorderLeftTop:
									result = HTTOPLEFT;
									return true;
								case IWindowListener::BorderRightTop:
									result = HTTOPRIGHT;
									return true;
								case IWindowListener::BorderLeftBottom:
									result = HTBOTTOMLEFT;
									return true;
								case IWindowListener::BorderRightBottom:
									result = HTBOTTOMRIGHT;
									return true;
								case IWindowListener::Title:
									result = HTCAPTION;
									return true;
								case IWindowListener::ButtonMinimum:
									result = HTMINBUTTON;
									return true;
								case IWindowListener::ButtonMaximum:
									result = HTMAXBUTTON;
									return true;
								case IWindowListener::ButtonClose:
									result = HTCLOSE;
									return true;
								case IWindowListener::Client:
									result = HTCLIENT;
									return true;
								case IWindowListener::Icon:
									result = HTSYSMENU;
									return true;
							}
						}
					}
						break;
						// ************************************** MISC
					case WM_SETCURSOR:
					{
						DWORD hitTestResult = LOWORD(lParam);
						if (hitTestResult == HTCLIENT)
						{
							HCURSOR cursorHandle = cursor->GetCursorHandle();
							if (::GetCursor() != cursorHandle)
							{
								::SetCursor(cursorHandle);
							}
							result = TRUE;
							return true;
						}
					}
						break;
					case WM_NCCALCSIZE:
						if ((BOOL)wParam && customFrameMode)
						{
							result = 0;
							return true;
						}
						break;
					case WM_NCACTIVATE:
						if (customFrameMode)
						{
							if (wParam == TRUE)
							{
								result = FALSE;
							}
							else
							{
								result = TRUE;
							}
							return true;
						}
						break;
					case WM_MOUSEACTIVATE:
						if (noActivate)
						{
							result = MA_NOACTIVATEANDEAT;
							return true;
						}
						break;
				}

				if (IsWindow(hwnd) != 0 && !GetClassStyle(CS_DROPSHADOW) && !noActivate)
				{
					if (transferFocusEvent && IsFocused())
					{
						auto window = this;
						while (window->parent && window->alwaysPassFocusToParent)
						{
							window = window->parent;
						}
						if (window != this)
						{
							window->SetFocus();
						}
					}
				}

				if (customFrameMode)
				{
					switch (uMsg)
					{
						case WM_NCLBUTTONDOWN:
							switch (wParam)
							{
								case HTMINBUTTON:
								case HTMAXBUTTON:
								case HTCLOSE:
									result = 0;
									return true;
							}
							break;
						case WM_LBUTTONUP:
						{
							POINTS location = MAKEPOINTS(lParam);
							for (auto & listener : listeners)
							{
								switch (listener->HitTest(CPoint(location.x, location.y)))
								{
									case IWindowListener::ButtonMinimum:
										ShowMinimized();
										return false;
									case IWindowListener::ButtonMaximum:
										if (GetSizeState() == IWindow::Maximized)
										{
											ShowRestored();
										}
										else
										{
											ShowMaximized();
										}
										return false;
									case IWindowListener::ButtonClose:
										Hide();
										return false;
								}
							}
						}
							break;
					}
				}
				return false;
			}

			Form::Form(HWND parent, CString className, HINSTANCE hInstance)
				: alwaysPassFocusToParent(false)
				, mouseLast(-1, -1)
				, mouseHoving(false)
				, customFrameMode(false)
				, noActivate(false)
			{
				DWORD exStyle = WS_EX_APPWINDOW | WS_EX_CONTROLPARENT;
				DWORD style = WS_BORDER | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
				handle = CreateWindowEx(exStyle, className, NULL, style,
					CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
					parent, NULL, hInstance, NULL);
			}

			Form::~Form()
			{
				for (auto & listener : listeners)
				{
					listener->Destroyed();
				}
				listeners.clear();
				DestroyWindow(handle);
			}

			void Form::InvokeDestroying()
			{
				for (cuint i = 0; i < listeners.size(); i++)
				{
					listeners[i]->Destroying();
				}
			}

			bool Form::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& result)
			{
				bool skip = false;
				{
					for (auto & handler : messageHandlers)
					{
						handler->BeforeHandle(hwnd, uMsg, wParam, lParam, skip);
					}
					if (skip)
					{
						return true;
					}
				}
				skip = HandleMessageInternal(hwnd, uMsg, wParam, lParam, result);
				if (GetFormFromHandle(hwnd))
				{
					for (auto & handler : messageHandlers)
					{
						handler->AfterHandle(hwnd, uMsg, wParam, lParam, skip, result);
					}
				}
				return skip;
			}

			HWND Form::GetWindowHandle()
			{
				return handle;
			}

			PassRefPtr<IGraphicsRenderTarget> Form::GetGraphicsHandler()
			{
				return graphicsHandler;
			}

			void Form::SetGraphicsHandler(PassRefPtr<IGraphicsRenderTarget> handler)
			{
				graphicsHandler = handler;
			}

			bool Form::InstallMessageHandler(PassRefPtr<IMessageHandler> handler)
			{
				return messageHandlers.insert(handler).second;
			}

			bool Form::UninstallMessageHandler(PassRefPtr<IMessageHandler> handler)
			{
				return messageHandlers.erase(handler) == 1;
			}

			CRect Form::GetBounds()
			{
				CRect rect;
				GetWindowRect(handle, &rect);
				return rect;
			}

			void Form::SetBounds(const CRect& bounds)
			{
				CRect newBounds = bounds;
				for (auto & listener : listeners)
				{
					listener->Moving(newBounds, true);
				}
				MoveWindow(handle, (int)newBounds.left, (int)newBounds.top, (int)newBounds.Width(), (int)newBounds.Height(), FALSE);
			}

			CSize Form::GetClientSize()
			{
				return GetClientBoundsInScreen().Size();
			}

			void Form::SetClientSize(CSize size)
			{
				CRect required(CPoint(), size);
				CRect bounds;
				GetWindowRect(handle, &bounds);
				AdjustWindowRect(&required, (DWORD)GetWindowLongPtr(handle, GWL_STYLE), FALSE);
				SetBounds(CRect(bounds.TopLeft(), required.Size()));
			}

			CRect Form::GetClientBoundsInScreen()
			{
				if (customFrameMode)
				{
					return GetBounds();
				}
				else
				{
					CRect required;
					CRect bounds;
					GetWindowRect(handle, &bounds);
					AdjustWindowRect(&required, (DWORD)GetWindowLongPtr(handle, GWL_STYLE), FALSE);
					return CRect(bounds.TopLeft() + (-required.TopLeft()),
						bounds.Size() - required.Size());
				}
			}

			CString Form::GetTitle()
			{
				return title;
			}

			void Form::SetTitle(CString _title)
			{
				title = _title;
				SetWindowText(handle, title);
			}

			PassRefPtr<ICursor> Form::GetCursor()
			{
				return cursor;
			}

			void Form::SetCursor(PassRefPtr<ICursor> _cursor)
			{
				RefPtr<Cursor> newCursor = _cursor;
				if (newCursor && cursor != newCursor)
				{
					cursor = newCursor;
					if (mouseHoving && IsVisible())
					{
						::SetCursor(cursor->GetCursorHandle());
					}
				}
			}

			CPoint Form::GetCaretPoint()
			{
				return caretPoint;
			}

			void Form::SetCaretPoint(CPoint CPoint)
			{
				caretPoint = CPoint;
				UpdateCompositionForContent();
			}

			PassRefPtr<IWindow> Form::GetParent()
			{
				return parent;
			}

			void Form::SetParent(PassRefPtr<IWindow> _parent)
			{
				RefPtr<Form> parent = _parent;
				if (parent)
				{
					SetWindowLongPtr(handle, GWLP_HWNDPARENT, (LONG_PTR)parent->handle);
				}
				else
				{
					SetWindowLongPtr(handle, GWLP_HWNDPARENT, NULL);
				}
			}

			bool Form::GetAlwaysPassFocusToParent()
			{
				return alwaysPassFocusToParent;
			}

			void Form::SetAlwaysPassFocusToParent(bool value)
			{
				alwaysPassFocusToParent = value;
			}

			void Form::EnableCustomFrameMode()
			{
				customFrameMode = true;
			}

			void Form::DisableCustomFrameMode()
			{
				customFrameMode = false;
			}

			bool Form::IsCustomFrameModeEnabled()
			{
				return customFrameMode;
			}

			Form::WindowSizeState Form::GetSizeState()
			{
				return IsIconic(handle) ? IWindow::Minimized :
					IsZoomed(handle) ? IWindow::Maximized : IWindow::Restored;
			}

			void Form::SetNoActivate(bool value)
			{
				noActivate = value;
			}

			void Form::Show()
			{
				ShowWindow(handle, SW_SHOWNORMAL);
			}

			void Form::Show(int nCmdShow)
			{
				ShowWindow(handle, nCmdShow);
			}

			void Form::ShowDeactivated()
			{
				ShowWindow(handle, SW_SHOWNA);
			}

			void Form::ShowRestored()
			{
				ShowWindow(handle, SW_RESTORE);
			}

			void Form::ShowMaximized()
			{
				ShowWindow(handle, SW_SHOWMAXIMIZED);
			}

			void Form::ShowMinimized()
			{
				ShowWindow(handle, SW_SHOWMINIMIZED);
			}

			void Form::Hide()
			{
				PostMessage(handle, WM_CLOSE, NULL, NULL);
			}

			bool Form::IsVisible()
			{
				return IsWindowVisible(handle) != 0;
			}

			void Form::Enable()
			{
				EnableWindow(handle, TRUE);
			}

			void Form::Disable()
			{
				EnableWindow(handle, FALSE);
			}

			bool Form::IsEnabled()
			{
				return IsWindowEnabled(handle) != 0;
			}

			void Form::SetFocus()
			{
				::SetFocus(handle);
			}

			bool Form::IsFocused()
			{
				return GetFocus() == handle;
			}

			void Form::SetActivate()
			{
				SetActiveWindow(handle);
			}

			bool Form::IsActivated()
			{
				return GetActiveWindow() == handle;
			}

			void Form::ShowInTaskBar()
			{
				SetExStyle(WS_EX_APPWINDOW, true);
			}

			void Form::HideInTaskBar()
			{
				SetExStyle(WS_EX_APPWINDOW, false);
			}

			bool Form::IsAppearedInTaskBar()
			{
				return GetExStyle(WS_EX_APPWINDOW);
			}

			void Form::EnableActivate()
			{
				SetExStyle(WS_EX_NOACTIVATE, false);
			}

			void Form::DisableActivate()
			{
				SetExStyle(WS_EX_NOACTIVATE, true);
			}

			bool Form::IsEnabledActivate()
			{
				return !GetExStyle(WS_EX_NOACTIVATE);
			}

			bool Form::RequireCapture()
			{
				SetCapture(handle);
				return true;
			}

			bool Form::ReleaseCapture()
			{
				::ReleaseCapture();
				return true;
			}

			bool Form::IsCapturing()
			{
				return GetCapture() == handle;
			}

			bool Form::GetMaximizedBox()
			{
				return GetStyle(WS_MAXIMIZEBOX);
			}

			void Form::SetMaximizedBox(bool visible)
			{
				SetStyle(WS_MAXIMIZEBOX, visible);
			}

			bool Form::GetMinimizedBox()
			{
				return GetStyle(WS_MINIMIZEBOX);
			}

			void Form::SetMinimizedBox(bool visible)
			{
				SetStyle(WS_MINIMIZEBOX, visible);
			}

			bool Form::GetBorder()
			{
				return GetStyle(WS_BORDER);
			}

			void Form::SetBorder(bool visible)
			{
				SetStyle(WS_BORDER, visible);
			}

			bool Form::GetSizeBox()
			{
				return GetStyle(WS_SIZEBOX);
			}

			void Form::SetSizeBox(bool visible)
			{
				SetStyle(WS_SIZEBOX, visible);
			}

			bool Form::GetIconVisible()
			{
				return GetStyle(WS_SYSMENU);
			}

			void Form::SetIconVisible(bool visible)
			{
				SetStyle(WS_SYSMENU, visible);
			}

			bool Form::GetTitleBar()
			{
				return GetStyle(WS_CAPTION);
			}

			void Form::SetTitleBar(bool visible)
			{
				SetStyle(WS_CAPTION, visible);
			}

			bool Form::GetTopMost()
			{
				return GetExStyle(WS_EX_TOPMOST);
			}

			void Form::SetTopMost(bool topmost)
			{
				SetWindowPos(handle, (topmost ? HWND_TOPMOST : HWND_NOTOPMOST), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
			}

			void Form::RedrawContent()
			{
				if (graphicsHandler)
				{
					PostMessage(handle, WM_PAINT, NULL, NULL);
				}
			}

			bool Form::InstallListener(PassRefPtr<IWindowListener> listener)
			{
				auto found = find(listeners.begin(), listeners.end(), listener);
				if (found == listeners.end())
				{
					listeners.push_back(listener);
					return true;
				}
				else
				{
					return false;
				}
			}

			bool Form::UninstallListener(PassRefPtr<IWindowListener> listener)
			{
				auto found = find(listeners.begin(), listeners.end(), listener);
				if (found == listeners.end())
				{
					return false;
				}
				else
				{
					listeners.erase(found);
					return true;
				}
			}
		}
	}
}
