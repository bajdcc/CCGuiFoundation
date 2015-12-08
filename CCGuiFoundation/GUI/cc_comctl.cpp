#include "stdafx.h"
#include "cc_presentation.h"
#include "cc_comctl.h"
#include "cc_label.h"
#include "cc_button.h"
#include "cc_textbox.h"
#include "cc_scroll.h"
#include "cc_toolstrip.h"
#include "cc_tab.h"
#include "cc_console.h"

using namespace cc::presentation::windows;
using namespace cc::presentation::element;

namespace cc
{
	namespace global
	{
		PassRefPtr<Window> ControlFactory::NewWindow()
		{
			return adoptRef(new Window(GetStorage()->GetTheme()->CreateWindowStyle()));
		}

		PassRefPtr<CustomControl> ControlFactory::NewCustomControl()
		{
			return adoptRef(new CustomControl(GetStorage()->GetTheme()->CreateCustomControlStyle()));
		}

		PassRefPtr<Label> ControlFactory::NewLabel()
		{
			return adoptRef(new Label(GetStorage()->GetTheme()->CreateLabelStyle()));
		}

		PassRefPtr<Button> ControlFactory::NewButton()
		{
			return adoptRef(new Button(GetStorage()->GetTheme()->CreateButtonStyle()));
		}

		PassRefPtr<SelectableButton> ControlFactory::NewCheckBox()
		{
			return adoptRef(new SelectableButton(GetStorage()->GetTheme()->CreateCheckBoxStyle()));
		}

		PassRefPtr<SelectableButton> ControlFactory::NewRadioButton()
		{
			return adoptRef(new SelectableButton(GetStorage()->GetTheme()->CreateRadioButtonStyle()));
		}

		PassRefPtr<Control> ControlFactory::NewGroupBox()
		{
			return adoptRef(new Control(GetStorage()->GetTheme()->CreateGroupBoxStyle()));
		}

		PassRefPtr<SinglelineTextBox> ControlFactory::NewSinglelineTextBox()
		{
			return adoptRef(new SinglelineTextBox(GetStorage()->GetTheme()->CreateTextBoxStyle()));
		}

		PassRefPtr<ScrollContainer> ControlFactory::NewScrollContainer()
		{
			return adoptRef(new ScrollContainer(GetStorage()->GetTheme()->CreateScrollContainerStyle()));
		}

		PassRefPtr<MultilineTextBox> ControlFactory::NewMultilineTextBox()
		{
			return adoptRef(new MultilineTextBox(GetStorage()->GetTheme()->CreateMultilineTextBoxStyle()));
		}

		PassRefPtr<ToolstripMenu> ControlFactory::NewMenu(PassRefPtr<Control> owner)
		{
			return adoptRef(new ToolstripMenu(GetStorage()->GetTheme()->CreateMenuStyle(), owner));
		}

		PassRefPtr<ToolstripMenuBar> ControlFactory::NewMenuBar()
		{
			return adoptRef(new ToolstripMenuBar(GetStorage()->GetTheme()->CreateMenuBarStyle()));
		}

		PassRefPtr<Control> ControlFactory::NewMenuSplitter()
		{
			return adoptRef(new Control(GetStorage()->GetTheme()->CreateMenuSplitterStyle()));
		}

		PassRefPtr<ToolstripButton> ControlFactory::NewMenuBarButton()
		{
			return adoptRef(new ToolstripButton(GetStorage()->GetTheme()->CreateMenuBarButtonStyle()));
		}

		PassRefPtr<ToolstripButton> ControlFactory::NewMenuItemButton()
		{
			return adoptRef(new ToolstripButton(GetStorage()->GetTheme()->CreateMenuItemButtonStyle()));
		}

		PassRefPtr<ToolstripToolBar> ControlFactory::NewToolBar()
		{
			return adoptRef(new ToolstripToolBar(GetStorage()->GetTheme()->CreateToolBarStyle()));
		}

		PassRefPtr<ToolstripButton> ControlFactory::NewToolBarButton()
		{
			return adoptRef(new ToolstripButton(GetStorage()->GetTheme()->CreateToolBarButtonStyle()));
		}

		PassRefPtr<ToolstripButton> ControlFactory::NewToolBarDropdownButton()
		{
			return adoptRef(new ToolstripButton(GetStorage()->GetTheme()->CreateToolBarDropdownButtonStyle()));
		}

