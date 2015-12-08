#include "stdafx.h"
#include "cc_comctl.h"
#include "cc_console.h"
#include "cc_presentation.h"
#include "cc_exception.h"

using namespace cc::exception;
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
				PassRefPtr<IScrollViewStyleProvider> Win8Style::CreateConsoleStyle()
				{
					return adoptRef(new Win8ConsoleProvider);
				}
			}

			Console::Console(PassRefPtr<IScrollViewStyleProvider> _styleController)
				: MultilineTextBox(_styleController)
				, inputMode(false)
				, inputStart(0, 0)
			{
				Init();
			}

			Console::~Console()
			{

			}

			void Console::Init()
			{
				readonly = true;
				Font font;
				font.fontFamily = _T("ËÎÌו");
				font.size = 16;
				SetFont(font);
				consoleThread.console = this;
				consoleThread.Notify();

				BeforeCharNotify.AttachMethod(this, &Console::OnBeforeCharNotify);
				AfterCharNotify.AttachMethod(this, &Console::OnAfterCharNotify);
			}

			void Console::SetReadonly(bool value)
			{

			}

			bool Console::CanCut()
			{
				if (inputMode && GetCaretBegin() >= inputStart)
					return TextBoxBase::CanCut();
				return false;
			}

			bool Console::CanPaste()
			{
				if (inputMode && GetCaretBegin() >= inputStart)
				{
					return TextBoxBase::CanPaste();
				}
				return false;
			}

			bool Console::CanUndo()
			{
				if (inputMode)
					return TextBoxBase::CanUndo();
				return false;
			}

			bool Console::CanRedo()
			{
				if (inputMode)
					return TextBoxBase::CanRedo();
				return false;
			}

			void Console::SetMode(bool input)
			{
				ClearUndoRedo();
				TextPos end = textElement->GetLines().GetEndPos();
				if (input)
				{
					inputStart = end;
					inputMode = input;
					readonly = !input;
				}
				else
				{
					inputMode = input;
					readonly = !input;
					Modify(end, end, consoleThread.bufferIO, true);
					consoleThread.Notify();
				}
			}

			void Console::AsyncExecute(const function<void(Console*)>& func)
			{
				GetStorage()->GetApplication()->InvokeInMainThread(bind(func, this));
			}

			void Console::OnBeforeCharNotify(PassRefPtr<Composition> sender, CharEventArgs& arguments)
			{
				if (arguments.code == VK_RETURN && !arguments.ctrl && !arguments.shift && !arguments.alt)
				{
					consoleThread.bufferIO = textElement->GetLines().GetText(inputStart, textElement->GetLines().GetEndPos());
				}
			}

			void Console::OnAfterCharNotify(PassRefPtr<Composition> sender, CharEventArgs& arguments)
			{
				if (arguments.code == VK_RETURN && !arguments.ctrl && !arguments.shift && !arguments.alt)
				{
					consoleThread.Notify();
				}
			}

			bool Console::CanModify(TextPos& start, TextPos& end, const CString& input, bool asKeyInput)
			{
				if ((start < inputStart || end < inputStart) || (asKeyInput && input == _T("\r")))
				{
					start = textElement->GetLines().GetEndPos();
					end = start;
				}
				return start >= inputStart && end >= inputStart;
			}

			void Console::ClearLastLine()
			{
				auto count = textElement->GetLines().GetCount();
				textElement->GetLines().GetLine(count - 1).Initialize();
			}

			Console::ConsoleThread::ConsoleThread()
				: semIO(0)
				, interrupt(false)
				, thrIO(bind(&ConsoleThread::ConsoleThreadCallback, this))
			{
			}

			Console::ConsoleThread::~ConsoleThread()
			{
				Exit();
			}

			void Console::ConsoleThread::Input(CString& text)
			{
				console->AsyncExecute([=](Console* _this)
				{
					_this->SetMode(true);
				});
				Wait();
				text = bufferIO;
			}

			void Console::ConsoleThread::Input(_tstring& text)
			{
				console->AsyncExecute([=](Console* _this)
				{
					_this->SetMode(true);
				});
				Wait();
				text = bufferIO;
			}

			void Console::ConsoleThread::Output(LPCTSTR text)
			{
				bufferIO = text;
				console->AsyncExecute([=](Console* _this)
				{
					_this->SetMode(false);
				});
				Wait();
			}

			void Console::ConsoleThread::Output(const CString& text)
			{
				bufferIO = text;
				console->AsyncExecute([=](Console* _this)
				{
					_this->SetMode(false);
				});
				Wait();
			}

			void Console::ConsoleThread::Output(const _tstring& text)
			{
				bufferIO = text.c_str();
				console->AsyncExecute([=](Console* _this)
				{
					_this->SetMode(false);
				});
				Wait();
			}

			void Console::ConsoleThread::SetColor(const CColor& color)
			{
				console->AsyncExecute([=](Console* _this)
				{
					_this->SetTextColor(color);
					Notify();
				});
				Wait();
			}

			void Console::ConsoleThread::SetColorToDefault()
			{
				console->AsyncExecute([=](Console* _this)
				{
					_this->SetTextColorToDefault();
					Notify();
				});
				Wait();
			}

			void Console::ConsoleThread::ClearLastLine()
			{
				console->AsyncExecute([=](Console* _this)
				{
					_this->ClearLastLine();
					Notify();
				});
				Wait();
			}

			void Console::ConsoleThread::Exit()
			{
				interrupt = true;
				semIO.signal();
				thrIO.join();
			}

			void Console::ConsoleThread::Wait()
			{
				semIO.wait();
				if (interrupt)
					throw runtime_thread_interrupt("Console Window - host thread interruption", this_thread::get_id());
			}

			void Console::ConsoleThread::Notify()
			{
				semIO.signal();
			}

			//////////////////////////////////////////////////////////////////////////

			Win8ConsoleBackground::Win8ConsoleBackground()
			{

			}

			Win8ConsoleBackground::~Win8ConsoleBackground()
			{

			}

			void Win8ConsoleBackground::AssociateStyleController(PassRefPtr<IControlStyleController> controller)
			{
				styleController = controller;
			}

			void Win8ConsoleBackground::SetFocusableComposition(PassRefPtr<Composition> value)
			{
				focusableComposition = value;
			}

			void Win8ConsoleBackground::SetVisuallyEnabled(bool value)
			{
				isVisuallyEnabled = value;
			}

			PassRefPtr<Composition> Win8ConsoleBackground::InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)
			{
				{
					RefPtr<SolidBackgroundElement> background = SolidBackgroundElement::Create();
					background->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));

					RefPtr<BoundsComposition> backgroundComposition = adoptRef(new BoundsComposition);
					boundsComposition->AddChild(backgroundComposition);
					backgroundComposition->SetAlignmentToParent(CRect(1, 1, 1, 1));
					backgroundComposition->SetOwnedElement(background);
				}
				{
					RefPtr<SolidBackgroundElement> background = SolidBackgroundElement::Create();
					background->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window));

					RefPtr<BoundsComposition> backgroundComposition = adoptRef(new BoundsComposition);
					boundsComposition->AddChild(backgroundComposition);
					backgroundComposition->SetAlignmentToParent(CRect(2, 2, 2, 2));
					backgroundComposition->SetOwnedElement(background);
					backgroundElement = background;
					backgroundElement->SetColor(CColor(0, 0, 0));
				}
				{
					RefPtr<SolidBorderElement> border = SolidBorderElement::Create();
					border->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Border));
					borderElement = border;
					borderElement->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Border));

					RefPtr<BoundsComposition> borderComposition = adoptRef(new BoundsComposition);
					boundsComposition->AddChild(borderComposition);
					borderComposition->SetAlignmentToParent(CRect(0, 0, 0, 0));
					borderComposition->SetOwnedElement(border);
				}
				{
					RefPtr<BoundsComposition> containerComposition = adoptRef(new BoundsComposition);
					boundsComposition->AddChild(containerComposition);
					containerComposition->SetAlignmentToParent(CRect(2, 2, 2, 2));
					return containerComposition;
				}
			}

			void Win8ConsoleBackground::InitializeTextElement(PassRefPtr<ColorizedTextElement> _textElement)
			{
				{
					textElement = _textElement;

					vector<ColorEntry> colors;
					{
						ColorEntry entry;
						entry.normal.text = CColor(192, 192, 192);
						entry.normal.background = CColor(0, 0, 0);
						entry.selectedFocused.text = CColor(63, 63, 63);
						entry.selectedFocused.background = CColor(255, 255, 255);
						entry.selectedUnfocused.text = CColor(192, 192, 192);
						entry.selectedUnfocused.background = CColor(0, 0, 0);
						colors.push_back(entry);
					}
					textElement->SetColors(colors);
					textElement->SetCaretColor(CColor(192, 192, 192));
				}
			}

			namespace style
			{
				Win8ConsoleProvider::Win8ConsoleProvider()
				{

				}

				Win8ConsoleProvider::~Win8ConsoleProvider()
				{

				}

				void Win8ConsoleProvider::AssociateStyleController(PassRefPtr<IControlStyleController> controller)
				{
					styleController = controller;
					background.AssociateStyleController(controller);
				}

				void Win8ConsoleProvider::SetFocusableComposition(PassRefPtr<Composition> value)
				{
					background.SetFocusableComposition(value);
					RefPtr<MultilineTextBoxStyleController> textBoxController = dynamic_cast<MultilineTextBoxStyleController*>(~styleController);
					if (textBoxController)
					{
						background.InitializeTextElement(textBoxController->GetTextElement());
					}
				}

				void Win8ConsoleProvider::SetVisuallyEnabled(bool value)
				{
					background.SetVisuallyEnabled(value);
				}

				PassRefPtr<Composition> Win8ConsoleProvider::InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)
				{
					return background.InstallBackground(boundsComposition);
				}
			}
		}
	}
}