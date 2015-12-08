#ifndef CC_INTERFACE
#define CC_INTERFACE

#include "WTF/RefPtr.h"
#include "cc_base.h"

using namespace std;
using namespace cc::base;
using namespace cc::base::direct2d;

namespace cc
{
	namespace presentation
	{
		namespace direct2d
		{
			namespace text
			{
				class CharMeasurer;
				class TextLines;
			}
		}
	}
}

using namespace cc::presentation::direct2d::text;

namespace cc
{
	class Interface : public virtual Object
	{
	public:
		Interface() = default;
		virtual ~Interface() = default;
	};

	namespace interfaces
	{
		namespace windows
		{
			class IGlobalStorage;
			class IScreen;
			class ICursor;
			class IWindow;
			class IWindowListener;
			class IController;
			class IControllerListener;
			class ICallbackService;
			class IResourceService;
			class IDelay;
			class IAsyncService;
			class IClipboardService;
			class IImage;
			class IImageFrame;
			class IImageFrameCache;
			class IImageService;
			class IScreenService;
			class IWindowService;
			class IInputService;
			class IDialogService;
			class IMessageHandler;
			class IForm;
		}

		namespace direct2d
		{
			class IGraphicsElement;
			class IGraphicsElementFactory;
			class IGraphicsRenderer;
			class IGraphicsRendererFactory;
			class IGraphicsRenderTarget;
			class IGraphicsParagraph;
			class IGraphicsLayoutProvider;
			class IGraphicsAnimation;

			class ID2DRenderTarget;
			class ID2DResourceManager;
			class ID2DProvider;

			class ID2DRendererCallback;
		}

		using namespace cc::interfaces::windows;
		using namespace cc::interfaces::direct2d;

		namespace windows
		{
			class IScreen : public virtual Interface
			{
			public:
				virtual CRect				GetBounds() = 0;
				virtual CRect				GetClientBounds() = 0;
				virtual CString				GetName() = 0;
				virtual HMONITOR			GetMonitor() = 0;
				virtual void				SetMonitor(HMONITOR hMonitor) = 0;
				virtual bool				IsPrimary() = 0;
			};

			class ICursor : public virtual Interface
			{
			public:
				enum CursorType
				{
					SmallWaiting,
					LargeWaiting,
					Arrow,
					Cross,
					Hand,
					Help,
					IBeam,
					SizeAll,
					SizeNESW,
					SizeNS,
					SizeNWSE,
					SizeWE,
					LastSystemCursor = SizeWE,
				};

				static const cint			CursorCount = LastSystemCursor + 1;

				virtual bool				IsSystemCursor() = 0;
				virtual ICursor::CursorType	GetSystemCursorType() = 0;
			};

			class IWindow : public Interface
			{
			public:
				virtual CRect				GetBounds() = 0;
				virtual void				SetBounds(const CRect& bounds) = 0;
				virtual CSize				GetClientSize() = 0;
				virtual void				SetClientSize(CSize size) = 0;
				virtual CRect				GetClientBoundsInScreen() = 0;

				virtual CString				GetTitle() = 0;
				virtual void				SetTitle(CString title) = 0;
				virtual PassRefPtr<ICursor>	GetCursor() = 0;
				virtual void				SetCursor(PassRefPtr<ICursor> cursor) = 0;
				virtual CPoint				GetCaretPoint() = 0;
				virtual void				SetCaretPoint(CPoint point) = 0;

				virtual PassRefPtr<IWindow>	GetParent() = 0;
				virtual void				SetParent(PassRefPtr<IWindow> parent) = 0;
				virtual bool				GetAlwaysPassFocusToParent() = 0;
				virtual void				SetAlwaysPassFocusToParent(bool value) = 0;

				virtual void				EnableCustomFrameMode() = 0;
				virtual void				DisableCustomFrameMode() = 0;
				virtual bool				IsCustomFrameModeEnabled() = 0;

				enum WindowSizeState
				{
					Minimized,
					Restored,
					Maximized,
				};

