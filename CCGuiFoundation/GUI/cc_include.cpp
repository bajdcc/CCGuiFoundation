#include "stdafx.h"
#include "cc_include.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_resource.h"
#include "cc_presentation.h"
#include "cc_controller.h"
#include "cc_direct2d.h"
#include "cc_control.h"
#include "cc_element.h"
#include "cc_comctl.h"
#include "cc_label.h"
#include "cc_button.h"
#include "cc_textbox.h"
#include "cc_scroll.h"
#include "cc_toolstrip.h"
#include "cc_tab.h"
#include "cc_console.h"

using namespace cc::interfaces::windows;
using namespace cc::presentation::control;
using namespace cc::presentation::control::composition;
using namespace cc::presentation::control::event_args;
using namespace cc::presentation::direct2d;
using namespace cc::presentation::direct2d::resource;
using namespace cc::presentation::element;
using namespace cc::presentation::windows;
using namespace cc::presentation::windows::controller;

void SetupD2D();

//////////////////////////////////////////////////////////////////////////

extern "C" CCGUI_API int SetupCCGuiFoundation()
{
	_tsetlocale(LC_CTYPE, _T("chs"));
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
// 	_CrtSetBreakAlloc(160);
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	auto_ptr<WinDebug> debug(new WinDebug);
	GetStorage()->SetDebug(debug.get());
	SetupD2D();
	::CoUninitialize();
	debug.reset();
	_CrtDumpMemoryLeaks();
	return 0;
}

class TestWindow : public Window
{
public:
	TestWindow()
		: Window(GetStorage()->GetTheme()->CreateWindowStyle())
	{
		InitComponent();
	}

	~TestWindow()
	{

	}

	void InitMenuBar()
	{
		menuBar = cc::global::ControlFactory::NewMenuBar();
		menuBar->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
		menuBar->GetBoundsComposition()->SetAlignmentToParent(CRect(0, 0, 0, 0));

		RefPtr<ToolstripButton> tb;
		menuBar->GetBuilder(nullptr)
			.Button(nullptr, L"File", tb)
			.BeginSubMenu()
			.Button(commands[0], tb)
			.Splitter()
			.Button(commands[1], tb)
			.EndSubMenu()
			.Button(0, L"Edit", tb)
			.BeginSubMenu()
			.Button(nullptr, L"JJ", tb)
			.Button(nullptr, L"Good", tb)
			.BeginSubMenu()
			.Button(0, L"TTT", tb)
			.Splitter()
			.Button(commands[2], tb)
			.BeginSubMenu()
			.Button(0, L"TTT", tb)
			.Splitter()
			.Button(commands[2], tb)
			.BeginSubMenu()
			.Button(0, L"TTT", tb)
			.Splitter()
			.Button(commands[2], tb)
			.BeginSubMenu()
			.Button(0, L"TTT", tb)
			.Splitter()
			.Button(commands[2], tb)
			.Splitter()
			.Button(commands[3], tb)
			.EndSubMenu().Splitter()
			.Button(commands[3], tb)
			.EndSubMenu().Splitter()
			.Button(commands[3], tb)
			.EndSubMenu()
			.Splitter()
			.Button(commands[3], tb)
			.EndSubMenu()
			.Button(nullptr, L"Ghhh", tb)
			.BeginSubMenu()
			.Button(commands[2], tb)
			.Splitter()
			.Button(commands[3], tb)
			.EndSubMenu()
			.EndSubMenu()
			;
	}

	void InitToolBar()
	{
		toolBar = cc::global::ControlFactory::NewToolBar();
		toolBar->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
		toolBar->GetBoundsComposition()->SetAlignmentToParent(CRect());

		RefPtr<ToolstripButton> tb;
		toolBar->GetBuilder(nullptr)
			.Button(commands[0], tb)
			.Splitter()
			.DropdownButton(commands[2], tb)
			.BeginSubMenu()
			.Button(commands[3], tb)
			.Button(commands[3], tb)
			.EndSubMenu()
			.Splitter()
			.SplitButton(commands[2], tb)
			.BeginSubMenu()
			.Button(commands[3], tb)
			.Button(commands[3], tb)
			.EndSubMenu()
			.Splitter()
			.Button(commands[3], tb)
			;
	}

