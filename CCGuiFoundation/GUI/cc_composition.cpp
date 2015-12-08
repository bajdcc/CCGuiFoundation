#include "stdafx.h"
#include "cc_control.h"
#include "cc_presentation.h"
#include "cc_composition.h"

using namespace cc::presentation::windows;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			namespace event_args
			{
				EventReceiver::EventReceiver(PassRefPtr<Composition> _sender)
					: sender(_sender.get())
					, leftButtonDown(_sender.get())
					, leftButtonUp(_sender.get())
					, leftButtonDoubleClick(_sender.get())
					, middleButtonDown(_sender.get())
					, middleButtonUp(_sender.get())
					, middleButtonDoubleClick(_sender.get())
					, rightButtonDown(_sender.get())
					, rightButtonUp(_sender.get())
					, rightButtonDoubleClick(_sender.get())
					, horizontalWheel(_sender.get())
					, verticalWheel(_sender.get())
					, mouseMove(_sender.get())
					, mouseEnter(_sender.get())
					, mouseLeave(_sender.get())
					, previewKey(_sender.get())
					, keyDown(_sender.get())
					, keyUp(_sender.get())
					, systemKeyDown(_sender.get())
					, systemKeyUp(_sender.get())
					, previewCharInput(_sender.get())
					, charInput(_sender.get())
					, gotFocus(_sender.get())
					, lostFocus(_sender.get())
					, caretNotify(_sender.get())
					, clipboardNotify(_sender.get())
				{

				}

				EventReceiver::~EventReceiver()
				{

				}

				PassRefPtr<Composition> EventReceiver::GetAssociatedComposition()
				{
					return sender;
				}

				EventArgs::EventArgs() : handled(false)
				{

				}

				EventArgs::EventArgs(PassRefPtr<Composition> composition) : compositionSource(composition.get())
					, eventSource(composition.get())
					, handled(false)
				{

				}

				RequestEventArgs::RequestEventArgs() :cancel(false)
				{

				}

				RequestEventArgs::RequestEventArgs(PassRefPtr<Composition> composition) : EventArgs(composition)
					, cancel(false)
				{

				}

				KeyEventArgs::KeyEventArgs()
				{

				}

				KeyEventArgs::KeyEventArgs(PassRefPtr<Composition> composition) : EventArgs(composition)
				{

				}

				CharEventArgs::CharEventArgs()
				{

				}

				CharEventArgs::CharEventArgs(PassRefPtr<Composition> composition) : EventArgs(composition)
				{

				}

				MouseEventArgs::MouseEventArgs()
				{

				}

				MouseEventArgs::MouseEventArgs(PassRefPtr<Composition> composition) : EventArgs(composition)
				{

				}
			}

			namespace composition
			{
				Composition::Composition()
					: associatedHitTestResult(IWindowListener::NoDecision)
					, visible(true)
					, minSizeLimitation(NoLimit)
				{

				}

				Composition::~Composition()
				{

				}

				PassRefPtr<Composition> Composition::GetParent()
				{
					return parent;
				}

				const Composition::CompositionList Composition::Children()
				{
					return children;
				}

				bool Composition::AddChild(PassRefPtr<Composition> child)
				{
					return InsertChild(children.size(), child);
				}

				bool Composition::InsertChild(cint index, PassRefPtr<Composition> child)
				{
					RefPtr<Composition> _child = child;
					if (!_child) return false;
					if (_child->GetParent()) return false;
					children.insert(children.begin() + index, _child);
					_child->parent = this;
					_child->SetRenderTarget(renderTarget);
					OnChildInserted(_child);
					_child->OnParentChanged(nullptr, _child->parent);
					return true;
				}

				bool Composition::RemoveChild(PassRefPtr<Composition> child)
				{
					RefPtr<Composition> _child = child;
					if (!_child) return false;
					auto found = find(children.begin(), children.end(), _child);
					if (found == children.end()) return false;
					_child->OnParentChanged(_child->parent, nullptr);
					OnChildRemoved(_child);
					_child->SetRenderTarget(nullptr);
					_child->parent = nullptr;
					RefPtr<GraphicsHost> host = GetRelatedGraphicsHost();
					if (host)
					{
						host->DisconnectComposition(_child);
					}
					children.erase(found);
					return true;
				}

				bool Composition::MoveChild(PassRefPtr<Composition> child, cint newIndex)
				{
					RefPtr<Composition> _child = child;
					if (!_child) return false;
					auto found = find(children.begin(), children.end(), _child);
					if (found == children.end()) return false;
					children.erase(found);
					children.insert(children.begin() + newIndex, _child);
					return true;
				}

				PassRefPtr<IGraphicsElement> Composition::GetOwnedElement()
				{
					return ownedElement;
				}

				void Composition::SetOwnedElement(PassRefPtr<IGraphicsElement> element)
				{
					if (ownedElement)
					{
						RefPtr<IGraphicsRenderer> renderer = ownedElement->GetRenderer();
						if (renderer)
						{
							renderer->SetRenderTarget(nullptr);
						}
					}
					ownedElement = element;
					if (ownedElement)
					{
						RefPtr<IGraphicsRenderer> renderer = ownedElement->GetRenderer();
						if (renderer)
						{
							renderer->SetRenderTarget(renderTarget);
						}
					}
				}

				bool Composition::GetVisible()
				{
					return visible;
				}

				void Composition::SetVisible(bool value)
				{
					visible = value;
				}

				Composition::MinSizeLimitation Composition::GetMinSizeLimitation()
				{
					return minSizeLimitation;
				}

				void Composition::SetMinSizeLimitation(MinSizeLimitation value)
				{
					minSizeLimitation = value;
				}

				PassRefPtr<IGraphicsRenderTarget> Composition::GetRenderTarget()
				{
					return renderTarget;
				}

				void Composition::SetRenderTarget(PassRefPtr<IGraphicsRenderTarget> value)
				{
					renderTarget = value;
					if (ownedElement)
					{
						RefPtr<IGraphicsRenderer> renderer = ownedElement->GetRenderer();
						if (renderer)
						{
							renderer->SetRenderTarget(renderTarget);
						}
					}
					for (auto & child : children)
					{
						child->SetRenderTarget(renderTarget);
					}
					if (GetStorage()->GetApplication()->GetMainWindow())
					{
						OnRenderTargetChanged();
					}
				}

				void Composition::Render(CSize offset)
				{
					if (visible && renderTarget && !renderTarget->IsClipperCoverWholeTarget())
					{
						CRect bounds = GetBounds();
						bounds.DeflateRect(margin);

						if (bounds.Width() >= 0 && bounds.Height() >= 0)
						{
							bounds.OffsetRect(offset);

							if (ownedElement)
							{
								RefPtr<IGraphicsRenderer> renderer = ownedElement->GetRenderer();
								if (renderer)
								{
									renderer->Render(bounds);
								}
							}
							if (!children.empty())
							{
								bounds.DeflateRect(internalMargin);

								if (bounds.Width() >= 0 && bounds.Height() >= 0)
								{
									offset = bounds.Size();
									renderTarget->PushClipper(bounds);
									if (!renderTarget->IsClipperCoverWholeTarget())
									{
										for (cuint i = 0; i < children.size(); i++)
										{
											children[i]->Render(CSize(bounds.left, bounds.top));
										}

									}
									renderTarget->PopClipper();
								}
							}
						}
					}
				}

				PassRefPtr<EventReceiver> Composition::GetEventReceiver()
				{
					if (!eventReceiver)
					{
						eventReceiver = adoptRef(new EventReceiver(this));
					}
					return eventReceiver;
				}

				bool Composition::HasEventReceiver()
				{
					return eventReceiver;
				}

				PassRefPtr<Composition> Composition::FindComposition(CPoint location)
				{
					if (!visible)
						return nullptr;
					CRect bounds = GetBounds();
					CRect relativeBounds = CRect(CPoint(), bounds.Size());
					if (relativeBounds.PtInRect(location))
					{
						CRect clientArea = GetClientArea();
						for (auto it = children.rbegin(); it != children.rend(); it++)
						{
							RefPtr<Composition> & child = *it;
							CRect childBounds = child->GetBounds();
							CSize offset = childBounds.TopLeft() + clientArea.TopLeft() - bounds.TopLeft();
							CPoint newLocation = location - offset;
							RefPtr<Composition> childResult = child->FindComposition(newLocation);
							if (childResult)
							{
								return childResult;
							}
						}
						return this;
					}
					else
					{
						return nullptr;
					}
				}

				CRect Composition::GetGlobalBounds()
				{
					CRect bounds = GetBounds();
					RawPtr<Composition> _composition = parent;
					while (_composition)
					{
						CRect clientArea = _composition->GetClientArea();
						bounds.OffsetRect(clientArea.TopLeft());
						_composition = _composition->parent;
					}
					return bounds;
				}

				PassRefPtr<Control> Composition::GetAssociatedControl()
				{
					return associatedControl;
				}

				PassRefPtr<GraphicsHost> Composition::GetAssociatedHost()
				{
					return associatedHost;
				}

				PassRefPtr<ICursor> Composition::GetAssociatedCursor()
				{
					return associatedCursor;
				}

				void Composition::SetAssociatedCursor(PassRefPtr<ICursor> cursor)
				{
					associatedCursor = cursor;
				}

				IWindowListener::HitTestResult Composition::GetAssociatedHitTestResult()
				{
					return associatedHitTestResult;
				}

				void Composition::SetAssociatedHitTestResult(IWindowListener::HitTestResult value)
				{
					associatedHitTestResult = value;
				}

				PassRefPtr<Control> Composition::GetRelatedControl()
				{
					RefPtr<Composition> _composition = this;
					while (_composition)
					{
						if (_composition->GetAssociatedControl())
						{
							return _composition->GetAssociatedControl();
						}
						else
						{
							_composition = _composition->GetParent();
						}
					}
					return nullptr;
				}

				PassRefPtr<GraphicsHost> Composition::GetRelatedGraphicsHost()
				{
					RefPtr<Composition> _composition = this;
					while (_composition)
					{
						if (_composition->GetAssociatedHost())
						{
							return _composition->GetAssociatedHost();
						}
						else
						{
							_composition = _composition->GetParent();
						}
					}
					return nullptr;
				}

				PassRefPtr<ControlHost> Composition::GetRelatedControlHost()
				{
					RefPtr<Composition> _composition = this;
					while (_composition)
					{
						if (_composition->GetAssociatedControl())
						{
							RefPtr<ControlHost> controlHost = dynamic_cast<ControlHost*>(_composition->GetAssociatedControl().get());
							if (controlHost)
							{
								return controlHost;
							}
						}
						_composition = _composition->GetParent();
					}
					return nullptr;
				}

				PassRefPtr<ICursor> Composition::GetRelatedCursor()
				{
					RefPtr<Composition> _composition = this;
					while (_composition)
					{
						if (_composition->GetAssociatedCursor())
						{
							return _composition->GetAssociatedCursor();
						}
						else
						{
							_composition = _composition->GetParent();
						}
					}
					return nullptr;
				}

				CRect Composition::GetMargin()
				{
					return margin;
				}

				void Composition::SetMargin(CRect value)
				{
					margin = value;
				}

				CRect Composition::GetInternalMargin()
				{
					return internalMargin;
				}

				void Composition::SetInternalMargin(CRect value)
				{
					internalMargin = value;
				}

				CSize Composition::GetPreferredMinSize()
				{
					return preferredMinSize;
				}

				void Composition::SetPreferredMinSize(CSize value)
				{
					preferredMinSize = value;
				}

				CRect Composition::GetClientArea()
				{
					CRect bounds = GetBounds();
					bounds.DeflateRect(margin);
					bounds.DeflateRect(internalMargin);
					return bounds;
				}

				void Composition::ForceCalculateSizeImmediately()
				{
					for (auto & child : children)
					{
						child->ForceCalculateSizeImmediately();
					}
				}

				bool Composition::IsSizeAffectParent()
				{
					return true;
				}

				CSize Composition::GetMinPreferredClientSize()
				{
					CSize minSize;
					if (minSizeLimitation != Composition::NoLimit)
					{
						if (ownedElement)
						{
							RefPtr<IGraphicsRenderer> renderer = ownedElement->GetRenderer();
							if (renderer)
							{
								minSize = renderer->GetMinSize();
							}
						}
					}
					if (minSizeLimitation == Composition::LimitToElementAndChildren)
					{
						for (auto & child : children)
						{
							if (child->IsSizeAffectParent())
							{
								CRect childBounds = child->GetPreferredBounds();
								if (minSize.cx < childBounds.right) minSize.cx = childBounds.right;
								if (minSize.cy < childBounds.bottom) minSize.cy = childBounds.bottom;
							}
						}
					}
					return minSize;
				}

				void Composition::OnControlParentChanged(PassRefPtr<Control> control)
				{
					if (associatedControl && associatedControl != control)
					{
						if (associatedControl->GetParent())
						{
							associatedControl->GetParent()->OnChildRemoved(associatedControl);
						}
						if (control)
						{
							control->OnChildInserted(associatedControl);
						}
					}
					else
					{
						for (auto & child : children)
						{
							child->OnControlParentChanged(control);
						}
					}
				}

				void Composition::OnChildInserted(PassRefPtr<Composition> child)
				{
					child->OnControlParentChanged(GetRelatedControl());
				}

				void Composition::OnChildRemoved(PassRefPtr<Composition> child)
				{
					child->OnControlParentChanged(nullptr);
				}

				void Composition::OnParentChanged(PassRefPtr<Composition> oldParent, PassRefPtr<Composition> newParent)
				{
					OnParentLineChanged();
				}

				void Composition::OnParentLineChanged()
				{
					for (auto & child : children)
					{
						child->OnParentLineChanged();
					}
				}

				void Composition::OnRenderTargetChanged()
				{
					if (associatedControl)
					{
						associatedControl->OnRenderTargetChanged(renderTarget);
					}
				}

				void Composition::SetAssociatedControl(PassRefPtr<Control> control)
				{
					if (associatedControl)
					{
						for (auto & child : children)
						{
							child->OnControlParentChanged(nullptr);
						}
					}
					associatedControl = control;
					if (associatedControl)
					{
						for (auto & child : children)
						{
							child->OnControlParentChanged(associatedControl);
						}
					}
				}

				void Composition::SetAssociatedHost(PassRefPtr<GraphicsHost> host)
				{
					associatedHost = host;
				}


				GraphicsSite::GraphicsSite()
				{
					BoundsChanged.SetAssociatedComposition(this);
				}

				GraphicsSite::~GraphicsSite()
				{

				}

				bool GraphicsSite::IsSizeAffectParent()
				{
					return true;
				}

				CSize GraphicsSite::GetMinPreferredClientSize()
				{
					CSize minSize;
					if (minSizeLimitation != Composition::NoLimit)
					{
						if (ownedElement)
						{
							RefPtr<IGraphicsRenderer> renderer = ownedElement->GetRenderer();
							if (renderer)
							{
								minSize = renderer->GetMinSize();
							}
						}
					}
					if (minSizeLimitation == Composition::LimitToElementAndChildren)
					{
						for (auto & child : children)
						{
							if (child->IsSizeAffectParent())
							{
								CRect childBounds = child->GetPreferredBounds();
								if (minSize.cx < childBounds.right) minSize.cx = childBounds.right;
								if (minSize.cy < childBounds.bottom) minSize.cy = childBounds.bottom;
							}
						}
					}
					return minSize;
				}

				CRect GraphicsSite::GetPreferredBounds()
				{
					return GetBoundsInternal(CRect(CPoint(), GetMinPreferredClientSize()));
				}

				CRect GraphicsSite::GetBoundsInternal(CRect expectedBounds)
				{
					CSize minSize = GetMinPreferredClientSize();
					if (minSize.cx < preferredMinSize.cx) minSize.cx = preferredMinSize.cx;
					if (minSize.cy < preferredMinSize.cy) minSize.cy = preferredMinSize.cy;

					minSize.cx += margin.left + margin.right + internalMargin.left + internalMargin.right;
					minSize.cy += margin.top + margin.bottom + internalMargin.top + internalMargin.bottom;
					cint w = expectedBounds.Width();
					cint h = expectedBounds.Height();
					if (minSize.cx < w) minSize.cx = w;
					if (minSize.cy < h) minSize.cy = h;
					return CRect(expectedBounds.TopLeft(), minSize);
				}

				void GraphicsSite::UpdatePreviousBounds(CRect bounds)
				{
					if (previousBounds != bounds)
					{
						previousBounds = bounds;
						EventArgs arguments(this);
						BoundsChanged.Execute(arguments);
					}
				}

				WindowComposition::WindowComposition()
				{

				}

				WindowComposition::~WindowComposition()
				{

				}

				PassRefPtr<IWindow> WindowComposition::GetAttachedWindow()
				{
					return attachedWindow;
				}

				void WindowComposition::SetAttachedWindow(PassRefPtr<IWindow> window)
				{
					attachedWindow = window;
					SetRenderTarget(attachedWindow ? GetStorage()->GetGraphicsResourceManager()->GetRenderTarget(attachedWindow) : nullptr);
				}

				CRect WindowComposition::GetBounds()
				{
					return attachedWindow ? CRect(CPoint(), attachedWindow->GetClientSize()) : CRect();
				}

				void WindowComposition::SetMargin(CRect value)
				{

				}

				BoundsComposition::BoundsComposition()
				{
					ClearAlignmentToParent();
				}

				BoundsComposition::~BoundsComposition()
				{

				}

				CRect BoundsComposition::GetPreferredBounds()
				{
					CRect result = GetBoundsInternal(compositionBounds);
					if (GetParent() && IsAlignedToParent())
					{
						if (alignmentToParent.left >= 0)
						{
							cint offset = alignmentToParent.left - result.left;
							result.left += offset;
							result.right += offset;
						}
						if (alignmentToParent.top >= 0)
						{
							cint offset = alignmentToParent.top - result.top;
							result.top += offset;
							result.bottom += offset;
						}
						if (alignmentToParent.right >= 0)
						{
							result.right += alignmentToParent.right;
						}
						if (alignmentToParent.bottom >= 0)
						{
							result.bottom += alignmentToParent.bottom;
						}
					}
					return result;
				}

				CRect BoundsComposition::GetBounds()
				{
					CRect result = GetPreferredBounds();
					if (GetParent() && IsAlignedToParent())
					{
						CSize clientSize = GetParent()->GetClientArea().Size();
						if (alignmentToParent.left >= 0 && alignmentToParent.right >= 0)
						{
							result.left = alignmentToParent.left;
							result.right = clientSize.cx - alignmentToParent.right;
						}
						else if (alignmentToParent.left >= 0)
						{
							cint width = result.Width();
							result.left = alignmentToParent.left;
							result.right = result.left + width;
						}
						else if (alignmentToParent.right >= 0)
						{
							cint width = result.Width();
							result.right = clientSize.cx - alignmentToParent.right;
							result.left = result.right - width;
						}

						if (alignmentToParent.top >= 0 && alignmentToParent.bottom >= 0)
						{
							result.top = alignmentToParent.top;
							result.bottom = clientSize.cy - alignmentToParent.bottom;
						}
						else if (alignmentToParent.top >= 0)
						{
							cint height = result.Height();
							result.top = alignmentToParent.top;
							result.bottom = result.top + height;
						}
						else if (alignmentToParent.bottom >= 0)
						{
							cint height = result.Height();
							result.bottom = clientSize.cy - alignmentToParent.bottom;
							result.top = result.bottom - height;
						}
					}
					UpdatePreviousBounds(result);
					return result;
				}

				void BoundsComposition::SetBounds(CRect value)
				{
					compositionBounds = value;
				}

				void BoundsComposition::ClearAlignmentToParent()
				{
					alignmentToParent = CRect(-1, -1, -1, -1);
				}

				CRect BoundsComposition::GetAlignmentToParent()
				{
					return alignmentToParent;
				}

				void BoundsComposition::SetAlignmentToParent(CRect value)
				{
					alignmentToParent = value;
				}

				bool BoundsComposition::IsAlignedToParent()
				{
					return (alignmentToParent != CRect(-1, -1, -1, -1)) == TRUE;
				}

				TableCellComposition::TableCellComposition()
					: row(-1)
					, column(-1)
					, rowSpan(1)
					, columnSpan(1)
				{
					SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				}

				TableCellComposition::~TableCellComposition()
				{

				}

				PassRefPtr<TableComposition> TableCellComposition::GetTableParent()
				{
					return tableParent;
				}

				cint TableCellComposition::GetRow()
				{
					return row;
				}

				cint TableCellComposition::GetRowSpan()
				{
					return rowSpan;
				}

				cint TableCellComposition::GetColumn()
				{
					return column;
				}

				cint TableCellComposition::GetColumnSpan()
				{
					return columnSpan;
				}

				bool TableCellComposition::SetSite(cint _row, cint _column, cint _rowSpan, cint _columnSpan)
				{
					if (SetSiteInternal(_row, _column, _rowSpan, _columnSpan))
					{
						if (tableParent)
						{
							tableParent->UpdateCellBounds();
						}
						return true;
					}
					else
					{
						return false;
					}
				}

				CRect TableCellComposition::GetBounds()
				{
					CRect result;
					if (tableParent && row != -1 && column != -1)
					{
						CRect bounds1, bounds2;
						{
							cint index = tableParent->GetSiteIndex(tableParent->rows, tableParent->columns, row, column);
							bounds1 = tableParent->cellBounds[index];
						}
						{
							cint index = tableParent->GetSiteIndex(tableParent->rows, tableParent->columns, row + rowSpan - 1, column + columnSpan - 1);
							bounds2 = tableParent->cellBounds[index];
							if (tableParent->GetMinSizeLimitation() == Composition::NoLimit)
							{
								if (row + rowSpan == tableParent->rows)
								{
									bounds2.bottom += tableParent->rowExtending;
								}
								if (column + columnSpan == tableParent->columns)
								{
									bounds2.right += tableParent->columnExtending;
								}
							}
						}
						auto t = bounds2.BottomRight();
						result = CRect(bounds1.TopLeft(), bounds2.BottomRight());
					}
					else
					{
						result = CRect();
					}
					UpdatePreviousBounds(result);
					return result;
				}

				void TableCellComposition::ClearSitedCells(PassRefPtr<TableComposition> table)
				{
					if (row != -1 && column != -1)
					{
						for (cint r = 0; r < rowSpan; r++)
						{
							for (cint c = 0; c < columnSpan; c++)
							{
								table->SetSitedCell(row + r, column + c, nullptr);
							}
						}
					}
				}

				void TableCellComposition::SetSitedCells(PassRefPtr<TableComposition> table)
				{
					for (cint r = 0; r < rowSpan; r++)
					{
						for (cint c = 0; c < columnSpan; c++)
						{
							table->SetSitedCell(row + r, column + c, this);
						}
					}
				}

				void TableCellComposition::ResetSiteInternal()
				{
					row = -1;
					column = -1;
					rowSpan = 1;
					columnSpan = 1;
				}

				bool TableCellComposition::SetSiteInternal(cint _row, cint _column, cint _rowSpan, cint _columnSpan)
				{
					if (tableParent)
					{
						if (_row < 0 || _row >= tableParent->rows || _column < 0 || _column >= tableParent->columns) return false;
						if (_rowSpan<1 || _row + _rowSpan>tableParent->rows || _columnSpan<1 || _column + _columnSpan>tableParent->columns) return false;

						for (cint r = 0; r < _rowSpan; r++)
						{
							for (cint c = 0; c < _columnSpan; c++)
							{
								RefPtr<TableCellComposition> cell = tableParent->GetSitedCell(_row + r, _column + c);
								if (cell && cell != this)
								{
									return false;
								}
							}
						}
						ClearSitedCells(tableParent);
					}

					row = _row;
					column = _column;
					rowSpan = _rowSpan;
					columnSpan = _columnSpan;

					if (tableParent)
					{
						SetSitedCells(tableParent);
					}
					return true;
				}

				void TableCellComposition::OnParentChanged(PassRefPtr<Composition> oldParent, PassRefPtr<Composition> newParent)
				{
					RefPtr<Composition> _oldParent = oldParent;
					RefPtr<Composition> _newParent = newParent;
					GraphicsSite::OnParentChanged(_oldParent, _newParent);
					if (tableParent)
					{
						ClearSitedCells(tableParent);
					}
					tableParent = dynamic_cast<TableComposition*>(_newParent.get());
					if (!tableParent || !SetSiteInternal(row, column, rowSpan, columnSpan))
					{
						ResetSiteInternal();
					}
					if (tableParent)
					{
						if (row != -1 && column != -1)
						{
							SetSiteInternal(row, column, rowSpan, columnSpan);
						}
						tableParent->UpdateCellBounds();
					}
				}

				void TableCellComposition::OnTableRowsAndColumnsChanged()
				{
					if (!SetSiteInternal(row, column, rowSpan, columnSpan))
					{
						ResetSiteInternal();
					}
				}

				TableComposition::TableComposition()
					: rows(0)
					, columns(0)
					, cellPadding(0)
					, rowExtending(0)
					, columnExtending(0)
				{
					SetRowsAndColumns(1, 1);
				}

				TableComposition::~TableComposition()
				{
				}

				cint TableComposition::GetRows()
				{
					return rows;
				}

				cint TableComposition::GetColumns()
				{
					return columns;
				}

				bool TableComposition::SetRowsAndColumns(cint _rows, cint _columns)
				{
					if (_rows <= 0 || _columns <= 0) return false;
					rowOptions.resize(_rows);
					columnOptions.resize(_columns);
					cellCompositions.resize(_rows * _columns);
					cellBounds.resize(_rows * _columns);
					for (cint i = 0; i < _rows * _columns; i++)
					{
						cellCompositions[i] = nullptr;
						cellBounds[i] = CRect();
					}
					rows = _rows;
					columns = _columns;
					for (auto & child : Children())
					{
						RefPtr<TableCellComposition> cell = dynamic_cast<TableCellComposition*>(child.get());
						if (cell)
						{
							cell->OnTableRowsAndColumnsChanged();
						}
					}
					UpdateCellBounds();
					return true;
				}

				PassRefPtr<TableCellComposition> TableComposition::GetSitedCell(cint _row, cint _column)
				{
					return cellCompositions[GetSiteIndex(rows, columns, _row, _column)];
				}

				CellOption TableComposition::GetRowOption(cint _row)
				{
					return rowOptions[_row];
				}

				void TableComposition::SetRowOption(cint _row, CellOption option)
				{
					rowOptions[_row] = option;
				}

				CellOption TableComposition::GetColumnOption(cint _column)
				{
					return columnOptions[_column];
				}

				void TableComposition::SetColumnOption(cint _column, CellOption option)
				{
					columnOptions[_column] = option;
				}

				cint TableComposition::GetCellPadding()
				{
					return cellPadding;
				}

				void TableComposition::SetCellPadding(cint value)
				{
					if (value < 0) value = 0;
					cellPadding = value;
				}

				CRect TableComposition::GetCellArea()
				{
					CRect bounds(CPoint(), BoundsComposition::GetBounds().Size());
					bounds.DeflateRect(margin);
					bounds.DeflateRect(internalMargin);
					bounds.DeflateRect(CSize(cellPadding, cellPadding));
					if (bounds.right < bounds.left) bounds.right = bounds.left;
					if (bounds.bottom < bounds.top) bounds.bottom = bounds.top;
					return bounds;
				}

				void TableComposition::UpdateCellBounds()
				{
					UpdateCellBoundsInternal();
					UpdateTableContentMinSize();
				}

				void TableComposition::ForceCalculateSizeImmediately()
				{
					BoundsComposition::ForceCalculateSizeImmediately();
					UpdateCellBounds();
				}

				CSize TableComposition::GetMinPreferredClientSize()
				{
					return tableContentMinSize;
				}

				CRect TableComposition::GetBounds()
				{
					CRect result;
					if (!IsAlignedToParent() && GetMinSizeLimitation() != Composition::NoLimit)
					{
						result = CRect(compositionBounds.TopLeft(), compositionBounds.Size() - CSize(columnExtending, rowExtending));
					}
					else
					{
						result = BoundsComposition::GetBounds();
					}

					bool cellMinSizeModified = false;
					decltype(cellCompositions) cells = cellCompositions;
					sort(cells.begin(), cells.end());
					cells.erase(unique(cells.begin(), cells.end()), cells.end());
					for (auto & cell : cells)
					{
						if (cell)
						{
							CSize newSize = cell->GetPreferredBounds().Size();
							if (cell->lastPreferredSize != newSize)
							{
								cell->lastPreferredSize = newSize;
								cellMinSizeModified = true;
							}
						}
					}

					if (previousBounds != result || cellMinSizeModified)
					{
						previousBounds = result;
						UpdateCellBounds();
					}
					return result;
				}

				cint TableComposition::GetSiteIndex(cint _rows, cint _columns, cint _row, cint _column)
				{
					return _row * _columns + _column;
				}

				void TableComposition::SetSitedCell(cint _row, cint _column, PassRefPtr<TableCellComposition> cell)
				{
					cellCompositions[GetSiteIndex(rows, columns, _row, _column)] = cell.get();
				}

				void TableComposition::UpdateCellBoundsInternal(
					vector<cint>& dimSizes,
					cint& dimSize,
					cint& dimSizeWithPercentage,
					vector<CellOption>& dimOptions,
					cint TableComposition::* dim1,
					cint TableComposition::* dim2,
					cint(*getSize)(CSize),
					cint(*getLocation)(PassRefPtr<TableCellComposition>),
					cint(*getSpan)(PassRefPtr<TableCellComposition>),
					cint(*getRow)(cint, cint),
					cint(*getCol)(cint, cint),
					cint maxPass
					)
				{
					for (cint pass = 0; pass < maxPass; pass++)
					{
						for (cint i = 0; i < this->*dim1; i++)
						{
							CellOption option = dimOptions[i];
							if (pass == 0)
							{
								dimSizes[i] = 0;
							}
							switch (option.composeType)
							{
								case CellOption::Absolute:
								{
									dimSizes[i] = option.absolute;
								}
									break;
								case CellOption::MinSize:
								{
									for (cint j = 0; j < this->*dim2; j++)
									{
										RefPtr<TableCellComposition> cell = GetSitedCell(getRow(i, j), getCol(i, j));
										if (cell)
										{
											bool accept = false;
											if (pass == 0)
											{
												accept = getSpan(cell) == 1;
											}
											else
											{
												accept = getLocation(cell) + getSpan(cell) == i + 1;
											}
											if (accept)
											{
												cint size = getSize(cell->GetPreferredBounds().Size());
												cint span = getSpan(cell);
												for (cint k = 1; k < span; k++)
												{
													size -= dimSizes[i - k] + cellPadding;
												}
												if (dimSizes[i] < size)
												{
													dimSizes[i] = size;
												}
											}
										}
									}
								}
									break;
							}
						}
					}

					bool percentageExists = false;
					for (cint i = 0; i < this->*dim1; i++)
					{
						CellOption option = dimOptions[i];
						if (option.composeType == CellOption::Percentage)
						{
							if (0.001 < option.percentage)
							{
								percentageExists = true;
							}
						}
					}

					if (percentageExists)
					{
						for (cint i = 0; i < this->*dim1; i++)
						{
							CellOption option = dimOptions[i];
							if (option.composeType == CellOption::Percentage)
							{
								if (0.001 < option.percentage)
								{
									for (cint j = 0; j < this->*dim2; j++)
									{
										RefPtr<TableCellComposition> cell = GetSitedCell(getRow(i, j), getCol(i, j));
										if (cell)
										{
											cint size = getSize(cell->GetPreferredBounds().Size());
											cint start = getLocation(cell);
											cint span = getSpan(cell);
											size -= (span - 1)*cellPadding;
											double totalPercentage = 0;

											for (cint k = start; k < start + span; k++)
											{
												if (dimOptions[k].composeType == CellOption::Percentage)
												{
													if (0.001 < dimOptions[k].percentage)
													{
														totalPercentage += dimOptions[k].percentage;
													}
												}
												else
												{
													size -= dimSizes[k];
												}
											}

											size = (cint)ceil(size*option.percentage / totalPercentage);
											if (dimSizes[i] < size)
											{
												dimSizes[i] = size;
											}
										}
									}
								}
							}
						}

						cint percentageTotalSize = 0;
						for (cint i = 0; i < this->*dim1; i++)
						{
							CellOption option = dimOptions[i];
							if (option.composeType == CellOption::Percentage)
							{
								if (0.001 < option.percentage)
								{
									cint size = (cint)ceil(dimSizes[i] / option.percentage);
									if (percentageTotalSize < size)
									{
										percentageTotalSize = size;
									}
								}
							}
						}

						double totalPercentage = 0;
						for (cint i = 0; i < this->*dim1; i++)
						{
							CellOption option = dimOptions[i];
							if (option.composeType == CellOption::Percentage)
							{
								if (0.001 < option.percentage)
								{
									totalPercentage += option.percentage;
								}
							}
						}

						for (cint i = 0; i < this->*dim1; i++)
						{
							CellOption option = dimOptions[i];
							if (option.composeType == CellOption::Percentage)
							{
								if (0.001 < option.percentage)
								{
									cint size = (cint)ceil(percentageTotalSize*option.percentage / totalPercentage);
									if (dimSizes[i] < size)
									{
										dimSizes[i] = size;
									}
								}
							}
						}
					}

					for (cint i = 0; i < this->*dim1; i++)
					{
						if (dimOptions[i].composeType != CellOption::Percentage)
						{
							dimSize += dimSizes[i];
						}
						dimSizeWithPercentage += dimSizes[i];
					}
				}

				void TableComposition::UpdateCellBoundsPercentages(
					vector<cint>& dimSizes,
					cint dimSize,
					cint maxDimSize,
					vector<CellOption>& dimOptions
					)
				{
					if (maxDimSize > dimSize)
					{
						double totalPercentage = 0;
						cint percentageCount = 0;
						for (auto & dimOption : dimOptions)
						{
							CellOption & option = dimOption;
							if (option.composeType == CellOption::Percentage)
							{
								totalPercentage += option.percentage;
								percentageCount++;
							}
						}
						if (percentageCount>0 && totalPercentage > 0.001)
						{
							for (cuint i = 0; i < dimOptions.size(); i++)
							{
								CellOption option = dimOptions[i];
								if (option.composeType == CellOption::Percentage)
								{
									dimSizes[i] = (cint)((maxDimSize - dimSize)*option.percentage / totalPercentage);
								}
							}
						}
					}
				}

				cint TableComposition::UpdateCellBoundsOffsets(
					vector<cint>& offsets,
					vector<cint>& sizes,
					cint start,
					cint max
					)
				{
					offsets[0] = start;
					for (cuint i = 1; i < offsets.size(); i++)
					{
						start += cellPadding + sizes[i - 1];
						offsets[i] = start;
					}

					cint last = offsets.size() - 1;
					cint right = offsets[last] + sizes[last];
					return max - right;
				}

				void TableComposition::UpdateCellBoundsInternal()
				{
					vector<cint> rowOffsets, columnOffsets, rowSizes, columnSizes;
					rowOffsets.resize(rows);
					rowSizes.resize(rows);
					columnOffsets.resize(columns);
					columnSizes.resize(columns);
					{
						cint rowTotal = (rows - 1)*cellPadding;
						cint columnTotal = (columns - 1)*cellPadding;
						cint rowTotalWithPercentage = rowTotal;
						cint columnTotalWithPercentage = columnTotal;

						UpdateCellBoundsInternal(
							rowSizes,
							rowTotal,
							rowTotalWithPercentage,
							rowOptions,
							&TableComposition::rows,
							&TableComposition::columns,
							&Y,
							&RL,
							&RS,
							&First,
							&Second,
							1
							);
						UpdateCellBoundsInternal(
							columnSizes,
							columnTotal,
							columnTotalWithPercentage,
							columnOptions,
							&TableComposition::columns,
							&TableComposition::rows,
							&X,
							&CL,
							&CS,
							&Second,
							&First,
							1
							);

						CRect area = GetCellArea();
						UpdateCellBoundsPercentages(rowSizes, rowTotal, area.Height(), rowOptions);
						UpdateCellBoundsPercentages(columnSizes, columnTotal, area.Width(), columnOptions);
						rowExtending = UpdateCellBoundsOffsets(rowOffsets, rowSizes, cellPadding, cellPadding + area.Height());
						columnExtending = UpdateCellBoundsOffsets(columnOffsets, columnSizes, cellPadding, cellPadding + area.Width());

						for (cint i = 0; i < rows; i++)
						{
							for (cint j = 0; j < columns; j++)
							{
								cint index = GetSiteIndex(rows, columns, i, j);
								cellBounds[index] = CRect(CPoint(columnOffsets[j], rowOffsets[i]), CSize(columnSizes[j], rowSizes[i]));
							}
						}
					}
				}

				void TableComposition::UpdateTableContentMinSize()
				{
					vector<cint> rowSizes, columnSizes;
					rowSizes.resize(rows);
					columnSizes.resize(columns);
					{
						cint rowTotal = (rows + 1)*cellPadding;
						cint columnTotal = (columns + 1)*cellPadding;
						cint rowTotalWithPercentage = rowTotal;
						cint columnTotalWithPercentage = columnTotal;

						UpdateCellBoundsInternal(
							rowSizes,
							rowTotal,
							rowTotalWithPercentage,
							rowOptions,
							&TableComposition::rows,
							&TableComposition::columns,
							&Y,
							&RL,
							&RS,
							&First,
							&Second,
							2
							);
						UpdateCellBoundsInternal(
							columnSizes,
							columnTotal,
							columnTotalWithPercentage,
							columnOptions,
							&TableComposition::columns,
							&TableComposition::rows,
							&X,
							&CL,
							&CS,
							&Second,
							&First,
							2
							);
						tableContentMinSize = CSize(columnTotalWithPercentage, rowTotalWithPercentage);
					}
					if (previousContentMinSize != tableContentMinSize)
					{
						previousContentMinSize = tableContentMinSize;
						UpdateCellBoundsInternal();
					}
				}

				void TableComposition::OnRenderTargetChanged()
				{
					if (GetRenderTarget())
					{
						UpdateTableContentMinSize();
					}
				}

				cint TableComposition::First(cint a, cint b)
				{
					return a;
				}

				cint TableComposition::Second(cint a, cint b)
				{
					return b;
				}

				cint TableComposition::X(CSize s)
				{
					return s.cx;
				}

				cint TableComposition::Y(CSize s)
				{
					return s.cy;
				}

				cint TableComposition::RL(PassRefPtr<TableCellComposition> cell)
				{
					return cell->GetRow();
				}

				cint TableComposition::CL(PassRefPtr<TableCellComposition> cell)
				{
					return cell->GetColumn();
				}

				cint TableComposition::RS(PassRefPtr<TableCellComposition> cell)
				{
					return cell->GetRowSpan();
				}

				cint TableComposition::CS(PassRefPtr<TableCellComposition> cell)
				{
					return cell->GetColumnSpan();
				}

				StackItemComposition::StackItemComposition()
				{
					SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				}

				StackItemComposition::~StackItemComposition()
				{

				}

				bool StackItemComposition::IsSizeAffectParent()
				{
					return false;
				}

				CRect StackItemComposition::GetBounds()
				{
					CRect result = bounds;
					if (stackParent)
					{
						auto& stackItems = stackParent->stackItems;
						cuint index;
						for (index = 0; index < stackItems.size(); index++)
						{
							if (stackItems[index] == this)
								break;
						}
						if (index != stackItems.size())
						{
							if (stackParent->stackItemBounds.size() != stackParent->stackItems.size())
							{
								stackParent->UpdateStackItemBounds();
							}
							result = stackParent->stackItemBounds[index];
						}
					}
					result.InflateRect(extraMargin);
					UpdatePreviousBounds(result);
					return result;
				}

				void StackItemComposition::SetBounds(CRect value)
				{
					bounds = value;
				}

				CRect StackItemComposition::GetExtraMargin()
				{
					return extraMargin;
				}

				void StackItemComposition::SetExtraMargin(CRect value)
				{
					extraMargin = value;
				}

				void StackItemComposition::OnParentChanged(PassRefPtr<Composition> oldParent, PassRefPtr<Composition> newParent)
				{
					GraphicsSite::OnParentChanged(oldParent.get(), newParent.get());
					stackParent = newParent == nullptr ? nullptr : dynamic_cast<StackComposition*>(newParent.get());
				}

				CSize StackItemComposition::GetMinSize()
				{
					return GetBoundsInternal(bounds).Size();
				}


				StackComposition::StackComposition()
					: direction(Horizontal)
					, padding(0)
				{
					BoundsChanged.AttachMethod(this, &StackComposition::OnBoundsChanged);
				}

				StackComposition::~StackComposition()
				{

				}

				const StackComposition::StackItemCompositionList& StackComposition::GetStackItems()
				{
					return stackItems;
				}

				PassRefPtr<StackItemComposition> StackComposition::GetStackItem(cint index)
				{
					return stackItems[index];
				}

				bool StackComposition::InsertStackItem(cint index, PassRefPtr<StackItemComposition> item)
				{
					if (!AddChild(item.get()))
						return false;
					return true;
				}

				StackComposition::Direction StackComposition::GetDirection()
				{
					return direction;
				}

				void StackComposition::SetDirection(Direction value)
				{
					direction = value;
				}

				cint StackComposition::GetPadding()
				{
					return padding;
				}

				void StackComposition::SetPadding(cint value)
				{
					padding = value;
				}

				CSize StackComposition::GetMinPreferredClientSize()
				{
					CSize minSize = BoundsComposition::GetMinPreferredClientSize();
					UpdateStackItemBounds();
					if (GetMinSizeLimitation() == Composition::LimitToElementAndChildren)
					{
						if (!ensuringVisibleStackItem || direction == Vertical)
						{
							if (minSize.cx < stackItemTotalSize.cx) minSize.cx = stackItemTotalSize.cx;
						}
						if (!ensuringVisibleStackItem || direction == Horizontal)
						{
							if (minSize.cy < stackItemTotalSize.cy) minSize.cy = stackItemTotalSize.cy;
						}
					}
					cint x = 0;
					cint y = 0;
					if (extraMargin.left > 0) x += extraMargin.left;
					if (extraMargin.right > 0) x += extraMargin.right;
					if (extraMargin.top > 0) y += extraMargin.top;
					if (extraMargin.bottom > 0) y += extraMargin.bottom;
					return minSize + CSize(x, y);
				}

				CRect StackComposition::GetBounds()
				{
					CRect bounds = BoundsComposition::GetBounds();
					previousBounds = bounds;
					FixStackItemSizes();
					return bounds;
				}

				CRect StackComposition::GetExtraMargin()
				{
					return extraMargin;
				}

				void StackComposition::SetExtraMargin(CRect value)
				{
					extraMargin = value;
				}

				bool StackComposition::IsStackItemClipped()
				{
					CRect clientArea = GetClientArea();
					for (auto & stackItem : stackItems)
					{
						CRect& stackItemBounds = stackItem->GetBounds();
						switch (direction)
						{
							case Horizontal:
							{
								if (stackItemBounds.left < 0 || stackItemBounds.right >= clientArea.Width())
								{
									return true;
								}
							}
								break;
							case Vertical:
							{
								if (stackItemBounds.top < 0 || stackItemBounds.bottom >= clientArea.Height())
								{
									return true;
								}
							}
								break;
						}
					}
					return false;
				}

				bool StackComposition::EnsureVisible(cuint index)
				{
					if (0 <= index && index < stackItems.size())
					{
						ensuringVisibleStackItem = stackItems[index];
					}
					else
					{
						ensuringVisibleStackItem = nullptr;
					}
					UpdateStackItemBounds();
					return ensuringVisibleStackItem != nullptr;
				}

				void StackComposition::UpdateStackItemBounds()
				{
					if (stackItemBounds.size() != stackItems.size())
					{
						stackItemBounds.resize(stackItems.size());
					}

					stackItemTotalSize = CSize(0, 0);
					cint x = extraMargin.left ? extraMargin.left : 0;
					cint y = extraMargin.top ? extraMargin.top : 0;
					switch (direction)
					{
						case StackComposition::Horizontal:
						{
							for (cuint i = 0; i < stackItems.size(); i++)
							{
								CSize itemSize = stackItems[i]->GetMinSize();
								if (i > 0) stackItemTotalSize.cx += padding;
								if (stackItemTotalSize.cy < itemSize.cy) stackItemTotalSize.cy = itemSize.cy;
								stackItemBounds[i] = CRect(CPoint(stackItemTotalSize.cx + x, y), CSize(itemSize.cx, 0));
								stackItemTotalSize.cx += itemSize.cx;
							}
						}
							break;
						case StackComposition::Vertical:
						{
							for (cuint i = 0; i<stackItems.size(); i++)
							{
								CSize itemSize = stackItems[i]->GetMinSize();
								if (i > 0) stackItemTotalSize.cy += padding;
								if (stackItemTotalSize.cx < itemSize.cx) stackItemTotalSize.cx = itemSize.cx;
								stackItemBounds[i] = CRect(CPoint(x, stackItemTotalSize.cy + y), CSize(0, itemSize.cy));
								stackItemTotalSize.cy += itemSize.cy;
							}
						}
							break;
					}

					FixStackItemSizes();
				}

				void StackComposition::FixStackItemSizes()
				{
					switch (direction)
					{
						case Horizontal:
						{
							cint y = 0;
							if (extraMargin.top > 0) y += extraMargin.top;
							if (extraMargin.bottom > 0) y += extraMargin.bottom;

							for (auto & stackItemBound : stackItemBounds)
							{
								stackItemBound.bottom = stackItemBound.top + previousBounds.Height() - y;
							}
						}
							break;
						case Vertical:
						{
							cint x = 0;
							if (extraMargin.left > 0) x += extraMargin.left;
							if (extraMargin.right > 0) x += extraMargin.right;

							for (auto & stackItemBound : stackItemBounds)
							{
								stackItemBound.right = stackItemBound.left + previousBounds.Width() - x;
							}
						}
							break;
					}

					cuint index;
					for (index = 0; index < stackItems.size(); index++)
					{
						if (stackItems[index] == ensuringVisibleStackItem)
							break;
					}
					if (index != stackItems.size())
					{
						CRect itemBounds = stackItemBounds[index];
						CSize size = previousBounds.Size();
						CSize offset;
						switch (direction)
						{
							case Horizontal:
							{
								if (itemBounds.left <= 0)
								{
									offset.cx = -itemBounds.left;
								}
								else if (itemBounds.right >= size.cx)
								{
									offset.cx = size.cx - itemBounds.right;
								}
							}
								break;
							case Vertical:
							{
								if (itemBounds.top <= 0)
								{
									offset.cy = -itemBounds.top;
								}
								else if (itemBounds.bottom >= size.cy)
								{
									offset.cy = size.cy - itemBounds.bottom;
								}
							}
								break;
						}
						for (auto & stackItemBound : stackItemBounds)
						{
							stackItemBound.OffsetRect(offset);
						}
					}
				}

				void StackComposition::OnBoundsChanged(PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					UpdateStackItemBounds();
				}

				void StackComposition::OnChildInserted(PassRefPtr<Composition> child)
				{
					BoundsComposition::OnChildInserted(child.get());
					RefPtr<StackItemComposition> item = dynamic_cast<StackItemComposition*>(child.get());
					if (item && find(stackItems.begin(), stackItems.end(), item) == stackItems.end())
					{
						stackItems.push_back(item);
					}
				}

				void StackComposition::OnChildRemoved(PassRefPtr<Composition> child)
				{
					BoundsComposition::OnChildRemoved(child.get());
					RefPtr<StackItemComposition> item = dynamic_cast<StackItemComposition*>(child.get());
					if (item)
					{
						stackItems.erase(find(stackItems.begin(), stackItems.end(), item));
						if (item == ensuringVisibleStackItem)
						{
							ensuringVisibleStackItem = nullptr;
							UpdateStackItemBounds();
						}
					}
				}

				SideAlignedComposition::SideAlignedComposition()
					: direction(Top)
					, maxLength(10)
					, maxRatio(1.0)
				{

				}

				SideAlignedComposition::~SideAlignedComposition()
				{

				}

				void SideAlignedComposition::SetDirection(Direction value)
				{
					direction = value;
				}

				cint SideAlignedComposition::GetMaxLength()
				{
					return maxLength;
				}

				void SideAlignedComposition::SetMaxLength(cint value)
				{
					if (value < 0) value = 0;
					maxLength = value;
				}

				double SideAlignedComposition::GetMaxRatio()
				{
					return maxRatio;
				}

				void SideAlignedComposition::SetMaxRatio(double value)
				{
					maxRatio =
						value < 0 ? 0 :
						value > 1 ? 1 :
						value;
				}

				bool SideAlignedComposition::IsSizeAffectParent()
				{
					return false;
				}

				CRect SideAlignedComposition::GetBounds()
				{
					CRect result;
					RefPtr<Composition> parent = GetParent();
					if (parent)
					{
						CRect bounds = parent->GetBounds();
						cint w = (cint)(bounds.Width()*maxRatio);
						cint h = (cint)(bounds.Height()*maxRatio);
						if (w > maxLength) w = maxLength;
						if (h > maxLength) h = maxLength;
						switch (direction)
						{
							case Left:
							{
								bounds.right = bounds.left + w;
							}
								break;
							case Top:
							{
								bounds.bottom = bounds.top + h;
							}
								break;
							case Right:
							{
								bounds.left = bounds.right - w;
							}
								break;
							case Bottom:
							{
								bounds.top = bounds.bottom - h;
							}
								break;
						}
						result = bounds;
					}
					UpdatePreviousBounds(result);
					return result;
				}

				PartialViewComposition::PartialViewComposition()
					: wRatio(0.0)
					, wPageSize(1.0)
					, hRatio(0.0)
					, hPageSize(1.0)
				{

				}

				PartialViewComposition::~PartialViewComposition()
				{

				}

				double PartialViewComposition::GetWidthRatio()
				{
					return wRatio;
				}

				double PartialViewComposition::GetWidthPageSize()
				{
					return wPageSize;
				}

				double PartialViewComposition::GetHeightRatio()
				{
					return hRatio;
				}

				double PartialViewComposition::GetHeightPageSize()
				{
					return hPageSize;
				}

				void PartialViewComposition::SetWidthRatio(double value)
				{
					wRatio = value;
				}

				void PartialViewComposition::SetWidthPageSize(double value)
				{
					wPageSize = value;
				}

				void PartialViewComposition::SetHeightRatio(double value)
				{
					hRatio = value;
				}

				void PartialViewComposition::SetHeightPageSize(double value)
				{
					hPageSize = value;
				}

				bool PartialViewComposition::IsSizeAffectParent()
				{
					return false;
				}

				CRect PartialViewComposition::GetBounds()
				{
					CRect result;
					RefPtr<Composition> parent = GetParent();
					if (parent)
					{
						CRect bounds = parent->GetBounds();
						cint w = bounds.Width();
						cint h = bounds.Height();
						cint pw = (cint)(wPageSize * w);
						cint ph = (cint)(hPageSize * h);

						cint ow = preferredMinSize.cx - pw;
						if (ow < 0) ow = 0;
						cint oh = preferredMinSize.cy - ph;
						if (oh < 0) oh = 0;

						w -= ow;
						h -= oh;
						pw += ow;
						ph += oh;

						result = CRect(CPoint((cint)(wRatio * w), (cint)(hRatio * h)), CSize(pw, ph));
					}
					UpdatePreviousBounds(result);
					return result;
				}

				SharedSizeItemComposition::SharedSizeItemComposition()
					: sharedWidth(false)
					, sharedHeight(false)
				{
					SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				}

				SharedSizeItemComposition::~SharedSizeItemComposition()
				{

				}

				const CString& SharedSizeItemComposition::GetGroup()
				{
					return group;
				}

				void SharedSizeItemComposition::SetGroup(const CString& value)
				{
					if (group != value)
					{
						group = value;
						Update();
					}
				}

				bool SharedSizeItemComposition::GetSharedWidth()
				{
					return sharedWidth;
				}

				void SharedSizeItemComposition::SetSharedWidth(bool value)
				{
					if (sharedWidth != value)
					{
						sharedWidth = value;
						Update();
					}
				}

				bool SharedSizeItemComposition::GetSharedHeight()
				{
					return sharedHeight;
				}

				void SharedSizeItemComposition::SetSharedHeight(bool value)
				{
					if (sharedHeight != value)
					{
						sharedHeight = value;
						Update();
					}
				}

				void SharedSizeItemComposition::Update()
				{
					if (parentRoot)
					{
						parentRoot->ForceCalculateSizeImmediately();
					}
				}

				void SharedSizeItemComposition::OnParentLineChanged()
				{
					BoundsComposition::OnParentLineChanged();
					if (parentRoot)
					{
						auto & childItems = parentRoot->childItems;
						childItems.erase(find(childItems.begin(), childItems.end(), this));
						parentRoot = nullptr;
					}

					RefPtr<Composition> current = GetParent();
					while (current)
					{
						if (auto item = dynamic_cast<SharedSizeItemComposition*>(current.get()))
						{
							break;
						}
						else if (auto root = dynamic_cast<SharedSizeComposition*>(current.get()))
						{
							parentRoot = root;
							break;
						}
						current = current->GetParent();
					}

					if (parentRoot)
					{
						parentRoot->childItems.push_back(this);
					}
				}

				SharedSizeComposition::SharedSizeComposition()
				{

				}

				SharedSizeComposition::~SharedSizeComposition()
				{

				}

				void SharedSizeComposition::AddSizeComponent(map<CString, cint>& sizes, const CString& group, cint sizeComponent)
				{
					auto index = sizes.find(group);
					if (index == sizes.end())
					{
						sizes.insert(make_pair(group, sizeComponent));
					}
					else if (index->second < sizeComponent)
					{
						index->second = sizeComponent;
					}
				}

				void SharedSizeComposition::ForceCalculateSizeImmediately()
				{
					map<CString, cint> widths, heights;

					for (auto & item : childItems)
					{
						auto group = item->GetGroup();
						auto minSize = item->GetPreferredMinSize();
						item->SetPreferredMinSize(CSize());
						auto size = item->GetPreferredBounds().Size();

						if (item->GetSharedWidth())
						{
							AddSizeComponent(widths, group, size.cx);
						}
						if (item->GetSharedHeight())
						{
							AddSizeComponent(heights, group, size.cy);
						}

						item->SetPreferredMinSize(minSize);
					}

					for (auto & item : childItems)
					{
						auto group = item->GetGroup();
						auto size = item->GetPreferredMinSize();

						if (item->GetSharedWidth())
						{
							size.cx = widths[group];
						}
						if (item->GetSharedHeight())
						{
							size.cy = heights[group];
						}

						item->SetPreferredMinSize(size);
					}

					BoundsComposition::ForceCalculateSizeImmediately();
				}
			}
		}
	}
}