				virtual void				SetNoActivate(bool value) = 0;
				virtual bool				GetExStyle(DWORD exStyle) = 0;
				virtual void				SetExStyle(DWORD exStyle, bool available) = 0;
				virtual bool				GetStyle(DWORD style) = 0;
				virtual void				SetStyle(DWORD style, bool available) = 0;
				virtual bool				GetClassStyle(DWORD style) = 0;
				virtual void				SetClassStyle(DWORD style, bool available) = 0;

				virtual WindowSizeState		GetSizeState() = 0;
				virtual void				Show() = 0;
				virtual void				Show(int nCmdShow) = 0;
				virtual void				ShowDeactivated() = 0;
				virtual void				ShowRestored() = 0;
				virtual void				ShowMaximized() = 0;
				virtual void				ShowMinimized() = 0;
				virtual void				Hide() = 0;
				virtual bool				IsVisible() = 0;

				virtual void				Enable() = 0;
				virtual void				Disable() = 0;
				virtual bool				IsEnabled() = 0;

				virtual void				SetFocus() = 0;
				virtual bool				IsFocused() = 0;
				virtual void				SetActivate() = 0;
				virtual bool				IsActivated() = 0;

				virtual void				ShowInTaskBar() = 0;
				virtual void				HideInTaskBar() = 0;
				virtual bool				IsAppearedInTaskBar() = 0;

				virtual void				EnableActivate() = 0;
				virtual void				DisableActivate() = 0;
				virtual bool				IsEnabledActivate() = 0;

				virtual bool				RequireCapture() = 0;
				virtual bool				ReleaseCapture() = 0;
				virtual bool				IsCapturing() = 0;

				virtual bool				GetMaximizedBox() = 0;
				virtual void				SetMaximizedBox(bool visible) = 0;
				virtual bool				GetMinimizedBox() = 0;
				virtual void				SetMinimizedBox(bool visible) = 0;
				virtual bool				GetBorder() = 0;
				virtual void				SetBorder(bool visible) = 0;
				virtual bool				GetSizeBox() = 0;
				virtual void				SetSizeBox(bool visible) = 0;
				virtual bool				GetIconVisible() = 0;
				virtual void				SetIconVisible(bool visible) = 0;
				virtual bool				GetTitleBar() = 0;
				virtual void				SetTitleBar(bool visible) = 0;
				virtual bool				GetTopMost() = 0;
				virtual void				SetTopMost(bool topmost) = 0;

				virtual void				RedrawContent() = 0;

				virtual bool				InstallListener(PassRefPtr<IWindowListener> listener) = 0;
				virtual bool				UninstallListener(PassRefPtr<IWindowListener> listener) = 0;
			};

			class IWindowListener : public Interface
			{
			public:
				enum HitTestResult
				{
					BorderNoSizing,
					BorderLeft,
					BorderRight,
					BorderTop,
					BorderBottom,
					BorderLeftTop,
					BorderRightTop,
					BorderLeftBottom,
					BorderRightBottom,
					Title,
					ButtonMinimum,
					ButtonMaximum,
					ButtonClose,
					Client,
					Icon,
					NoDecision,
				};

				virtual HitTestResult		HitTest(CPoint location);
				virtual void				Moving(CRect& bounds, bool fixSizeOnly);
				virtual void				Moved();
				virtual void				Enabled();
				virtual void				Disabled();
				virtual void				GotFocus();
				virtual void				LostFocus();
				virtual void				Activated();
				virtual void				Deactivated();
				virtual void				Opened();
				virtual void				Closing(bool& cancel);
				virtual void				Closed();
				virtual void				Paint();
				virtual void				Destroying();
				virtual void				Destroyed();

