#include "stdafx.h"
#include "cc_element.h"
#include "cc_control.h"
#include "cc_style.h"
#include <gdiplus.h>

using namespace cc::presentation::element;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			namespace style
			{
				DefaultBehaviorStyleController::DefaultBehaviorStyleController()
				{

				}

				DefaultBehaviorStyleController::~DefaultBehaviorStyleController()
				{

				}

				void DefaultBehaviorStyleController::AttachWindow(PassRefPtr<Window> _window)
				{
					window = _window;
				}

				void DefaultBehaviorStyleController::InitializeWindowProperties()
				{

				}

				void DefaultBehaviorStyleController::SetSizeState(IWindow::WindowSizeState value)
				{

				}

				bool DefaultBehaviorStyleController::GetMaximizedBox()
				{
					if (window->GetWindow())
					{
						return window->GetWindow()->GetMaximizedBox();
					}
					else
					{
						return false;
					}
				}

				void DefaultBehaviorStyleController::SetMaximizedBox(bool visible)
				{
					if (window->GetWindow())
					{
						window->GetWindow()->SetMaximizedBox(visible);
					}
				}

				bool DefaultBehaviorStyleController::GetMinimizedBox()
				{
					if (window->GetWindow())
					{
						return window->GetWindow()->GetMinimizedBox();
					}
					else
					{
						return false;
					}
				}

				void DefaultBehaviorStyleController::SetMinimizedBox(bool visible)
				{
					if (window->GetWindow())
					{
						window->GetWindow()->SetMinimizedBox(visible);
					}
				}

				bool DefaultBehaviorStyleController::GetBorder()
				{
					if (window->GetWindow())
					{
						return window->GetWindow()->GetBorder();
					}
					else
					{
						return false;
					}
				}

				void DefaultBehaviorStyleController::SetBorder(bool visible)
				{
					if (window->GetWindow())
					{
						window->GetWindow()->SetBorder(visible);
					}
				}

				bool DefaultBehaviorStyleController::GetSizeBox()
				{
					if (window->GetWindow())
					{
						return window->GetWindow()->GetSizeBox();
					}
					else
					{
						return false;
					}
				}

				void DefaultBehaviorStyleController::SetSizeBox(bool visible)
				{
					if (window->GetWindow())
					{
						window->GetWindow()->SetSizeBox(visible);
					}
				}

				bool DefaultBehaviorStyleController::GetIconVisible()
				{
					if (window->GetWindow())
					{
						return window->GetWindow()->GetIconVisible();
					}
					else
					{
						return false;
					}
				}

				void DefaultBehaviorStyleController::SetIconVisible(bool visible)
				{
					if (window->GetWindow())
					{
						window->GetWindow()->SetIconVisible(visible);
					}
				}

				bool DefaultBehaviorStyleController::GetTitleBar()
				{
					if (window->GetWindow())
					{
						return window->GetWindow()->GetTitleBar();
					}
					else
					{
						return false;
					}
				}

				void DefaultBehaviorStyleController::SetTitleBar(bool visible)
				{
					if (window->GetWindow())
					{
						window->GetWindow()->SetTitleBar(visible);
					}
				}

				PassRefPtr<IWindowStyleController> DefaultBehaviorStyleController::CreateTooltipStyle()
				{
					return nullptr;
				}

				bool DefaultBehaviorStyleController::ShowShadow()
				{
					return false;
				}

				PassRefPtr<IWindowStyleController> Win8Style::CreateWindowStyle()
				{
					return adoptRef(new Win8WindowStyle);
				}

				PassRefPtr<IWindowStyleController> Win8Style::CreateTooltipStyle()
				{
					return adoptRef(new Win8TooltipStyle);
				}

				PassRefPtr<IControlStyleController> Win8Style::CreateCustomControlStyle()
				{
					return adoptRef(new EmptyStyleController);
				}

				Win8WindowStyle::Win8WindowStyle()
				{
					RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
					element->SetColor(GetSystemColor(CT_Window));

					boundsComposition = adoptRef(new BoundsComposition);
					boundsComposition->SetOwnedElement(element);
				}

				Win8WindowStyle::~Win8WindowStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8WindowStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8WindowStyle::GetContainerComposition()
				{
					return boundsComposition;
				}

				void Win8WindowStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8WindowStyle::SetText(const CString& value)
				{

				}

				void Win8WindowStyle::SetFont(const Font& value)
				{

				}

				void Win8WindowStyle::SetVisuallyEnabled(bool value)
				{

				}

				CColor Win8WindowStyle::GetSystemColor(ColorType type)
				{
					switch (type)
					{
						case CT_Window:
							return Gdiplus::Color::WhiteSmoke;
						case CT_Tooltip:
							return CColor(241, 242, 247);
						case CT_Text_Enable:
							return CColor(27, 41, 62);
						case CT_Text_Disable:
							return CColor(131, 131, 131);
						case CT_Border:
							return CColor(171, 173, 179);
						case CT_Menu_Border:
							return CColor(151, 151, 151);
						case CT_Menu_Splitter:
							return CColor(215, 215, 215);
						case CT_Tab:
							return Gdiplus::Color::WhiteSmoke;
						default:
							return Gdiplus::Color::White;
					}
				}

				ColorEntry Win8WindowStyle::GetTextColor()
				{
					ColorEntry entry;
					entry.normal.text = Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable);
					entry.normal.background = CColor(0, 0, 0, 0);
					entry.selectedFocused.text = Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable);
					entry.selectedFocused.background = CColor(173, 214, 255);
					entry.selectedUnfocused.text = Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable);
					entry.selectedUnfocused.background = CColor(229, 235, 241);
					return entry;
				}

				EmptyStyleController::EmptyStyleController()
				{
					boundsComposition = adoptRef(new BoundsComposition);
				}

				EmptyStyleController::~EmptyStyleController()
				{

				}

				PassRefPtr<BoundsComposition> EmptyStyleController::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> EmptyStyleController::GetContainerComposition()
				{
					return boundsComposition;
				}

				void EmptyStyleController::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void EmptyStyleController::SetText(const CString& value)
				{

				}

				void EmptyStyleController::SetFont(const Font& value)
				{

				}

				void EmptyStyleController::SetVisuallyEnabled(bool value)
				{

				}

				Win8TooltipStyle::Win8TooltipStyle()
				{
					boundsComposition = adoptRef(new BoundsComposition);
					boundsComposition->SetAlignmentToParent(CRect(0, 0, 0, 0));
					boundsComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					{
						RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Border));
						boundsComposition->SetOwnedElement(element);
					}

					containerComposition = adoptRef(new BoundsComposition);
					containerComposition->SetAlignmentToParent(CRect(1, 1, 1, 1));
					containerComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					boundsComposition->AddChild(containerComposition);
					{
						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Tooltip));
						containerComposition->SetOwnedElement(element);
					}
				}

				Win8TooltipStyle::~Win8TooltipStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8TooltipStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8TooltipStyle::GetContainerComposition()
				{
					return containerComposition;
				}

				void Win8TooltipStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8TooltipStyle::SetText(const CString& value)
				{

				}

				void Win8TooltipStyle::SetFont(const Font& value)
				{

				}

				void Win8TooltipStyle::SetVisuallyEnabled(bool value)
				{

				}

				bool Win8TooltipStyle::ShowShadow()
				{
					return true;
				}
			}
		}
	}
}