#ifndef CC_RESOURCE
#define CC_RESOURCE

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"

using namespace cc::base;
using namespace cc::base::direct2d;
using namespace cc::interfaces::windows;
using namespace cc::interfaces::direct2d;

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			namespace helper
			{
				HWND							GetHWNDFromWindow(PassRefPtr<IWindow> window);
				PassRefPtr<IForm>				GetFormFromHandle(HWND hwnd);
				PassRefPtr<IForm>				GetForm(PassRefPtr<IWindow> window);
				CComPtr<IWICImagingFactory>		GetWICImagingFactory();
				CComPtr<IWICBitmap>				GetWICBitmap(PassRefPtr<IImageFrame> frame);
				bool							IsKeyPressing(cint code);
				bool							IsKeyToggled(cint code);
			}

			namespace resource
			{
				namespace procedure
				{
					LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					LRESULT CALLBACK CtrlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
				}

				class Cursor : public ICursor
				{
					typedef ICursor::CursorType CursorType;
				public:
					Cursor(HCURSOR _handle);
					Cursor(CursorType type);

					bool								IsSystemCursor()override;
					CursorType							GetSystemCursorType()override;
					HCURSOR								GetCursorHandle();

				protected:
					HCURSOR								handle;
					bool								isSystemCursor;
					CursorType							systemCursorType;
				};

				class Screen : public IScreen
				{
				protected:
					HMONITOR							monitor;
				public:
					Screen();

					CRect								GetBounds()override;
					CRect								GetClientBounds()override;
					CString								GetName()override;
					HMONITOR							GetMonitor()override;
					void								SetMonitor(HMONITOR hMonitor);
					bool								IsPrimary()override;
				};

				class ImageFrame : public IImageFrame
				{
				public:
					ImageFrame(PassRefPtr<IImage> _image, CComPtr<IWICBitmapFrameDecode> frameDecode);
					ImageFrame(PassRefPtr<IImage> _image, CComPtr<IWICBitmap> sourceBitmap);
					~ImageFrame();

					PassRefPtr<IImage>					GetImage()override;
					CSize								GetSize()override;
					bool								SetCache(void* key, PassRefPtr<IImageFrameCache> cache)override;
					PassRefPtr<IImageFrameCache>		GetCache(void* key)override;
					PassRefPtr<IImageFrameCache>		RemoveCache(void* key)override;
					CComPtr<IWICBitmap>					GetFrameBitmap();

				protected:
					RawPtr<IImage>						image;
					CComPtr<IWICBitmap>					frameBitmap;
					map<void*, RefPtr<IImageFrameCache>>caches;
					void								Initialize(CComPtr<IWICBitmapSource> bitmapSource);
				};

				class Image : public IImage
				{
				public:
					Image(PassRefPtr<IImageService> _imageService, CComPtr<IWICBitmapDecoder> _bitmapDecoder);
					~Image();

					PassRefPtr<IImageService>			GetImageService()override;
					IImage::FormatType					GetFormat()override;
					cint								GetFrameCount()override;
					PassRefPtr<IImageFrame>				GetFrame(cint index)override;

				protected:
					RawPtr<IImageService>				imageService;
					CComPtr<IWICBitmapDecoder>			bitmapDecoder;
					vector<RefPtr<ImageFrame>>			frames;
				};

				class BitmapImage : public IImage
				{
				public:
					BitmapImage(PassRefPtr<IImageService> _imageService,
						CComPtr<IWICBitmap> sourceBitmap, FormatType _formatType);
					~BitmapImage();

					PassRefPtr<IImageService>			GetImageService()override;
					IImage::FormatType					GetFormat()override;
					cint								GetFrameCount()override;
					PassRefPtr<IImageFrame>				GetFrame(cint index)override;

				protected:
					RawPtr<IImageService>				imageService;
					RefPtr<ImageFrame>					frame;
					FormatType							formatType;
				};
			}
		}

		namespace direct2d
		{
			namespace text
			{
				class CharMeasurer;
				class TextLines;

				class CharMeasurer : public virtual Interface
				{
					static const size_t widthSize = CharClassSize;
				public:
					CharMeasurer(cint _rowHeight);
					~CharMeasurer();

					void								SetRenderTarget(PassRefPtr<IGraphicsRenderTarget> value);
					cint								MeasureWidth(TCHAR character);
					cint								GetRowHeight();

				protected:
					virtual cint						MeasureWidthInternal(TCHAR character, PassRefPtr<IGraphicsRenderTarget> renderTarget) = 0;
					virtual cint						GetRowHeightInternal(PassRefPtr<IGraphicsRenderTarget> renderTarget) = 0;

					RawPtr<IGraphicsRenderTarget>		oldRenderTarget;
					cint								rowHeight;
					cint*								widths;
				};

				class TextLines : public Object
				{
				public:
					TextLines();
					~TextLines();

					cint								GetCount();
					TextLine&							GetLine(cint row);
					PassRefPtr<CharMeasurer>			GetCharMeasurer();
					void								SetCharMeasurer(PassRefPtr<CharMeasurer> value);
					PassRefPtr<IGraphicsRenderTarget>	GetRenderTarget();
					void								SetRenderTarget(PassRefPtr<IGraphicsRenderTarget> value);
					CString								GetText(TextPos start, TextPos end);
					CString								GetText();
					void								SetText(const CString& value, cint colorIndex);

					bool								RemoveLines(cint start, cint count);
					bool								IsAvailable(TextPos pos);
					TextPos								Normalize(TextPos pos);
					TextPos								Modify(TextPos start, TextPos end, LPCTSTR* inputs, cint* inputCounts, cint rows, cint colorIndex);
					TextPos								Modify(TextPos start, TextPos end, LPCTSTR input, cint inputCount, cint colorIndex);
					TextPos								Modify(TextPos start, TextPos end, LPCTSTR input, cint colorIndex);
					TextPos								Modify(TextPos start, TextPos end, const CString& input, cint colorIndex);
					void								Clear();

					void								ClearMeasurement();
					cint								GetTabSpaceCount();
					void								SetTabSpaceCount(cint value);
					void								MeasureRow(cint row);
					cint								GetRowWidth(cint row);
					cint								GetRowHeight();
					cint								GetMaxWidth();
					cint								GetMaxHeight();
					TextPos								GetTextPosFromPoint(CPoint point);
					CPoint								GetPointFromTextPos(TextPos pos);
					CRect								GetRectFromTextPos(TextPos pos);
					TCHAR								GetPasswordChar();
					void								SetPasswordChar(TCHAR value);
					TextPos								GetEndPos();

				protected:
					vector<TextLine>					lines;
					RefPtr<CharMeasurer>				charMeasurer;
					RefPtr<IGraphicsRenderTarget>		renderTarget;
					cint								tabWidth;
					cint								tabSpaceCount;
					TCHAR								passwordChar;
				};
			}

			namespace allocator
			{
				template <class TKey, class TValue, bool GetPtr>
				class CachedResourceAllocator
				{
				public:
					static const size_t DeadPackageMax = 32;

					template<bool _GetPtr>
					struct CachedType
					{
					};

					template<>
					struct CachedType<true>
					{
						typedef RefPtr<TValue> ValueType;
						typedef PassRefPtr<TValue> ReturnType;
					};

					template<>
					struct CachedType<false>
					{
						typedef TValue ValueType;
						typedef TValue ReturnType;
					};

					typedef typename CachedType<GetPtr>::ValueType ValueType;
					typedef typename CachedType<GetPtr>::ReturnType ReturnType;

					struct Package
					{
						ValueType resource;
						cint reference;
						bool operator==(const Package& package) const { return false; }
						bool operator!=(const Package& package) const { return true; }
					};

					struct DeadPackage
					{
						TKey key;
						ValueType value;
						bool operator==(const DeadPackage& package) const { return false; }
						bool operator!=(const DeadPackage& package) const { return true; }
					};

					map<TKey, Package> aliveResources;
					vector<DeadPackage> deadResources;

				public:
					ReturnType Create(const TKey& key)
					{
						auto alive = aliveResources.find(key);
						if (alive != aliveResources.end())
						{
							alive->second.reference++;
						}
						ValueType resource;
						auto dead = find_if(deadResources.begin(), deadResources.end(), [&](DeadPackage deadPackage){ return deadPackage.key == key; });
						if (dead != deadResources.end())
						{
							resource = dead->value;
							deadResources.erase(dead);
						}
						if (!resource)
						{
							resource = CreateInternal(key);
						}
						Package package;
						package.resource = resource;
						package.reference = 1;
						aliveResources.insert(make_pair(key, package));
						return package.resource;
					}

					void Destroy(const TKey& key)
					{
						auto alive = aliveResources.find(key);
						if (alive != aliveResources.end())
						{
							auto package = alive->second;
							alive->second.reference--;
							if (alive->second.reference == 0)
							{
								aliveResources.erase(alive);
								if (deadResources.size() == DeadPackageMax)
								{
									deadResources.pop_back();
								}
								DeadPackage deadPackage;
								deadPackage.key = key;
								deadPackage.value = package.resource;
								deadResources.insert(deadResources.begin(), deadPackage);
							}
							else
							{
								aliveResources.insert(make_pair(key, package));
							}
						}
					}

				protected:
					virtual ReturnType CreateInternal(const TKey&) = 0;
				};

				template <class TKey, class TValue>
				class CachedObjectAllocatorBase : public CachedResourceAllocator<TKey, TValue, true>
				{
				};

				class CachedTextFormatAllocator : public CachedObjectAllocatorBase<Font, D2DTextFormatPackage>
				{
				public:
					static CComPtr<IDWriteTextFormat> CreateDirect2DFont(const Font& font);
				protected:
					PassRefPtr<D2DTextFormatPackage> CreateInternal(const Font& font)override;
				};

				class CachedCharMeasurerAllocator : public CachedObjectAllocatorBase<Font, CharMeasurer>
				{
				protected:
					class D2DCharMeasurer : public CharMeasurer
					{
					protected:
						CComPtr<IDWriteTextFormat>		font;
						cint							size;

						CSize MeasureInternal(wchar_t character, PassRefPtr<IGraphicsRenderTarget> renderTarget);
						cint MeasureWidthInternal(wchar_t character, PassRefPtr<IGraphicsRenderTarget> renderTarget);
						cint GetRowHeightInternal(PassRefPtr<IGraphicsRenderTarget> renderTarget);
					public:
						D2DCharMeasurer(CComPtr<IDWriteTextFormat> _font, cint _size);
					};
				protected:
					PassRefPtr<CharMeasurer> CreateInternal(const Font& value)override;
				};

				template <class TKey, class TValue>
				class CachedCOMAllocatorBase : public CachedResourceAllocator<TKey, CComPtr<TValue>, false>
				{
				public:
					CachedCOMAllocatorBase()
					{

					}
					~CachedCOMAllocatorBase()
					{

					}

					void SetRenderTarget(ID2DRenderTarget* _guiRenderTarget)
					{
						guiRenderTarget = _guiRenderTarget;
					}
				protected:
					RawPtr<ID2DRenderTarget> guiRenderTarget;
				};

				class CachedSolidBrushAllocator : public CachedCOMAllocatorBase<CColor, ID2D1SolidColorBrush>
				{
				protected:
					CComPtr<ID2D1SolidColorBrush> CreateInternal(const CColor& color);
				};

				class CachedLinearBrushAllocator : public CachedCOMAllocatorBase<pair<CColor, CColor>, ID2D1LinearGradientBrush>
				{
				protected:
					CComPtr<ID2D1LinearGradientBrush> CreateInternal(const pair<CColor, CColor>& colors);
				};
			}

			class D2DImageFrameCache;
			class D2DRenderTarget;
			class D2DLayoutProvider;

			namespace resource
			{
				using namespace cc::presentation::direct2d::text;
				using namespace cc::presentation::direct2d::allocator;

				class GraphicsResourceManager;
				class D2DResourceManager;

				class GraphicsResourceManager : public Interface
				{
					typedef map<CString, RefPtr<IGraphicsElementFactory>>		elementFactoryMap;
					typedef map<CString, RefPtr<IGraphicsRendererFactory>>		rendererFactoryMap;
				public:
					GraphicsResourceManager();
					~GraphicsResourceManager();

					virtual bool									RegisterElementFactory(PassRefPtr<IGraphicsElementFactory> factory);
					virtual bool									RegisterRendererFactory(const CString& elementTypeName, PassRefPtr<IGraphicsRendererFactory> factory);
					virtual PassRefPtr<IGraphicsElementFactory>		GetElementFactory(const CString& elementTypeName);
					virtual PassRefPtr<IGraphicsRendererFactory>	GetRendererFactory(const CString& elementTypeName);
					virtual PassRefPtr<IGraphicsRenderTarget>		GetRenderTarget(PassRefPtr<IWindow> window) = 0;
					virtual void									RecreateRenderTarget(PassRefPtr<IWindow> window) = 0;

				protected:
					elementFactoryMap								elementFactories;
					rendererFactoryMap								rendererFactories;
				};

				class D2DResourceManager : public GraphicsResourceManager, public virtual IControllerListener, public virtual ID2DResourceManager
				{
				public:
					D2DResourceManager();

					PassRefPtr<IGraphicsRenderTarget>				GetRenderTarget(PassRefPtr<IWindow> window)override;
					void											RecreateRenderTarget(PassRefPtr<IWindow> window)override;
					void											WindowCreated(PassRefPtr<IWindow> window)override;
					void											WindowDestroying(PassRefPtr<IWindow> window)override;
					virtual	PassRefPtr<D2DTextFormatPackage>		CreateDirect2DTextFormat(const Font& font)override;
					virtual void									DestroyDirect2DTextFormat(const Font& font)override;
					virtual PassRefPtr<CharMeasurer>				CreateDirect2DCharMeasurer(const Font& font)override;
					virtual void									DestroyDirect2DCharMeasurer(const Font& font)override;

				protected:
					set<RefPtr<D2DRenderTarget>>					renderTargets;

					CachedTextFormatAllocator						textFormats;
					CachedCharMeasurerAllocator						charMeasurers;
				};
			}
		}
	}
}

#endif