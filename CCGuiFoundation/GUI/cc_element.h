#ifndef CC_ELEMENT
#define CC_ELEMENT

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_presentation.h"

using namespace cc::base;
using namespace cc::presentation::windows;
using namespace cc::presentation::direct2d::text;

namespace cc
{
	namespace presentation
	{
		namespace element
		{
			namespace graphics_element
			{
#pragma region Base
				template <class TElement>
				class GraphicsElement : public IGraphicsElement
				{
				public:
					class Factory : public IGraphicsElementFactory
					{
					public:
						Factory()
						{

						}
						CString GetElementTypeName()
						{
							return TElement::GetElementTypeName();
						}
						PassRefPtr<IGraphicsElement> Create()
						{
							RefPtr<TElement> element = adoptRef(new TElement);
							element->factory = this;
							RefPtr<IGraphicsRendererFactory> rendererFactory = GetStorage()->GetGraphicsResourceManager()->GetRendererFactory(GetElementTypeName());
							if (rendererFactory)
							{
								element->renderer = rendererFactory->Create();
								element->renderer->Initialize(element);
							}
							return element;
						}
					};
				public:
					static PassRefPtr<TElement> Create()
					{
						return GetStorage()->GetGraphicsResourceManager()->GetElementFactory(TElement::GetElementTypeName())->Create();
					}
					PassRefPtr<IGraphicsElementFactory> GetFactory()override
					{
						return factory;
					}
					PassRefPtr<IGraphicsRenderer> GetRenderer()override
					{
						return renderer;
					}
				protected:
					RawPtr<IGraphicsElementFactory>			factory;
					RefPtr<IGraphicsRenderer>				renderer;
				};

				template <class TElement, class TRenderer, class TTarget>
				class GraphicsRenderer : public IGraphicsRenderer
				{
				public:
					class Factory : public IGraphicsRendererFactory
					{
					public:
						Factory()
						{

						}
						PassRefPtr<IGraphicsRenderer> Create()
						{
							RefPtr<TRenderer> renderer = adoptRef(new TRenderer);
							renderer->factory = this;
							renderer->element = nullptr;
							renderer->renderTarget = nullptr;
							return renderer;
						}
					};
				public:
					GraphicsRenderer()
					{

					}
					static void Register()
					{
						GetStorage()->RegisterFactories(adoptRef(new TElement::Factory), adoptRef(new TRenderer::Factory));
					}
					PassRefPtr<IGraphicsRendererFactory> GetFactory()override
					{
						return factory;
					}
					void Initialize(PassRefPtr<IGraphicsElement> _element)override
					{
						element = dynamic_cast<TElement*>(_element.get());
						InitializeInternal();
					}
					void Finalize()override
					{
						FinalizeInternal();
					}
					void SetRenderTarget(PassRefPtr<IGraphicsRenderTarget> _renderTarget)override
					{
						RefPtr<TTarget> oldRenderTarget = renderTarget;
						renderTarget = dynamic_cast<ID2DRenderTarget*>(_renderTarget.get());
						RenderTargetChangedInternal(oldRenderTarget, renderTarget);
					}
					CSize GetMinSize()override
					{
						return minSize;
					}
					virtual void InitializeInternal() = 0;
					virtual void FinalizeInternal() = 0;
					virtual void RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget) = 0;

				protected:
					RawPtr<IGraphicsRendererFactory>	factory;
					RawPtr<TElement>					element;
					RawPtr<TTarget>						renderTarget;
					CSize								minSize;
				};

				template <class TElement, class TRenderer, class TBrush, class TBrushProperty>
				class GraphicsBrushRenderer : public GraphicsRenderer<TElement, TRenderer, ID2DRenderTarget>
				{
				protected:
					void InitializeInternal()override
					{

					}
					void FinalizeInternal()override
					{
						DestroyBrush(renderTarget);
					}
					void RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget)override
					{
						DestroyBrush(oldRenderTarget);
						CreateBrush(newRenderTarget);
					}
					virtual void CreateBrush(PassRefPtr<ID2DRenderTarget> renderTarget) = 0;
					virtual void DestroyBrush(PassRefPtr<ID2DRenderTarget> renderTarget) = 0;

					TBrushProperty			oldColor;
					CComPtr<TBrush>			brush;
				};

