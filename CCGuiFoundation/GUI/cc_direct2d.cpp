#include "stdafx.h"
#include "cc_direct2d.h"
#include "cc_presentation.h"

namespace cc
{
	namespace presentation
	{
		namespace direct2d
		{
			D2D1::ColorF GetD2DColor(CColor color)
			{
				return D2D1::ColorF(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
			}

			D2DControllerListener::D2DControllerListener()
			{
				HRESULT hr;
				ID2D1Factory* d2d1;
				hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d1);
				if (SUCCEEDED(hr))
					D2D1Factory.Attach(d2d1);
				IDWriteFactory* dwrite;
				hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwrite));
				if (SUCCEEDED(hr))
					DWriteFactory.Attach(dwrite);
			}

			void D2DControllerListener::WindowCreated(PassRefPtr<IWindow> window)
			{
				RefPtr<IWindow> Window = window;
				RefPtr<D2DWindowListener> listener = adoptRef(new D2DWindowListener(Window, D2D1Factory));
				Window->InstallListener(listener);
				WindowListeners.insert(make_pair(Window, listener));
			}

			void D2DControllerListener::WindowDestroying(PassRefPtr<IWindow> window)
			{
				RefPtr<IWindow> Window = window;
				RefPtr<D2DWindowListener> listener = WindowListeners[Window];
				WindowListeners.erase(Window);
				Window->UninstallListener(listener);
			}			

			D2DWindowListener::D2DWindowListener(PassRefPtr<IWindow> _window, CComPtr<ID2D1Factory> _d2dFactory)
				: window(_window)
				, d2dFactory(_d2dFactory)
			{

			}

			void D2DWindowListener::RebuildCanvas(CSize size)
			{
				if (!d2dRenderTarget)
				{
					ID2D1HwndRenderTarget* renderTarget;
					D2D1_RENDER_TARGET_PROPERTIES tp = D2D1::RenderTargetProperties();
					tp.dpiX = 96;
					tp.dpiY = 96;
					size.cx = __max(size.cx, 1);
					size.cy = __max(size.cy, 1);
					HRESULT hr = d2dFactory->CreateHwndRenderTarget(
						tp,
						D2D1::HwndRenderTargetProperties(
						GetHWNDFromWindow(window),
						D2D1::SizeU((int)size.cx, (int)size.cy)
						),
						&renderTarget
						);
					if (SUCCEEDED(hr))
					{
						d2dRenderTarget.Attach(renderTarget);
						d2dRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
					}
				}
				else if (previousSize != size)
				{
					HRESULT hr = d2dRenderTarget->Resize(D2D1::SizeU((int)size.cx, (int)size.cy));
					ATLTRACE(atlTraceWindowing, 0, "%S\n", (LPCTSTR)size.ToString());
				}
				previousSize = size;
			}

			void D2DWindowListener::Moved()
			{
				RebuildCanvas(window->GetClientSize());
			}

			void D2DWindowListener::Paint()
			{

			}

			void D2DWindowListener::RecreateRenderTarget()
			{
				d2dRenderTarget = nullptr;
			}

			CComPtr<ID2D1RenderTarget> D2DWindowListener::GetDirect2DRenderTarget()
			{
				if (!d2dRenderTarget) Moved();
				return dynamic_cast<ID2D1RenderTarget*>(d2dRenderTarget.p);
			}

			void D2DProvider::RecreateRenderTarget(PassRefPtr<IWindow> window)
			{
				if (window)
				{
					GetStorage()->GetDirect2DControllerListener()->WindowListeners[window]->RecreateRenderTarget();
				}
			}

			CComPtr<ID2D1RenderTarget> D2DProvider::GetDirect2DRenderTarget(PassRefPtr<IWindow> window)
			{
				return GetStorage()->GetDirect2DRenderTarget(window);
			}

			CComPtr<ID2D1Factory> D2DProvider::GetDirect2DFactory()
			{
				return GetStorage()->GetDirect2DFactory();
			}

			CComPtr<IDWriteFactory> D2DProvider::GetDirectWriteFactory()
			{
				return GetStorage()->GetDirectWriteFactory();
			}

			PassRefPtr<ID2DRenderTarget> D2DProvider::GetBindedRenderTarget(PassRefPtr<IWindow> window)
			{
				return GetForm(window)->GetGraphicsHandler();
			}

			void D2DProvider::SetBindedRenderTarget(PassRefPtr<IWindow> window, PassRefPtr<ID2DRenderTarget> renderTarget)
			{
				GetForm(window)->SetGraphicsHandler(renderTarget);
			}

			CComPtr<IWICImagingFactory> D2DProvider::GetWICImagingFactory()
			{
				return cc::presentation::windows::helper::GetWICImagingFactory();
			}

			CComPtr<IWICBitmap> D2DProvider::GetWICBitmap(PassRefPtr<IImageFrame> frame)
			{
				return cc::presentation::windows::helper::GetWICBitmap(frame);
			}