				virtual void				LeftButtonDown(const MouseInfo& info);
				virtual void				LeftButtonUp(const MouseInfo& info);
				virtual void				LeftButtonDoubleClick(const MouseInfo& info);
				virtual void				RightButtonDown(const MouseInfo& info);
				virtual void				RightButtonUp(const MouseInfo& info);
				virtual void				RightButtonDoubleClick(const MouseInfo& info);
				virtual void				MiddleButtonDown(const MouseInfo& info);
				virtual void				MiddleButtonUp(const MouseInfo& info);
				virtual void				MiddleButtonDoubleClick(const MouseInfo& info);
				virtual void				HorizontalWheel(const MouseInfo& info);
				virtual void				VerticalWheel(const MouseInfo& info);
				virtual void				MouseMoving(const MouseInfo& info);
				virtual void				MouseEntered();
				virtual void				MouseLeaved();

				virtual void				KeyDown(const KeyInfo& info);
				virtual void				KeyUp(const KeyInfo& info);
				virtual void				SysKeyDown(const KeyInfo& info);
				virtual void				SysKeyUp(const KeyInfo& info);
				virtual void				Char(const CharInfo& info);
			};

			class IController : public virtual Interface
			{
			public:
				virtual PassRefPtr<ICallbackService>		GetCallbackService() = 0;
				virtual PassRefPtr<IResourceService>		GetResourceService() = 0;
				virtual PassRefPtr<IAsyncService>			GetAsyncService() = 0;
				virtual PassRefPtr<IClipboardService>		GetClipboardService() = 0;
				virtual PassRefPtr<IImageService>			GetImageService() = 0;
				virtual PassRefPtr<IScreenService>			GetScreenService() = 0;
				virtual PassRefPtr<IWindowService>			GetWindowService() = 0;
				virtual PassRefPtr<IInputService>			GetInputService() = 0;
				virtual PassRefPtr<IDialogService>			GetDialogService() = 0;
			};

			class IControllerListener : public Interface
			{
			public:
				virtual void					LeftButtonDown(CPoint position);
				virtual void					LeftButtonUp(CPoint position);
				virtual void					RightButtonDown(CPoint position);
				virtual void					RightButtonUp(CPoint position);
				virtual void					MouseMoving(CPoint position);
				virtual void					GlobalTimer();
				virtual void					ClipboardUpdated();
				virtual void					WindowCreated(PassRefPtr<IWindow> window);
				virtual void					WindowDestroying(PassRefPtr<IWindow> window);
			};

			class ICallbackService : public virtual Interface
			{
			public:
				virtual bool					InstallListener(PassRefPtr<IControllerListener> listener) = 0;
				virtual bool					UninstallListener(PassRefPtr<IControllerListener> listener) = 0;
			};

			class IResourceService : public virtual Interface
			{
			public:
				virtual PassRefPtr<ICursor>		GetSystemCursor(ICursor::CursorType type) = 0;
				virtual PassRefPtr<ICursor>		GetDefaultSystemCursor() = 0;
				virtual Font					GetDefaultFont() = 0;
				virtual void					SetDefaultFont(const Font& value) = 0;
			};

			class IDelay : public virtual Interface
			{
			public:
				enum ExecuteStatus
				{
					Pending,
					Executing,
					Executed,
					Canceled,
				};

				virtual ExecuteStatus			GetStatus() = 0;
				virtual bool					Delay(cint milliseconds) = 0;
				virtual bool					Cancel() = 0;
			};

			class IAsyncService : public virtual Interface
			{
			public:
				virtual bool					IsInMainThread() = 0;
				virtual void					ExecuteAsyncTasks() = 0;
				virtual void					InvokeAsync(const function<void()>& proc) = 0;
				virtual void					InvokeInMainThread(const function<void()>& proc) = 0;
				virtual bool					InvokeInMainThreadAndWait(const function<void()>& proc, cint milliseconds = -1) = 0;
				virtual PassRefPtr<IDelay>		DelayExecute(const function<void()>& proc, cint milliseconds) = 0;
				virtual PassRefPtr<IDelay>		DelayExecuteInMainThread(const function<void()>& proc, cint milliseconds) = 0;
			};

			class IClipboardService : public virtual Interface
			{
			public:
				virtual bool					ContainsText() = 0;
				virtual CString					GetText() = 0;
				virtual bool					SetText(const CString& value) = 0;
			};

