#ifndef CC_CONSOLE
#define CC_CONSOLE

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_control.h"
#include "cc_element.h"
#include "cc_comctl.h"
#include "cc_textbox.h"

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
			class Console : public MultilineTextBox
			{
			public:
				Console(PassRefPtr<IScrollViewStyleProvider> _styleController);
				~Console();

				virtual void		SetReadonly(bool value);

				bool				CanCut()override;
				bool				CanPaste()override;
				bool				CanUndo()override;
				bool				CanRedo()override;

				void				SetMode(bool input);
				void				ClearLastLine();
				void				AsyncExecute(const function<void(Console*)>& func);

			protected:
				class ConsoleThread
				{
				public:
					ConsoleThread();
					~ConsoleThread();

					void				Input(CString& text);
					void				Input(_tstring& text);
					void				Output(LPCTSTR text);
					void				Output(const CString& text);
					void				Output(const _tstring& text);
					void				SetColor(const CColor& color);
					void				SetColorToDefault();
					void				ClearLastLine();

					void				ConsoleThreadCallback();
					void				Exit();

					void				Wait();
					void				Notify();

					semaphore			semIO;
					CString				bufferIO;
					thread				thrIO;
					RawPtr<Console>		console;
					bool				interrupt;
				};

			protected:
				void				Init();
				void				OnBeforeCharNotify(PassRefPtr<Composition> sender, CharEventArgs& arguments);
				void				OnAfterCharNotify(PassRefPtr<Composition> sender, CharEventArgs& arguments);
				bool				CanModify(TextPos& start, TextPos& end, const CString& input, bool asKeyInput)override;

			protected:
				bool				inputMode;
				TextPos				inputStart;
				ConsoleThread		consoleThread;
			};

			class Win8ConsoleBackground : public Object
			{
			public:
				Win8ConsoleBackground();
				~Win8ConsoleBackground();

				void										AssociateStyleController(PassRefPtr<IControlStyleController> controller);
				void										SetFocusableComposition(PassRefPtr<Composition> value);
				void										SetVisuallyEnabled(bool value);
				virtual PassRefPtr<Composition>				InstallBackground(PassRefPtr<BoundsComposition> boundsComposition);
				void										InitializeTextElement(PassRefPtr<ColorizedTextElement> _textElement);

			protected:
				RefPtr<SolidBorderElement>					borderElement;
				RefPtr<SolidBackgroundElement>				backgroundElement;
				RawPtr<Composition>							focusableComposition;
				bool										isVisuallyEnabled;
				RawPtr<IControlStyleController>				styleController;
				RefPtr<ColorizedTextElement>				textElement;
			};

			namespace style
			{
				class Win8ConsoleProvider : public Win8ScrollViewProvider
				{
				public:
					Win8ConsoleProvider();
					~Win8ConsoleProvider();

					void										AssociateStyleController(PassRefPtr<IControlStyleController> controller)override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetVisuallyEnabled(bool value)override;
					PassRefPtr<Composition>						InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)override;

				protected:
					Win8ConsoleBackground						background;
					RawPtr<IControlStyleController>				styleController;
				};
			}
		}
	}
}

#endif