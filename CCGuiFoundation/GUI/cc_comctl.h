#ifndef CC_COMCTL
#define CC_COMCTL

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_control.h"
#include "cc_element.h"

using namespace cc::base;
using namespace cc::interfaces::windows;
using namespace cc::presentation::control;
using namespace cc::presentation::element;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			class CustomControl;
			class Scroll;
			class Tab;

			class Label;
			class Button;
			class SelectableButton;

			class SinglelineTextBox;
			class ScrollContainer;
			class MultilineTextBox;

			class ToolstripMenu;
			class ToolstripMenuBar;
			class ToolstripButton;
			class ToolstripToolBar;

			class Console;

			template<class TStyleController>
			class TransferringAnimationHostGetter
			{
			protected:
				PassRefPtr<GraphicsHost> GetHost(PassRefPtr<TStyleController> _style)
				{
					return _style->GetBoundsComposition()->GetRelatedGraphicsHost();
				}
			};

			template<class TStyleController>
			class TransferringAnimationFocusableHostGetter
			{
			protected:
				PassRefPtr<GraphicsHost> GetHost(PassRefPtr<TStyleController> _style)
				{
					return _style->focusableComposition->GetRelatedGraphicsHost();
				}
			};

			template<class TState, class TStyleController, class THostGetter>
			class TransferringAnimationBase : public TimeBasedAnimation, public THostGetter
			{
			public:
				TransferringAnimationBase(TStyleController* _style, const TState& begin)
					: TimeBasedAnimation(0)
					, colorBegin(begin)
					, colorEnd(begin)
					, colorCurrent(begin)
					, style(_style)
					, stopped(true)
					, disabled(false)
					, enableAnimation(true)
				{
				}
				void Disable()
				{
					disabled = true;
				}
				void Play(cint currentPosition, cint totalLength)override
				{
					if (!disabled)
					{
						PlayInternal(currentPosition, totalLength);
					}
				}
				void Stop()override
				{
					stopped = true;
				}
				bool GetEnableAnimation()
				{
					return enableAnimation;
				}
				void SetEnableAnimation(bool value)
				{
					enableAnimation = value;
				}
				void Transfer(const TState& end)
				{
					if (colorEnd != end)
					{
						RefPtr<GraphicsHost> host = GetHost(style);
						if (enableAnimation && host)
						{
							Restart(120);
							if (stopped)
							{
								colorBegin = colorEnd;
								colorEnd = end;
								style->ToAnimation(host->GetAnimationManager());
								stopped = false;
							}
							else
							{
								colorBegin = colorCurrent;
								colorEnd = end;
							}
						}
						else
						{
							colorBegin = end;
							colorEnd = end;
							colorCurrent = end;
							Play(1, 1);
						}
					}
				}

			protected:
				virtual void PlayInternal(cint currentPosition, cint totalLength) = 0;

				TState									colorBegin;
				TState									colorEnd;
				TState									colorCurrent;
				TStyleController*						style;
				bool									stopped;
				bool									disabled;
				bool									enableAnimation;
			};

			template<class TState, class TStyleController>
			class TransferringAnimation : public TransferringAnimationBase<TState, TStyleController, TransferringAnimationHostGetter<TStyleController>>
			{
			public:
				TransferringAnimation(TStyleController* _style, const TState& begin)
					: TransferringAnimationBase(_style, begin)
				{
				}
			};

			template<class TState, class TStyleController>
			class TransferringAnimationFocusable : public TransferringAnimationBase<TState, TStyleController, TransferringAnimationFocusableHostGetter<TStyleController>>
			{
			public:
				TransferringAnimationFocusable(TStyleController* _style, const TState& begin)
					: TransferringAnimationBase(_style, begin)
				{
				}
			};

			//////////////////////////////////////////////////////////////////////////

			namespace helper
			{
				BYTE		IntToColor(cint color);
				CColor		BlendColor(CColor c1, CColor c2, cint currentPosition, cint totalLength);
			}

			class FragmentBuilder
			{
			private:
				static PassRefPtr<BoundsComposition>				BuildDockedElementContainer(PassRefPtr<IGraphicsElement> element);
			public:
				static void											FillUpArrow(PassRefPtr<PolygonElement> element);
				static void											FillDownArrow(PassRefPtr<PolygonElement> element);
				static void											FillLeftArrow(PassRefPtr<PolygonElement> element);
				static void											FillRightArrow(PassRefPtr<PolygonElement> element);

				static PassRefPtr<PolygonElement>					BuildUpArrow();
				static PassRefPtr<PolygonElement>					BuildDownArrow();
				static PassRefPtr<PolygonElement>					BuildLeftArrow();
				static PassRefPtr<PolygonElement>					BuildRightArrow();

				static PassRefPtr<BoundsComposition>				BuildUpArrow(RefPtr<PolygonElement>& elementOut);
				static PassRefPtr<BoundsComposition>				BuildDownArrow(RefPtr<PolygonElement>& elementOut);
				static PassRefPtr<BoundsComposition>				BuildLeftArrow(RefPtr<PolygonElement>& elementOut);
				static PassRefPtr<BoundsComposition>				BuildRightArrow(RefPtr<PolygonElement>& elementOut);
			};
		}
	}

	namespace global
	{
		using namespace cc::presentation::control;

		class ControlFactory
		{
		public:
			static PassRefPtr<Window>				NewWindow();
			static PassRefPtr<CustomControl>		NewCustomControl();
			static PassRefPtr<Tab>					NewTab();

			static PassRefPtr<Label>				NewLabel();
			static PassRefPtr<Button>				NewButton();
			static PassRefPtr<SelectableButton>		NewCheckBox();
			static PassRefPtr<SelectableButton>		NewRadioButton();
			static PassRefPtr<Control>				NewGroupBox();

			static PassRefPtr<SinglelineTextBox>	NewSinglelineTextBox();
			static PassRefPtr<ScrollContainer>		NewScrollContainer();
			static PassRefPtr<MultilineTextBox>		NewMultilineTextBox();

			static PassRefPtr<ToolstripMenu>		NewMenu(PassRefPtr<Control> owner);
			static PassRefPtr<ToolstripMenuBar>		NewMenuBar();
			static PassRefPtr<Control>				NewMenuSplitter();
			static PassRefPtr<ToolstripButton>		NewMenuBarButton();
			static PassRefPtr<ToolstripButton>		NewMenuItemButton();

			static PassRefPtr<ToolstripToolBar>		NewToolBar();
			static PassRefPtr<ToolstripButton>		NewToolBarButton();
			static PassRefPtr<ToolstripButton>		NewToolBarDropdownButton();
			static PassRefPtr<ToolstripButton>		NewToolBarSplitButton();
			static PassRefPtr<Control>				NewToolBarSplitter();

			static PassRefPtr<Scroll>				NewHScroll();
			static PassRefPtr<Scroll>				NewVScroll();
			static PassRefPtr<Console>				NewConsole();
		};
	}

	namespace presentation
	{
		namespace control
		{
			class CustomControl :public Control, public ControlHostRoot
			{
			public:
				CustomControl(PassRefPtr<IControlStyleController> _styleController);
				~CustomControl();
			};
		}
	}
}

#endif