	void InitCommand()
	{
		{
			RefPtr<ToolstripCommand> cmd = adoptRef(new ToolstripCommand);
			commands.push_back(cmd);
			cmd->SetText(L"New");
			cmd->SetImage(adoptRef(new ImageData(GetStorage()->GetController()->GetImageService()->CreateImageFromFile(L"R:\\_New.png"), 0, L"")));
			cmd->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'N'));
			cmd->Executed.AttachLambda([this](PassRefPtr<Composition> sender, EventArgs& args)
			{
				//PostMessage(GetHWNDFromWindow(GetWindow()), WM_CLOSE, 0, 0);
				Hide();
			});
			AddComponent(cmd);
		}
		{
			RefPtr<ToolstripCommand> cmd = adoptRef(new ToolstripCommand);
			commands.push_back(cmd);
			cmd->SetText(L"Open");
			cmd->SetImage(adoptRef(new ImageData(GetStorage()->GetController()->GetImageService()->CreateImageFromFile(L"R:\\_Open.png"), 0, L"")));
			cmd->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'O'));
			cmd->Executed.AttachLambda([this](PassRefPtr<Composition> sender, EventArgs& args)
			{
				GetStorage()->GetController()->GetDialogService()->
					ShowMessageBox(GetWindow(), L"hello", L"hello");
			});
			AddComponent(cmd);
		}
		{
			RefPtr<ToolstripCommand> cmd = adoptRef(new ToolstripCommand);
			commands.push_back(cmd);
			cmd->SetText(L"Print");
			cmd->SetImage(adoptRef(new ImageData(GetStorage()->GetController()->GetImageService()->CreateImageFromFile(L"R:\\_Print.png"), 0, L"")));
			cmd->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'P'));
			AddComponent(cmd);
		}
		{
			RefPtr<ToolstripCommand> cmd = adoptRef(new ToolstripCommand);
			commands.push_back(cmd);
			cmd->SetEnabled(false);
			cmd->SetText(L"Delete");
			cmd->SetImage(adoptRef(new ImageData(GetStorage()->GetController()->GetImageService()->CreateImageFromFile(L"R:\\_Delete.png"), 0, L"")));
			cmd->SetShortcut(shortcutKeyManager->CreateShortcut(true, true, true, L'D'));
			AddComponent(cmd);
		}
	}

	void InitComponent()
	{
		shortcutKeyManager = adoptRef(new ShortcutKeyManager);
		SetShortcutKeyManager(shortcutKeyManager);

		InitCommand();

		Font font;
		font.size = 48;
		font.fontFamily = _T("方正隶变_GBK");

		Font font1;
		font1.size = 12;
		font1.fontFamily = _T("Microsoft Yahei");

		SetText(_T("Test"));

		RefPtr<TableComposition> tableRoot = adoptRef(new TableComposition);
		{
			tableRoot->SetCellPadding(0);
			tableRoot->SetAlignmentToParent(CRect(0, 0, 0, 0));
			tableRoot->SetRowsAndColumns(3, 1);
			tableRoot->SetRowOption(0, CellOption::MinSizeOption());
			tableRoot->SetRowOption(1, CellOption::MinSizeOption());
			tableRoot->SetRowOption(2, CellOption::PercentageOption(1.0));
			tableRoot->SetColumnOption(0, CellOption::PercentageOption(1.0));
			GetContainerComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);

			GetContainerComposition()->AddChild(tableRoot);
		}

		{
			RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
			tableRoot->AddChild(cell);
			cell->SetSite(0, 0, 1, 1);

			InitMenuBar();
			cell->AddChild(menuBar->GetBoundsComposition());
		}
		{
			RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
			tableRoot->AddChild(cell);
			cell->SetSite(1, 0, 1, 1);
			cell->SetInternalMargin(CRect(1, 0, 1, 0));

			InitToolBar();
			cell->AddChild(toolBar->GetBoundsComposition());
		}
		{
			RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
			tableRoot->AddChild(cell);

			cell->SetSite(2, 0, 1, 1);
			cell->SetInternalMargin(CRect(1, 1, 1, 1));

			tabControl = cc::global::ControlFactory::NewTab();
			tabControl->GetBoundsComposition()->SetAlignmentToParent(CRect(1, 1, 1, 1));
			cell->AddChild(tabControl->GetBoundsComposition());
		}

		RefPtr<TableComposition> table = adoptRef(new TableComposition);
		{
			table->SetRowsAndColumns(4, 3);
			table->SetRowOption(0, CellOption::PercentageOption(0.4));
			table->SetRowOption(1, CellOption::PercentageOption(0.4));
			table->SetRowOption(2, CellOption::PercentageOption(0.1));
			table->SetRowOption(3, CellOption::AbsoluteOption(140));
			table->SetColumnOption(0, CellOption::PercentageOption(0.35));
			table->SetColumnOption(1, CellOption::PercentageOption(0.35));
			table->SetColumnOption(2, CellOption::PercentageOption(0.30));
			table->SetAlignmentToParent(CRect());
			table->SetCellPadding(6);
		}
		{
			mainTabPage = tabControl->CreatePage();
			mainTabPage->SetText(L"Main Page");
			mainTabPage->GetContainerComposition()->SetInternalMargin(CRect(2, 2, 2, 2));
			mainTabPage->GetContainerComposition()->AddChild(table);
			mainTabPage->GetContainerComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
		}
		{
			input = cc::global::ControlFactory::NewMultilineTextBox();
			input->SetText(L"Hello!");
			input->SetFont(font);
			input->GetBoundsComposition()->SetAlignmentToParent(CRect(1, 1, 1, 1));

			secondTabPage = tabControl->CreatePage();
			secondTabPage->SetText(L"Second Page");
			secondTabPage->GetContainerComposition()->SetInternalMargin(CRect(2, 2, 2, 2));
			secondTabPage->GetContainerComposition()->AddChild(input->GetBoundsComposition());
			secondTabPage->GetContainerComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
		}
		{
			RefPtr<Console> console = cc::global::ControlFactory::NewConsole();
			console->GetBoundsComposition()->SetAlignmentToParent(CRect());

			RefPtr<TabPage> consolePage = tabControl->CreatePage();
			consolePage->SetText(L"CONSOLE");
			consolePage->GetContainerComposition()->SetInternalMargin(CRect());
			consolePage->GetContainerComposition()->AddChild(console->GetBoundsComposition());
			consolePage->GetContainerComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
			tabControl->SetSelectedPage(consolePage);
		}
		{
			RefPtr<Console> console = cc::global::ControlFactory::NewConsole();
			console->GetBoundsComposition()->SetAlignmentToParent(CRect());

			RefPtr<TabPage> consolePage = tabControl->CreatePage();
			consolePage->SetText(L"CONSOLE2");
			consolePage->GetContainerComposition()->SetInternalMargin(CRect());
			consolePage->GetContainerComposition()->AddChild(console->GetBoundsComposition());
			consolePage->GetContainerComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
		}

		GetContainerComposition()->GetEventReceiver()->mouseMove.AttachMethod(this, &TestWindow::ShowPos);

		{
			RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
			cell->SetSite(0, 0, 1, 2);

			table->AddChild(cell);

			RefPtr<ScrollContainer> scrollContainer = cc::global::ControlFactory::NewScrollContainer();
			{
				RefPtr<Label> label = cc::global::ControlFactory::NewLabel();
				labelEx = label;
				{
					label->SetFont(font);
					label->SetText(_T("GUI Foundation!\n这是一个测试程序。\nHaha\n————————————————————————————"));
					label->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					label->GetBoundsComposition()->SetAlignmentToParent(CRect(2, 2, 2, 2));
				}
				scrollContainer->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				scrollContainer->GetBoundsComposition()->SetAlignmentToParent(CRect(1, 1, 1, 1));
				scrollContainer->AddChild(label);
			}

			cell->AddChild(scrollContainer->GetBoundsComposition());
		}
		{
			RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
			cell->SetSite(1, 0, 1, 1);

			table->AddChild(cell);

			RefPtr<Label> label = cc::global::ControlFactory::NewLabel();
			{
				labelPos = label;
				label->SetText(_T("Welcome!"));
				label->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				label->GetBoundsComposition()->SetAlignmentToParent(CRect());

				label->GetEventReceiver()->leftButtonDown.AttachLambda([](PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					GetStorage()->GetApplication()->InvokeAsync([]()
					{
						Sleep(1000);
						GetStorage()->GetApplication()->InvokeInMainThreadAndWait([](){
							RefPtr<IWindow> w = GetStorage()->GetApplication()->GetMainWindow()->GetWindow();
							GetStorage()->GetController()->GetDialogService()->
								ShowMessageBox(w, L"hello", L"hello");
						});
					});
				});
			}

			cell->AddChild(label->GetBoundsComposition());
		}
		{
			RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
			cell->SetSite(1, 1, 1, 1);

			table->AddChild(cell);

			RefPtr<Label> label = cc::global::ControlFactory::NewLabel();
			{
				RefPtr<ICursor> hand = GetStorage()->GetController()->GetResourceService()->GetSystemCursor(ICursor::Hand);
				label->GetBoundsComposition()->SetAssociatedCursor(hand);

				label->SetFont(font);
				label->SetText(_T("Welcome!"));
				label->SetTextColor(Gdiplus::Color::Blue);
				label->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				label->GetBoundsComposition()->SetAlignmentToParent(CRect());

				label->GetEventReceiver()->mouseEnter.AttachLambda([](PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					Font font = sender->GetRelatedControl()->GetFont();
					font.underline = true;
					sender->GetRelatedControl()->SetFont(font);
				});
				label->GetEventReceiver()->mouseLeave.AttachLambda([](PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					Font font = sender->GetRelatedControl()->GetFont();
					font.underline = false;
					sender->GetRelatedControl()->SetFont(font);
				});
				label->GetEventReceiver()->leftButtonDown.AttachLambda([](PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					{
						vector<CString> selectionFileNames;
						cint selectionFilterIndex = 0;
						if (GetStorage()->GetController()->GetDialogService()->ShowFileDialog(
							sender->GetRelatedGraphicsHost()->GetWindow(),
							selectionFileNames,
							selectionFilterIndex,
							IDialogService::FileDialogOpen,
							L"Open a text document",
							L"",
							L"",
							L".txt",
							L"Text Files(*.txt)|*.txt|All Files(*.*)|*.*",
							(IDialogService::FileDialogOptions)
							(IDialogService::FileDialogAddToRecent
							| IDialogService::FileDialogDereferenceLinks
							| IDialogService::FileDialogDirectoryMustExist
							| IDialogService::FileDialogFileMustExist
							)
							))
						{
							GetStorage()->GetController()->GetDialogService()->ShowMessageBox(sender->GetRelatedGraphicsHost()->GetWindow(), selectionFileNames[0], L"hello");
						}
					}
				});
			}

			cell->AddChild(label->GetBoundsComposition());
		}
		{
			RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
			cell->SetSite(2, 0, 1, 2);

			table->AddChild(cell);

			RefPtr<Direct2DElement> element = Direct2DElement::Create();
			element->BeforeRenderTargetChanged.AttachLambda([&](PassRefPtr<Composition> sender, Direct2DElementEventArgs& args){
				if (GetWindow())
				{
					Font defaultFont;
					defaultFont.size = 16;
					defaultFont.fontFamily = _T("Microsoft Yahei");
					GetStorage()->GetDirect2DResourceManager()->DestroyDirect2DTextFormat(defaultFont);

					args.renderTarget->DestroyDirect2DBrush(Gdiplus::Color::Gray);
					args.renderTarget->DestroyDirect2DBrush(Gdiplus::Color::Blue);
				}
			});
			element->AfterRenderTargetChanged.AttachLambda([&](PassRefPtr<Composition> sender, Direct2DElementEventArgs& args){
				if (GetWindow())
				{
					Font defaultFont;
					defaultFont.size = 16;
					defaultFont.fontFamily = _T("Microsoft Yahei");
					d2dTextFormat = GetStorage()->GetDirect2DResourceManager()->CreateDirect2DTextFormat(defaultFont);
					d2dTextFormat->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
					d2dTextFormat->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

					brush1 = args.renderTarget->CreateDirect2DBrush(Gdiplus::Color::Gray);
					brush2 = args.renderTarget->CreateDirect2DBrush(Gdiplus::Color::Blue);
				}
			});
			element->Rendering.AttachLambda([&](PassRefPtr<Composition> sender, Direct2DElementEventArgs& args){
				if (GetWindow())
				{
					auto rect = D2D1::RectF(
						(FLOAT)args.bounds.left,
						(FLOAT)args.bounds.top,
						(FLOAT)args.bounds.right,
						(FLOAT)args.bounds.bottom);
					args.renderTargetD2D->FillRectangle(rect, brush1);
					
					{
						wstringstream ss;
						auto t = system_clock::to_time_t(chrono::system_clock::now());
						tm m;
						localtime_s(&m, &t);
						ss << put_time(&m, _T("%Y/%m/%d %H:%M:%S"));
						auto time = ss.str();
						args.renderTargetD2D->DrawText(time.c_str(), time.size(), d2dTextFormat->textFormat, rect, brush2);
					}
				}
			});

			RefPtr<BoundsComposition> composition = adoptRef(new BoundsComposition);
			composition->SetAlignmentToParent(CRect(1, 1, 1, 1));
			composition->SetOwnedElement(element);
			element->BeforeRenderTargetChanged.SetAssociatedComposition(composition);
			element->AfterRenderTargetChanged.SetAssociatedComposition(composition);
			element->Rendering.SetAssociatedComposition(composition);

			cell->AddChild(composition);
		}
		{
			RefPtr<TableCellComposition> tableCell = adoptRef(new TableCellComposition);
			tableCell->SetSite(0, 2, 3, 1);

			table->AddChild(tableCell);

			RefPtr<TableComposition> tableButton = adoptRef(new TableComposition);
			{
				tableButton->SetRowsAndColumns(2, 2);
				tableButton->SetRowOption(0, CellOption::PercentageOption(0.5));
				tableButton->SetRowOption(1, CellOption::PercentageOption(0.5));
				tableButton->SetColumnOption(0, CellOption::PercentageOption(0.5));
				tableButton->SetColumnOption(1, CellOption::PercentageOption(0.5));
				tableButton->SetAlignmentToParent(CRect(4, 4, 4, 4));
				tableButton->SetCellPadding(6);
				tableCell->AddChild(tableButton);
			}
			{
				{
					RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
					tableButton->AddChild(cell);
					cell->SetSite(0, 0, 1, 2);

					RefPtr<MultilineTextBox> multiTextBox = cc::global::ControlFactory::NewMultilineTextBox();
					multiText = multiTextBox;
					multiTextBox->SetText(L"Target\nhahaha\n---\n你好！！");
					multiTextBox->SetFont(font);
					multiTextBox->GetBoundsComposition()->SetAlignmentToParent(CRect(1, 1, 1, 1));
					cell->AddChild(multiTextBox->GetBoundsComposition());
				}
				{
					RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
					tableButton->AddChild(cell);
					cell->SetSite(1, 0, 1, 1);

					RefPtr<Button> buttonEnable = cc::global::ControlFactory::NewButton();
					buttonEnable->SetText(L"Enable");
					buttonEnable->SetFont(font);
					buttonEnable->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					buttonEnable->GetBoundsComposition()->SetAlignmentToParent(CRect());
					buttonEnable->GetEventReceiver()->leftButtonDown.AttachMethod(this, &TestWindow::GetText1);
					{
						RefPtr<Label> textDisplay = cc::global::ControlFactory::NewLabel();
						textDisplay->SetText(L"hello!");
						textDisplay->SetFont(font);
						buttonEnable->SetTooltipControl(textDisplay);
					};
					cell->AddChild(buttonEnable->GetBoundsComposition());
				}
				{
					RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
					tableButton->AddChild(cell);
					cell->SetSite(1, 1, 1, 1);

					RefPtr<Button> buttonDisable = cc::global::ControlFactory::NewButton();
					buttonDisable->SetText(L"Disable");
					buttonDisable->SetFont(font);
					buttonDisable->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					buttonDisable->GetBoundsComposition()->SetAlignmentToParent(CRect());
					buttonDisable->GetEventReceiver()->leftButtonDown.AttachMethod(this, &TestWindow::GetText2);
					{
						RefPtr<Label> textDisplay = cc::global::ControlFactory::NewLabel();
						textDisplay->SetText(L"hello!");
						buttonDisable->SetTooltipControl(textDisplay);
					};
					cell->AddChild(buttonDisable->GetBoundsComposition());
				}
			}
		}
		{
			RefPtr<TableCellComposition> tableCell = adoptRef(new TableCellComposition);
			tableCell->SetSite(3, 0, 1, 3);

			table->AddChild(tableCell);

			RefPtr<TableComposition> tableButton = adoptRef(new TableComposition);
			{
				tableButton->SetRowsAndColumns(1, 3);
				tableButton->SetRowOption(0, CellOption::PercentageOption(1));
				tableButton->SetColumnOption(0, CellOption::PercentageOption(0.3));
				tableButton->SetColumnOption(1, CellOption::PercentageOption(0.3));
				tableButton->SetColumnOption(2, CellOption::PercentageOption(0.3));
				tableButton->SetAlignmentToParent(CRect(4, 4, 4, 4));
				tableButton->SetCellPadding(6);
				tableCell->AddChild(tableButton);
			}
			{
				{
					RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
					tableButton->AddChild(cell);
					cell->SetSite(0, 0, 1, 1);

					RefPtr<Control> groupBox = cc::global::ControlFactory::NewGroupBox();
					groupBox->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					groupBox->GetContainerComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					groupBox->GetContainerComposition()->SetInternalMargin(CRect(10, 10, 10, 10));
					groupBox->GetBoundsComposition()->SetAlignmentToParent(CRect());
					groupBox->SetText(L"group ck");
					groupBox->SetFont(font1);
					cell->AddChild(groupBox->GetBoundsComposition());

					RefPtr<StackComposition> stack = adoptRef(new StackComposition);
					stack->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					stack->SetDirection(StackComposition::Vertical);
					stack->SetAlignmentToParent(CRect());
					stack->SetPadding(6);
					groupBox->GetContainerComposition()->AddChild(stack);

					CString name;
					for (int i = 0; i < 3; i++)
					{
						RefPtr<SelectableButton> button = cc::global::ControlFactory::NewCheckBox();
						name.Format(L"Option %d", i + 1);
						button->SetText(name);
						button->SetFont(font1);
						button->GetBoundsComposition()->SetAlignmentToParent(CRect());

						RefPtr<StackItemComposition> stackItem = adoptRef(new StackItemComposition);
						stack->AddChild(stackItem);
						stackItem->AddChild(button->GetBoundsComposition());
					}
				}
				{
					RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
					tableButton->AddChild(cell);
					cell->SetSite(0, 1, 1, 1);

					RefPtr<Control> groupBox = cc::global::ControlFactory::NewGroupBox();
					groupBox->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					groupBox->GetContainerComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					groupBox->GetContainerComposition()->SetInternalMargin(CRect(10, 10, 10, 10));
					groupBox->GetBoundsComposition()->SetAlignmentToParent(CRect());
					groupBox->SetText(L"group rd");
					groupBox->SetFont(font1);
					cell->AddChild(groupBox->GetBoundsComposition());

					RefPtr<StackComposition> stack = adoptRef(new StackComposition);
					stack->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					stack->SetDirection(StackComposition::Vertical);
					stack->SetAlignmentToParent(CRect());
					stack->SetPadding(6);
					groupBox->GetContainerComposition()->AddChild(stack);

					RefPtr<GroupController> groupController = adoptRef(new MutexGroupController);
					AddComponent(groupController);

					CString name;
					for (int i = 0; i < 3; i++)
					{
						RefPtr<SelectableButton> button = cc::global::ControlFactory::NewRadioButton();
						name.Format(L"Option %d", i + 1);
						button->SetText(name);
						button->SetFont(font1);
						button->SetGroupController(groupController);
						button->GetBoundsComposition()->SetAlignmentToParent(CRect());

						RefPtr<StackItemComposition> stackItem = adoptRef(new StackItemComposition);
						stack->AddChild(stackItem);
						stackItem->AddChild(button->GetBoundsComposition());
					}
				}
				{
					RefPtr<TableCellComposition> cell = adoptRef(new TableCellComposition);
					tableButton->AddChild(cell);
					cell->SetSite(0, 2, 1, 1);

					RefPtr<SinglelineTextBox> textBox = cc::global::ControlFactory::NewSinglelineTextBox();
					singleText = textBox;
					textBox->SetText(_T("Disable"));
					textBox->SetFont(font);
					textBox->GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
					textBox->GetBoundsComposition()->SetAlignmentToParent(CRect());
					cell->AddChild(textBox->GetBoundsComposition());
				}
			}
		}

		ForceCalculateSizeImmediately();
		MoveToScreenCenter();
	}

