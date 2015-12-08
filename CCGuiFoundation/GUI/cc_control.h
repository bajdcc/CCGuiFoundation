#ifndef CC_CONTROL
#define CC_CONTROL

#include "stdafx.h"
#include "cc_base.h"
#include "cc_composition.h"
#include "cc_style.h"
#include "cc_interface.h"

using namespace cc::base;
using namespace cc::interfaces::windows;
using namespace cc::presentation::control::composition;
using namespace cc::presentation::control::event_args;
using namespace cc::presentation::control::style;

namespace cc
{
	namespace interfaces
	{
		namespace windows
		{
			class IShortcutKeyManager;
			class IShortcutKeyItem;

			class IShortcutKeyManager : public Interface
			{
			public:
				virtual cuint							GetItemCount() = 0;
				virtual bool							Execute(const KeyInfo& info) = 0;
			};

			class IShortcutKeyItem : public Interface
			{
			public:
				NotifyEvent								Executed;

				virtual PassRefPtr<IShortcutKeyManager>	GetManager() = 0;
				virtual CString							GetName() = 0;
			};
		}
	}

	namespace presentation
	{
		namespace control
		{
			class Window;
			class Popup;
			class Tooltip;

			class Application : public IControllerListener
			{
				friend class Window;
				friend class Popup;
			public:
				Application();
				~Application();

			public:
				void							InvokeClipboardNotify(PassRefPtr<Composition> composition, EventArgs& arguments);
				virtual void					LeftButtonDown(CPoint position) override;
				virtual void					RightButtonDown(CPoint position) override;
				virtual void					ClipboardUpdated() override;

			protected:
				void							RegisterWindow(Window* window);
				void							UnregisterWindow(Window* window);
				void							RegisterPopupOpened(PassRefPtr<Popup> popup);
				void							RegisterPopupClosed(PassRefPtr<Popup> popup);
				void							OnMouseDown(CPoint location);
				void							TooltipMouseEnter(PassRefPtr<Composition> sender, EventArgs& arguments);
				void							TooltipMouseLeave(PassRefPtr<Composition> sender, EventArgs& arguments);

			public:
				void							Run(PassRefPtr<Window> window);
				PassRefPtr<Window>				GetMainWindow();
				PassRefPtr<Window>				GetWindow(CPoint location);
				PassRefPtr<Window>				GetWindow(PassRefPtr<IWindow> window);
				void							ShowTooltip(PassRefPtr<Control> owner, PassRefPtr<Control> tooltip, cint preferredContentWidth, CPoint location);
				void							CloseTooltip();
				PassRefPtr<Control>				GetTooltipOwner();

				typedef function<void()> Action;

				bool							IsInMainThread();
				void							InvokeAsync(const Action& proc);
				void							InvokeInMainThread(const Action& proc);
				bool							InvokeInMainThreadAndWait(const Action& proc, cint milliseconds = -1);
				PassRefPtr<IDelay>				DelayExecute(const Action& proc, cint milliseconds);
				PassRefPtr<IDelay>				DelayExecuteInMainThread(const Action& proc, cint milliseconds);
				void							RunTask(const Action& proc);

			protected:
				RefPtr<Window>					mainWindow;
				set<Window*>					windows;
				RawPtr<Window>					sharedTooltipOwnerWindow;
				RawPtr<Control>					sharedTooltipOwner;
				RefPtr<Tooltip>					sharedTooltipControl;
				bool							sharedTooltipHovering;
				bool							sharedTooltipClosing;
				set<Popup*>						openingPopups;
			};

			class GraphicsAnimationManager : public Object
			{
				typedef list<RefPtr<IGraphicsAnimation>>	AnimationList;
			public:
				GraphicsAnimationManager();
				~GraphicsAnimationManager();

				void							AddAnimation(PassRefPtr<IGraphicsAnimation> animation);
				bool							HasAnimation();
				void							Play();

			protected:
				AnimationList					playingAnimations;
			};

