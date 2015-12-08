#ifndef CC_COMPOSITION
#define CC_COMPOSITION

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			class GraphicsHost;
			class Component;
			class Control;
			class ControlHost;
			class ControlHostRoot;
			class Window;

			namespace composition
			{
				class Composition;
			}

			namespace event_args
			{
				using namespace cc::presentation::control::composition;

				template<typename T>
				class Event
				{
				public:
					typedef void(RawFunctionType)(PassRefPtr<Composition>, T&);
					typedef function<RawFunctionType> FunctionType;
					typedef T ArgumentType;

					class IHandler : public virtual Interface
					{
					public:
						virtual void Execute(PassRefPtr<Composition>, T&) = 0;
					};

					class HandlerContainer
					{
					public:
						RefPtr<IHandler> handler;
					};

					class FunctionHandler : public IHandler
					{
					protected:
						FunctionType handler;
					public:
						FunctionHandler(const FunctionType& _handler)
							: handler(_handler)
						{

						}

						void Execute(PassRefPtr<Composition> sender, T& argument) override
						{
							handler(sender, argument);
						}
					};

				protected:
					RawPtr<Composition> sender;
					set<RefPtr<IHandler>> handlers;

				public:
					Event()
					{

					}

					Event(PassRefPtr<Composition> _sender)
						: sender(_sender)
					{
					}

					PassRefPtr<Composition> GetAssociatedComposition()
					{
						return sender;
					}

					void SetAssociatedComposition(PassRefPtr<Composition> _sender)
					{
						sender = _sender;
					}

					bool Attach(PassRefPtr<IHandler> handler)
					{
						typedef decltype(handlers) SET;
						return handlers.insert(handler).second;
					}

					template<typename TClass, typename TMethod>
					PassRefPtr<IHandler> AttachMethod(TClass* receiver, TMethod TClass::* method)
					{
						RefPtr<IHandler> handler = adoptRef(new FunctionHandler(FunctionType(bind(method, receiver, placeholders::_1, placeholders::_2))));
						Attach(handler);
						return handler;
					}

					PassRefPtr<IHandler> AttachFunction(RawFunctionType* function)
					{
						RefPtr<IHandler> handler = adoptRef(new FunctionHandler(FunctionType(function)));
						Attach(handler);
						return handler;
					}

					PassRefPtr<IHandler> AttachFunction(const FunctionType& function)
					{
						RefPtr<IHandler> handler = adoptRef(new FunctionHandler(function));
						Attach(handler);
						return handler;
					}

					template<typename TLambda>
					PassRefPtr<IHandler> AttachLambda(const TLambda& lambda)
					{
						RefPtr<IHandler> handler = adoptRef(new FunctionHandler(FunctionType(lambda)));
						Attach(handler);
						return handler;
					}

					bool Detach(PassRefPtr<IHandler> handler)
					{
						return handlers.erase(handler) == 1;
					}

					void ExecuteWithNewSender(T& argument, PassRefPtr<Composition> newSender)
					{
						for (auto & handle : handlers)
						{
							handle->Execute(newSender ? newSender : sender, argument);
						}
					}

					void Execute(T& argument)
					{
						ExecuteWithNewSender(argument, nullptr);
					}

					void Execute(const T& argument)
					{
						auto t = argument;
						ExecuteWithNewSender(t, nullptr);
					}
				};

				struct EventArgs
				{
					EventArgs();
					EventArgs(PassRefPtr<Composition> composition);
					RefPtr<Composition>		compositionSource;
					RefPtr<Composition>		eventSource;
					bool					handled;
				};

				struct RequestEventArgs : public EventArgs
				{
					RequestEventArgs();
					RequestEventArgs(PassRefPtr<Composition> composition);
					bool cancel;
				};

				struct KeyEventArgs : public EventArgs, public KeyInfo
				{
					KeyEventArgs();
					KeyEventArgs(PassRefPtr<Composition> composition);
				};

				struct CharEventArgs : public EventArgs, public CharInfo
				{
					CharEventArgs();
					CharEventArgs(PassRefPtr<Composition> composition);
				};

				struct MouseEventArgs : public EventArgs, public MouseInfo
				{
					MouseEventArgs();
					MouseEventArgs(PassRefPtr<Composition> composition);
				};

				typedef Event<EventArgs>			NotifyEvent;
				typedef Event<RequestEventArgs>		RequestEvent;
				typedef Event<KeyEventArgs>			KeyEvent;
				typedef Event<CharEventArgs>		CharEvent;
				typedef Event<MouseEventArgs>		MouseEvent;

				class EventReceiver : public Object
				{
				public:
					EventReceiver(PassRefPtr<Composition> _sender);
					~EventReceiver();

					PassRefPtr<Composition>		GetAssociatedComposition();

					MouseEvent					leftButtonDown;
					MouseEvent					leftButtonUp;
					MouseEvent					leftButtonDoubleClick;
					MouseEvent					middleButtonDown;
					MouseEvent					middleButtonUp;
					MouseEvent					middleButtonDoubleClick;
					MouseEvent					rightButtonDown;
					MouseEvent					rightButtonUp;
					MouseEvent					rightButtonDoubleClick;
					MouseEvent					horizontalWheel;
					MouseEvent					verticalWheel;
					MouseEvent					mouseMove;
					NotifyEvent					mouseEnter;
					NotifyEvent					mouseLeave;

					KeyEvent					previewKey;
					KeyEvent					keyDown;
					KeyEvent					keyUp;
					KeyEvent					systemKeyDown;
					KeyEvent					systemKeyUp;
					CharEvent					previewCharInput;
					CharEvent					charInput;
					NotifyEvent					gotFocus;
					NotifyEvent					lostFocus;
					NotifyEvent					caretNotify;
					NotifyEvent					clipboardNotify;

				protected:
					RawPtr<Composition>			sender;
				};
			}

			namespace composition
			{
				using namespace cc::presentation::control::event_args;

				class Composition;
				class TableCellComposition;
				class TableComposition;
				class StackItemComposition;
				class StackComposition;
				class SideAlignedComposition;
				class PartialViewComposition;
				class SharedSizeItemComposition;
				class SharedSizeComposition;

				class Composition : public Object
				{
					typedef vector<RefPtr<Composition>> CompositionList;
				public:
					enum MinSizeLimitation
					{
						NoLimit,
						LimitToElement,
						LimitToElementAndChildren,
					};

				public:
					Composition();
					~Composition();

					PassRefPtr<Composition>						GetParent();
					const CompositionList						Children();
					bool										AddChild(PassRefPtr<Composition> child);
					bool										InsertChild(cint index, PassRefPtr<Composition> child);
					bool										RemoveChild(PassRefPtr<Composition> child);
					bool										MoveChild(PassRefPtr<Composition> child, cint newIndex);

					PassRefPtr<IGraphicsElement>				GetOwnedElement();
					void										SetOwnedElement(PassRefPtr<IGraphicsElement> element);
					bool										GetVisible();
					void										SetVisible(bool value);
					MinSizeLimitation							GetMinSizeLimitation();
					void										SetMinSizeLimitation(MinSizeLimitation value);
					PassRefPtr<IGraphicsRenderTarget>			GetRenderTarget();
					void										SetRenderTarget(PassRefPtr<IGraphicsRenderTarget> value);

					void										Render(CSize offset);
					PassRefPtr<EventReceiver>					GetEventReceiver();
					bool										HasEventReceiver();
					PassRefPtr<Composition>						FindComposition(CPoint location);
					CRect										GetGlobalBounds();

					PassRefPtr<Control>							GetAssociatedControl();
					PassRefPtr<GraphicsHost>					GetAssociatedHost();
					PassRefPtr<ICursor>							GetAssociatedCursor();
					void										SetAssociatedCursor(PassRefPtr<ICursor> cursor);
					IWindowListener::HitTestResult				GetAssociatedHitTestResult();
					void										SetAssociatedHitTestResult(IWindowListener::HitTestResult value);

					PassRefPtr<Control>							GetRelatedControl();
					PassRefPtr<GraphicsHost>					GetRelatedGraphicsHost();
					PassRefPtr<ControlHost>						GetRelatedControlHost();
					PassRefPtr<ICursor>							GetRelatedCursor();

					virtual CRect								GetMargin();
					virtual void								SetMargin(CRect value);
					virtual CRect								GetInternalMargin();
					virtual void								SetInternalMargin(CRect value);
					virtual CSize								GetPreferredMinSize();
					virtual void								SetPreferredMinSize(CSize value);
					virtual CRect								GetClientArea();
					virtual void								ForceCalculateSizeImmediately();

					virtual bool								IsSizeAffectParent() = 0;
					virtual CSize								GetMinPreferredClientSize() = 0;
					virtual CRect								GetPreferredBounds() = 0;
					virtual CRect								GetBounds() = 0;

				protected:
					virtual void								OnControlParentChanged(PassRefPtr<Control> control);
					virtual void								OnChildInserted(PassRefPtr<Composition> child);
					virtual void								OnChildRemoved(PassRefPtr<Composition> child);
					virtual void								OnParentChanged(PassRefPtr<Composition> oldParent, PassRefPtr<Composition> newParent);
					virtual void								OnParentLineChanged();
					virtual void								OnRenderTargetChanged();

				public:
					virtual void								SetAssociatedControl(PassRefPtr<Control> control);
					virtual void								SetAssociatedHost(PassRefPtr<GraphicsHost> host);

				protected:
					CompositionList								children;
					RawPtr<Composition>							parent;
					RefPtr<IGraphicsElement>					ownedElement;
					bool										visible;
					RefPtr<IGraphicsRenderTarget>				renderTarget;
					MinSizeLimitation							minSizeLimitation;

					RefPtr<EventReceiver>						eventReceiver;
					RawPtr<Control>								associatedControl;
					RawPtr<GraphicsHost>						associatedHost;
					RawPtr<ICursor>								associatedCursor;
					IWindowListener::HitTestResult				associatedHitTestResult;

					CRect										margin;
					CRect										internalMargin;
					CSize										preferredMinSize;
				};

				class GraphicsSite : public Composition
				{
				public:
					GraphicsSite();
					~GraphicsSite();

					bool								IsSizeAffectParent()override;
					CSize								GetMinPreferredClientSize()override;
					CRect								GetPreferredBounds()override;

					NotifyEvent							BoundsChanged;

				protected:
					virtual CRect						GetBoundsInternal(CRect expectedBounds);
					void								UpdatePreviousBounds(CRect bounds);

					CRect								previousBounds;
				};

				class WindowComposition : public GraphicsSite
				{
				public:
					WindowComposition();
					~WindowComposition();

					PassRefPtr<IWindow>					GetAttachedWindow();
					void								SetAttachedWindow(PassRefPtr<IWindow> window);

					CRect								GetBounds()override;
					void								SetMargin(CRect value)override;

				protected:
					RawPtr<IWindow>						attachedWindow;
				};

				class BoundsComposition : public GraphicsSite
				{
				public:
					BoundsComposition();
					~BoundsComposition();

					CRect								GetPreferredBounds()override;
					CRect								GetBounds()override;
					void								SetBounds(CRect value);

					void								ClearAlignmentToParent();
					CRect								GetAlignmentToParent();
					void								SetAlignmentToParent(CRect value);
					bool								IsAlignedToParent();

				protected:
					CRect								compositionBounds;
					CRect								alignmentToParent;
				};

				class TableCellComposition : public GraphicsSite
				{
					friend class TableComposition;
				public:
					TableCellComposition();
					~TableCellComposition();

					PassRefPtr<TableComposition>		GetTableParent();

					cint								GetRow();
					cint								GetRowSpan();
					cint								GetColumn();
					cint								GetColumnSpan();
					bool								SetSite(cint _row, cint _column, cint _rowSpan, cint _columnSpan);

					CRect								GetBounds()override;

				protected:
					void								ClearSitedCells(PassRefPtr<TableComposition> table);
					void								SetSitedCells(PassRefPtr<TableComposition> table);
					void								ResetSiteInternal();
					bool								SetSiteInternal(cint _row, cint _column, cint _rowSpan, cint _columnSpan);
					void								OnParentChanged(PassRefPtr<Composition> oldParent, PassRefPtr<Composition> newParent)override;
					void								OnTableRowsAndColumnsChanged();

					cint								row;
					cint								rowSpan;
					cint								column;
					cint								columnSpan;
					RawPtr<TableComposition>			tableParent;
					CSize								lastPreferredSize;
				};

				class TableComposition : public BoundsComposition
				{
					friend class TableCellComposition;
				public:
					TableComposition();
					~TableComposition();

					cint								GetRows();
					cint								GetColumns();
					bool								SetRowsAndColumns(cint _rows, cint _columns);
					PassRefPtr<TableCellComposition>	GetSitedCell(cint _row, cint _column);

					CellOption							GetRowOption(cint _row);
					void								SetRowOption(cint _row, CellOption option);
					CellOption							GetColumnOption(cint _column);
					void								SetColumnOption(cint _column, CellOption option);

					cint								GetCellPadding();
					void								SetCellPadding(cint value);
					CRect								GetCellArea();
					void								UpdateCellBounds();

					void								ForceCalculateSizeImmediately()override;
					CSize								GetMinPreferredClientSize()override;
					CRect								GetBounds()override;

				protected:
					cint								GetSiteIndex(cint _rows, cint _columns, cint _row, cint _column);
					void								SetSitedCell(cint _row, cint _column, PassRefPtr<TableCellComposition> cell);

					void								UpdateCellBoundsInternal(
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
						);
					void								UpdateCellBoundsPercentages(
						vector<cint>& dimSizes,
						cint dimSize,
						cint maxDimSize,
						vector<CellOption>& dimOptions
						);
					cint								UpdateCellBoundsOffsets(
						vector<cint>& offsets,
						vector<cint>& sizes,
						cint start,
						cint max
						);
					void								UpdateCellBoundsInternal();
					void								UpdateTableContentMinSize();
					void								OnRenderTargetChanged()override;

					static cint First(cint a, cint b);
					static cint Second(cint a, cint b);
					static cint X(CSize s);
					static cint Y(CSize s);
					static cint RL(PassRefPtr<TableCellComposition> cell);
					static cint CL(PassRefPtr<TableCellComposition> cell);
					static cint RS(PassRefPtr<TableCellComposition> cell);
					static cint CS(PassRefPtr<TableCellComposition> cell);

				protected:
					cint								rows;
					cint								columns;
					cint								cellPadding;
					cint								rowExtending;
					cint								columnExtending;
					vector<CellOption>					rowOptions;
					vector<CellOption>					columnOptions;
					vector<TableCellComposition*>		cellCompositions;
					vector<CRect>						cellBounds;
					CRect								previousBounds;
					CSize								previousContentMinSize;
					CSize								tableContentMinSize;
				};

				class StackItemComposition : public GraphicsSite
				{
					friend class StackComposition;
				public:
					StackItemComposition();
					~StackItemComposition();

					bool								IsSizeAffectParent()override;
					CRect								GetBounds()override;
					void								SetBounds(CRect value);

					CRect								GetExtraMargin();
					void								SetExtraMargin(CRect value);

				protected:
					void								OnParentChanged(PassRefPtr<Composition> oldParent, PassRefPtr<Composition> newParent)override;
					CSize								GetMinSize();

					RawPtr<StackComposition>			stackParent;
					CRect								bounds;
					CRect								extraMargin;
				};

				class StackComposition : public BoundsComposition
				{
					friend class StackItemComposition;
					typedef vector<RefPtr<StackItemComposition>> StackItemCompositionList;
				public:
					StackComposition();
					~StackComposition();

					enum Direction
					{
						Horizontal,
						Vertical,
					};

					const StackItemCompositionList&		GetStackItems();
					PassRefPtr<StackItemComposition>	GetStackItem(cint index);
					bool								InsertStackItem(cint index, PassRefPtr<StackItemComposition> item);

					Direction							GetDirection();
					void								SetDirection(Direction value);
					cint								GetPadding();
					void								SetPadding(cint value);

					CSize								GetMinPreferredClientSize()override;
					CRect								GetBounds()override;

					CRect								GetExtraMargin();
					void								SetExtraMargin(CRect value);
					bool								IsStackItemClipped();
					bool								EnsureVisible(cuint index);

				protected:
					void								UpdateStackItemBounds();
					void								FixStackItemSizes();
					void								OnBoundsChanged(PassRefPtr<Composition> sender, EventArgs& arguments);
					void								OnChildInserted(PassRefPtr<Composition> child)override;
					void								OnChildRemoved(PassRefPtr<Composition> child)override;

					Direction							direction;
					StackItemCompositionList			stackItems;
					vector<CRect>						stackItemBounds;
					CSize								stackItemTotalSize;
					cint								padding;
					CRect								previousBounds;
					CRect								extraMargin;
					RefPtr<StackItemComposition>		ensuringVisibleStackItem;
				};

				class SideAlignedComposition : public GraphicsSite
				{
				public:
					enum Direction
					{
						Left,
						Top,
						Right,
						Bottom,
					};

				public:
					SideAlignedComposition();
					~SideAlignedComposition();

					Direction							GetDirection();
					void								SetDirection(Direction value);
					cint								GetMaxLength();
					void								SetMaxLength(cint value);
					double								GetMaxRatio();
					void								SetMaxRatio(double value);

					bool								IsSizeAffectParent()override;
					CRect								GetBounds()override;

				protected:
					Direction							direction;
					cint								maxLength;
					double								maxRatio;
				};

				class PartialViewComposition : public GraphicsSite
				{
				public:
					PartialViewComposition();
					~PartialViewComposition();

					double								GetWidthRatio();
					double								GetWidthPageSize();
					double								GetHeightRatio();
					double								GetHeightPageSize();
					void								SetWidthRatio(double value);
					void								SetWidthPageSize(double value);
					void								SetHeightRatio(double value);
					void								SetHeightPageSize(double value);

					bool								IsSizeAffectParent()override;
					CRect								GetBounds()override;

				protected:
					double								wRatio;
					double								wPageSize;
					double								hRatio;
					double								hPageSize;
				};

				class SharedSizeItemComposition : public BoundsComposition
				{
				public:
					SharedSizeItemComposition();
					~SharedSizeItemComposition();

					const CString&						GetGroup();
					void								SetGroup(const CString& value);
					bool								GetSharedWidth();
					void								SetSharedWidth(bool value);
					bool								GetSharedHeight();
					void								SetSharedHeight(bool value);

				protected:
					RawPtr<SharedSizeComposition>		parentRoot;
					CString								group;
					bool								sharedWidth;
					bool								sharedHeight;

					void								Update();
					void								OnParentLineChanged()override;
				};

				class SharedSizeComposition : public BoundsComposition
				{
					friend class SharedSizeItemComposition;
				public:
					SharedSizeComposition();
					~SharedSizeComposition();

					void								ForceCalculateSizeImmediately()override;

				protected:
					static void							AddSizeComponent(map<CString, cint>& sizes, const CString& group, cint sizeComponent);
					
					vector<RefPtr<SharedSizeItemComposition>> childItems;
				};
			}
		}
	}
}

#endif