protected:
	void GetText1(PassRefPtr<Composition> sender, MouseEventArgs& args)
	{
		labelEx->SetText(multiText->GetText());
	}

	void GetText2(PassRefPtr<Composition> sender, MouseEventArgs& args)
	{
		multiText->SetReadonly(!multiText->GetReadonly());
	}

	void ShowPos(PassRefPtr<Composition> sender, MouseEventArgs& args)
	{
		CString str;
		str.Format(L"%s", args.pt.ToString());
		labelPos->SetText(str);
	}

private:
	RawPtr<Label>					labelEx;
	RawPtr<Label>					labelPos;
	RawPtr<SinglelineTextBox>		singleText;
	RawPtr<MultilineTextBox>		multiText;

	RefPtr<Tab>						tabControl;
	RefPtr<TabPage>					mainTabPage;
	RefPtr<TabPage>					secondTabPage;

	RefPtr<D2DTextFormatPackage>	d2dTextFormat;
	CComPtr<ID2D1Brush>				brush1;
	CComPtr<ID2D1Brush>				brush2;

	RefPtr<ToolstripMenuBar>		menuBar;
	RefPtr<ToolstripToolBar>		toolBar;
	RefPtr<MultilineTextBox>		textBox;
	RefPtr<ShortcutKeyManager>		shortcutKeyManager;

	RefPtr<MultilineTextBox>		input;

	vector<RefPtr<ToolstripCommand>> commands;
};