				template <class TElement, class TRenderer, class TBrush, class TBrushProperty>
				class GraphicsSolidBrushRenderer : public GraphicsBrushRenderer<TElement, TRenderer, TBrush, TBrushProperty>
				{
				public:
					void OnElementStateChanged()override
					{
						if (renderTarget)
						{
							TBrushProperty color = element->GetColor();
							if (oldColor != color)
							{
								DestroyBrush(renderTarget);
								CreateBrush(renderTarget);
							}
						}
					}

				protected:
					void CreateBrush(PassRefPtr<ID2DRenderTarget> _renderTarget) override
					{
						if (_renderTarget)
						{
							oldColor = element->GetColor();
							brush = _renderTarget->CreateDirect2DBrush(oldColor);
						}
					}
					void DestroyBrush(PassRefPtr<ID2DRenderTarget> _renderTarget) override
					{
						if (_renderTarget && brush)
						{
							_renderTarget->DestroyDirect2DBrush(oldColor);
							brush = nullptr;
						}
					}
				};

				template <class TElement, class TRenderer, class TBrush, class TBrushProperty>
				class GraphicsGradientBrushRenderer : public GraphicsBrushRenderer<TElement, TRenderer, TBrush, TBrushProperty>
				{
				public:
					void OnElementStateChanged()override
					{
						if (renderTarget)
						{
							TBrushProperty color = TBrushProperty(element->GetColor1(), element->GetColor2());
							if (oldColor != color)
							{
								DestroyBrush(renderTarget);
								CreateBrush(renderTarget);
							}
						}
					}

				protected:
					void CreateBrush(PassRefPtr<ID2DRenderTarget> _renderTarget) override
					{
						if (_renderTarget)
						{
							oldColor = pair<CColor, CColor>(element->GetColor1(), element->GetColor2());
							brush = _renderTarget->CreateDirect2DLinearBrush(oldColor.first, oldColor.second);
						}
					}
					void DestroyBrush(PassRefPtr<ID2DRenderTarget> _renderTarget) override
					{
						if (_renderTarget && brush)
						{
							_renderTarget->DestroyDirect2DLinearBrush(oldColor.first, oldColor.second);
							brush = nullptr;
						}
					}
				};
			}
#pragma endregion Base

			using namespace cc::presentation::element::graphics_element;

			enum ElementShape
			{
				Rectangle,
				Ellipse,
			};

#pragma region SolidBorder
			class SolidBorderElement : public GraphicsElement<SolidBorderElement>
			{
			public:
				SolidBorderElement();
				~SolidBorderElement();

				static CString						GetElementTypeName();

				CColor								GetColor();
				void								SetColor(CColor value);
				ElementShape						GetShape();
				void								SetShape(ElementShape value);

			protected:
				CColor								color;
				ElementShape						shape;
			};

			class SolidBorderElementRenderer : public GraphicsSolidBrushRenderer<SolidBorderElement, SolidBorderElementRenderer, ID2D1Brush, CColor>
			{
			public:
				void								Render(CRect bounds)override;
			};
#pragma endregion SolidBorder

#pragma region RoundBorder
			class RoundBorderElement : public GraphicsElement<RoundBorderElement>
			{
			public:
				RoundBorderElement();
				~RoundBorderElement();

				static CString						GetElementTypeName();

				CColor								GetColor();
				void								SetColor(CColor value);
				cint								GetRadius();
				void								SetRadius(cint value);

			protected:
				CColor								color;
				cint								radius;
			};

			class RoundBorderElementRenderer : public GraphicsSolidBrushRenderer<RoundBorderElement, RoundBorderElementRenderer, ID2D1Brush, CColor>
			{
			public:
				void								Render(CRect bounds)override;
			};
#pragma endregion RoundBorder

#pragma region Border3D
			class Border3DElement : public GraphicsElement<Border3DElement>
			{
			public:
				Border3DElement();
				~Border3DElement();

				static CString						GetElementTypeName();

				CColor								GetColor1();
				void								SetColor1(CColor value);
				CColor								GetColor2();
				void								SetColor2(CColor value);
				void								SetColors(CColor value1, CColor value2);

			protected:
				CColor								color1;
				CColor								color2;
			};

			class Border3DElementRenderer : public GraphicsRenderer<Border3DElement, Border3DElementRenderer, ID2DRenderTarget>
			{
			public:
				void								Render(CRect bounds)override;
				void								OnElementStateChanged()override;

			protected:
				void								CreateBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								DestroyBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								InitializeInternal();
				void								FinalizeInternal();
				void								RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget);

				CColor								oldColor1;
				CColor								oldColor2;
				CComPtr<ID2D1SolidColorBrush>		brush1;
				CComPtr<ID2D1SolidColorBrush>		brush2;
			};
