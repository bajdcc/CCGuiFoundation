#ifndef CC_BUTTON
#define CC_BUTTON

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
			namespace style
			{
				class IButtonStyleController;
				class ISelectableButtonStyleController;

				struct Win8ButtonColors
				{
					CColor										borderColor;
					CColor										g1;
					CColor										g2;
					CColor										textColor;
					CColor										bullet;

					bool operator==(const Win8ButtonColors& colors);
					bool operator!=(const Win8ButtonColors& colors);

					void										SetAlphaWithoutText(BYTE a);

					static Win8ButtonColors						Blend(const Win8ButtonColors& c1, const Win8ButtonColors& c2, cint ratio, cint total);

					static Win8ButtonColors						ButtonNormal();
					static Win8ButtonColors						ButtonActive();
					static Win8ButtonColors						ButtonPressed();
					static Win8ButtonColors						ButtonDisabled();

					static Win8ButtonColors						ItemNormal();
					static Win8ButtonColors						ItemActive();
					static Win8ButtonColors						ItemSelected();
					static Win8ButtonColors						ItemDisabled();

					static Win8ButtonColors						CheckedNormal(bool selected);
					static Win8ButtonColors						CheckedActive(bool selected);
					static Win8ButtonColors						CheckedPressed(bool selected);
					static Win8ButtonColors						CheckedDisabled(bool selected);

					static Win8ButtonColors						ScrollHandleNormal();
					static Win8ButtonColors						ScrollHandleActive();
					static Win8ButtonColors						ScrollHandlePressed();
					static Win8ButtonColors						ScrollHandleDisabled();
					static Win8ButtonColors						ScrollArrowNormal();
					static Win8ButtonColors						ScrollArrowActive();
					static Win8ButtonColors						ScrollArrowPressed();
					static Win8ButtonColors						ScrollArrowDisabled();

					static Win8ButtonColors						ToolstripButtonNormal();
					static Win8ButtonColors						ToolstripButtonActive();
					static Win8ButtonColors						ToolstripButtonPressed();
					static Win8ButtonColors						ToolstripButtonSelected();
					static Win8ButtonColors						ToolstripButtonDisabled();

					static Win8ButtonColors						MenuBarButtonNormal();
					static Win8ButtonColors						MenuBarButtonActive();
					static Win8ButtonColors						MenuBarButtonPressed();
					static Win8ButtonColors						MenuBarButtonDisabled();

					static Win8ButtonColors						MenuItemButtonNormal();
					static Win8ButtonColors						MenuItemButtonNormalActive();
					static Win8ButtonColors						MenuItemButtonSelected();
					static Win8ButtonColors						MenuItemButtonSelectedActive();
					static Win8ButtonColors						MenuItemButtonDisabled();
					static Win8ButtonColors						MenuItemButtonDisabledActive();

					static Win8ButtonColors						TabPageHeaderNormal();
					static Win8ButtonColors						TabPageHeaderActive();
					static Win8ButtonColors						TabPageHeaderSelected();
				};

				struct Win8ButtonElements
				{
					RefPtr<SolidBorderElement>					rectBorderElement;
					RefPtr<GradientBackgroundElement>			backgroundElement;
					RefPtr<SolidLabelElement>					textElement;
					RefPtr<BoundsComposition>					textComposition;
					RefPtr<BoundsComposition>					mainComposition;
					RefPtr<BoundsComposition>					backgroundComposition;

					static Win8ButtonElements					Create(Alignment horizontal = Alignment::StringAlignmentCenter, Alignment vertical = Alignment::StringAlignmentCenter);
					void										Apply(const Win8ButtonColors& colors);
				};

				struct Win8CheckedButtonElements
				{
					RefPtr<SolidBorderElement>					bulletBorderElement;
					RefPtr<GradientBackgroundElement>			bulletBackgroundElement;
					RefPtr<SolidLabelElement>					bulletCheckElement;
					RefPtr<SolidBackgroundElement>				bulletRadioElement;
					RefPtr<SolidLabelElement>					textElement;
					RefPtr<BoundsComposition>					textComposition;
					RefPtr<BoundsComposition>					mainComposition;

					static Win8CheckedButtonElements			Create(ElementShape shape, bool backgroundVisible);
					void										Apply(const Win8ButtonColors& colors);
				};

				class GroupController : public Component
				{
				public:
					GroupController();
					~GroupController();

					virtual void						Attach(SelectableButton* button);
					virtual void						Detach(SelectableButton* button);
					virtual void						OnSelectedChanged(PassRefPtr<SelectableButton> button) = 0;

				protected:
					set<SelectableButton*>				buttons;
				};

				class MutexGroupController : public GroupController
				{
				public:
					MutexGroupController();
					~MutexGroupController();

					void								OnSelectedChanged(PassRefPtr<SelectableButton> button)override;

				protected:
					bool								suppress;
				};
			}

			class Button : public Control
			{
			public:
				Button(PassRefPtr<IButtonStyleController> _styleController);
				~Button();

				enum ControlState
				{
					Normal,
					Active,
					Pressed,
				};

				NotifyEvent								Clicked;

				bool									GetClickOnMouseUp();
				void									SetClickOnMouseUp(bool value);

			protected:
				void									OnParentLineChanged()override;
				void									UpdateControlState();
				void									OnLeftButtonDown(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnLeftButtonUp(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnMouseEnter(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnMouseLeave(PassRefPtr<Composition> sender, EventArgs& arguments);

				bool									clickOnMouseUp;
				bool									mousePressing;
				bool									mouseHoving;
				ControlState							controlState;
				RefPtr<IButtonStyleController>			styleController;
			};

			class SelectableButton : public Button
			{
			public:
				SelectableButton(PassRefPtr<ISelectableButtonStyleController> _styleController);
				~SelectableButton();

				NotifyEvent								GroupControllerChanged;
				NotifyEvent								AutoSelectionChanged;
				NotifyEvent								SelectedChanged;

				virtual PassRefPtr<GroupController>		GetGroupController();
				virtual void							SetGroupController(PassRefPtr<GroupController> value);

				virtual bool							GetAutoSelection();
				virtual void							SetAutoSelection(bool value);

				virtual bool							GetSelected();
				virtual void							SetSelected(bool value);

			protected:
				void									OnClicked(PassRefPtr<Composition> sender, EventArgs& arguments);

				RefPtr<ISelectableButtonStyleController>	styleController;
				RawPtr<GroupController>						groupController;
				bool										autoSelection;
				bool										isSelected;
			};

			namespace style
			{
				class IButtonStyleController : public IControlStyleController
				{
				public:
					virtual void							Transfer(Button::ControlState value) = 0;
				};

				class ISelectableButtonStyleController : public IButtonStyleController
				{
				public:
					virtual void							SetSelected(bool value) = 0;
				};

				class Win8ButtonStyleBase : public ISelectableButtonStyleController
				{
				public:
					Win8ButtonStyleBase(const Win8ButtonColors& initialColor, Alignment horizontal, Alignment vertical);
					~Win8ButtonStyleBase();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;
					void										SetSelected(bool value)override;
					void										Transfer(Button::ControlState value)override;

					bool										GetTransparentWhenInactive();
					void										SetTransparentWhenInactive(bool value);
					bool										GetTransparentWhenDisabled();
					void										SetTransparentWhenDisabled(bool value);
					bool										GetAutoSizeForText();
					void										SetAutoSizeForText(bool value);

					void										ToAnimation(PassRefPtr<GraphicsAnimationManager> manager);

				protected:
					class TransferringAnimationType : public TransferringAnimation<Win8ButtonColors, Win8ButtonStyleBase>
					{
					public:
						TransferringAnimationType(Win8ButtonStyleBase* _style, const Win8ButtonColors& begin);
					protected:
						void									PlayInternal(cint currentPosition, cint totalLength)override;
					};

					virtual void								TransferInternal(Button::ControlState value, bool enabled, bool selected) = 0;
					virtual void								AfterApplyColors(const Win8ButtonColors& colors);

					Win8ButtonElements							elements;
					RefPtr<TransferringAnimationType>			transferringAnimation;
					Button::ControlState						controlStyle;
					bool										isVisuallyEnabled;
					bool										isSelected;
					bool										transparentWhenInactive;
					bool										transparentWhenDisabled;
				};

				class Win8ButtonStyle : public Win8ButtonStyleBase
				{
				public:
					Win8ButtonStyle();
					~Win8ButtonStyle();

				protected:
					void										TransferInternal(Button::ControlState value, bool enabled, bool selected)override;
				};

				class Win8CheckBoxStyle : public ISelectableButtonStyleController
				{
				public:
					enum BulletStyle
					{
						CheckBox,
						RadioButton,
					};

					Win8CheckBoxStyle(BulletStyle bulletStyle, bool backgroundVisible = false);
					~Win8CheckBoxStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;
					void										SetSelected(bool value)override;
					void										Transfer(Button::ControlState value)override;

					void										ToAnimation(PassRefPtr<GraphicsAnimationManager> manager);

				protected:
					class TransferringAnimationType : public TransferringAnimation<Win8ButtonColors, Win8CheckBoxStyle>
					{
					public:
						TransferringAnimationType(Win8CheckBoxStyle* _style, const Win8ButtonColors& begin);
					protected:
						void									PlayInternal(cint currentPosition, cint totalLength)override;
					};

					void										TransferInternal(Button::ControlState value, bool enabled, bool selected);

					Win8CheckedButtonElements					elements;
					RefPtr<TransferringAnimationType>			transferringAnimation;
					Button::ControlState						controlStyle;
					bool										isVisuallyEnabled;
					bool										isSelected;
				};

				class Win8GroupBoxStyle : public IControlStyleController
				{
				public:
					Win8GroupBoxStyle();
					~Win8GroupBoxStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

					void										ToAnimation(PassRefPtr<GraphicsAnimationManager> manager);

				protected:
					class TransferringAnimationType : public TransferringAnimation<CColor, Win8GroupBoxStyle>
					{
					public:
						TransferringAnimationType(Win8GroupBoxStyle* _style, const CColor& begin);
					protected:
						void									PlayInternal(cint currentPosition, cint totalLength)override;
					};

					void										SetMargins(cint fontSize);

					RefPtr<BoundsComposition>					boundsComposition;
					RefPtr<BoundsComposition>					borderComposition;
					RefPtr<BoundsComposition>					textComposition;
					RefPtr<BoundsComposition>					textBackgroundComposition;
					RefPtr<BoundsComposition>					containerComposition;
					RefPtr<SolidLabelElement>					textElement;
					RefPtr<TransferringAnimationType>			transferringAnimation;
				};
			}			
		}
	}
}

#endif