			D2DImageFrameCache::D2DImageFrameCache(PassRefPtr<ID2DRenderTarget> _renderTarget)
				: renderTarget(_renderTarget)
			{

			}

			D2DImageFrameCache::~D2DImageFrameCache()
			{

			}

			void D2DImageFrameCache::OnAttach(PassRefPtr<IImageFrame> frame)
			{
				cachedFrame = frame;
				ID2D1Bitmap* d2dBitmap;
				HRESULT hr = renderTarget->GetDirect2DRenderTarget()->CreateBitmapFromWicBitmap(
					GetStorage()->GetDirect2DProvider()->GetWICBitmap(cachedFrame),
					&d2dBitmap
					);
				if (SUCCEEDED(hr))
				{
					bitmap.Attach(d2dBitmap);
				}
			}

			void D2DImageFrameCache::OnDetach(PassRefPtr<IImageFrame> frame)
			{
				renderTarget->DestroyBitmapCache(cachedFrame);
			}

			PassRefPtr<IImageFrame> D2DImageFrameCache::GetFrame()
			{
				return cachedFrame;
			}

			CComPtr<ID2D1Bitmap> D2DImageFrameCache::GetBitmap(bool enabled)
			{
				if (enabled)
				{
					return bitmap;
				}
				else
				{
					if (!disabledBitmap)
					{
						CComPtr<IWICBitmap> frameBitmap = GetStorage()->GetDirect2DProvider()->GetWICBitmap(cachedFrame);
						ID2D1Bitmap* d2dBitmap;
						HRESULT hr = renderTarget->GetDirect2DRenderTarget()->CreateBitmapFromWicBitmap(
							frameBitmap,
							&d2dBitmap
							);
						if (SUCCEEDED(hr))
						{
							disabledBitmap.Attach(d2dBitmap);

							WICRect rect;
							rect.X = 0;
							rect.Y = 0;
							rect.Width = bitmap->GetPixelSize().width;
							rect.Height = bitmap->GetPixelSize().height;
							BYTE* buffer = new BYTE[rect.Width * rect.Height * 4];
							hr = frameBitmap->CopyPixels(&rect, rect.Width * 4, rect.Width * rect.Height * 4, buffer);
							if (SUCCEEDED(hr))
							{
								cuint count = rect.Width * rect.Height;
								BYTE* read = buffer;
								for (cuint i = 0; i < count; i++)
								{
									BYTE g = (read[0] + read[1] + read[2]) / 6 + read[3] / 2;
									read[0] = g;
									read[1] = g;
									read[2] = g;
									read += 4;
								}
								D2D1_RECT_U d2dRect = D2D1::RectU(0, 0, rect.Width, rect.Height);
								d2dBitmap->CopyFromMemory(&d2dRect,	buffer, rect.Width * 4);
							}
							delete[] buffer;
						}
					}
					return disabledBitmap;
				}
			}

			CComPtr<IDWriteRenderingParams> D2DRenderTarget::CreateRenderingParams(DWRITE_RENDERING_MODE renderingMode, CComPtr<IDWriteRenderingParams> defaultParams, CComPtr<IDWriteFactory> dwriteFactory)
			{
				IDWriteRenderingParams* renderingParams;
				FLOAT gamma = defaultParams->GetGamma();
				FLOAT enhancedContrast = defaultParams->GetEnhancedContrast();
				FLOAT clearTypeLevel = defaultParams->GetClearTypeLevel();
				DWRITE_PIXEL_GEOMETRY pixelGeometry = defaultParams->GetPixelGeometry();
				HRESULT hr = dwriteFactory->CreateCustomRenderingParams(
					gamma,
					enhancedContrast,
					clearTypeLevel,
					pixelGeometry,
					renderingMode,
					&renderingParams);
				if (SUCCEEDED(hr))
				{
					CComPtr<IDWriteRenderingParams> RenderingParams;
					RenderingParams.Attach(renderingParams);
					return RenderingParams;
				}
				else
				{
					return nullptr;
				}
			}

			D2DRenderTarget::D2DRenderTarget(PassRefPtr<IWindow> _window)
				: window(_window)
				, clipperCoverWholeTargetCounter(0)
				, locked(false)
			{
				solidBrushes.SetRenderTarget(this);
				linearBrushes.SetRenderTarget(this);

				CComPtr<IDWriteFactory> dwriteFactory = GetStorage()->GetDirect2DProvider()->GetDirectWriteFactory();
				CComPtr<IDWriteRenderingParams> defaultParams;
				HRESULT hr = dwriteFactory->CreateRenderingParams(&defaultParams);
				if (SUCCEEDED(hr))
				{
					noAntialiasParams = CreateRenderingParams(DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL, defaultParams, dwriteFactory);
					horizontalAntialiasParams = CreateRenderingParams(DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL, defaultParams, dwriteFactory);
					bidirectionalAntialiasParams = CreateRenderingParams(DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC, defaultParams, dwriteFactory);
				}
			}