		PassRefPtr<ToolstripButton> ControlFactory::NewToolBarSplitButton()
		{
			return adoptRef(new ToolstripButton(GetStorage()->GetTheme()->CreateToolBarSplitButtonStyle()));
		}

		PassRefPtr<Control> ControlFactory::NewToolBarSplitter()
		{
			return adoptRef(new Control(GetStorage()->GetTheme()->CreateToolBarSplitterStyle()));
		}

		PassRefPtr<Tab> ControlFactory::NewTab()
		{
			return adoptRef(new Tab(GetStorage()->GetTheme()->CreateTabStyle()));
		}

		PassRefPtr<Scroll> ControlFactory::NewHScroll()
		{
			return adoptRef(new Scroll(GetStorage()->GetTheme()->CreateHScrollStyle()));
		}

		PassRefPtr<Scroll> ControlFactory::NewVScroll()
		{
			return adoptRef(new Scroll(GetStorage()->GetTheme()->CreateVScrollStyle()));
		}

		PassRefPtr<Console> ControlFactory::NewConsole()
		{
			return adoptRef(new Console(GetStorage()->GetTheme()->CreateConsoleStyle()));
		}
	}

	namespace presentation
	{
		namespace control
		{
			namespace style
			{
				PassRefPtr<IScrollViewStyleProvider> Win8Style::CreateScrollContainerStyle()
				{
					return adoptRef(new Win8ScrollViewProvider);
				}

				GroupController::GroupController()
				{

				}

				GroupController::~GroupController()
				{
					while (!buttons.empty())
					{
						auto button = buttons.begin();
						(*button)->SetGroupController(nullptr);
					}
				}

				void GroupController::Attach(SelectableButton* button)
				{
					buttons.insert(button);
				}

				void GroupController::Detach(SelectableButton* button)
				{
					buttons.erase(button);
				}

				MutexGroupController::MutexGroupController()
					: suppress(false)
				{

				}

				MutexGroupController::~MutexGroupController()
				{

				}

				void MutexGroupController::OnSelectedChanged(PassRefPtr<SelectableButton> button)
				{
					if (!suppress)
					{
						suppress = true;
						for (auto & _button : buttons)
						{
							_button->SetSelected(_button == button);
						}
						suppress = false;
					}
				}
			}

			CustomControl::CustomControl(PassRefPtr<IControlStyleController> _styleController)
				: Control(_styleController)
			{

			}

			CustomControl::~CustomControl()
			{

			}

			PassRefPtr<BoundsComposition> FragmentBuilder::BuildDockedElementContainer(PassRefPtr<IGraphicsElement> element)
			{
				RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
				composition->SetOwnedElement(element);
				composition->SetMinSizeLimitation(Composition::LimitToElement);
				composition->SetAlignmentToParent(CRect(0, 0, 0, 0));
				return composition;
			}

			void FragmentBuilder::FillUpArrow(PassRefPtr<PolygonElement> element)
			{
				CPoint points[] = { CPoint(0, 3), CPoint(3, 0), CPoint(6, 3) };
				element->SetSize(CSize(7, 4));
				element->SetPoints(points, sizeof(points) / sizeof(*points));
			}

			void FragmentBuilder::FillDownArrow(PassRefPtr<PolygonElement> element)
			{
				CPoint points[] = { CPoint(0, 0), CPoint(3, 3), CPoint(6, 0) };
				element->SetSize(CSize(7, 4));
				element->SetPoints(points, sizeof(points) / sizeof(*points));
			}

			void FragmentBuilder::FillLeftArrow(PassRefPtr<PolygonElement> element)
			{
				CPoint points[] = { CPoint(3, 0), CPoint(0, 3), CPoint(3, 6) };
				element->SetSize(CSize(4, 7));
				element->SetPoints(points, sizeof(points) / sizeof(*points));
			}

			void FragmentBuilder::FillRightArrow(PassRefPtr<PolygonElement> element)
			{
				CPoint points[] = { CPoint(0, 0), CPoint(3, 3), CPoint(0, 6) };
				element->SetSize(CSize(4, 7));
				element->SetPoints(points, sizeof(points) / sizeof(*points));
			}

