#include "stdafx.h"
#include "cc_comctl.h"
#include "cc_textbox.h"
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
				PassRefPtr<ISinglelineTextBoxStyleProvider> Win8Style::CreateTextBoxStyle()
				{
					return adoptRef(new Win8SinglelineTextBoxProvider);
				}

				PassRefPtr<IScrollViewStyleProvider> Win8Style::CreateMultilineTextBoxStyle()
				{
					return adoptRef(new Win8MultilineTextBoxProvider);
				}
			}

			namespace callback
			{
				TextBoxBaseCallback::TextBoxBaseCallback(PassRefPtr<ColorizedTextElement> _textElement, PassRefPtr<Composition> _textComposition)
					: textElement(_textElement)
					, textComposition(_textComposition)
				{

				}

				TextBoxBaseCallback::~TextBoxBaseCallback()
				{

				}

				TextPos TextBoxBaseCallback::GetLeftWord(TextPos pos)
				{
					return pos;
				}

				TextPos TextBoxBaseCallback::GetRightWord(TextPos pos)
				{
					return pos;
				}

				void TextBoxBaseCallback::GetWord(TextPos pos, TextPos& begin, TextPos& end)
				{
					begin = pos;
					end = pos;
				}

				cint TextBoxBaseCallback::GetPageRows()
				{
					return textComposition->GetBounds().Height() / textElement->GetLines().GetRowHeight();
				}

				bool TextBoxBaseCallback::BeforeModify(TextPos start, TextPos end, const CString& originalText, CString& inputText)
				{
					return true;
				}

				SinglelineTextBoxElementOperatorCallback::SinglelineTextBoxElementOperatorCallback(PassRefPtr<SinglelineTextBox> _textControl)
					: TextBoxBaseCallback(
					dynamic_cast<SinglelineTextBoxStyleController*>(_textControl->GetStyleController().get())->GetTextElement(),
					dynamic_cast<SinglelineTextBoxStyleController*>(_textControl->GetStyleController().get())->GetTextComposition()
					)
					, textControl(_textControl)
					, textController(dynamic_cast<SinglelineTextBoxStyleController*>(_textControl->GetStyleController().get()))
				{

				}

				void SinglelineTextBoxElementOperatorCallback::AfterModify(TextPos originalStart, TextPos originalEnd, const CString& originalText, TextPos inputStart, TextPos inputEnd, const CString& inputText)
				{

				}

				bool SinglelineTextBoxElementOperatorCallback::BeforeModify(TextPos start, TextPos end, const CString& originalText, CString& inputText)
				{
					auto length = inputText.GetLength();
					LPCTSTR input = inputText;
					for (cint i = 0; i < length; i++, input++)
					{
						if (*input == _T('\0') || *input == _T('\r') || *input == _T('\n'))
						{
							length = i;
							break;
						}
					}
					if (length != inputText.GetLength())
					{
						inputText = inputText.Left(length);
					}
					return true;
				}

				void SinglelineTextBoxElementOperatorCallback::ScrollToView(CPoint point)
				{
					cint newX = point.x;
					cint oldX = textElement->GetViewPosition().x;
					cint marginX = 0;
					if (oldX < newX)
					{
						marginX = SinglelineTextBox::TextMargin;
					}
					else if (oldX > newX)
					{
						marginX = -SinglelineTextBox::TextMargin;
					}

					newX += marginX;
					cint minX = -SinglelineTextBox::TextMargin;
					cint maxX = textElement->GetLines().GetMaxWidth() + SinglelineTextBox::TextMargin - textComposition->GetBounds().Width();
					if (newX >= maxX)
					{
						newX = maxX - 1;
					}
					if (newX < minX)
					{
						newX = minX;
					}
					textElement->SetViewPosition(CPoint(newX, -SinglelineTextBox::TextMargin));
				}

				cint SinglelineTextBoxElementOperatorCallback::GetTextMargin()
				{
					return SinglelineTextBox::TextMargin;
				}

				MultilineTextBoxElementOperatorCallback::MultilineTextBoxElementOperatorCallback(PassRefPtr<MultilineTextBox> _textControl)
					: TextBoxBaseCallback(
					dynamic_cast<MultilineTextBoxStyleController*>(_textControl->GetStyleController().get())->GetTextElement(),
					dynamic_cast<MultilineTextBoxStyleController*>(_textControl->GetStyleController().get())->GetTextComposition()
					)
					, textControl(_textControl)
					, textController(dynamic_cast<MultilineTextBoxStyleController*>(_textControl->GetStyleController().get()))
				{

				}

				void MultilineTextBoxElementOperatorCallback::AfterModify(TextPos originalStart, TextPos originalEnd, const CString& originalText, TextPos inputStart, TextPos inputEnd, const CString& inputText)
				{
					textControl->CalculateView();
				}

				void MultilineTextBoxElementOperatorCallback::ScrollToView(CPoint point)
				{
					point.x += MultilineTextBox::TextMargin;
					point.y += MultilineTextBox::TextMargin;
					CPoint oldPoint(textControl->GetHorizontalScroll()->GetPosition(), textControl->GetVerticalScroll()->GetPosition());
					cint marginX = 0;
					cint marginY = 0;
					if (oldPoint.x < point.x)
					{
						marginX = MultilineTextBox::TextMargin;
					}
					else if (oldPoint.x > point.x)
					{
						marginX = -MultilineTextBox::TextMargin;
					}
					if (oldPoint.y < point.y)
					{
						marginY = MultilineTextBox::TextMargin;
					}
					else if (oldPoint.y > point.y)
					{
						marginY = -MultilineTextBox::TextMargin;
					}
					textControl->GetHorizontalScroll()->SetPosition(point.x + marginX);
					textControl->GetVerticalScroll()->SetPosition(point.y + marginY);
				}

				cint MultilineTextBoxElementOperatorCallback::GetTextMargin()
				{
					return MultilineTextBox::TextMargin;
				}
			}

			TextBoxBase::TextBoxBase()
				: editVersion(0)
				, colorIndex(0)
				, dragging(false)
				, readonly(false)
			{
				undoRedoProcessor = adoptRef(new TextBoxUndoRedoProcessor);
				AttachTextEditCallback(undoRedoProcessor);

				internalShortcutKeyManager = adoptRef(new ShortcutKeyManager);
				AddShortcutCommand(_T('Z'), bind([=](){ bind(&TextBoxBase::Undo, this)(); }));
				AddShortcutCommand(_T('Y'), bind([=](){ bind(&TextBoxBase::Redo, this)(); }));
				AddShortcutCommand(_T('A'), bind([=](){ bind(&TextBoxBase::SelectAll, this)(); }));
				AddShortcutCommand(_T('X'), bind([=](){ bind(&TextBoxBase::Cut, this)(); }));
				AddShortcutCommand(_T('C'), bind([=](){ bind(&TextBoxBase::Copy, this)(); }));
				AddShortcutCommand(_T('V'), bind([=](){ bind(&TextBoxBase::Paste, this)(); }));

				colorIndexes.insert(make_pair(Win8WindowStyle::GetTextColor().normal.text, 0));
			}

			TextBoxBase::~TextBoxBase()
			{

			}

			bool TextBoxBase::CanCut()
			{
				return !readonly && textElement->GetCaretBegin() != textElement->GetCaretEnd() && textElement->GetPasswordChar() == _T('\0');
			}

			bool TextBoxBase::CanCopy()
			{
				return textElement->GetCaretBegin() != textElement->GetCaretEnd() && textElement->GetPasswordChar() == _T('\0');
			}

			bool TextBoxBase::CanPaste()
			{
				return !readonly && GetStorage()->GetController()->GetClipboardService()->ContainsText() && textElement->GetPasswordChar() == _T('\0');
			}

			bool TextBoxBase::Cut()
			{
				if (CanCut())
				{
					GetStorage()->GetController()->GetClipboardService()->SetText(GetSelectionText());
					SetSelectionText(_T(""));
					return true;
				}
				else
				{
					return false;
				}
			}

			bool TextBoxBase::Copy()
			{
				if (CanCopy())
				{
					GetStorage()->GetController()->GetClipboardService()->SetText(GetSelectionText());
					return true;
				}
				else
				{
					return false;
				}
			}

			bool TextBoxBase::Paste()
			{
				if (CanPaste())
				{
					SetSelectionText(GetStorage()->GetController()->GetClipboardService()->GetText());
					return true;
				}
				else
				{
					return false;
				}
			}

			bool TextBoxBase::GetReadonly()
			{
				return readonly;
			}

			void TextBoxBase::SetReadonly(bool value)
			{
				readonly = value;
			}

			void TextBoxBase::SelectAll()
			{
				cint row = textElement->GetLines().GetCount() - 1;
				Move(TextPos(0, 0), false);
				Move(TextPos(row, textElement->GetLines().GetLine(row).dataLength), true);
			}

			void TextBoxBase::Select(TextPos begin, TextPos end)
			{
				Move(begin, false);
				Move(end, true);
			}

			CString TextBoxBase::GetSelectionText()
			{
				TextPos selectionBegin = textElement->GetCaretBegin()<textElement->GetCaretEnd() ? textElement->GetCaretBegin() : textElement->GetCaretEnd();
				TextPos selectionEnd = textElement->GetCaretBegin()>textElement->GetCaretEnd() ? textElement->GetCaretBegin() : textElement->GetCaretEnd();
				return textElement->GetLines().GetText(selectionBegin, selectionEnd);
			}

			void TextBoxBase::SetSelectionText(const CString& value)
			{
				Modify(textElement->GetCaretBegin(), textElement->GetCaretEnd(), value, false);
			}

			void TextBoxBase::SetSelectionTextAsKeyInput(const CString& value)
			{
				Modify(textElement->GetCaretBegin(), textElement->GetCaretEnd(), value, true);
			}

			CString TextBoxBase::GetRowText(cint row)
			{
				TextPos start = textElement->GetLines().Normalize(TextPos(row, 0));
				TextPos end = TextPos(start.row, textElement->GetLines().GetLine(start.row).dataLength);
				return GetFragmentText(start, end);
			}

			CString TextBoxBase::GetFragmentText(TextPos start, TextPos end)
			{
				start = textElement->GetLines().Normalize(start);
				end = textElement->GetLines().Normalize(end);
				return textElement->GetLines().GetText(start, end);
			}

			TextPos TextBoxBase::GetCaretBegin()
			{
				return textElement->GetCaretBegin();
			}

			TextPos TextBoxBase::GetCaretEnd()
			{
				return textElement->GetCaretEnd();
			}

			TextPos TextBoxBase::GetCaretSmall()
			{
				TextPos c1 = GetCaretBegin();
				TextPos c2 = GetCaretEnd();
				return c1 < c2 ? c1 : c2;
			}

			TextPos TextBoxBase::GetCaretLarge()
			{
				TextPos c1 = GetCaretBegin();
				TextPos c2 = GetCaretEnd();
				return c1 > c2 ? c1 : c2;
			}

			cint TextBoxBase::GetRowWidth(cint row)
			{
				return textElement->GetLines().GetRowWidth(row);
			}

			cint TextBoxBase::GetRowHeight()
			{
				return textElement->GetLines().GetRowHeight();
			}

			cint TextBoxBase::GetMaxWidth()
			{
				return textElement->GetLines().GetMaxWidth();
			}

			cint TextBoxBase::GetMaxHeight()
			{
				return textElement->GetLines().GetMaxHeight();
			}

			TextPos TextBoxBase::GetTextPosFromPoint(CPoint point)
			{
				CPoint view = textElement->GetViewPosition();
				return textElement->GetLines().GetTextPosFromPoint(CPoint(point.x + view.x, point.y + view.y));
			}

			CPoint TextBoxBase::GetPointFromTextPos(TextPos pos)
			{
				CPoint view = textElement->GetViewPosition();
				CPoint result = textElement->GetLines().GetPointFromTextPos(pos);
				return CPoint(result.x - view.x, result.y - view.y);
			}

			CRect TextBoxBase::GetRectFromTextPos(TextPos pos)
			{
				CPoint view = textElement->GetViewPosition();
				CRect result = textElement->GetLines().GetRectFromTextPos(pos);
				return CRect(CPoint(result.left - view.x, result.top - view.y), result.Size());
			}

			TextPos TextBoxBase::GetNearestTextPos(CPoint point)
			{
				CPoint viewPosition = textElement->GetViewPosition();
				CPoint mousePosition = CPoint(point.x + viewPosition.x, point.y + viewPosition.y);
				TextPos pos = textElement->GetLines().GetTextPosFromPoint(mousePosition);
				if (pos.column < textElement->GetLines().GetLine(pos.row).dataLength)
				{
					CRect rect = textElement->GetLines().GetRectFromTextPos(pos);
					if (abs((int)(rect.left - mousePosition.x)) >= abs((int)(rect.right - 1 - mousePosition.x)))
					{
						pos.column++;
					}
				}
				return pos;
			}

			cuint TextBoxBase::GetEditVersion()
			{
				return editVersion;
			}

			bool TextBoxBase::CanUndo()
			{
				return !readonly && undoRedoProcessor->CanUndo();
			}

			bool TextBoxBase::CanRedo()
			{
				return !readonly && undoRedoProcessor->CanRedo();
			}

			void TextBoxBase::ClearUndoRedo()
			{
				undoRedoProcessor->ClearUndoRedo();
			}

			bool TextBoxBase::GetModified()
			{
				return undoRedoProcessor->GetModified();
			}

			void TextBoxBase::NotifyModificationSaved()
			{
				undoRedoProcessor->NotifyModificationSaved();
			}

			bool TextBoxBase::Undo()
			{
				if (CanUndo())
				{
					return undoRedoProcessor->Undo();
				}
				else
				{
					return false;
				}
			}

			bool TextBoxBase::Redo()
			{
				if (CanRedo())
				{
					return undoRedoProcessor->Redo();
				}
				else
				{
					return false;
				}
			}

			void TextBoxBase::Install(PassRefPtr<ColorizedTextElement> _textElement, PassRefPtr<Composition> _textComposition, PassRefPtr<Control> _textControl)
			{
				textElement = _textElement;
				textComposition = _textComposition;
				textControl = _textControl;
				textComposition->SetAssociatedCursor(GetStorage()->GetController()->GetResourceService()->GetSystemCursor(ICursor::IBeam));
				SelectionChanged.SetAssociatedComposition(textControl->GetBoundsComposition());

				RefPtr<Composition> focusableComposition = textControl->GetFocusableComposition();
				focusableComposition->GetEventReceiver()->gotFocus.AttachMethod(this, &TextBoxBase::OnGotFocus);
				focusableComposition->GetEventReceiver()->lostFocus.AttachMethod(this, &TextBoxBase::OnLostFocus);
				focusableComposition->GetEventReceiver()->caretNotify.AttachMethod(this, &TextBoxBase::OnCaretNotify);
				textComposition->GetEventReceiver()->leftButtonDown.AttachMethod(this, &TextBoxBase::OnLeftButtonDown);
				textComposition->GetEventReceiver()->leftButtonUp.AttachMethod(this, &TextBoxBase::OnLeftButtonUp);
				textComposition->GetEventReceiver()->mouseMove.AttachMethod(this, &TextBoxBase::OnMouseMove);
				focusableComposition->GetEventReceiver()->keyDown.AttachMethod(this, &TextBoxBase::OnKeyDown);
				focusableComposition->GetEventReceiver()->charInput.AttachMethod(this, &TextBoxBase::OnCharInput);

				for (auto & textEditCallback : textEditCallbacks)
				{
					textEditCallback->Attach(textElement, elementModifyLock, textComposition, editVersion);
				}
			}

			PassRefPtr<ITextBoxBaseCallback> TextBoxBase::GetCallback()
			{
				return callback;
			}

			void TextBoxBase::SetCallback(PassRefPtr<ITextBoxBaseCallback> value)
			{
				callback = value;
			}

			bool TextBoxBase::AttachTextEditCallback(PassRefPtr<ITextEditCallback> value)
			{
				RefPtr<ITextEditCallback> _value = value;
				if (textEditCallbacks.find(_value) != textEditCallbacks.end())
				{
					return false;
				}
				else
				{
					textEditCallbacks.insert(_value);
					if (textElement)
					{
						_value->Attach(textElement, elementModifyLock, textComposition, editVersion);
					}
					return true;
				}
			}

			bool TextBoxBase::DetachTextEditCallback(PassRefPtr<ITextEditCallback> value)
			{
				if (textEditCallbacks.erase(value))
				{
					value->Detach();
					return true;
				}
				else
				{
					return false;
				}
			}

			void TextBoxBase::AddShortcutCommand(cint key, const function<void()>& eventHandler)
			{
				RefPtr<IShortcutKeyItem> item = internalShortcutKeyManager->CreateShortcut(true, false, false, key);
				item->Executed.AttachLambda([=](PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					eventHandler();
				});
			}

			PassRefPtr<ColorizedTextElement> TextBoxBase::GetTextElement()
			{
				return textElement;
			}

			void TextBoxBase::UnsafeSetText(const CString& value)
			{
				if (textElement)
				{
					TextPos end;
					if (textElement->GetLines().GetCount() > 0)
					{
						end.row = textElement->GetLines().GetCount() - 1;
						end.column = textElement->GetLines().GetLine(end.row).dataLength;
					}
					Modify(TextPos(), end, value, false);
				}
			}

			void TextBoxBase::UpdateCaretPoint()
			{
				RefPtr<GraphicsHost> host = textComposition->GetRelatedGraphicsHost();
				if (host)
				{
					CRect caret = textElement->GetLines().GetRectFromTextPos(textElement->GetCaretEnd());
					CPoint view = textElement->GetViewPosition();
					cint x = caret.left - view.x;
					cint y = caret.bottom - view.y;
					host->SetCaretPoint(CPoint(x, y), textComposition);
				}
			}

			void TextBoxBase::Move(TextPos pos, bool shift)
			{
				TextPos oldBegin = textElement->GetCaretBegin();
				TextPos oldEnd = textElement->GetCaretEnd();

				pos = textElement->GetLines().Normalize(pos);
				if (!shift)
				{
					textElement->SetCaretBegin(pos);
				}
				textElement->SetCaretEnd(pos);
				if (textControl)
				{
					RefPtr<GraphicsHost> host = textComposition->GetRelatedGraphicsHost();
					if (host)
					{
						if (host->GetFocusedComposition() == textControl->GetFocusableComposition())
						{
							textElement->SetCaretVisible(true);
						}
					}
				}

				CRect bounds = textElement->GetLines().GetRectFromTextPos(pos);
				CRect view = CRect(textElement->GetViewPosition(), textComposition->GetBounds().Size());
				CPoint viewPoint = view.TopLeft();

				if (view.right > view.left && view.bottom > view.top)
				{
					if (bounds.left < view.left)
					{
						viewPoint.x = bounds.left;
					}
					else if (bounds.right > view.right)
					{
						viewPoint.x = bounds.right - view.Width();
					}
					if (bounds.top < view.top)
					{
						viewPoint.y = bounds.top;
					}
					else if (bounds.bottom > view.bottom)
					{
						viewPoint.y = bounds.bottom - view.Height();
					}
				}

				callback->ScrollToView(viewPoint);
				callback->ScrollToView(viewPoint);
				UpdateCaretPoint();

				TextPos newBegin = textElement->GetCaretBegin();
				TextPos newEnd = textElement->GetCaretEnd();
				if (oldBegin != newBegin || oldEnd != newEnd)
				{
					TextCaretChangedStruct arguments;
					arguments.oldBegin = oldBegin;
					arguments.oldEnd = oldEnd;
					arguments.newBegin = newBegin;
					arguments.newEnd = newEnd;
					arguments.editVersion = editVersion;
					for (auto & textEditCallback : textEditCallbacks)
					{
						textEditCallback->TextCaretChanged(arguments);
					}
					SelectionChanged.Execute(textControl->GetNotifyEventArguments());
				}
			}

			bool TextBoxBase::CanModify(TextPos& start, TextPos& end, const CString& input, bool asKeyInput)
			{
				return true;
			}

			void TextBoxBase::Modify(TextPos start, TextPos end, const CString& input, bool asKeyInputs)
			{
				if (!CanModify(start, end, input, asKeyInputs))
					return;

				if (start > end)
				{
					TextPos temp = start;
					start = end;
					end = temp;
				}
				TextPos originalStart = start;
				TextPos originalEnd = end;
				CString originalText = textElement->GetLines().GetText(start, end);
				CString inputText = input;
				if (callback->BeforeModify(start, end, originalText, inputText))
				{
					{
						TextEditPreviewStruct arguments;
						arguments.originalStart = originalStart;
						arguments.originalEnd = originalEnd;
						arguments.originalText = originalText;
						arguments.inputText = inputText;
						arguments.editVersion = editVersion;
						arguments.keyInput = asKeyInputs;
						for (auto & textEditCallback : textEditCallbacks)
						{
							textEditCallback->TextEditPreview(arguments);
						}

						inputText = arguments.inputText;
						if (originalStart != arguments.originalStart || originalEnd != arguments.originalEnd)
						{
							originalStart = arguments.originalStart;
							originalEnd = arguments.originalEnd;
							originalText = textElement->GetLines().GetText(originalStart, originalEnd);
							start = originalStart;
							end = originalEnd;
						}
					}

					{
						lock_guard<spin_mutex> lock(elementModifyLock);
						end = textElement->GetLines().Modify(start, end, inputText, colorIndex);
					}

					callback->AfterModify(originalStart, originalEnd, originalText, start, end, inputText);

					editVersion++;
					{
						TextEditNotifyStruct arguments;
						arguments.originalStart = originalStart;
						arguments.originalEnd = originalEnd;
						arguments.originalText = originalText;
						arguments.inputStart = start;
						arguments.inputEnd = end;
						arguments.inputText = inputText;
						arguments.editVersion = editVersion;
						arguments.keyInput = asKeyInputs;
						for (auto & textEditCallback : textEditCallbacks)
						{
							textEditCallback->TextEditNotify(arguments);
						}
					}

					Move(end, false);

					for (auto & textEditCallback : textEditCallbacks)
					{
						textEditCallback->TextEditFinished(editVersion);
					}

					textControl->TextChanged.Execute(textControl->GetNotifyEventArguments());
				}
			}

			bool TextBoxBase::ProcessKey(cint code, bool shift, bool ctrl)
			{
				RefPtr<IShortcutKeyItem> item = internalShortcutKeyManager->TryGetShortcut(ctrl, shift, false, code);
				if (item)
				{
					EventArgs arguments;
					item->Executed.Execute(arguments);
					return true;
				}

				TextPos begin = textElement->GetCaretBegin();
				TextPos end = textElement->GetCaretEnd();

				switch (code)
				{
					case VK_ESCAPE:
						return true;
					case VK_RETURN:
						break;
					case VK_UP:
						end.row--;
						Move(end, shift);
						return true;
					case VK_DOWN:
						end.row++;
						Move(end, shift);
						return true;
					case VK_LEFT:
					{
						if (ctrl)
						{
							Move(callback->GetLeftWord(end), shift);
						}
						else
						{
							if (end.column == 0)
							{
								if (end.row > 0)
								{
									end.row--;
									end = textElement->GetLines().Normalize(end);
									end.column = textElement->GetLines().GetLine(end.row).dataLength;
								}
							}
							else
							{
								end.column--;
							}
							Move(end, shift);
						}
					}
						return true;
					case VK_RIGHT:
					{
						if (ctrl)
						{
							Move(callback->GetRightWord(end), shift);
						}
						else
						{
							if (end.column == textElement->GetLines().GetLine(end.row).dataLength)
							{
								if (end.row < textElement->GetLines().GetCount() - 1)
								{
									end.row++;
									end.column = 0;
								}
							}
							else
							{
								end.column++;
							}
							Move(end, shift);
						}
					}
						return true;
					case VK_HOME:
					{
						if (ctrl)
						{
							Move(TextPos(0, 0), shift);
						}
						else
						{
							end.column = 0;
							Move(end, shift);
						}
					}
						return true;
					case VK_END:
					{
						if (ctrl)
						{
							end.row = textElement->GetLines().GetCount() - 1;
						}
						end.column = textElement->GetLines().GetLine(end.row).dataLength;
						Move(end, shift);
					}
						return true;
					case VK_PRIOR:
					{
						end.row -= callback->GetPageRows();
						Move(end, shift);
					}
						return true;
					case VK_NEXT:
					{
						end.row += callback->GetPageRows();
						Move(end, shift);
					}
						return true;
					case VK_BACK:
						if (!readonly)
						{
							if (ctrl && !shift)
							{
								ProcessKey(VK_HOME, true, false);
								ProcessKey(VK_BACK, false, false);
							}
							else if (!ctrl && shift)
							{
								ProcessKey(VK_UP, true, false);
								ProcessKey(VK_BACK, false, false);
							}
							else
							{
								if (begin == end)
								{
									ProcessKey(VK_LEFT, true, false);
								}
								SetSelectionTextAsKeyInput(_T(""));
							}
							return true;
						}
						break;
					case VK_DELETE:
						if (!readonly)
						{
							if (ctrl && !shift)
							{
								ProcessKey(VK_END, true, false);
								ProcessKey(VK_DELETE, false, false);
							}
							else if (!ctrl && shift)
							{
								ProcessKey(VK_DOWN, true, false);
								ProcessKey(VK_DELETE, false, false);
							}
							else
							{
								if (begin == end)
								{
									ProcessKey(VK_RIGHT, true, false);
								}
								SetSelectionTextAsKeyInput(_T(""));
							}
							return true;
						}
						break;
				}
				return false;
			}

			void TextBoxBase::OnGotFocus(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				textElement->SetFocused(true);
				textElement->SetCaretVisible(true);
				UpdateCaretPoint();
			}

			void TextBoxBase::OnLostFocus(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				textElement->SetFocused(false);
				textElement->SetCaretVisible(false);
			}

			void TextBoxBase::OnCaretNotify(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				textElement->SetCaretVisible(!textElement->GetCaretVisible());
			}

			void TextBoxBase::OnLeftButtonDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
			{
				if (textControl->GetVisuallyEnabled() && arguments.compositionSource == arguments.eventSource)
				{
					dragging = true;
					TextPos pos = GetNearestTextPos(arguments.pt);
					Move(pos, arguments.shift);
				}
			}

			void TextBoxBase::OnLeftButtonUp(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
			{
				if (textControl->GetVisuallyEnabled() && arguments.compositionSource == arguments.eventSource)
				{
					dragging = false;
				}
			}

			void TextBoxBase::OnMouseMove(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
			{
				if (textControl->GetVisuallyEnabled() && arguments.compositionSource == arguments.eventSource)
				{
					if (dragging)
					{
						TextPos pos = GetNearestTextPos(arguments.pt);
						Move(pos, true);
					}
				}
			}

			void TextBoxBase::OnKeyDown(PassRefPtr<Composition> sender, KeyEventArgs& arguments)
			{
				if (textControl->GetVisuallyEnabled() && arguments.compositionSource == arguments.eventSource)
				{
					if (ProcessKey(arguments.code, arguments.shift, arguments.ctrl))
					{
						arguments.handled = true;
					}
				}
			}

			void TextBoxBase::OnCharInput(PassRefPtr<Composition> sender, CharEventArgs& arguments)
			{
				if (textControl->GetVisuallyEnabled() && arguments.compositionSource == arguments.eventSource)
				{
					if (!readonly)
					{
						BeforeCharNotify.Execute(arguments);
						if (arguments.code != VK_ESCAPE && arguments.code != VK_BACK && !arguments.ctrl)
						{
							SetSelectionTextAsKeyInput(CString((TCHAR)arguments.code));
						}
						AfterCharNotify.Execute(arguments);
					}
				}
			}

			bool TextBoxBase::SetTextColor(CColor color)
			{
				auto found = colorIndexes.find(color);
				if (found == colorIndexes.end())
				{
					ColorEntry entry = textElement->GetColors()[0];
					entry.normal.text = color;
					entry.selectedFocused.text = CColor(color.value ^ 1);
					colorIndex = textElement->AddColor(entry);
					colorIndexes.insert(make_pair(color, colorIndex));
					return true;
				}
				else
				{
					colorIndex = found->second;
					return false;
				}
			}

			void TextBoxBase::SetTextColorToDefault()
			{
				colorIndex = 0;
			}

			cint TextBoxBase::GetTextColorIndex(CColor color)
			{
				auto found = colorIndexes.find(color);
				if (found == colorIndexes.end())
				{
					return found->second;
				}
				else
				{
					return -1;
				}
			}

			GeneralUndoRedoProcessor::GeneralUndoRedoProcessor()
				: firstFutureStep(0)
				, savedStep(0)
				, performingUndoRedo(false)
			{

			}

			GeneralUndoRedoProcessor::~GeneralUndoRedoProcessor()
			{

			}

			bool GeneralUndoRedoProcessor::CanUndo()
			{
				return firstFutureStep > 0;
			}

			bool GeneralUndoRedoProcessor::CanRedo()
			{
				return (cint)steps.size() > firstFutureStep;
			}

			void GeneralUndoRedoProcessor::ClearUndoRedo()
			{
				if (!performingUndoRedo)
				{
					steps.clear();
					firstFutureStep = 0;
					savedStep = -1;
				}
			}

			bool GeneralUndoRedoProcessor::GetModified()
			{
				return firstFutureStep != savedStep;
			}

			void GeneralUndoRedoProcessor::NotifyModificationSaved()
			{
				if (!performingUndoRedo)
				{
					savedStep = firstFutureStep;
				}
			}

			bool GeneralUndoRedoProcessor::Undo()
			{
				if (!CanUndo()) return false;
				performingUndoRedo = true;
				firstFutureStep--;
				steps[firstFutureStep]->Undo();
				performingUndoRedo = false;
				return true;
			}

			bool GeneralUndoRedoProcessor::Redo()
			{
				if (!CanRedo()) return false;
				performingUndoRedo = true;
				firstFutureStep++;
				steps[firstFutureStep - 1]->Redo();
				performingUndoRedo = false;
				return true;
			}

			void GeneralUndoRedoProcessor::PushStep(PassRefPtr<IGeneralEditStep> step)
			{
				if (!performingUndoRedo)
				{
					if (firstFutureStep<savedStep)
					{
						savedStep = -1;
					}

					cint count = steps.size() - firstFutureStep;
					if (count > 0)
					{
						steps.erase(steps.begin() + firstFutureStep, steps.begin() + firstFutureStep + count);
					}

					steps.push_back(step);
					firstFutureStep = steps.size();
				}
			}

			void TextBoxEditStep::Undo()
			{
				RefPtr<TextBoxBase> ci = dynamic_cast<TextBoxBase*>(processor->ownerComposition->GetRelatedControl().get());
				if (ci)
				{
					ci->Select(arguments.inputStart, arguments.inputEnd);
					ci->SetSelectionText(arguments.originalText);
					ci->Select(arguments.originalStart, arguments.originalEnd);
				}
			}

			void TextBoxEditStep::Redo()
			{
				RefPtr<TextBoxBase> ci = dynamic_cast<TextBoxBase*>(processor->ownerComposition->GetRelatedControl().get());
				if (ci)
				{
					ci->Select(arguments.originalStart, arguments.originalEnd);
					ci->SetSelectionText(arguments.inputText);
					ci->Select(arguments.inputStart, arguments.inputEnd);
				}
			}

			TextBoxUndoRedoProcessor::TextBoxUndoRedoProcessor()
			{

			}

			TextBoxUndoRedoProcessor::~TextBoxUndoRedoProcessor()
			{

			}

			void TextBoxUndoRedoProcessor::Attach(PassRefPtr<ColorizedTextElement> element, spin_mutex& elementModifyLock, PassRefPtr<Composition> ownerComposition, cuint editVersion)
			{
				this->ownerComposition = ownerComposition;
			}

			void TextBoxUndoRedoProcessor::Detach()
			{
				ClearUndoRedo();
			}

			void TextBoxUndoRedoProcessor::TextEditPreview(TextEditPreviewStruct& arguments)
			{

			}

			void TextBoxUndoRedoProcessor::TextEditNotify(const TextEditNotifyStruct& arguments)
			{
				RefPtr<TextBoxEditStep> step = adoptRef(new TextBoxEditStep);
				step->processor = this;
				step->arguments = arguments;
				PushStep(step);
			}

			void TextBoxUndoRedoProcessor::TextCaretChanged(const TextCaretChangedStruct& arguments)
			{

			}

			void TextBoxUndoRedoProcessor::TextEditFinished(cuint editVersion)
			{

			}

			namespace style
			{
				SinglelineTextBoxStyleController::SinglelineTextBoxStyleController(PassRefPtr<ISinglelineTextBoxStyleProvider> _styleProvider)
					: styleProvider(_styleProvider)
				{
					boundsComposition = adoptRef(new BoundsComposition);
					containerComposition = styleProvider->InstallBackground(boundsComposition);

					textElement = ColorizedTextElement::Create();
					textElement->SetViewPosition(CPoint(-SinglelineTextBox::TextMargin, -SinglelineTextBox::TextMargin));

					textCompositionTable = adoptRef(new TableComposition);
					textCompositionTable->SetAlignmentToParent(CRect());
					textCompositionTable->SetRowsAndColumns(3, 1);
					textCompositionTable->SetRowOption(0, CellOption::PercentageOption(0.5));
					textCompositionTable->SetRowOption(1, CellOption::AbsoluteOption(0));
					textCompositionTable->SetRowOption(2, CellOption::PercentageOption(0.5));
					textCompositionTable->SetColumnOption(0, CellOption::PercentageOption(1.0));
					containerComposition->AddChild(textCompositionTable);

					textComposition = adoptRef(new TableCellComposition);
					textComposition->SetOwnedElement(textElement);
					textCompositionTable->AddChild(textComposition);
					textComposition->SetSite(1, 0, 1, 1);

					styleProvider->AssociateStyleController(this);
				}

				SinglelineTextBoxStyleController::~SinglelineTextBoxStyleController()
				{

				}

				void SinglelineTextBoxStyleController::Initialize(PassRefPtr<SinglelineTextBox> control)
				{
					textBox = control;
				}

				void SinglelineTextBoxStyleController::RearrangeTextElement()
				{
					textCompositionTable->SetRowOption(1, CellOption::AbsoluteOption(textElement->GetLines().GetRowHeight() + 2 * SinglelineTextBox::TextMargin));
				}

				PassRefPtr<BoundsComposition> SinglelineTextBoxStyleController::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> SinglelineTextBoxStyleController::GetContainerComposition()
				{
					return containerComposition;
				}

				void SinglelineTextBoxStyleController::SetFocusableComposition(PassRefPtr<Composition> value)
				{
					styleProvider->SetFocusableComposition(value);
					textBox->Install(textElement, textComposition, textBox);
					if (!textBox->GetCallback())
					{
						if (!defaultCallback)
						{
							defaultCallback = adoptRef(new SinglelineTextBoxElementOperatorCallback(textBox));
						}
						textBox->SetCallback(defaultCallback);
					}
				}

				CString SinglelineTextBoxStyleController::GetText()
				{
					return textElement->GetLines().GetText();
				}

				void SinglelineTextBoxStyleController::SetText(const CString& value)
				{
					if (textBox)
					{
						textBox->UnsafeSetText(value);
					}
					textElement->SetCaretBegin(TextPos(0, 0));
					textElement->SetCaretEnd(TextPos(0, 0));
					styleProvider->SetText(value);
				}

				void SinglelineTextBoxStyleController::SetFont(const Font& value)
				{
					textElement->SetFont(value);
					styleProvider->SetFont(value);
				}

				void SinglelineTextBoxStyleController::SetVisuallyEnabled(bool value)
				{
					textElement->SetVisuallyEnabled(value);
					styleProvider->SetVisuallyEnabled(value);
				}

				PassRefPtr<ColorizedTextElement> SinglelineTextBoxStyleController::GetTextElement()
				{
					return textElement;
				}

				PassRefPtr<Composition> SinglelineTextBoxStyleController::GetTextComposition()
				{
					return textComposition;
				}

				void SinglelineTextBoxStyleController::SetViewPosition(CPoint value)
				{
					textElement->SetViewPosition(value);
				}

				MultilineTextBoxStyleController::MultilineTextBoxStyleController(PassRefPtr<IScrollViewStyleProvider> _styleProvider)
					: ScrollViewStyleController(_styleProvider)
				{
					textElement = ColorizedTextElement::Create();

					textComposition = adoptRef(new BoundsComposition);
					textComposition->SetAlignmentToParent(CRect(0, 0, 0, 0));
					textComposition->SetOwnedElement(textElement);

					GetInternalContainerComposition()->AddChild(textComposition);
				}

				MultilineTextBoxStyleController::~MultilineTextBoxStyleController()
				{

				}

				void MultilineTextBoxStyleController::Initialize(PassRefPtr<MultilineTextBox> control)
				{
					textBox = control;
				}

				void MultilineTextBoxStyleController::SetFocusableComposition(PassRefPtr<Composition> value)
				{
					ScrollViewStyleController::SetFocusableComposition(value);
					textBox->Install(textElement, textComposition, scrollView);
					if (!textBox->GetCallback())
					{
						if (!defaultCallback)
						{
							defaultCallback = adoptRef(new MultilineTextBoxElementOperatorCallback(dynamic_cast<MultilineTextBox*>(~scrollView)));
						}
						textBox->SetCallback(defaultCallback);
					}
				}

				CString MultilineTextBoxStyleController::GetText()
				{
					return textElement->GetLines().GetText();
				}

				void MultilineTextBoxStyleController::SetText(const CString& value)
				{
					if (textBox)
					{
						textBox->UnsafeSetText(value);
					}
					textElement->SetCaretBegin(TextPos(0, 0));
					textElement->SetCaretEnd(TextPos(0, 0));
					ScrollViewStyleController::SetText(value);
				}

				void MultilineTextBoxStyleController::SetFont(const Font& value)
				{
					textElement->SetFont(value);
					ScrollViewStyleController::SetFont(value);
				}

				void MultilineTextBoxStyleController::SetVisuallyEnabled(bool value)
				{
					textElement->SetVisuallyEnabled(value);
					ScrollViewStyleController::SetVisuallyEnabled(value);
				}

				PassRefPtr<ColorizedTextElement> MultilineTextBoxStyleController::GetTextElement()
				{
					return textElement;
				}

				PassRefPtr<Composition> MultilineTextBoxStyleController::GetTextComposition()
				{
					return textComposition;
				}

				void MultilineTextBoxStyleController::SetViewPosition(CPoint value)
				{
					textElement->SetViewPosition(value);
				}
			}

			SinglelineTextBox::SinglelineTextBox(PassRefPtr<ISinglelineTextBoxStyleProvider> styleProvider)
				: Control(adoptRef(new SinglelineTextBoxStyleController(styleProvider)))
			{
				styleController = dynamic_cast<SinglelineTextBoxStyleController*>(GetStyleController().get());
				styleController->Initialize(this);
				SetFocusableComposition(boundsComposition);

				boundsComposition->GetEventReceiver()->leftButtonDown.AttachMethod(this, &SinglelineTextBox::OnBoundsMouseButtonDown);
				boundsComposition->GetEventReceiver()->middleButtonDown.AttachMethod(this, &SinglelineTextBox::OnBoundsMouseButtonDown);
				boundsComposition->GetEventReceiver()->rightButtonDown.AttachMethod(this, &SinglelineTextBox::OnBoundsMouseButtonDown);
			}

			SinglelineTextBox::~SinglelineTextBox()
			{

			}

			const CString& SinglelineTextBox::GetText()
			{
				text = styleController->GetText();
				return text;
			}

			void SinglelineTextBox::SetText(const CString& value)
			{
				Control::SetText(value);
			}

			void SinglelineTextBox::SetFont(const Font& value)
			{
				Control::SetFont(value);
				styleController->RearrangeTextElement();
			}

			TCHAR SinglelineTextBox::GetPasswordChar()
			{
				return styleController->GetTextElement()->GetPasswordChar();
			}

			void SinglelineTextBox::SetPasswordChar(TCHAR value)
			{
				styleController->GetTextElement()->SetPasswordChar(value);
			}

			void SinglelineTextBox::OnRenderTargetChanged(PassRefPtr<IGraphicsRenderTarget> renderTarget)
			{
				styleController->RearrangeTextElement();
				Control::OnRenderTargetChanged(renderTarget);
			}

			void SinglelineTextBox::OnBoundsMouseButtonDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
			{
				if (GetVisuallyEnabled())
				{
					boundsComposition->GetRelatedGraphicsHost()->SetFocus(boundsComposition);
				}
			}

			MultilineTextBox::MultilineTextBox(PassRefPtr<IScrollViewStyleProvider> styleProvider)
				: ScrollView(adoptRef(dynamic_cast<ScrollViewStyleController*>(new MultilineTextBoxStyleController(styleProvider))))
			{
				styleController = dynamic_cast<MultilineTextBoxStyleController*>(GetStyleController().get());
				styleController->Initialize(this);
				SetFocusableComposition(boundsComposition);

				boundsComposition->GetEventReceiver()->leftButtonDown.AttachMethod(this, &MultilineTextBox::OnBoundsMouseButtonDown);
				boundsComposition->GetEventReceiver()->middleButtonDown.AttachMethod(this, &MultilineTextBox::OnBoundsMouseButtonDown);
				boundsComposition->GetEventReceiver()->rightButtonDown.AttachMethod(this, &MultilineTextBox::OnBoundsMouseButtonDown);
			}

			MultilineTextBox::~MultilineTextBox()
			{

			}

			const CString& MultilineTextBox::GetText()
			{
				text = styleController->GetText();
				return text;
			}

			void MultilineTextBox::SetText(const CString& value)
			{
				text = styleController->GetText();
				ScrollView::SetText(value);
				CalculateView();
			}

			void MultilineTextBox::SetFont(const Font& value)
			{
				ScrollView::SetFont(value);
				CalculateViewAndSetScroll();
			}

			void MultilineTextBox::CalculateViewAndSetScroll()
			{
				CalculateView();
				cint smallMove = styleController->GetTextElement()->GetLines().GetRowHeight();
				cint bigMove = smallMove * 5;
				styleController->GetHorizontalScroll()->SetSmallMove(smallMove);
				styleController->GetHorizontalScroll()->SetBigMove(bigMove);
				styleController->GetVerticalScroll()->SetSmallMove(smallMove);
				styleController->GetVerticalScroll()->SetBigMove(bigMove);
			}

			CSize MultilineTextBox::QueryFullSize()
			{
				auto& lines = styleController->GetTextElement()->GetLines();
				return CSize(lines.GetMaxWidth() + TextMargin * 2, lines.GetMaxHeight() + TextMargin * 2);
			}

			void MultilineTextBox::UpdateView(CRect viewBounds)
			{
				styleController->SetViewPosition(viewBounds.TopLeft() - CSize(TextMargin, TextMargin));
			}

			void MultilineTextBox::OnRenderTargetChanged(PassRefPtr<IGraphicsRenderTarget> renderTarget)
			{
				CalculateViewAndSetScroll();
				ScrollView::OnRenderTargetChanged(renderTarget);
			}

			void MultilineTextBox::OnBoundsMouseButtonDown(PassRefPtr<Composition> sender, MouseEventArgs& arguments)
			{
				if (GetVisuallyEnabled())
				{
					boundsComposition->GetRelatedGraphicsHost()->SetFocus(boundsComposition);
				}
			}

			Win8TextBoxBackground::Win8TextBoxBackground()
				: isMouseEnter(false)
				, isFocused(false)
				, isVisuallyEnabled(false)
			{
				transferringAnimation = adoptRef(new TransferringAnimationType(this, Win8TextBoxColors::Normal()));
			}

			Win8TextBoxBackground::~Win8TextBoxBackground()
			{
				transferringAnimation->Disable();
			}

			void Win8TextBoxBackground::AssociateStyleController(PassRefPtr<IControlStyleController> controller)
			{
				styleController = controller;
			}

			void Win8TextBoxBackground::SetFocusableComposition(PassRefPtr<Composition> value)
			{
				focusableComposition = value;
				focusableComposition->GetEventReceiver()->mouseEnter.AttachMethod(this, &Win8TextBoxBackground::OnBoundsMouseEnter);
				focusableComposition->GetEventReceiver()->mouseLeave.AttachMethod(this, &Win8TextBoxBackground::OnBoundsMouseLeave);
				focusableComposition->GetEventReceiver()->gotFocus.AttachMethod(this, &Win8TextBoxBackground::OnBoundsGotFocus);
				focusableComposition->GetEventReceiver()->lostFocus.AttachMethod(this, &Win8TextBoxBackground::OnBoundsLostFocus);
			}

			void Win8TextBoxBackground::SetVisuallyEnabled(bool value)
			{
				isVisuallyEnabled = value;
				UpdateStyle();
			}

			PassRefPtr<Composition> Win8TextBoxBackground::InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)
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
				}
				{
					RefPtr<SolidBorderElement> border = SolidBorderElement::Create();
					border->SetColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Border));
					borderElement = border;

					RefPtr<BoundsComposition> borderComposition = adoptRef(new BoundsComposition);
					boundsComposition->AddChild(borderComposition);
					borderComposition->SetAlignmentToParent(CRect(0, 0, 0, 0));
					borderComposition->SetOwnedElement(border);
				}
				Apply(Win8TextBoxColors::Normal());
				{
					RefPtr<BoundsComposition> containerComposition = adoptRef(new BoundsComposition);
					boundsComposition->AddChild(containerComposition);
					containerComposition->SetAlignmentToParent(CRect(2, 2, 2, 2));
					return containerComposition;
				}
			}

			void Win8TextBoxBackground::InitializeTextElement(PassRefPtr<ColorizedTextElement> _textElement)
			{
				textElement = _textElement;
				textElement->AddColor(Win8WindowStyle::GetTextColor());
				textElement->SetCaretColor(CColor(0, 0, 0));
			}

			void Win8TextBoxBackground::ToAnimation(PassRefPtr<GraphicsAnimationManager> manager)
			{
				manager->AddAnimation(transferringAnimation);
			}

			void Win8TextBoxBackground::UpdateStyle()
			{
				if (!isVisuallyEnabled)
				{
					transferringAnimation->Transfer(Win8TextBoxColors::Disabled());
				}
				else if (isFocused)
				{
					transferringAnimation->Transfer(Win8TextBoxColors::Focused());
				}
				else if (isMouseEnter)
				{
					transferringAnimation->Transfer(Win8TextBoxColors::Active());
				}
				else
				{
					transferringAnimation->Transfer(Win8TextBoxColors::Normal());
				}
			}

			void Win8TextBoxBackground::Apply(const Win8TextBoxColors& colors)
			{
				borderElement->SetColor(colors.borderColor);
				backgroundElement->SetColor(colors.backgroundColor);
			}

			void Win8TextBoxBackground::OnBoundsMouseEnter(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				isMouseEnter = true;
				UpdateStyle();
			}

			void Win8TextBoxBackground::OnBoundsMouseLeave(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				isMouseEnter = false;
				UpdateStyle();
			}

			void Win8TextBoxBackground::OnBoundsGotFocus(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				isFocused = true;
				UpdateStyle();
			}

			void Win8TextBoxBackground::OnBoundsLostFocus(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				isFocused = false;
				UpdateStyle();
			}

			Win8TextBoxBackground::TransferringAnimationType::TransferringAnimationType(Win8TextBoxBackground* _style, const Win8TextBoxColors& begin) : TransferringAnimationFocusable(_style, begin)
			{

			}

			void Win8TextBoxBackground::TransferringAnimationType::PlayInternal(cint currentPosition, cint totalLength)
			{
				colorCurrent = Win8TextBoxColors::Blend(colorBegin, colorEnd, currentPosition, totalLength);
				style->Apply(colorCurrent);
			}

			namespace style
			{
				Win8SinglelineTextBoxProvider::Win8SinglelineTextBoxProvider()
				{

				}

				Win8SinglelineTextBoxProvider::~Win8SinglelineTextBoxProvider()
				{

				}

				void Win8SinglelineTextBoxProvider::AssociateStyleController(PassRefPtr<IControlStyleController> controller)
				{
					styleController = controller;
					background.AssociateStyleController(controller);
				}

				void Win8SinglelineTextBoxProvider::SetFocusableComposition(PassRefPtr<Composition> value)
				{
					background.SetFocusableComposition(value);
					RefPtr<SinglelineTextBoxStyleController> textBoxController = dynamic_cast<SinglelineTextBoxStyleController*>(~styleController);
					background.InitializeTextElement(textBoxController->GetTextElement());
				}

				void Win8SinglelineTextBoxProvider::SetText(const CString& value)
				{

				}

				void Win8SinglelineTextBoxProvider::SetFont(const Font& value)
				{

				}

				void Win8SinglelineTextBoxProvider::SetVisuallyEnabled(bool value)
				{
					background.SetVisuallyEnabled(value);
				}

				PassRefPtr<Composition> Win8SinglelineTextBoxProvider::InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)
				{
					return background.InstallBackground(boundsComposition);
				}

				Win8MultilineTextBoxProvider::Win8MultilineTextBoxProvider()
				{

				}

				Win8MultilineTextBoxProvider::~Win8MultilineTextBoxProvider()
				{

				}

				void Win8MultilineTextBoxProvider::AssociateStyleController(PassRefPtr<IControlStyleController> controller)
				{
					styleController = controller;
					background.AssociateStyleController(controller);
				}

				void Win8MultilineTextBoxProvider::SetFocusableComposition(PassRefPtr<Composition> value)
				{
					background.SetFocusableComposition(value);
					RefPtr<MultilineTextBoxStyleController> textBoxController = dynamic_cast<MultilineTextBoxStyleController*>(~styleController);
					if (textBoxController)
					{
						background.InitializeTextElement(textBoxController->GetTextElement());
					}
				}

				void Win8MultilineTextBoxProvider::SetVisuallyEnabled(bool value)
				{
					background.SetVisuallyEnabled(value);
				}

				PassRefPtr<Composition> Win8MultilineTextBoxProvider::InstallBackground(PassRefPtr<BoundsComposition> boundsComposition)
				{
					return background.InstallBackground(boundsComposition);
				}
			}
		}
	}
}