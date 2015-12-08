#ifndef CC_FORM
#define CC_FORM

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_presentation.h"
#include "cc_resource.h"

using namespace cc::base;
using namespace cc::interfaces::windows;
using namespace cc::presentation::windows::resource;

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			class Form : public IWindow, public IForm
			{
			public:
				Form(HWND parent, CString className, HINSTANCE hInstance);
				~Form();

				void								InvokeDestroying();
				bool								HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& result);
				HWND								GetWindowHandle()override;
				PassRefPtr<IGraphicsRenderTarget>	GetGraphicsHandler()override;
				void								SetGraphicsHandler(PassRefPtr<IGraphicsRenderTarget> handler)override;

				bool								InstallMessageHandler(PassRefPtr<IMessageHandler> handler)override;
				bool								UninstallMessageHandler(PassRefPtr<IMessageHandler> handler)override;

				CRect								GetBounds();
				void								SetBounds(const CRect& bounds);
				CSize								GetClientSize();
				void								SetClientSize(CSize size);
				CRect								GetClientBoundsInScreen();
				CString								GetTitle();
				void								SetTitle(CString _title);
				PassRefPtr<ICursor>					GetCursor();
				void								SetCursor(PassRefPtr<ICursor> _cursor);
				CPoint								GetCaretPoint();
				void								SetCaretPoint(CPoint CPoint);
				PassRefPtr<IWindow>					GetParent();
				void								SetParent(PassRefPtr<IWindow> _parent);
				bool								GetAlwaysPassFocusToParent();
				void								SetAlwaysPassFocusToParent(bool value);
				void								EnableCustomFrameMode();
				void								DisableCustomFrameMode();
				bool								IsCustomFrameModeEnabled();
				WindowSizeState						GetSizeState();
				void								SetNoActivate(bool value);
				void								Show();
				void								Show(int nCmdShow);
				void								ShowDeactivated();
				void								ShowRestored();
				void								ShowMaximized();
				void								ShowMinimized();
				void								Hide();
				bool								IsVisible();
				void								Enable();
				void								Disable();
				bool								IsEnabled();
				void								SetFocus();
				bool								IsFocused();
				void								SetActivate();
				bool								IsActivated();
				void								ShowInTaskBar();
				void								HideInTaskBar();
				bool								IsAppearedInTaskBar();
				void								EnableActivate();
				void								DisableActivate();
				bool								IsEnabledActivate();
				bool								RequireCapture();
				bool								ReleaseCapture();
				bool								IsCapturing();
				bool								GetMaximizedBox();
				void								SetMaximizedBox(bool visible);
				bool								GetMinimizedBox();
				void								SetMinimizedBox(bool visible);
				bool								GetBorder();
				void								SetBorder(bool visible);
				bool								GetSizeBox();
				void								SetSizeBox(bool visible);
				bool								GetIconVisible();
				void								SetIconVisible(bool visible);
				bool								GetTitleBar();
				void								SetTitleBar(bool visible);
				bool								GetTopMost();
				void								SetTopMost(bool topmost);
				void								RedrawContent();
				bool								InstallListener(PassRefPtr<IWindowListener> listener);
				bool								UninstallListener(PassRefPtr<IWindowListener> listener);
				bool								GetExStyle(DWORD exStyle)override;
				void								SetExStyle(DWORD exStyle, bool available)override;
				bool								GetStyle(DWORD style)override;
				void								SetStyle(DWORD style, bool available)override;
				bool								GetClassStyle(DWORD style)override;
				void								SetClassStyle(DWORD style, bool available)override;

			protected:
				DWORD								InternalGetExStyle();
				void								InternalSetExStyle(DWORD exStyle);
				MouseInfo							ConvertMouse(WPARAM wParam, LPARAM lParam, bool wheelMessage, bool nonClient);
				KeyInfo								ConvertKey(WPARAM wParam, LPARAM lParam);
				CharInfo							ConvertChar(WPARAM wParam);
				void								TrackMouse(bool enable);
				void								UpdateCompositionForContent();
				bool								HandleMessageInternal(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& result);

			protected:
				HWND								handle;
				CString								title;
				RefPtr<Cursor>						cursor;
				CPoint								caretPoint;
				RawPtr<Form>						parent;
				bool								alwaysPassFocusToParent;
				vector<RawPtr<IWindowListener>>		listeners;
				CPoint								mouseLast;
				cint								mouseHoving;
				RefPtr<Interface>					graphicsHandler;
				bool								customFrameMode;
				set<RefPtr<IMessageHandler>>		messageHandlers;
				bool								noActivate;
			};
		}
	}
}

#endif