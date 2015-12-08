#include "stdafx.h"
#include "cc_comctl.h"
#include "cc_button.h"
#include "cc_presentation.h"

using namespace cc::presentation::windows;
using namespace cc::presentation::element;
using namespace cc::presentation::control::helper;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			Button::Button(PassRefPtr<IButtonStyleController> _styleController)
				: Control(_styleController.get())
				, styleController(_styleController.get())
				, clickOnMouseUp(true)
				, mousePressing(false)
				, mouseHoving(false)
				, controlState(ControlState::Normal)
			{
				Clicked.SetAssociatedComposition(boundsComposition);
				styleController->Transfer(ControlState::Normal);
				SetFocusableComposition(boundsComposition);

				GetEventReceiver()->leftButtonDown.AttachMethod(this, &Button::OnLeftButtonDown);
				GetEventReceiver()->leftButtonUp.AttachMethod(this, &Button::OnLeftButtonUp);
				GetEventReceiver()->mouseEnter.AttachMethod(this, &Button::OnMouseEnter);
				GetEventReceiver()->mouseLeave.AttachMethod(this, &Button::OnMouseLeave);
			}

			Button::~Button()
			{

			}

			bool Button::GetClickOnMouseUp()
			{
				return clickOnMouseUp;
			}

			void Button::SetClickOnMouseUp(bool value)
			{
				clickOnMouseUp = value;
			}

			void Button::OnParentLineChanged()
			{
				Control::OnParentLineChanged();
				if (!GetRelatedControlHost())
				{
					mousePressing = false;
					mouseHoving = false;
					UpdateControlState();
				}
			}

			void Button::UpdateControlState()
			{
				ControlState newControlState = ControlState::Normal;
				if (mousePressing)
				{
					if (mouseHoving)
					{
						newControlState = ControlState::Pressed;
					}
					else
					{
						newControlState = ControlState::Active;
					}
				}
				else
				{
					if (mouseHoving)
					{
						newControlState = ControlState::Active;
					}
					else
					{
						newControlState = ControlState::Normal;
					}
				}
				if (controlState != newControlState)
				{
					controlState = newControlState;
					styleController->Transfer(controlState);
				}
			}

			void Button::OnLeftButtonDown(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (arguments.eventSource == boundsComposition)
				{
					mousePressing = true;
					boundsComposition->GetRelatedGraphicsHost()->SetFocus(boundsComposition);
					UpdateControlState();
					if (!clickOnMouseUp && arguments.eventSource->GetAssociatedControl() == this)
					{
						Clicked.Execute(GetNotifyEventArguments());
					}
				}
			}

			void Button::OnLeftButtonUp(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (arguments.eventSource == boundsComposition)
				{
					mousePressing = false;
					UpdateControlState();
				}
				if (GetVisuallyEnabled())
				{
					if (mouseHoving && clickOnMouseUp && arguments.eventSource->GetAssociatedControl() == this)
					{
						Clicked.Execute(GetNotifyEventArguments());
					}
				}
			}

			void Button::OnMouseEnter(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (arguments.eventSource == boundsComposition)
				{
					mouseHoving = true;
					UpdateControlState();
				}
			}

			void Button::OnMouseLeave(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (arguments.eventSource == boundsComposition)
				{
					mouseHoving = false;
					UpdateControlState();
				}
			}

			SelectableButton::SelectableButton(PassRefPtr<ISelectableButtonStyleController> _styleController)
				: Button(_styleController.get())
				, styleController(_styleController.get())
				, autoSelection(true)
				, isSelected(false)
			{
				GroupControllerChanged.SetAssociatedComposition(boundsComposition);
				AutoSelectionChanged.SetAssociatedComposition(boundsComposition);
				SelectedChanged.SetAssociatedComposition(boundsComposition);

				Clicked.AttachMethod(this, &SelectableButton::OnClicked);
				styleController->SetSelected(isSelected);
			}

			SelectableButton::~SelectableButton()
			{
				if (groupController)
				{
					groupController->Detach(this);
				}
			}

			PassRefPtr<GroupController> SelectableButton::GetGroupController()
			{
				return groupController;
			}

			void SelectableButton::SetGroupController(PassRefPtr<GroupController> value)
			{
				if (groupController)
				{
					groupController->Detach(this);
				}
				groupController = value;
				if (groupController)
				{
					groupController->Attach(this);
				}
				GroupControllerChanged.Execute(GetNotifyEventArguments());
			}

			bool SelectableButton::GetAutoSelection()
			{
				return autoSelection;
			}

			void SelectableButton::SetAutoSelection(bool value)
			{
				if (autoSelection != value)
				{
					autoSelection = value;
					AutoSelectionChanged.Execute(GetNotifyEventArguments());
				}
			}

			bool SelectableButton::GetSelected()
			{
				return isSelected;
			}

			void SelectableButton::SetSelected(bool value)
			{
				if (isSelected != value)
				{
					isSelected = value;
					styleController->SetSelected(isSelected);
					if (groupController)
					{
						groupController->OnSelectedChanged(this);
					}
					SelectedChanged.Execute(GetNotifyEventArguments());
				}
			}

			void SelectableButton::OnClicked(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (autoSelection)
				{
					SetSelected(!GetSelected());
				}
			}

			namespace style
			{
				PassRefPtr<IButtonStyleController> Win8Style::CreateButtonStyle()
				{
					return adoptRef(new Win8ButtonStyle);
				}

				PassRefPtr<ISelectableButtonStyleController> Win8Style::CreateCheckBoxStyle()
				{
					return adoptRef(new Win8CheckBoxStyle(Win8CheckBoxStyle::CheckBox));
				}

				PassRefPtr<ISelectableButtonStyleController> Win8Style::CreateRadioButtonStyle()
				{
					return adoptRef(new Win8CheckBoxStyle(Win8CheckBoxStyle::RadioButton));
				}

				PassRefPtr<IControlStyleController> Win8Style::CreateGroupBoxStyle()
				{
					return adoptRef(new Win8GroupBoxStyle);
				}

				Win8ButtonStyleBase::Win8ButtonStyleBase(const Win8ButtonColors& initialColor, Alignment horizontal, Alignment vertical)
					: controlStyle(Button::Normal)
					, isVisuallyEnabled(true)
					, isSelected(false)
					, transparentWhenInactive(false)
					, transparentWhenDisabled(false)
				{
					elements = Win8ButtonElements::Create(horizontal, vertical);
					elements.Apply(initialColor);
					transferringAnimation = adoptRef(new TransferringAnimationType(this, initialColor));
				}

				Win8ButtonStyleBase::~Win8ButtonStyleBase()
				{
					transferringAnimation->Disable();
				}

				PassRefPtr<BoundsComposition> Win8ButtonStyleBase::GetBoundsComposition()
				{
					return elements.mainComposition;
				}

				PassRefPtr<Composition> Win8ButtonStyleBase::GetContainerComposition()
				{
					return elements.mainComposition;
				}

				void Win8ButtonStyleBase::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8ButtonStyleBase::SetText(const CString& value)
				{
					elements.textElement->SetText(value);
				}

				void Win8ButtonStyleBase::SetFont(const Font& value)
				{
					elements.textElement->SetFont(value);
					elements.textComposition->SetMargin(CRect(3, 3, 3, 3));
				}

				void Win8ButtonStyleBase::SetVisuallyEnabled(bool value)
				{
					if (isVisuallyEnabled != value)
					{
						isVisuallyEnabled = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected);
					}
				}

				void Win8ButtonStyleBase::SetSelected(bool value)
				{
					if (isSelected != value)
					{
						isSelected = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected);
					}
				}

				void Win8ButtonStyleBase::Transfer(Button::ControlState value)
				{
					if (controlStyle != value)
					{
						controlStyle = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected);
					}
				}

				bool Win8ButtonStyleBase::GetTransparentWhenInactive()
				{
					return transparentWhenInactive;
				}

				void Win8ButtonStyleBase::SetTransparentWhenInactive(bool value)
				{
					transparentWhenInactive = value;
					TransferInternal(controlStyle, isVisuallyEnabled, isSelected);
				}

				bool Win8ButtonStyleBase::GetTransparentWhenDisabled()
				{
					return transparentWhenDisabled;
				}

				void Win8ButtonStyleBase::SetTransparentWhenDisabled(bool value)
				{
					transparentWhenDisabled = value;
					TransferInternal(controlStyle, isVisuallyEnabled, isSelected);
				}

				bool Win8ButtonStyleBase::GetAutoSizeForText()
				{
					return elements.textComposition->GetMinSizeLimitation() != Composition::NoLimit;
				}

				void Win8ButtonStyleBase::SetAutoSizeForText(bool value)
				{
					if (value)
					{
						elements.textComposition->SetMinSizeLimitation(Composition::LimitToElement);
					}
					else
					{
						elements.textComposition->SetMinSizeLimitation(Composition::NoLimit);
					}
				}

				void Win8ButtonStyleBase::ToAnimation(PassRefPtr<GraphicsAnimationManager> manager)
				{
					manager->AddAnimation(transferringAnimation);
				}

				void Win8ButtonStyleBase::AfterApplyColors(const Win8ButtonColors& colors)
				{

				}

				Win8ButtonStyleBase::TransferringAnimationType::TransferringAnimationType(Win8ButtonStyleBase* _style, const Win8ButtonColors& begin) : TransferringAnimation(_style, begin)
				{

				}

				void Win8ButtonStyleBase::TransferringAnimationType::PlayInternal(cint currentPosition, cint totalLength)
				{
					colorCurrent = Win8ButtonColors::Blend(colorBegin, colorEnd, currentPosition, totalLength);
					style->elements.Apply(colorCurrent);
					style->AfterApplyColors(colorCurrent);
				}

				Win8ButtonStyle::Win8ButtonStyle()
					: Win8ButtonStyleBase(Win8ButtonColors::ButtonNormal(), Alignment::StringAlignmentCenter, Alignment::StringAlignmentCenter)
				{

				}

				Win8ButtonStyle::~Win8ButtonStyle()
				{

				}

				void Win8ButtonStyle::TransferInternal(Button::ControlState value, bool enabled, bool selected)
				{
					Win8ButtonColors targetColor;
					if (enabled)
					{
						switch (value)
						{
							case Button::Normal:
								targetColor = Win8ButtonColors::ButtonNormal();
								if (transparentWhenInactive)
								{
									targetColor.SetAlphaWithoutText(0);
								}
								break;
							case Button::Active:
								targetColor = Win8ButtonColors::ButtonActive();
								break;
							case Button::Pressed:
								targetColor = Win8ButtonColors::ButtonPressed();
								break;
						}
					}
					else
					{
						targetColor = Win8ButtonColors::ButtonDisabled();
						if (transparentWhenDisabled)
						{
							targetColor.SetAlphaWithoutText(0);
						}
					}
					transferringAnimation->Transfer(targetColor);
				}

				Win8CheckBoxStyle::Win8CheckBoxStyle(BulletStyle bulletStyle, bool backgroundVisible /*= false*/)
					: controlStyle(Button::Normal)
					, isVisuallyEnabled(true)
					, isSelected(false)
				{
					Win8ButtonColors initialColor = Win8ButtonColors::CheckedNormal(isSelected);
					elements = Win8CheckedButtonElements::Create(bulletStyle == CheckBox ? ElementShape::Rectangle : ElementShape::Ellipse, backgroundVisible);
					elements.Apply(initialColor);
					transferringAnimation = adoptRef(new TransferringAnimationType(this, initialColor));
				}

				Win8CheckBoxStyle::~Win8CheckBoxStyle()
				{
					transferringAnimation->Disable();
				}

				PassRefPtr<BoundsComposition> Win8CheckBoxStyle::GetBoundsComposition()
				{
					return elements.mainComposition;
				}

				PassRefPtr<Composition> Win8CheckBoxStyle::GetContainerComposition()
				{
					return elements.mainComposition;
				}

				void Win8CheckBoxStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8CheckBoxStyle::SetText(const CString& value)
				{
					elements.textElement->SetText(value);
				}

				void Win8CheckBoxStyle::SetFont(const Font& value)
				{
					elements.textElement->SetFont(value);
					elements.textComposition->SetMargin(CRect(3, 3, 3, 3));
				}

				void Win8CheckBoxStyle::SetVisuallyEnabled(bool value)
				{
					if (isVisuallyEnabled != value)
					{
						isVisuallyEnabled = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected);
					}
				}

				void Win8CheckBoxStyle::SetSelected(bool value)
				{
					if (isSelected != value)
					{
						isSelected = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected);
					}
				}

				void Win8CheckBoxStyle::Transfer(Button::ControlState value)
				{
					if (controlStyle != value)
					{
						controlStyle = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected);
					}
				}

				void Win8CheckBoxStyle::ToAnimation(PassRefPtr<GraphicsAnimationManager> manager)
				{
					manager->AddAnimation(transferringAnimation);
				}

				void Win8CheckBoxStyle::TransferInternal(Button::ControlState value, bool enabled, bool selected)
				{
					if (enabled)
					{
						switch (value)
						{
							case Button::Normal:
								transferringAnimation->Transfer(Win8ButtonColors::CheckedNormal(selected));
								break;
							case Button::Active:
								transferringAnimation->Transfer(Win8ButtonColors::CheckedActive(selected));
								break;
							case Button::Pressed:
								transferringAnimation->Transfer(Win8ButtonColors::CheckedPressed(selected));
								break;
						}
					}
					else
					{
						transferringAnimation->Transfer(Win8ButtonColors::CheckedDisabled(selected));
					}
				}

				Win8CheckBoxStyle::TransferringAnimationType::TransferringAnimationType(Win8CheckBoxStyle* _style, const Win8ButtonColors& begin) : TransferringAnimation(_style, begin)
				{

				}

				void Win8CheckBoxStyle::TransferringAnimationType::PlayInternal(cint currentPosition, cint totalLength)
				{
					colorCurrent = Win8ButtonColors::Blend(colorBegin, colorEnd, currentPosition, totalLength);
					style->elements.Apply(colorCurrent);
				}

				Win8GroupBoxStyle::Win8GroupBoxStyle()
				{
					boundsComposition = adoptRef(new BoundsComposition);
					{
						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));

						boundsComposition->SetOwnedElement(element);
						boundsComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					}

					borderComposition = adoptRef(new BoundsComposition);
					{
						RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
						element->SetColor(CColor(221, 221, 221));

						borderComposition->SetOwnedElement(element);
						boundsComposition->AddChild(borderComposition);
					}

					textBackgroundComposition = adoptRef(new BoundsComposition);
					{
						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));

						textBackgroundComposition->SetOwnedElement(element);
						textBackgroundComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
						boundsComposition->AddChild(textBackgroundComposition);
					}

					textComposition = adoptRef(new BoundsComposition);
					{
						RefPtr<SolidLabelElement> element = SolidLabelElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable));
						textElement = element;

						textComposition->SetOwnedElement(element);
						textComposition->SetAlignmentToParent(CRect());
						textComposition->SetMinSizeLimitation(Composition::LimitToElement);
						textBackgroundComposition->AddChild(textComposition);
					}

					containerComposition = adoptRef(new BoundsComposition);
					{
						boundsComposition->AddChild(containerComposition);
					}

					SetMargins(0);
					transferringAnimation = adoptRef(new TransferringAnimationType(this, Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable)));
				}

				Win8GroupBoxStyle::~Win8GroupBoxStyle()
				{
					transferringAnimation->Disable();
				}

				PassRefPtr<BoundsComposition> Win8GroupBoxStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8GroupBoxStyle::GetContainerComposition()
				{
					return containerComposition;
				}

				void Win8GroupBoxStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8GroupBoxStyle::SetText(const CString& value)
				{
					textElement->SetText(value);
				}

				void Win8GroupBoxStyle::SetFont(const Font& value)
				{
					textElement->SetFont(value);
					SetMargins(value.size);
				}

				void Win8GroupBoxStyle::SetVisuallyEnabled(bool value)
				{
					if (value)
					{
						transferringAnimation->Transfer(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable));
					}
					else
					{
						transferringAnimation->Transfer(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Disable));
					}
				}

				void Win8GroupBoxStyle::ToAnimation(PassRefPtr<GraphicsAnimationManager> manager)
				{
					manager->AddAnimation(transferringAnimation);
				}

				void Win8GroupBoxStyle::SetMargins(cint fontSize)
				{
					fontSize += 4;
					cint half = fontSize / 2;
					borderComposition->SetAlignmentToParent(CRect(0, half, 0, 0));
					containerComposition->SetAlignmentToParent(CRect(1, fontSize, 1, 1));
					textBackgroundComposition->SetAlignmentToParent(CRect(half, 2, -1, -1));
				}

				Win8GroupBoxStyle::TransferringAnimationType::TransferringAnimationType(Win8GroupBoxStyle* _style, const CColor& begin) : TransferringAnimation(_style, begin)
				{

				}

				void Win8GroupBoxStyle::TransferringAnimationType::PlayInternal(cint currentPosition, cint totalLength)
				{
					colorCurrent = BlendColor(colorBegin, colorEnd, currentPosition, totalLength);
					style->textElement->SetColor(colorCurrent);
				}
			}
		}
	}
}