void SetupD2D()
{
	auto * pStorage = GetStorage();
	auto instance = pStorage->GetInstance();
	{
		RefPtr<IController> controller = adoptRef(new Controller(instance));
		RefPtr<IControllerListener> controllerListener = adoptRef(new D2DControllerListener);
		pStorage->SetDirect2DProvider(adoptRef(new D2DProvider()));
		pStorage->Init(controller, controllerListener, controllerListener);
		RefPtr<Application> application = adoptRef(new Application);
		RefPtr<ITheme> theme = adoptRef(new Win8Style);
		pStorage->Init2(application, theme);
	}
	{
		auto callbackService = pStorage->GetController()->GetCallbackService();
		RefPtr<IControllerListener> listener = pStorage->GetControllerListener();
		callbackService->InstallListener(listener);
		{
			RefPtr<D2DResourceManager> resourseManager = adoptRef(new D2DResourceManager);
			pStorage->SetGraphicsResourceManager(resourseManager);
			pStorage->SetDirect2DResourceManager(resourseManager);
			callbackService->InstallListener(resourseManager);
			pStorage->GetController()->GetInputService()->StartTimer();
			{
				SolidBorderElementRenderer::Register();
				RoundBorderElementRenderer::Register();
				Splitter3DElementRenderer::Register();
				SolidBackgroundElementRenderer::Register();
				GradientBackgroundElementRenderer::Register();
				Border3DElementRenderer::Register();
				SolidLabelElementRenderer::Register();
				ImageFrameElementRenderer::Register();
				PolygonElementRenderer::Register();
				ColorizedTextElementRenderer::Register();
				Direct2DElementRenderer::Register();
			}
			{				
				pStorage->GetApplication()->Run(adoptRef(new TestWindow));
			}
			pStorage->SetDirect2DResourceManager(nullptr);
			pStorage->SetGraphicsResourceManager(nullptr);
		}
		callbackService->UninstallListener(pStorage->GetApplication());
		callbackService->UninstallListener(listener);
	}
	pStorage->Destroy();
}