			PassRefPtr<PolygonElement> FragmentBuilder::BuildUpArrow()
			{
				RefPtr<PolygonElement> element = PolygonElement::Create();
				FillUpArrow(element);
				element->SetBorderColor(CColor(0, 0, 0));
				element->SetBackgroundColor(CColor(0, 0, 0));
				return element;
			}

			PassRefPtr<BoundsComposition> FragmentBuilder::BuildUpArrow(RefPtr<PolygonElement>& elementOut)
			{
				elementOut = BuildUpArrow();
				return BuildDockedElementContainer(elementOut);
			}

			PassRefPtr<PolygonElement> FragmentBuilder::BuildDownArrow()
			{
				RefPtr<PolygonElement> element = PolygonElement::Create();
				FillDownArrow(element);
				element->SetBorderColor(CColor(0, 0, 0));
				element->SetBackgroundColor(CColor(0, 0, 0));
				return element;
			}

			PassRefPtr<BoundsComposition> FragmentBuilder::BuildDownArrow(RefPtr<PolygonElement>& elementOut)
			{
				elementOut = BuildDownArrow();
				return BuildDockedElementContainer(elementOut);
			}

			PassRefPtr<PolygonElement> FragmentBuilder::BuildLeftArrow()
			{
				RefPtr<PolygonElement> element = PolygonElement::Create();
				FillLeftArrow(element);
				element->SetBorderColor(CColor(0, 0, 0));
				element->SetBackgroundColor(CColor(0, 0, 0));
				return element;
			}

			PassRefPtr<BoundsComposition> FragmentBuilder::BuildLeftArrow(RefPtr<PolygonElement>& elementOut)
			{
				elementOut = BuildLeftArrow();
				return BuildDockedElementContainer(elementOut);
			}

			PassRefPtr<PolygonElement> FragmentBuilder::BuildRightArrow()
			{
				RefPtr<PolygonElement> element = PolygonElement::Create();
				FillRightArrow(element);
				element->SetBorderColor(CColor(0, 0, 0));
				element->SetBackgroundColor(CColor(0, 0, 0));
				return element;
			}

			PassRefPtr<BoundsComposition> FragmentBuilder::BuildRightArrow(RefPtr<PolygonElement>& elementOut)
			{
				elementOut = BuildRightArrow();
				return BuildDockedElementContainer(elementOut);
			}
		}
	}

	namespace presentation
	{
		namespace control
		{
			namespace helper
			{
				BYTE IntToColor(cint color)
				{
					return color < 0 ?
						0 : color > 0xFF
						? 0xFF : (BYTE)color;
				}

				CColor BlendColor(CColor c1, CColor c2, cint currentPosition, cint totalLength)
				{
					return CColor(
						IntToColor((c2.r*currentPosition + c1.r*(totalLength - currentPosition)) / totalLength),
						IntToColor((c2.g*currentPosition + c1.g*(totalLength - currentPosition)) / totalLength),
						IntToColor((c2.b*currentPosition + c1.b*(totalLength - currentPosition)) / totalLength),
						IntToColor((c2.a*currentPosition + c1.a*(totalLength - currentPosition)) / totalLength)
						);
				}
			}
		}

		namespace control
		{
			namespace style
			{
				using namespace helper;

				bool Win8ButtonColors::operator!=(const Win8ButtonColors& colors)
				{
					return !(*this == colors);
				}

				bool Win8ButtonColors::operator==(const Win8ButtonColors& colors)
				{
					return
						borderColor == colors.borderColor &&
						g1 == colors.g1 &&
						g2 == colors.g2 &&
						textColor == colors.textColor &&
						bullet == colors.bullet;
				}

				void Win8ButtonColors::SetAlphaWithoutText(unsigned char a)
				{
					borderColor.a = a;
					g1.a = a;
					g2.a = a;
				}

				Win8ButtonColors Win8ButtonColors::Blend(const Win8ButtonColors& c1, const Win8ButtonColors& c2, cint ratio, cint total)
				{
					if (ratio<0) ratio = 0;
					else if (ratio>total) ratio = total;

					Win8ButtonColors result;
					result.borderColor = BlendColor(c1.borderColor, c2.borderColor, ratio, total);
					result.g1 = BlendColor(c1.g1, c2.g1, ratio, total);
					result.g2 = BlendColor(c1.g2, c2.g2, ratio, total);
					result.textColor = BlendColor(c1.textColor, c2.textColor, ratio, total);
					result.bullet = BlendColor(c1.bullet, c2.bullet, ratio, total);
					return result;
				}