#pragma endregion Border3D

#pragma region Splitter3D
			class Splitter3DElement : public GraphicsElement<Splitter3DElement>
			{
			public:
				Splitter3DElement();
				~Splitter3DElement();

				enum Direction
				{
					Horizontal,
					Vertical,
				};

				static CString						GetElementTypeName();

				CColor								GetColor1();
				void								SetColor1(CColor value);
				CColor								GetColor2();
				void								SetColor2(CColor value);
				void								SetColors(CColor value1, CColor value2);
				Direction							GetDirection();
				void								SetDirection(Direction value);

			protected:
				CColor								color1;
				CColor								color2;
				Direction							direction;
			};

			class Splitter3DElementRenderer : public GraphicsRenderer<Splitter3DElement, Splitter3DElementRenderer, ID2DRenderTarget>
			{
			public:
				void								Render(CRect bounds)override;
				void								OnElementStateChanged()override;

			protected:
				void								CreateBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								DestroyBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								InitializeInternal();
				void								FinalizeInternal();
				void								RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget);

				CColor								oldColor1;
				CColor								oldColor2;
				CComPtr<ID2D1SolidColorBrush>		brush1;
				CComPtr<ID2D1SolidColorBrush>		brush2;
			};
#pragma endregion Splitter3D

#pragma region SolidBackground
			class SolidBackgroundElement : public GraphicsElement<SolidBackgroundElement>
			{
			public:
				SolidBackgroundElement();
				~SolidBackgroundElement();

				static CString						GetElementTypeName();

				CColor								GetColor();
				void								SetColor(CColor value);
				ElementShape						GetShape();
				void								SetShape(ElementShape value);

			protected:
				CColor								color;
				ElementShape						shape;
			};

			class SolidBackgroundElementRenderer : public GraphicsSolidBrushRenderer<SolidBackgroundElement, SolidBackgroundElementRenderer, ID2D1SolidColorBrush, CColor>
			{
			public:
				void								Render(CRect bounds)override;
			};
#pragma endregion SolidBackground

#pragma region GradientBackground
			class GradientBackgroundElement : public GraphicsElement<GradientBackgroundElement>
			{
			public:
				GradientBackgroundElement();
				~GradientBackgroundElement();

				enum Direction
				{
					Horizontal,
					Vertical,
					Slash,
					Backslash,
				};

				static CString						GetElementTypeName();

				CColor								GetColor1();
				void								SetColor1(CColor value);
				CColor								GetColor2();
				void								SetColor2(CColor value);
				void								SetColors(CColor value1, CColor value2);
				Direction							GetDirection();
				void								SetDirection(Direction value);
				ElementShape						GetShape();
				void								SetShape(ElementShape value);

			protected:
				CColor								color1;
				CColor								color2;
				Direction							direction;
				ElementShape						shape;
			};

			class GradientBackgroundElementRenderer : public GraphicsGradientBrushRenderer<GradientBackgroundElement, GradientBackgroundElementRenderer, ID2D1LinearGradientBrush, pair<CColor, CColor>>
			{
			public:
				void								Render(CRect bounds)override;
			};
#pragma endregion GradientBackground

