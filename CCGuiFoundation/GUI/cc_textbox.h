#ifndef CC_TEXTBOX
#define CC_TEXTBOX

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_thread.h"
#include "cc_control.h"
#include "cc_element.h"
#include "cc_scroll.h"

using namespace cc::base;
using namespace cc::threading;
using namespace cc::interfaces::windows;
using namespace cc::presentation::control;
using namespace cc::presentation::element;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			class TextBoxBase;
			class IGeneralEditStep;
			class GeneralUndoRedoProcessor;
			class TextBoxEditStep;
			class TextBoxUndoRedoProcessor;
			class SinglelineTextBox;
			class MultilineTextBox;

			namespace style
			{
				class ISinglelineTextBoxStyleProvider;
				class SinglelineTextBoxStyleController;
				class MultilineTextBoxStyleController;

				struct Win8TextBoxColors
				{
					CColor										borderColor;
					CColor										backgroundColor;

					bool operator==(const Win8TextBoxColors& colors);
					bool operator!=(const Win8TextBoxColors& colors);

					static Win8TextBoxColors					Blend(const Win8TextBoxColors& c1, const Win8TextBoxColors& c2, cint ratio, cint total);
					static Win8TextBoxColors					Normal();
					static Win8TextBoxColors					Active();
					static Win8TextBoxColors					Focused();
					static Win8TextBoxColors					Disabled();
				};
			}

			namespace callback
			{
				class TextBoxBaseCallback : public ITextBoxBaseCallback
				{
				public:
					TextBoxBaseCallback(PassRefPtr<ColorizedTextElement> _textElement, PassRefPtr<Composition> _textComposition);
					~TextBoxBaseCallback();

					TextPos											GetLeftWord(TextPos pos)override;
					TextPos											GetRightWord(TextPos pos)override;
					void											GetWord(TextPos pos, TextPos& begin, TextPos& end)override;
					cint											GetPageRows()override;
					bool											BeforeModify(TextPos start, TextPos end, const CString& originalText, CString& inputText)override;
				
				protected:
					RawPtr<ColorizedTextElement>					textElement;
					RawPtr<Composition>								textComposition;
					bool											readonly;
				};

				class SinglelineTextBoxElementOperatorCallback : public TextBoxBaseCallback
				{
				public:
					SinglelineTextBoxElementOperatorCallback(PassRefPtr<SinglelineTextBox> _textControl);

					void											AfterModify(TextPos originalStart, TextPos originalEnd, const CString& originalText, TextPos inputStart, TextPos inputEnd, const CString& inputText)override;
					bool											BeforeModify(TextPos start, TextPos end, const CString& originalText, CString& inputText)override;
					void											ScrollToView(CPoint point)override;
					cint											GetTextMargin()override;

				protected:
					RawPtr<SinglelineTextBox>						textControl;
					RawPtr<SinglelineTextBoxStyleController>		textController;
				};

				class MultilineTextBoxElementOperatorCallback : public TextBoxBaseCallback
				{
				public:
					MultilineTextBoxElementOperatorCallback(PassRefPtr<MultilineTextBox> _textControl);

					void											AfterModify(TextPos originalStart, TextPos originalEnd, const CString& originalText, TextPos inputStart, TextPos inputEnd, const CString& inputText)override;
					void											ScrollToView(CPoint point)override;
					cint											GetTextMargin()override;

				protected:
					RawPtr<MultilineTextBox>						textControl;
					RawPtr<MultilineTextBoxStyleController>			textController;
				};

				class ITextEditCallback : public Interface
				{
				public:
					virtual void									Attach(PassRefPtr<ColorizedTextElement> element, spin_mutex& elementModifyLock, PassRefPtr<Composition> ownerComposition, cuint editVersion) = 0;
					virtual void									Detach() = 0;
					virtual void									TextEditPreview(TextEditPreviewStruct& arguments) = 0;
					virtual void									TextEditNotify(const TextEditNotifyStruct& arguments) = 0;
					virtual void									TextCaretChanged(const TextCaretChangedStruct& arguments) = 0;
					virtual void									TextEditFinished(cuint editVersion) = 0;
				};
			}

			using namespace callback;

			class TextBoxBase : virtual public Object
			{
			public:
				TextBoxBase();
				~TextBoxBase();

				NotifyEvent											SelectionChanged;
				CharEvent											AfterCharNotify;
				CharEvent											BeforeCharNotify;

				virtual bool										CanCut();
				virtual bool										CanCopy();
				virtual bool										CanPaste();
				bool												Cut();
				bool												Copy();
				bool												Paste();

				bool												GetReadonly();
				virtual void										SetReadonly(bool value);

				void												SelectAll();
				void												Select(TextPos begin, TextPos end);
				CString												GetSelectionText();
				void												SetSelectionText(const CString& value);
				void												SetSelectionTextAsKeyInput(const CString& value);

				CString												GetRowText(cint row);
				CString												GetFragmentText(TextPos start, TextPos end);

				TextPos												GetCaretBegin();
				TextPos												GetCaretEnd();
				TextPos												GetCaretSmall();
				TextPos												GetCaretLarge();

				cint												GetRowWidth(cint row);
				cint												GetRowHeight();
				cint												GetMaxWidth();
				cint												GetMaxHeight();
				TextPos												GetTextPosFromPoint(CPoint point);
				CPoint												GetPointFromTextPos(TextPos pos);
				CRect												GetRectFromTextPos(TextPos pos);
				TextPos												GetNearestTextPos(CPoint point);

				cuint												GetEditVersion();
				virtual bool										CanUndo();
				virtual bool										CanRedo();
				void												ClearUndoRedo();
				bool												GetModified();
				void												NotifyModificationSaved();
				bool												Undo();
				bool												Redo();

				bool												SetTextColor(CColor color);
				void												SetTextColorToDefault();
				cint												GetTextColorIndex(CColor color);

			protected:
				void												Install(PassRefPtr<ColorizedTextElement> _textElement, PassRefPtr<Composition> _textComposition, PassRefPtr<Control> _textControl);
				PassRefPtr<ITextBoxBaseCallback>					GetCallback();
				void												SetCallback(PassRefPtr<ITextBoxBaseCallback> value);
				bool												AttachTextEditCallback(PassRefPtr<ITextEditCallback> value);
				bool												DetachTextEditCallback(PassRefPtr<ITextEditCallback> value);
				void												AddShortcutCommand(cint key, const function<void()>& eventHandler);
				PassRefPtr<ColorizedTextElement>					GetTextElement();
				void												UnsafeSetText(const CString& value);

			protected:
				void												UpdateCaretPoint();
				void												Move(TextPos pos, bool shift);
				virtual bool										CanModify(TextPos& start, TextPos& end, const CString& input, bool asKeyInput);
				void												Modify(TextPos start, TextPos end, const CString& input, bool asKeyInputs);
				virtual bool										ProcessKey(cint code, bool shift, bool ctrl);

				void												OnGotFocus(PassRefPtr<Composition> sender, EventArgs& arguments);
				void												OnLostFocus(PassRefPtr<Composition> sender, EventArgs& arguments);
				void												OnCaretNotify(PassRefPtr<Composition> sender, EventArgs& arguments);

				void												OnLeftButtonDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments);
				void												OnLeftButtonUp(PassRefPtr<Composition> sender, MouseEventArgs& arguments);
				void												OnMouseMove(PassRefPtr<Composition> sender, MouseEventArgs& arguments);
				void												OnKeyDown(PassRefPtr<Composition> sender, KeyEventArgs& arguments);
				void												OnCharInput(PassRefPtr<Composition> sender, CharEventArgs& arguments);

			protected:
				RefPtr<ColorizedTextElement>						textElement;
				RefPtr<Composition>									textComposition;
				cuint												editVersion;
				RawPtr<Control>										textControl;
				RefPtr<ITextBoxBaseCallback>						callback;
				bool												dragging;
				bool												readonly;
				RefPtr<TextBoxUndoRedoProcessor>					undoRedoProcessor;

				spin_mutex											elementModifyLock;
				set<RefPtr<ITextEditCallback>>						textEditCallbacks;
				RefPtr<ShortcutKeyManager>							internalShortcutKeyManager;

				cint												colorIndex;
				map<CColor, cint>									colorIndexes;
			};

			class IGeneralEditStep : public Interface
			{
			public:
				virtual void								Undo() = 0;
				virtual void								Redo() = 0;
			};

			class GeneralUndoRedoProcessor
			{
			public:
				GeneralUndoRedoProcessor();
				~GeneralUndoRedoProcessor();

				bool										CanUndo();
				bool										CanRedo();
				void										ClearUndoRedo();
				bool										GetModified();
				void										NotifyModificationSaved();
				bool										Undo();
				bool										Redo();

			protected:
				void										PushStep(PassRefPtr<IGeneralEditStep> step);

				vector<RefPtr<IGeneralEditStep>>			steps;
				cint										firstFutureStep;
				cint										savedStep;
				bool										performingUndoRedo;
			};

			class TextBoxEditStep : public IGeneralEditStep
			{
			public:
				void										Undo();
				void										Redo();

				RawPtr<TextBoxUndoRedoProcessor>			processor;
				TextEditNotifyStruct						arguments;
			};

			class TextBoxUndoRedoProcessor : public GeneralUndoRedoProcessor, public ITextEditCallback
			{
				friend class TextBoxEditStep;
			public:
				TextBoxUndoRedoProcessor();
				~TextBoxUndoRedoProcessor();

				void										Attach(PassRefPtr<ColorizedTextElement> element, spin_mutex& elementModifyLock, PassRefPtr<Composition> ownerComposition, cuint editVersion)override;
				void										Detach()override;
				void										TextEditPreview(TextEditPreviewStruct& arguments)override;
				void										TextEditNotify(const TextEditNotifyStruct& arguments)override;
				void										TextCaretChanged(const TextCaretChangedStruct& arguments)override;
				void										TextEditFinished(cuint editVersion)override;
			
			protected:
				RawPtr<Composition>							ownerComposition;
			};

			namespace style
			{
				class ISinglelineTextBoxStyleProvider : public IControlStyleProvider
				{
				public:
					virtual PassRefPtr<Composition>			InstallBackground(PassRefPtr<BoundsComposition> background) = 0;
				};

				class SinglelineTextBoxStyleController : public IControlStyleController
				{
				public:
					SinglelineTextBoxStyleController(PassRefPtr<ISinglelineTextBoxStyleProvider> _styleProvider);
					~SinglelineTextBoxStyleController();

					void									Initialize(PassRefPtr<SinglelineTextBox> control);
					void									RearrangeTextElement();
					PassRefPtr<BoundsComposition>			GetBoundsComposition();
					PassRefPtr<Composition>					GetContainerComposition();
					void									SetFocusableComposition(PassRefPtr<Composition> value);

					CString									GetText();
					void									SetText(const CString& value);
					void									SetFont(const Font& value);
					void									SetVisuallyEnabled(bool value);

					PassRefPtr<ColorizedTextElement>		GetTextElement();
					PassRefPtr<Composition>					GetTextComposition();
					void									SetViewPosition(CPoint value);

				protected:
					RefPtr<ISinglelineTextBoxStyleProvider>	styleProvider;
					RefPtr<BoundsComposition>				boundsComposition;
					RefPtr<Composition>						containerComposition;

					RawPtr<SinglelineTextBox>				textBox;
					RefPtr<ColorizedTextElement>			textElement;
					RefPtr<TableComposition>				textCompositionTable;
					RefPtr<TableCellComposition>			textComposition;
					RefPtr<ITextBoxBaseCallback>			defaultCallback;
				};

				class MultilineTextBoxStyleController : public ScrollViewStyleController
				{
				public:
					MultilineTextBoxStyleController(PassRefPtr<IScrollViewStyleProvider> _styleProvider);
					~MultilineTextBoxStyleController();

					void									Initialize(PassRefPtr<MultilineTextBox> control);
					void									SetFocusableComposition(PassRefPtr<Composition> value);

					CString									GetText();
					void									SetText(const CString& value);
					void									SetFont(const Font& value);
					void									SetVisuallyEnabled(bool value);

					PassRefPtr<ColorizedTextElement>		GetTextElement();
					PassRefPtr<Composition>					GetTextComposition();
					void									SetViewPosition(CPoint value);

				protected:
					RawPtr<MultilineTextBox>				textBox;
					RefPtr<ColorizedTextElement>			textElement;
					RefPtr<BoundsComposition>				textComposition;
					RefPtr<ITextBoxBaseCallback>			defaultCallback;
				};
			}

			class SinglelineTextBox : public Control, public TextBoxBase
			{
				friend class SinglelineTextBoxStyleController;
			public:
				static const cint							TextMargin = 3;

			public:
				SinglelineTextBox(PassRefPtr<ISinglelineTextBoxStyleProvider> styleProvider);
				~SinglelineTextBox();

				const CString&								GetText()override;
				void										SetText(const CString& value)override;
				void										SetFont(const Font& value)override;
				TCHAR										GetPasswordChar();
				void										SetPasswordChar(TCHAR value);

			protected:
				void										OnRenderTargetChanged(PassRefPtr<IGraphicsRenderTarget> renderTarget)override;
				void										OnBoundsMouseButtonDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments);

				RefPtr<SinglelineTextBoxStyleController>	styleController;
			};

			class MultilineTextBox : public ScrollView, public TextBoxBase
			{
				friend class MultilineTextBoxStyleController;
			public:
				static const cint							TextMargin = 3;

			public:
				MultilineTextBox(PassRefPtr<IScrollViewStyleProvider> styleProvider);
				~MultilineTextBox();

				const CString&								GetText()override;
				void										SetText(const CString& value)override;
				void										SetFont(const Font& value)override;

			protected:
				void										CalculateViewAndSetScroll();
				CSize										QueryFullSize()override;
				void										UpdateView(CRect viewBounds)override;
				void										OnRenderTargetChanged(PassRefPtr<IGraphicsRenderTarget> renderTarget)override;
				void										OnBoundsMouseButtonDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments);

				RefPtr<MultilineTextBoxStyleController>		styleController;
			};

			class Win8TextBoxBackground : public Object
			{
				friend class TransferringAnimationFocusableHostGetter<Win8TextBoxBackground>;
			public:
				Win8TextBoxBackground();
				~Win8TextBoxBackground();

				void										AssociateStyleController(PassRefPtr<IControlStyleController> controller);
				void										SetFocusableComposition(PassRefPtr<Composition> value);
				void										SetVisuallyEnabled(bool value);
				virtual PassRefPtr<Composition>				InstallBackground(PassRefPtr<BoundsComposition> boundsComposition);
				void										InitializeTextElement(PassRefPtr<ColorizedTextElement> _textElement);

				void										ToAnimation(PassRefPtr<GraphicsAnimationManager> manager);

			protected:
				class TransferringAnimationType : public TransferringAnimationFocusable<Win8TextBoxColors, Win8TextBoxBackground>
				{
				public:
					TransferringAnimationType(Win8TextBoxBackground* _style, const Win8TextBoxColors& begin);
				protected:
					void									PlayInternal(cint currentPosition, cint totalLength)override;
				};

				void										UpdateStyle();
				void										Apply(const Win8TextBoxColors& colors);

				void										OnBoundsMouseEnter(PassRefPtr<Composition> sender, EventArgs& arguments);
				void										OnBoundsMouseLeave(PassRefPtr<Composition> sender, EventArgs& arguments);
				void										OnBoundsGotFocus(PassRefPtr<Composition> sender, EventArgs& arguments);
				void										OnBoundsLostFocus(PassRefPtr<Composition> sender, EventArgs& arguments);

				RefPtr<SolidBorderElement>					borderElement;
				RefPtr<SolidBackgroundElement>				backgroundElement;
				RawPtr<Composition>							focusableComposition;
				bool										isMouseEnter;
				bool										isFocused;
				bool										isVisuallyEnabled;
				RefPtr<TransferringAnimationType>			transferringAnimation;
				RawPtr<IControlStyleController>				styleController;
				RefPtr<ColorizedTextElement>				textElement;
			};

			namespace style
			{
				class Win8SinglelineTextBoxProvider : public ISinglelineTextBoxStyleProvider
				{
				public:
					Win8SinglelineTextBoxProvider();
					~Win8SinglelineTextBoxProvider();

					void										AssociateStyleController(PassRefPtr<IControlStyleController> controller)override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;
					PassRefPtr<Composition>						InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)override;

				protected:
					Win8TextBoxBackground						background;
					RawPtr<IControlStyleController>				styleController;
				};

				class Win8MultilineTextBoxProvider : public Win8ScrollViewProvider
				{
				public:
					Win8MultilineTextBoxProvider();
					~Win8MultilineTextBoxProvider();

					void										AssociateStyleController(PassRefPtr<IControlStyleController> controller)override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetVisuallyEnabled(bool value)override;
					PassRefPtr<Composition>						InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)override;

				protected:
					Win8TextBoxBackground						background;
					RawPtr<IControlStyleController>				styleController;
				};
			}
		}
	}
}

#endif