				Win8ButtonColors Win8ButtonColors::ButtonNormal()
				{
					Win8ButtonColors colors =
					{
						CColor(172, 172, 172),
						CColor(239, 239, 239),
						CColor(229, 229, 229),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ButtonActive()
				{
					Win8ButtonColors colors =
					{
						CColor(126, 180, 234),
						CColor(235, 244, 252),
						CColor(220, 236, 252),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ButtonPressed()
				{
					Win8ButtonColors colors =
					{
						CColor(86, 157, 229),
						CColor(218, 236, 252),
						CColor(196, 224, 252),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ButtonDisabled()
				{
					Win8ButtonColors colors =
					{
						CColor(173, 178, 181),
						CColor(252, 252, 252),
						CColor(252, 252, 252),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Disable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ItemNormal()
				{
					Win8ButtonColors colors =
					{
						CColor(112, 192, 231, 0),
						CColor(229, 243, 251, 0),
						CColor(229, 243, 251, 0),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ItemActive()
				{
					Win8ButtonColors colors =
					{
						CColor(112, 192, 231),
						CColor(229, 243, 251),
						CColor(229, 243, 251),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ItemSelected()
				{
					Win8ButtonColors colors =
					{
						CColor(102, 167, 232),
						CColor(209, 232, 255),
						CColor(209, 232, 255),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ItemDisabled()
				{
					Win8ButtonColors colors =
					{
						CColor(112, 192, 231, 0),
						CColor(229, 243, 251, 0),
						CColor(229, 243, 251, 0),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Disable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::CheckedNormal(bool selected)
				{
					Win8ButtonColors colors =
					{
						CColor(112, 112, 112),
						CColor(255, 255, 255),
						CColor(255, 255, 255),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
						CColor(0, 0, 0),
					};
					if (!selected)
					{
						colors.bullet.a = 0;
					}
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::CheckedActive(bool selected)
				{
					Win8ButtonColors colors =
					{
						CColor(51, 153, 255),
						CColor(243, 249, 255),
						CColor(243, 249, 255),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
						CColor(33, 33, 33),
					};
					if (!selected)
					{
						colors.bullet.a = 0;
					}
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::CheckedPressed(bool selected)
				{
					Win8ButtonColors colors =
					{
						CColor(0, 124, 222),
						CColor(217, 236, 255),
						CColor(217, 236, 255),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
						CColor(0, 0, 0),
					};
					if (!selected)
					{
						colors.bullet.a = 0;
					}
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::CheckedDisabled(bool selected)
				{
					Win8ButtonColors colors =
					{
						CColor(188, 188, 188),
						CColor(230, 230, 230),
						CColor(230, 230, 230),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Disable),
						CColor(112, 112, 112),
					};
					if (!selected)
					{
						colors.bullet.a = 0;
					}
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ScrollHandleNormal()
				{
					Win8ButtonColors colors =
					{
						CColor(205, 205, 205),
						CColor(205, 205, 205),
						CColor(205, 205, 205),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ScrollHandleActive()
				{
					Win8ButtonColors colors =
					{
						CColor(166, 166, 166),
						CColor(166, 166, 166),
						CColor(166, 166, 166),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ScrollHandlePressed()
				{
					Win8ButtonColors colors =
					{
						CColor(166, 166, 166),
						CColor(166, 166, 166),
						CColor(166, 166, 166),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ScrollHandleDisabled()
				{
					Win8ButtonColors colors =
					{
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ScrollArrowNormal()
				{
					Win8ButtonColors colors =
					{
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						CColor(96, 96, 96),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ScrollArrowActive()
				{
					Win8ButtonColors colors =
					{
						CColor(218, 218, 218),
						CColor(218, 218, 218),
						CColor(218, 218, 218),
						CColor(0, 0, 0),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ScrollArrowPressed()
				{
					Win8ButtonColors colors =
					{
						CColor(96, 96, 96),
						CColor(96, 96, 96),
						CColor(96, 96, 96),
						CColor(255, 255, 255),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ScrollArrowDisabled()
				{
					Win8ButtonColors colors =
					{
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						CColor(191, 191, 191),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ToolstripButtonNormal()
				{
					Win8ButtonColors colors =
					{
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ToolstripButtonActive()
				{
					Win8ButtonColors colors =
					{
						CColor(120, 174, 229),
						CColor(209, 226, 242),
						CColor(209, 226, 242),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ToolstripButtonPressed()
				{
					Win8ButtonColors colors =
					{
						CColor(96, 161, 226),
						CColor(180, 212, 244),
						CColor(180, 212, 244),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ToolstripButtonSelected()
				{
					Win8ButtonColors colors =
					{
						CColor(96, 161, 226),
						CColor(233, 241, 250),
						CColor(233, 241, 250),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::ToolstripButtonDisabled()
				{
					Win8ButtonColors colors =
					{
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Disable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuBarButtonNormal()
				{
					Win8ButtonColors colors =
					{
						CColor(245, 246, 247),
						CColor(245, 246, 247),
						CColor(245, 246, 247),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuBarButtonActive()
				{
					Win8ButtonColors colors =
					{
						CColor(122, 177, 232),
						CColor(213, 231, 248),
						CColor(213, 231, 248),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuBarButtonPressed()
				{
					Win8ButtonColors colors =
					{
						CColor(98, 163, 229),
						CColor(184, 216, 249),
						CColor(184, 216, 249),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuBarButtonDisabled()
				{
					Win8ButtonColors colors =
					{
						CColor(245, 246, 247),
						CColor(245, 246, 247),
						CColor(245, 246, 247),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Disable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuItemButtonNormal()
				{
					Win8ButtonColors colors =
					{
						CColor(240, 240, 240),
						CColor(240, 240, 240),
						CColor(240, 240, 240),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Menu_Splitter),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuItemButtonNormalActive()
				{
					Win8ButtonColors colors =
					{
						CColor(120, 174, 229),
						CColor(209, 226, 242),
						CColor(209, 226, 242),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
						CColor(187, 204, 220),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuItemButtonSelected()
				{
					Win8ButtonColors colors =
					{
						CColor(120, 174, 229),
						CColor(233, 241, 250),
						CColor(233, 241, 250),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
						CColor(233, 241, 250),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuItemButtonSelectedActive()
				{
					Win8ButtonColors colors =
					{
						CColor(120, 174, 229),
						CColor(233, 241, 250),
						CColor(233, 241, 250),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
						CColor(187, 204, 220),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuItemButtonDisabled()
				{
					Win8ButtonColors colors =
					{
						CColor(240, 240, 240),
						CColor(240, 240, 240),
						CColor(240, 240, 240),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Disable),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Menu_Splitter),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::MenuItemButtonDisabledActive()
				{
					Win8ButtonColors colors =
					{
						CColor(120, 174, 229),
						CColor(209, 226, 242),
						CColor(209, 226, 242),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Disable),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Menu_Splitter),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::TabPageHeaderNormal()
				{
					Win8ButtonColors colors =
					{
						CColor(172, 172, 172),
						CColor(239, 239, 239),
						CColor(229, 229, 229),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::TabPageHeaderActive()
				{
					Win8ButtonColors colors =
					{
						CColor(126, 180, 234),
						CColor(236, 244, 252),
						CColor(221, 237, 252),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonColors Win8ButtonColors::TabPageHeaderSelected()
				{
					Win8ButtonColors colors =
					{
						CColor(172, 172, 172),
						CColor(255, 255, 255),
						CColor(255, 255, 255),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable),
					};
					return colors;
				}

				Win8ButtonElements Win8ButtonElements::Create(Alignment horizontal /*= Alignment::StringAlignmentCenter*/, Alignment vertical /*= Alignment::StringAlignmentCenter*/)
				{
					Win8ButtonElements button;
					{
						button.mainComposition = adoptRef(new BoundsComposition);
						button.mainComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					}
					{
						RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
						button.rectBorderElement = element;

						RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
						button.mainComposition->AddChild(composition);
						composition->SetAlignmentToParent(CRect());
						composition->SetOwnedElement(element);
					}
					{
						RefPtr<GradientBackgroundElement> element = GradientBackgroundElement::Create();
						button.backgroundElement = element;
						element->SetDirection(GradientBackgroundElement::Vertical);
						element->SetShape(ElementShape::Rectangle);

						RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
						button.backgroundComposition = composition;
						button.mainComposition->AddChild(composition);
						composition->SetAlignmentToParent(CRect(1, 1, 1, 1));
						composition->SetOwnedElement(element);
					}
					{
						button.textElement = SolidLabelElement::Create();
						button.textElement->SetAlignments(horizontal, vertical);
						button.textComposition = adoptRef(new BoundsComposition);
						button.textComposition->SetOwnedElement(button.textElement);
						button.textComposition->SetMargin(CRect());
						button.textComposition->SetMinSizeLimitation(Composition::LimitToElement);
						button.textComposition->SetAlignmentToParent(CRect());
						button.mainComposition->AddChild(button.textComposition);
					}
					return button;
				}

				void Win8ButtonElements::Apply(const Win8ButtonColors& colors)
				{
					if (rectBorderElement)
					{
						rectBorderElement->SetColor(colors.borderColor);
					}
					backgroundElement->SetColors(colors.g1, colors.g2);
					textElement->SetColor(colors.textColor);
				}

				Win8CheckedButtonElements Win8CheckedButtonElements::Create(ElementShape shape, bool backgroundVisible)
				{
					const cint checkSize = 13;
					const cint checkPadding = 2;

					Win8CheckedButtonElements button;
					{
						button.mainComposition = adoptRef(new BoundsComposition);
						button.mainComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					}
					{
						RefPtr<TableComposition> mainTable = adoptRef(new TableComposition);
						button.mainComposition->AddChild(mainTable);
						if (backgroundVisible)
						{
							RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
							element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));
							mainTable->SetOwnedElement(element);
						}
						mainTable->SetRowsAndColumns(1, 2);
						mainTable->SetAlignmentToParent(CRect());
						mainTable->SetRowOption(0, CellOption::PercentageOption(1.0));
						mainTable->SetColumnOption(0, CellOption::AbsoluteOption(checkSize + 2 * checkPadding));
						mainTable->SetColumnOption(1, CellOption::PercentageOption(1.0));

						{
							RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
							mainTable->AddChild(cell);
							cell->SetSite(0, 0, 1, 1);

							RefPtr<TableComposition> table = adoptRef(new TableComposition);
							cell->AddChild(table);
							table->SetRowsAndColumns(3, 1);
							table->SetAlignmentToParent(CRect());
							table->SetRowOption(0, CellOption::PercentageOption(0.5));
							table->SetRowOption(1, CellOption::MinSizeOption());
							table->SetRowOption(2, CellOption::PercentageOption(0.5));

							{
								RefPtr<TableCellComposition> checkCell = adoptRef(new TableCellComposition);
								table->AddChild(checkCell);
								checkCell->SetSite(1, 0, 1, 1);
								{
									RefPtr<BoundsComposition> borderBounds = adoptRef(new BoundsComposition);
									checkCell->AddChild(borderBounds);
									borderBounds->SetAlignmentToParent(CRect(checkPadding, -1, checkPadding, -1));
									borderBounds->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
									{
										RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
										button.bulletBorderElement = element;
										element->SetShape(shape);

										RefPtr<BoundsComposition> bounds = adoptRef(new BoundsComposition);
										borderBounds->AddChild(bounds);
										bounds->SetOwnedElement(element);
										bounds->SetAlignmentToParent(CRect());
										bounds->SetBounds(CRect(CPoint(), CSize(checkSize, checkSize)));
									}
									{
										RefPtr<GradientBackgroundElement> element = GradientBackgroundElement::Create();
										button.bulletBackgroundElement = element;
										element->SetShape(shape);
										element->SetDirection(GradientBackgroundElement::Vertical);

										RefPtr<BoundsComposition> bounds = adoptRef(new BoundsComposition);
										borderBounds->AddChild(bounds);
										bounds->SetOwnedElement(element);
										bounds->SetAlignmentToParent(CRect(1, 1, 1, 1));
									}
								}

								button.bulletCheckElement = nullptr;
								button.bulletRadioElement = nullptr;
								if (shape == ElementShape::Rectangle)
								{
									button.bulletCheckElement = SolidLabelElement::Create();
									{
										Font font;
										font.fontFamily = _T("Webdings");
										font.size = 16;
										font.bold = true;
										button.bulletCheckElement->SetFont(font);
									}
									button.bulletCheckElement->SetText(_T("a"));
									button.bulletCheckElement->SetAlignments(Alignment::StringAlignmentCenter, Alignment::StringAlignmentCenter);

									RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
									composition->SetOwnedElement(button.bulletCheckElement);
									composition->SetAlignmentToParent(CRect());
									checkCell->AddChild(composition);
								}
								else
								{
									button.bulletRadioElement = SolidBackgroundElement::Create();
									button.bulletRadioElement->SetShape(ElementShape::Ellipse);

									RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
									composition->SetOwnedElement(button.bulletRadioElement);
									composition->SetAlignmentToParent(CRect(checkPadding + 3, 3, checkPadding + 3, 3));
									checkCell->AddChild(composition);
								}
							}
						}
						{
							RefPtr<TableCellComposition> textCell = adoptRef(new TableCellComposition);
							mainTable->AddChild(textCell);
							textCell->SetSite(0, 1, 1, 1);
							{
								button.textElement = SolidLabelElement::Create();
								button.textElement->SetAlignments(Alignment::StringAlignmentNear, Alignment::StringAlignmentCenter);
								button.textComposition = adoptRef(new BoundsComposition);
								button.textComposition->SetOwnedElement(button.textElement);
								button.textComposition->SetMargin(CRect());
								button.textComposition->SetMinSizeLimitation(Composition::LimitToElement);
								button.textComposition->SetAlignmentToParent(CRect());
								textCell->AddChild(button.textComposition);
							}
						}
					}
					return button;
				}

				void Win8CheckedButtonElements::Apply(const Win8ButtonColors& colors)
				{
					bulletBorderElement->SetColor(colors.borderColor);
					bulletBackgroundElement->SetColors(colors.g1, colors.g2);
					textElement->SetColor(colors.textColor);
					if (bulletCheckElement)
					{
						bulletCheckElement->SetColor(colors.bullet);
					}
					if (bulletRadioElement)
					{
						bulletRadioElement->SetColor(colors.bullet);
					}
				}

				bool Win8TextBoxColors::operator!=(const Win8TextBoxColors& colors)
				{
					return !(*this == colors);
				}

				bool Win8TextBoxColors::operator==(const Win8TextBoxColors& colors)
				{
					return
						borderColor == colors.borderColor &&
						backgroundColor == colors.backgroundColor;
				}

				Win8TextBoxColors Win8TextBoxColors::Blend(const Win8TextBoxColors& c1, const Win8TextBoxColors& c2, cint ratio, cint total)
				{
					if (ratio < 0) ratio = 0;
					else if (ratio > total) ratio = total;

					Win8TextBoxColors result;
					result.borderColor = BlendColor(c1.borderColor, c2.borderColor, ratio, total);
					result.backgroundColor = BlendColor(c1.backgroundColor, c2.backgroundColor, ratio, total);
					return result;
				}

				Win8TextBoxColors Win8TextBoxColors::Normal()
				{
					Win8TextBoxColors result =
					{
						CColor(171, 173, 179),
						CColor(255, 255, 255),
					};
					return result;
				}

				Win8TextBoxColors Win8TextBoxColors::Active()
				{
					Win8TextBoxColors result =
					{
						CColor(126, 180, 234),
						CColor(255, 255, 255),
					};
					return result;
				}

				Win8TextBoxColors Win8TextBoxColors::Focused()
				{
					Win8TextBoxColors result =
					{
						CColor(86, 157, 229),
						CColor(255, 255, 255),
					};
					return result;
				}

				Win8TextBoxColors Win8TextBoxColors::Disabled()
				{
					Win8TextBoxColors result =
					{
						CColor(217, 217, 217),
						Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window),
					};
					return result;
				}

				Win8MenuItemButtonElements Win8MenuItemButtonElements::Create()
				{
					Win8MenuItemButtonElements button;
					{
						button.mainComposition = adoptRef(new BoundsComposition);
						button.mainComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					}
					{
						RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
						button.borderElement = element;

						RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
						button.mainComposition->AddChild(composition);
						composition->SetAlignmentToParent(CRect(0, 0, 0, 0));
						composition->SetOwnedElement(element);
					}
					{
						RefPtr<GradientBackgroundElement> element = GradientBackgroundElement::Create();
						button.backgroundElement = element;
						element->SetDirection(GradientBackgroundElement::Vertical);

						RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
						button.mainComposition->AddChild(composition);
						composition->SetAlignmentToParent(CRect(1, 1, 1, 1));
						composition->SetOwnedElement(element);
					}
					{
						RefPtr<TableComposition> table = adoptRef(new TableComposition);
						button.mainComposition->AddChild(table);
						table->SetAlignmentToParent(CRect(2, 0, 2, 0));
						table->SetRowsAndColumns(1, 5);

						table->SetRowOption(0, CellOption::PercentageOption(1.0));
						table->SetColumnOption(0, CellOption::AbsoluteOption(24));
						table->SetColumnOption(1, CellOption::AbsoluteOption(1));
						table->SetColumnOption(2, CellOption::PercentageOption(1.0));
						table->SetColumnOption(3, CellOption::MinSizeOption());
						table->SetColumnOption(4, CellOption::AbsoluteOption(10));

						{
							RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
							table->AddChild(cell);
							cell->SetSite(0, 0, 1, 1);
							button.splitterComposition = cell;

							RefPtr<ImageFrameElement> element = ImageFrameElement::Create();
							button.imageElement = element;
							element->SetStretch(false);
							element->SetAlignments(Alignment::StringAlignmentCenter, Alignment::StringAlignmentCenter);
							cell->SetOwnedElement(element);
						}
						{
							RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
							table->AddChild(cell);
							cell->SetSite(0, 1, 1, 1);
							button.splitterComposition = cell;

							RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
							button.splitterElement = element;
							cell->SetOwnedElement(element);
						}
						{
							RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
							table->AddChild(cell);
							cell->SetSite(0, 2, 1, 1);

							button.textElement = SolidLabelElement::Create();
							button.textElement->SetAlignments(Alignment::StringAlignmentNear, Alignment::StringAlignmentCenter);

							button.textComposition = adoptRef(new SharedSizeItemComposition);
							button.textComposition->SetGroup(_T("MenuItem-Text"));
							button.textComposition->SetSharedWidth(true);
							button.textComposition->SetOwnedElement(button.textElement);
							button.textComposition->SetMargin(CRect(0, 0, 0, 0));
							button.textComposition->SetMinSizeLimitation(Composition::LimitToElement);
							button.textComposition->SetAlignmentToParent(CRect());

							cell->AddChild(button.textComposition);
						}
						{
							RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
							table->AddChild(cell);
							cell->SetSite(0, 3, 1, 1);

							button.shortcutElement = SolidLabelElement::Create();
							button.shortcutElement->SetAlignments(Alignment::StringAlignmentFar, Alignment::StringAlignmentCenter);

							button.shortcutComposition = adoptRef(new SharedSizeItemComposition);
							button.shortcutComposition->SetGroup(_T("MenuItem-Shortcut"));
							button.shortcutComposition->SetSharedWidth(true);
							button.shortcutComposition->SetOwnedElement(button.shortcutElement);
							button.shortcutComposition->SetMargin(CRect(0, 0, 0, 0));
							button.shortcutComposition->SetMinSizeLimitation(Composition::LimitToElement);
							button.shortcutComposition->SetAlignmentToParent(CRect());

							cell->AddChild(button.shortcutComposition);
						}
						{
							button.subMenuArrowElement = FragmentBuilder::BuildRightArrow();

							RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
							button.subMenuArrowComposition = cell;
							cell->SetMinSizeLimitation(Composition::LimitToElement);
							table->AddChild(cell);
							cell->SetSite(0, 4, 1, 1);
							cell->SetOwnedElement(button.subMenuArrowElement);
							cell->SetVisible(false);
						}
					}
					return button;
				}

				void Win8MenuItemButtonElements::Apply(const Win8ButtonColors& colors)
				{
					borderElement->SetColor(colors.borderColor);
					backgroundElement->SetColors(colors.g1, colors.g2);
					splitterElement->SetColor(colors.bullet);
					textElement->SetColor(colors.textColor);
					shortcutElement->SetColor(colors.textColor);
					subMenuArrowElement->SetBackgroundColor(colors.textColor);
					subMenuArrowElement->SetBorderColor(colors.textColor);
				}

				void Win8MenuItemButtonElements::SetActive(bool value)
				{
					if (value)
					{
						splitterComposition->SetMargin(CRect(0, 1, 0, 2));
					}
					else
					{
						splitterComposition->SetMargin(CRect(0, 0, 0, 0));
					}
				}

				void Win8MenuItemButtonElements::SetSubMenuExisting(bool value)
				{
					subMenuArrowComposition->SetVisible(value);
				}
			}
		}
	}
}