			class TimeBasedAnimation : public IGraphicsAnimation
			{
			public:
				TimeBasedAnimation(cint totalMilliseconds);
				~TimeBasedAnimation();

				void							Restart(cint totalMilliseconds = -1);
				cint							GetTotalLength()override;
				cint							GetCurrentPosition()override;

			protected:
				long long						startTime;
				cint							length;
			};

			class ShortcutKeyManager;
			class ShortcutKeyItem : public IShortcutKeyItem
			{
			public:
				ShortcutKeyItem(PassRefPtr<ShortcutKeyManager> _shortcutKeyManager, bool _ctrl, bool _shift, bool _alt, cint _key);
				~ShortcutKeyItem();

				PassRefPtr<IShortcutKeyManager>	GetManager()override;
				CString							GetName()override;
				bool							CanActivate(const KeyInfo& info);
				bool							CanActivate(bool _ctrl, bool _shift, bool _alt, cint _key);
			
				bool							ctrl;
				bool							shift;
				bool							alt;
				cint							key;

			protected:
				RawPtr<ShortcutKeyManager>		shortcutKeyManager;
			};

			class ShortcutKeyManager : public IShortcutKeyManager
			{
				typedef set<RefPtr<ShortcutKeyItem>>	ShortcutKeyItemList;
			public:
				ShortcutKeyManager();
				~ShortcutKeyManager();

				cuint							GetItemCount()override;
				bool							Execute(const KeyInfo& info)override;

				PassRefPtr<IShortcutKeyItem>	CreateShortcut(bool ctrl, bool shift, bool alt, cint key);
				bool							DestroyShortcut(bool ctrl, bool shift, bool alt, cint key);
				PassRefPtr<IShortcutKeyItem>	TryGetShortcut(bool ctrl, bool shift, bool alt, cint key);
			
			protected:
				ShortcutKeyItemList				shortcutKeyItems;
			};

			class Control : virtual public Object
 			{
 				friend class Composition;
 				typedef vector<RefPtr<Control>> ControlList;
 			public:
 				Control(PassRefPtr<IControlStyleController> _styleController);
 				~Control();
 
 				NotifyEvent								VisibleChanged;
 				NotifyEvent								EnabledChanged;
 				NotifyEvent								VisuallyEnabledChanged;
 				NotifyEvent								TextChanged;
 				NotifyEvent								FontChanged;
 
 				EventArgs								GetNotifyEventArguments();
 				PassRefPtr<IControlStyleController>		GetStyleController();
 				PassRefPtr<BoundsComposition>			GetBoundsComposition();
 				PassRefPtr<Composition>					GetContainerComposition();
 				PassRefPtr<Composition>					GetFocusableComposition();
 				PassRefPtr<EventReceiver>				GetEventReceiver();
 				PassRefPtr<Control>						GetParent();
 				cint									GetChildrenCount();
 				PassRefPtr<Control>						GetChild(cint index);
 				bool									AddChild(PassRefPtr<Control> control);
 				bool									HasChild(PassRefPtr<Control> control);
 
 				virtual PassRefPtr<ControlHost>			GetRelatedControlHost();
 				virtual bool							GetVisuallyEnabled();
 				virtual bool							GetEnabled();
 				virtual void							SetEnabled(bool value);
				virtual bool							GetVisible();
				virtual void							SetVisible(bool value);
 				virtual const CString&					GetText();
 				virtual void							SetText(const CString& value);
 				virtual const Font&						GetFont();
 				virtual void							SetFont(const Font& value);
 				virtual void							SetFocus();

				PassRefPtr<Control>						GetTooltipControl();
				PassRefPtr<Control>						SetTooltipControl(PassRefPtr<Control> value);
				cint									GetTooltipWidth();
				void									SetTooltipWidth(cint value);
				bool									DisplayTooltip(CPoint location);
				void									CloseTooltip();

				virtual Object*							QueryService(UINT nId);
				template<typename T>
				T* QueryTypedService()
				{
					return dynamic_cast<T*>(QueryService(T::ServiceId));
				}
 