#pragma region SolidLabel
			class SolidLabelElement : public GraphicsElement<SolidLabelElement>
			{
			public:
				SolidLabelElement();
				~SolidLabelElement();

				static CString						GetElementTypeName();

				CColor								GetColor();
				void								SetColor(CColor value);
				const Font&							GetFont();
				void								SetFont(const Font& value);
				const CString&						GetText();
				void								SetText(const CString& value);

				Alignment							GetHorizontalAlignment();
				Alignment							GetVerticalAlignment();
				void								SetHorizontalAlignment(Alignment value);
				void								SetVerticalAlignment(Alignment value);
				void								SetAlignments(Alignment horizontal, Alignment vertical);

				bool								GetWrapLine();
				void								SetWrapLine(bool value);
				bool								GetEllipse();
				void								SetEllipse(bool value);
				bool								GetMultiline();
				void								SetMultiline(bool value);
				bool								GetWrapLineHeightCalculation();
				void								SetWrapLineHeightCalculation(bool value);

			protected:
				CColor								color;
				Font								fontProperties;
				CString								text;
				Alignment							hAlignment;
				Alignment							vAlignment;
				bool								wrapLine;
				bool								ellipse;
				bool								multiline;
				bool								wrapLineHeightCalculation;
			};

			class SolidLabelElementRenderer : public GraphicsRenderer<SolidLabelElement, SolidLabelElementRenderer, ID2DRenderTarget>
			{
			public:
				SolidLabelElementRenderer();

				void								Render(CRect bounds)override;
				void								OnElementStateChanged()override;

			protected:
				void								CreateBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								DestroyBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								CreateTextFormat(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								DestroyTextFormat(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								CreateTextLayout();
				void								DestroyTextLayout();
				void								UpdateMinSize();

				void								InitializeInternal();
				void								FinalizeInternal();
				void								RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget);

				CColor								oldColor;
				Font								oldFont;
				CString								oldText;
				CComPtr<ID2D1SolidColorBrush>		brush;
				RefPtr<D2DTextFormatPackage>		textFormat;
				CComPtr<IDWriteTextLayout>			textLayout;
				cint								oldMaxWidth;
			};
#pragma endregion SolidLabel

#pragma region ImageFrame
			class ImageFrameElement : public GraphicsElement<ImageFrameElement>
			{
			public:
				ImageFrameElement();
				~ImageFrameElement();

				static CString						GetElementTypeName();

				RefPtr<IImage>						GetImage();
				cint								GetFrameIndex();
				void								SetImage(PassRefPtr<IImage> value);
				void								SetFrameIndex(cint value);
				void								SetImage(PassRefPtr<IImage> _image, cint _frameIndex);

				Alignment							GetHorizontalAlignment();
				Alignment							GetVerticalAlignment();
				void								SetHorizontalAlignment(Alignment value);
				void								SetVerticalAlignment(Alignment value);
				void								SetAlignments(Alignment horizontal, Alignment vertical);

				bool								GetStretch();
				void								SetStretch(bool value);
				bool								GetEnabled();
				void								SetEnabled(bool value);

			protected:
				RefPtr<IImage>						image;
				cint								frameIndex;
				Alignment							hAlignment;
				Alignment							vAlignment;
				bool								stretch;
				bool								enabled;
			};

			class ImageFrameElementRenderer : public GraphicsRenderer<ImageFrameElement, ImageFrameElementRenderer, ID2DRenderTarget>
			{
			public:
				void								Render(CRect bounds)override;
				void								OnElementStateChanged()override;

			protected:
				void								UpdateBitmap(PassRefPtr<ID2DRenderTarget> renderTarget);

				void								InitializeInternal();
				void								FinalizeInternal();
				void								RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget);

				CComPtr<ID2D1Bitmap>				bitmap;
			};
#pragma endregion ImageFrame

#pragma region Polygon
			class PolygonElement : public GraphicsElement<PolygonElement>
			{
			public:
				PolygonElement();
				~PolygonElement();

				static CString						GetElementTypeName();

				CSize								GetSize();
				void								SetSize(CSize value);

				const CPoint&						GetPoint(cint index);
				cint								GetPointCount();
				void								SetPoints(const CPoint* p, cint count);

				const vector<CPoint>&				GetPointsArray();
				void								SetPointsArray(const vector<CPoint>& value);

				CColor								GetBorderColor();
				void								SetBorderColor(CColor value);
				CColor								GetBackgroundColor();
				void								SetBackgroundColor(CColor value);

			protected:
				CSize								size;
				vector<CPoint>						points;
				CColor								borderColor;
				CColor								backgroundColor;
			};

			class PolygonElementRenderer : public GraphicsRenderer<PolygonElement, PolygonElementRenderer, ID2DRenderTarget>
			{
			public:
				void								Render(CRect bounds)override;
				void								OnElementStateChanged()override;

			protected:
				void								CreateGeometry();
				void								DestroyGeometry();
				void								FillGeometry(CPoint offset);
				void								RecreateResource(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget);

				void								InitializeInternal();
				void								FinalizeInternal();
				void								RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget);

				CColor								oldBorderColor;
				CColor								oldBackgroundColor;
				CComPtr<ID2D1SolidColorBrush>		borderBrush;
				CComPtr<ID2D1SolidColorBrush>		backgroundBrush;
				vector<CPoint>						oldPoints;
				CComPtr<ID2D1PathGeometry>			geometry;
			};
#pragma endregion Polygon

