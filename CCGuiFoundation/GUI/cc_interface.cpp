#include "stdafx.h"
#include "cc_interface.h"

namespace cc
{
	namespace interfaces
	{
		namespace windows
		{
			IWindowListener::HitTestResult IWindowListener::HitTest(CPoint location)
			{
				return NoDecision;
			}

			void IWindowListener::Moving(CRect& bounds, bool fixSizeOnly)
			{

			}

			void IWindowListener::Moved()
			{

			}

			void IWindowListener::Enabled()
			{

			}

			void IWindowListener::Disabled()
			{

			}

			void IWindowListener::GotFocus()
			{

			}

			void IWindowListener::LostFocus()
			{

			}

			void IWindowListener::Activated()
			{

			}

			void IWindowListener::Deactivated()
			{

			}

			void IWindowListener::Opened()
			{

			}

			void IWindowListener::Closing(bool& cancel)
			{

			}

			void IWindowListener::Closed()
			{

			}

			void IWindowListener::Paint()
			{

			}

			void IWindowListener::Destroying()
			{

			}

			void IWindowListener::Destroyed()
			{

			}

			void IWindowListener::LeftButtonDown(const MouseInfo& info)
			{

			}

			void IWindowListener::LeftButtonUp(const MouseInfo& info)
			{

			}

			void IWindowListener::LeftButtonDoubleClick(const MouseInfo& info)
			{

			}

			void IWindowListener::RightButtonDown(const MouseInfo& info)
			{

			}

			void IWindowListener::RightButtonUp(const MouseInfo& info)
			{

			}

			void IWindowListener::RightButtonDoubleClick(const MouseInfo& info)
			{

			}

			void IWindowListener::MiddleButtonDown(const MouseInfo& info)
			{

			}

			void IWindowListener::MiddleButtonUp(const MouseInfo& info)
			{

			}

			void IWindowListener::MiddleButtonDoubleClick(const MouseInfo& info)
			{

			}

			void IWindowListener::HorizontalWheel(const MouseInfo& info)
			{

			}

			void IWindowListener::VerticalWheel(const MouseInfo& info)
			{

			}

			void IWindowListener::MouseMoving(const MouseInfo& info)
			{

			}

			void IWindowListener::MouseEntered()
			{

			}

			void IWindowListener::MouseLeaved()
			{

			}

			void IWindowListener::KeyDown(const KeyInfo& info)
			{

			}

			void IWindowListener::KeyUp(const KeyInfo& info)
			{

			}

			void IWindowListener::SysKeyDown(const KeyInfo& info)
			{

			}

			void IWindowListener::SysKeyUp(const KeyInfo& info)
			{

			}

			void IWindowListener::Char(const CharInfo& info)
			{

			}

			void IControllerListener::LeftButtonDown(CPoint position)
			{

			}

			void IControllerListener::LeftButtonUp(CPoint position)
			{

			}

			void IControllerListener::RightButtonDown(CPoint position)
			{

			}

			void IControllerListener::RightButtonUp(CPoint position)
			{

			}

			void IControllerListener::MouseMoving(CPoint position)
			{

			}

			void IControllerListener::GlobalTimer()
			{

			}

			void IControllerListener::ClipboardUpdated()
			{

			}

			void IControllerListener::WindowCreated(PassRefPtr<IWindow> window)
			{

			}

			void IControllerListener::WindowDestroying(PassRefPtr<IWindow> window)
			{

			}
		}
	}
}