 			protected:
 				virtual void							OnChildInserted(PassRefPtr<Control> control);
 				virtual void							OnChildRemoved(PassRefPtr<Control> control);
 				virtual void							OnParentChanged(PassRefPtr<Control> oldParent, PassRefPtr<Control> newParent);
 				virtual void							OnParentLineChanged();
 				virtual void							OnRenderTargetChanged(PassRefPtr<IGraphicsRenderTarget> renderTarget);
 				virtual void							OnBeforeReleaseGraphicsHost();
 				virtual void							UpdateVisuallyEnabled();
 				void									SetFocusableComposition(PassRefPtr<Composition> value);
 
 			protected:
 				RefPtr<IControlStyleController>			styleController;
 				RefPtr<BoundsComposition>				boundsComposition;
 				RefPtr<Composition>						focusableComposition;
 				RefPtr<EventReceiver>					eventReceiver;
 
 				bool									isEnabled;
 				bool									isVisuallyEnabled;
				bool									isVisible;
 				CString									text;
 				Font									font;
 
 				RawPtr<Control>							parent;
 				ControlList								children;
				RefPtr<Control>							tooltipControl;
				cint									tooltipWidth;
 			};

 			class Component : public virtual Object
 			{
 			public:
 				Component();
 				~Component();
 
 				virtual void							Attach(ControlHostRoot* rootObject);
 				virtual void							Detach(ControlHostRoot* rootObject);
 			};

 			class ControlHostRoot : public virtual Object
 			{
 			public:
 				ControlHostRoot();
 				~ControlHostRoot();
 
 				bool										AddComponent(PassRefPtr<Component> component);
 				bool										RemoveComponent(PassRefPtr<Component> component);
 				bool										ContainsComponent(PassRefPtr<Component> component);
 
 			protected:
 				void										ClearComponents();
 				void										FinalizeInstance();
 
 				set<RefPtr<Component>>						components;
 			};

 			class ControlHost : public Control, public ControlHostRoot, protected IWindowListener
 			{
 			public:
 				ControlHost(PassRefPtr<IControlStyleController> _styleController);
 				~ControlHost();
 
 				NotifyEvent									WindowGotFocus;
 				NotifyEvent									WindowLostFocus;
 				NotifyEvent									WindowActivated;
 				NotifyEvent									WindowDeactivated;
 				NotifyEvent									WindowOpened;
 				RequestEvent								WindowClosing;
 				NotifyEvent									WindowClosed;
 				NotifyEvent									WindowDestroying;
 
 				PassRefPtr<GraphicsHost>					GetGraphicsHost();
 				PassRefPtr<Composition>						GetMainComposition();
 				PassRefPtr<IWindow>							GetWindow();
 				void										SetWindow(PassRefPtr<IWindow> window);
 				void										ForceCalculateSizeImmediately();
 
 				bool										GetEnabled()override;
 				void										SetEnabled(bool value)override;
 				bool										GetFocused();
 				void										SetFocused();
 				bool										GetActivated();
 				void										SetActivated();
 				bool										GetShowInTaskBar();
 				void										SetShowInTaskBar(bool value);
 				bool										GetEnabledActivate();
 				void										SetEnabledActivate(bool value);
 				bool										GetTopMost();
 				void										SetTopMost(bool topmost);
 
				PassRefPtr<IShortcutKeyManager>				GetShortcutKeyManager();
				void										SetShortcutKeyManager(PassRefPtr<IShortcutKeyManager> manager);
				PassRefPtr<GraphicsAnimationManager>		GetAnimationManager();

 				CSize										GetClientSize();
 				void										SetClientSize(CSize value);
 				CRect										GetBounds();
 				void										SetBounds(CRect value);
 				PassRefPtr<ControlHost>						GetRelatedControlHost()override;
 				const CString&								GetText()override;
 				void										SetText(const CString& value)override;
 				
