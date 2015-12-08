#include "stdafx.h"
#include "cc_control.h"
#include "cc_presentation.h"
#include "cc_element.h"

using namespace cc::presentation::windows;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			Application::Application()
				: sharedTooltipHovering(false)
				, sharedTooltipClosing(false)
			{
				GetStorage()->GetController()->GetCallbackService()->InstallListener(this);
			}

			Application::~Application()
			{
				GetStorage()->GetController()->GetCallbackService()->UninstallListener(this);
			}

			void Application::LeftButtonDown(CPoint position)
			{
				OnMouseDown(position);
			}

			void Application::RightButtonDown(CPoint position)
			{
				OnMouseDown(position);
			}

			void Application::ClipboardUpdated()
			{
				for (auto & window : windows)
				{
					EventArgs arguments = window->GetNotifyEventArguments();
					window->ClipboardUpdated.Execute(window->GetNotifyEventArguments());
					InvokeClipboardNotify(window->GetBoundsComposition(), arguments);
				}
			}

			void Application::RegisterWindow(Window* window)
			{
				windows.insert(window);
			}

			void Application::UnregisterWindow(Window* window)
			{
				windows.erase(window);
			}

			void Application::OnMouseDown(CPoint location)
			{
				RefPtr<Window> Window = GetWindow(location);
				for (auto & window : windows)
				{
					if (window != Window)
					{
						window->MouseClickedOnOtherWindow(Window);
					}
				}
			}

			void Application::Run(PassRefPtr<Window> window)
			{
				if (!mainWindow)
				{
					mainWindow = window;
					GetStorage()->GetController()->GetWindowService()->Run(mainWindow->GetWindow());
					mainWindow.clear();
					sharedTooltipControl.clear();
				}
			}

			PassRefPtr<Window> Application::GetMainWindow()
			{
				return mainWindow;
			}

			PassRefPtr<Window> Application::GetWindow(CPoint location)
			{
				RefPtr<IWindow> Window = GetStorage()->GetController()->GetWindowService()->GetWindow(location);
				if (Window)
				{
					for (auto & window : windows)
					{
						if (window->GetWindow() == Window)
						{
							return window;
						}
					}
				}
				return nullptr;
			}

			PassRefPtr<Window> Application::GetWindow(PassRefPtr<IWindow> _window)
			{
				for (auto & window : windows)
				{
					if (window->GetWindow() == _window)
					{
						return window;
					}
				}
				return nullptr;
			}

			void Application::InvokeClipboardNotify(PassRefPtr<Composition> composition, EventArgs& arguments)
			{
				RefPtr<Composition> _composition = composition;
				if (_composition->HasEventReceiver())
				{
					_composition->GetEventReceiver()->clipboardNotify.Execute(arguments);
				}
				for (auto & subComposition : _composition->Children())
				{
					InvokeClipboardNotify(subComposition, arguments);
				}
			}

			bool Application::IsInMainThread()
			{
				return GetStorage()->GetController()->GetAsyncService()->IsInMainThread();
			}

			void Application::InvokeAsync(const Action& proc)
			{
				return GetStorage()->GetController()->GetAsyncService()->InvokeAsync(proc);
			}

			void Application::InvokeInMainThread(const Action& proc)
			{
				return GetStorage()->GetController()->GetAsyncService()->InvokeInMainThread(proc);
			}

			bool Application::InvokeInMainThreadAndWait(const Action& proc, cint milliseconds /*= -1*/)
			{
				return GetStorage()->GetController()->GetAsyncService()->InvokeInMainThreadAndWait(proc, milliseconds);
			}

			PassRefPtr<IDelay> Application::DelayExecute(const Action& proc, cint milliseconds)
			{
				return GetStorage()->GetController()->GetAsyncService()->DelayExecute(proc, milliseconds);
			}

			PassRefPtr<IDelay> Application::DelayExecuteInMainThread(const Action& proc, cint milliseconds)
			{
				return GetStorage()->GetController()->GetAsyncService()->DelayExecuteInMainThread(proc, milliseconds);
			}

			void Application::RunTask(const Action& proc)
			{
				if (IsInMainThread())
				{
					proc();
				}
				else
				{
					InvokeInMainThreadAndWait([&proc]()
					{
						proc();
					});
				}
			}

			void Application::RegisterPopupOpened(PassRefPtr<Popup> popup)
			{
				auto found = openingPopups.find(popup.get());
				if (found == openingPopups.end())
				{
					openingPopups.insert(popup.get());
					if (openingPopups.size() == 1)
					{
						GetStorage()->GetController()->GetInputService()->StartHookMouse();
					}
				}
			}

			void Application::RegisterPopupClosed(PassRefPtr<Popup> popup)
			{
				if (openingPopups.erase(popup.get()) == 1)
				{
					if (openingPopups.empty())
					{
						GetStorage()->GetController()->GetInputService()->StopHookMouse();
					}
				}
			}

			void Application::TooltipMouseEnter(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				sharedTooltipHovering = true;
			}

			void Application::TooltipMouseLeave(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				sharedTooltipHovering = false;
				if (sharedTooltipClosing)
				{
					CloseTooltip();
				}
			}

			void Application::ShowTooltip(PassRefPtr<Control> owner, PassRefPtr<Control> tooltip, cint preferredContentWidth, CPoint location)
			{
				RawPtr<Window> ownerWindow = dynamic_cast<Window*>(owner->GetRelatedControlHost().get());
				if (sharedTooltipOwnerWindow != ownerWindow)
				{
					sharedTooltipControl = nullptr;
				}

				if (!sharedTooltipControl)
				{
					RefPtr<IWindowStyleController> tooltipStyle;
					if (ownerWindow)
					{
						tooltipStyle = dynamic_cast<IWindowStyleController*>(ownerWindow->GetStyleController().get())->CreateTooltipStyle();
					}
					if (!tooltipStyle)
					{
						tooltipStyle = GetStorage()->GetTheme()->CreateTooltipStyle();
					}

					sharedTooltipControl = adoptRef(new Tooltip(tooltipStyle));
					sharedTooltipControl->GetBoundsComposition()->GetEventReceiver()->mouseEnter.AttachMethod(this, &Application::TooltipMouseEnter);
					sharedTooltipControl->GetBoundsComposition()->GetEventReceiver()->mouseLeave.AttachMethod(this, &Application::TooltipMouseLeave);
				}

				sharedTooltipHovering = false;
				sharedTooltipClosing = false;
				sharedTooltipOwnerWindow = ownerWindow;
				sharedTooltipOwner = owner.get();
				sharedTooltipControl->SetTemporaryContentControl(tooltip);
				sharedTooltipControl->SetPreferredContentWidth(preferredContentWidth);
				sharedTooltipControl->SetClientSize(CSize(10, 10));				
				sharedTooltipControl->ShowPopup(owner.get(), location);
			}

			void Application::CloseTooltip()
			{
				if (sharedTooltipControl)
				{
					if (sharedTooltipHovering)
					{
						sharedTooltipClosing = true;
					}
					else
					{
						sharedTooltipClosing = false;
						sharedTooltipControl->Close();
					}
				}
			}

			PassRefPtr<Control> Application::GetTooltipOwner()
			{
				if (!sharedTooltipControl)
					return nullptr;
				if (!sharedTooltipControl->GetTemporaryContentControl())
					return nullptr;
				return sharedTooltipOwner;
			}

			GraphicsAnimationManager::GraphicsAnimationManager()
			{

			}

			GraphicsAnimationManager::~GraphicsAnimationManager()
			{

			}

			void GraphicsAnimationManager::AddAnimation(PassRefPtr<IGraphicsAnimation> animation)
			{
				playingAnimations.push_back(animation);
			}

			bool GraphicsAnimationManager::HasAnimation()
			{
				return !playingAnimations.empty();
			}

			void GraphicsAnimationManager::Play()
			{
				for (auto i = playingAnimations.begin(); i != playingAnimations.end();)
				{
					RefPtr<IGraphicsAnimation> animation = *i;
					cint totalLength = animation->GetTotalLength();
					cint currentPosition = animation->GetCurrentPosition();
					animation->Play(currentPosition, totalLength);
					if (currentPosition >= totalLength)
					{
						i = playingAnimations.erase(i);
						animation->Stop();
					}
					else
					{
						i++;
					}
				}
			}
			TimeBasedAnimation::TimeBasedAnimation(cint totalMilliseconds)
				: startTime(0)
				, length(totalMilliseconds)
			{
				Restart();
			}

			TimeBasedAnimation::~TimeBasedAnimation()
			{

			}

			void TimeBasedAnimation::Restart(cint totalMilliseconds /*= -1*/)
			{
				startTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
				if (totalMilliseconds > -1)
				{
					length = totalMilliseconds;
				}

			}

			cint TimeBasedAnimation::GetTotalLength()
			{
				return length;
			}

			cint TimeBasedAnimation::GetCurrentPosition()
			{
				return (cint)(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - startTime);
			}

			ShortcutKeyItem::ShortcutKeyItem(PassRefPtr<ShortcutKeyManager> _shortcutKeyManager, bool _ctrl, bool _shift, bool _alt, cint _key)
				: shortcutKeyManager(_shortcutKeyManager)
				, ctrl(_ctrl)
				, shift(_shift)
				, alt(_alt)
				, key(_key)
			{

			}

			ShortcutKeyItem::~ShortcutKeyItem()
			{

			}

			PassRefPtr<IShortcutKeyManager> ShortcutKeyItem::GetManager()
			{
				return shortcutKeyManager;
			}

			CString ShortcutKeyItem::GetName()
			{
				CString name;
				if (ctrl) name += _T("Ctrl+");
				if (shift) name += _T("Shift+");
				if (alt) name += _T("Alt+");
				name += GetStorage()->GetController()->GetInputService()->GetKeyName(key);
				return name;
			}

			bool ShortcutKeyItem::CanActivate(const KeyInfo& info)
			{
				return
					info.ctrl == ctrl &&
					info.shift == shift &&
					info.alt == alt &&
					info.code == key;
			}

			bool ShortcutKeyItem::CanActivate(bool _ctrl, bool _shift, bool _alt, cint _key)
			{
				return
					_ctrl == ctrl &&
					_shift == shift &&
					_alt == alt &&
					_key == key;
			}

			ShortcutKeyManager::ShortcutKeyManager()
			{

			}

			ShortcutKeyManager::~ShortcutKeyManager()
			{

			}

			cuint ShortcutKeyManager::GetItemCount()
			{
				return shortcutKeyItems.size();
			}

			bool ShortcutKeyManager::Execute(const KeyInfo& info)
			{
				bool executed = false;
				for (auto & shortcutKeyItem : shortcutKeyItems)
				{
					if (shortcutKeyItem->CanActivate(info))
					{
						EventArgs arguments;
						shortcutKeyItem->Executed.Execute(arguments);
						executed = true;
					}
				}
				return executed;
			}

			PassRefPtr<IShortcutKeyItem> ShortcutKeyManager::CreateShortcut(bool ctrl, bool shift, bool alt, cint key)
			{
				for (auto & shortcutKeyItem : shortcutKeyItems)
				{
					if (shortcutKeyItem->CanActivate(ctrl, shift, alt, key))
					{
						return shortcutKeyItem;
					}
				}
				RefPtr<ShortcutKeyItem> item = adoptRef(new ShortcutKeyItem(this, ctrl, shift, alt, key));
				shortcutKeyItems.insert(item);
				return item;
			}

			bool ShortcutKeyManager::DestroyShortcut(bool ctrl, bool shift, bool alt, cint key)
			{
				for (auto & shortcutKeyItem : shortcutKeyItems)
				{
					if (shortcutKeyItem->CanActivate(ctrl, shift, alt, key))
					{
						shortcutKeyItems.erase(shortcutKeyItem);
						return true;
					}
				}
				return false;
			}

			PassRefPtr<IShortcutKeyItem> ShortcutKeyManager::TryGetShortcut(bool ctrl, bool shift, bool alt, cint key)
			{
				for (auto & shortcutKeyItem : shortcutKeyItems)
				{
					if (shortcutKeyItem->CanActivate(ctrl, shift, alt, key))
					{
						return shortcutKeyItem;
					}
				}
				return nullptr;
			}

			Control::Control(PassRefPtr<IControlStyleController> _styleController)
				: styleController(_styleController.get())
				, boundsComposition(_styleController->GetBoundsComposition())
				, eventReceiver(_styleController->GetBoundsComposition()->GetEventReceiver())
				, isEnabled(true)
				, isVisuallyEnabled(true)
				, isVisible(true)
				, tooltipWidth(50)
			{
				boundsComposition->SetAssociatedControl(this);
				VisibleChanged.SetAssociatedComposition(boundsComposition);
				EnabledChanged.SetAssociatedComposition(boundsComposition);
				VisuallyEnabledChanged.SetAssociatedComposition(boundsComposition);
				TextChanged.SetAssociatedComposition(boundsComposition);
				FontChanged.SetAssociatedComposition(boundsComposition);

				font = GetStorage()->GetController()->GetResourceService()->GetDefaultFont();
				styleController->SetFont(font);
				styleController->SetText(text);
				styleController->SetVisuallyEnabled(isVisuallyEnabled);
			}

			Control::~Control()
			{
				if (parent || !styleController)
				{
					children.clear();
				}
				else
				{
					while (!children.empty())
					{
						auto it = children.rbegin();
						(*it)->GetBoundsComposition()->GetParent()->RemoveChild((*it)->GetBoundsComposition());
					}
					boundsComposition = nullptr;
				}
			}

			EventArgs Control::GetNotifyEventArguments()
			{
				return EventArgs(boundsComposition);
			}

			PassRefPtr<IControlStyleController> Control::GetStyleController()
			{
				return styleController;
			}

			PassRefPtr<BoundsComposition> Control::GetBoundsComposition()
			{
				return boundsComposition;
			}

			PassRefPtr<Composition> Control::GetContainerComposition()
			{
				return styleController->GetContainerComposition();
			}

			PassRefPtr<Composition> Control::GetFocusableComposition()
			{
				return focusableComposition;
			}

			PassRefPtr<EventReceiver> Control::GetEventReceiver()
			{
				return eventReceiver;
			}

			PassRefPtr<Control> Control::GetParent()
			{
				return parent;
			}

			cint Control::GetChildrenCount()
			{
				return children.size();
			}

			PassRefPtr<Control> Control::GetChild(cint index)
			{
				return children[index];
			}

			bool Control::AddChild(PassRefPtr<Control> control)
			{
				return GetContainerComposition()->AddChild(control->GetBoundsComposition());
			}

			bool Control::HasChild(PassRefPtr<Control> control)
			{
				return find(children.begin(), children.end(), control) != children.end();
			}

			PassRefPtr<ControlHost> Control::GetRelatedControlHost()
			{
				return parent ? parent->GetRelatedControlHost() : nullptr;
			}

			bool Control::GetVisuallyEnabled()
			{
				return isVisuallyEnabled;
			}

			bool Control::GetEnabled()
			{
				return isEnabled;
			}

			void Control::SetEnabled(bool value)
			{
				if ((isEnabled && !value) || (!isEnabled && value))
				{
					isEnabled = value;
					EnabledChanged.Execute(GetNotifyEventArguments());
					UpdateVisuallyEnabled();
				}
			}

			bool Control::GetVisible()
			{
				return isVisible;
			}

			void Control::SetVisible(bool value)
			{
				boundsComposition->SetVisible(value);
				if ((isVisible && !value) || (!isVisible && value))
				{
					isVisible = value;
					VisibleChanged.Execute(GetNotifyEventArguments());
				}
			}

			const CString& Control::GetText()
			{
				return text;
			}

			void Control::SetText(const CString& value)
			{
				if (text != value)
				{
					text = value;
					styleController->SetText(text);
					TextChanged.Execute(GetNotifyEventArguments());
				}
			}

			const Font& Control::GetFont()
			{
				return font;
			}

			void Control::SetFont(const Font& value)
			{
				if (font != value)
				{
					font = value;
					styleController->SetFont(font);
					FontChanged.Execute(GetNotifyEventArguments());
				}
			}

			void Control::SetFocus()
			{
				if (focusableComposition)
				{
					RefPtr<GraphicsHost> host = focusableComposition->GetRelatedGraphicsHost();
					if (host)
					{
						host->SetFocus(focusableComposition);
					}
				}
			}

			PassRefPtr<Control> Control::GetTooltipControl()
			{
				return tooltipControl;
			}

			PassRefPtr<Control> Control::SetTooltipControl(PassRefPtr<Control> value)
			{
				RefPtr<Control> oldTooltipControl = tooltipControl;
				tooltipControl = value;
				return oldTooltipControl;
			}

			cint Control::GetTooltipWidth()
			{
				return tooltipWidth;
			}

			void Control::SetTooltipWidth(cint value)
			{
				tooltipWidth = value;
			}

			bool Control::DisplayTooltip(CPoint location)
			{
				if (!tooltipControl)
					return false;
				GetStorage()->GetApplication()->ShowTooltip(this, tooltipControl, tooltipWidth, location);
				return true;
			}

			void Control::CloseTooltip()
			{
				if (GetStorage()->GetApplication()->GetTooltipOwner() == this)
				{
					GetStorage()->GetApplication()->CloseTooltip();
				}
			}

			void Control::OnChildInserted(PassRefPtr<Control> control)
			{
				RefPtr<Control> _control = control;
				RefPtr<Control> oldParent = _control->parent;
				children.push_back(_control);
				_control->parent = this;
				_control->OnParentChanged(oldParent, _control->parent);
				_control->UpdateVisuallyEnabled();
			}

			void Control::OnChildRemoved(PassRefPtr<Control> control)
			{
				RefPtr<Control> _control = control;
				RefPtr<Control> oldParent = _control->parent;
				_control->parent = nullptr;
				children.erase(find(children.begin(), children.end(), _control));
				_control->OnParentChanged(oldParent, _control->parent);
			}

			void Control::OnParentChanged(PassRefPtr<Control> oldParent, PassRefPtr<Control> newParent)
			{
				OnParentLineChanged();
			}

			void Control::OnParentLineChanged()
			{
				for (auto & child : children)
				{
					child->OnParentLineChanged();
				}
			}

			void Control::OnRenderTargetChanged(PassRefPtr<IGraphicsRenderTarget> renderTarget)
			{

			}

			void Control::OnBeforeReleaseGraphicsHost()
			{
				for (auto & child : children)
				{
					child->OnBeforeReleaseGraphicsHost();
				}
			}

			void Control::UpdateVisuallyEnabled()
			{
				bool newValue = isEnabled && (!parent ? true : parent->GetVisuallyEnabled());
				if (isVisuallyEnabled != newValue)
				{
					isVisuallyEnabled = newValue;
					styleController->SetVisuallyEnabled(isVisuallyEnabled);
					VisuallyEnabledChanged.Execute(GetNotifyEventArguments());

					for (auto & child : children)
					{
						child->UpdateVisuallyEnabled();
					}
				}
			}

			void Control::SetFocusableComposition(PassRefPtr<Composition> value)
			{
				if (focusableComposition != value)
				{
					focusableComposition = value;
					styleController->SetFocusableComposition(focusableComposition);
				}
			}

			Object* Control::QueryService(UINT nId)
			{
				if (parent)
				{
					return parent->QueryService(nId);
				}
				else
				{
					return nullptr;
				}
			}

			Component::Component()
			{

			}

			Component::~Component()
			{

			}

			void Component::Attach(ControlHostRoot* rootObject)
			{

			}

			void Component::Detach(ControlHostRoot* rootObject)
			{

			}

			ControlHostRoot::ControlHostRoot()
			{

			}

			ControlHostRoot::~ControlHostRoot()
			{

			}

			bool ControlHostRoot::AddComponent(PassRefPtr<Component> component)
			{
				RefPtr<Component> _component = component;
				if (!components.insert(_component).second)
				{
					return false;
				}
				else
				{
					_component->Attach(this);
					return true;
				}
			}

			bool ControlHostRoot::RemoveComponent(PassRefPtr<Component> component)
			{
				RefPtr<Component> _component = component;
				if (components.erase(_component) == 1)
				{
					_component->Detach(this);
					return true;
				}
				return false;
			}

			bool ControlHostRoot::ContainsComponent(PassRefPtr<Component> component)
			{
				return components.find(component) != components.end();
			}

			void ControlHostRoot::ClearComponents()
			{
				for (auto & component : components)
				{
					component->Detach(this);
				}
				components.clear();
			}

			void ControlHostRoot::FinalizeInstance()
			{
				ClearComponents();
			}

			ControlHost::ControlHost(PassRefPtr<IControlStyleController> _styleController)
				: Control(_styleController.get())
			{
				GetStyleController()->GetBoundsComposition()->SetAlignmentToParent(CRect());

				WindowGotFocus.SetAssociatedComposition(GetStyleController()->GetBoundsComposition());
				WindowLostFocus.SetAssociatedComposition(GetStyleController()->GetBoundsComposition());
				WindowActivated.SetAssociatedComposition(GetStyleController()->GetBoundsComposition());
				WindowDeactivated.SetAssociatedComposition(GetStyleController()->GetBoundsComposition());
				WindowOpened.SetAssociatedComposition(GetStyleController()->GetBoundsComposition());
				WindowClosing.SetAssociatedComposition(GetStyleController()->GetBoundsComposition());
				WindowClosed.SetAssociatedComposition(GetStyleController()->GetBoundsComposition());
				WindowDestroying.SetAssociatedComposition(GetStyleController()->GetBoundsComposition());

				host = adoptRef(new GraphicsHost);
				host->GetMainComposition()->AddChild(GetStyleController()->GetBoundsComposition());
			}

			ControlHost::~ControlHost()
			{
				OnBeforeReleaseGraphicsHost();
				FinalizeInstance();
				host->GetMainComposition()->RemoveChild(GetStyleController()->GetBoundsComposition());
				styleController = nullptr;
				host = nullptr;
			}

			PassRefPtr<GraphicsHost> ControlHost::GetGraphicsHost()
			{
				return host;
			}

			PassRefPtr<Composition> ControlHost::GetMainComposition()
			{
				return host->GetMainComposition();
			}

			PassRefPtr<IWindow> ControlHost::GetWindow()
			{
				return host->GetWindow();
			}

			void ControlHost::SetWindow(PassRefPtr<IWindow> window)
			{
				if (host->GetWindow())
				{
					host->GetWindow()->UninstallListener(this);
				}
				host->SetWindow(window);
				if (host->GetWindow())
				{
					host->GetWindow()->InstallListener(this);
				}
				OnWindowChanged();
			}

			void ControlHost::ForceCalculateSizeImmediately()
			{
				boundsComposition->ForceCalculateSizeImmediately();
				SetBounds(GetBounds());
			}

			bool ControlHost::GetEnabled()
			{
				if (host->GetWindow())
				{
					return host->GetWindow()->IsEnabled();
				}
				else
				{
					return false;
				}
			}

			void ControlHost::SetEnabled(bool value)
			{
				if (host->GetWindow())
				{
					if (value)
					{
						host->GetWindow()->Enable();
					}
					else
					{
						host->GetWindow()->Disable();
					}
				}
			}

			bool ControlHost::GetFocused()
			{
				if (host->GetWindow())
				{
					return host->GetWindow()->IsFocused();
				}
				else
				{
					return false;
				}
			}

			void ControlHost::SetFocused()
			{
				if (host->GetWindow())
				{
					host->GetWindow()->SetFocus();
				}
			}

			bool ControlHost::GetActivated()
			{
				if (host->GetWindow())
				{
					return host->GetWindow()->IsActivated();
				}
				else
				{
					return false;
				}
			}

			void ControlHost::SetActivated()
			{
				if (host->GetWindow())
				{
					host->GetWindow()->SetActivate();
				}
			}

			bool ControlHost::GetShowInTaskBar()
			{
				if (host->GetWindow())
				{
					return host->GetWindow()->IsAppearedInTaskBar();
				}
				else
				{
					return false;
				}
			}

			void ControlHost::SetShowInTaskBar(bool value)
			{
				if (host->GetWindow())
				{
					if (value)
					{
						host->GetWindow()->ShowInTaskBar();
					}
					else
					{
						host->GetWindow()->HideInTaskBar();
					}
				}
			}

			bool ControlHost::GetEnabledActivate()
			{
				if (host->GetWindow())
				{
					return host->GetWindow()->IsEnabledActivate();
				}
				else
				{
					return false;
				}
			}

			void ControlHost::SetEnabledActivate(bool value)
			{
				if (host->GetWindow())
				{
					if (value)
					{
						host->GetWindow()->EnableActivate();
					}
					else
					{
						host->GetWindow()->DisableActivate();
					}
				}
			}

			bool ControlHost::GetTopMost()
			{
				if (host->GetWindow())
				{
					return host->GetWindow()->GetTopMost();
				}
				else
				{
					return false;
				}
			}

			void ControlHost::SetTopMost(bool topmost)
			{
				if (host->GetWindow())
				{
					host->GetWindow()->SetTopMost(topmost);
				}
			}

			PassRefPtr<IShortcutKeyManager> ControlHost::GetShortcutKeyManager()
			{
				return host->GetShortcutKeyManager();
			}

			void ControlHost::SetShortcutKeyManager(PassRefPtr<IShortcutKeyManager> manager)
			{
				host->SetShortcutKeyManager(manager);
			}

			PassRefPtr<GraphicsAnimationManager> ControlHost::GetAnimationManager()
			{
				return host->GetAnimationManager();
			}

			CSize ControlHost::GetClientSize()
			{
				if (host->GetWindow())
				{
					return host->GetWindow()->GetClientSize();
				}
				else
				{
					return CSize();
				}
			}

			void ControlHost::SetClientSize(CSize value)
			{
				if (host->GetWindow())
				{
					host->GetWindow()->SetClientSize(value);
				}
			}

			CRect ControlHost::GetBounds()
			{
				if (host->GetWindow())
				{
					return host->GetWindow()->GetBounds();
				}
				else
				{
					return CRect();
				}
			}

			void ControlHost::SetBounds(CRect value)
			{
				if (host->GetWindow())
				{
					host->GetWindow()->SetBounds(value);
				}
			}

			PassRefPtr<ControlHost> ControlHost::GetRelatedControlHost()
			{
				return this;
			}

			const CString& ControlHost::GetText()
			{
				CString result;
				if (host->GetWindow())
				{
					result = host->GetWindow()->GetTitle();
				}
				if (result != Control::GetText())
				{
					Control::SetText(result);
				}
				return Control::GetText();
			}

			void ControlHost::SetText(const CString& value)
			{
				if (host->GetWindow())
				{
					host->GetWindow()->SetTitle(value);
					Control::SetText(value);
				}
			}

			PassRefPtr<IScreen> ControlHost::GetRelatedScreen()
			{
				if (host->GetWindow())
				{
					return GetStorage()->GetController()->GetScreenService()->GetScreen(host->GetWindow());
				}
				else
				{
					return nullptr;
				}
			}

			void ControlHost::Show()
			{
				if (host->GetWindow())
				{
					host->GetWindow()->Show();
				}
			}

			void ControlHost::Show(int nCmdShow)
			{
				if (host->GetWindow())
				{
					host->GetWindow()->Show(nCmdShow);
				}
			}

			void ControlHost::ShowDeactivated()
			{
				if (host->GetWindow())
				{
					host->GetWindow()->ShowDeactivated();
				}
			}

			void ControlHost::ShowRestored()
			{
				if (host->GetWindow())
				{
					host->GetWindow()->ShowRestored();
				}
			}

			void ControlHost::ShowMaximized()
			{
				if (host->GetWindow())
				{
					host->GetWindow()->ShowMaximized();
				}
			}

			void ControlHost::ShowMinimized()
			{
				if (host->GetWindow())
				{
					host->GetWindow()->ShowMinimized();
				}
			}

			void ControlHost::Hide()
			{
				if (host->GetWindow())
				{
					host->GetWindow()->Hide();
				}
			}

			void ControlHost::Close()
			{
				RefPtr<IWindow> window = host->GetWindow();
				if (window)
				{
					if (GetStorage()->GetController()->GetWindowService()->GetMainWindow() != window)
					{
						window->Hide();
					}
					else
					{
						SetWindow(nullptr);
						GetStorage()->GetController()->GetWindowService()->DestroyWindow(window);
					}
				}
			}

			bool ControlHost::GetOpening()
			{
				RefPtr<IWindow> window = host->GetWindow();
				if (window)
				{
					return window->IsVisible();
				}
				return false;
			}

			void ControlHost::OnWindowChanged()
			{

			}

			void ControlHost::OnVisualStatusChanged()
			{

			}

			PassRefPtr<Control> ControlHost::GetTooltipOwner(CPoint location)
			{
				RefPtr<Composition> composition = GetBoundsComposition()->FindComposition(location);
				if (composition)
				{
					RefPtr<Control> control = composition->GetRelatedControl();
					while (control)
					{
						if (control->GetTooltipControl())
						{
							return control;
						}
						control = control->GetParent();
					}
				}
				return nullptr;
			}

			void ControlHost::MoveIntoTooltipControl(PassRefPtr<Control> tooltipControl, CPoint location)
			{
				if (tooltipLocation != location)
				{
					tooltipLocation = location;
					{
						RefPtr<Control> currentOwner = GetStorage()->GetApplication()->GetTooltipOwner();
						if (currentOwner && currentOwner != tooltipControl)
						{
							if (tooltipCloseDelay)
							{
								tooltipCloseDelay->Cancel();
								tooltipCloseDelay = nullptr;
							}
							GetStorage()->GetApplication()->DelayExecuteInMainThread([=]()
							{
								currentOwner->CloseTooltip();
							}, TooltipDelayCloseTime);
						}
					}
					if (!tooltipControl)
					{
						if (tooltipOpenDelay)
						{
							tooltipOpenDelay->Cancel();
							tooltipOpenDelay = nullptr;
						}
					}
					else if (tooltipOpenDelay)
					{
						tooltipOpenDelay->Delay(TooltipDelayOpenTime);
					}
					else if (GetStorage()->GetApplication()->GetTooltipOwner() != tooltipControl)
					{
						tooltipOpenDelay = GetStorage()->GetApplication()->DelayExecuteInMainThread([this]()
						{
							RefPtr<Control> owner = GetTooltipOwner(tooltipLocation);
							if (owner)
							{
								CPoint offset = owner->GetBoundsComposition()->GetGlobalBounds().TopLeft();
								CPoint pt(tooltipLocation.x - offset.x, tooltipLocation.y - offset.y + 24);
								owner->DisplayTooltip(pt);
								tooltipOpenDelay = nullptr;
								tooltipCloseDelay = GetStorage()->GetApplication()->DelayExecuteInMainThread([this, owner]()
								{
									owner->CloseTooltip();
								}, TooltipDelayLifeTime);
							}
						}, TooltipDelayOpenTime);
					}
				}
			}

			void ControlHost::MouseMoving(const MouseInfo& info)
			{
				if (!info.left && !info.middle && !info.right)
				{
					RefPtr<Control> tooltipControl = GetTooltipOwner(tooltipLocation);
					MoveIntoTooltipControl(tooltipControl, info.pt);
				}
			}

			void ControlHost::MouseLeaved()
			{
				MoveIntoTooltipControl(nullptr, CPoint(-1, -1));
			}

			void ControlHost::Moved()
			{
				OnVisualStatusChanged();
			}

			void ControlHost::Enabled()
			{
				Control::SetEnabled(true);
				OnVisualStatusChanged();
			}

			void ControlHost::Disabled()
			{
				Control::SetEnabled(false);
				OnVisualStatusChanged();
			}

			void ControlHost::GotFocus()
			{
				WindowGotFocus.Execute(GetNotifyEventArguments());
				OnVisualStatusChanged();
			}

			void ControlHost::LostFocus()
			{
				WindowLostFocus.Execute(GetNotifyEventArguments());
				OnVisualStatusChanged();
			}

			void ControlHost::Activated()
			{
				WindowActivated.Execute(GetNotifyEventArguments());
				OnVisualStatusChanged();
			}

			void ControlHost::Deactivated()
			{
				WindowDeactivated.Execute(GetNotifyEventArguments());
				OnVisualStatusChanged();
			}

			void ControlHost::Opened()
			{
				WindowOpened.Execute(GetNotifyEventArguments());
			}

			void ControlHost::Closing(bool& cancel)
			{
				RequestEventArgs arguments(GetStyleController()->GetBoundsComposition());
				arguments.cancel = cancel;
				WindowClosing.Execute(arguments);
				if (!arguments.handled)
				{
					cancel = arguments.cancel;
				}
			}

			void ControlHost::Closed()
			{
				WindowClosed.Execute(GetNotifyEventArguments());
			}

			void ControlHost::Destroying()
			{
				WindowDestroying.Execute(GetNotifyEventArguments());
				SetWindow(nullptr);
			}

			GraphicsHost::GraphicsHost()
				: lastCaretTime(0)
				, CaretInterval(500)
			{
				animationManager = adoptRef(new GraphicsAnimationManager);
				windowComposition = adoptRef(new WindowComposition);
				windowComposition->SetAssociatedHost(this);
				windowComposition->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
			}

			GraphicsHost::~GraphicsHost()
			{
				windowComposition = nullptr;
			}

			PassRefPtr<IWindow> GraphicsHost::GetWindow()
			{
				return window;
			}

			void GraphicsHost::SetWindow(PassRefPtr<IWindow> _window)
			{
				if (window != _window)
				{
					if (window)
					{
						GetStorage()->GetController()->GetCallbackService()->UninstallListener(this);
						window->UninstallListener(this);
					}
					window = _window;
					windowComposition->SetAttachedWindow(window);
					if (window)
					{
						window->InstallListener(this);
						GetStorage()->GetController()->GetCallbackService()->InstallListener(this);
						previousClientSize = window->GetClientSize();
						minSize = windowComposition->GetPreferredBounds().Size();
						window->SetCaretPoint(caretPoint);
					}
				}
			}

			PassRefPtr<Composition> GraphicsHost::GetMainComposition()
			{
				return windowComposition;
			}

			void GraphicsHost::Render()
			{
				RefPtr<IGraphicsRenderTarget> renderTarget = windowComposition->GetRenderTarget();
				if (window && window->IsVisible() && renderTarget)
				{
					bool success = renderTarget->StartRendering();
					if (!success)
						return;

					windowComposition->Render(CSize());
					HRESULT result = renderTarget->StopRendering();
					window->RedrawContent();

					if (FAILED(result))
					{
						if (result == D2DERR_RECREATE_TARGET)
						{
							windowComposition->SetAttachedWindow(nullptr);
							GetStorage()->GetGraphicsResourceManager()->RecreateRenderTarget(window);
							windowComposition->SetAttachedWindow(window);
						}
						else
						{
							ASSERT(!"D2DERR");
						}
					}
				}
			}

			PassRefPtr<IShortcutKeyManager> GraphicsHost::GetShortcutKeyManager()
			{
				return shortcutKeyManager;
			}

			void GraphicsHost::SetShortcutKeyManager(PassRefPtr<IShortcutKeyManager> manager)
			{
				shortcutKeyManager = manager;
			}

			bool GraphicsHost::SetFocus(PassRefPtr<Composition> composition)
			{
				RefPtr<Composition> _composition = composition;
				if (!_composition || _composition->GetRelatedGraphicsHost() != this)
				{
					return false;
				}
				if (focusedComposition && focusedComposition->HasEventReceiver())
				{
					EventArgs arguments;
					arguments.compositionSource = focusedComposition;
					arguments.eventSource = focusedComposition;
					focusedComposition->GetEventReceiver()->lostFocus.Execute(arguments);
				}
				focusedComposition = _composition;
				SetCaretPoint(CPoint());
				if (focusedComposition && focusedComposition->HasEventReceiver())
				{
					EventArgs arguments;
					arguments.compositionSource = focusedComposition;
					arguments.eventSource = focusedComposition;
					focusedComposition->GetEventReceiver()->gotFocus.Execute(arguments);
				}
				return true;
			}

			PassRefPtr<Composition> GraphicsHost::GetFocusedComposition()
			{
				return focusedComposition;
			}

			CPoint GraphicsHost::GetCaretPoint()
			{
				return caretPoint;
			}

			void GraphicsHost::SetCaretPoint(CPoint value, PassRefPtr<Composition> referenceComposition /*= nullptr*/)
			{
				if (referenceComposition)
				{
					CRect bounds = referenceComposition->GetGlobalBounds();
					value += bounds.TopLeft();
				}
				caretPoint = value;
				if (window)
				{
					window->SetCaretPoint(caretPoint);
				}
			}

			PassRefPtr<GraphicsAnimationManager> GraphicsHost::GetAnimationManager()
			{
				return animationManager;
			}

			void GraphicsHost::DisconnectComposition(PassRefPtr<Composition> composition)
			{
				DisconnectCompositionInternal(composition);
			}

			void GraphicsHost::DisconnectCompositionInternal(PassRefPtr<Composition> composition)
			{
				RefPtr<Composition> _composition = composition;
				for (auto & child : _composition->Children())
				{
					DisconnectCompositionInternal(child);
				}
				if (mouseCaptureComposition == _composition)
				{
					if (window)
					{
						window->ReleaseCapture();
					}
					mouseCaptureComposition = nullptr;
				}
				if (focusedComposition == _composition)
				{
					focusedComposition = nullptr;
				}
				{
					auto found = find(mouseEnterCompositions.begin(), mouseEnterCompositions.end(), _composition);
					if (found != mouseEnterCompositions.end())
					{
						mouseEnterCompositions.erase(found);
					}
				}
			}

			void GraphicsHost::MouseCapture(const MouseInfo& info)
			{
				if (window && (info.left || info.middle || info.right))
				{
					if (!window->IsCapturing() && !info.nonClient)
					{
						window->RequireCapture();
						mouseCaptureComposition = windowComposition->FindComposition(info.pt);
					}
				}
			}

			void GraphicsHost::MouseUncapture(const MouseInfo& info)
			{
				if (window && !(info.left || info.middle || info.right))
				{
					window->ReleaseCapture();
					mouseCaptureComposition = nullptr;
				}
			}

			void GraphicsHost::OnCharInput(const CharInfo& info, PassRefPtr<Composition> composition, CharEvent EventReceiver::* eventReceiverEvent)
			{
				vector<RefPtr<Composition>> compositions;
				RefPtr<Composition> _composition = composition;
				while (_composition)
				{
					if (_composition->HasEventReceiver())
					{
						compositions.push_back(_composition);
					}
					_composition = _composition->GetParent();
				}

				CharEventArgs arguments(_composition);
				(CharInfo&)arguments = info;

				for (auto it = compositions.rbegin(); it != compositions.rend(); it++)
				{
					(*it)->GetEventReceiver()->previewCharInput.Execute(arguments);
					if (arguments.handled)
					{
						return;
					}
				}

				for (auto & __composition : compositions)
				{
					(__composition->GetEventReceiver().get()->*eventReceiverEvent).Execute(arguments);
					if (arguments.handled)
					{
						return;
					}
				}
			}

			void GraphicsHost::OnKeyInput(const KeyInfo& info, PassRefPtr<Composition> composition, KeyEvent EventReceiver::* eventReceiverEvent)
			{
				vector<RefPtr<Composition>> compositions;
				RefPtr<Composition> _composition = composition;
				while (_composition)
				{
					if (_composition->HasEventReceiver())
					{
						compositions.push_back(_composition);
					}
					_composition = _composition->GetParent();
				}

				KeyEventArgs arguments(_composition);
				(KeyInfo&)arguments = info;

				for (auto it = compositions.rbegin(); it != compositions.rend(); it++)
				{
					(*it)->GetEventReceiver()->previewKey.Execute(arguments);
					if (arguments.handled)
					{
						return;
					}
				}

				for (auto & __composition : compositions)
				{
					(__composition->GetEventReceiver().get()->*eventReceiverEvent).Execute(arguments);
					if (arguments.handled)
					{
						return;
					}
				}
			}

			void GraphicsHost::RaiseMouseEvent(MouseEventArgs& arguments, PassRefPtr<Composition> composition, MouseEvent EventReceiver::* eventReceiverEvent)
			{
				RefPtr<Composition> _composition = composition;
				arguments.compositionSource = _composition;
				arguments.eventSource = nullptr;
				CPoint pt = arguments.pt;

				while (_composition)
				{
					if (_composition->HasEventReceiver())
					{
						if (!arguments.eventSource)
						{
							arguments.eventSource = _composition;
						}
						RefPtr<EventReceiver> eventReceiver = _composition->GetEventReceiver();
						(eventReceiver.get()->*eventReceiverEvent).Execute(arguments);
						if (arguments.handled)
						{
							break;
						}
					}

					RefPtr<Composition> parent = _composition->GetParent();
					if (parent)
					{
						CRect parentBounds = parent->GetBounds();
						CRect clientArea = parent->GetClientArea();
						CRect childBounds = _composition->GetBounds();

						pt += childBounds.TopLeft() + clientArea.TopLeft() - parentBounds.TopLeft();
						arguments.pt = pt;
					}
					_composition = parent;
				}
			}

			void GraphicsHost::OnMouseInput(const MouseInfo& info, MouseEvent EventReceiver::* eventReceiverEvent)
			{
				RefPtr<Composition> _composition;
				if (mouseCaptureComposition)
				{
					_composition = mouseCaptureComposition;
				}
				else
				{
					_composition = windowComposition->FindComposition(info.pt);
				}
				if (_composition)
				{
					CRect bounds = _composition->GetGlobalBounds();
					MouseEventArgs arguments;
					(MouseInfo&)arguments = info;
					arguments.pt -= bounds.TopLeft();
					RaiseMouseEvent(arguments, _composition, eventReceiverEvent);
				}
			}

			IWindowListener::HitTestResult GraphicsHost::HitTest(CPoint location)
			{
				CRect bounds = window->GetBounds();
				CRect clientBounds = window->GetClientBoundsInScreen();
				CPoint clientLocation = location + bounds.TopLeft() - clientBounds.TopLeft();
				RefPtr<Composition> hitComposition = windowComposition->FindComposition(clientLocation);
				while (hitComposition)
				{
					IWindowListener::HitTestResult result = hitComposition->GetAssociatedHitTestResult();
					if (result == IWindowListener::NoDecision)
					{
						hitComposition = hitComposition->GetParent();
					}
					else
					{
						return result;
					}
				}
				return IWindowListener::NoDecision;
			}

			void GraphicsHost::Moving(CRect& bounds, bool fixSizeOnly)
			{
				CRect oldBounds = window->GetBounds();
				minSize = windowComposition->GetPreferredBounds().Size();
				CSize minWindowSize = minSize + (oldBounds.Size() - window->GetClientSize());
				if (bounds.Width() < minWindowSize.cx)
				{
					if (fixSizeOnly)
					{
						if (bounds.Width() < minWindowSize.cx)
						{
							bounds.right = bounds.left + minWindowSize.cx;
						}
					}
					else if (oldBounds.left != bounds.left)
					{
						bounds.left = oldBounds.right - minWindowSize.cx;
					}
					else if (oldBounds.right != bounds.right)
					{
						bounds.right = oldBounds.left + minWindowSize.cx;
					}
				}
				if (bounds.Height() < minWindowSize.cy)
				{
					if (fixSizeOnly)
					{
						if (bounds.Height() < minWindowSize.cy)
						{
							bounds.bottom = bounds.top + minWindowSize.cy;
						}
					}
					else if (oldBounds.top != bounds.top)
					{
						bounds.top = oldBounds.bottom - minWindowSize.cy;
					}
					else if (oldBounds.bottom != bounds.bottom)
					{
						bounds.bottom = oldBounds.top + minWindowSize.cy;
					}
				}
			}

			void GraphicsHost::Moved()
			{
				CSize size = window->GetClientSize();
				if (previousClientSize != size)
				{
					previousClientSize = size;
					minSize = windowComposition->GetPreferredBounds().Size();
					Render();
				}
			}

			void GraphicsHost::LeftButtonDown(const MouseInfo& info)
			{
				MouseCapture(info);
				OnMouseInput(info, &EventReceiver::leftButtonDown);
			}

			void GraphicsHost::LeftButtonUp(const MouseInfo& info)
			{
				OnMouseInput(info, &EventReceiver::leftButtonUp);
				MouseUncapture(info);
			}

			void GraphicsHost::LeftButtonDoubleClick(const MouseInfo& info)
			{
				LeftButtonDown(info);
				OnMouseInput(info, &EventReceiver::leftButtonDoubleClick);
			}

			void GraphicsHost::RightButtonDown(const MouseInfo& info)
			{
				MouseCapture(info);
				OnMouseInput(info, &EventReceiver::rightButtonDown);
			}

			void GraphicsHost::RightButtonUp(const MouseInfo& info)
			{
				OnMouseInput(info, &EventReceiver::rightButtonUp);
				MouseUncapture(info);
			}

			void GraphicsHost::RightButtonDoubleClick(const MouseInfo& info)
			{
				RightButtonDown(info);
				OnMouseInput(info, &EventReceiver::rightButtonDoubleClick);
			}

			void GraphicsHost::MiddleButtonDown(const MouseInfo& info)
			{
				MouseCapture(info);
				OnMouseInput(info, &EventReceiver::middleButtonDown);
			}

			void GraphicsHost::MiddleButtonUp(const MouseInfo& info)
			{
				OnMouseInput(info, &EventReceiver::middleButtonUp);
				MouseUncapture(info);
			}

			void GraphicsHost::MiddleButtonDoubleClick(const MouseInfo& info)
			{
				MiddleButtonDown(info);
				OnMouseInput(info, &EventReceiver::middleButtonDoubleClick);
			}

			void GraphicsHost::HorizontalWheel(const MouseInfo& info)
			{
				OnMouseInput(info, &EventReceiver::horizontalWheel);
			}

			void GraphicsHost::VerticalWheel(const MouseInfo& info)
			{
				OnMouseInput(info, &EventReceiver::verticalWheel);
			}

			void GraphicsHost::MouseMoving(const MouseInfo& info)
			{
				CompositionList newCompositions;
				{
					RefPtr<Composition> _composition = windowComposition->FindComposition(info.pt);
					while (_composition)
					{
						newCompositions.push_back(_composition);
						_composition = _composition->GetParent();
					}
				}
				reverse(newCompositions.begin(), newCompositions.end());

				cint newCompositionSize = (cint)newCompositions.size();
				cint mouseEnterCompositionSize = (cint)mouseEnterCompositions.size();
				cint firstDifferentIndex = mouseEnterCompositionSize;
				for (cint i = 0; i < mouseEnterCompositionSize; i++)
				{
					if (i == newCompositionSize)
					{
						firstDifferentIndex = newCompositionSize;
						break;
					}
					if (mouseEnterCompositions[i] != newCompositions[i])
					{
						firstDifferentIndex = i;
						break;
					}
				}

				for (cint i = mouseEnterCompositionSize - 1; i >= firstDifferentIndex; i--)
				{
					RefPtr<Composition> _composition = mouseEnterCompositions[i];
					if (_composition->HasEventReceiver())
					{
						_composition->GetEventReceiver()->mouseLeave.Execute(EventArgs(_composition));
					}
				}

				mouseEnterCompositions = newCompositions;
				mouseEnterCompositionSize = (cint)mouseEnterCompositions.size();
				for (cint i = firstDifferentIndex; i < mouseEnterCompositionSize; i++)
				{
					RefPtr<Composition> _composition = mouseEnterCompositions[i];
					if (_composition->HasEventReceiver())
					{
						_composition->GetEventReceiver()->mouseEnter.Execute(EventArgs(_composition));
					}
				}

				RefPtr<ICursor> cursor;
				if (!newCompositions.empty())
				{
					cursor = (newCompositions.back())->GetRelatedCursor();
				}
				if (cursor)
				{
					window->SetCursor(cursor);
				}
				else
				{
					window->SetCursor(GetStorage()->GetController()->GetResourceService()->GetDefaultSystemCursor());
				}

				OnMouseInput(info, &EventReceiver::mouseMove);
			}

			void GraphicsHost::MouseEntered()
			{

			}

			void GraphicsHost::MouseLeaved()
			{
				for (auto it = mouseEnterCompositions.rbegin(); it != mouseEnterCompositions.rend(); it++)
				{
					RefPtr<Composition>& _composition = *it;
					if (_composition->HasEventReceiver())
					{
						_composition->GetEventReceiver()->mouseLeave.Execute(EventArgs(_composition));
					}
				}
				mouseEnterCompositions.clear();
			}

			void GraphicsHost::KeyDown(const KeyInfo& info)
			{
				if (shortcutKeyManager && shortcutKeyManager->Execute(info))
				{
					return;
				}
				if (focusedComposition && focusedComposition->HasEventReceiver())
				{
					OnKeyInput(info, focusedComposition, &EventReceiver::keyDown);
				}
			}

			void GraphicsHost::KeyUp(const KeyInfo& info)
			{
				if (focusedComposition && focusedComposition->HasEventReceiver())
				{
					OnKeyInput(info, focusedComposition, &EventReceiver::keyUp);
				}
			}

			void GraphicsHost::SysKeyDown(const KeyInfo& info)
			{
				if (focusedComposition && focusedComposition->HasEventReceiver())
				{
					OnKeyInput(info, focusedComposition, &EventReceiver::systemKeyDown);
				}
			}

			void GraphicsHost::SysKeyUp(const KeyInfo& info)
			{
				if (focusedComposition && focusedComposition->HasEventReceiver())
				{
					OnKeyInput(info, focusedComposition, &EventReceiver::systemKeyUp);
				}
			}

			void GraphicsHost::Char(const CharInfo& info)
			{
				if (focusedComposition && focusedComposition->HasEventReceiver())
				{
					OnCharInput(info, focusedComposition, &EventReceiver::charInput);
				}
			}

			void GraphicsHost::GlobalTimer()
			{
				if (animationManager->HasAnimation() && windowComposition->GetRenderTarget())
				{
					animationManager->Play();
				}

				auto now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

				if (now - lastCaretTime >= CaretInterval)
				{
					lastCaretTime = now;
					if (focusedComposition && focusedComposition->HasEventReceiver())
					{
						focusedComposition->GetEventReceiver()->caretNotify.Execute(EventArgs(focusedComposition));
					}
				}

				Render();
			}

			Window::Window(PassRefPtr<IWindowStyleController> _styleController)
				: ControlHost(_styleController.get())
				, styleController(_styleController.get())
			{
				RefPtr<IWindow> window = GetStorage()->GetController()->GetWindowService()->CreatWindow(_styleController->ShowShadow());
				styleController->AttachWindow(this);
				SetWindow(window);
				GetStorage()->GetApplication()->RegisterWindow(this);
				ClipboardUpdated.SetAssociatedComposition(GetBoundsComposition());
			}

			Window::~Window()
			{
				GetStorage()->GetApplication()->UnregisterWindow(this);
				RefPtr<IWindow> window = host->GetWindow();
				if (window)
				{
					SetWindow(nullptr);
					GetStorage()->GetController()->GetWindowService()->DestroyWindow(window);
				}
			}

			void Window::MoveToScreenCenter()
			{
				RefPtr<IScreen> screen = GetRelatedScreen();
				if (screen)
				{
					CRect screenBounds = screen->GetClientBounds();
					CRect windowBounds = GetBounds();
					windowBounds.OffsetRect(CSize((screenBounds.Width() - windowBounds.Width()) / 2, (screenBounds.Height() - windowBounds.Height()) / 2));
					SetBounds(windowBounds);
				}
			}

			bool Window::GetMaximizedBox()
			{
				return styleController->GetMaximizedBox();
			}

			void Window::SetMaximizedBox(bool visible)
			{
				styleController->SetMaximizedBox(visible);
			}

			bool Window::GetMinimizedBox()
			{
				return styleController->GetMinimizedBox();
			}

			void Window::SetMinimizedBox(bool visible)
			{
				styleController->SetMinimizedBox(visible);
			}

			bool Window::GetBorder()
			{
				return styleController->GetBorder();
			}

			void Window::SetBorder(bool visible)
			{
				styleController->SetBorder(visible);
			}

			bool Window::GetSizeBox()
			{
				return styleController->GetSizeBox();
			}

			void Window::SetSizeBox(bool visible)
			{
				styleController->SetSizeBox(visible);
			}

			bool Window::GetIconVisible()
			{
				return styleController->GetIconVisible();
			}

			void Window::SetIconVisible(bool visible)
			{
				styleController->SetIconVisible(visible);
			}

			bool Window::GetTitleBar()
			{
				return styleController->GetTitleBar();
			}

			void Window::SetTitleBar(bool visible)
			{
				styleController->SetTitleBar(visible);
			}

			void Window::ShowModal(PassRefPtr<Window> owner, const function<void()>& callback)
			{
				owner->SetEnabled(false);
				GetWindow()->SetParent(owner->GetWindow());
				auto_ptr<Event<EventArgs>::HandlerContainer> container;
				container.reset(new Event<EventArgs>::HandlerContainer);
				container->handler = WindowClosed.AttachLambda([&](PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					GetStorage()->GetApplication()->InvokeInMainThread([&]()
					{
						WindowClosed.Detach(container->handler);
						GetWindow()->SetParent(nullptr);
						callback();
						owner->SetEnabled(true);
						owner->SetActivated();
					});
				});
				Show();
			}

			void Window::ShowModalAndDelete(PassRefPtr<Window> owner, const function<void()>& callback)
			{
				owner->SetEnabled(false);
				GetWindow()->SetParent(owner->GetWindow());
				WindowClosed.AttachLambda([&](PassRefPtr<Composition> sender, EventArgs& arguments)
				{
					GetStorage()->GetApplication()->InvokeInMainThread([&]()
					{
						GetWindow()->SetParent(nullptr);
						callback();
						owner->SetEnabled(true);
						owner->SetActivated();
						delete this;
					});
				});
				Show();
			}

			void Window::Moved()
			{
				ControlHost::Moved();
				styleController->SetSizeState(GetWindow()->GetSizeState());
			}

			void Window::OnWindowChanged()
			{
				styleController->InitializeWindowProperties();
				ControlHost::OnWindowChanged();
			}

			void Window::OnVisualStatusChanged()
			{
				ControlHost::OnVisualStatusChanged();
			}

			void Window::MouseClickedOnOtherWindow(PassRefPtr<Window> window)
			{

			}

			Popup::Popup(PassRefPtr<IWindowStyleController> _styleController)
				: Window(_styleController)
			{
				SetMinimizedBox(false);
				SetMaximizedBox(false);
				SetSizeBox(false);
				SetTitleBar(false);
				SetShowInTaskBar(false);
				GetBoundsComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);

				WindowOpened.AttachMethod(this, &Popup::PopupOpened);
				WindowClosed.AttachMethod(this, &Popup::PopupClosed);
			}

			Popup::~Popup()
			{
				GetStorage()->GetApplication()->RegisterPopupClosed(this);
			}

			bool Popup::IsClippedByScreen(CPoint location)
			{
				SetBounds(CRect(location, GetBounds().Size()));
				RefPtr<IWindow> window = GetWindow();
				if (window)
				{
					RefPtr<IScreen> screen = GetStorage()->GetController()->GetScreenService()->GetScreen(window);
					if (screen)
					{
						CRect screenBounds = screen->GetClientBounds();
						CRect windowBounds = window->GetBounds();
						return !screenBounds.PtInRect(windowBounds.TopLeft()) || !screenBounds.PtInRect(windowBounds.BottomRight());
					}
				}
				return true;
			}

			void Popup::ShowPopup(CPoint location, PassRefPtr<IScreen> screen)
			{
				RefPtr<IWindow> window = GetWindow();
				if (window)
				{
					RefPtr<IScreen> _screen = screen;
					if (!_screen)
					{
						SetBounds(CRect(location, GetBounds().Size()));
						_screen = GetStorage()->GetController()->GetScreenService()->GetScreen(window);
					}

					if (_screen)
					{
						CRect screenBounds = _screen->GetClientBounds();
						CSize size = window->GetBounds().Size();

						if (location.x < screenBounds.left)
						{
							location.x = screenBounds.left;
						}
						else if (location.x + size.cx > screenBounds.right)
						{
							location.x = screenBounds.right - size.cx;
						}

						if (location.y < screenBounds.top)
						{
							location.y = screenBounds.top;
						}
						else if (location.y + size.cy > screenBounds.bottom)
						{
							location.y = screenBounds.bottom - size.cy;
						}
					}

					SetBounds(CRect(location, GetBounds().Size()));
					ShowDeactivated();
				}
			}

			void Popup::ShowPopup(PassRefPtr<Control> control, CRect bounds, bool preferredTopBottomSide)
			{
				RefPtr<IWindow> window = GetWindow();
				if (window)
				{
					CPoint locations[4];
					CSize size = window->GetBounds().Size();

					RefPtr<ControlHost> controlHost = control->GetBoundsComposition()->GetRelatedControlHost();
					if (controlHost)
					{
						RefPtr<IWindow> controlWindow = controlHost->GetWindow();
						if (controlWindow)
						{
							CPoint controlClientOffset = control->GetBoundsComposition()->GetGlobalBounds().TopLeft();
							CPoint controlWindowOffset = controlWindow->GetClientBoundsInScreen().TopLeft();
							bounds.OffsetRect(controlClientOffset);
							bounds.OffsetRect(controlWindowOffset);

							if (preferredTopBottomSide)
							{
								locations[0] = CPoint(bounds.left, bounds.bottom);
								locations[1] = CPoint(bounds.right - size.cx, bounds.bottom);
								locations[2] = CPoint(bounds.left, bounds.top - size.cy);
								locations[3] = CPoint(bounds.right - size.cx, bounds.top - size.cy);
							}
							else
							{
								locations[0] = CPoint(bounds.right, bounds.top);
								locations[1] = CPoint(bounds.right, bounds.bottom - size.cy);
								locations[2] = CPoint(bounds.left - size.cx, bounds.top);
								locations[3] = CPoint(bounds.left - size.cx, bounds.bottom - size.cy);
							}

							window->SetParent(controlWindow);
							for (cint i = 0; i < 4; i++)
							{
								if (!IsClippedByScreen(locations[i]))
								{
									ShowPopup(locations[i], nullptr);
									return;
								}
							}
							ShowPopup(locations[0], GetStorage()->GetController()->GetScreenService()->GetScreen(controlWindow));
						}
					}
				}
			}

			void Popup::ShowPopup(PassRefPtr<Control> control, CPoint location)
			{
				RefPtr<IWindow> window = GetWindow();
				if (window)
				{
					CPoint locations[4];
					CSize size = window->GetBounds().Size();
					CRect controlBounds = control->GetBoundsComposition()->GetGlobalBounds();

					RefPtr<ControlHost> controlHost = control->GetBoundsComposition()->GetRelatedControlHost();
					if (controlHost)
					{
						RefPtr<IWindow> controlWindow = controlHost->GetWindow();
						if (controlWindow)
						{
							CPoint controlClientOffset = controlWindow->GetClientBoundsInScreen().TopLeft();
							CPoint pt = controlBounds.TopLeft() + controlClientOffset + location;
							window->SetParent(controlWindow);
							ShowPopup(pt, GetStorage()->GetController()->GetScreenService()->GetScreen(controlWindow));
						}
					}
				}
			}

			void Popup::ShowPopup(PassRefPtr<Control> control, bool preferredTopBottomSide)
			{
				RefPtr<IWindow> window = GetWindow();
				if (window)
				{
					CSize size = window->GetBounds().Size();
					CRect bounds(CPoint(), control->GetBoundsComposition()->GetBounds().Size());
					ShowPopup(control, bounds, preferredTopBottomSide);
				}
			}

			void Popup::MouseClickedOnOtherWindow(PassRefPtr<Window> window)
			{
				Hide();
			}

			void Popup::PopupOpened(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				GetStorage()->GetApplication()->RegisterPopupOpened(this);
			}

			void Popup::PopupClosed(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				GetStorage()->GetApplication()->RegisterPopupClosed(this);
				RefPtr<IWindow> window = GetWindow();
				if (window)
				{
					window->SetParent(nullptr);
				}
			}

			Tooltip::Tooltip(PassRefPtr<IWindowStyleController> _styleController)
				: Popup(_styleController)
			{
				GetWindow()->SetAlwaysPassFocusToParent(true);
				GetWindow()->SetNoActivate(true);

				GetContainerComposition()->SetMinSizeLimitation(Composition::LimitToElementAndChildren);
				GetContainerComposition()->SetPreferredMinSize(CSize(20, 10));
				GetStorage()->GetController()->GetCallbackService()->InstallListener(this);

				WindowOpened.AttachMethod(this, &Tooltip::TooltipOpened);
				WindowClosed.AttachMethod(this, &Tooltip::TooltipClosed);
			}

			Tooltip::~Tooltip()
			{
				GetStorage()->GetController()->GetCallbackService()->UninstallListener(this);
			}

			cint Tooltip::GetPreferredContentWidth()
			{
				return GetContainerComposition()->GetPreferredMinSize().cx;
			}

			void Tooltip::SetPreferredContentWidth(cint value)
			{
				GetContainerComposition()->SetPreferredMinSize(CSize(value, 10));
			}

			PassRefPtr<Control> Tooltip::GetTemporaryContentControl()
			{
				return temporaryContentControl;
			}

			void Tooltip::SetTemporaryContentControl(PassRefPtr<Control> control)
			{
				if (temporaryContentControl && HasChild(temporaryContentControl))
				{
					GetContainerComposition()->RemoveChild(temporaryContentControl->GetBoundsComposition());
					temporaryContentControl = nullptr;
				}
				temporaryContentControl = control;
				if (control)
				{
					control->GetBoundsComposition()->SetAlignmentToParent(CRect());
					AddChild(control);
				}
			}

			void Tooltip::GlobalTimer()
			{

			}

			void Tooltip::TooltipOpened(PassRefPtr<Composition> sender, EventArgs& arguments)
			{

			}

			void Tooltip::TooltipClosed(PassRefPtr<Composition> sender, EventArgs& arguments)
			{
				SetTemporaryContentControl(nullptr);
			}
		}
	}
}