			class IImage : public virtual Interface
			{
			public:
				enum FormatType
				{
					Bmp,
					Gif,
					Icon,
					Jpeg,
					Png,
					Tiff,
					Wmp,
					Unknown,
				};

				virtual PassRefPtr<IImageService>	GetImageService() = 0;
				virtual FormatType					GetFormat() = 0;
				virtual cint						GetFrameCount() = 0;
				virtual PassRefPtr<IImageFrame>		GetFrame(cint index) = 0;
			};

			class IImageFrame : public virtual Interface
			{
			public:
				virtual PassRefPtr<IImage>				GetImage() = 0;
				virtual CSize							GetSize() = 0;
				virtual bool							SetCache(void* key, PassRefPtr<IImageFrameCache> cache) = 0;
				virtual PassRefPtr<IImageFrameCache>	GetCache(void* key) = 0;
				virtual PassRefPtr<IImageFrameCache>	RemoveCache(void* key) = 0;
			};

			class IImageFrameCache : public Interface
			{
			public:
				virtual void					OnAttach(PassRefPtr<IImageFrame> frame) = 0;
				virtual void					OnDetach(PassRefPtr<IImageFrame> frame) = 0;
			};

			class IImageService : public virtual Interface
			{
			public:
				virtual PassRefPtr<IImage>		CreateImageFromFile(const CString& path) = 0;
				virtual PassRefPtr<IImage>		CreateImageFromMemory(void* buffer, cint length) = 0;
			};

			class IScreenService : public virtual Interface
			{
			public:
				virtual cint					GetScreenCount() = 0;
				virtual PassRefPtr<IScreen>		GetScreen(cint index) = 0;
				virtual PassRefPtr<IScreen>		GetScreen(PassRefPtr<IWindow> window) = 0;
			};

			class IWindowService : public virtual Interface
			{
			public:
				virtual PassRefPtr<IWindow>		CreatWindow(bool shadow = false) = 0;
				virtual void					DestroyWindow(PassRefPtr<IWindow> window) = 0;
				virtual PassRefPtr<IWindow>		GetMainWindow() = 0;
				virtual PassRefPtr<IWindow>		GetWindow(CPoint location) = 0;
				virtual void					Run(PassRefPtr<IWindow> window) = 0;
			};

			class IInputService : public virtual Interface
			{
			public:
				virtual void					StartHookMouse() = 0;
				virtual void					StopHookMouse() = 0;
				virtual bool					IsHookingMouse() = 0;

				virtual void					StartTimer() = 0;
				virtual void					StopTimer() = 0;
				virtual bool					IsTimerEnabled() = 0;

				virtual bool					IsKeyPressing(cint code) = 0;
				virtual bool					IsKeyToggled(cint code) = 0;

				virtual CString					GetKeyName(cint code) = 0;
				virtual cint					GetKey(const CString& name) = 0;
			};

			class IDialogService : public virtual Interface
			{
			public:
				enum MessageBoxButtonsInput
				{
					DisplayOK,
					DisplayOKCancel,
					DisplayYesNo,
					DisplayYesNoCancel,
					DisplayRetryCancel,
					DisplayAbortRetryIgnore,
					DisplayCancelTryAgainContinue,
				};

				enum MessageBoxButtonsOutput
				{
					SelectOK,
					SelectCancel,
					SelectYes,
					SelectNo,
					SelectRetry,
					SelectAbort,
					SelectIgnore,
					SelectTryAgain,
					SelectContinue,
				};

				enum MessageBoxDefaultButton
				{
					DefaultFirst,
					DefaultSecond,
					DefaultThird,
				};

				enum MessageBoxIcons
				{
					IconNone,
					IconError,
					IconQuestion,
					IconWarning,
					IconInformation,
				};

				enum MessageBoxModalOptions
				{
					ModalWindow,
					ModalTask,
					ModalSystem,
				};

				virtual MessageBoxButtonsOutput	ShowMessageBox(PassRefPtr<IWindow> window,
					const CString& text,
					const CString& title,
					MessageBoxButtonsInput buttons = DisplayOK,
					MessageBoxDefaultButton defaultButton = DefaultFirst,
					MessageBoxIcons icon = IconNone,
					MessageBoxModalOptions modal = ModalWindow) = 0;