			D2DRenderTarget::~D2DRenderTarget()
			{
				for (auto & imageCache : imageCaches)
				{
					imageCache->GetFrame()->RemoveCache(this);
				}
			}

			CComPtr<ID2D1RenderTarget> D2DRenderTarget::GetDirect2DRenderTarget()
			{
				return d2dRenderTarget ?
					dynamic_cast<ID2D1RenderTarget*>(d2dRenderTarget.p) :
					GetStorage()->GetDirect2DProvider()->GetDirect2DRenderTarget(window);
			}

			CComPtr<ID2D1Bitmap> D2DRenderTarget::GetBitmap(PassRefPtr<IImageFrame> frame, bool enabled)
			{
				RefPtr<IImageFrameCache> cache = frame->GetCache(this);
				if (cache)
				{
					return dynamic_cast<D2DImageFrameCache*>(cache.get())->GetBitmap(enabled);
				}
				else
				{
					RefPtr<D2DImageFrameCache> d2dCache = adoptRef(new D2DImageFrameCache(this));
					if (frame->SetCache(this, d2dCache))
					{
						imageCaches.insert(d2dCache);
						return d2dCache->GetBitmap(enabled);
					}
					else
					{
						return nullptr;
					}
				}
			}

			void D2DRenderTarget::DestroyBitmapCache(PassRefPtr<IImageFrame> frame)
			{
				imageCaches.erase(frame->GetCache(this));
			}

			void D2DRenderTarget::SetTextAntialias(bool antialias, bool verticalAntialias)
			{
				CComPtr<IDWriteRenderingParams> params;
				if (!antialias)
				{
					params = noAntialiasParams;
				}
				else if (!verticalAntialias)
				{
					params = horizontalAntialiasParams;
				}
				else
				{
					params = bidirectionalAntialiasParams;
				}
				if (params.p != NULL && d2dRenderTarget.p != NULL)
				{
					d2dRenderTarget->SetTextRenderingParams(params);
				}
			}

			bool D2DRenderTarget::StartRendering()
			{
				d2dRenderTarget = GetStorage()->GetDirect2DProvider()->GetDirect2DRenderTarget(window);
				if (!d2dRenderTarget)
					return false;
				d2dRenderTarget->BeginDraw();
				d2dRenderTarget->Clear(GetD2DColor(Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Window)));
				return true;
			}

			HRESULT D2DRenderTarget::StopRendering()
			{
				HRESULT result = d2dRenderTarget->EndDraw();
				d2dRenderTarget = nullptr;
				return result;
			}

			void D2DRenderTarget::PushClipper(CRect clipper)
			{
				if (clipperCoverWholeTargetCounter > 0)
				{
					clipperCoverWholeTargetCounter++;
				}
				else
				{
					CRect previousClipper = GetClipper();
					CRect currentClipper;

					if (currentClipper.IntersectRect(&previousClipper, &clipper))
					{
						clippers.push_back(currentClipper);
						d2dRenderTarget->PushAxisAlignedClip(
							D2D1::RectF((FLOAT)currentClipper.left, (FLOAT)currentClipper.top, (FLOAT)currentClipper.right, (FLOAT)currentClipper.bottom),
							D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
							);
					}
					else
					{
						clipperCoverWholeTargetCounter++;
					}
				}
			}

			void D2DRenderTarget::PopClipper()
			{
				if (clipperCoverWholeTargetCounter > 0)
				{
					clipperCoverWholeTargetCounter--;
				}
				else if (!clippers.empty())
				{
					clippers.pop_back();
					d2dRenderTarget->PopAxisAlignedClip();
				}
			}

			CRect D2DRenderTarget::GetClipper()
			{
				if (clippers.empty())
				{
					return CRect(CPoint(), window->GetClientSize());
				}
				else
				{
					return clippers.back();
				}
			}

			bool D2DRenderTarget::IsClipperCoverWholeTarget()
			{
				return clipperCoverWholeTargetCounter > 0;
			}

			CComPtr<ID2D1SolidColorBrush> D2DRenderTarget::CreateDirect2DBrush(CColor color)
			{
				return solidBrushes.Create(color);
			}

			void D2DRenderTarget::DestroyDirect2DBrush(CColor color)
			{
				solidBrushes.Destroy(color);
			}

			CComPtr<ID2D1LinearGradientBrush> D2DRenderTarget::CreateDirect2DLinearBrush(CColor c1, CColor c2)
			{
				return linearBrushes.Create(pair<CColor, CColor>(c1, c2));
			}

			void D2DRenderTarget::DestroyDirect2DLinearBrush(CColor c1, CColor c2)
			{
				linearBrushes.Destroy(pair<CColor, CColor>(c1, c2));
			}
		}
	}
}