 				PassRefPtr<IScreen>							GetRelatedScreen();
				void										Show();
				void										Show(int nCmdShow);
				void										ShowDeactivated();
 				void										ShowRestored();
 				void										ShowMaximized();
 				void										ShowMinimized();
 				void										Hide();
				void										Close();
				bool										GetOpening();
 
 			protected:
 				virtual void								OnWindowChanged();
 				virtual void								OnVisualStatusChanged();
 
				PassRefPtr<Control>							GetTooltipOwner(CPoint location);
				void										MoveIntoTooltipControl(PassRefPtr<Control> tooltipControl, CPoint location);
				void										MouseMoving(const MouseInfo& info)override;
 				void										MouseLeaved()override;
 				void										Moved()override;
 				void										Enabled()override;
 				void										Disabled()override;
 				void										GotFocus()override;
 				void										LostFocus()override;
 				void										Activated()override;
 				void										Deactivated()override;
 				void										Opened()override;
 				void										Closing(bool& cancel)override;
 				void										Closed()override;
 				void										Destroying()override;
 
 			protected:
				static const cint							TooltipDelayOpenTime = 500;
				static const cint							TooltipDelayCloseTime = 500;
				static const cint							TooltipDelayLifeTime = 5000;

				RefPtr<IDelay>								tooltipOpenDelay;
				RefPtr<IDelay>								tooltipCloseDelay;
				CPoint										tooltipLocation;

 				RefPtr<GraphicsHost>						host;
 			};
 
			class GraphicsHost : virtual public Object, private IWindowListener, private IControllerListener
 			{
 				typedef vector<RefPtr<Composition>>		CompositionList;
 
 			public:
 				GraphicsHost();
 				~GraphicsHost();
 
 				PassRefPtr<IWindow>						GetWindow();
 				void									SetWindow(PassRefPtr<IWindow> _window);
 				PassRefPtr<Composition>					GetMainComposition();
 				void									Render();
 
				PassRefPtr<IShortcutKeyManager>			GetShortcutKeyManager();
				void									SetShortcutKeyManager(PassRefPtr<IShortcutKeyManager> manager);
 				bool									SetFocus(PassRefPtr<Composition> composition);
 				PassRefPtr<Composition>					GetFocusedComposition();
 				CPoint									GetCaretPoint();
 				void									SetCaretPoint(CPoint value, PassRefPtr<Composition> referenceComposition = nullptr);
 
				PassRefPtr<GraphicsAnimationManager>	GetAnimationManager();
				void									DisconnectComposition(PassRefPtr<Composition> composition);

 			protected:
 				void									DisconnectCompositionInternal(PassRefPtr<Composition> composition);
 				void									MouseCapture(const MouseInfo& info);
 				void									MouseUncapture(const MouseInfo& info);
 				void									OnCharInput(const CharInfo& info, PassRefPtr<Composition> composition, CharEvent EventReceiver::* eventReceiverEvent);
 				void									OnKeyInput(const KeyInfo& info, PassRefPtr<Composition> composition, KeyEvent EventReceiver::* eventReceiverEvent);
 				void									RaiseMouseEvent(MouseEventArgs& arguments, PassRefPtr<Composition> composition, MouseEvent EventReceiver::* eventReceiverEvent);
 				void									OnMouseInput(const MouseInfo& info, MouseEvent EventReceiver::* eventReceiverEvent);
 
 			private:
 				IWindowListener::HitTestResult			HitTest(CPoint location)override;
 				void									Moving(CRect& bounds, bool fixSizeOnly)override;
 				void									Moved()override;
 
 				void									LeftButtonDown(const MouseInfo& info)override;
 				void									LeftButtonUp(const MouseInfo& info)override;
 				void									LeftButtonDoubleClick(const MouseInfo& info)override;
 				void									RightButtonDown(const MouseInfo& info)override;
 				void									RightButtonUp(const MouseInfo& info)override;
 				void									RightButtonDoubleClick(const MouseInfo& info)override;
 				void									MiddleButtonDown(const MouseInfo& info)override;
 				void									MiddleButtonUp(const MouseInfo& info)override;
 				void									MiddleButtonDoubleClick(const MouseInfo& info)override;
 				void									HorizontalWheel(const MouseInfo& info)override;
 				void									VerticalWheel(const MouseInfo& info)override;
 				void									MouseMoving(const MouseInfo& info)override;
 				void									MouseEntered()override;
 				void									MouseLeaved()override;
 