				enum ColorDialogCustomColorOptions
				{
					CustomColorDisabled,
					CustomColorEnabled,
					CustomColorOpened,
				};

				virtual bool ShowColorDialog(PassRefPtr<IWindow> window,
					CColor& selection,
					bool selected = false,
					ColorDialogCustomColorOptions customColorOptions = CustomColorEnabled,
					CColor* customColors = 0) = 0;

				virtual bool ShowFontDialog(PassRefPtr<IWindow> window,
					Font& selectionFont,
					CColor& selectionColor,
					bool selected = false,
					bool showEffect = true,
					bool forceFontExist = true) = 0;

				enum FileDialogTypes
				{
					FileDialogOpen,
					FileDialogOpenPreview,
					FileDialogSave,
					FileDialogSavePreview,
				};

				enum FileDialogOptions
				{
					FileDialogAllowMultipleSelection = 1,
					FileDialogFileMustExist = 2,
					FileDialogShowReadOnlyCheckBox = 4,
					FileDialogDereferenceLinks = 8,
					FileDialogShowNetworkButton = 16,
					FileDialogPromptCreateFile = 32,
					FileDialogPromptOverwriteFile = 64,
					FileDialogDirectoryMustExist = 128,
					FileDialogAddToRecent = 256,
				};

				virtual bool ShowFileDialog(PassRefPtr<IWindow> window,
					vector<CString>& selectionFileNames,
					cint& selectionFilterIndex,
					FileDialogTypes dialogType,
					const CString& title,
					const CString& initialFileName,
					const CString& initialDirectory,
					const CString& defaultExtension,
					const CString& filter,
					FileDialogOptions options) = 0;
			};

			class IMessageHandler : public Interface
			{
			public:
				virtual void								BeforeHandle(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& skip) = 0;
				virtual void								AfterHandle(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& skip, LRESULT& result) = 0;
			};

			class IForm : public Interface
			{
			public:
				virtual HWND								GetWindowHandle() = 0;
				virtual PassRefPtr<IGraphicsRenderTarget>	GetGraphicsHandler() = 0;
				virtual void								SetGraphicsHandler(PassRefPtr<IGraphicsRenderTarget> handler) = 0;
				virtual bool								InstallMessageHandler(PassRefPtr<IMessageHandler> handler) = 0;
				virtual bool								UninstallMessageHandler(PassRefPtr<IMessageHandler> handler) = 0;
			};
		}

		namespace direct2d
		{
			class IGraphicsElement : public virtual Interface
			{
			public:
				virtual PassRefPtr<IGraphicsElementFactory>		GetFactory() = 0;
				virtual PassRefPtr<IGraphicsRenderer>			GetRenderer() = 0;
			};

			class IGraphicsElementFactory : public Interface
			{
			public:
				virtual CString									GetElementTypeName() = 0;
				virtual PassRefPtr<IGraphicsElement>			Create() = 0;
			};

			class IGraphicsRenderer : public Interface
			{
			public:
				virtual PassRefPtr<IGraphicsRendererFactory>	GetFactory() = 0;

				virtual void				Initialize(PassRefPtr<IGraphicsElement> element) = 0;
				virtual void				Finalize() = 0;
				virtual void				SetRenderTarget(PassRefPtr<IGraphicsRenderTarget> renderTarget) = 0;
				virtual void				Render(CRect bounds) = 0;
				virtual void				OnElementStateChanged() = 0;
				virtual CSize				GetMinSize() = 0;
			};

			class IGraphicsRendererFactory : public Interface
			{
			public:
				virtual PassRefPtr<IGraphicsRenderer>			Create() = 0;
			};

			class IGraphicsRenderTarget : public Interface
			{
			public:
				virtual bool				StartRendering() = 0;
				virtual HRESULT				StopRendering() = 0;
				virtual void				PushClipper(CRect clipper) = 0;
				virtual void				PopClipper() = 0;
				virtual CRect				GetClipper() = 0;
				virtual bool				IsClipperCoverWholeTarget() = 0;
			};

