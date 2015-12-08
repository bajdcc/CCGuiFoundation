#ifndef CC_PRESENTATION
#define CC_PRESENTATION

#include "stdafx.h"
#include "cc_base.h"
#include "cc_debug.h"
#include "cc_interface.h"
#include "cc_resource.h"
#include "cc_direct2d.h"
#include "cc_control.h"

using namespace cc::debug;
using namespace cc::interfaces::windows;
using namespace cc::presentation::control;
using namespace cc::presentation::control::style;
using namespace cc::presentation::direct2d;
using namespace cc::presentation::direct2d::resource;

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			class WinDebug;

			class GlobalStorage
			{
			public:
				GlobalStorage();
				~GlobalStorage();

			public:
				void										SetDebug(WinDebug* debug);
				void										Init(PassRefPtr<IController>, PassRefPtr<IControllerListener>, PassRefPtr<D2DControllerListener>);
				void										Init2(PassRefPtr<Application>, PassRefPtr<ITheme>);
				void										Destroy();
				PassRefPtr<Application>						GetApplication();
				PassRefPtr<ITheme>							GetTheme();
				PassRefPtr<IController>						GetController();
				PassRefPtr<IControllerListener>				GetControllerListener();
				PassRefPtr<D2DControllerListener>			GetDirect2DControllerListener();
				PassRefPtr<GraphicsResourceManager>			GetGraphicsResourceManager();
				void										SetGraphicsResourceManager(PassRefPtr<GraphicsResourceManager> resourceManager);
				bool										RegisterFactories(PassRefPtr<IGraphicsElementFactory> elementFactory, PassRefPtr<IGraphicsRendererFactory> rendererFactory);
				PassRefPtr<D2DResourceManager>				GetDirect2DResourceManager();
				void										SetDirect2DResourceManager(PassRefPtr<D2DResourceManager> resourceManager);

				CComPtr<ID2D1RenderTarget>					GetDirect2DRenderTarget(PassRefPtr<IWindow> window);
				CComPtr<ID2D1Factory>						GetDirect2DFactory();
				CComPtr<IDWriteFactory>						GetDirectWriteFactory();

				PassRefPtr<ID2DProvider>					GetDirect2DProvider();
				void										SetDirect2DProvider(PassRefPtr<ID2DProvider> provider);

				HINSTANCE									GetInstance();
				static GlobalStorage*						GetStorage();

				WinDebug*									GetDebug();

			private:
				RefPtr<Application> application;
				RefPtr<ITheme> theme;
				RefPtr<IController> controller;
				RefPtr<IControllerListener> controllerListener;
				RefPtr<GraphicsResourceManager> graphicsResourceManager;
				RefPtr<D2DResourceManager> d2dResourceManager;
				RefPtr<ID2DProvider> d2dProvider;
				RefPtr<D2DControllerListener> controllerListenerD2D;
				WinDebug* winDebug;
			};

			GlobalStorage* GetStorage();

			class WinClass
			{
			protected:
				CString									name;
				WNDCLASSEX								windowClass;
				ATOM									windowAtom;

			public:
				WinClass(CString _name, bool shadow, bool ownDC, WNDPROC procedure, HINSTANCE hInstance);

				bool IsAvailable() const;
				CString GetName() const;
				ATOM GetClassAtom() const;
			};

			class WinMessageLoop
			{
			public:
				WinMessageLoop();

				void Run();
				BOOL PumpMessage();
				BOOL IsIdleMessage(MSG* pMsg);
				BOOL OnIdle(LONG lCount);

			private:
				int     m_nDisablePumpCount;
				UINT    m_nMsgLast;
				MSG		m_msg;
				CPoint	m_ptMousePos;
			};

			class WinDebug
			{
			public:
				WinDebug();
				~WinDebug();

				LPCTSTR DebugGetMessageName(UINT message);
				void ReportDirect2DResource();

			private:
				auto_ptr<MSGMAP> debug_msgMap;
				TCHAR msgBuffer[80];
			};
		}
	}
}

#endif