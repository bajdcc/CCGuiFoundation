#include "stdafx.h"
#include "cc_presentation.h"
#include "cc_controller.h"
#include "cc_direct2d.h"

void SetupD2D();

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			GlobalStorage::GlobalStorage()
				: winDebug(nullptr)
			{

			}

			GlobalStorage::~GlobalStorage()
			{

			}

			void GlobalStorage::Init(
				PassRefPtr<IController> controller,
				PassRefPtr<IControllerListener> contrllerListener,
				PassRefPtr<D2DControllerListener> controllerListenerD2D)
			{
				this->controller = controller;
				this->controllerListener = contrllerListener;
				this->controllerListenerD2D = controllerListenerD2D;				
			}
			
			void GlobalStorage::Init2(PassRefPtr<Application> application,
				PassRefPtr<ITheme> theme)
			{
				this->application = application;
				this->theme = theme;
			}

			void GlobalStorage::Destroy()
			{
				theme.clear();
				application.clear();

				d2dProvider.clear();
				d2dResourceManager.clear();

				graphicsResourceManager.clear();

				controllerListenerD2D.clear();
				controllerListener.clear();
				controller.clear();
			}

			PassRefPtr<Application> GlobalStorage::GetApplication()
			{
				return application;
			}

			PassRefPtr<ITheme> GlobalStorage::GetTheme()
			{
				return theme;
			}

			PassRefPtr<IController> GlobalStorage::GetController()
			{
				return controller;
			}

			PassRefPtr<IControllerListener> GlobalStorage::GetControllerListener()
			{
				return controllerListener;
			}

			PassRefPtr<D2DControllerListener> GlobalStorage::GetDirect2DControllerListener()
			{
				return controllerListenerD2D;
			}

			PassRefPtr<GraphicsResourceManager> GlobalStorage::GetGraphicsResourceManager()
			{
				return graphicsResourceManager;
			}

			void GlobalStorage::SetGraphicsResourceManager(PassRefPtr<GraphicsResourceManager> resourceManager)
			{
				graphicsResourceManager = resourceManager;
			}

			bool GlobalStorage::RegisterFactories(PassRefPtr<IGraphicsElementFactory> elementFactory, PassRefPtr<IGraphicsRendererFactory> rendererFactory)
			{
				RefPtr<IGraphicsElementFactory> _elementFactory = elementFactory;
				RefPtr<IGraphicsRendererFactory> _rendererFactory = rendererFactory;
				if (graphicsResourceManager && _elementFactory && _rendererFactory)
				{
					if (graphicsResourceManager->RegisterElementFactory(_elementFactory))
					{
						if (graphicsResourceManager->RegisterRendererFactory(_elementFactory->GetElementTypeName(), _rendererFactory))
						{
							return true;
						}
					}
				}
				return false;
			}

			PassRefPtr<D2DResourceManager> GlobalStorage::GetDirect2DResourceManager()
			{
				return d2dResourceManager;
			}

			void GlobalStorage::SetDirect2DResourceManager(PassRefPtr<D2DResourceManager> resourceManager)
			{
				d2dResourceManager = resourceManager;
			}

			HINSTANCE GlobalStorage::GetInstance()
			{
				return (HINSTANCE)GetModuleHandle(NULL);
			}

			GlobalStorage* GlobalStorage::GetStorage()
			{
				static GlobalStorage This;
				return &This;
			}

			CComPtr<ID2D1RenderTarget> GlobalStorage::GetDirect2DRenderTarget(PassRefPtr<IWindow> window)
			{
				auto found = controllerListenerD2D->WindowListeners.find(window);
				return found == controllerListenerD2D->WindowListeners.end() ? nullptr : found->second->GetDirect2DRenderTarget();
			}

			CComPtr<ID2D1Factory> GlobalStorage::GetDirect2DFactory()
			{
				return controllerListenerD2D->D2D1Factory;
			}

			CComPtr<IDWriteFactory> GlobalStorage::GetDirectWriteFactory()
			{
				return controllerListenerD2D->DWriteFactory;
			}

			PassRefPtr<ID2DProvider> GlobalStorage::GetDirect2DProvider()
			{
				return d2dProvider;
			}

			void GlobalStorage::SetDirect2DProvider(PassRefPtr<ID2DProvider> provider)
			{
				d2dProvider = provider;
			}

			WinDebug* GlobalStorage::GetDebug()
			{
				return winDebug;
			}

			void GlobalStorage::SetDebug(WinDebug* debug)
			{
				winDebug = debug;
			}

			GlobalStorage* GetStorage()
			{
				return GlobalStorage::GetStorage();
			}

			WinClass::WinClass(CString _name, bool shadow, bool ownDC, WNDPROC procedure, HINSTANCE hInstance)
			{
				name = _name;
				windowClass.cbSize = sizeof(windowClass);
				windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | (shadow ? CS_DROPSHADOW : 0) | (ownDC ? CS_OWNDC : 0);
				windowClass.lpfnWndProc = procedure;
				windowClass.cbClsExtra = 0;
				windowClass.cbWndExtra = 0;
				windowClass.hInstance = hInstance;
				windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
				windowClass.hCursor = NULL;
				windowClass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
				windowClass.lpszMenuName = NULL;
				windowClass.lpszClassName = name.GetBuffer();
				windowClass.hIconSm = NULL;
				windowAtom = ::RegisterClassEx(&windowClass);
			}

			bool WinClass::IsAvailable() const
			{
				return windowAtom != 0;
			}

			CString WinClass::GetName() const
			{
				return name;
			}

			ATOM WinClass::GetClassAtom() const
			{
				return windowAtom;
			}

			WinMessageLoop::WinMessageLoop()
				: m_nDisablePumpCount(0)
				, m_nMsgLast(WM_NULL)
			{

			}

			void WinMessageLoop::Run()
			{
				// for tracking the idle time state
				BOOL bIdle = TRUE;
				LONG lIdleCount = 0;

				// acquire and dispatch messages until a WM_QUIT message is received.
				for (;;)
				{
					// phase1: check to see if we can do idle work
					while (bIdle && !PeekMessage(&m_msg, NULL, NULL, NULL, PM_NOREMOVE))
					{
						// call OnIdle while in bIdle state
						if (!OnIdle(lIdleCount++))
							bIdle = FALSE; // assume "no idle" state
					}

					// phase2: pump messages while available
					do
					{
						// pump message, but quit on WM_QUIT
						if (!PumpMessage())
							return;

						// reset "no idle" state after pumping "normal" message
						if (IsIdleMessage(&m_msg))
						{
							bIdle = TRUE;
							lIdleCount = 0;
						}

					} while (PeekMessage(&m_msg, NULL, NULL, NULL, PM_NOREMOVE));
				}
			}

			BOOL WinMessageLoop::PumpMessage()
			{
				if (!GetMessage(&m_msg, NULL, NULL, NULL))
				{
					m_nDisablePumpCount++; // application must die
					// Note: prevents calling message loop things in 'ExitInstance'
					// will never be decremented
					return FALSE;
				}

				if (m_nDisablePumpCount != 0)
				{
					ATLTRACE("Error: PumpMessage called when not permitted.");
					ASSERT(FALSE);
				}

				// process this message
				TranslateMessage(&m_msg);

#ifdef _DEBUG
				if (m_msg.message != WM_TIMER &&
					m_msg.message != WM_PAINT &&
					m_msg.message != WM_MOUSEMOVE &&
					m_msg.message != WM_NCMOUSEMOVE)
				{
					ATLTRACE(atlTraceWindowing, 0, "hwnd: 0x%08x message: %-30S[0x%04x] {W:0x%08X,L:0x%08X}\n", m_msg.hwnd,
						GetStorage()->GetDebug()->DebugGetMessageName(m_msg.message), m_msg.message, m_msg.wParam, m_msg.lParam);
				}
#endif // _DEBUG

				DispatchMessage(&m_msg);
				GetStorage()->GetController()->GetAsyncService()->ExecuteAsyncTasks();

				return TRUE;
			}

			BOOL WinMessageLoop::IsIdleMessage(MSG* pMsg)
			{
				// Return FALSE if the message just dispatched should _not_
				// cause OnIdle to be run.  Messages which do not usually
				// affect the state of the user interface and happen very
				// often are checked for.

				// redundant WM_MOUSEMOVE and WM_NCMOUSEMOVE
				if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_NCMOUSEMOVE || pMsg->message == WM_MOUSEWHEEL)
				{
					// mouse move at same position as last mouse move?
					if (m_ptMousePos == pMsg->pt && pMsg->message == m_nMsgLast)
						return FALSE;

					m_ptMousePos = pMsg->pt;  // remember for next time
					m_nMsgLast = pMsg->message;
					return TRUE;
				}

				// WM_PAINT and WM_SYSTIMER (caret blink)
				return pMsg->message != WM_PAINT && pMsg->message != 0x0118;
			}

			BOOL WinMessageLoop::OnIdle(LONG lCount)
			{
				return lCount < 0;
			}

			WinDebug::WinDebug()
				: debug_msgMap(new MSGMAP)
			{
				Msg_Init(*debug_msgMap);
			}

			WinDebug::~WinDebug()
			{

			}

			LPCTSTR WinDebug::DebugGetMessageName(UINT message)
			{
				if (message >= 0xC000)
				{
					// Window message registered with 'RegisterWindowMessage'
					//  (actually a USER atom)
					if (GetClipboardFormatName(message, msgBuffer, sizeof(msgBuffer)* sizeof(TCHAR)))
						return msgBuffer;
				}
				auto found = debug_msgMap->find(message);
				if (found != debug_msgMap->end())
					return found->second;
				else
					return _T("UNKNOWN");
			}

			void WinDebug::ReportDirect2DResource()
			{
				HRESULT hr;
				HMODULE hDxgiDebug = GetModuleHandle(_T("Dxgidebug.dll"));
				if (!hDxgiDebug) return;
				typedef HRESULT(WINAPI *pfnDXGIGetDebugInterface)(REFIID riid, void **ppDebug);
				pfnDXGIGetDebugInterface _DXGIGetDebugInterface = (pfnDXGIGetDebugInterface)GetProcAddress(hDxgiDebug, "DXGIGetDebugInterface");
				if (!_DXGIGetDebugInterface) return;
				CComPtr<IDXGIDebug1> dxgiDebug;
				hr = _DXGIGetDebugInterface(__uuidof(IDXGIDebug1), (void**)&dxgiDebug);
				if (FAILED(hr))	return;
				hr = dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				if (FAILED(hr))	return;
			}
		}
	}
}