 				void									KeyDown(const KeyInfo& info)override;
 				void									KeyUp(const KeyInfo& info)override;
 				void									SysKeyDown(const KeyInfo& info)override;
 				void									SysKeyUp(const KeyInfo& info)override;
 				void									Char(const CharInfo& info)override;
 
 				void									GlobalTimer()override;
 
 			public:
				const long long							CaretInterval;
 
 			protected:
 				RefPtr<IWindow>							window;
 				RefPtr<WindowComposition>				windowComposition;
 				RefPtr<Composition>						focusedComposition;
				RefPtr<IShortcutKeyManager>				shortcutKeyManager;
 				CSize									previousClientSize;
 				CSize									minSize;
 				CPoint									caretPoint;
 				long long								lastCaretTime;
 
				RefPtr<GraphicsAnimationManager>		animationManager;
 				RefPtr<Composition>						mouseCaptureComposition;
 				CompositionList							mouseEnterCompositions;
 			};

			class Window : public ControlHost
			{
				friend class Application;
			public:
				Window(PassRefPtr<IWindowStyleController> _styleController);
				~Window();

				NotifyEvent								ClipboardUpdated;

				void									MoveToScreenCenter();

				bool									GetMaximizedBox();
				void									SetMaximizedBox(bool visible);
				bool									GetMinimizedBox();
				void									SetMinimizedBox(bool visible);
				bool									GetBorder();
				void									SetBorder(bool visible);
				bool									GetSizeBox();
				void									SetSizeBox(bool visible);
				bool									GetIconVisible();
				void									SetIconVisible(bool visible);
				bool									GetTitleBar();
				void									SetTitleBar(bool visible);
				void									ShowModal(PassRefPtr<Window> owner, const function<void()>& callback);
				void									ShowModalAndDelete(PassRefPtr<Window> owner, const function<void()>& callback);

			protected:
				void									Moved()override;
				void									OnWindowChanged()override;
				void									OnVisualStatusChanged()override;
				virtual void							MouseClickedOnOtherWindow(PassRefPtr<Window> window);

			protected:
				RefPtr<IWindowStyleController>			styleController;
			};

			class Popup : public Window
			{
			public:
				Popup(PassRefPtr<IWindowStyleController> _styleController);
				~Popup();

				bool									IsClippedByScreen(CPoint location);
				void									ShowPopup(CPoint location, PassRefPtr<IScreen> screen);
				void									ShowPopup(PassRefPtr<Control> control, CRect bounds, bool preferredTopBottomSide);
				void									ShowPopup(PassRefPtr<Control> control, CPoint location);
				void									ShowPopup(PassRefPtr<Control> control, bool preferredTopBottomSide);

			protected:
				void									MouseClickedOnOtherWindow(PassRefPtr<Window> window)override;

				void									PopupOpened(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									PopupClosed(PassRefPtr<Composition> sender, EventArgs& arguments);
			};

			class Tooltip : public Popup, private IControllerListener
			{
			public:
				Tooltip(PassRefPtr<IWindowStyleController> _styleController);
				~Tooltip();

				cint									GetPreferredContentWidth();
				void									SetPreferredContentWidth(cint value);

				PassRefPtr<Control>						GetTemporaryContentControl();
				void									SetTemporaryContentControl(PassRefPtr<Control> control);

			protected:
				void									GlobalTimer()override;
				void									TooltipOpened(PassRefPtr<Composition> sender, EventArgs& arguments);
				void									TooltipClosed(PassRefPtr<Composition> sender, EventArgs& arguments);

				RawPtr<Control>							temporaryContentControl;
			};
		}
	}
}

#endif