#include "stdafx.h"
#include "cc_comctl.h"
#include "cc_tab.h"
#include "cc_presentation.h"
#include "cc_toolstrip.h"

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
				PassRefPtr<ITabStyleController> Win8Style::CreateTabStyle()
				{
					return adoptRef(new Win8TabStyle);
				}
			}

			TabPage::TabPage()
			{
				containerControl = adoptRef(new Control(adoptRef(new EmptyStyleController)));
				containerControl->GetBoundsComposition()->SetAlignmentToParent(CRect(2, 2, 2, 2));

				AltChanged.SetAssociatedComposition(containerControl->GetBoundsComposition());
				TextChanged.SetAssociatedComposition(containerControl->GetBoundsComposition());
				PageInstalled.SetAssociatedComposition(containerControl->GetBoundsComposition());
				PageUninstalled.SetAssociatedComposition(containerControl->GetBoundsComposition());
			}

			TabPage::~TabPage()
			{

			}

			PassRefPtr<Composition> TabPage::GetContainerComposition()
			{
				return containerControl->GetContainerComposition();
			}

			PassRefPtr<Tab> TabPage::GetOwnerTab()
			{
				return owner;
			}

			const CString& TabPage::GetText()
			{
				return text;
			}

			void TabPage::SetText(const CString& value)
			{
				if (text != value)
				{
					text = value;
					cint index = 0;
					for (auto & tabPage : owner->tabPages)
					{
						if (tabPage == this)
							break;
						index++;
					}
					if (index < (cint)owner->tabPages.size())
					{
						if (owner)
						{
							owner->styleController->SetTabText(index, text);
						}
						TextChanged.Execute(containerControl->GetNotifyEventArguments());
					}
				}
			}

			bool TabPage::GetSelected()
			{
				return owner->GetSelectedPage() == this;
			}

			bool TabPage::AssociateTab(PassRefPtr<Tab> _owner)
			{
				if (owner)
				{
					return false;
				}
				else
				{
					owner = _owner;
					PageInstalled.Execute(containerControl->GetNotifyEventArguments());
					return true;
				}
			}

			bool TabPage::DeassociateTab(PassRefPtr<Tab> _owner)
			{
				if (owner && owner == _owner)
				{
					PageUninstalled.Execute(containerControl->GetNotifyEventArguments());
					owner = nullptr;
					return true;
				}
				else
				{
					return false;
				}
			}

			TabCommandExecutor::TabCommandExecutor(PassRefPtr<Tab> _tab)
				: tab(_tab)
			{

			}

			TabCommandExecutor::~TabCommandExecutor()
			{

			}

			void TabCommandExecutor::ShowTab(cint index)
			{
				tab->SetSelectedPage(tab->GetPage(index));
			}

			Tab::Tab(PassRefPtr<ITabStyleController> _styleController)
				: Control(_styleController.get())
				, styleController(_styleController.get())
				, selectedPageIndex(-1)
			{
				commandExecutor = adoptRef(new TabCommandExecutor(this));
				styleController->SetCommandExecutor(commandExecutor);
			}

			Tab::~Tab()
			{

			}

			PassRefPtr<TabPage> Tab::CreatePage(cint index /*= -1*/)
			{
				RefPtr<TabPage> page = adoptRef(new TabPage);
				if (CreatePage(page, index))
				{
					return page;
				}
				else
				{
					return nullptr;
				}
			}

			bool Tab::CreatePage(PassRefPtr<TabPage> page, cint index /*= -1*/)
			{
				RefPtr<TabPage> _page = page;
				if (index >= (cint)tabPages.size())
				{
					index = (cint)tabPages.size() - 1;
				}
				else if (index < -1)
				{
					index = -1;
				}

				if (_page->AssociateTab(this))
				{
					if (index = -1)
					{
						tabPages.push_back(_page);
						index = tabPages.size() - 1;
					}
					else
					{
						tabPages.insert(tabPages.begin() + index, _page);
					}
					GetContainerComposition()->AddChild(_page->GetContainerComposition());
					styleController->InsertTab(index);
					styleController->SetTabText(index, _page->GetText());

					if (selectedPageIndex == -1)
					{
						SetSelectedPage(_page);
					}
					_page->GetContainerComposition()->SetVisible(_page == tabPages[selectedPageIndex]);
					return true;
				}
				else
				{
					return false;
				}
			}

			bool Tab::RemovePage(PassRefPtr<TabPage> value)
			{
				if (value->GetOwnerTab() == this && value->DeassociateTab(this))
				{
					cint index = 0;
					for (auto & tabPage : tabPages)
					{
						if (tabPage == value)
							break;
						index++;
					}
					if (index == (cint)tabPages.size())
					{
						return false;
					}
					styleController->RemoveTab(index);
					GetContainerComposition()->RemoveChild(value->GetContainerComposition());
					tabPages.erase(tabPages.begin() + index);
					if (tabPages.empty())
					{
						SetSelectedPage(nullptr);
						return nullptr;
					}
					else if (tabPages[selectedPageIndex] == value)
					{
						auto _index = __max(index - 1, 0);
						SetSelectedPage(tabPages[_index]);
					}
					return true;
				}
				else
				{
					return false;
				}
			}

			bool Tab::MovePage(PassRefPtr<TabPage> page, cint newIndex)
			{
				RefPtr<TabPage> _page = page;
				if (!_page) return false;
				cint index = 0;
				for (auto & tabPage : tabPages)
				{
					if (tabPage == _page)
						break;
					index++;
				}
				if (index == (cint)tabPages.size())
					return false;
				tabPages.erase(tabPages.begin() + index);
				tabPages.insert(tabPages.begin() + newIndex, _page);
				styleController->MoveTab(index, newIndex);
				styleController->SetSelectedTab(selectedPageIndex);
				return true;
			}

			PassRefPtr<TabPage> Tab::GetPage(cint index)
			{
				return tabPages[index];
			}

			PassRefPtr<TabPage> Tab::GetSelectedPage()
			{
				return tabPages[selectedPageIndex];
			}

			bool Tab::SetSelectedPage(PassRefPtr<TabPage> value)
			{
				RefPtr<TabPage> _value = value;
				if (!_value)
				{
					if (tabPages.empty())
					{
						selectedPageIndex = -1;
					}
				}
				else if (_value->GetOwnerTab() == this)
				{
					if (selectedPageIndex == -1 ? _value != nullptr : tabPages[selectedPageIndex] != _value)
					{
						cint index = 0;
						for (auto & tabPage : tabPages)
						{
							if (tabPage == _value)
								break;
							index++;
						}
						if (index == (cint)tabPages.size())
							return false;
						selectedPageIndex = index;
						for (cint i = 0; i < (cint)tabPages.size(); i++)
						{
							bool selected = tabPages[i] == _value;
							tabPages[i]->GetContainerComposition()->SetVisible(selected);
							if (selected)
							{
								styleController->SetSelectedTab(i);
							}
						}
						SelectedPageChanged.Execute(GetNotifyEventArguments());
					}
				}
				return selectedPageIndex == -1 ?
					_value == nullptr :
					tabPages[selectedPageIndex] == _value;
			}

			namespace style
			{
				Win8TabPageHeaderStyle::Win8TabPageHeaderStyle()
					: Win8ButtonStyleBase(Win8ButtonColors::TabPageHeaderNormal(), Alignment::StringAlignmentNear, Alignment::StringAlignmentCenter)
				{
					transferringAnimation->SetEnableAnimation(false);
					{
						CRect margin = elements.backgroundComposition->GetAlignmentToParent();
						margin.bottom = 0;
						elements.backgroundComposition->SetAlignmentToParent(margin);
					}
				}

				Win8TabPageHeaderStyle::~Win8TabPageHeaderStyle()
				{

				}

				void Win8TabPageHeaderStyle::SetFont(const Font& value)
				{
					Win8ButtonStyleBase::SetFont(value);
					CRect margin = elements.textComposition->GetMargin();
					margin.left *= 2;
					margin.right *= 2;
					elements.textComposition->SetMargin(margin);
				}

				void Win8TabPageHeaderStyle::TransferInternal(Button::ControlState value, bool enabled, bool selected)
				{
					if (selected)
					{
						transferringAnimation->Transfer(Win8ButtonColors::TabPageHeaderSelected());
					}
					else
					{
						switch (value)
						{
							case Button::Normal:
								transferringAnimation->Transfer(Win8ButtonColors::TabPageHeaderNormal());
								break;
							case Button::Active:
							case Button::Pressed:
								transferringAnimation->Transfer(Win8ButtonColors::TabPageHeaderActive());
								break;
						}
					}
				}

				Win8TabStyle::Win8TabStyle()
				{
					Initialize();
				}

				Win8TabStyle::~Win8TabStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8TabStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8TabStyle::GetContainerComposition()
				{
					return containerComposition;
				}

				void Win8TabStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8TabStyle::SetText(const CString& value)
				{

				}

				void Win8TabStyle::SetFont(const Font& value)
				{
					headerFont = value;
				}

				void Win8TabStyle::SetVisuallyEnabled(bool value)
				{

				}

				void Win8TabStyle::SetCommandExecutor(PassRefPtr<ITabCommandExecutor> value)
				{
					commandExecutor = value;
				}

				void Win8TabStyle::InsertTab(cint index)
				{
					RefPtr<SelectableButton> button = adoptRef(new SelectableButton(CreateHeaderStyleController()));
					button->SetAutoSelection(false);
					button->SetFont(headerFont);
					button->GetBoundsComposition()->SetAlignmentToParent(CRect(0, 0, 0, 0));
					button->SetGroupController(headerController);
					button->Clicked.AttachMethod(this, &Win8TabStyle::OnHeaderButtonClicked);

					RefPtr<StackItemComposition> item = adoptRef(new StackItemComposition);
					item->AddChild(button->GetBoundsComposition());
					tabHeaderComposition->InsertStackItem(index, item);
					headerButtons.insert(headerButtons.begin() + index, button);

					RefPtr<ToolstripButton> menuItem = adoptRef(new ToolstripButton(CreateMenuItemStyleController()));
					menuItem->Clicked.AttachMethod(this, &Win8TabStyle::OnHeaderOverflowMenuButtonClicked);
					headerOverflowMenu->GetToolstripItems().Insert(index, menuItem);

					UpdateHeaderLayout();
				}

				void Win8TabStyle::SetTabText(cint index, const CString& value)
				{
					headerButtons[index]->SetText(value);
					headerOverflowMenu->GetToolstripItems().Get(index)->SetText(value);

					UpdateHeaderLayout();
				}

				void Win8TabStyle::RemoveTab(cint index)
				{
					RefPtr<StackItemComposition> item = tabHeaderComposition->GetStackItem(index);
					RefPtr<SelectableButton> button = headerButtons[index];

					tabHeaderComposition->RemoveChild(item);
					item->RemoveChild(button->GetBoundsComposition());
					headerButtons.erase(headerButtons.begin() + index);

					headerOverflowMenu->GetToolstripItems().RemoveAt(index);

					UpdateHeaderLayout();
				}

				void Win8TabStyle::MoveTab(cint oldIndex, cint newIndex)
				{
					RefPtr<StackItemComposition> item = tabHeaderComposition->GetStackItem(oldIndex);
					tabHeaderComposition->RemoveChild(item);
					tabHeaderComposition->InsertStackItem(newIndex, item);

					RefPtr<SelectableButton> button = headerButtons[oldIndex];
					headerButtons.erase(headerButtons.begin() + oldIndex);
					headerButtons.insert(headerButtons.begin() + newIndex, button);

					UpdateHeaderLayout();
				}

				void Win8TabStyle::SetSelectedTab(cint index)
				{
					headerButtons[index]->SetSelected(true);

					UpdateHeaderLayout();
				}

				void Win8TabStyle::Initialize()
				{
					boundsComposition = adoptRef(new TableComposition);
					boundsComposition->SetRowsAndColumns(2, 2);
					boundsComposition->SetRowOption(0, CellOption::MinSizeOption());
					boundsComposition->SetRowOption(1, CellOption::PercentageOption(1.0));
					boundsComposition->SetColumnOption(0, CellOption::PercentageOption(1.0));
					boundsComposition->SetColumnOption(1, CellOption::AbsoluteOption(0));
					{
						RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
						boundsComposition->AddChild(cell);
						cell->SetSite(0, 0, 1, 1);

						tabHeaderComposition = adoptRef(new StackComposition);
						tabHeaderComposition->SetExtraMargin(CRect(2, 2, 2, 0));
						tabHeaderComposition->SetAlignmentToParent(CRect(0, 0, 0, 0));
						tabHeaderComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
						tabHeaderComposition->BoundsChanged.AttachMethod(this, &Win8TabStyle::OnTabHeaderBoundsChanged);
						cell->AddChild(tabHeaderComposition);
					}
					{
						RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
						boundsComposition->AddChild(cell);
						cell->SetSite(0, 1, 1, 1);

						headerOverflowButton = adoptRef(new Button(CreateMenuButtonStyleController()));
						headerOverflowButton->GetContainerComposition()->AddChild(FragmentBuilder::BuildDownArrow(headerOverflowArrowElement));
						headerOverflowButton->GetBoundsComposition()->SetAlignmentToParent(CRect(-1, 0, 0, 0));
						headerOverflowButton->Clicked.AttachMethod(this, &Win8TabStyle::OnHeaderOverflowButtonClicked);
						cell->AddChild(headerOverflowButton->GetBoundsComposition());
					}
					{
						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(GetBorderColor());

						tabContentTopLineComposition = adoptRef(new BoundsComposition);
						tabContentTopLineComposition->SetOwnedElement(element);
						tabContentTopLineComposition->SetAlignmentToParent(CRect(0, -1, 0, 0));
						tabContentTopLineComposition->SetPreferredMinSize(CSize(0, 1));
						tabHeaderComposition->AddChild(tabContentTopLineComposition);
					}
					{
						RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
						boundsComposition->AddChild(cell);
						cell->SetSite(1, 0, 1, 2);

						RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
						element->SetColor(GetBackgroundColor());

						containerComposition = adoptRef(new BoundsComposition);
						containerComposition->SetOwnedElement(element);
						containerComposition->SetAlignmentToParent(CRect(1, 0, 1, 1));
						containerComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
						cell->AddChild(containerComposition);

						{
							RefPtr<SolidBorderElement> element = SolidBorderElement::Create();
							element->SetColor(GetBorderColor());
							cell->SetOwnedElement(element);
						}
						{
							RefPtr<SolidBackgroundElement> element = SolidBackgroundElement::Create();
							element->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Tab));
							containerComposition->SetOwnedElement(element);
						}
					}

					headerOverflowMenu = adoptRef(new ToolstripMenu(CreateMenuStyleController(), 0));
					headerController = adoptRef(new MutexGroupController);
				}

				PassRefPtr<ISelectableButtonStyleController> Win8TabStyle::CreateHeaderStyleController()
				{
					return adoptRef(new Win8TabPageHeaderStyle);
				}

				PassRefPtr<IButtonStyleController> Win8TabStyle::CreateMenuButtonStyleController()
				{
					return adoptRef(new Win8ButtonStyle);
				}

				PassRefPtr<IWindowStyleController> Win8TabStyle::CreateMenuStyleController()
				{
					return adoptRef(new Win8MenuStyle);
				}

				PassRefPtr<IMenuButtonStyleController> Win8TabStyle::CreateMenuItemStyleController()
				{
					return adoptRef(new Win8MenuItemButtonStyle);
				}

				CColor Win8TabStyle::GetBorderColor()
				{
					return Win8ButtonColors::TabPageHeaderNormal().borderColor;
				}

				CColor Win8TabStyle::GetBackgroundColor()
				{
					return Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Tab);
				}

				void Win8TabStyle::OnHeaderButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					if (commandExecutor)
					{
						RefPtr<SelectableButton> value = dynamic_cast<SelectableButton*>(sender->GetAssociatedControl().get());
						cint index = 0;
						for (auto & tabPage : headerButtons)
						{
							if (tabPage == value)
								break;
							index++;
						}
						if (index < (cint)headerButtons.size())
						{
							commandExecutor->ShowTab(index);
						}
					}
				}

				void Win8TabStyle::OnTabHeaderBoundsChanged(PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					cint height = headerOverflowButton->GetBoundsComposition()->GetBounds().Height();
					headerOverflowButton->GetBoundsComposition()->SetBounds(CRect(CPoint(), CSize(height, 0)));

					UpdateHeaderLayout();
				}

				void Win8TabStyle::OnHeaderOverflowButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					headerOverflowMenu->SetClientSize(CSize());
					headerOverflowMenu->ShowPopup(headerOverflowButton, true);
				}

				void Win8TabStyle::OnHeaderOverflowMenuButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					auto index = headerOverflowMenu->GetToolstripItems().IndexOf(sender->GetRelatedControl());
					if (index)
					{
						commandExecutor->ShowTab(~index);
					}
				}

				void Win8TabStyle::UpdateHeaderOverflowButtonVisibility()
				{
					if (tabHeaderComposition->IsStackItemClipped())
					{
						boundsComposition->SetColumnOption(1, CellOption::MinSizeOption());
					}
					else
					{
						boundsComposition->SetColumnOption(1, CellOption::AbsoluteOption(0));
					}
					boundsComposition->ForceCalculateSizeImmediately();
				}

				void Win8TabStyle::UpdateHeaderZOrder()
				{
					cint itemCount = (cint)tabHeaderComposition->GetStackItems().size();
					cint childCount = (cint)tabHeaderComposition->Children().size();
					for (cint i = 0; i < itemCount; i++)
					{
						RefPtr<StackItemComposition> item = tabHeaderComposition->GetStackItem(i);
						if (headerButtons[i]->GetSelected())
						{
							tabHeaderComposition->MoveChild(item, childCount - 1);
							item->SetExtraMargin(CRect(2, 2, 2, 0));
						}
						else
						{
							item->SetExtraMargin(CRect(0, 0, 0, 0));
						}
					}
					if (childCount > 1)
					{
						tabHeaderComposition->MoveChild(tabContentTopLineComposition, childCount - 2);
					}
				}

				void Win8TabStyle::UpdateHeaderVisibilityIndex()
				{
					cint itemCount = (cint)tabHeaderComposition->GetStackItems().size();
					cint selectedItem = -1;
					for (cint i = 0; i < itemCount; i++)
					{
						if (headerButtons[i]->GetSelected())
						{
							selectedItem = i;
						}
					}

					if (selectedItem != -1)
					{
						tabHeaderComposition->EnsureVisible(selectedItem);
					}
				}

				void Win8TabStyle::UpdateHeaderLayout()
				{
					UpdateHeaderZOrder();
					UpdateHeaderVisibilityIndex();
					UpdateHeaderOverflowButtonVisibility();
				}
			}
		}
	}
}