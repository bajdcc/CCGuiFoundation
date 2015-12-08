#include "stdafx.h"
#include "cc_toolstrip.h"
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
				PassRefPtr<IWindowStyleController> Win8Style::CreateMenuStyle()
				{
					return adoptRef(new Win8MenuStyle);
				}

				PassRefPtr<IControlStyleController> Win8Style::CreateMenuBarStyle()
				{
					return adoptRef(new Win8MenuBarStyle);
				}

				PassRefPtr<IControlStyleController> Win8Style::CreateMenuSplitterStyle()
				{
					return adoptRef(new Win8MenuSplitterStyle);
				}

				PassRefPtr<IMenuButtonStyleController> Win8Style::CreateMenuBarButtonStyle()
				{
					return adoptRef(new Win8MenuBarButtonStyle);
				}

				PassRefPtr<IMenuButtonStyleController> Win8Style::CreateMenuItemButtonStyle()
				{
					return adoptRef(new Win8MenuItemButtonStyle);
				}

				PassRefPtr<IControlStyleController> Win8Style::CreateToolBarStyle()
				{
					return adoptRef(new Win8ToolstripToolBarStyle);
				}

				PassRefPtr<IMenuButtonStyleController> Win8Style::CreateToolBarButtonStyle()
				{
					return adoptRef(new Win8ToolstripButtonStyle(Win8ToolstripButtonStyle::CommandButton));
				}

				PassRefPtr<IMenuButtonStyleController> Win8Style::CreateToolBarDropdownButtonStyle()
				{
					return adoptRef(new Win8ToolstripButtonStyle(Win8ToolstripButtonStyle::DropdownButton));
				}

				PassRefPtr<IMenuButtonStyleController> Win8Style::CreateToolBarSplitButtonStyle()
				{
					return adoptRef(new Win8ToolstripButtonStyle(Win8ToolstripButtonStyle::SplitButton));
				}

				PassRefPtr<IControlStyleController> Win8Style::CreateToolBarSplitterStyle()
				{
					return adoptRef(new Win8ToolstripSplitterStyle);
				}

				ImageData::ImageData(PassRefPtr<IImage> _image, cint _frameIndex, const CString& _filePath)
					: image(_image)
					, frameIndex(_frameIndex)
					, filePath(_filePath)
				{

				}
			}

			IMenuService::IMenuService()
			{

			}

			void IMenuService::MenuItemExecuted()
			{
				if (openingMenu)
				{
					openingMenu->Hide();
				}
				if (GetParentMenuService())
				{
					GetParentMenuService()->MenuItemExecuted();
				}
			}

			PassRefPtr<Menu> IMenuService::GetOpeningMenu()
			{
				return openingMenu;
			}

			PassRefPtr<Menu> IMenuService::GetMenu()
			{
				return nullptr;
			}

			void IMenuService::MenuOpened(PassRefPtr<Menu> menu)
			{
				if (openingMenu != menu && openingMenu)
				{
					openingMenu->Hide();
				}
				openingMenu = menu;
			}

			void IMenuService::MenuClosed(PassRefPtr<Menu> menu)
			{
				if (openingMenu == menu)
				{
					openingMenu = nullptr;
				}
			}

			Menu::Menu(PassRefPtr<IWindowStyleController> _styleController, PassRefPtr<Control> _owner)
				: Popup(_styleController)
				, owner(_owner)
			{
				GetWindow()->SetAlwaysPassFocusToParent(true);
				GetWindow()->SetNoActivate(true);

				UpdateMenuService();
				WindowOpened.AttachMethod(this, &Menu::OnWindowOpened);
				WindowClosed.AttachMethod(this, &Menu::OnWindowClosed);
			}

			Menu::~Menu()
			{

			}

			void Menu::UpdateMenuService()
			{
				if (owner)
				{
					parentMenuService = owner->QueryTypedService<IMenuService>();
				}
			}

			Object* Menu::QueryService(UINT nId)
			{
				if (nId == IMenuService::ServiceId)
				{
					return dynamic_cast<IMenuService*>(this);
				}
				else
				{
					return Popup::QueryService(nId);
				}
			}

			PassRefPtr<Menu> Menu::GetMenu()
			{
				return this;
			}

			void Menu::MouseClickedOnOtherWindow(PassRefPtr<Window> window)
			{
				Menu* targetMenu = dynamic_cast<Menu*>(window.get());
				if (!targetMenu)
				{
					Hide();
				}
			}

			void Menu::OnWindowOpened(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (parentMenuService)
				{
					parentMenuService->MenuOpened(this);
				}
			}

			void Menu::OnWindowClosed(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (parentMenuService)
				{
					parentMenuService->MenuClosed(this);
					RefPtr<Menu> openingSubMenu = GetOpeningMenu();
					if (openingSubMenu)
					{
						openingSubMenu->Hide();
					}
				}
			}

			PassRefPtr<IMenuService> Menu::GetParentMenuService()
			{
				return parentMenuService;
			}

			Menu::Direction Menu::GetPreferredDirection()
			{
				return IMenuService::Vertical;
			}

			bool Menu::IsActiveState()
			{
				return true;
			}

			bool Menu::IsSubMenuActivatedByMouseDown()
			{
				return false;
			}

			void Menu::MenuItemExecuted()
			{
				IMenuService::MenuItemExecuted();
				Hide();
			}

			MenuBar::MenuBar(PassRefPtr<IControlStyleController> _styleController)
				: Control(_styleController)
			{

			}

			MenuBar::~MenuBar()
			{

			}

			PassRefPtr<IMenuService> MenuBar::GetParentMenuService()
			{
				return nullptr;
			}

			MenuBar::Direction MenuBar::GetPreferredDirection()
			{
				return IMenuService::Horizontal;
			}

			bool MenuBar::IsActiveState()
			{
				return GetOpeningMenu();
			}

			bool MenuBar::IsSubMenuActivatedByMouseDown()
			{
				return true;
			}

			Object* MenuBar::QueryService(UINT nId)
			{
				if (nId == IMenuService::ServiceId)
				{
					return dynamic_cast<IMenuService*>(this);
				}
				else
				{
					return Control::QueryService(nId);
				}
			}

			MenuButton::MenuButton(PassRefPtr<IMenuButtonStyleController> _styleController)
				: SelectableButton(_styleController.get())
				, styleController(_styleController.get())
				, ownedSubMenu(false)
				, cascadeAction(true)
			{
				SetAutoSelection(false);
				SubMenuOpeningChanged.SetAssociatedComposition(boundsComposition);
				ImageChanged.SetAssociatedComposition(boundsComposition);
				ShortcutTextChanged.SetAssociatedComposition(boundsComposition);
				GetSubMenuHost()->Clicked.AttachMethod(this, &MenuButton::OnClicked);
				GetSubMenuHost()->GetEventReceiver()->mouseEnter.AttachMethod(this, &MenuButton::OnMouseEnter);
			}

			MenuButton::~MenuButton()
			{

			}

			PassRefPtr<ImageData> MenuButton::GetImage()
			{
				return image;
			}

			void MenuButton::SetImage(PassRefPtr<ImageData> value)
			{
				if (image != value)
				{
					image = value;
					styleController->SetImage(image);
					ImageChanged.Execute(GetNotifyEventArguments());
				}
			}

			const CString& MenuButton::GetShortcutText()
			{
				return shortcutText;
			}

			void MenuButton::SetShortcutText(const CString& value)
			{
				if (shortcutText != value)
				{
					shortcutText = value;
					styleController->SetShortcutText(shortcutText);
					ShortcutTextChanged.Execute(GetNotifyEventArguments());
				}
			}

			bool MenuButton::IsSubMenuExists()
			{
				return subMenu;
			}

			PassRefPtr<Menu> MenuButton::GetSubMenu()
			{
				return subMenu;
			}

			void MenuButton::CreateSubMenu(PassRefPtr<IWindowStyleController> subMenuStyleController)
			{
				if (!subMenu)
				{
					RefPtr<Menu> newSubMenu = adoptRef(new Menu(subMenuStyleController
						? subMenuStyleController
						: styleController->CreateSubMenuStyleController(), this));
					SetSubMenu(newSubMenu, true);
				}
			}

			void MenuButton::SetSubMenu(PassRefPtr<Menu> value, bool owned)
			{
				subMenu = value;
				ownedSubMenu = owned;
				if (subMenu)
				{
					subMenu->WindowOpened.AttachMethod(this, &MenuButton::OnSubMenuWindowOpened);
					subMenu->WindowClosed.AttachMethod(this, &MenuButton::OnSubMenuWindowClosed);
				}
				styleController->SetSubMenuExisting(subMenu != 0);
			}

			void MenuButton::DestroySubMenu()
			{
				if (subMenu)
				{
					subMenu = nullptr;
					ownedSubMenu = false;
					styleController->SetSubMenuExisting(false);
				}
			}

			bool MenuButton::GetOwnedSubMenu()
			{
				return subMenu && ownedSubMenu;
			}

			bool MenuButton::GetSubMenuOpening()
			{
				if (subMenu)
				{
					return subMenu->GetOpening();
				}
				else
				{
					return false;
				}
			}

			void MenuButton::SetSubMenuOpening(bool value)
			{
				if (subMenu)
				{
					if (value)
					{
						subMenu->SetClientSize(preferredMenuClientSize);
						IMenuService::Direction direction = GetSubMenuDirection();
						subMenu->ShowPopup(GetSubMenuHost(), direction == IMenuService::Horizontal);
					}
					else
					{
						subMenu->Close();
					}
				}
			}

			CSize MenuButton::GetPreferredMenuClientSize()
			{
				return preferredMenuClientSize;
			}

			void MenuButton::SetPreferredMenuClientSize(CSize value)
			{
				preferredMenuClientSize = value;
			}

			bool MenuButton::GetCascadeAction()
			{
				return cascadeAction;
			}

			void MenuButton::SetCascadeAction(bool value)
			{
				cascadeAction = value;
			}

			PassRefPtr<Button> MenuButton::GetSubMenuHost()
			{
				RefPtr<Button> button = styleController->GetSubMenuHost();
				return button ? button : this;
			}

			void MenuButton::OpenSubMenuInternal()
			{
				if (!GetSubMenuOpening())
				{
					if (ownerMenuService)
					{
						RefPtr<Menu> openingSiblingMenu = ownerMenuService->GetOpeningMenu();
						if (openingSiblingMenu)
						{
							openingSiblingMenu->Hide();
						}
					}
					SetSubMenuOpening(true);
				}
			}

			void MenuButton::OnParentLineChanged()
			{
				Button::OnParentLineChanged();
				ownerMenuService = QueryTypedService<IMenuService>();
				if (ownerMenuService)
				{
					SetClickOnMouseUp(!ownerMenuService->IsSubMenuActivatedByMouseDown());
				}
				if (subMenu)
				{
					subMenu->UpdateMenuService();
				}
			}

			void MenuButton::OnSubMenuWindowOpened(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				SubMenuOpeningChanged.Execute(GetNotifyEventArguments());
				styleController->SetSubMenuOpening(true);
			}

			void MenuButton::OnSubMenuWindowClosed(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				SubMenuOpeningChanged.Execute(GetNotifyEventArguments());
				styleController->SetSubMenuOpening(false);
			}

			void MenuButton::OnMouseEnter(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (GetVisuallyEnabled())
				{
					if (cascadeAction && ownerMenuService && ownerMenuService->IsActiveState())
					{
						OpenSubMenuInternal();
					}
				}
			}

			void MenuButton::OnClicked(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (GetVisuallyEnabled())
				{
					if (GetSubMenu())
					{
						OpenSubMenuInternal();
					}
					else if (ownerMenuService)
					{
						ownerMenuService->MenuItemExecuted();
					}
				}
			}

			IMenuService::Direction MenuButton::GetSubMenuDirection()
			{
				return ownerMenuService ? ownerMenuService->GetPreferredDirection() : IMenuService::Horizontal;
			}

			ToolstripCommand::ToolstripCommand()
				: enabled(true)
				, selected(false)
			{

			}

			ToolstripCommand::~ToolstripCommand()
			{

			}

			void ToolstripCommand::Attach(ControlHostRoot* rootObject)
			{
				shortcutOwner = dynamic_cast<ControlHost*>(rootObject);
			}

			void ToolstripCommand::Detach(ControlHostRoot* rootObject)
			{
				ReplaceShortcut(nullptr);
				shortcutOwner = nullptr;
			}

			PassRefPtr<ImageData> ToolstripCommand::GetImage()
			{
				return image;
			}

			void ToolstripCommand::SetImage(PassRefPtr<ImageData> value)
			{
				if (image != value)
				{
					image = value;
					InvokeDescriptionChanged();
				}
			}

			const CString& ToolstripCommand::GetText()
			{
				return text;
			}

			void ToolstripCommand::SetText(const CString& value)
			{
				if (text != value)
				{
					text = value;
					InvokeDescriptionChanged();
				}
			}

			PassRefPtr<ShortcutKeyItem> ToolstripCommand::GetShortcut()
			{
				return shortcutKeyItem;
			}

			void ToolstripCommand::SetShortcut(PassRefPtr<ShortcutKeyItem> value)
			{
				ReplaceShortcut(value);
			}

			bool ToolstripCommand::GetEnabled()
			{
				return enabled;
			}

			void ToolstripCommand::SetEnabled(bool value)
			{
				if (enabled != value)
				{
					enabled = value;
					InvokeDescriptionChanged();
				}
			}

			bool ToolstripCommand::GetSelected()
			{
				return selected;
			}

			void ToolstripCommand::SetSelected(bool value)
			{
				if (selected != value)
				{
					selected = value;
					InvokeDescriptionChanged();
				}
			}

			void ToolstripCommand::OnShortcutKeyItemExecuted(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				Executed.ExecuteWithNewSender(arguments, sender);
			}

			void ToolstripCommand::InvokeDescriptionChanged()
			{
				EventArgs arguments;
				DescriptionChanged.Execute(arguments);
			}

			void ToolstripCommand::ReplaceShortcut(PassRefPtr<ShortcutKeyItem> value)
			{
				if (shortcutKeyItem != value)
				{
					if (shortcutKeyItem)
					{
						shortcutKeyItem->Executed.Detach(shortcutKeyItemExecutedHandler);
						RefPtr<ShortcutKeyManager> manager = shortcutOwner->GetShortcutKeyManager();
						if (manager)
						{
							manager->DestroyShortcut(shortcutKeyItem->ctrl, shortcutKeyItem->shift, shortcutKeyItem->alt, shortcutKeyItem->key);
						}
					}
					{
						shortcutKeyItem = nullptr;
						shortcutKeyItemExecutedHandler = nullptr;
					}
					if (value)
					{
						shortcutKeyItem = value;
						shortcutKeyItemExecutedHandler = shortcutKeyItem->Executed.AttachMethod(this, &ToolstripCommand::OnShortcutKeyItemExecuted);
					}
					InvokeDescriptionChanged();
				}
			}

			ToolstripCollection::ToolstripCollection(PassRefPtr<IContentCallback> _contentCallback, PassRefPtr<StackComposition> _stackComposition)
				: contentCallback(_contentCallback)
				, stackComposition(_stackComposition)
			{

			}

			ToolstripCollection::~ToolstripCollection()
			{

			}

			void ToolstripCollection::InvokeUpdateLayout()
			{
				if (contentCallback)
				{
					contentCallback->UpdateLayout();
				}
			}

			void ToolstripCollection::OnInterestingMenuButtonPropertyChanged(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				InvokeUpdateLayout();
			}

			bool ToolstripCollection::QueryInsert(SizeType index, ParamType child)
			{
				return true;
			}

			bool ToolstripCollection::QueryRemove(SizeType index, ParamType child)
			{
				return true;
			}

			void ToolstripCollection::BeforeInsert(SizeType index, ParamType child)
			{

			}

			void ToolstripCollection::BeforeRemove(SizeType index, ParamType child)
			{
				RefPtr<StackItemComposition> stackItem = stackComposition->GetStackItem(index);
				stackComposition->RemoveChild(stackItem);
				stackItem->RemoveChild(child->GetBoundsComposition());
				stackItem = nullptr;
				InvokeUpdateLayout();
			}

			void ToolstripCollection::AfterInsert(SizeType index, ParamType child)
			{
				RefPtr<StackItemComposition> stackItem = adoptRef(new StackItemComposition);
				child->GetBoundsComposition()->SetAlignmentToParent(CRect());
				stackItem->AddChild(child->GetBoundsComposition());
				stackComposition->InsertChild(index, stackItem);

				RefPtr<MenuButton> menuButton = dynamic_cast<MenuButton*>(child.get());
				if (menuButton)
				{
					menuButton->TextChanged.AttachMethod(this, &ToolstripCollection::OnInterestingMenuButtonPropertyChanged);
					menuButton->ShortcutTextChanged.AttachMethod(this, &ToolstripCollection::OnInterestingMenuButtonPropertyChanged);
				}
				InvokeUpdateLayout();
			}

			void ToolstripCollection::AfterRemove(SizeType index, SizeType count)
			{
				InvokeUpdateLayout();
			}

 			ToolstripBuilder::ToolstripBuilder(Environment _environment, PassRefPtr<ToolstripCollection> _toolstripItems)
 				: environment(_environment)
 				, toolstripItems(_toolstripItems)
 			{
 
 			}
 
 			ToolstripBuilder::~ToolstripBuilder()
 			{
 
 			}
 
 			ToolstripBuilder& ToolstripBuilder::Button(PassRefPtr<ImageData> image, const CString& text, RefPtr<ToolstripButton>& result)
 			{
				lastCreatedButton = nullptr;
				switch (environment)
				{
					case Env_Menu:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateMenuItemButtonStyle()));
						break;
					case Env_MenuBar:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateMenuBarButtonStyle()));
						break;
					case Env_ToolBar:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateToolBarButtonStyle()));
						break;
				}
				if (lastCreatedButton)
				{
					lastCreatedButton->SetImage(image);
					lastCreatedButton->SetText(text);
					if (!result)
					{
						result = lastCreatedButton;
					}
					toolstripItems->Add(lastCreatedButton);
				}
				return *this;
			}
 
 			ToolstripBuilder& ToolstripBuilder::Button(PassRefPtr<ToolstripCommand> command, RefPtr<ToolstripButton>& result)
 			{
				lastCreatedButton = nullptr;
				switch (environment)
				{
					case Env_Menu:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateMenuItemButtonStyle()));
						break;
					case Env_MenuBar:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateMenuBarButtonStyle()));
						break;
					case Env_ToolBar:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateToolBarButtonStyle()));
						break;
				}
				if (lastCreatedButton)
				{
					lastCreatedButton->SetCommand(command);
					if (!result)
					{
						result = lastCreatedButton;
					}
					toolstripItems->Add(lastCreatedButton);
				}
				return *this;
 			}
 
 			ToolstripBuilder& ToolstripBuilder::DropdownButton(PassRefPtr<ImageData> image, const CString& text, RefPtr<ToolstripButton>& result)
 			{
				lastCreatedButton = nullptr;
				switch (environment)
				{
					case Env_ToolBar:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateToolBarDropdownButtonStyle()));
						break;
				}
				if (lastCreatedButton)
				{
					lastCreatedButton->SetImage(image);
					lastCreatedButton->SetText(text);
					if (!result)
					{
						result = lastCreatedButton;
					}
					toolstripItems->Add(lastCreatedButton);
				}
				return *this;
 			}
 
 			ToolstripBuilder& ToolstripBuilder::DropdownButton(PassRefPtr<ToolstripCommand> command, RefPtr<ToolstripButton>& result)
 			{
				lastCreatedButton = nullptr;
				switch (environment)
				{
					case Env_ToolBar:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateToolBarDropdownButtonStyle()));
						break;
				}
				if (lastCreatedButton)
				{
					lastCreatedButton->SetCommand(command);
					if (!result)
					{
						result = lastCreatedButton;
					}
					toolstripItems->Add(lastCreatedButton);
				}
				return *this;
			}
 
 			ToolstripBuilder& ToolstripBuilder::SplitButton(PassRefPtr<ImageData> image, const CString& text, RefPtr<ToolstripButton>& result)
 			{
				lastCreatedButton = nullptr;
				switch (environment)
				{
					case Env_ToolBar:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateToolBarSplitButtonStyle()));
						break;
				}
				if (lastCreatedButton)
				{
					lastCreatedButton->SetImage(image);
					lastCreatedButton->SetText(text);
					if (!result)
					{
						result = lastCreatedButton;
					}
					toolstripItems->Add(lastCreatedButton);
				}
				return *this;
			}
 
 			ToolstripBuilder& ToolstripBuilder::SplitButton(PassRefPtr<ToolstripCommand> command, RefPtr<ToolstripButton>& result)
 			{
				lastCreatedButton = nullptr;
				switch (environment)
				{
					case Env_ToolBar:
						lastCreatedButton = adoptRef(new ToolstripButton(theme->CreateToolBarSplitButtonStyle()));
						break;
				}
				if (lastCreatedButton)
				{
					lastCreatedButton->SetCommand(command);
					if (!result)
					{
						result = lastCreatedButton;
					}
					toolstripItems->Add(lastCreatedButton);
				}
				return *this;
			}
 
 			ToolstripBuilder& ToolstripBuilder::Splitter()
 			{
				lastCreatedButton = nullptr;
				switch (environment)
				{
					case Env_Menu:
						toolstripItems->Add(adoptRef(new Control(theme->CreateMenuSplitterStyle())));
						break;
					case Env_ToolBar:
						toolstripItems->Add(adoptRef(new Control(theme->CreateToolBarSplitterStyle())));
						break;
				}
				return *this;
			}
 
			ToolstripBuilder& ToolstripBuilder::CommonControl(PassRefPtr<Control> control)
 			{
				toolstripItems->Add(control);
				return *this;
			}
 
 			ToolstripBuilder& ToolstripBuilder::BeginSubMenu()
 			{
				if (lastCreatedButton)
				{
					lastCreatedButton->CreateToolstripSubMenu(nullptr);
					RefPtr<ToolstripMenu> menu = lastCreatedButton->GetToolstripSubMenu();
					if (menu)
					{
						menu->GetBuilder(nullptr)->previousBuilder = this;
						return *menu->GetBuilder(nullptr);
					}
				}
				ASSERT(!"ERROR");
				return *this;
			}
 
 			ToolstripBuilder& ToolstripBuilder::EndSubMenu()
 			{
				return *previousBuilder;
			}

			ToolstripMenu::ToolstripMenu(PassRefPtr<IWindowStyleController> _styleController, PassRefPtr<Control> _owner)
				: Menu(_styleController, _owner)
			{
				sharedSizeRootComposition = adoptRef(new SharedSizeComposition);
				sharedSizeRootComposition->SetAlignmentToParent(CRect());
				sharedSizeRootComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				GetContainerComposition()->AddChild(sharedSizeRootComposition);

				stackComposition = adoptRef(new StackComposition);
				stackComposition->SetDirection(StackComposition::Vertical);
				stackComposition->SetAlignmentToParent(CRect());
				stackComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				sharedSizeRootComposition->AddChild(stackComposition);

				toolstripItems = adoptRef(new ToolstripCollection(this, stackComposition));
				builder = adoptRef(new ToolstripBuilder(ToolstripBuilder::Env_Menu, toolstripItems));
			}

			ToolstripMenu::~ToolstripMenu()
			{

			}

			ToolstripCollection& ToolstripMenu::GetToolstripItems()
			{
				return *toolstripItems;
			}

			PassRefPtr<ToolstripBuilder> ToolstripMenu::GetBuilder(PassRefPtr<ITheme> themeObject)
			{
				builder->theme = themeObject ? themeObject : GetStorage()->GetTheme();
				return builder;
			}

			void ToolstripMenu::UpdateLayout()
			{
				sharedSizeRootComposition->ForceCalculateSizeImmediately();
			}

			ToolstripMenuBar::ToolstripMenuBar(PassRefPtr<IControlStyleController> _styleController)
				: MenuBar(_styleController)
			{
				stackComposition = adoptRef(new StackComposition);
				stackComposition->SetDirection(StackComposition::Horizontal);
				stackComposition->SetAlignmentToParent(CRect());
				stackComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				GetContainerComposition()->AddChild(stackComposition);

				toolstripItems = adoptRef(new ToolstripCollection(nullptr, stackComposition));
				builder = adoptRef(new ToolstripBuilder(ToolstripBuilder::Env_MenuBar, toolstripItems));
			}

			ToolstripMenuBar::~ToolstripMenuBar()
			{

			}

			ToolstripCollection& ToolstripMenuBar::GetToolstripItems()
			{
				return *toolstripItems;
			}

			ToolstripBuilder& ToolstripMenuBar::GetBuilder(PassRefPtr<ITheme> themeObject)
			{
				builder->theme = themeObject ? themeObject : GetStorage()->GetTheme();
				return *builder;
			}

			ToolstripToolBar::ToolstripToolBar(PassRefPtr<IControlStyleController> _styleController)
				: Control(_styleController)
			{
				stackComposition = adoptRef(new StackComposition);
				stackComposition->SetDirection(StackComposition::Horizontal);
				stackComposition->SetAlignmentToParent(CRect());
				stackComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				GetContainerComposition()->AddChild(stackComposition);

				toolstripItems = adoptRef(new ToolstripCollection(nullptr, stackComposition));
				builder = adoptRef(new ToolstripBuilder(ToolstripBuilder::Env_ToolBar, toolstripItems));
			}

			ToolstripToolBar::~ToolstripToolBar()
			{

			}

			ToolstripCollection& ToolstripToolBar::GetToolstripItems()
			{
				return *toolstripItems;
			}

			ToolstripBuilder& ToolstripToolBar::GetBuilder(PassRefPtr<ITheme> themeObject)
			{
				builder->theme = themeObject ? themeObject : GetStorage()->GetTheme();
				return *builder;
			}

			ToolstripButton::ToolstripButton(PassRefPtr<IMenuButtonStyleController> _styleController)
				: MenuButton(_styleController)
			{
				Clicked.AttachMethod(this, &ToolstripButton::OnClicked);
			}

			ToolstripButton::~ToolstripButton()
			{

			}

			PassRefPtr<ToolstripCommand> ToolstripButton::GetCommand()
			{
				return command;
			}

			void ToolstripButton::SetCommand(PassRefPtr<ToolstripCommand> value)
			{
				if (command != value)
				{
					if (command)
					{
						command->DescriptionChanged.Detach(descriptionChangedHandler);
					}
					command = nullptr;
					descriptionChangedHandler = nullptr;
					if (value)
					{
						command = value;
						descriptionChangedHandler = command->DescriptionChanged.AttachMethod(this, &ToolstripButton::OnCommandDescriptionChanged);
					}
					UpdateCommandContent();
				}
			}

			PassRefPtr<ToolstripMenu> ToolstripButton::GetToolstripSubMenu()
			{
				return dynamic_cast<ToolstripMenu*>(GetSubMenu().get());
			}

			void ToolstripButton::CreateToolstripSubMenu(PassRefPtr<IWindowStyleController> subMenuStyleController)
			{
				if (!subMenu)
				{
					RefPtr<ToolstripMenu> newSubMenu = adoptRef(new ToolstripMenu(subMenuStyleController ? subMenuStyleController : styleController->CreateSubMenuStyleController(), this));
					SetSubMenu(newSubMenu, true);
				}
			}

			void ToolstripButton::UpdateCommandContent()
			{
				if (command)
				{
					SetImage(command->GetImage());
					SetText(command->GetText());
					SetEnabled(command->GetEnabled());
					SetSelected(command->GetSelected());
					if (command->GetShortcut())
					{
						SetShortcutText(command->GetShortcut()->GetName());
					}
					else
					{
						SetShortcutText(_T(""));
					}
				}
				else
				{
					SetImage(nullptr);
					SetText(_T(""));
					SetEnabled(true);
					SetSelected(false);
					SetShortcutText(_T(""));
				}
			}

			void ToolstripButton::OnClicked(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				if (command)
				{
					command->Executed.ExecuteWithNewSender(arguments, sender);
				}
			}

			void ToolstripButton::OnCommandDescriptionChanged(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				UpdateCommandContent();
			}

			namespace style
			{
				Win8MenuStyle::Win8MenuStyle()
				{
					{
						RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Menu_Border));
						boundsComposition = adoptRef(new BoundsComposition);
						boundsComposition->SetOwnedElement(element);
					}
					{
						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));
						RefPtr<BoundsComposition> subBorder = adoptRef(new BoundsComposition);
						subBorder->SetOwnedElement(element);
						subBorder->SetAlignmentToParent(CRect(1, 1, 1, 1));
						boundsComposition->AddChild(subBorder);
					}
					{
						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));
						containerComposition = adoptRef(new BoundsComposition);
						containerComposition->SetOwnedElement(element);
						containerComposition->SetAlignmentToParent(CRect(3, 3, 3, 3));
						containerComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
						boundsComposition->AddChild(containerComposition);
					}
				}

				Win8MenuStyle::~Win8MenuStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8MenuStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8MenuStyle::GetContainerComposition()
				{
					return containerComposition;
				}

				void Win8MenuStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8MenuStyle::SetText(const CString& value)
				{

				}

				void Win8MenuStyle::SetFont(const Font& value)
				{

				}

				void Win8MenuStyle::SetVisuallyEnabled(bool value)
				{

				}

				bool Win8MenuStyle::ShowShadow()
				{
					return false;
				}

				Win8MenuBarStyle::Win8MenuBarStyle()
				{
					boundsComposition = adoptRef(new BoundsComposition);
					{
						RefPtr<SolidBackgroundElement> solid = SolidBackgroundElement::Create();
						solid->SetColor(CColor(245, 246, 247));
						boundsComposition->SetOwnedElement(solid);
						boundsComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					}
					{
						RefPtr<Splitter3DElement> element = Splitter3DElement::Create();
						element->SetColors(CColor(232, 233, 234), CColor(240, 240, 240));
						element->SetDirection(Splitter3DElement::Horizontal);

						RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
						composition->SetOwnedElement(element);
						composition->SetPreferredMinSize(CSize(0, 2));
						composition->SetAlignmentToParent(CRect(0, -1, 0, 0));
						boundsComposition->AddChild(composition);
					}
					containerComposition = adoptRef(new BoundsComposition);
					{
						containerComposition->SetAlignmentToParent(CRect(0, 0, 0, 0));
						containerComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
						boundsComposition->AddChild(containerComposition);
					}
				}

				Win8MenuBarStyle::~Win8MenuBarStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8MenuBarStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8MenuBarStyle::GetContainerComposition()
				{
					return containerComposition;
				}

				void Win8MenuBarStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8MenuBarStyle::SetText(const CString& value)
				{

				}

				void Win8MenuBarStyle::SetFont(const Font& value)
				{

				}

				void Win8MenuBarStyle::SetVisuallyEnabled(bool value)
				{

				}

				Win8MenuBarButtonStyle::Win8MenuBarButtonStyle()
					: controlStyle(Button::Normal)
					, isVisuallyEnabled(true)
					, isOpening(false)
				{
					Win8ButtonColors initialColor = Win8ButtonColors::MenuBarButtonNormal();
					initialColor.SetAlphaWithoutText(0);

					elements = Win8ButtonElements::Create(Alignment::StringAlignmentCenter, Alignment::StringAlignmentCenter);
					elements.Apply(initialColor);
				}

				Win8MenuBarButtonStyle::~Win8MenuBarButtonStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8MenuBarButtonStyle::GetBoundsComposition()
				{
					return elements.mainComposition;
				}

				PassRefPtr<Composition> Win8MenuBarButtonStyle::GetContainerComposition()
				{
					return elements.mainComposition;
				}

				void Win8MenuBarButtonStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8MenuBarButtonStyle::SetText(const CString& value)
				{
					elements.textElement->SetText(value);
				}

				void Win8MenuBarButtonStyle::SetFont(const Font& value)
				{
					elements.textElement->SetFont(value);
					elements.textComposition->SetMargin(CRect(3, 3, 3, 3));
					CRect margin = elements.textComposition->GetMargin();
					margin.left *= 3;
					margin.right *= 3;
					elements.textComposition->SetMargin(margin);
				}

				void Win8MenuBarButtonStyle::SetVisuallyEnabled(bool value)
				{
					if (isVisuallyEnabled != value)
					{
						isVisuallyEnabled = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isOpening);
					}
				}

				void Win8MenuBarButtonStyle::SetSelected(bool value)
				{

				}

				PassRefPtr<IWindowStyleController> Win8MenuBarButtonStyle::CreateSubMenuStyleController()
				{
					return adoptRef(new Win8MenuStyle);
				}

				void Win8MenuBarButtonStyle::SetSubMenuExisting(bool value)
				{

				}

				void Win8MenuBarButtonStyle::SetSubMenuOpening(bool value)
				{
					if (isOpening != value)
					{
						isOpening = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isOpening);
					}
				}

				PassRefPtr<Button> Win8MenuBarButtonStyle::GetSubMenuHost()
				{
					return nullptr;
				}

				void Win8MenuBarButtonStyle::SetImage(PassRefPtr<ImageData> value)
				{

				}

				void Win8MenuBarButtonStyle::SetShortcutText(const CString& value)
				{

				}

				void Win8MenuBarButtonStyle::Transfer(Button::ControlState value)
				{
					if (controlStyle != value)
					{
						controlStyle = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isOpening);
					}
				}

				void Win8MenuBarButtonStyle::TransferInternal(Button::ControlState value, bool enabled, bool opening)
				{
					Win8ButtonColors targetColor;
					if (!enabled)
					{
						targetColor = Win8ButtonColors::MenuBarButtonDisabled();
						targetColor.SetAlphaWithoutText(0);
					}
					else if (opening)
					{
						targetColor = Win8ButtonColors::MenuBarButtonPressed();
					}
					else
					{
						switch (value)
						{
							case Button::Normal:
								targetColor = Win8ButtonColors::MenuBarButtonNormal();
								targetColor.SetAlphaWithoutText(0);
								break;
							case Button::Active:
								targetColor = Win8ButtonColors::MenuBarButtonActive();
								break;
							case Button::Pressed:
								targetColor = Win8ButtonColors::MenuBarButtonPressed();
								break;
						}
					}
					elements.Apply(targetColor);
				}

				Win8MenuItemButtonStyle::Win8MenuItemButtonStyle()
					: controlStyle(Button::Normal)
					, isVisuallyEnabled(true)
					, isSelected(false)
					, isOpening(false)
				{
					elements = Win8MenuItemButtonElements::Create();
					elements.Apply(Win8ButtonColors::MenuItemButtonNormal());
				}

				Win8MenuItemButtonStyle::~Win8MenuItemButtonStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8MenuItemButtonStyle::GetBoundsComposition()
				{
					return elements.mainComposition;
				}

				PassRefPtr<Composition> Win8MenuItemButtonStyle::GetContainerComposition()
				{
					return elements.mainComposition;
				}

				void Win8MenuItemButtonStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8MenuItemButtonStyle::SetText(const CString& value)
				{
					elements.textElement->SetText(value);
				}

				void Win8MenuItemButtonStyle::SetFont(const Font& value)
				{
					elements.textElement->SetFont(value);
					elements.textComposition->SetMargin(CRect(3, 3, 3, 3));
					elements.shortcutElement->SetFont(value);
					elements.shortcutComposition->SetMargin(CRect(3, 3, 3, 3));
				}

				void Win8MenuItemButtonStyle::SetVisuallyEnabled(bool value)
				{
					if (isVisuallyEnabled != value)
					{
						isVisuallyEnabled = value;
						elements.imageElement->SetEnabled(value);
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected, isOpening);
					}
				}

				void Win8MenuItemButtonStyle::SetSelected(bool value)
				{
					if (isSelected != value)
					{
						isSelected = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected, isOpening);
					}
				}

				PassRefPtr<IWindowStyleController> Win8MenuItemButtonStyle::CreateSubMenuStyleController()
				{
					return adoptRef(new Win8MenuStyle);
				}

				void Win8MenuItemButtonStyle::SetSubMenuExisting(bool value)
				{
					elements.SetSubMenuExisting(value);
				}

				void Win8MenuItemButtonStyle::SetSubMenuOpening(bool value)
				{
					if (isOpening != value)
					{
						isOpening = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected, isOpening);
					}
				}

				PassRefPtr<Button> Win8MenuItemButtonStyle::GetSubMenuHost()
				{
					return nullptr;
				}

				void Win8MenuItemButtonStyle::SetImage(PassRefPtr<ImageData> value)
				{
					if (value)
					{
						elements.imageElement->SetImage(value->image, value->frameIndex);
					}
					else
					{
						elements.imageElement->SetImage(nullptr, 0);
					}
				}

				void Win8MenuItemButtonStyle::SetShortcutText(const CString& value)
				{
					elements.shortcutElement->SetText(value);
				}

				void Win8MenuItemButtonStyle::Transfer(Button::ControlState value)
				{
					if (controlStyle != value)
					{
						controlStyle = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected, isOpening);
					}
				}

				void Win8MenuItemButtonStyle::TransferInternal(Button::ControlState value, bool enabled, bool selected, bool opening)
				{
					Win8ButtonColors targetColor;
					bool active = false;
					if (enabled)
					{
						if (opening)
						{
							targetColor = Win8ButtonColors::MenuItemButtonNormalActive();
							active = true;
						}
						else
						{
							switch (value)
							{
								case Button::Normal:
									targetColor = selected ? Win8ButtonColors::MenuItemButtonSelected() : Win8ButtonColors::MenuItemButtonNormal();
									break;
								case Button::Active:
								case Button::Pressed:
									targetColor = selected ? Win8ButtonColors::MenuItemButtonSelectedActive() : Win8ButtonColors::MenuItemButtonNormalActive();
									active = true;
									break;
							}
						}
					}
					else
					{
						switch (value)
						{
							case Button::Normal:
								targetColor = Win8ButtonColors::MenuItemButtonDisabled();
								break;
							case Button::Active:
							case Button::Pressed:
								targetColor = Win8ButtonColors::MenuItemButtonDisabledActive();
								active = true;
								break;
						}
					}
					elements.Apply(targetColor);
					elements.SetActive(active || selected);
				}

				Win8MenuSplitterStyle::Win8MenuSplitterStyle()
				{
					boundsComposition = adoptRef(new BoundsComposition);
					boundsComposition->SetPreferredMinSize(CSize(26, 5));
					{
						RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Menu_Splitter));

						RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
						composition->SetPreferredMinSize(CSize(1, 0));
						composition->SetOwnedElement(element);
						composition->SetAlignmentToParent(CRect(26, 0, -1, 0));
						boundsComposition->AddChild(composition);
					}
					{
						RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Menu_Splitter));

						RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
						composition->SetPreferredMinSize(CSize(0, 1));
						composition->SetOwnedElement(element);
						composition->SetAlignmentToParent(CRect(27, 2, 0, 2));
						boundsComposition->AddChild(composition);
					}
				}

				Win8MenuSplitterStyle::~Win8MenuSplitterStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8MenuSplitterStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8MenuSplitterStyle::GetContainerComposition()
				{
					return boundsComposition;
				}

				void Win8MenuSplitterStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8MenuSplitterStyle::SetText(const CString& value)
				{

				}

				void Win8MenuSplitterStyle::SetFont(const Font& value)
				{

				}

				void Win8MenuSplitterStyle::SetVisuallyEnabled(bool value)
				{

				}

				Win8ToolstripToolBarStyle::Win8ToolstripToolBarStyle()
				{
					{
						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));

						boundsComposition = adoptRef(new BoundsComposition);
						boundsComposition->SetOwnedElement(element);
						boundsComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					}
					{
						RefPtr<Splitter3DElement> element = Splitter3DElement::Create();
						element->SetColors(CColor(160, 160, 160), CColor(255, 255, 255));
						element->SetDirection(Splitter3DElement::Horizontal);

						RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
						composition->SetPreferredMinSize(CSize(0, 2));
						composition->SetAlignmentToParent(CRect(0, 0, 0, -1));
						composition->SetOwnedElement(element);
						boundsComposition->AddChild(composition);
					}
					{
						containerComposition = adoptRef(new BoundsComposition);
						containerComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
						containerComposition->SetAlignmentToParent(CRect(0, 2, 0, 0));
						boundsComposition->AddChild(containerComposition);
					}
				}

				Win8ToolstripToolBarStyle::~Win8ToolstripToolBarStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8ToolstripToolBarStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8ToolstripToolBarStyle::GetContainerComposition()
				{
					return containerComposition;
				}

				void Win8ToolstripToolBarStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8ToolstripToolBarStyle::SetText(const CString& value)
				{

				}

				void Win8ToolstripToolBarStyle::SetFont(const Font& value)
				{

				}

				void Win8ToolstripToolBarStyle::SetVisuallyEnabled(bool value)
				{

				}

				Win8ToolstripButtonDropdownStyle::Win8ToolstripButtonDropdownStyle()
					: controlState(Button::Normal)
					, isVisuallyEnabled(false)
				{
					boundsComposition = adoptRef(new BoundsComposition);
					boundsComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					{
						RefPtr<SolidBorderElement> splitterElement = SolidBorderElement::Create();
						splitterElement->SetColor(CColor(132, 132, 132));

						splitterComposition = adoptRef(new BoundsComposition);
						splitterComposition->SetAlignmentToParent(CRect(0, 3, -1, 3));
						splitterComposition->SetMinSizeLimitation(Composition::LimitToElement);
						splitterComposition->SetOwnedElement(splitterElement);
						splitterComposition->SetPreferredMinSize(CSize(1, 0));
						splitterComposition->SetVisible(false);
						boundsComposition->AddChild(splitterComposition);
					}
					{
						containerComposition = adoptRef(new BoundsComposition);
						containerComposition->SetAlignmentToParent(CRect(4, 0, 4, 0));
						containerComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
						boundsComposition->AddChild(containerComposition);
					}
				}

				Win8ToolstripButtonDropdownStyle::~Win8ToolstripButtonDropdownStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8ToolstripButtonDropdownStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8ToolstripButtonDropdownStyle::GetContainerComposition()
				{
					return containerComposition;
				}

				void Win8ToolstripButtonDropdownStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8ToolstripButtonDropdownStyle::SetText(const CString& value)
				{

				}

				void Win8ToolstripButtonDropdownStyle::SetFont(const Font& value)
				{

				}

				void Win8ToolstripButtonDropdownStyle::SetVisuallyEnabled(bool value)
				{
					if (isVisuallyEnabled != value)
					{
						isVisuallyEnabled = value;
						TransferInternal(controlState, isVisuallyEnabled);
					}
				}

				void Win8ToolstripButtonDropdownStyle::Transfer(Button::ControlState value)
				{
					if (controlState != value)
					{
						controlState = value;
						TransferInternal(controlState, isVisuallyEnabled);
					}
				}

				void Win8ToolstripButtonDropdownStyle::TransferInternal(Button::ControlState value, bool enabled)
				{
					splitterComposition->SetVisible(value != Button::Normal && enabled);
				}

				Win8ToolstripButtonStyle::Win8ToolstripButtonStyle(ButtonStyle _buttonStyle)
					: controlStyle(Button::Normal)
					, buttonStyle(_buttonStyle)
					, isVisuallyEnabled(true)
					, isSelected(false)
					, isOpening(false)
				{
					elements = Win8ButtonElements::Create(Alignment::StringAlignmentCenter, Alignment::StringAlignmentCenter);
					elements.Apply(Win8ButtonColors::ToolstripButtonNormal());
					transferringAnimation = adoptRef(new TransferringAnimationType(this, Win8ButtonColors::ToolstripButtonNormal()));

					elements.textComposition->SetMinSizeLimitation(Composition::NoLimit);
					imageElement = ImageFrameElement::Create();
					imageComposition = adoptRef(new BoundsComposition);
					imageComposition->SetOwnedElement(imageElement);
					imageComposition->SetMinSizeLimitation(Composition::LimitToElement);
					imageComposition->SetAlignmentToParent(CRect(4, 4, 4, 4));

					if (_buttonStyle == CommandButton)
					{
						GetContainerComposition()->AddChild(imageComposition);
					}
					else
					{
						RefPtr<TableComposition> table = adoptRef(new TableComposition);
						table->SetAlignmentToParent(CRect(0, 0, 0, 0));
						table->SetRowsAndColumns(1, 2);
						table->SetRowOption(0, CellOption::PercentageOption(1.0));
						table->SetColumnOption(0, CellOption::PercentageOption(1.0));
						table->SetColumnOption(1, CellOption::MinSizeOption());

						{
							RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
							table->AddChild(cell);
							cell->SetSite(0, 0, 1, 1);

							cell->AddChild(imageComposition);
						}
						{
							RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
							table->AddChild(cell);
							cell->SetSite(0, 1, 1, 1);
							RefPtr<PolygonElement> arrow = 0;
							RefPtr<BoundsComposition> arrowComposition = FragmentBuilder::BuildDownArrow(arrow);

							switch (_buttonStyle)
							{
								case DropdownButton:
								{
									arrowComposition->SetAlignmentToParent(CRect(0, 0, 4, 0));
									cell->AddChild(arrowComposition);
								}
									break;
								case SplitButton:
								{

									subMenuHost = adoptRef(new Button(adoptRef(new Win8ToolstripButtonDropdownStyle)));
									subMenuHost->GetContainerComposition()->AddChild(arrowComposition);
									subMenuHost->GetBoundsComposition()->SetAlignmentToParent(CRect(0, 0, 0, 0));
									cell->AddChild(subMenuHost->GetBoundsComposition());
								}
									break;
							}
						}
						GetContainerComposition()->AddChild(table);
					}
				}

				Win8ToolstripButtonStyle::~Win8ToolstripButtonStyle()
				{
					transferringAnimation->Disable();
				}

				PassRefPtr<BoundsComposition> Win8ToolstripButtonStyle::GetBoundsComposition()
				{
					return elements.mainComposition;
				}

				PassRefPtr<Composition> Win8ToolstripButtonStyle::GetContainerComposition()
				{
					return elements.mainComposition;
				}

				void Win8ToolstripButtonStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8ToolstripButtonStyle::SetText(const CString& value)
				{

				}

				void Win8ToolstripButtonStyle::SetFont(const Font& value)
				{
					elements.textElement->SetFont(value);
					elements.textComposition->SetMargin(CRect(3, 3, 3, 3));
				}

				void Win8ToolstripButtonStyle::SetVisuallyEnabled(bool value)
				{
					if (isVisuallyEnabled != value)
					{
						isVisuallyEnabled = value;
						imageElement->SetEnabled(value);
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected, isOpening);
					}
				}

				void Win8ToolstripButtonStyle::SetSelected(bool value)
				{
					if (isSelected != value)
					{
						isSelected = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected, isOpening);
					}
				}

				PassRefPtr<IWindowStyleController> Win8ToolstripButtonStyle::CreateSubMenuStyleController()
				{
					return adoptRef(new Win8MenuStyle);
				}

				void Win8ToolstripButtonStyle::SetSubMenuExisting(bool value)
				{

				}

				void Win8ToolstripButtonStyle::SetSubMenuOpening(bool value)
				{
					if (isOpening != value)
					{
						isOpening = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected, isOpening);
					}
				}

				PassRefPtr<Button> Win8ToolstripButtonStyle::GetSubMenuHost()
				{
					return subMenuHost;
				}

				void Win8ToolstripButtonStyle::SetImage(PassRefPtr<ImageData> value)
				{
					if (value)
					{
						imageElement->SetImage(value->image, value->frameIndex);
					}
					else
					{
						imageElement->SetImage(nullptr, 0);
					}
				}

				void Win8ToolstripButtonStyle::SetShortcutText(const CString& value)
				{

				}

				void Win8ToolstripButtonStyle::Transfer(Button::ControlState value)
				{
					if (controlStyle != value)
					{
						controlStyle = value;
						TransferInternal(controlStyle, isVisuallyEnabled, isSelected, isOpening);
					}
				}

				void Win8ToolstripButtonStyle::ToAnimation(PassRefPtr<GraphicsAnimationManager> manager)
				{
					manager->AddAnimation(transferringAnimation);
				}

				void Win8ToolstripButtonStyle::TransferInternal(Button::ControlState value, bool enabled, bool selected, bool menuOpening)
				{
					Win8ButtonColors targetColor;
					if (enabled)
					{
						if (menuOpening)
						{
							value = Button::Pressed;
						}
						switch (value)
						{
							case Button::Normal:
								targetColor = selected ? Win8ButtonColors::ToolstripButtonSelected() : Win8ButtonColors::ToolstripButtonNormal();
								break;
							case Button::Active:
								targetColor = selected ? Win8ButtonColors::ToolstripButtonSelected() : Win8ButtonColors::ToolstripButtonActive();
								break;
							case Button::Pressed:
								targetColor = Win8ButtonColors::ToolstripButtonPressed();
								break;
						}
					}
					else
					{
						targetColor = Win8ButtonColors::ToolstripButtonDisabled();
					}
					transferringAnimation->Transfer(targetColor);
				}

				Win8ToolstripButtonStyle::TransferringAnimationType::TransferringAnimationType(Win8ToolstripButtonStyle* _style, const Win8ButtonColors& begin)	: TransferringAnimation(_style, begin)
				{

				}

				void Win8ToolstripButtonStyle::TransferringAnimationType::PlayInternal(cint currentPosition, cint totalLength)
				{
					colorCurrent = Win8ButtonColors::Blend(colorBegin, colorEnd, currentPosition, totalLength);
					style->elements.Apply(colorCurrent);
				}

				Win8ToolstripSplitterStyle::Win8ToolstripSplitterStyle()
				{
					RefPtr<BoundsComposition> bounds = adoptRef(new BoundsComposition);
					bounds->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					boundsComposition = bounds;

					RefPtr<SolidBackgroundElement> backgroundElement = SolidBackgroundElement::Create();
					bounds->SetOwnedElement(backgroundElement);
					backgroundElement->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));

					RefPtr<BoundsComposition> splitterComposition = adoptRef(new BoundsComposition);
					bounds->AddChild(splitterComposition);
					splitterComposition->SetPreferredMinSize(CSize(1, 0));
					splitterComposition->SetAlignmentToParent(CRect(3, 3, 3, 3));

					RefPtr<SolidBorderElement> splitterElement = SolidBorderElement::Create();
					splitterComposition->SetOwnedElement(splitterElement);
					splitterElement->SetColor(CColor(132, 132, 132));
				}

				Win8ToolstripSplitterStyle::~Win8ToolstripSplitterStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8ToolstripSplitterStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8ToolstripSplitterStyle::GetContainerComposition()
				{
					return boundsComposition;
				}

				void Win8ToolstripSplitterStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8ToolstripSplitterStyle::SetText(const CString& value)
				{

				}

				void Win8ToolstripSplitterStyle::SetFont(const Font& value)
				{

				}

				void Win8ToolstripSplitterStyle::SetVisuallyEnabled(bool value)
				{

				}
			}
		}
	}
}