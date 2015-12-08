#ifndef CC_DIRECT2D
#define CC_DIRECT2D

#include "stdafx.h"
#include <DXGIDebug.h>
#include "cc_interface.h"
#include "cc_resource.h"

using namespace cc::interfaces::windows;
using namespace cc::presentation::windows;
using namespace cc::presentation::windows::helper;
using namespace cc::presentation::direct2d::allocator;

namespace cc
{
	namespace presentation
	{
		namespace direct2d
		{
			D2D1::ColorF GetD2DColor(CColor color);

			class D2DControllerListener;
			class D2DWindowListener;

			class D2DProvider;

			class D2DControllerListener : public IControllerListener
			{
			public:
				D2DControllerListener();

				void											WindowCreated(PassRefPtr<IWindow> window);
				void											WindowDestroying(PassRefPtr<IWindow> window);

				map<RefPtr<IWindow>, RefPtr<D2DWindowListener>>	WindowListeners;
				CComPtr<ID2D1Factory>							D2D1Factory;
				CComPtr<IDWriteFactory>							DWriteFactory;
			};

			class D2DWindowListener : public IWindowListener
			{
			public:
				D2DWindowListener(PassRefPtr<IWindow> _window, CComPtr<ID2D1Factory> _d2dFactory);

				void									Moved();
				void									Paint();
				CComPtr<ID2D1RenderTarget>				GetDirect2DRenderTarget();
				void									RecreateRenderTarget();

			protected:
				void									RebuildCanvas(CSize size);

				CComPtr<ID2D1Factory>					d2dFactory;
				CComPtr<ID2D1HwndRenderTarget>			d2dRenderTarget;
				RefPtr<IWindow>							window;
				CSize									previousSize;
			};

			class D2DProvider : public ID2DProvider
			{
			public:
				virtual void							RecreateRenderTarget(PassRefPtr<IWindow> window);
				virtual CComPtr<ID2D1RenderTarget>		GetDirect2DRenderTarget(PassRefPtr<IWindow> window);
				virtual CComPtr<ID2D1Factory>			GetDirect2DFactory();
				virtual CComPtr<IDWriteFactory>			GetDirectWriteFactory();
				virtual PassRefPtr<ID2DRenderTarget>	GetBindedRenderTarget(PassRefPtr<IWindow> window);
				virtual void							SetBindedRenderTarget(PassRefPtr<IWindow> window, PassRefPtr<ID2DRenderTarget> renderTarget);
				virtual CComPtr<IWICImagingFactory>		GetWICImagingFactory();
				virtual CComPtr<IWICBitmap>				GetWICBitmap(PassRefPtr<IImageFrame> frame);
			};

			class D2DImageFrameCache : public IImageFrameCache
			{
			public:
				D2DImageFrameCache(PassRefPtr<ID2DRenderTarget> _renderTarget);
				~D2DImageFrameCache();

				void								OnAttach(PassRefPtr<IImageFrame> frame)override;
				void								OnDetach(PassRefPtr<IImageFrame> frame)override;
				PassRefPtr<IImageFrame>				GetFrame();
				CComPtr<ID2D1Bitmap>				GetBitmap(bool enabled);
		
			protected:
				RefPtr<ID2DRenderTarget>			renderTarget;
				RawPtr<IImageFrame>					cachedFrame;
				CComPtr<ID2D1Bitmap>				bitmap;
				CComPtr<ID2D1Bitmap>				disabledBitmap;
			};

			class D2DRenderTarget : public ID2DRenderTarget
			{
			public:
				D2DRenderTarget(PassRefPtr<IWindow> _window);
				~D2DRenderTarget();

				CComPtr<ID2D1RenderTarget>			GetDirect2DRenderTarget()override;
				CComPtr<ID2D1Bitmap>				GetBitmap(PassRefPtr<IImageFrame> frame, bool enabled)override;
				void								DestroyBitmapCache(PassRefPtr<IImageFrame> frame)override;
				void								SetTextAntialias(bool antialias, bool verticalAntialias)override;
				bool								StartRendering()override;
				HRESULT								StopRendering()override;
				void								PushClipper(CRect clipper)override;
				void								PopClipper()override;
				CRect								GetClipper()override;
				bool								IsClipperCoverWholeTarget()override;
				CComPtr<ID2D1SolidColorBrush>		CreateDirect2DBrush(CColor color)override;
				void								DestroyDirect2DBrush(CColor color)override;
				CComPtr<ID2D1LinearGradientBrush>	CreateDirect2DLinearBrush(CColor c1, CColor c2)override;
				void								DestroyDirect2DLinearBrush(CColor c1, CColor c2)override;

			protected:
				CComPtr<IDWriteRenderingParams>		CreateRenderingParams(DWRITE_RENDERING_MODE renderingMode, CComPtr<IDWriteRenderingParams> defaultParams, CComPtr<IDWriteFactory> dwriteFactory);

				RawPtr<IWindow>						window;
				CComPtr<ID2D1HwndRenderTarget>		d2dRenderTarget;
				vector<CRect>						clippers;
				cuint								clipperCoverWholeTargetCounter;
				bool								locked;

				CachedSolidBrushAllocator			solidBrushes;
				CachedLinearBrushAllocator			linearBrushes;
				set<RefPtr<D2DImageFrameCache>>		imageCaches;

				CComPtr<IDWriteRenderingParams>		noAntialiasParams;
				CComPtr<IDWriteRenderingParams>		horizontalAntialiasParams;
				CComPtr<IDWriteRenderingParams>		bidirectionalAntialiasParams;
			};
		}
	}
}

#endif