#include "stdafx.h"
#include "cc_scroll.h"
#include "cc_presentation.h"

using namespace cc::presentation::windows;
using namespace cc::presentation::element;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			namespace style
			{
				PassRefPtr<IScrollStyleController> Win8Style::CreateHScrollStyle()
				{
					return adoptRef(new Win8ScrollStyle(Win8ScrollStyle::Horizontal));
				}

				PassRefPtr<IScrollStyleController> Win8Style::CreateVScrollStyle()
				{
					return adoptRef(new Win8ScrollStyle(Win8ScrollStyle::Horizontal));
				}

				ScrollViewStyleController::ScrollViewStyleController(PassRefPtr<IScrollViewStyleProvider> _styleProvider)
					: styleProvider(_styleProvider)
					, horizontalAlwaysVisible(false)
					, verticalAlwaysVisible(false)
				{
					horizontalScroll = adoptRef(new Scroll(styleProvider->CreateHorizontalScrollStyle()));
					horizontalScroll->GetBoundsComposition()->SetAlignmentToParent(CRect());
					horizontalScroll->SetEnabled(false);
					verticalScroll = adoptRef(new Scroll(styleProvider->CreateVerticalScrollStyle()));
					verticalScroll->GetBoundsComposition()->SetAlignmentToParent(CRect());
					verticalScroll->SetEnabled(false);

					boundsComposition = adoptRef(new BoundsComposition);
					RefPtr<Composition> tableContainerComposition = styleProvider->InstallBackground(boundsComposition);

					tableComposition = adoptRef(new TableComposition);
					tableContainerComposition->AddChild(tableComposition);
					tableComposition->SetAlignmentToParent(CRect());
					tableComposition->SetRowsAndColumns(2, 2);
					tableComposition->SetRowOption(0, CellOption::PercentageOption(1.0));
					tableComposition->SetRowOption(1, CellOption::MinSizeOption());
					tableComposition->SetColumnOption(0, CellOption::PercentageOption(1.0));
					tableComposition->SetColumnOption(1, CellOption::MinSizeOption());
					UpdateTable();
					{
						RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
						tableComposition->AddChild(cell);
						cell->SetSite(1, 0, 1, 1);
						cell->AddChild(horizontalScroll->GetBoundsComposition());
					}
					{
						RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
						tableComposition->AddChild(cell);
						cell->SetSite(0, 1, 1, 1);
						cell->AddChild(verticalScroll->GetBoundsComposition());
					}
					{
						RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
						tableComposition->AddChild(cell);
						RefPtr<BoundsComposition> blank = adoptRef(new BoundsComposition);
						blank->SetAlignmentToParent(CRect());
						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));
						blank->SetOwnedElement(element);
						cell->SetSite(1, 1, 1, 1);
						cell->AddChild(blank);
					}

					containerCellComposition = adoptRef(new TableCellComposition);
					tableComposition->AddChild(containerCellComposition);
					containerCellComposition->SetSite(0, 0, 1, 1);

					containerComposition = adoptRef(new BoundsComposition);
					containerComposition->SetAlignmentToParent(CRect());
					containerCellComposition->AddChild(containerComposition);

					styleProvider->AssociateStyleController(this);
				}

				ScrollViewStyleController::~ScrollViewStyleController()
				{

				}

				void ScrollViewStyleController::SetScrollView(PassRefPtr<ScrollView> _scrollView)
				{
					scrollView = _scrollView;
				}

				void ScrollViewStyleController::AdjustView(CSize fullSize)
				{
					CSize viewSize=containerComposition->GetBounds().Size();
					if(fullSize.cx <= viewSize.cx)
					{
						horizontalScroll->SetEnabled(false);
						horizontalScroll->SetPosition(0);
					}
					else
					{
						horizontalScroll->SetEnabled(true);
						horizontalScroll->SetTotalSize(fullSize.cx);
						horizontalScroll->SetPageSize(viewSize.cx);
					}
					if(fullSize.cy <= viewSize.cy)
					{
						verticalScroll->SetEnabled(false);
						verticalScroll->SetPosition(0);
					}
					else
					{
						verticalScroll->SetEnabled(true);
						verticalScroll->SetTotalSize(fullSize.cy);
						verticalScroll->SetPageSize(viewSize.cy);
					}
					UpdateTable();
				}

				PassRefPtr<IScrollViewStyleProvider> ScrollViewStyleController::GetStyleProvider()
				{
					return styleProvider;
				}

				PassRefPtr<Scroll> ScrollViewStyleController::GetHorizontalScroll()
				{
					return horizontalScroll;
				}

				PassRefPtr<Scroll> ScrollViewStyleController::GetVerticalScroll()
				{
					return verticalScroll;
				}

				PassRefPtr<TableComposition> ScrollViewStyleController::GetInternalTableComposition()
				{
					return tableComposition;
				}

				PassRefPtr<BoundsComposition> ScrollViewStyleController::GetInternalContainerComposition()
				{
					return containerComposition;
				}

				bool ScrollViewStyleController::GetHorizontalAlwaysVisible()
				{
					return horizontalAlwaysVisible;
				}

				void ScrollViewStyleController::SetHorizontalAlwaysVisible(bool value)
				{
					if (horizontalAlwaysVisible != value)
					{
						horizontalAlwaysVisible = value;
						scrollView->CalculateView();
					}
				}

				bool ScrollViewStyleController::GetVerticalAlwaysVisible()
				{
					return verticalAlwaysVisible;
				}

				void ScrollViewStyleController::SetVerticalAlwaysVisible(bool value)
				{
					if (verticalAlwaysVisible != value)
					{
						verticalAlwaysVisible = value;
						scrollView->CalculateView();
					}
				}

				PassRefPtr<BoundsComposition> ScrollViewStyleController::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> ScrollViewStyleController::GetContainerComposition()
				{
					return containerComposition;
				}

				void ScrollViewStyleController::SetFocusableComposition(PassRefPtr<Composition> value)
				{
					styleProvider->SetFocusableComposition(value);
				}

				void ScrollViewStyleController::SetText(const CString& value)
				{
					styleProvider->SetText(value);
				}

				void ScrollViewStyleController::SetFont(const Font& value)
				{
					styleProvider->SetFont(value);
				}

				void ScrollViewStyleController::SetVisuallyEnabled(bool value)
				{
					styleProvider->SetVisuallyEnabled(value);
				}

				void ScrollViewStyleController::UpdateTable()
				{
					if (horizontalScroll->GetEnabled() || horizontalAlwaysVisible)
					{
						tableComposition->SetRowOption(1, CellOption::AbsoluteOption(styleProvider->GetDefaultScrollSize()));
					}
					else
					{
						tableComposition->SetRowOption(1, CellOption::AbsoluteOption(0));
					}
					if (verticalScroll->GetEnabled() || verticalAlwaysVisible)
					{
						tableComposition->SetColumnOption(1, CellOption::AbsoluteOption(styleProvider->GetDefaultScrollSize()));
					}
					else
					{
						tableComposition->SetColumnOption(1, CellOption::AbsoluteOption(0));
					}
					tableComposition->UpdateCellBounds();
				}

				ScrollContainerStyleController::ScrollContainerStyleController(PassRefPtr<IScrollViewStyleProvider> styleProvider)
					: ScrollViewStyleController(styleProvider)
					, extendToFullWidth(false)
				{
					controlContainerComposition = adoptRef(new BoundsComposition);
					controlContainerComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					GetInternalContainerComposition()->AddChild(controlContainerComposition);
				}

				ScrollContainerStyleController::~ScrollContainerStyleController()
				{

				}

				PassRefPtr<Composition> ScrollContainerStyleController::GetContainerComposition()
				{
					return controlContainerComposition;
				}

				void ScrollContainerStyleController::MoveContainer(CPoint leftTop)
				{
					controlContainerComposition->SetBounds(CRect(leftTop, CSize()));
				}

				bool ScrollContainerStyleController::GetExtendToFullWidth()
				{
					return extendToFullWidth;
				}

				void ScrollContainerStyleController::SetExtendToFullWidth(bool value)
				{
					if (extendToFullWidth != value)
					{
						extendToFullWidth = value;
						if (value)
						{
							controlContainerComposition->SetAlignmentToParent(CRect(0, -1, 0, -1));
						}
						else
						{
							controlContainerComposition->SetAlignmentToParent(CRect(-1, -1, -1, -1));
						}
					}
				}

				Win8ScrollViewProvider::Win8ScrollViewProvider()
				{

				}

				Win8ScrollViewProvider::~Win8ScrollViewProvider()
				{

				}

				PassRefPtr<IScrollStyleController> Win8ScrollViewProvider::CreateHorizontalScrollStyle()
				{
					return adoptRef(new Win8ScrollStyle(Win8ScrollStyle::Horizontal));
				}

				PassRefPtr<IScrollStyleController> Win8ScrollViewProvider::CreateVerticalScrollStyle()
				{
					return adoptRef(new Win8ScrollStyle(Win8ScrollStyle::Vertical));
				}

				cint Win8ScrollViewProvider::GetDefaultScrollSize()
				{
					return Win8ScrollStyle::DefaultSize;
				}

				PassRefPtr<Composition> Win8ScrollViewProvider::InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)
				{
					RefPtr<SolidBorderElement> border = SolidBorderElement::Create();
					border->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Border));
					boundsComposition->SetOwnedElement(border);
					boundsComposition->SetInternalMargin(CRect(1, 1, 1, 1));

					RefPtr<SolidBackgroundElement> background = SolidBackgroundElement::Create();
					background->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));

					RefPtr<BoundsComposition> backgroundComposition = adoptRef(new BoundsComposition);
					boundsComposition->AddChild(backgroundComposition);
					backgroundComposition->SetAlignmentToParent(CRect(0, 0, 0, 0));
					backgroundComposition->SetOwnedElement(background);

					return boundsComposition;
				}

				void Win8ScrollViewProvider::AssociateStyleController(PassRefPtr<IControlStyleController> controller)
				{

				}

				void Win8ScrollViewProvider::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8ScrollViewProvider::SetText(const CString& value)
				{

				}

				void Win8ScrollViewProvider::SetFont(const Font& value)
				{

				}

				void Win8ScrollViewProvider::SetVisuallyEnabled(bool value)
				{

				}

				ScrollStyleBase::ScrollStyleBase(Direction _direction)
					: direction(_direction)
					, commandExecutor(0)
					, totalSize(1)
					, pageSize(1)
					, position(0)
					, draggingHandle(false)
				{

				}

				ScrollStyleBase::~ScrollStyleBase()
				{

				}

				PassRefPtr<BoundsComposition> ScrollStyleBase::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> ScrollStyleBase::GetContainerComposition()
				{
					return containerComposition;
				}

				void ScrollStyleBase::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void ScrollStyleBase::SetText(const CString& value)
				{

				}

				void ScrollStyleBase::SetFont(const Font& value)
				{

				}

				void ScrollStyleBase::SetVisuallyEnabled(bool value)
				{

				}

				void ScrollStyleBase::SetCommandExecutor(PassRefPtr<IScrollCommandExecutor> value)
				{
					commandExecutor = value;
				}

				void ScrollStyleBase::SetTotalSize(cint value)
				{
					if (totalSize != value)
					{
						totalSize = value;
						UpdateHandle();
					}
				}

				void ScrollStyleBase::SetPageSize(cint value)
				{
					if (pageSize != value)
					{
						pageSize = value;
						UpdateHandle();
					}
				}

				void ScrollStyleBase::SetPosition(cint value)
				{
					if (position != value)
					{
						position = value;
						UpdateHandle();
					}
				}

				void ScrollStyleBase::UpdateHandle()
				{
					double handlePageSize = (double)pageSize / (double)totalSize;
					double handleRatio = (double)position / (double)totalSize;
					switch (direction)
					{
						case Horizontal:
							handleComposition->SetWidthRatio(handleRatio);
							handleComposition->SetWidthPageSize(handlePageSize);
							break;
						case Vertical:
							handleComposition->SetHeightRatio(handleRatio);
							handleComposition->SetHeightPageSize(handlePageSize);
							break;
					}
				}

				void ScrollStyleBase::OnDecreaseButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					if (commandExecutor)
					{
						commandExecutor->SmallDecrease();
					}
				}

				void ScrollStyleBase::OnIncreaseButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					if (commandExecutor)
					{
						commandExecutor->SmallIncrease();
					}
				}

				void ScrollStyleBase::OnHandleMouseDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
				{
					if (commandExecutor && handleButton->GetVisuallyEnabled())
					{
						draggingHandle = true;
						draggingStartLocation = arguments.pt;
					}
				}

				void ScrollStyleBase::OnHandleMouseMove(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
				{
					if (draggingHandle)
					{
						cint totalPixels = 0;
						cint currentOffset = 0;
						cint newOffset = 0;
						switch (direction)
						{
							case Horizontal:
								totalPixels = handleComposition->GetParent()->GetBounds().Width();
								currentOffset = handleComposition->GetBounds().left;
								newOffset = currentOffset + (arguments.pt.x - draggingStartLocation.x);
								break;
							case Vertical:
								totalPixels = handleComposition->GetParent()->GetBounds().Height();
								currentOffset = handleComposition->GetBounds().top;
								newOffset = currentOffset + (arguments.pt.y - draggingStartLocation.y);
								break;
						}

						double ratio = (double)newOffset / (double)totalPixels;
						cint newPosition = (cint)(ratio * totalSize);

						cint offset1 = (cint)(((double)newPosition / (double)totalSize) * (double)totalPixels);
						cint offset2 = (cint)(((double)(newPosition + 1) / (double)totalSize) * (double)totalPixels);
						cint delta1 = abs((int)(offset1 - newOffset));
						cint delta2 = abs((int)(offset2 - newOffset));
						if (delta1 < delta2)
						{
							commandExecutor->SetPosition(newPosition);
						}
						else
						{
							commandExecutor->SetPosition(newPosition + 1);
						}
					}
				}

				void ScrollStyleBase::OnHandleMouseUp(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
				{
					draggingHandle = false;
				}

				void ScrollStyleBase::OnBigMoveMouseDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
				{
					if (commandExecutor && handleButton->GetVisuallyEnabled())
					{
						if (arguments.eventSource == arguments.compositionSource)
						{
							CRect handleBounds = handleComposition->GetBounds();
							switch (direction)
							{
								case Horizontal:
									if (arguments.pt.x < handleBounds.left)
									{
										commandExecutor->BigDecrease();
									}
									else if (arguments.pt.x >= handleBounds.right)
									{
										commandExecutor->BigIncrease();
									}
									break;
								case Vertical:
									if (arguments.pt.y < handleBounds.top)
									{
										commandExecutor->BigDecrease();
									}
									else if (arguments.pt.y >= handleBounds.bottom)
									{
										commandExecutor->BigIncrease();
									}
									break;
							}
						}
					}
				}

				void ScrollStyleBase::BuildStyle(cint defaultSize, cint arrowSize)
				{
					boundsComposition = adoptRef(new BoundsComposition);
					containerComposition = InstallBackground(boundsComposition, direction);
					{
						RefPtr<BoundsComposition> handleBoundsComposition = adoptRef(new BoundsComposition);
						containerComposition->AddChild(handleBoundsComposition);

						handleComposition = adoptRef(new PartialViewComposition);
						handleBoundsComposition->AddChild(handleComposition);
						handleBoundsComposition->GetEventReceiver()->leftButtonDown.AttachMethod(this, &ScrollStyleBase::OnBigMoveMouseDown);

						switch (direction)
						{
							case Horizontal:
								handleBoundsComposition->SetAlignmentToParent(CRect(defaultSize, 0, defaultSize, 0));
								handleComposition->SetPreferredMinSize(CSize(defaultSize / 2, 0));
								boundsComposition->SetPreferredMinSize(CSize(0, defaultSize));
								break;
							case Vertical:
								handleBoundsComposition->SetAlignmentToParent(CRect(0, defaultSize, 0, defaultSize));
								handleComposition->SetPreferredMinSize(CSize(0, defaultSize / 2));
								boundsComposition->SetPreferredMinSize(CSize(defaultSize, 0));
								break;
						}

						handleButton = adoptRef(new Button(CreateHandleButtonStyle(direction)));
						handleButton->GetBoundsComposition()->SetAlignmentToParent(CRect(1, 1, 1, 1));
						handleComposition->AddChild(handleButton->GetBoundsComposition());

						handleButton->GetBoundsComposition()->GetEventReceiver()->leftButtonDown.AttachMethod(this, &ScrollStyleBase::OnHandleMouseDown);
						handleButton->GetBoundsComposition()->GetEventReceiver()->mouseMove.AttachMethod(this, &ScrollStyleBase::OnHandleMouseMove);
						handleButton->GetBoundsComposition()->GetEventReceiver()->leftButtonUp.AttachMethod(this, &ScrollStyleBase::OnHandleMouseUp);
					}
					{
						decreaseButton = adoptRef(new Button(CreateDecreaseButtonStyle(direction)));
						decreaseButton->GetBoundsComposition()->SetAlignmentToParent(CRect(1, 1, 1, 1));
						decreaseButton->Clicked.AttachMethod(this, &ScrollStyleBase::OnDecreaseButtonClicked);

						increaseButton = adoptRef(new Button(CreateIncreaseButtonStyle(direction)));
						increaseButton->GetBoundsComposition()->SetAlignmentToParent(CRect(1, 1, 1, 1));
						increaseButton->Clicked.AttachMethod(this, &ScrollStyleBase::OnIncreaseButtonClicked);
					}
					{
						RefPtr<SideAlignedComposition> decreaseComposition = adoptRef(new SideAlignedComposition);
						decreaseComposition->SetMaxLength(defaultSize);
						decreaseComposition->SetMaxRatio(0.5);
						decreaseComposition->AddChild(decreaseButton->GetBoundsComposition());
						containerComposition->AddChild(decreaseComposition);

						RefPtr<SideAlignedComposition> increaseComposition = adoptRef(new SideAlignedComposition);
						increaseComposition->SetMaxLength(defaultSize);
						increaseComposition->SetMaxRatio(0.5);
						increaseComposition->AddChild(increaseButton->GetBoundsComposition());
						containerComposition->AddChild(increaseComposition);

						switch (direction)
						{
							case Horizontal:
							{
								decreaseComposition->SetDirection(SideAlignedComposition::Left);
								increaseComposition->SetDirection(SideAlignedComposition::Right);
							}
								break;
							case Vertical:
							{
								decreaseComposition->SetDirection(SideAlignedComposition::Top);
								increaseComposition->SetDirection(SideAlignedComposition::Bottom);
							}
								break;
						}
					}
				}

				Win8ScrollStyle::Win8ScrollStyle(Direction _direction)
					: ScrollStyleBase(_direction)
				{
					BuildStyle(DefaultSize, ArrowSize);
				}

				Win8ScrollStyle::~Win8ScrollStyle()
				{

				}

				PassRefPtr<IButtonStyleController> Win8ScrollStyle::CreateDecreaseButtonStyle(Direction direction)
				{
					RefPtr<Win8ScrollArrowButtonStyle> decreaseButtonStyle = adoptRef(new Win8ScrollArrowButtonStyle(direction, false));
					return decreaseButtonStyle;
				}

				PassRefPtr<IButtonStyleController> Win8ScrollStyle::CreateIncreaseButtonStyle(Direction direction)
				{
					RefPtr<Win8ScrollArrowButtonStyle> increaseButtonStyle = adoptRef(new Win8ScrollArrowButtonStyle(direction, true));
					return increaseButtonStyle;
				}

				PassRefPtr<IButtonStyleController> Win8ScrollStyle::CreateHandleButtonStyle(Direction direction)
				{
					RefPtr<Win8ScrollHandleButtonStyle> handleButtonStyle = adoptRef(new Win8ScrollHandleButtonStyle);
					return handleButtonStyle;
				}

				PassRefPtr<BoundsComposition> Win8ScrollStyle::InstallBackground(PassRefPtr<BoundsComposition> boundsComposition, Direction direction)
				{
					{
						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));

						boundsComposition->SetOwnedElement(element);
					}

					return boundsComposition;
				}

				Win8ScrollArrowButtonStyle::Win8ScrollArrowButtonStyle(ScrollStyleBase::Direction direction, bool increaseButton)
					: Win8ButtonStyleBase(Win8ButtonColors::ScrollArrowNormal(), Alignment::StringAlignmentCenter, Alignment::StringAlignmentCenter)
				{
					switch (direction)
					{
						case ScrollStyleBase::Horizontal:
							if (increaseButton)
							{
								GetContainerComposition()->AddChild(FragmentBuilder::BuildRightArrow(arrowElement));
							}
							else
							{
								GetContainerComposition()->AddChild(FragmentBuilder::BuildLeftArrow(arrowElement));
							}
							break;
						case ScrollStyleBase::Vertical:
							if (increaseButton)
							{
								GetContainerComposition()->AddChild(FragmentBuilder::BuildDownArrow(arrowElement));
							}
							else
							{
								GetContainerComposition()->AddChild(FragmentBuilder::BuildUpArrow(arrowElement));
							}
							break;
					}
				}

				Win8ScrollArrowButtonStyle::~Win8ScrollArrowButtonStyle()
				{

				}

				void Win8ScrollArrowButtonStyle::TransferInternal(Button::ControlState value, bool enabled, bool selected)
				{
					Win8ButtonColors targetColor;
					if (enabled)
					{
						switch (value)
						{
							case Button::Normal:
								targetColor = Win8ButtonColors::ScrollArrowNormal();
								break;
							case Button::Active:
								targetColor = Win8ButtonColors::ScrollArrowActive();
								break;
							case Button::Pressed:
								targetColor = Win8ButtonColors::ScrollArrowPressed();
								break;
						}
					}
					else
					{
						targetColor = Win8ButtonColors::ScrollArrowDisabled();
					}
					transferringAnimation->Transfer(targetColor);
				}

				void Win8ScrollArrowButtonStyle::AfterApplyColors(const Win8ButtonColors& colors)
				{
					Win8ButtonStyleBase::AfterApplyColors(colors);
					arrowElement->SetBorderColor(colors.textColor);
					arrowElement->SetBackgroundColor(colors.textColor);
				}

				Win8ScrollHandleButtonStyle::Win8ScrollHandleButtonStyle()
					: Win8ButtonStyleBase(Win8ButtonColors::ScrollHandleNormal(), Alignment::StringAlignmentCenter, Alignment::StringAlignmentCenter)
				{

				}

				Win8ScrollHandleButtonStyle::~Win8ScrollHandleButtonStyle()
				{

				}

				void Win8ScrollHandleButtonStyle::TransferInternal(Button::ControlState value, bool enabled, bool selected)
				{
					Win8ButtonColors targetColor;
					if (enabled)
					{
						switch (value)
						{
							case Button::Normal:
								targetColor = Win8ButtonColors::ScrollHandleNormal();
								break;
							case Button::Active:
								targetColor = Win8ButtonColors::ScrollHandleActive();
								break;
							case Button::Pressed:
								targetColor = Win8ButtonColors::ScrollHandlePressed();
								break;
						}
					}
					else
					{
						targetColor = Win8ButtonColors::ScrollHandleDisabled();
					}
					transferringAnimation->Transfer(targetColor);
				}
			}

			ScrollCommandExecutor::ScrollCommandExecutor(PassRefPtr<Scroll> _scroll)
				: scroll(_scroll)
			{

			}

			ScrollCommandExecutor::~ScrollCommandExecutor()
			{

			}

			void ScrollCommandExecutor::SmallDecrease()
			{
				scroll->SetPosition(scroll->GetPosition() - scroll->GetSmallMove());
			}

			void ScrollCommandExecutor::SmallIncrease()
			{
				scroll->SetPosition(scroll->GetPosition() + scroll->GetSmallMove());
			}

			void ScrollCommandExecutor::BigDecrease()
			{
				scroll->SetPosition(scroll->GetPosition() - scroll->GetBigMove());
			}

			void ScrollCommandExecutor::BigIncrease()
			{
				scroll->SetPosition(scroll->GetPosition() + scroll->GetBigMove());
			}

			void ScrollCommandExecutor::SetTotalSize(cint value)
			{
				scroll->SetTotalSize(value);
			}

			void ScrollCommandExecutor::SetPageSize(cint value)
			{
				scroll->SetPageSize(value);
			}

			void ScrollCommandExecutor::SetPosition(cint value)
			{
				scroll->SetPosition(value);
			}

			Scroll::Scroll(PassRefPtr<IScrollStyleController> _styleController)
				: Control(_styleController.get())
				, styleController(_styleController.get())
				, totalSize(100)
				, pageSize(10)
				, position(0)
				, smallMove(1)
				, bigMove(10)
			{
				TotalSizeChanged.SetAssociatedComposition(boundsComposition);
				PageSizeChanged.SetAssociatedComposition(boundsComposition);
				PositionChanged.SetAssociatedComposition(boundsComposition);
				SmallMoveChanged.SetAssociatedComposition(boundsComposition);
				BigMoveChanged.SetAssociatedComposition(boundsComposition);

				commandExecutor = adoptRef(new ScrollCommandExecutor(this));
				styleController->SetCommandExecutor(commandExecutor);
				styleController->SetPageSize(pageSize);
				styleController->SetTotalSize(totalSize);
				styleController->SetPosition(position);
			}

			Scroll::~Scroll()
			{

			}

			cint Scroll::GetTotalSize()
			{
				return totalSize;
			}

			void Scroll::SetTotalSize(cint value)
			{
				if (totalSize != value && 0 < value)
				{
					totalSize = value;
					if (pageSize > totalSize)
					{
						SetPageSize(totalSize);
					}
					if (position > GetMaxPosition())
					{
						SetPosition(GetMaxPosition());
					}
					styleController->SetTotalSize(totalSize);
					TotalSizeChanged.Execute(GetNotifyEventArguments());
				}
			}

			cint Scroll::GetPageSize()
			{
				return pageSize;
			}

			void Scroll::SetPageSize(cint value)
			{
				if (pageSize != value && 0 <= value && value <= totalSize)
				{
					pageSize = value;
					if (position > GetMaxPosition())
					{
						SetPosition(GetMaxPosition());
					}
					styleController->SetPageSize(pageSize);
					PageSizeChanged.Execute(GetNotifyEventArguments());
				}
			}

			cint Scroll::GetPosition()
			{
				return position;
			}

			void Scroll::SetPosition(cint value)
			{
				cint min = GetMinPosition();
				cint max = GetMaxPosition();
				cint newPosition =
					value < min ? min :
					value > max ? max :
					value;
				if (position != newPosition)
				{
					position = newPosition;
					styleController->SetPosition(position);
					PositionChanged.Execute(GetNotifyEventArguments());
				}
			}

			cint Scroll::GetSmallMove()
			{
				return smallMove;
			}

			void Scroll::SetSmallMove(cint value)
			{
				if (value > 0 && smallMove != value)
				{
					smallMove = value;
					SmallMoveChanged.Execute(GetNotifyEventArguments());
				}
			}

			cint Scroll::GetBigMove()
			{
				return bigMove;
			}

			void Scroll::SetBigMove(cint value)
			{
				if (value > 0 && bigMove != value)
				{
					bigMove = value;
					BigMoveChanged.Execute(GetNotifyEventArguments());
				}
			}

			cint Scroll::GetMinPosition()
			{
				return 0;
			}

			cint Scroll::GetMaxPosition()
			{
				return totalSize - pageSize;
			}

			ScrollView::ScrollView(PassRefPtr<IScrollViewStyleProvider> styleProvider)
				: Control(adoptRef(new ScrollViewStyleController(styleProvider)))
				, supressScrolling(false)
			{
				Initialize();
			}

			ScrollView::ScrollView(PassRefPtr<ScrollViewStyleController> _styleController)
				: Control(_styleController)
				, supressScrolling(false)
			{
				Initialize();
			}

			ScrollView::~ScrollView()
			{

			}

			void ScrollView::SetFont(const Font& value)
			{
				Control::SetFont(value);
				CalculateView();
			}

			void ScrollView::CalculateView()
			{
				if (!supressScrolling)
				{
					CSize fullSize = QueryFullSize();
					while (true)
					{
						styleController->AdjustView(fullSize);
						supressScrolling = true;
						CallUpdateView();
						supressScrolling = false;

						CSize newSize = QueryFullSize();
						if (fullSize == newSize)
						{
							cint smallMove = GetSmallMove();
							styleController->GetHorizontalScroll()->SetSmallMove(smallMove);
							styleController->GetVerticalScroll()->SetSmallMove(smallMove);
							CSize bigMove = GetBigMove();
							styleController->GetHorizontalScroll()->SetBigMove(bigMove.cx);
							styleController->GetVerticalScroll()->SetBigMove(bigMove.cy);
							break;
						}
						else
						{
							fullSize = newSize;
						}
					}
				}
			}

			CSize ScrollView::GetViewSize()
			{
				CSize viewSize = styleController->GetInternalContainerComposition()->GetBounds().Size();
				return viewSize;
			}

			CRect ScrollView::GetViewBounds()
			{
				CPoint viewPosition =
					CPoint(
					styleController->GetHorizontalScroll()->GetPosition(),
					styleController->GetVerticalScroll()->GetPosition()
					);
				CSize viewSize = GetViewSize();
				return CRect(viewPosition, viewSize);
			}

			PassRefPtr<Scroll> ScrollView::GetHorizontalScroll()
			{
				return styleController->GetHorizontalScroll();
			}

			PassRefPtr<Scroll> ScrollView::GetVerticalScroll()
			{
				return styleController->GetVerticalScroll();
			}

			bool ScrollView::GetHorizontalAlwaysVisible()
			{
				return styleController->GetHorizontalAlwaysVisible();
			}

			void ScrollView::SetHorizontalAlwaysVisible(bool value)
			{
				styleController->SetHorizontalAlwaysVisible(value);
			}

			bool ScrollView::GetVerticalAlwaysVisible()
			{
				return styleController->GetVerticalAlwaysVisible();
			}

			void ScrollView::SetVerticalAlwaysVisible(bool value)
			{
				styleController->SetVerticalAlwaysVisible(value);
			}

			void ScrollView::OnContainerBoundsChanged(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				CalculateView();
			}

			void ScrollView::OnHorizontalScroll(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (!supressScrolling)
				{
					CallUpdateView();
				}
			}

			void ScrollView::OnVerticalScroll(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (!supressScrolling)
				{
					CallUpdateView();
				}
			}

			void ScrollView::OnHorizontalWheel(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
			{
				if (!supressScrolling && GetVisuallyEnabled())
				{
					auto scroll = styleController->GetHorizontalScroll();
					if (scroll->GetEnabled())
					{
						cint position = scroll->GetPosition();
						cint move = scroll->GetSmallMove();
						position -= move*arguments.wheel / 60;
						scroll->SetPosition(position);
					}
				}
			}

			void ScrollView::OnVerticalWheel(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
			{
				if (!supressScrolling && GetVisuallyEnabled())
				{
					auto scroll = styleController->GetVerticalScroll();
					if (scroll->GetEnabled())
					{
						cint position = scroll->GetPosition();
						cint move = scroll->GetSmallMove();
						position -= move*arguments.wheel / 60;
						scroll->SetPosition(position);
					}
				}
			}

			void ScrollView::CallUpdateView()
			{
				CRect viewBounds = GetViewBounds();
				UpdateView(viewBounds);
			}

			void ScrollView::Initialize()
			{
				styleController = dynamic_cast<ScrollViewStyleController*>(GetStyleController().get());
				styleController->SetScrollView(this);

				styleController->GetInternalContainerComposition()->BoundsChanged.AttachMethod(this, &ScrollView::OnContainerBoundsChanged);
				styleController->GetHorizontalScroll()->PositionChanged.AttachMethod(this, &ScrollView::OnHorizontalScroll);
				styleController->GetVerticalScroll()->PositionChanged.AttachMethod(this, &ScrollView::OnVerticalScroll);
				styleController->GetBoundsComposition()->GetEventReceiver()->horizontalWheel.AttachMethod(this, &ScrollView::OnHorizontalWheel);
				styleController->GetBoundsComposition()->GetEventReceiver()->verticalWheel.AttachMethod(this, &ScrollView::OnVerticalWheel);
			}

			cint ScrollView::GetSmallMove()
			{
				return GetFont().size * 2;
			}

			CSize ScrollView::GetBigMove()
			{
				return GetViewSize();
			}

			ScrollContainer::ScrollContainer(PassRefPtr<IScrollViewStyleProvider> styleProvider)
				: ScrollView(adoptRef(dynamic_cast<ScrollViewStyleController*>(new ScrollContainerStyleController(styleProvider))))
			{
				styleController = dynamic_cast<ScrollContainerStyleController*>(GetStyleController().get());
				BoundsComposition* composition = dynamic_cast<BoundsComposition*>(styleController->GetContainerComposition().get());
				composition->BoundsChanged.AttachMethod(this, &ScrollContainer::OnControlContainerBoundsChanged);
			}

			ScrollContainer::~ScrollContainer()
			{

			}

			bool ScrollContainer::GetExtendToFullWidth()
			{
				return styleController->GetExtendToFullWidth();
			}

			void ScrollContainer::SetExtendToFullWidth(bool value)
			{
				styleController->SetExtendToFullWidth(value);
			}

			void ScrollContainer::OnControlContainerBoundsChanged(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				CalculateView();
			}

			CSize ScrollContainer::QueryFullSize()
			{
				return styleController->GetContainerComposition()->GetBounds().Size();
			}

			void ScrollContainer::UpdateView(CRect viewBounds)
			{
				styleController->MoveContainer(-viewBounds.TopLeft());
			}
		}
	}
}