			class IGraphicsParagraph : public virtual Interface
			{
			public:
				static const cint NullInteractionId = -1;

				enum Alignment
				{
					Left = 0,
					Top = 0,
					Center = 1,
					Right = 2,
					Bottom = 2,
				};

				enum TextStyle
				{
					Bold = 1,
					Italic = 2,
					Underline = 4,
					Strikeline = 8,
				};

				enum BreakCondition
				{
					StickToPreviousRun,
					StickToNextRun,
					Alone,
				};

				enum CaretRelativePosition
				{
					CaretFirst,
					CaretLast,
					CaretLineFirst,
					CaretLineLast,
					CaretMoveLeft,
					CaretMoveRight,
					CaretMoveUp,
					CaretMoveDown,
				};

				struct InlineObjectProperties
				{
					CSize					size;
					cint					baseline;
					BreakCondition			breakCondition;

					InlineObjectProperties()
						: baseline(-1)
					{
					}
				};

				virtual PassRefPtr<IGraphicsLayoutProvider>	GetProvider() = 0;
				virtual PassRefPtr<IGraphicsRenderTarget>	GetRenderTarget() = 0;
				virtual bool								GetWrapLine() = 0;
				virtual void								SetWrapLine(bool value) = 0;
				virtual cint								GetMaxWidth() = 0;
				virtual void								SetMaxWidth(cint value) = 0;
				virtual Alignment							GetParagraphAlignment() = 0;
				virtual void								SetParagraphAlignment(Alignment value) = 0;

				virtual bool								SetFont(cint start, cint length, const CString& value) = 0;
				virtual bool								SetSize(cint start, cint length, cint value) = 0;
				virtual bool								SetStyle(cint start, cint length, TextStyle value) = 0;
				virtual bool								SetColor(cint start, cint length, CColor value) = 0;
				virtual bool								SetBackgroundColor(cint start, cint length, CColor value) = 0;
				virtual bool								SetInlineObject(cint start, cint length, const InlineObjectProperties& properties, PassRefPtr<IGraphicsElement> value) = 0;
				virtual bool								ResetInlineObject(cint start, cint length) = 0;

				virtual cint								GetHeight() = 0;
				virtual bool								OpenCaret(cint caret, CColor color, bool frontSide) = 0;
				virtual bool								CloseCaret() = 0;
				virtual void								Render(CRect bounds) = 0;

				virtual cint								GetCaret(cint comparingCaret, CaretRelativePosition position, bool& preferFrontSide) = 0;
				virtual CRect								GetCaretBounds(cint caret, bool frontSide) = 0;
				virtual cint								GetCaretFromPoint(CPoint point) = 0;
				virtual PassRefPtr<IGraphicsElement>		GetInlineObjectFromPoint(CPoint point, cint& start, cint& length) = 0;
				virtual cint								GetNearestCaretFromTextPos(cint textPos, bool frontSide) = 0;
				virtual bool								IsValidCaret(cint caret) = 0;
				virtual bool								IsValidTextPos(cint textPos) = 0;
			};

			class IGraphicsLayoutProvider : public virtual Interface
			{
			public:
				virtual PassRefPtr<IGraphicsParagraph>		CreateParagraph(const CString& text, PassRefPtr<IGraphicsRenderTarget> renderTarget) = 0;
			};

			class IGraphicsAnimation : public virtual Interface
			{
			public:
				virtual cint								GetTotalLength() = 0;
				virtual cint								GetCurrentPosition() = 0;
				virtual void								Play(cint currentPosition, cint totalLength) = 0;
				virtual void								Stop() = 0;
			};

			class ID2DRenderTarget : public IGraphicsRenderTarget
			{
			public:
				virtual CComPtr<ID2D1RenderTarget>			GetDirect2DRenderTarget() = 0;
				virtual CComPtr<ID2D1Bitmap>				GetBitmap(PassRefPtr<IImageFrame> frame, bool enabled) = 0;
				virtual void								DestroyBitmapCache(PassRefPtr<IImageFrame> frame) = 0;
				virtual void								SetTextAntialias(bool antialias, bool verticalAntialias) = 0;

