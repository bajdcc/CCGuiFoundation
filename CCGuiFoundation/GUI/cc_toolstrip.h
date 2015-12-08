#ifndef CC_TOOLSTRIP
#define CC_TOOLSTRIP

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_control.h"
#include "cc_element.h"
#include "cc_comctl.h"
#include "cc_button.h"
#include "cc_list.h"

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
			using namespace helper;

			class Menu;
			class IMenuService : public Interface
			{
			public:
				enum Direction
				{
					Horizontal,
					Vertical,
				};

				static const UINT						ServiceId = 0x10;

			public:
				IMenuService();
 
				virtual PassRefPtr<IMenuService>		GetParentMenuService() = 0;
				virtual Direction						GetPreferredDirection() = 0;
				virtual bool							IsActiveState() = 0;
				virtual bool							IsSubMenuActivatedByMouseDown() = 0;
 
				virtual void							MenuItemExecuted();
				virtual PassRefPtr<Menu>				GetOpeningMenu();
				virtual PassRefPtr<Menu>				GetMenu();
				virtual void							MenuOpened(PassRefPtr<Menu> menu);
				virtual void							MenuClosed(PassRefPtr<Menu> menu);
 
			protected:
				RawPtr<Menu>							openingMenu;
			};
 
			class Menu : public Popup, private IMenuService
			{ 
			public:
				Menu(PassRefPtr<IWindowStyleController> _styleController, PassRefPtr<Control> _owner);
				~Menu();

				void									UpdateMenuService();
				Object*									QueryService(UINT nId)override;
				PassRefPtr<Menu>						GetMenu()override;

			protected:
				void									MouseClickedOnOtherWindow(PassRefPtr<Window> window)override;
				void									OnWindowOpened(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnWindowClosed(PassRefPtr<Composition> sender, EventArgs& arguments);
				
				RawPtr<Control>							owner;

			private:
				PassRefPtr<IMenuService>				GetParentMenuService()override;
				Direction								GetPreferredDirection()override;
				bool									IsActiveState()override;
				bool									IsSubMenuActivatedByMouseDown()override;
				void									MenuItemExecuted()override;

				RawPtr<IMenuService>					parentMenuService;
			};
 
			class MenuBar : public Control, private IMenuService
			{
			public:
				MenuBar(PassRefPtr<IControlStyleController> _styleController);
				~MenuBar();
 
				Object*									QueryService(UINT nId)override;

			private:
				PassRefPtr<IMenuService>				GetParentMenuService()override;
				Direction								GetPreferredDirection()override;
				bool									IsActiveState()override;
				bool									IsSubMenuActivatedByMouseDown()override;
			};
 
			namespace style
			{
				class ImageData : public Object
				{
				public:
					ImageData(PassRefPtr<IImage> _image, cint _frameIndex, const CString& _filePath);

					RefPtr<IImage>						image;
					cint								frameIndex;
					CString								filePath;
				};

				class IMenuButtonStyleController : public ISelectableButtonStyleController
				{
				public:
					virtual PassRefPtr<IWindowStyleController>		CreateSubMenuStyleController() = 0;
					virtual void									SetSubMenuExisting(bool value) = 0;
					virtual void									SetSubMenuOpening(bool value) = 0;
					virtual PassRefPtr<Button>						GetSubMenuHost() = 0;
					virtual void									SetImage(PassRefPtr<ImageData> value) = 0;
					virtual void									SetShortcutText(const CString& value) = 0;
				};
			}

			class MenuButton : public SelectableButton
			{
			public:
				MenuButton(PassRefPtr<IMenuButtonStyleController> _styleController);
				~MenuButton();
 
				NotifyEvent								SubMenuOpeningChanged;
				NotifyEvent								ImageChanged;
				NotifyEvent								ShortcutTextChanged;
 
				PassRefPtr<ImageData>					GetImage();
				void									SetImage(PassRefPtr<ImageData> value);
				const CString&							GetShortcutText();
				void									SetShortcutText(const CString& value);
 
				bool									IsSubMenuExists();
				PassRefPtr<Menu>						GetSubMenu();
				void									CreateSubMenu(PassRefPtr<IWindowStyleController> subMenuStyleController);
				void									SetSubMenu(PassRefPtr<Menu> value, bool owned);
				void									DestroySubMenu();
				bool									GetOwnedSubMenu();
 
				bool									GetSubMenuOpening();
				void									SetSubMenuOpening(bool value);
 
				CSize									GetPreferredMenuClientSize();
				void									SetPreferredMenuClientSize(CSize value);
 
				bool									GetCascadeAction();
				void									SetCascadeAction(bool value);

			protected:
				PassRefPtr<Button>						GetSubMenuHost();
				void									OpenSubMenuInternal();
				void									OnParentLineChanged()override;

				void									OnSubMenuWindowOpened(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnSubMenuWindowClosed(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnMouseEnter(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnClicked(PassRefPtr<Composition> sender, EventArgs& arguments);

				virtual IMenuService::Direction			GetSubMenuDirection();

				RawPtr<IMenuButtonStyleController>		styleController;
				RefPtr<ImageData>						image;
				CString									shortcutText;
				RefPtr<Menu>							subMenu;
				bool									ownedSubMenu;
				CSize									preferredMenuClientSize;
				RawPtr<IMenuService>					ownerMenuService;
				bool									cascadeAction;
			};
 
			//////////////////////////////////////////////////////////////////////////

			class ToolstripCommand : public Component
			{
			public:
				ToolstripCommand();
				~ToolstripCommand();
  
				void									Attach(ControlHostRoot* rootObject)override;
				void									Detach(ControlHostRoot* rootObject)override;
  
				NotifyEvent								Executed;  
				NotifyEvent								DescriptionChanged;
  
				PassRefPtr<ImageData>					GetImage();
				void									SetImage(PassRefPtr<ImageData> value);
				const CString&							GetText();
				void									SetText(const CString& value);
				PassRefPtr<ShortcutKeyItem>				GetShortcut();
				void									SetShortcut(PassRefPtr<ShortcutKeyItem> value);
				bool									GetEnabled();
				void									SetEnabled(bool value);
				bool									GetSelected();
				void									SetSelected(bool value);

			protected:
				RefPtr<ImageData>						image;
				CString									text;
				RefPtr<ShortcutKeyItem>					shortcutKeyItem;
				bool									enabled;
				bool									selected;
				RefPtr<NotifyEvent::IHandler>			shortcutKeyItemExecutedHandler;
				RawPtr<ControlHost>						shortcutOwner;

				void									OnShortcutKeyItemExecuted(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									InvokeDescriptionChanged();
				void									ReplaceShortcut(PassRefPtr<ShortcutKeyItem> value);
			};
  
			class ToolstripCollection : public ItemsBase<Control>
			{
			public:
				ToolstripCollection(PassRefPtr<IContentCallback> _contentCallback, PassRefPtr<StackComposition> _stackComposition);
				~ToolstripCollection();

			protected:
				void										InvokeUpdateLayout();
				void										OnInterestingMenuButtonPropertyChanged(PassRefPtr<Composition> sender, EventArgs& arguments);
				bool										QueryInsert(SizeType index, ParamType child)override;
				bool										QueryRemove(SizeType index, ParamType child)override;
				void										BeforeInsert(SizeType index, ParamType child)override;
				void										BeforeRemove(SizeType index, ParamType child)override;
				void										AfterInsert(SizeType index, ParamType child)override;
				void										AfterRemove(SizeType index, SizeType count)override;

				RawPtr<IContentCallback>					contentCallback;
				RawPtr<StackComposition>					stackComposition;
			};
	
			class ToolstripButton;
			class ToolstripBuilder : public Object
			{
				friend class ToolstripMenu;
				friend class ToolstripMenuBar;
				friend class ToolstripToolBar;
			public:
				~ToolstripBuilder();
  
				enum Environment
				{
					Env_Menu,
					Env_MenuBar,
					Env_ToolBar,
				};

				ToolstripBuilder&						Button(PassRefPtr<ImageData> image, const CString& text, RefPtr<ToolstripButton>& result);
				ToolstripBuilder&						Button(PassRefPtr<ToolstripCommand> command, RefPtr<ToolstripButton>& result);
				ToolstripBuilder&						DropdownButton(PassRefPtr<ImageData> image, const CString& text, RefPtr<ToolstripButton>& result);
				ToolstripBuilder&						DropdownButton(PassRefPtr<ToolstripCommand> command, RefPtr<ToolstripButton>& result);
				ToolstripBuilder&						SplitButton(PassRefPtr<ImageData> image, const CString& text, RefPtr<ToolstripButton>& result);
				ToolstripBuilder&						SplitButton(PassRefPtr<ToolstripCommand> command, RefPtr<ToolstripButton>& result);
				ToolstripBuilder&						Splitter();
				ToolstripBuilder&						CommonControl(PassRefPtr<Control> control);
				ToolstripBuilder&						BeginSubMenu();
				ToolstripBuilder&						EndSubMenu();

			protected:
				ToolstripBuilder(Environment _environment, PassRefPtr<ToolstripCollection> _toolstripItems);

				Environment								environment;
				RefPtr<ToolstripCollection>				toolstripItems;
				RawPtr<ToolstripBuilder>				previousBuilder;
				RefPtr<ITheme>							theme;
				RefPtr<ToolstripButton>					lastCreatedButton;
			};

			class ToolstripMenu : public Menu, protected IContentCallback
			{
			public:
				ToolstripMenu(PassRefPtr<IWindowStyleController> _styleController, PassRefPtr<Control> _owner);
				~ToolstripMenu();
   
				ToolstripCollection&					GetToolstripItems();
				PassRefPtr<ToolstripBuilder>			GetBuilder(PassRefPtr<ITheme> themeObject);

			protected:
				void									UpdateLayout()override;

				RefPtr<SharedSizeComposition>			sharedSizeRootComposition;
				RefPtr<StackComposition>				stackComposition;
				RefPtr<ToolstripCollection>				toolstripItems;
				RefPtr<ToolstripBuilder>				builder;
			};
   
			class ToolstripMenuBar : public MenuBar
			{
			public:
				ToolstripMenuBar(PassRefPtr<IControlStyleController> _styleController);
				~ToolstripMenuBar();
   
				ToolstripCollection&					GetToolstripItems();
				ToolstripBuilder&						GetBuilder(PassRefPtr<ITheme> themeObject);

			protected:
				RefPtr<StackComposition>				stackComposition;
				RefPtr<ToolstripCollection>				toolstripItems;
				RefPtr<ToolstripBuilder>				builder;
			};
   
			class ToolstripToolBar : public Control
			{   
			public:
				ToolstripToolBar(PassRefPtr<IControlStyleController> _styleController);
				~ToolstripToolBar();
   
				ToolstripCollection&					GetToolstripItems();
				ToolstripBuilder&						GetBuilder(PassRefPtr<ITheme> themeObject);

			protected:
				RefPtr<StackComposition>				stackComposition;
				RefPtr<ToolstripCollection>				toolstripItems;
				RefPtr<ToolstripBuilder>				builder;
			};
   
			class ToolstripButton : public MenuButton
			{
			public:
				ToolstripButton(PassRefPtr<IMenuButtonStyleController> _styleController);
				~ToolstripButton();
   
				PassRefPtr<ToolstripCommand>			GetCommand();
				void									SetCommand(PassRefPtr<ToolstripCommand> value);
   
				PassRefPtr<ToolstripMenu>				GetToolstripSubMenu();
				void									CreateToolstripSubMenu(PassRefPtr<IWindowStyleController> subMenuStyleController);

			protected:
				void									UpdateCommandContent();
				void									OnClicked(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									OnCommandDescriptionChanged(PassRefPtr<Composition> sender, EventArgs& arguments);

				RefPtr<ToolstripCommand>				command;
				RefPtr<NotifyEvent::IHandler>			descriptionChangedHandler;
			};

			namespace style
			{
				struct Win8MenuItemButtonElements
				{
					RefPtr<SolidBorderElement>					borderElement;
					RefPtr<GradientBackgroundElement>			backgroundElement;
					RefPtr<SolidBorderElement>					splitterElement;
					RefPtr<TableCellComposition>				splitterComposition;
					RefPtr<ImageFrameElement>					imageElement;
					RefPtr<SolidLabelElement>					textElement;
					RefPtr<SharedSizeItemComposition>			textComposition;
					RefPtr<SolidLabelElement>					shortcutElement;
					RefPtr<SharedSizeItemComposition>			shortcutComposition;
					RefPtr<PolygonElement>						subMenuArrowElement;
					RefPtr<Composition>							subMenuArrowComposition;
					RefPtr<BoundsComposition>					mainComposition;

					static Win8MenuItemButtonElements			Create();
					void										Apply(const Win8ButtonColors& colors);
					void										SetActive(bool value);
					void										SetSubMenuExisting(bool value);
				};

				class Win8MenuStyle : public DefaultBehaviorStyleController
				{
				public:
					Win8MenuStyle();
					~Win8MenuStyle();

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

				class Win8MenuBarStyle : public IControlStyleController
				{
				public:
					Win8MenuBarStyle();
					~Win8MenuBarStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

				protected:
					RefPtr<BoundsComposition>					boundsComposition;
					RefPtr<BoundsComposition>					containerComposition;
				};

				class Win8MenuBarButtonStyle : public IMenuButtonStyleController
				{
				public:
					Win8MenuBarButtonStyle();
					~Win8MenuBarButtonStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

					void										SetSelected(bool value)override;
					PassRefPtr<IWindowStyleController>			CreateSubMenuStyleController()override;
					void										SetSubMenuExisting(bool value)override;
					void										SetSubMenuOpening(bool value)override;
					PassRefPtr<Button>							GetSubMenuHost()override;
					void										SetImage(PassRefPtr<ImageData> value)override;
					void										SetShortcutText(const CString& value)override;
					void										Transfer(Button::ControlState value)override;

				protected:
					void										TransferInternal(Button::ControlState value, bool enabled, bool opening);

					Win8ButtonElements							elements;
					Button::ControlState						controlStyle;
					bool										isVisuallyEnabled;
					bool										isOpening;
				};

				class Win8MenuItemButtonStyle : public IMenuButtonStyleController
				{
				public:
					Win8MenuItemButtonStyle();
					~Win8MenuItemButtonStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

					void										SetSelected(bool value)override;
					PassRefPtr<IWindowStyleController>			CreateSubMenuStyleController()override;
					void										SetSubMenuExisting(bool value)override;
					void										SetSubMenuOpening(bool value)override;
					PassRefPtr<Button>							GetSubMenuHost()override;
					void										SetImage(PassRefPtr<ImageData> value)override;
					void										SetShortcutText(const CString& value)override;
					void										Transfer(Button::ControlState value)override;

				protected:
					void										TransferInternal(Button::ControlState value, bool enabled, bool selected, bool opening);

					Win8MenuItemButtonElements					elements;
					Button::ControlState						controlStyle;
					bool										isVisuallyEnabled;
					bool										isSelected;
					bool										isOpening;
				};

				class Win8MenuSplitterStyle : public IControlStyleController
				{
				public:
					Win8MenuSplitterStyle();
					~Win8MenuSplitterStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

				protected:
					RefPtr<BoundsComposition>					boundsComposition;
				};

				class Win8ToolstripToolBarStyle : public IControlStyleController
				{
				public:
					Win8ToolstripToolBarStyle();
					~Win8ToolstripToolBarStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

				protected:
					RefPtr<BoundsComposition>					boundsComposition;
					RefPtr<BoundsComposition>					containerComposition;
				};

				class Win8ToolstripButtonDropdownStyle : public IButtonStyleController
				{
				public:
					Win8ToolstripButtonDropdownStyle();
					~Win8ToolstripButtonDropdownStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;
					void										Transfer(Button::ControlState value)override;

				protected:
					virtual void								TransferInternal(Button::ControlState value, bool enabled);

					RefPtr<BoundsComposition>					boundsComposition;
					RefPtr<BoundsComposition>					splitterComposition;
					RefPtr<BoundsComposition>					containerComposition;
					bool										isVisuallyEnabled;
					Button::ControlState						controlState;
				};

				class Win8ToolstripButtonStyle : public IMenuButtonStyleController
				{
				public:
					enum ButtonStyle
					{
						CommandButton,
						DropdownButton,
						SplitButton,
					};

				public:
					Win8ToolstripButtonStyle(ButtonStyle _buttonStyle);
					~Win8ToolstripButtonStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

					void										SetSelected(bool value)override;
					PassRefPtr<IWindowStyleController>			CreateSubMenuStyleController()override;
					void										SetSubMenuExisting(bool value)override;
					void										SetSubMenuOpening(bool value)override;
					PassRefPtr<Button>							GetSubMenuHost()override;
					void										SetImage(PassRefPtr<ImageData> value)override;
					void										SetShortcutText(const CString& value)override;
					void										Transfer(Button::ControlState value)override;
					
					void										ToAnimation(PassRefPtr<GraphicsAnimationManager> manager);

				protected:
					class TransferringAnimationType : public TransferringAnimation<Win8ButtonColors, Win8ToolstripButtonStyle>
					{
					public:
						TransferringAnimationType(Win8ToolstripButtonStyle* _style, const Win8ButtonColors& begin);
					protected:
						void									PlayInternal(cint currentPosition, cint totalLength)override;
					};

					virtual void								TransferInternal(Button::ControlState value, bool enabled, bool selected, bool menuOpening);

					Win8ButtonElements							elements;
					RefPtr<TransferringAnimationType>			transferringAnimation;
					Button::ControlState						controlStyle;
					bool										isVisuallyEnabled;
					bool										isSelected;
					bool										isOpening;
					RefPtr<ImageFrameElement>					imageElement;
					RefPtr<BoundsComposition>					imageComposition;
					ButtonStyle									buttonStyle;
					RefPtr<Button>								subMenuHost;
				};

				class Win8ToolstripSplitterStyle : public IControlStyleController
				{
				public:
					Win8ToolstripSplitterStyle();
					~Win8ToolstripSplitterStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

				protected:
					RefPtr<BoundsComposition>					boundsComposition;
				};
			}
		}
	}
}

#endif