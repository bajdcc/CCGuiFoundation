#ifndef CC_STYLE
#define CC_STYLE

#include "stdafx.h"
#include "cc_base.h"
#include "cc_composition.h"
#include "cc_interface.h"

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			namespace style
			{
				using namespace cc::presentation::control::composition;

				class ITheme;
				class IThemeHelper;
				class EmptyStyleController;
				class DefaultBehaviorStyleController;

				class IWindowStyleController;
				class IControlStyleController;
				class IControlStyleProvider;

				class ILabelStyleController;
				class IButtonStyleController;
				class ISelectableButtonStyleController;
				class IGroupBoxStyleController;

				class ISinglelineTextBoxStyleProvider;
				class IScrollViewStyleProvider;
				class IMenuButtonStyleController;

				class ITabStyleController;
				class IScrollStyleController;

				class ITheme : public Interface
				{
				public:
					virtual PassRefPtr<IWindowStyleController>				CreateWindowStyle() = 0;
					virtual PassRefPtr<IWindowStyleController>				CreateTooltipStyle() = 0;

					virtual PassRefPtr<IControlStyleController>				CreateCustomControlStyle() = 0;
					virtual PassRefPtr<ILabelStyleController>				CreateLabelStyle() = 0;
					virtual PassRefPtr<IButtonStyleController>				CreateButtonStyle() = 0;
					virtual PassRefPtr<ISelectableButtonStyleController>	CreateCheckBoxStyle() = 0;
					virtual PassRefPtr<ISelectableButtonStyleController>	CreateRadioButtonStyle() = 0;
					virtual PassRefPtr<IControlStyleController>				CreateGroupBoxStyle() = 0;
					virtual PassRefPtr<ISinglelineTextBoxStyleProvider>		CreateTextBoxStyle() = 0;
					virtual PassRefPtr<IScrollViewStyleProvider>			CreateScrollContainerStyle() = 0;
					virtual PassRefPtr<IScrollViewStyleProvider>			CreateMultilineTextBoxStyle() = 0;

					virtual PassRefPtr<IWindowStyleController>				CreateMenuStyle() = 0;
					virtual PassRefPtr<IControlStyleController>				CreateMenuBarStyle() = 0;
					virtual PassRefPtr<IControlStyleController>				CreateMenuSplitterStyle() = 0;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateMenuBarButtonStyle() = 0;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateMenuItemButtonStyle() = 0;
					virtual PassRefPtr<IControlStyleController>				CreateToolBarStyle() = 0;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateToolBarButtonStyle() = 0;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateToolBarDropdownButtonStyle() = 0;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateToolBarSplitButtonStyle() = 0;
					virtual PassRefPtr<IControlStyleController>				CreateToolBarSplitterStyle() = 0;

					virtual PassRefPtr<ITabStyleController>					CreateTabStyle() = 0;
					virtual PassRefPtr<IScrollStyleController>				CreateHScrollStyle() = 0;
					virtual PassRefPtr<IScrollStyleController>				CreateVScrollStyle() = 0;
					virtual PassRefPtr<IScrollViewStyleProvider>			CreateConsoleStyle() = 0;
				};

				class Win8Style : public ITheme
				{
					virtual PassRefPtr<IWindowStyleController>				CreateWindowStyle()override;
					virtual PassRefPtr<IWindowStyleController>				CreateTooltipStyle()override;

					virtual PassRefPtr<IControlStyleController>				CreateCustomControlStyle()override;
					virtual PassRefPtr<ILabelStyleController>				CreateLabelStyle()override;
					virtual PassRefPtr<IButtonStyleController>				CreateButtonStyle()override;
					virtual PassRefPtr<ISelectableButtonStyleController>	CreateCheckBoxStyle()override;
					virtual PassRefPtr<ISelectableButtonStyleController>	CreateRadioButtonStyle()override;
					virtual PassRefPtr<IControlStyleController>				CreateGroupBoxStyle()override;
					virtual PassRefPtr<ISinglelineTextBoxStyleProvider>		CreateTextBoxStyle()override;
					virtual PassRefPtr<IScrollViewStyleProvider>			CreateScrollContainerStyle()override;
					virtual PassRefPtr<IScrollViewStyleProvider>			CreateMultilineTextBoxStyle()override;

					virtual PassRefPtr<IWindowStyleController>				CreateMenuStyle()override;
					virtual PassRefPtr<IControlStyleController>				CreateMenuBarStyle()override;
					virtual PassRefPtr<IControlStyleController>				CreateMenuSplitterStyle()override;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateMenuBarButtonStyle()override;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateMenuItemButtonStyle()override;
					virtual PassRefPtr<IControlStyleController>				CreateToolBarStyle()override;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateToolBarButtonStyle()override;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateToolBarDropdownButtonStyle()override;
					virtual PassRefPtr<IMenuButtonStyleController>			CreateToolBarSplitButtonStyle()override;
					virtual PassRefPtr<IControlStyleController>				CreateToolBarSplitterStyle()override;

					virtual PassRefPtr<ITabStyleController>					CreateTabStyle()override;
					virtual PassRefPtr<IScrollStyleController>				CreateHScrollStyle()override;
					virtual PassRefPtr<IScrollStyleController>				CreateVScrollStyle()override;
					virtual PassRefPtr<IScrollViewStyleProvider>			CreateConsoleStyle()override;
				};

				class IControlStyleController : public Interface
				{
				public:
					virtual PassRefPtr<BoundsComposition>			GetBoundsComposition() = 0;
					virtual PassRefPtr<Composition>					GetContainerComposition() = 0;
					virtual void									SetFocusableComposition(PassRefPtr<Composition> value) = 0;
					virtual void									SetText(const CString& value) = 0;
					virtual void									SetFont(const Font& value) = 0;
					virtual void									SetVisuallyEnabled(bool value) = 0;
				};

				class EmptyStyleController : public IControlStyleController
				{
				public:
					EmptyStyleController();
					~EmptyStyleController();

					PassRefPtr<BoundsComposition>					GetBoundsComposition()override;
					PassRefPtr<Composition>							GetContainerComposition()override;
					void											SetFocusableComposition(PassRefPtr<Composition> value)override;
					void											SetText(const CString& value)override;
					void											SetFont(const Font& value)override;
					void											SetVisuallyEnabled(bool value)override;

				protected:
					RefPtr<BoundsComposition>						boundsComposition;
				};

				class IControlStyleProvider : public Interface
				{
				public:
					virtual void									AssociateStyleController(PassRefPtr<IControlStyleController> controller) = 0;
					virtual void									SetFocusableComposition(PassRefPtr<Composition> value) = 0;
					virtual void									SetText(const CString& value) = 0;
					virtual void									SetFont(const Font& value) = 0;
					virtual void									SetVisuallyEnabled(bool value) = 0;
				};

				class IWindowStyleController : public IControlStyleController
				{
				public:
					virtual void									AttachWindow(PassRefPtr<Window> _window) = 0;
					virtual void									InitializeWindowProperties() = 0;
					virtual void									SetSizeState(IWindow::WindowSizeState value) = 0;
					virtual bool									GetMaximizedBox() = 0;
					virtual void									SetMaximizedBox(bool visible) = 0;
					virtual bool									GetMinimizedBox() = 0;
					virtual void									SetMinimizedBox(bool visible) = 0;
					virtual bool									GetBorder() = 0;
					virtual void									SetBorder(bool visible) = 0;
					virtual bool									GetSizeBox() = 0;
					virtual void									SetSizeBox(bool visible) = 0;
					virtual bool									GetIconVisible() = 0;
					virtual void									SetIconVisible(bool visible) = 0;
					virtual bool									GetTitleBar() = 0;
					virtual void									SetTitleBar(bool visible) = 0;
					virtual PassRefPtr<IWindowStyleController>		CreateTooltipStyle() = 0;
					virtual bool									ShowShadow() = 0;
				};

				class DefaultBehaviorStyleController : virtual public IWindowStyleController
				{
				public:
					DefaultBehaviorStyleController();
					~DefaultBehaviorStyleController();

					void											AttachWindow(PassRefPtr<Window> _window)override;
					void											InitializeWindowProperties()override;
					void											SetSizeState(IWindow::WindowSizeState value)override;
					bool											GetMaximizedBox()override;
					void											SetMaximizedBox(bool visible)override;
					bool											GetMinimizedBox()override;
					void											SetMinimizedBox(bool visible)override;
					bool											GetBorder()override;
					void											SetBorder(bool visible)override;
					bool											GetSizeBox()override;
					void											SetSizeBox(bool visible)override;
					bool											GetIconVisible()override;
					void											SetIconVisible(bool visible)override;
					bool											GetTitleBar()override;
					void											SetTitleBar(bool visible)override;
					PassRefPtr<IWindowStyleController>				CreateTooltipStyle()override;
					bool											ShowShadow();

				protected:
					RawPtr<Window>									window;
				};

				class Win8WindowStyle : public DefaultBehaviorStyleController
				{
				public:
					Win8WindowStyle();
					~Win8WindowStyle();

					PassRefPtr<BoundsComposition>		GetBoundsComposition()override;
					PassRefPtr<Composition>				GetContainerComposition()override;
					void								SetFocusableComposition(PassRefPtr<Composition> value)override;
					void								SetText(const CString& value)override;
					void								SetFont(const Font& value)override;
					void								SetVisuallyEnabled(bool value)override;

				public:
					enum ColorType
					{
						CT_Window,
						CT_Tooltip,
						CT_Text_Enable,
						CT_Text_Disable,
						CT_Border,
						CT_Menu_Border,
						CT_Menu_Splitter,
						CT_Tab,
					};

					static CColor						GetSystemColor(ColorType type);
					static ColorEntry					GetTextColor();

				protected:
					RefPtr<BoundsComposition>			boundsComposition;
				};

				class Win8TooltipStyle : public DefaultBehaviorStyleController
				{
				public:
					Win8TooltipStyle();
					~Win8TooltipStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

					bool										ShowShadow();

				protected:
					RefPtr<BoundsComposition>					boundsComposition;
					RefPtr<BoundsComposition>					containerComposition;
				};
			}
		}
	}
}

#endif