				virtual CComPtr<ID2D1SolidColorBrush>		CreateDirect2DBrush(CColor color) = 0;
				virtual void								DestroyDirect2DBrush(CColor color) = 0;
				virtual CComPtr<ID2D1LinearGradientBrush>	CreateDirect2DLinearBrush(CColor c1, CColor c2) = 0;
				virtual void								DestroyDirect2DLinearBrush(CColor c1, CColor c2) = 0;
			};

			class ID2DResourceManager : public Interface
			{
			public:
				virtual PassRefPtr<D2DTextFormatPackage>	CreateDirect2DTextFormat(const Font& font) = 0;
				virtual void								DestroyDirect2DTextFormat(const Font& font) = 0;
				virtual PassRefPtr<CharMeasurer>			CreateDirect2DCharMeasurer(const Font& font) = 0;
				virtual void								DestroyDirect2DCharMeasurer(const Font& font) = 0;
			};

			class ID2DProvider : public Interface
			{
			public:
				virtual void								RecreateRenderTarget(PassRefPtr<IWindow> window) = 0;
				virtual CComPtr<ID2D1RenderTarget>			GetDirect2DRenderTarget(PassRefPtr<IWindow> window) = 0;
				virtual CComPtr<ID2D1Factory>				GetDirect2DFactory() = 0;
				virtual CComPtr<IDWriteFactory>				GetDirectWriteFactory() = 0;
				virtual PassRefPtr<ID2DRenderTarget>		GetBindedRenderTarget(PassRefPtr<IWindow> window) = 0;
				virtual void								SetBindedRenderTarget(PassRefPtr<IWindow> window, PassRefPtr<ID2DRenderTarget> renderTarget) = 0;
				virtual CComPtr<IWICImagingFactory>			GetWICImagingFactory() = 0;
				virtual CComPtr<IWICBitmap>					GetWICBitmap(PassRefPtr<IImageFrame> frame) = 0;
			};

			class IRendererCallback : public Interface
			{
			public:
				virtual CColor								GetBackgroundColor(cint textPosition) = 0;
				virtual PassRefPtr<ID2DRenderTarget>		GetDirect2DRenderTarget() = 0;
			};

			class IColorizedTextCallback : public Interface
			{
			public:
				virtual void								ColorChanged() = 0;
				virtual void								FontChanged() = 0;
			};

			class ITextBoxBaseCallback : public Interface
			{
			public:
				virtual TextPos									GetLeftWord(TextPos pos) = 0;
				virtual TextPos									GetRightWord(TextPos pos) = 0;
				virtual void									GetWord(TextPos pos, TextPos& begin, TextPos& end) = 0;
				virtual cint									GetPageRows() = 0;
				virtual bool									BeforeModify(TextPos start, TextPos end, const CString& originalText, CString& inputText) = 0;
				virtual void									AfterModify(TextPos originalStart, TextPos originalEnd, const CString& originalText, TextPos inputStart, TextPos inputEnd, const CString& inputText) = 0;
				virtual void									ScrollToView(CPoint point) = 0;
				virtual cint									GetTextMargin() = 0;
			};

			class IScrollCommandExecutor : public Interface
			{
			public:
				virtual void									SmallDecrease() = 0;
				virtual void									SmallIncrease() = 0;
				virtual void									BigDecrease() = 0;
				virtual void									BigIncrease() = 0;

				virtual void									SetTotalSize(cint value) = 0;
				virtual void									SetPageSize(cint value) = 0;
				virtual void									SetPosition(cint value) = 0;
			};

			class ITabCommandExecutor : public Interface
			{
			public:
				virtual void									ShowTab(cint index) = 0;
			};

			class IContentCallback : public Interface
			{
			public:
				virtual void									UpdateLayout() = 0;
			};
		}
	}
}

#endif