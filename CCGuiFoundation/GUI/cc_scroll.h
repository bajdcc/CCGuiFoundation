#ifndef CC_SCROLL
#define CC_SCROLL

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_control.h"
#include "cc_element.h"
#include "cc_comctl.h"
#include "cc_button.h"

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
			class Scroll;
			class ScrollView;
			class ScrollContainer;

			namespace style
			{
				class IScrollStyleController : public IControlStyleController
				{
				public:
					virtual void										SetCommandExecutor(PassRefPtr<IScrollCommandExecutor> value) = 0;
					virtual void										SetTotalSize(cint value) = 0;
					virtual void										SetPageSize(cint value) = 0;
					virtual void										SetPosition(cint value) = 0;
				};

				class IScrollViewStyleProvider : public IControlStyleProvider
				{
				public:
					virtual PassRefPtr<IScrollStyleController>			CreateHorizontalScrollStyle() = 0;
					virtual PassRefPtr<IScrollStyleController>			CreateVerticalScrollStyle() = 0;
					virtual cint										GetDefaultScrollSize() = 0;
					virtual PassRefPtr<Composition>						InstallBackground(PassRefPtr<BoundsComposition> boundsComposition) = 0;
				};

				class ScrollViewStyleController : public IControlStyleController
				{
				public:
					ScrollViewStyleController(PassRefPtr<IScrollViewStyleProvider> _styleProvider);
					~ScrollViewStyleController();

					void									SetScrollView(PassRefPtr<ScrollView> _scrollView);
					void									AdjustView(CSize fullSize);
					PassRefPtr<IScrollViewStyleProvider>	GetStyleProvider();

					PassRefPtr<Scroll>						GetHorizontalScroll();
					PassRefPtr<Scroll>						GetVerticalScroll();

					PassRefPtr<TableComposition>			GetInternalTableComposition();
					PassRefPtr<BoundsComposition>			GetInternalContainerComposition();

					bool									GetHorizontalAlwaysVisible();
					void									SetHorizontalAlwaysVisible(bool value);
					bool									GetVerticalAlwaysVisible();
					void									SetVerticalAlwaysVisible(bool value);

					PassRefPtr<BoundsComposition>			GetBoundsComposition()override;
					PassRefPtr<Composition>					GetContainerComposition()override;
					void									SetFocusableComposition(PassRefPtr<Composition> value)override;
					void									SetText(const CString& value)override;
					void									SetFont(const Font& value)override;
					void									SetVisuallyEnabled(bool value)override;

				protected:
					void									UpdateTable();

					RefPtr<IScrollViewStyleProvider>		styleProvider;
					RawPtr<ScrollView>						scrollView;
					RefPtr<Scroll>							horizontalScroll;
					RefPtr<Scroll>							verticalScroll;
					RefPtr<BoundsComposition>				boundsComposition;
					RefPtr<TableComposition>				tableComposition;
					RefPtr<TableCellComposition>			containerCellComposition;
					RefPtr<BoundsComposition>				containerComposition;
					bool									horizontalAlwaysVisible;
					bool									verticalAlwaysVisible;
				};

				class ScrollContainerStyleController : public ScrollViewStyleController
				{
				public:
					ScrollContainerStyleController(PassRefPtr<IScrollViewStyleProvider> styleProvider);
					~ScrollContainerStyleController();

					PassRefPtr<Composition>					GetContainerComposition()override;
					void									MoveContainer(CPoint leftTop);

					bool									GetExtendToFullWidth();
					void									SetExtendToFullWidth(bool value);

				protected:
					RefPtr<BoundsComposition>				controlContainerComposition;
					bool									extendToFullWidth;
				};

				class ScrollStyleBase : public IScrollStyleController
				{
				public:
					enum Direction
					{
						Horizontal,
						Vertical,
					};

				public:
					ScrollStyleBase(Direction _direction);
					~ScrollStyleBase();

					PassRefPtr<BoundsComposition>						GetBoundsComposition()override;
					PassRefPtr<Composition>								GetContainerComposition()override;
					void												SetFocusableComposition(PassRefPtr<Composition> value)override;
					void												SetText(const CString& value)override;
					void												SetFont(const Font& value)override;
					void												SetVisuallyEnabled(bool value)override;
					void												SetCommandExecutor(PassRefPtr<IScrollCommandExecutor> value)override;
					void												SetTotalSize(cint value)override;
					void												SetPageSize(cint value)override;
					void												SetPosition(cint value)override;
				protected:
					void												UpdateHandle();
					void												OnDecreaseButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments);
					void												OnIncreaseButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments);
					void												OnHandleMouseDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments);
					void												OnHandleMouseMove(PassRefPtr<Composition> sender, MouseEventArgs& arguments);
					void												OnHandleMouseUp(PassRefPtr<Composition> sender, MouseEventArgs& arguments);
					void												OnBigMoveMouseDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments);

					virtual PassRefPtr<IButtonStyleController>			CreateDecreaseButtonStyle(Direction direction) = 0;
					virtual PassRefPtr<IButtonStyleController>			CreateIncreaseButtonStyle(Direction direction) = 0;
					virtual PassRefPtr<IButtonStyleController>			CreateHandleButtonStyle(Direction direction) = 0;
					virtual PassRefPtr<BoundsComposition>				InstallBackground(PassRefPtr<BoundsComposition> boundsComposition, Direction direction) = 0;
					void												BuildStyle(cint defaultSize, cint arrowSize);

					Direction											direction;
					RefPtr<IScrollCommandExecutor>						commandExecutor;
					RefPtr<Button>										decreaseButton;
					RefPtr<Button>										increaseButton;
					RefPtr<Button>										handleButton;
					RefPtr<PartialViewComposition>						handleComposition;
					RefPtr<BoundsComposition>							boundsComposition;
					RefPtr<BoundsComposition>							containerComposition;

					cint												totalSize;
					cint												pageSize;
					cint												position;
					CPoint												draggingStartLocation;
					bool												draggingHandle;
				};

				class Win8ScrollStyle : public ScrollStyleBase
				{
				public:
					Win8ScrollStyle(Direction _direction);
					~Win8ScrollStyle();

					static const cint							DefaultSize = 16;
					static const cint							ArrowSize = 8;

				protected:
					PassRefPtr<IButtonStyleController>			CreateDecreaseButtonStyle(Direction direction)override;
					PassRefPtr<IButtonStyleController>			CreateIncreaseButtonStyle(Direction direction)override;
					PassRefPtr<IButtonStyleController>			CreateHandleButtonStyle(Direction direction)override;
					PassRefPtr<BoundsComposition>				InstallBackground(PassRefPtr<BoundsComposition> boundsComposition, Direction direction)override;
				};

				class Win8ScrollViewProvider : public IScrollViewStyleProvider
				{
				public:
					Win8ScrollViewProvider();
					~Win8ScrollViewProvider();

					virtual PassRefPtr<IScrollStyleController>	CreateHorizontalScrollStyle()override;
					virtual PassRefPtr<IScrollStyleController>	CreateVerticalScrollStyle()override;
					virtual cint								GetDefaultScrollSize()override;
					virtual PassRefPtr<Composition>				InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)override;

					virtual void								AssociateStyleController(PassRefPtr<IControlStyleController> controller)override;
					virtual void								SetFocusableComposition(PassRefPtr<Composition> value)override;
					virtual void								SetText(const CString& value)override;
					virtual void								SetFont(const Font& value)override;
					virtual void								SetVisuallyEnabled(bool value)override;
				};

				class Win8ScrollArrowButtonStyle : public Win8ButtonStyleBase
				{
				public:
					Win8ScrollArrowButtonStyle(ScrollStyleBase::Direction direction, bool increaseButton);
					~Win8ScrollArrowButtonStyle();

				protected:
					void										TransferInternal(Button::ControlState value, bool enabled, bool selected)override;
					void										AfterApplyColors(const Win8ButtonColors& colors)override;

					RefPtr<PolygonElement>						arrowElement;
				};

				class Win8ScrollHandleButtonStyle : public Win8ButtonStyleBase
				{
				public:
					Win8ScrollHandleButtonStyle();
					~Win8ScrollHandleButtonStyle();

				protected:
					void										TransferInternal(Button::ControlState value, bool enabled, bool selected)override;
				};
			}

			using namespace style;

			class ScrollCommandExecutor : public IScrollCommandExecutor
			{
			public:
				ScrollCommandExecutor(PassRefPtr<Scroll> _scroll);
				~ScrollCommandExecutor();

				void									SmallDecrease()override;
				void									SmallIncrease()override;
				void									BigDecrease()override;
				void									BigIncrease()override;

				void									SetTotalSize(cint value)override;
				void									SetPageSize(cint value)override;
				void									SetPosition(cint value)override;

			protected:
				RawPtr<Scroll>							scroll;
			};

			class Scroll : public Control
			{
			public:
				Scroll(PassRefPtr<IScrollStyleController> _styleController);
				~Scroll();

				NotifyEvent								TotalSizeChanged;
				NotifyEvent								PageSizeChanged;
				NotifyEvent								PositionChanged;
				NotifyEvent								SmallMoveChanged;
				NotifyEvent								BigMoveChanged;

				virtual cint							GetTotalSize();
				virtual void							SetTotalSize(cint value);
				virtual cint							GetPageSize();
				virtual void							SetPageSize(cint value);
				virtual cint							GetPosition();
				virtual void							SetPosition(cint value);
				virtual cint							GetSmallMove();
				virtual void							SetSmallMove(cint value);
				virtual cint							GetBigMove();
				virtual void							SetBigMove(cint value);

				cint									GetMinPosition();
				cint									GetMaxPosition();

			protected:
				RefPtr<IScrollStyleController>			styleController;
				RefPtr<ScrollCommandExecutor>			commandExecutor;
				cint									totalSize;
				cint									pageSize;
				cint									position;
				cint									smallMove;
				cint									bigMove;
			};

			class ScrollView : public Control
			{
			public:
				ScrollView(PassRefPtr<IScrollViewStyleProvider> styleProvider);
				~ScrollView();

				virtual void							SetFont(const Font& value);

				void									CalculateView();
				CSize									GetViewSize();
				CRect									GetViewBounds();

				PassRefPtr<Scroll>						GetHorizontalScroll();
				PassRefPtr<Scroll>						GetVerticalScroll();
				bool									GetHorizontalAlwaysVisible();
				void									SetHorizontalAlwaysVisible(bool value);
				bool									GetVerticalAlwaysVisible();
				void									SetVerticalAlwaysVisible(bool value);

			protected:
				ScrollView(PassRefPtr<ScrollViewStyleController> _styleController);

				void									OnContainerBoundsChanged(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnHorizontalScroll(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnVerticalScroll(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnHorizontalWheel(PassRefPtr<Composition> sender, MouseEventArgs& arguments);
				void									OnVerticalWheel(PassRefPtr<Composition> sender, MouseEventArgs& arguments);
				void									CallUpdateView();
				void									Initialize();

				virtual CSize							QueryFullSize() = 0;
				virtual void							UpdateView(CRect viewBounds) = 0;
				virtual cint							GetSmallMove();
				virtual CSize							GetBigMove();

				RefPtr<ScrollViewStyleController>		styleController;
				bool									supressScrolling;
			};

			class ScrollContainer : public ScrollView
			{
			public:
				ScrollContainer(PassRefPtr<IScrollViewStyleProvider> styleProvider);
				~ScrollContainer();

				bool									GetExtendToFullWidth();
				void									SetExtendToFullWidth(bool value);

			protected:
				void									OnControlContainerBoundsChanged(PassRefPtr<Composition> sender, EventArgs& arguments);
				CSize									QueryFullSize()override;
				void									UpdateView(CRect viewBounds)override;

				RefPtr<ScrollContainerStyleController>	styleController;
			};
		}
	}
}

#endif