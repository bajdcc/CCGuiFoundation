#ifndef CC_TAB
#define CC_TAB

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
			namespace style
			{
				class ITabStyleController : public IControlStyleController
				{
				public:
					virtual void								SetCommandExecutor(PassRefPtr<ITabCommandExecutor> value) = 0;
					virtual void								InsertTab(cint index) = 0;
					virtual void								SetTabText(cint index, const CString& value) = 0;
					virtual void								RemoveTab(cint index) = 0;
					virtual void								MoveTab(cint oldIndex, cint newIndex) = 0;
					virtual void								SetSelectedTab(cint index) = 0;
				};
			}

			class Tab;

			class TabPage : public Object
			{
				friend class Tab;
			public:
				TabPage();
				~TabPage();

				NotifyEvent										AltChanged;
				NotifyEvent										TextChanged;
				NotifyEvent										PageInstalled;
				NotifyEvent										PageUninstalled;

				PassRefPtr<Composition>							GetContainerComposition();
				PassRefPtr<Tab>									GetOwnerTab();
				const CString&									GetText();
				void											SetText(const CString& value);
				bool											GetSelected();

			protected:
				bool											AssociateTab(PassRefPtr<Tab> _owner);
				bool											DeassociateTab(PassRefPtr<Tab> _owner);

				RefPtr<Control>									containerControl;
				RawPtr<Tab>										owner;
				CString											text;
			};

			class TabCommandExecutor : public ITabCommandExecutor
			{
			public:
				TabCommandExecutor(PassRefPtr<Tab> _tab);
				~TabCommandExecutor();

				void											ShowTab(cint index)override;

			protected:
				RawPtr<Tab>										tab;
			};

			class Tab : public Control
			{
				friend class TabPage;
			public:
				Tab(PassRefPtr<ITabStyleController> _styleController);
				~Tab();

				NotifyEvent										SelectedPageChanged;

				PassRefPtr<TabPage>								CreatePage(cint index = -1);
				bool											CreatePage(PassRefPtr<TabPage> page, cint index = -1);
				bool											RemovePage(PassRefPtr<TabPage> value);
				bool											MovePage(PassRefPtr<TabPage> page, cint newIndex);
				PassRefPtr<TabPage>								GetPage(cint index);

				PassRefPtr<TabPage>								GetSelectedPage();
				bool											SetSelectedPage(PassRefPtr<TabPage> value);

			protected:
				RefPtr<TabCommandExecutor>						commandExecutor;
				RawPtr<ITabStyleController>						styleController;
				vector<RefPtr<TabPage>>							tabPages;
				cint											selectedPageIndex;
			};

			namespace style
			{
				class Win8TabPageHeaderStyle : public Win8ButtonStyleBase
				{
				public:
					Win8TabPageHeaderStyle();
					~Win8TabPageHeaderStyle();

					void										SetFont(const Font& value)override;

				protected:
					void										TransferInternal(Button::ControlState value, bool enabled, bool selected)override;
				};

				class Win8TabStyle : public ITabStyleController
				{
				public:
					Win8TabStyle();
					~Win8TabStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;

					void										SetCommandExecutor(PassRefPtr<ITabCommandExecutor> value)override;
					void										InsertTab(cint index)override;
					void										SetTabText(cint index, const CString& value)override;
					void										RemoveTab(cint index)override;
					void										MoveTab(cint oldIndex, cint newIndex)override;
					void										SetSelectedTab(cint index)override;

				protected:
					void										Initialize();

					PassRefPtr<ISelectableButtonStyleController>CreateHeaderStyleController();
					PassRefPtr<IButtonStyleController>			CreateMenuButtonStyleController();
					PassRefPtr<IWindowStyleController>			CreateMenuStyleController();
					PassRefPtr<IMenuButtonStyleController>		CreateMenuItemStyleController();
					CColor										GetBorderColor();
					CColor										GetBackgroundColor();

					void										OnHeaderButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments);
					void										OnTabHeaderBoundsChanged(PassRefPtr<Composition> sender, EventArgs& arguments);
					void										OnHeaderOverflowButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments);
					void										OnHeaderOverflowMenuButtonClicked(PassRefPtr<Composition> sender, EventArgs& arguments);

					void										UpdateHeaderOverflowButtonVisibility();
					void										UpdateHeaderZOrder();
					void										UpdateHeaderVisibilityIndex();
					void										UpdateHeaderLayout();

					RefPtr<TableComposition>					boundsComposition;
					RefPtr<BoundsComposition>					containerComposition;
					RefPtr<StackComposition>					tabHeaderComposition;
					RefPtr<BoundsComposition>					tabContentTopLineComposition;
					Font										headerFont;
					RefPtr<ITabCommandExecutor>					commandExecutor;

					RefPtr<MutexGroupController>				headerController;
					vector<RefPtr<SelectableButton>>			headerButtons;
					RefPtr<PolygonElement>						headerOverflowArrowElement;
					RefPtr<Button>								headerOverflowButton;
					RefPtr<ToolstripMenu>						headerOverflowMenu;
				};
			}
		}
	}
}

#endif