#pragma region ColorizedText
			class ColorizedTextElement : public GraphicsElement<ColorizedTextElement>
			{
			public:
				ColorizedTextElement();
				~ColorizedTextElement();

				static CString						GetElementTypeName();

				TextLines&							GetLines();
				PassRefPtr<IColorizedTextCallback>	GetCallback();
				void								SetCallback(PassRefPtr<IColorizedTextCallback> value);

				const vector<ColorEntry>&			GetColors();
				void								SetColors(const vector<ColorEntry>& value);
				cint								AddColor(const ColorEntry& value);
				const Font&							GetFont();
				void								SetFont(const Font& value);
				TCHAR								GetPasswordChar();
				void								SetPasswordChar(TCHAR value);
				CPoint								GetViewPosition();
				void								SetViewPosition(CPoint value);
				bool								GetVisuallyEnabled();
				void								SetVisuallyEnabled(bool value);
				bool								GetFocused();
				void								SetFocused(bool value);

				TextPos								GetCaretBegin();
				void								SetCaretBegin(TextPos value);
				TextPos								GetCaretEnd();
				void								SetCaretEnd(TextPos value);
				bool								GetCaretVisible();
				void								SetCaretVisible(bool value);
				CColor								GetCaretColor();
				void								SetCaretColor(CColor value);

			protected:
				RefPtr<IColorizedTextCallback>		callback;
				vector<ColorEntry>					colors;
				Font								font;
				CPoint								viewPosition;
				bool								isVisuallyEnabled;
				bool								isFocused;

				TextPos								caretBegin;
				TextPos								caretEnd;
				bool								caretVisible;
				CColor								caretColor;

				TextLines							lines;
			};

			class ColorizedTextElementRenderer : public GraphicsRenderer<ColorizedTextElement, ColorizedTextElementRenderer, ID2DRenderTarget>, public IColorizedTextCallback
			{
			public:
				void								Render(CRect bounds)override;
				void								OnElementStateChanged()override;

			protected:
				void								CreateTextBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								DestroyTextBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								CreateCaretBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);
				void								DestroyCaretBrush(PassRefPtr<ID2DRenderTarget> _renderTarget);

				void								ColorChanged()override;
				void								FontChanged()override;
				PassRefPtr<CharMeasurer>			GetCharMeasurer();

				void								InitializeInternal();
				void								FinalizeInternal();
				void								RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget);

				Font								oldFont;
				RefPtr<D2DTextFormatPackage>		textFormat;
				vector<ColorEntryResource>			colors;
				CColor								oldCaretColor;
				CComPtr<ID2D1SolidColorBrush>		caretBrush;
			};
#pragma endregion ColorizedText

#pragma region Direct2D
			class Direct2DElement;

			namespace event_args
			{
				struct Direct2DElementEventArgs : EventArgs
				{
				public:
					RawPtr<Direct2DElement>			element;
					RawPtr<ID2DRenderTarget>		renderTarget;
					CComPtr<ID2D1RenderTarget>		renderTargetD2D;
					CComPtr<IDWriteFactory>			factoryDWrite;
					CComPtr<ID2D1Factory>			factoryD2D;
					CRect							bounds;

					Direct2DElementEventArgs(
						PassRefPtr<Direct2DElement> _element,
						PassRefPtr<ID2DRenderTarget> _renderTarget,
						CComPtr<ID2D1RenderTarget> _renderTargetD2D,
						CComPtr<IDWriteFactory> _fdw,
						CComPtr<ID2D1Factory> _fd2d,
						CRect _bounds)
						: element(_element)
						, renderTarget(_renderTarget)
						, renderTargetD2D(_renderTargetD2D)
						, factoryDWrite(_fdw)
						, factoryD2D(_fd2d)
						, bounds(_bounds)
					{
					}
				};
			}

			using namespace event_args;

			class Direct2DElement : public GraphicsElement<Direct2DElement>
			{
			public:
				Direct2DElement();
				~Direct2DElement();

				static CString						GetElementTypeName();

				Event<Direct2DElementEventArgs>		BeforeRenderTargetChanged;
				Event<Direct2DElementEventArgs>		AfterRenderTargetChanged;
				Event<Direct2DElementEventArgs>		Rendering;
			};

			class Direct2DElementRenderer : public GraphicsRenderer<Direct2DElement, Direct2DElementRenderer, ID2DRenderTarget>
			{
			public:
				Direct2DElementRenderer();
				~Direct2DElementRenderer();

				void								Render(CRect bounds)override;
				void								OnElementStateChanged()override;

			protected:
				void								InitializeInternal();
				void								FinalizeInternal();
				void								RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget);
			};
#pragma endregion Direct2D
		}
	}
}

#endif