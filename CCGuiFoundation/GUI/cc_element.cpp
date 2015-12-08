#include "stdafx.h"
#include "cc_presentation.h"
#include "cc_element.h"

using namespace cc::presentation::windows;

namespace cc
{
	namespace presentation
	{
		namespace element
		{
#pragma region SolidBorder
			SolidBorderElement::SolidBorderElement()
				: shape(ElementShape::Rectangle)
			{

			}

			SolidBorderElement::~SolidBorderElement()
			{
				renderer->Finalize();
			}

			CString SolidBorderElement::GetElementTypeName()
			{
				return _T("SolidBorder");
			}

			CColor SolidBorderElement::GetColor()
			{
				return color;
			}

			void SolidBorderElement::SetColor(CColor value)
			{
				if (color != value)
				{
					color = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			ElementShape SolidBorderElement::GetShape()
			{
				return shape;
			}

			void SolidBorderElement::SetShape(ElementShape value)
			{
				shape = value;
			}

			void SolidBorderElementRenderer::Render(CRect bounds)
			{
				CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
				switch (element->GetShape())
				{
					case ElementShape::Rectangle:
						d2dRenderTarget->DrawRectangle(
							D2D1::RectF((FLOAT)bounds.left + 0.5f, (FLOAT)bounds.top + 0.5f, (FLOAT)bounds.right - 0.5f, (FLOAT)bounds.bottom - 0.5f),
							brush
							);
						break;
					case ElementShape::Ellipse:
						d2dRenderTarget->DrawEllipse(
							D2D1::Ellipse(D2D1::Point2F((bounds.left + bounds.right) / 2.0f, (bounds.top + bounds.bottom) / 2.0f), bounds.Width() / 2.0f, bounds.Height() / 2.0f),
							brush
							);
						break;
				}
			}
#pragma endregion SolidBorder

#pragma region RoundBorder
			RoundBorderElement::RoundBorderElement()
				: radius(10)
			{

			}

			RoundBorderElement::~RoundBorderElement()
			{
				renderer->Finalize();
			}

			CString RoundBorderElement::GetElementTypeName()
			{
				return _T("RoundBorder");
			}

			CColor RoundBorderElement::GetColor()
			{
				return color;
			}

			void RoundBorderElement::SetColor(CColor value)
			{
				if (color != value)
				{
					color = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			cint RoundBorderElement::GetRadius()
			{
				return radius;
			}

			void RoundBorderElement::SetRadius(cint value)
			{
				if (radius != value)
				{
					radius = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			void RoundBorderElementRenderer::Render(CRect bounds)
			{
				CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
				d2dRenderTarget->DrawRoundedRectangle(
					D2D1::RoundedRect(
					D2D1::RectF((FLOAT)bounds.left + 0.5f, (FLOAT)bounds.top + 0.5f, (FLOAT)bounds.right - 0.5f, (FLOAT)bounds.bottom - 0.5f),
					(FLOAT)element->GetRadius(),
					(FLOAT)element->GetRadius()
					),
					brush
					);
			}
#pragma endregion RoundBorder

#pragma region Border3D
			Border3DElement::Border3DElement()
			{

			}

			Border3DElement::~Border3DElement()
			{
				renderer->Finalize();
			}

			CString Border3DElement::GetElementTypeName()
			{
				return _T("Border3D");
			}

			CColor Border3DElement::GetColor1()
			{
				return color1;
			}

			void Border3DElement::SetColor1(CColor value)
			{
				if (color1 != value)
				{
					color1 = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			CColor Border3DElement::GetColor2()
			{
				return color2;
			}

			void Border3DElement::SetColor2(CColor value)
			{
				if (color2 != value)
				{
					color2 = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			void Border3DElement::SetColors(CColor value1, CColor value2)
			{
				if (color1 != value1 || color2 != value2)
				{
					color1 = value1;
					color2 = value2;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			void Border3DElementRenderer::Render(CRect bounds)
			{
				CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
				D2D1_RECT_F rect = D2D1::RectF((FLOAT)bounds.left + 0.5f, (FLOAT)bounds.top + 0.5f, (FLOAT)bounds.right - 0.5f, (FLOAT)bounds.bottom - 0.5f);

				d2dRenderTarget->DrawLine(D2D1::Point2F(rect.left, rect.top), D2D1::Point2F(rect.right, rect.top), brush1);
				d2dRenderTarget->DrawLine(D2D1::Point2F(rect.left, rect.top), D2D1::Point2F(rect.left, rect.bottom), brush1);
				d2dRenderTarget->DrawLine(D2D1::Point2F(rect.right, rect.bottom), D2D1::Point2F(rect.left, rect.bottom), brush2);
				d2dRenderTarget->DrawLine(D2D1::Point2F(rect.right, rect.bottom), D2D1::Point2F(rect.right, rect.top), brush2);
			}

			void Border3DElementRenderer::OnElementStateChanged()
			{
				if (renderTarget)
				{
					CColor color1 = element->GetColor1();
					CColor color2 = element->GetColor2();
					if (oldColor1 != color1 || oldColor2 != color2)
					{
						DestroyBrush(renderTarget);
						CreateBrush(renderTarget);
					}
				}
			}

			void Border3DElementRenderer::CreateBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					oldColor1 = element->GetColor1();
					oldColor2 = element->GetColor2();
					brush1 = _renderTarget->CreateDirect2DBrush(oldColor1);
					brush2 = _renderTarget->CreateDirect2DBrush(oldColor2);
				}
			}

			void Border3DElementRenderer::DestroyBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					if (brush1)
					{
						_renderTarget->DestroyDirect2DBrush(oldColor1);
						brush1 = nullptr;
					}
					if (brush2)
					{
						_renderTarget->DestroyDirect2DBrush(oldColor2);
						brush2 = nullptr;
					}
				}
			}

			void Border3DElementRenderer::InitializeInternal()
			{

			}

			void Border3DElementRenderer::FinalizeInternal()
			{
				DestroyBrush(renderTarget);
			}

			void Border3DElementRenderer::RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget)
			{
				DestroyBrush(oldRenderTarget);
				CreateBrush(newRenderTarget);
			}

#pragma endregion Border3D

#pragma region Splitter3D
			Splitter3DElement::Splitter3DElement()
				: direction(Horizontal)
			{

			}

			Splitter3DElement::~Splitter3DElement()
			{
				renderer->Finalize();
			}

			CString Splitter3DElement::GetElementTypeName()
			{
				return _T("Splitter3D");
			}

			CColor Splitter3DElement::GetColor1()
			{
				return color1;
			}

			void Splitter3DElement::SetColor1(CColor value)
			{
				if (color1 != value)
				{
					color1 = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			CColor Splitter3DElement::GetColor2()
			{
				return color2;
			}

			void Splitter3DElement::SetColor2(CColor value)
			{
				if (color2 != value)
				{
					color2 = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			void Splitter3DElement::SetColors(CColor value1, CColor value2)
			{
				if (color1 != value1 || color2 != value2)
				{
					color1 = value1;
					color2 = value2;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			Splitter3DElement::Direction Splitter3DElement::GetDirection()
			{
				return direction;
			}

			void Splitter3DElement::SetDirection(Direction value)
			{
				direction = value;
			}

			void Splitter3DElementRenderer::Render(CRect bounds)
			{
				D2D1_POINT_2F p11, p12, p21, p22;

				switch (element->GetDirection())
				{
					case Splitter3DElement::Horizontal:
					{
						cint y = bounds.top + bounds.Height() / 2 - 1;
						p11 = D2D1::Point2F((FLOAT)bounds.left, (FLOAT)y + 0.5f);
						p12 = D2D1::Point2F((FLOAT)bounds.right, (FLOAT)y + 0.5f);
						p21 = D2D1::Point2F((FLOAT)bounds.left, (FLOAT)y + 1.5f);
						p22 = D2D1::Point2F((FLOAT)bounds.right, (FLOAT)y + 1.5f);
					}
						break;
					case Splitter3DElement::Vertical:
					{
						cint x = bounds.left + bounds.Width() / 2 - 1;
						p11 = D2D1::Point2F((FLOAT)x + 0.5f, (FLOAT)bounds.top - 0.0f);
						p12 = D2D1::Point2F((FLOAT)x + 0.5f, (FLOAT)bounds.bottom + 0.0f);
						p21 = D2D1::Point2F((FLOAT)x + 1.5f, (FLOAT)bounds.top - 0.0f);
						p22 = D2D1::Point2F((FLOAT)x + 1.5f, (FLOAT)bounds.bottom + 0.0f);
					}
						break;
				}

				CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();

				d2dRenderTarget->DrawLine(p11, p12, brush1);
				d2dRenderTarget->DrawLine(p21, p22, brush2);
			}

			void Splitter3DElementRenderer::OnElementStateChanged()
			{
				if (renderTarget)
				{
					CColor color1 = element->GetColor1();
					CColor color2 = element->GetColor2();
					if (oldColor1 != color1 || oldColor2 != color2)
					{
						DestroyBrush(renderTarget);
						CreateBrush(renderTarget);
					}
				}
			}

			void Splitter3DElementRenderer::CreateBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					oldColor1 = element->GetColor1();
					oldColor2 = element->GetColor2();
					brush1 = _renderTarget->CreateDirect2DBrush(oldColor1);
					brush2 = _renderTarget->CreateDirect2DBrush(oldColor2);
				}
			}

			void Splitter3DElementRenderer::DestroyBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					if (brush1)
					{
						_renderTarget->DestroyDirect2DBrush(oldColor1);
						brush1 = nullptr;
					}
					if (brush2)
					{
						_renderTarget->DestroyDirect2DBrush(oldColor2);
						brush2 = nullptr;
					}
				}
			}

			void Splitter3DElementRenderer::InitializeInternal()
			{

			}

			void Splitter3DElementRenderer::FinalizeInternal()
			{
				DestroyBrush(renderTarget);
			}

			void Splitter3DElementRenderer::RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget)
			{
				DestroyBrush(oldRenderTarget);
				CreateBrush(newRenderTarget);
			}

#pragma endregion Splitter3D

#pragma region SolidBackground
			SolidBackgroundElement::SolidBackgroundElement()
				: shape(ElementShape::Rectangle)
			{

			}

			SolidBackgroundElement::~SolidBackgroundElement()
			{
				renderer->Finalize();
			}

			CString SolidBackgroundElement::GetElementTypeName()
			{
				return _T("SolidBackground");
			}

			CColor SolidBackgroundElement::GetColor()
			{
				return color;
			}

			void SolidBackgroundElement::SetColor(CColor value)
			{
				if (color != value)
				{
					color = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			ElementShape SolidBackgroundElement::GetShape()
			{
				return shape;
			}

			void SolidBackgroundElement::SetShape(ElementShape value)
			{
				shape = value;
			}

			void SolidBackgroundElementRenderer::Render(CRect bounds)
			{
				CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
				switch (element->GetShape())
				{
					case ElementShape::Rectangle:
						d2dRenderTarget->FillRectangle(
							D2D1::RectF((FLOAT)bounds.left, (FLOAT)bounds.top, (FLOAT)bounds.right, (FLOAT)bounds.bottom),
							brush
							);
						break;
					case ElementShape::Ellipse:
						d2dRenderTarget->FillEllipse(
							D2D1::Ellipse(D2D1::Point2F((bounds.left + bounds.right) / 2.0f, (bounds.top + bounds.bottom) / 2.0f), bounds.Width() / 2.0f, bounds.Height() / 2.0f),
							brush
							);
						break;
				}
			}
#pragma endregion SolidBackground

#pragma region GradientBackground
			GradientBackgroundElement::GradientBackgroundElement()
				: shape(ElementShape::Rectangle)
				, direction(Horizontal)
			{

			}

			GradientBackgroundElement::~GradientBackgroundElement()
			{
				renderer->Finalize();
			}

			CString GradientBackgroundElement::GetElementTypeName()
			{
				return _T("GradientBackground");
			}

			CColor GradientBackgroundElement::GetColor1()
			{
				return color1;
			}

			void GradientBackgroundElement::SetColor1(CColor value)
			{
				if (color1 != value)
				{
					color1 = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			CColor GradientBackgroundElement::GetColor2()
			{
				return color2;
			}

			void GradientBackgroundElement::SetColor2(CColor value)
			{
				if (color2 != value)
				{
					color2 = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			void GradientBackgroundElement::SetColors(CColor value1, CColor value2)
			{
				if (color1 != value1 || color2 != value2)
				{
					color1 = value1;
					color2 = value2;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			GradientBackgroundElement::Direction GradientBackgroundElement::GetDirection()
			{
				return direction;
			}

			void GradientBackgroundElement::SetDirection(Direction value)
			{
				direction = value;
			}

			ElementShape GradientBackgroundElement::GetShape()
			{
				return shape;
			}

			void GradientBackgroundElement::SetShape(ElementShape value)
			{
				shape = value;
			}

			void GradientBackgroundElementRenderer::Render(CRect bounds)
			{
				D2D1_POINT_2F points[2];
				switch (element->GetDirection())
				{
					case GradientBackgroundElement::Horizontal:
					{
						points[0].x = (FLOAT)bounds.left;
						points[0].y = (FLOAT)bounds.top;
						points[1].x = (FLOAT)bounds.right;
						points[1].y = (FLOAT)bounds.top;
					}
						break;
					case GradientBackgroundElement::Vertical:
					{
						points[0].x = (FLOAT)bounds.left;
						points[0].y = (FLOAT)bounds.top;
						points[1].x = (FLOAT)bounds.left;
						points[1].y = (FLOAT)bounds.bottom;
					}
						break;
					case GradientBackgroundElement::Slash:
					{
						points[0].x = (FLOAT)bounds.right;
						points[0].y = (FLOAT)bounds.top;
						points[1].x = (FLOAT)bounds.left;
						points[1].y = (FLOAT)bounds.bottom;
					}
						break;
					case GradientBackgroundElement::Backslash:
					{
						points[0].x = (FLOAT)bounds.left;
						points[0].y = (FLOAT)bounds.top;
						points[1].x = (FLOAT)bounds.right;
						points[1].y = (FLOAT)bounds.bottom;
					}
						break;
				}

				brush->SetStartPoint(points[0]);
				brush->SetEndPoint(points[1]);

				CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
				switch (element->GetShape())
				{
					case ElementShape::Rectangle:
						d2dRenderTarget->FillRectangle(
							D2D1::RectF((FLOAT)bounds.left, (FLOAT)bounds.top, (FLOAT)bounds.right, (FLOAT)bounds.bottom),
							brush
							);
						break;
					case ElementShape::Ellipse:
						d2dRenderTarget->FillEllipse(
							D2D1::Ellipse(D2D1::Point2F((bounds.left + bounds.right) / 2.0f, (bounds.top + bounds.bottom) / 2.0f), bounds.Width() / 2.0f, bounds.Height() / 2.0f),
							brush
							);
						break;
				}
			}
#pragma endregion GradientBackground

#pragma region SolidLabel
			SolidLabelElement::SolidLabelElement()
				: hAlignment(Alignment::StringAlignmentNear)
				, vAlignment(Alignment::StringAlignmentNear)
				, wrapLine(false)
				, ellipse(false)
				, multiline(false)
				, wrapLineHeightCalculation(false)
			{
				fontProperties.fontFamily = _T("Microsoft Yahei");
				fontProperties.size = 12;
			}

			SolidLabelElement::~SolidLabelElement()
			{
				renderer->Finalize();
			}

			CString SolidLabelElement::GetElementTypeName()
			{
				return _T("SolidLabel");
			}

			CColor SolidLabelElement::GetColor()
			{
				return color;
			}

			void SolidLabelElement::SetColor(CColor value)
			{
				if (color != value)
				{
					color = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			const Font& SolidLabelElement::GetFont()
			{
				return fontProperties;
			}

			void SolidLabelElement::SetFont(const Font& value)
			{
				if (fontProperties != value)
				{
					fontProperties = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			const CString& SolidLabelElement::GetText()
			{
				return text;
			}

			void SolidLabelElement::SetText(const CString& value)
			{
				if (text != value)
				{
					text = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			Alignment SolidLabelElement::GetHorizontalAlignment()
			{
				return hAlignment;
			}

			Alignment SolidLabelElement::GetVerticalAlignment()
			{
				return vAlignment;
			}

			void SolidLabelElement::SetHorizontalAlignment(Alignment value)
			{
				SetAlignments(value, vAlignment);
			}

			void SolidLabelElement::SetVerticalAlignment(Alignment value)
			{
				SetAlignments(hAlignment, value);
			}

			void SolidLabelElement::SetAlignments(Alignment horizontal, Alignment vertical)
			{
				if (hAlignment != horizontal || vAlignment != vertical)
				{
					hAlignment = horizontal;
					vAlignment = vertical;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			bool SolidLabelElement::GetWrapLine()
			{
				return wrapLine;
			}

			void SolidLabelElement::SetWrapLine(bool value)
			{
				if (wrapLine != value)
				{
					wrapLine = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			bool SolidLabelElement::GetEllipse()
			{
				return ellipse;
			}

			void SolidLabelElement::SetEllipse(bool value)
			{
				if (ellipse != value)
				{
					ellipse = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			bool SolidLabelElement::GetMultiline()
			{
				return multiline;
			}

			void SolidLabelElement::SetMultiline(bool value)
			{
				if (multiline != value)
				{
					multiline = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			bool SolidLabelElement::GetWrapLineHeightCalculation()
			{
				return wrapLineHeightCalculation;
			}

			void SolidLabelElement::SetWrapLineHeightCalculation(bool value)
			{
				if (wrapLineHeightCalculation != value)
				{
					wrapLineHeightCalculation = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			SolidLabelElementRenderer::SolidLabelElementRenderer()
				: oldMaxWidth(-1)
			{

			}

			void SolidLabelElementRenderer::Render(CRect bounds)
			{
				if (!textLayout)
				{
					CreateTextLayout();
				}

				cint x = 0;
				cint y = 0;
				switch (element->GetHorizontalAlignment())
				{
					case Alignment::StringAlignmentNear:
						x = bounds.left;
						break;
					case Alignment::StringAlignmentCenter:
						x = bounds.left + (bounds.Width() - minSize.cx) / 2;
						break;
					case Alignment::StringAlignmentFar:
						x = bounds.right - minSize.cx;
						break;
				}
				switch (element->GetVerticalAlignment())
				{
					case Alignment::StringAlignmentNear:
						y = bounds.top;
						break;
					case Alignment::StringAlignmentCenter:
						y = bounds.top + (bounds.Height() - minSize.cy) / 2;
						break;
					case Alignment::StringAlignmentFar:
						y = bounds.bottom - minSize.cy;
						break;
				}

				renderTarget->SetTextAntialias(oldFont.antialias, oldFont.verticalAntialias);

				if (!element->GetEllipse() && !element->GetMultiline() && !element->GetWrapLine())
				{
					CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
					d2dRenderTarget->DrawTextLayout(
						D2D1::Point2F((FLOAT)x, (FLOAT)y),
						textLayout,
						brush,
						D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
						);
				}
				else
				{
					CComPtr<IDWriteFactory> dwriteFactory = GetStorage()->GetDirect2DProvider()->GetDirectWriteFactory();
					DWRITE_TRIMMING trimming;
					CComPtr<IDWriteInlineObject> inlineObject;
					textLayout->GetTrimming(&trimming, &inlineObject);

					textLayout->SetWordWrapping(element->GetWrapLine() ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
					if (element->GetEllipse())
					{
						textLayout->SetTrimming(&textFormat->trimming, textFormat->ellipseInlineObject);
					}
					switch (element->GetHorizontalAlignment())
					{
						case Alignment::StringAlignmentNear:
							textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
							break;
						case Alignment::StringAlignmentCenter:
							textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
							break;
						case Alignment::StringAlignmentFar:
							textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
							break;
					}
					if (!element->GetMultiline() && !element->GetWrapLine())
					{
						switch (element->GetVerticalAlignment())
						{
							case Alignment::StringAlignmentNear:
								textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
								break;
							case Alignment::StringAlignmentCenter:
								textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
								break;
							case Alignment::StringAlignmentFar:
								textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
								break;
						}
					}

					CRect textBounds = bounds;
					if (element->GetEllipse() && !element->GetMultiline() && !element->GetWrapLine())
					{
						textBounds = CRect(CPoint(textBounds.left, y), CSize(bounds.Width(), minSize.cy));
					}

					textLayout->SetMaxWidth((FLOAT)textBounds.Width());
					textLayout->SetMaxHeight((FLOAT)textBounds.Height());

					CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
					d2dRenderTarget->DrawTextLayout(
						D2D1::Point2F((FLOAT)textBounds.left, (FLOAT)textBounds.top),
						textLayout,
						brush,
						D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
						);

					textLayout->SetTrimming(&trimming, inlineObject);
					if (oldMaxWidth != textBounds.Width())
					{
						oldMaxWidth = textBounds.Width();
						UpdateMinSize();
					}
				}
			}

			void SolidLabelElementRenderer::OnElementStateChanged()
			{
				if (renderTarget)
				{
					CColor color = element->GetColor();
					if (oldColor != color)
					{
						DestroyBrush(renderTarget);
						CreateBrush(renderTarget);
					}

					Font font = element->GetFont();
					if (oldFont != font)
					{
						DestroyTextFormat(renderTarget);
						CreateTextFormat(renderTarget);
					}
				}
				oldText = element->GetText();
				UpdateMinSize();
			}

			void SolidLabelElementRenderer::CreateBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					oldColor = element->GetColor();
					brush = _renderTarget->CreateDirect2DBrush(oldColor);
				}
			}

			void SolidLabelElementRenderer::DestroyBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget && brush)
				{
					_renderTarget->DestroyDirect2DBrush(oldColor);
					brush = nullptr;
				}
			}

			void SolidLabelElementRenderer::CreateTextFormat(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					RefPtr<ID2DResourceManager> resourceManager = GetStorage()->GetDirect2DResourceManager();
					oldFont = element->GetFont();
					textFormat = resourceManager->CreateDirect2DTextFormat(oldFont);
				}
			}

			void SolidLabelElementRenderer::DestroyTextFormat(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget && textFormat)
				{
					RefPtr<ID2DResourceManager> resourceManager = GetStorage()->GetDirect2DResourceManager();
					resourceManager->DestroyDirect2DTextFormat(oldFont);
					textFormat = nullptr;
				}
			}

			void SolidLabelElementRenderer::CreateTextLayout()
			{
				if (textFormat)
				{
					BSTR _text = oldText.AllocSysString();
					HRESULT hr = GetStorage()->GetDirect2DProvider()->GetDirectWriteFactory()->CreateTextLayout(
						_text,
						oldText.GetLength(),
						textFormat->textFormat,
						0,
						0,
						&textLayout);
					SysFreeString(_text);
					if (SUCCEEDED(hr))
					{
						if (oldFont.underline)
						{
							DWRITE_TEXT_RANGE textRange;
							textRange.startPosition = 0;
							textRange.length = oldText.GetLength();
							textLayout->SetUnderline(TRUE, textRange);
						}
						if (oldFont.strikeline)
						{
							DWRITE_TEXT_RANGE textRange;
							textRange.startPosition = 0;
							textRange.length = oldText.GetLength();
							textLayout->SetStrikethrough(TRUE, textRange);
						}
					}
					else
					{
						textLayout = nullptr;
					}
				}
			}

			void SolidLabelElementRenderer::DestroyTextLayout()
			{
				if (textLayout)
				{
					textLayout = nullptr;
				}
			}

			void SolidLabelElementRenderer::UpdateMinSize()
			{
				float maxWidth = 0;
				DestroyTextLayout();
				bool calculateSizeFromTextLayout = false;
				if (renderTarget)
				{
					if (element->GetWrapLine())
					{
						if (element->GetWrapLineHeightCalculation())
						{
							CreateTextLayout();
							if (textLayout)
							{
								maxWidth = textLayout->GetMaxWidth();
								if (oldMaxWidth != -1)
								{
									textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
									textLayout->SetMaxWidth((float)oldMaxWidth);
								}
								calculateSizeFromTextLayout = true;
							}
						}
					}
					else
					{
						CreateTextLayout();
						if (textLayout)
						{
							maxWidth = textLayout->GetMaxWidth();
							calculateSizeFromTextLayout = true;
						}
					}
				}
				if (calculateSizeFromTextLayout)
				{
					DWRITE_TEXT_METRICS metrics;
					HRESULT hr = textLayout->GetMetrics(&metrics);
					if (SUCCEEDED(hr))
					{
						cint width = 0;
						if (!element->GetEllipse() && !element->GetWrapLine() && !element->GetMultiline())
						{
							width = (cint)ceil(metrics.widthIncludingTrailingWhitespace);
						}
						minSize = CSize(width, (cint)ceil(metrics.height));
					}
					textLayout->SetMaxWidth(maxWidth);
				}
				else
				{
					minSize = CSize();
				}
			}

			void SolidLabelElementRenderer::InitializeInternal()
			{

			}

			void SolidLabelElementRenderer::FinalizeInternal()
			{
				DestroyTextLayout();
				DestroyBrush(renderTarget);
				DestroyTextFormat(renderTarget);
			}

			void SolidLabelElementRenderer::RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget)
			{
				DestroyBrush(oldRenderTarget.get());
				DestroyTextFormat(oldRenderTarget.get());
				CreateBrush(newRenderTarget.get());
				CreateTextFormat(newRenderTarget.get());
				UpdateMinSize();
			}
#pragma endregion SolidLabel

#pragma region ImageFrame
			ImageFrameElement::ImageFrameElement()
				: frameIndex(0)
				, hAlignment(Alignment::StringAlignmentNear)
				, vAlignment(Alignment::StringAlignmentNear)
				, stretch(false)
				, enabled(true)
			{

			}

			ImageFrameElement::~ImageFrameElement()
			{
				renderer->Finalize();
			}

			CString ImageFrameElement::GetElementTypeName()
			{
				return _T("ImageFrame");
			}

			RefPtr<IImage> ImageFrameElement::GetImage()
			{
				return image;
			}

			cint ImageFrameElement::GetFrameIndex()
			{
				return frameIndex;
			}

			void ImageFrameElement::SetImage(PassRefPtr<IImage> value)
			{
				SetImage(value, frameIndex);
			}

			void ImageFrameElement::SetImage(PassRefPtr<IImage> _image, cint _frameIndex)
			{
				if (image != _image || frameIndex != _frameIndex)
				{
					if (!_image)
					{
						image = nullptr;
						frameIndex = 0;
					}
					else if (0 <= _frameIndex && _frameIndex < _image->GetFrameCount())
					{
						image = _image;
						frameIndex = _frameIndex;
					}
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			void ImageFrameElement::SetFrameIndex(cint value)
			{
				SetImage(image, value);
			}

			Alignment ImageFrameElement::GetHorizontalAlignment()
			{
				return hAlignment;
			}

			Alignment ImageFrameElement::GetVerticalAlignment()
			{
				return vAlignment;
			}

			void ImageFrameElement::SetHorizontalAlignment(Alignment value)
			{
				SetAlignments(value, vAlignment);
			}

			void ImageFrameElement::SetVerticalAlignment(Alignment value)
			{
				SetAlignments(hAlignment, value);
			}

			void ImageFrameElement::SetAlignments(Alignment horizontal, Alignment vertical)
			{
				if (hAlignment != horizontal || vAlignment != vertical)
				{
					hAlignment = horizontal;
					vAlignment = vertical;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			bool ImageFrameElement::GetStretch()
			{
				return stretch;
			}

			void ImageFrameElement::SetStretch(bool value)
			{
				if (stretch != value)
				{
					stretch = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			bool ImageFrameElement::GetEnabled()
			{
				return enabled;
			}

			void ImageFrameElement::SetEnabled(bool value)
			{
				if (enabled != value)
				{
					enabled = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			void ImageFrameElementRenderer::Render(CRect bounds)
			{
				if (!bitmap)
					return;

				CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
				D2D1_RECT_F source = D2D1::RectF(0, 0, (FLOAT)minSize.cx, (FLOAT)minSize.cy);
				D2D1_RECT_F destination;
				if (element->GetStretch())
				{
					destination = D2D1::RectF((FLOAT)bounds.left, (FLOAT)bounds.top, (FLOAT)bounds.right, (FLOAT)bounds.bottom);
				}
				else
				{
					cint x = 0;
					cint y = 0;
					switch (element->GetHorizontalAlignment())
					{
						case Alignment::StringAlignmentNear:
							x = bounds.left;
							break;
						case Alignment::StringAlignmentCenter:
							x = bounds.left + (bounds.Width() - minSize.cx) / 2;
							break;
						case Alignment::StringAlignmentFar:
							x = bounds.right - minSize.cx;
							break;
					}
					switch (element->GetVerticalAlignment())
					{
						case Alignment::StringAlignmentNear:
							y = bounds.top;
							break;
						case Alignment::StringAlignmentCenter:
							y = bounds.top + (bounds.Height() - minSize.cy) / 2;
							break;
						case Alignment::StringAlignmentFar:
							y = bounds.bottom - minSize.cy;
							break;
					}
					destination = D2D1::RectF((FLOAT)x, (FLOAT)y, (FLOAT)(x + minSize.cx), (FLOAT)(y + minSize.cy));
				}
				if (element->GetImage()->GetFormat() == IImage::Gif && element->GetFrameIndex() > 0)
				{
					cint max = element->GetFrameIndex();
					for (cint i = 0; i <= max; i++)
					{
						CComPtr<ID2D1Bitmap> frameBitmap = renderTarget->GetBitmap(element->GetImage()->GetFrame(i), element->GetEnabled());
						d2dRenderTarget->DrawBitmap(frameBitmap, destination, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, source);
					}
				}
				else
				{
					d2dRenderTarget->DrawBitmap(bitmap, destination, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, source);
				}
			}

			void ImageFrameElementRenderer::OnElementStateChanged()
			{
				UpdateBitmap(renderTarget);
			}

			void ImageFrameElementRenderer::UpdateBitmap(PassRefPtr<ID2DRenderTarget> renderTarget)
			{
				if (renderTarget && element->GetImage())
				{
					RefPtr<IImageFrame> frame = element->GetImage()->GetFrame(element->GetFrameIndex());
					bitmap = renderTarget->GetBitmap(frame, element->GetEnabled());
					minSize = frame->GetSize();
				}
				else
				{
					bitmap = nullptr;
					minSize = CSize(0, 0);
				}
			}

			void ImageFrameElementRenderer::InitializeInternal()
			{

			}

			void ImageFrameElementRenderer::FinalizeInternal()
			{

			}

			void ImageFrameElementRenderer::RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget)
			{
				UpdateBitmap(newRenderTarget);
			}
#pragma endregion ImageFrame
		
#pragma region Polygon
			PolygonElement::PolygonElement()
			{

			}

			PolygonElement::~PolygonElement()
			{

			}

			CString PolygonElement::GetElementTypeName()
			{
				return _T("Polygon");
			}

			CSize PolygonElement::GetSize()
			{
				return size;
			}

			void PolygonElement::SetSize(CSize value)
			{
				if (size != value)
				{
					size = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			const CPoint& PolygonElement::GetPoint(cint index)
			{
				return points[index];
			}

			cint PolygonElement::GetPointCount()
			{
				return points.size();
			}

			void PolygonElement::SetPoints(const CPoint* p, cint count)
			{
				points.resize(count);
				copy(p, p + count, points.begin());
				if (renderer)
				{
					renderer->OnElementStateChanged();
				}
			}

			const vector<CPoint>& PolygonElement::GetPointsArray()
			{
				return points;
			}

			void PolygonElement::SetPointsArray(const vector<CPoint>& value)
			{
				points = value;
			}

			CColor PolygonElement::GetBorderColor()
			{
				return borderColor;
			}

			void PolygonElement::SetBorderColor(CColor value)
			{
				if (borderColor != value)
				{
					borderColor = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			CColor PolygonElement::GetBackgroundColor()
			{
				return backgroundColor;
			}

			void PolygonElement::SetBackgroundColor(CColor value)
			{
				if (backgroundColor != value)
				{
					backgroundColor = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			void PolygonElementRenderer::Render(CRect bounds)
			{
				if (renderTarget && geometry)
				{
					CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
					cint offsetX = (bounds.Width() - minSize.cx) / 2 + bounds.left;
					cint offsetY = (bounds.Height() - minSize.cy) / 2 + bounds.top;

					D2D1_MATRIX_3X2_F oldT, newT;
					d2dRenderTarget->GetTransform(&oldT);
					newT = D2D1::Matrix3x2F::Translation((FLOAT)offsetX, (FLOAT)offsetY);
					d2dRenderTarget->SetTransform(&newT);

					d2dRenderTarget->FillGeometry(geometry, backgroundBrush);
					d2dRenderTarget->DrawGeometry(geometry, borderBrush);
					d2dRenderTarget->SetTransform(&oldT);
				}
			}

			void PolygonElementRenderer::OnElementStateChanged()
			{
				auto & pointArray = element->GetPointsArray();

				minSize = element->GetSize();
				RecreateResource(renderTarget, renderTarget);

				bool polygonModified = false;
				if (oldPoints.size() != pointArray.size())
				{
					polygonModified = true;
				}
				else
				{
					for (cuint i = 0; i < oldPoints.size(); i++)
					{
						if (oldPoints[i] != pointArray[i])
						{
							polygonModified = true;
							break;
						}
					}
				}
				if (polygonModified)
				{
					DestroyGeometry();
					CreateGeometry();
				}
			}

			void PolygonElementRenderer::CreateGeometry()
			{
				auto & pointArray = element->GetPointsArray();
				oldPoints = pointArray;
				if (oldPoints.size() >= 3)
				{
					CComPtr<ID2D1PathGeometry> pg;
					GetStorage()->GetDirect2DFactory()->CreatePathGeometry(&pg);
					if (pg)
					{
						geometry = pg;
						FillGeometry(CPoint());
					}
				}
			}

			void PolygonElementRenderer::DestroyGeometry()
			{
				if (geometry)
				{
					geometry = nullptr;
				}
			}

			void PolygonElementRenderer::FillGeometry(CPoint offset)
			{
				if (!geometry)
					return;

				CComPtr<ID2D1GeometrySink> pgs;
				geometry->Open(&pgs);
				if (pgs)
				{
					D2D1_POINT_2F p;
					p.x = (FLOAT)(oldPoints[0].x + offset.x) + 0.5f;
					p.y = (FLOAT)(oldPoints[0].y + offset.y) + 0.5f;
					pgs->BeginFigure(p, D2D1_FIGURE_BEGIN_FILLED);
					for (auto & oldPoint : oldPoints)
					{
						p.x = (FLOAT)(oldPoint.x + offset.x) + 0.5f;
						p.y = (FLOAT)(oldPoint.y + offset.y) + 0.5f;
						pgs->AddLine(p);
					}
					pgs->EndFigure(D2D1_FIGURE_END_CLOSED);
					pgs->Close();
				}
			}

			void PolygonElementRenderer::RecreateResource(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget)
			{
				if (oldRenderTarget == newRenderTarget)
				{
					if (oldRenderTarget)
					{
						if (oldBorderColor != element->GetBorderColor())
						{
							oldRenderTarget->DestroyDirect2DBrush(oldBorderColor);
							oldBorderColor = element->GetBorderColor();
							borderBrush = newRenderTarget->CreateDirect2DBrush(oldBorderColor);
						}
						if (oldBackgroundColor != element->GetBackgroundColor())
						{
							oldRenderTarget->DestroyDirect2DBrush(oldBackgroundColor);
							oldBackgroundColor = element->GetBackgroundColor();
							backgroundBrush = newRenderTarget->CreateDirect2DBrush(oldBackgroundColor);
						}
					}
				}
				else
				{
					if (oldRenderTarget)
					{
						oldRenderTarget->DestroyDirect2DBrush(oldBorderColor);
						oldRenderTarget->DestroyDirect2DBrush(oldBackgroundColor);
					}
					if (newRenderTarget)
					{
						oldBorderColor = element->GetBorderColor();
						oldBackgroundColor = element->GetBackgroundColor();
						borderBrush = newRenderTarget->CreateDirect2DBrush(oldBorderColor);
						backgroundBrush = newRenderTarget->CreateDirect2DBrush(oldBackgroundColor);
					}
				}
			}

			void PolygonElementRenderer::InitializeInternal()
			{
				oldBorderColor = element->GetBorderColor();
				oldBackgroundColor = element->GetBackgroundColor();
				RecreateResource(nullptr, renderTarget);
				CreateGeometry();
			}

			void PolygonElementRenderer::FinalizeInternal()
			{
				DestroyGeometry();
				RecreateResource(renderTarget, nullptr);
			}

			void PolygonElementRenderer::RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget)
			{
				RecreateResource(oldRenderTarget, newRenderTarget);
			}
#pragma endregion Polygon

#pragma region ColorizedText
			ColorizedTextElement::ColorizedTextElement()
				: isVisuallyEnabled(true)
				, isFocused(false)
				, caretVisible(false)
			{

			}

			ColorizedTextElement::~ColorizedTextElement()
			{
				renderer->Finalize();
			}

			CString ColorizedTextElement::GetElementTypeName()
			{
				return _T("ColorizedText");
			}

			TextLines& ColorizedTextElement::GetLines()
			{
				return lines;
			}

			PassRefPtr<IColorizedTextCallback> ColorizedTextElement::GetCallback()
			{
				return callback;
			}

			void ColorizedTextElement::SetCallback(PassRefPtr<IColorizedTextCallback> value)
			{
				callback = value;
				if (!callback)
				{
					lines.SetCharMeasurer(nullptr);
				}
			}

			const vector<ColorEntry>& ColorizedTextElement::GetColors()
			{
				return colors;
			}

			void ColorizedTextElement::SetColors(const vector<ColorEntry>& value)
			{
				colors = value;
				if (callback) callback->ColorChanged();
				if (renderer)
				{
					renderer->OnElementStateChanged();
				}
			}

			cint ColorizedTextElement::AddColor(const ColorEntry& value)
			{
				colors.push_back(value);
				if (callback) callback->ColorChanged();
				if (renderer)
				{
					renderer->OnElementStateChanged();
				}
				return (cint)colors.size() - 1;
			}

			const Font& ColorizedTextElement::GetFont()
			{
				return font;
			}

			void ColorizedTextElement::SetFont(const Font& value)
			{
				if (font != value)
				{
					font = value;
					if (callback)
					{
						callback->FontChanged();
					}
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			TCHAR ColorizedTextElement::GetPasswordChar()
			{
				return lines.GetPasswordChar();
			}

			void ColorizedTextElement::SetPasswordChar(TCHAR value)
			{
				if (lines.GetPasswordChar() != value)
				{
					lines.SetPasswordChar(value);
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			CPoint ColorizedTextElement::GetViewPosition()
			{
				return viewPosition;
			}

			void ColorizedTextElement::SetViewPosition(CPoint value)
			{
				if (viewPosition != value)
				{
					viewPosition = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			bool ColorizedTextElement::GetVisuallyEnabled()
			{
				return isVisuallyEnabled;
			}

			void ColorizedTextElement::SetVisuallyEnabled(bool value)
			{
				if (isVisuallyEnabled != value)
				{
					isVisuallyEnabled = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			bool ColorizedTextElement::GetFocused()
			{
				return isFocused;
			}

			void ColorizedTextElement::SetFocused(bool value)
			{
				if (isFocused != value)
				{
					isFocused = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			TextPos ColorizedTextElement::GetCaretBegin()
			{
				return caretBegin;
			}

			void ColorizedTextElement::SetCaretBegin(TextPos value)
			{
				caretBegin = value;
			}

			TextPos ColorizedTextElement::GetCaretEnd()
			{
				return caretEnd;
			}

			void ColorizedTextElement::SetCaretEnd(TextPos value)
			{
				caretEnd = value;
			}

			bool ColorizedTextElement::GetCaretVisible()
			{
				return caretVisible;
			}

			void ColorizedTextElement::SetCaretVisible(bool value)
			{
				caretVisible = value;
			}

			CColor ColorizedTextElement::GetCaretColor()
			{
				return caretColor;
			}

			void ColorizedTextElement::SetCaretColor(CColor value)
			{
				if (caretColor != value)
				{
					caretColor = value;
					if (renderer)
					{
						renderer->OnElementStateChanged();
					}
				}
			}

			void ColorizedTextElementRenderer::Render(CRect bounds)
			{
				if (renderTarget)
				{
					CComPtr<ID2D1RenderTarget> d2dRenderTarget = renderTarget->GetDirect2DRenderTarget();
					TCHAR passwordChar = element->GetPasswordChar();
					CPoint viewPosition = element->GetViewPosition();
					CRect viewBounds(viewPosition, bounds.Size());
					cint startRow = element->GetLines().GetTextPosFromPoint(CPoint(viewBounds.left, viewBounds.top)).row;
					cint endRow = element->GetLines().GetTextPosFromPoint(CPoint(viewBounds.right, viewBounds.bottom)).row;
					TextPos selectionBegin = __min(element->GetCaretBegin(), element->GetCaretEnd());
					TextPos selectionEnd = __max(element->GetCaretBegin(), element->GetCaretEnd());
					bool focused = element->GetFocused();

					renderTarget->SetTextAntialias(oldFont.antialias, oldFont.verticalAntialias);

					for (cint row = startRow; row <= endRow; row++)
					{
						CRect startRect = element->GetLines().GetRectFromTextPos(TextPos(row, 0));
						CPoint startPoint = startRect.TopLeft();
						cint startColumn = element->GetLines().GetTextPosFromPoint(CPoint(viewBounds.left, startPoint.y)).column;
						cint endColumn = element->GetLines().GetTextPosFromPoint(CPoint(viewBounds.right, startPoint.y)).column;
						TextLine& line = element->GetLines().GetLine(row);

						cint x = startColumn == 0 ? 0 : line.att[startColumn - 1].rightOffset;
						for (cint column = startColumn; column <= endColumn; column++)
						{
							bool inSelection = false;
							if (selectionBegin.row == selectionEnd.row)
							{
								inSelection = (row == selectionBegin.row && selectionBegin.column <= column && column < selectionEnd.column);
							}
							else if (row == selectionBegin.row)
							{
								inSelection = selectionBegin.column <= column;
							}
							else if (row == selectionEnd.row)
							{
								inSelection = column < selectionEnd.column;
							}
							else
							{
								inSelection = selectionBegin.row < row && row < selectionEnd.row;
							}

							bool crlf = column == line.dataLength;
							cint colorIndex = crlf ? 0 : line.att[column].colorIndex;
							if (colorIndex >= (cint)colors.size())
							{
								colorIndex = 0;
							}
							ColorItemResource& color =
								!inSelection ? colors[colorIndex].normal :
								focused ? colors[colorIndex].selectedFocused :
								colors[colorIndex].selectedUnfocused;
							cint x2 = crlf ? x + startRect.Height() / 2 : line.att[column].rightOffset;
							cint tx = x - viewPosition.x + bounds.left;
							cint ty = startPoint.y - viewPosition.y + bounds.top;

							if (color.background.a > 0)
							{
								d2dRenderTarget->FillRectangle(D2D1::RectF((FLOAT)tx, (FLOAT)ty, (FLOAT)(tx + (x2 - x)), (FLOAT)(ty + startRect.Height())), color.backgroundBrush);
							}
							if (!crlf)
							{
								d2dRenderTarget->DrawText(
									(passwordChar ? &passwordChar : &line.text[column]),
									1,
									textFormat->textFormat,
									D2D1::RectF((FLOAT)tx, (FLOAT)ty, (FLOAT)tx + 1, (FLOAT)ty + 1),
									color.textBrush,
									D2D1_DRAW_TEXT_OPTIONS_NO_SNAP,
									DWRITE_MEASURING_MODE_GDI_NATURAL
									);
							}
							x = x2;
						}
					}

					if (element->GetCaretVisible() && element->GetLines().IsAvailable(element->GetCaretEnd()))
					{
						CPoint caretPoint = element->GetLines().GetPointFromTextPos(element->GetCaretEnd());
						cint height = element->GetLines().GetRowHeight();
						CPoint p1(caretPoint.x - viewPosition.x + bounds.left, caretPoint.y - viewPosition.y + bounds.top + 1);
						CPoint p2(caretPoint.x - viewPosition.x + bounds.left, caretPoint.y + height - viewPosition.y + bounds.top - 1);
						d2dRenderTarget->DrawLine(
							D2D1::Point2F((FLOAT)p1.x + 0.5f, (FLOAT)p1.y),
							D2D1::Point2F((FLOAT)p2.x + 0.5f, (FLOAT)p2.y),
							caretBrush
							);
						d2dRenderTarget->DrawLine(
							D2D1::Point2F((FLOAT)p1.x - 0.5f, (FLOAT)p1.y),
							D2D1::Point2F((FLOAT)p2.x - 0.5f, (FLOAT)p2.y),
							caretBrush
							);
					}
				}
			}

			void ColorizedTextElementRenderer::OnElementStateChanged()
			{
				if (renderTarget)
				{
					CColor caretColor = element->GetCaretColor();
					if (oldCaretColor != caretColor)
					{
						DestroyCaretBrush(renderTarget);
						CreateCaretBrush(renderTarget);
					}
				}
			}

			void ColorizedTextElementRenderer::CreateTextBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					auto & colorArray = element->GetColors();
					colors.resize(colorArray.size());
					for (cuint i = 0; i < colors.size(); i++)
					{
						ColorEntry entry = colorArray[i];
						ColorEntryResource newEntry;

						newEntry.normal.text = entry.normal.text;
						newEntry.normal.textBrush = _renderTarget->CreateDirect2DBrush(newEntry.normal.text);
						newEntry.normal.background = entry.normal.background;
						newEntry.normal.backgroundBrush = _renderTarget->CreateDirect2DBrush(newEntry.normal.background);
						newEntry.selectedFocused.text = entry.selectedFocused.text;
						newEntry.selectedFocused.textBrush = _renderTarget->CreateDirect2DBrush(newEntry.selectedFocused.text);
						newEntry.selectedFocused.background = entry.selectedFocused.background;
						newEntry.selectedFocused.backgroundBrush = _renderTarget->CreateDirect2DBrush(newEntry.selectedFocused.background);
						newEntry.selectedUnfocused.text = entry.selectedUnfocused.text;
						newEntry.selectedUnfocused.textBrush = _renderTarget->CreateDirect2DBrush(newEntry.selectedUnfocused.text);
						newEntry.selectedUnfocused.background = entry.selectedUnfocused.background;
						newEntry.selectedUnfocused.backgroundBrush = _renderTarget->CreateDirect2DBrush(newEntry.selectedUnfocused.background);
						colors[i] = newEntry;
					}
				}
			}

			void ColorizedTextElementRenderer::DestroyTextBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					for (auto & color : colors)
					{
						_renderTarget->DestroyDirect2DBrush(color.normal.text);
						_renderTarget->DestroyDirect2DBrush(color.normal.background);
						_renderTarget->DestroyDirect2DBrush(color.selectedFocused.text);
						_renderTarget->DestroyDirect2DBrush(color.selectedFocused.background);
						_renderTarget->DestroyDirect2DBrush(color.selectedUnfocused.text);
						_renderTarget->DestroyDirect2DBrush(color.selectedUnfocused.background);
					}
					colors.clear();
				}
			}

			void ColorizedTextElementRenderer::CreateCaretBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					oldCaretColor = element->GetCaretColor();
					caretBrush = _renderTarget->CreateDirect2DBrush(oldCaretColor);
				}
			}

			void ColorizedTextElementRenderer::DestroyCaretBrush(PassRefPtr<ID2DRenderTarget> _renderTarget)
			{
				if (_renderTarget)
				{
					if (caretBrush)
					{
						_renderTarget->DestroyDirect2DBrush(oldCaretColor);
						caretBrush = nullptr;
					}
				}
			}

			void ColorizedTextElementRenderer::ColorChanged()
			{
				DestroyTextBrush(renderTarget);
				CreateTextBrush(renderTarget);
			}

			void ColorizedTextElementRenderer::FontChanged()
			{
				RefPtr<ID2DResourceManager> resourceManager = GetStorage()->GetDirect2DResourceManager();
				if (textFormat)
				{
					resourceManager->DestroyDirect2DTextFormat(oldFont);
					resourceManager->DestroyDirect2DCharMeasurer(oldFont);
				}
				oldFont = element->GetFont();
				textFormat = resourceManager->CreateDirect2DTextFormat(oldFont);
				element->GetLines().SetCharMeasurer(resourceManager->CreateDirect2DCharMeasurer(oldFont));
			}

			PassRefPtr<CharMeasurer> ColorizedTextElementRenderer::GetCharMeasurer()
			{
				return nullptr;
			}

			void ColorizedTextElementRenderer::InitializeInternal()
			{
				textFormat = nullptr;
				caretBrush = nullptr;
				element->SetCallback(this);
			}

			void ColorizedTextElementRenderer::FinalizeInternal()
			{
				DestroyTextBrush(renderTarget);
				DestroyCaretBrush(renderTarget);

				RefPtr<ID2DResourceManager> resourceManager = GetStorage()->GetDirect2DResourceManager();
				if (textFormat)
				{
					resourceManager->DestroyDirect2DTextFormat(oldFont);
					resourceManager->DestroyDirect2DCharMeasurer(oldFont);
				}
			}

			void ColorizedTextElementRenderer::RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget)
			{
				DestroyTextBrush(oldRenderTarget.get());
				DestroyCaretBrush(oldRenderTarget.get());
				CreateTextBrush(newRenderTarget.get());
				CreateCaretBrush(newRenderTarget.get());
				element->GetLines().SetRenderTarget(newRenderTarget.get());
			}
#pragma endregion ColorizedText

#pragma region Direct2D
			Direct2DElement::Direct2DElement()
			{

			}

			Direct2DElement::~Direct2DElement()
			{

			}

			CString Direct2DElement::GetElementTypeName()
			{
				return _T("Direct2D");
			}

			Direct2DElementRenderer::Direct2DElementRenderer()
			{

			}

			Direct2DElementRenderer::~Direct2DElementRenderer()
			{

			}

			void Direct2DElementRenderer::Render(CRect bounds)
			{
				if (renderTarget)
				{
					CComPtr<IDWriteFactory> fdw = GetStorage()->GetDirect2DProvider()->GetDirectWriteFactory();
					CComPtr<ID2D1Factory> fd2d = GetStorage()->GetDirect2DProvider()->GetDirect2DFactory();
					renderTarget->PushClipper(bounds);
					if (!renderTarget->IsClipperCoverWholeTarget())
					{
						CComPtr<ID2D1RenderTarget> rt = renderTarget->GetDirect2DRenderTarget();
						Direct2DElementEventArgs arguments(element, renderTarget, rt, fdw, fd2d, bounds);
						element->Rendering.Execute(arguments);
					}
					renderTarget->PopClipper();
				}
			}

			void Direct2DElementRenderer::OnElementStateChanged()
			{

			}

			void Direct2DElementRenderer::InitializeInternal()
			{

			}

			void Direct2DElementRenderer::FinalizeInternal()
			{

			}

			void Direct2DElementRenderer::RenderTargetChangedInternal(PassRefPtr<ID2DRenderTarget> oldRenderTarget, PassRefPtr<ID2DRenderTarget> newRenderTarget)
			{
				CComPtr<IDWriteFactory> fdw = GetStorage()->GetDirect2DProvider()->GetDirectWriteFactory();
				CComPtr<ID2D1Factory> fd2d = GetStorage()->GetDirect2DProvider()->GetDirect2DFactory();
				if (oldRenderTarget)
				{
					Direct2DElementEventArgs arguments(element, oldRenderTarget, oldRenderTarget->GetDirect2DRenderTarget(), fdw, fd2d, CRect());
					element->BeforeRenderTargetChanged.Execute(arguments);
				}
				if (newRenderTarget)
				{
					Direct2DElementEventArgs arguments(element, newRenderTarget, newRenderTarget->GetDirect2DRenderTarget(), fdw, fd2d, CRect());
					element->AfterRenderTargetChanged.Execute(arguments);
				}
			}
#pragma endregion Direct2D
		}
	}
}