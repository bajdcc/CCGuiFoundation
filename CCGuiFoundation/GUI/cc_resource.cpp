#include "stdafx.h"
#include "cc_resource.h"
#include "cc_controller.h"
#include "cc_service.h"
#include "cc_debug.h"

using namespace cc::presentation::windows;
using namespace cc::presentation::windows::controller;
using namespace cc::presentation::windows::helper;
using namespace cc::presentation::windows::service;
using namespace cc::presentation::direct2d::text;
using namespace cc::debug;

namespace cc
{
	namespace presentation
	{
		namespace windows
		{
			namespace helper
			{
				HWND GetHWNDFromWindow(PassRefPtr<IWindow> window)
				{
					if (!window) return NULL;
					auto form = GetForm(window);
					if (!form) return NULL;
					return form->GetWindowHandle();
				}

				PassRefPtr<IForm> GetFormFromHandle(HWND hwnd)
				{
					RefPtr<Controller> controller = GetStorage()->GetController();
					if (controller)
					{
						return controller->GetFormFromHandle(hwnd);
					}
					return nullptr;
				}

				PassRefPtr<IForm> GetForm(PassRefPtr<IWindow> window)
				{
					return PassRefPtr<Form>(window);
				}

				CComPtr<IWICImagingFactory> GetWICImagingFactory()
				{
					RefPtr<ImageService> imageService = GetStorage()->GetController()->GetImageService();
					return imageService->GetImagingFactory();
				}

				CComPtr<IWICBitmap> GetWICBitmap(PassRefPtr<IImageFrame> frame)
				{
					RefPtr<ImageFrame> imageFrame = frame;
					return imageFrame->GetFrameBitmap();
				}

				bool IsKeyPressing(cint code)
				{
					return (GetKeyState((int)code) & 0xF0) != 0;
				}

				bool IsKeyToggled(cint code)
				{
					return (GetKeyState((int)code) & 0x0F) != 0;
				}
			}

			namespace resource
			{
				namespace procedure
				{
					LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
					{
#ifdef _DEBUG
						if (uMsg != WM_PAINT &&
							uMsg != WM_SETCURSOR &&
							uMsg != WM_NCHITTEST &&
							uMsg != WM_MOUSEMOVE &&
							uMsg != WM_NCMOUSEMOVE)
						{
							ATLTRACE(atlTraceHosting, 0, "hwnd: 0x%08x message: %-30S[0x%04x] {W:0x%08X,L:0x%08X}\n", hwnd,
								GetStorage()->GetDebug()->DebugGetMessageName(uMsg), uMsg, wParam, lParam);
						}
#endif // _DEBUG
						RefPtr<IController> controller = GetStorage()->GetController();
						if (controller)
						{
							auto * ctrl = dynamic_cast<Controller*>(controller.get());
							LRESULT result = 0;
							if (ctrl->HandleMessage(hwnd, uMsg, wParam, lParam, result))
							{
								return result;
							}
						}
						return DefWindowProc(hwnd, uMsg, wParam, lParam);
					}

					LRESULT CALLBACK CtrlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
					{
#ifdef _DEBUG
						if (uMsg != WM_TIMER)
						{
							ATLTRACE(atlTraceControls, 0, "hwnd: 0x%08x message: %-30S[0x%04x] {W:0x%08X,L:0x%08X}\n", hwnd,
								GetStorage()->GetDebug()->DebugGetMessageName(uMsg), uMsg, wParam, lParam);
						}
#endif // _DEBUG
						RefPtr<IController> controller = GetStorage()->GetController();
						if (controller)
						{
							auto callbackService = static_cast<CallbackService*>(controller->GetCallbackService().get());
							switch (uMsg)
							{
								case WM_TIMER:
									callbackService->InvokeGlobalTimer();
									break;
								case WM_CLIPBOARDUPDATE:
									callbackService->InvokeClipboardUpdated();
									break;
							}
						}
						return DefWindowProc(hwnd, uMsg, wParam, lParam);
					}

					LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
					{
						RefPtr<Controller> controller = GetStorage()->GetController();
						if (controller)
						{
							MSLLHOOKSTRUCT* mouseHookStruct = (MSLLHOOKSTRUCT*)lParam;
							CPoint location(mouseHookStruct->pt.x, mouseHookStruct->pt.y);
							controller->InvokeMouseHook(wParam, location);
						}
						return CallNextHookEx(NULL, nCode, wParam, lParam);
					}
				}

				Cursor::Cursor(HCURSOR _handle)
					: handle(_handle)
					, isSystemCursor(false)
					, systemCursorType(ICursor::Arrow)
				{
				}

				Cursor::Cursor(ICursor::CursorType type)
					: handle(NULL)
					, isSystemCursor(true)
					, systemCursorType(type)
				{
					LPWSTR id = NULL;
					switch (type)
					{
						case SmallWaiting:
							id = IDC_APPSTARTING;
							break;
						case LargeWaiting:
							id = IDC_WAIT;
							break;
						case Arrow:
							id = IDC_ARROW;
							break;
						case Cross:
							id = IDC_CROSS;
							break;
						case Hand:
							id = IDC_HAND;
							break;
						case Help:
							id = IDC_HELP;
							break;
						case IBeam:
							id = IDC_IBEAM;
							break;
						case SizeAll:
							id = IDC_SIZEALL;
							break;
						case SizeNESW:
							id = IDC_SIZENESW;
							break;
						case SizeNS:
							id = IDC_SIZENS;
							break;
						case SizeNWSE:
							id = IDC_SIZENWSE;
							break;
						case SizeWE:
							id = IDC_SIZEWE;
							break;
					}
					handle = (HCURSOR)LoadImage(NULL, id, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
				}

				bool Cursor::IsSystemCursor()
				{
					return isSystemCursor;
				}

				ICursor::CursorType Cursor::GetSystemCursorType()
				{
					return systemCursorType;
				}

				HCURSOR Cursor::GetCursorHandle()
				{
					return handle;
				}

				void ImageFrame::Initialize(CComPtr<IWICBitmapSource> bitmapSource)
				{
					CComPtr<IWICImagingFactory> factory = GetWICImagingFactory();
					CComPtr<IWICFormatConverter> converter;
					{
						IWICFormatConverter* formatConverter;
						HRESULT hr = factory->CreateFormatConverter(&formatConverter);
						if (SUCCEEDED(hr))
						{
							converter.Attach(formatConverter);
							converter->Initialize(
								bitmapSource,
								GUID_WICPixelFormat32bppPBGRA,
								WICBitmapDitherTypeNone,
								NULL,
								0.0f,
								WICBitmapPaletteTypeCustom);
						}
					}

					IWICBitmap* bitmap;
					CComPtr<IWICBitmapSource> convertedBitmapSource;
					if (converter)
					{
						convertedBitmapSource = converter;
					}
					else
					{
						convertedBitmapSource = bitmapSource;
					}
					HRESULT hr = factory->CreateBitmapFromSource(convertedBitmapSource, WICBitmapCacheOnLoad, &bitmap);
					if (SUCCEEDED(hr))
					{
						frameBitmap.Attach(bitmap);
					}
				}

				ImageFrame::ImageFrame(PassRefPtr<IImage> _image, CComPtr<IWICBitmapFrameDecode> frameDecode)
					:image(_image)
				{
					Initialize(frameDecode.p);
				}

				ImageFrame::ImageFrame(PassRefPtr<IImage> _image, CComPtr<IWICBitmap> sourceBitmap)
					: image(_image)
				{
					Initialize(sourceBitmap.p);
				}

				ImageFrame::~ImageFrame()
				{
					for (auto & cache : caches)
					{
						cache.second->OnDetach(this);
					}
				}

				PassRefPtr<IImage> ImageFrame::GetImage()
				{
					return image;
				}

				CSize ImageFrame::GetSize()
				{
					UINT width = 0;
					UINT height = 0;
					frameBitmap->GetSize(&width, &height);
					return CSize(width, height);
				}

				bool ImageFrame::SetCache(void* key, PassRefPtr<IImageFrameCache> cache)
				{
					if (!caches.insert(make_pair(key, cache.get())).second)
					{
						return false;
					}
					cache->OnAttach(this);
					return true;
				}

				PassRefPtr<IImageFrameCache> ImageFrame::GetCache(void* key)
				{
					auto found = caches.find(key);
					if (found == caches.end())
						return nullptr;
					return found->second;
				}

				PassRefPtr<IImageFrameCache> ImageFrame::RemoveCache(void* key)
				{
					auto found = caches.find(key);
					if (found == caches.end())
						return nullptr;
					RefPtr<IImageFrameCache> obj = found->second;
					found->second->OnDetach(this);
					caches.erase(found);
					return obj;
				}

				CComPtr<IWICBitmap> ImageFrame::GetFrameBitmap()
				{
					return frameBitmap;
				}

				Image::Image(PassRefPtr<IImageService> _imageService, CComPtr<IWICBitmapDecoder> _bitmapDecoder)
					: imageService(_imageService)
					, bitmapDecoder(_bitmapDecoder)
				{
					UINT count = 0;
					bitmapDecoder->GetFrameCount(&count);
					frames.resize(count);
				}

				Image::~Image()
				{
				}

				PassRefPtr<IImageService> Image::GetImageService()
				{
					return imageService;
				}

				IImage::FormatType Image::GetFormat()
				{
					GUID formatGUID;
					HRESULT hr = bitmapDecoder->GetContainerFormat(&formatGUID);
					if (SUCCEEDED(hr))
					{
						if (formatGUID == GUID_ContainerFormatBmp)
						{
							return IImage::Bmp;
						}
						else if (formatGUID == GUID_ContainerFormatPng)
						{
							return IImage::Png;
						}
						else if (formatGUID == GUID_ContainerFormatGif)
						{
							return IImage::Gif;
						}
						else if (formatGUID == GUID_ContainerFormatJpeg)
						{
							return IImage::Jpeg;
						}
						else if (formatGUID == GUID_ContainerFormatIco)
						{
							return IImage::Icon;
						}
						else if (formatGUID == GUID_ContainerFormatTiff)
						{
							return IImage::Tiff;
						}
						else if (formatGUID == GUID_ContainerFormatWmp)
						{
							return IImage::Wmp;
						}
					}
					return IImage::Unknown;
				}

				cint Image::GetFrameCount()
				{
					return frames.size();
				}

				PassRefPtr<IImageFrame> Image::GetFrame(cint index)
				{
					if (0 <= index && index < GetFrameCount())
					{
						auto frame = frames[index];
						if (!frame)
						{
							CComPtr<IWICBitmapFrameDecode> frameDecode;
							HRESULT hr = bitmapDecoder->GetFrame((int)index, &frameDecode);
							if (SUCCEEDED(hr))
							{
								frame = adoptRef(new ImageFrame(this, frameDecode));
							}
						}
						return frame;
					}
					else
					{
						return nullptr;
					}
				}

				BitmapImage::BitmapImage(PassRefPtr<IImageService> _imageService,
					CComPtr<IWICBitmap> sourceBitmap, FormatType _formatType)
					: imageService(_imageService)
					, formatType(_formatType)
				{
					frame = adoptRef(new ImageFrame(this, sourceBitmap));
				}

				BitmapImage::~BitmapImage()
				{
				}

				PassRefPtr<IImageService> BitmapImage::GetImageService()
				{
					return imageService;
				}

				IImage::FormatType BitmapImage::GetFormat()
				{
					return formatType;
				}

				cint BitmapImage::GetFrameCount()
				{
					return 1;
				}

				PassRefPtr<IImageFrame> BitmapImage::GetFrame(cint index)
				{
					return index == 0 ? frame : nullptr;
				}

				Screen::Screen()
				{
					monitor = NULL;
				}

				CRect Screen::GetBounds()
				{
					MONITORINFOEX info;
					info.cbSize = sizeof(MONITORINFOEX);
					GetMonitorInfo(monitor, &info);
					return info.rcMonitor;
				}

				CRect Screen::GetClientBounds()
				{
					MONITORINFOEX info;
					info.cbSize = sizeof(MONITORINFOEX);
					GetMonitorInfo(monitor, &info);
					return info.rcWork;
				}

				CString Screen::GetName()
				{
					MONITORINFOEX info;
					info.cbSize = sizeof(MONITORINFOEX);
					GetMonitorInfo(monitor, &info);

					TCHAR buffer[sizeof(info.szDevice) / sizeof(*info.szDevice) + 1];
					memset(buffer, 0, sizeof(buffer));
					memcpy(buffer, info.szDevice, sizeof(info.szDevice));
					return buffer;
				}

				HMONITOR Screen::GetMonitor()
				{
					return monitor;
				}

				void Screen::SetMonitor(HMONITOR hMonitor)
				{
					monitor = hMonitor;
				}

				bool Screen::IsPrimary()
				{
					MONITORINFOEX info;
					info.cbSize = sizeof(MONITORINFOEX);
					GetMonitorInfo(monitor, &info);
					return info.dwFlags == MONITORINFOF_PRIMARY;
				}
			}
		}
		namespace direct2d
		{
			namespace text
			{
				CharMeasurer::CharMeasurer(cint _rowHeight)
					: rowHeight(_rowHeight)
				{
					widths = new cint[widthSize];
					memset(widths, 0, widthSize * sizeof(cint));
				}

				CharMeasurer::~CharMeasurer()
				{
					if (widths)
						delete[] widths;
				}

				void CharMeasurer::SetRenderTarget(PassRefPtr<IGraphicsRenderTarget> value)
				{
					if (oldRenderTarget != value)
					{
						oldRenderTarget = value;
						rowHeight = GetRowHeightInternal(oldRenderTarget);
						memset(widths, 0, widthSize * sizeof(cint));
					}
				}

				cint CharMeasurer::MeasureWidth(TCHAR character)
				{
					cint w = widths[character];
					if (w == 0)
					{
						widths[character] = w = MeasureWidthInternal(character, oldRenderTarget);
					}
					return w;
				}

				cint CharMeasurer::GetRowHeight()
				{
					return rowHeight;
				}

				TextLines::TextLines()
					: tabWidth(1)
					, tabSpaceCount(4)
					, passwordChar(_T('\0'))
				{
					TextLine line;
					line.Initialize();
					lines.push_back(line);
				}

				TextLines::~TextLines()
				{
					RemoveLines(0, lines.size());
				}

				cint TextLines::GetCount()
				{
					return (cint)lines.size();
				}

				TextLine& TextLines::GetLine(cint row)
				{
					return lines[row];
				}

				PassRefPtr<CharMeasurer> TextLines::GetCharMeasurer()
				{
					return charMeasurer;
				}

				void TextLines::SetCharMeasurer(PassRefPtr<CharMeasurer> value)
				{
					charMeasurer = value;
					if (charMeasurer)
						charMeasurer->SetRenderTarget(renderTarget);
					ClearMeasurement();
				}

				PassRefPtr<IGraphicsRenderTarget> TextLines::GetRenderTarget()
				{
					return renderTarget;
				}

				void TextLines::SetRenderTarget(PassRefPtr<IGraphicsRenderTarget> value)
				{
					renderTarget = value;
					if (charMeasurer)
						charMeasurer->SetRenderTarget(renderTarget);
					ClearMeasurement();
				}

				CString TextLines::GetText(TextPos start, TextPos end)
				{
					if (!IsAvailable(start) || !IsAvailable(end) || start > end)
						return _T("");

					if (start.row == end.row)
					{
						return move(CString(lines[start.row].text + start.column, end.column - start.column));
					}

					cint count = 0;
					for (cint i = start.row + 1; i < end.row; i++)
					{
						count += lines[i].dataLength;
					}
					count += lines[start.row].dataLength - start.column;
					count += end.column;

					cint size = count + (end.row - start.row) * 2;
					TCHAR* buffer = new TCHAR[size];
					memset(buffer, 0, size*sizeof(TCHAR));
					auto writing = buffer;

					for (cint i = start.row; i <= end.row; i++)
					{
						TCHAR* text = lines[i].text;
						cint chars = 0;
						if (i == start.row)
						{
							text += start.column;
							chars = lines[i].dataLength - start.column;
						}
						else if (i == end.row)
						{
							chars = end.column;
						}
						else
						{
							chars = lines[i].dataLength;
						}

						if (i != start.row)
						{
							*writing++ = _T('\r');
							*writing++ = _T('\n');
						}
						memcpy(writing, text, chars*sizeof(TCHAR));
						writing += chars;
					}
					CString str(buffer, size);
					delete[] buffer;
					return str;
				}

				CString TextLines::GetText()
				{
					return GetText(TextPos(0, 0), TextPos(lines.size() - 1, lines[lines.size() - 1].dataLength));
				}

				void TextLines::SetText(const CString& value, cint colorIndex)
				{
					Modify(TextPos(0, 0), TextPos(lines.size() - 1, lines[lines.size() - 1].dataLength), value, colorIndex);
				}

				bool TextLines::RemoveLines(cint start, cint count)
				{
					if (start < 0 || count < 0 || start + count > (cint)lines.size())
						return false;
					for (cint i = start + count - 1; i >= start; i--)
					{
						lines[i].Finalize();
					}
					lines.erase(lines.begin() + start, lines.begin() + start + count);
					return true;

				}

				bool TextLines::IsAvailable(TextPos pos)
				{
					return 0 <= pos.row && pos.row < (cint)lines.size() && 0 <= pos.column && pos.column <= (cint)lines[pos.row].dataLength;
				}

				TextPos TextLines::Normalize(TextPos pos)
				{
					if (pos.row < 0)
					{
						return TextPos(0, 0);
					}
					else if (pos.row >= (cint)lines.size())
					{
						return TextPos(lines.size() - 1, lines[lines.size() - 1].dataLength);
					}
					else
					{
						TextLine& line = lines[pos.row];
						if (pos.column < 0)
						{
							return TextPos(pos.row, 0);
						}
						else if (pos.column > (cint)line.dataLength)
						{
							return TextPos(pos.row, line.dataLength);
						}
						else
						{
							return pos;
						}
					}
				}

				TextPos TextLines::Modify(TextPos start, TextPos end, LPCTSTR* inputs, cint* inputCounts, cint rows, cint colorIndex)
				{
					if (!IsAvailable(start) || !IsAvailable(end) || start > end || colorIndex < 0)
						return TextPos(-1, -1);

					if (rows == 1)
					{
						if (start.row == end.row)
						{
							lines[start.row].Modify(start.column, end.column - start.column, inputs[0], inputCounts[0], colorIndex);
						}
						else
						{
							if (end.row - start.row > 1)
							{
								RemoveLines(start.row + 1, end.row - start.row - 1);
							}
							cint modifyCount = lines[start.row].dataLength - start.column + end.column;
							lines[start.row].AppendAndFinalize(lines[start.row + 1]);
							lines.erase(lines.begin() + start.row + 1);
							lines[start.row].Modify(start.column, modifyCount, inputs[0], inputCounts[0], colorIndex);
						}
						return TextPos(start.row, start.column + inputCounts[0]);
					}

					if (start.row == end.row)
					{
						TextLine newLine = lines[start.row].Split(end.column);
						lines.insert(lines.begin() + start.row + 1, newLine);
						end = TextPos(start.row + 1, 0);
					}

					cint oldMiddleLines = end.row - start.row - 1;
					cint newMiddleLines = rows - 2;
					if (oldMiddleLines < newMiddleLines)
					{
						for (cint i = oldMiddleLines; i < newMiddleLines; i++)
						{
							TextLine line;
							line.Initialize();
							lines.insert(lines.begin() + end.row, line);
						}
					}
					else if (oldMiddleLines>newMiddleLines)
					{
						RemoveLines(start.row + newMiddleLines + 1, oldMiddleLines - newMiddleLines);
					}
					end.row += newMiddleLines - oldMiddleLines;

					lines[start.row].Modify(start.column, lines[start.row].dataLength - start.column, inputs[0], inputCounts[0], colorIndex);
					lines[end.row].Modify(0, end.column, inputs[rows - 1], inputCounts[rows - 1], colorIndex);
					for (cint i = 1; i < rows - 1; i++)
					{
						lines[start.row + i].Modify(0, lines[start.row + i].dataLength, inputs[i], inputCounts[i], colorIndex);
					}
					return TextPos(end.row, inputCounts[rows - 1]);
				}

				TextPos TextLines::Modify(TextPos start, TextPos end, LPCTSTR input, cint inputCount, cint colorIndex)
				{
					vector<LPCTSTR> inputs;
					vector<cint> inputCounts;
					LPCTSTR previous = input;
					LPCTSTR current = input;

					while (true)
					{
						if (current == input + inputCount)
						{
							inputs.push_back(previous);
							inputCounts.push_back(current - previous);
							break;
						}
						else if (*current == _T('\r') || *current == _T('\n'))
						{
							inputs.push_back(previous);
							inputCounts.push_back(current - previous);
							previous = current + (current[1] == _T('\n') ? 2 : 1);
							current = previous;
						}
						else
						{
							current++;
						}
					}

					return Modify(start, end, &inputs[0], &inputCounts[0], (cint)inputs.size(), colorIndex);
				}

				TextPos TextLines::Modify(TextPos start, TextPos end, LPCTSTR input, cint colorIndex)
				{
					return Modify(start, end, input, _tcslen(input), colorIndex);
				}

				TextPos TextLines::Modify(TextPos start, TextPos end, const CString& input, cint colorIndex)
				{
					return Modify(start, end, input, input.GetLength(), colorIndex);
				}

				void TextLines::Clear()
				{
					RemoveLines(0, lines.size());
					TextLine line;
					line.Initialize();
					lines.push_back(line);
				}

				void TextLines::ClearMeasurement()
				{
					for (auto & line : lines)
					{
						line.availableOffsetCount = 0;
					}
					if (charMeasurer)
					{
						tabWidth = tabSpaceCount*charMeasurer->MeasureWidth(_T(' '));
					}
					if (tabWidth == 0)
					{
						tabWidth = 1;
					}
				}

				cint TextLines::GetTabSpaceCount()
				{
					return tabSpaceCount;
				}

				void TextLines::SetTabSpaceCount(cint value)
				{
					if (value < 1)
						value = 1;
					if (tabSpaceCount != value)
					{
						tabSpaceCount = value;
						ClearMeasurement();
					}
				}

				void TextLines::MeasureRow(cint row)
				{
					TextLine& line = lines[row];
					cint offset = 0;
					if (line.availableOffsetCount)
					{
						offset = line.att[line.availableOffsetCount - 1].rightOffset;
					}
					for (cint i = line.availableOffsetCount; i < line.dataLength; i++)
					{
						CharAtt& att = line.att[i];
						TCHAR c = line.text[i];
						cint width = 0;
						if (passwordChar)
						{
							width = charMeasurer->MeasureWidth(passwordChar);
						}
						else if (c == _T('\t'))
						{
							width = tabWidth - offset % tabWidth;
						}
						else
						{
							width = charMeasurer->MeasureWidth(line.text[i]);
						}
						offset += width;
						att.rightOffset = (int)offset;
					}
					line.availableOffsetCount = line.dataLength;
				}

				cint TextLines::GetRowWidth(cint row)
				{
					if (row < 0 || row >= (cint)lines.size()) return -1;
					TextLine& line = lines[row];
					if (line.dataLength == 0)
					{
						return 0;
					}
					else
					{
						MeasureRow(row);
						return line.att[line.dataLength - 1].rightOffset;
					}
				}

				cint TextLines::GetRowHeight()
				{
					return charMeasurer->GetRowHeight();
				}

				cint TextLines::GetMaxWidth()
				{
					cint width = 0;
					for (cint i = 0; i < (cint)lines.size(); i++)
					{
						cint rowWidth = GetRowWidth(i);
						if (width < rowWidth)
						{
							width = rowWidth;
						}
					}
					return width;
				}

				cint TextLines::GetMaxHeight()
				{
					return lines.size() * charMeasurer->GetRowHeight();
				}

				TextPos TextLines::GetTextPosFromPoint(CPoint point)
				{
					cint h = charMeasurer->GetRowHeight();
					if (point.y < 0)
					{
						point.y = 0;
					}
					else if (point.y >= h * (cint)lines.size())
					{
						point.y = h * (cint)lines.size() - 1;
					}

					cint row = point.y / h;
					if (point.x < 0)
					{
						return TextPos(row, 0);
					}
					else if (point.x >= (cint)GetRowWidth(row))
					{
						return TextPos(row, lines[row].dataLength);
					}
					TextLine& line = lines[row];

					cint i1 = 0, i2 = line.dataLength;
					cint p1 = 0, p2 = line.att[line.dataLength - 1].rightOffset;
					while (i2 - i1 > 1)
					{
						cint i = (i1 + i2) / 2;
						cint p = i == 0 ? 0 : line.att[i - 1].rightOffset;
						if ((cint)point.x < p)
						{
							i2 = i;
							p2 = p;
						}
						else
						{
							i1 = i;
							p1 = p;
						}
					}
					return TextPos(row, i1);
				}

				CPoint TextLines::GetPointFromTextPos(TextPos pos)
				{
					if (IsAvailable(pos))
					{
						cint y = pos.row*charMeasurer->GetRowHeight();
						if (pos.column == 0)
						{
							return CPoint(0, y);
						}
						else
						{
							MeasureRow(pos.row);
							TextLine& line = lines[pos.row];
							return CPoint(line.att[pos.column - 1].rightOffset, y);
						}
					}
					else
					{
						return CPoint(-1, -1);
					}
				}

				CRect TextLines::GetRectFromTextPos(TextPos pos)
				{
					CPoint point = GetPointFromTextPos(pos);
					if (point == CPoint(-1, -1))
					{
						return CRect(-1, -1, -1, -1);
					}
					else
					{
						cint h = charMeasurer->GetRowHeight();
						TextLine& line = lines[pos.row];
						if (pos.column == line.dataLength)
						{
							return CRect(point, CSize(h / 2, h));
						}
						else
						{
							return CRect(point, CSize(line.att[pos.column].rightOffset - point.x, h));
						}
					}
				}

				TCHAR TextLines::GetPasswordChar()
				{
					return passwordChar;

				}

				void TextLines::SetPasswordChar(TCHAR value)
				{
					passwordChar = value;
					ClearMeasurement();
				}

				TextPos TextLines::GetEndPos()
				{
					TextPos end;
					end.row = lines.size() - 1;
					end.column = lines[end.row].dataLength;
					return end;
				}
			}

			namespace resource
			{
				GraphicsResourceManager::GraphicsResourceManager()
				{

				}

				GraphicsResourceManager::~GraphicsResourceManager()
				{

				}

				bool GraphicsResourceManager::RegisterElementFactory(PassRefPtr<IGraphicsElementFactory> factory)
				{
					return elementFactories.insert(make_pair(factory->GetElementTypeName(), factory)).second;
				}

				bool GraphicsResourceManager::RegisterRendererFactory(const CString& elementTypeName, PassRefPtr<IGraphicsRendererFactory> factory)
				{
					return rendererFactories.insert(make_pair(elementTypeName, factory)).second;
				}

				PassRefPtr<IGraphicsElementFactory> GraphicsResourceManager::GetElementFactory(const CString& elementTypeName)
				{
					auto found = elementFactories.find(elementTypeName);
					return found == elementFactories.end() ? nullptr : found->second;
				}

				PassRefPtr<IGraphicsRendererFactory> GraphicsResourceManager::GetRendererFactory(const CString& elementTypeName)
				{
					auto found = rendererFactories.find(elementTypeName);
					return found == rendererFactories.end() ? nullptr : found->second;
				}

				D2DResourceManager::D2DResourceManager()
				{

				}

				PassRefPtr<IGraphicsRenderTarget> D2DResourceManager::GetRenderTarget(PassRefPtr<IWindow> window)
				{
					return GetStorage()->GetDirect2DProvider()->GetBindedRenderTarget(window);
				}

				void D2DResourceManager::RecreateRenderTarget(PassRefPtr<IWindow> window)
				{
					RefPtr<IWindow> _window = window;
					WindowDestroying(_window);
					GetStorage()->GetDirect2DProvider()->RecreateRenderTarget(_window);
					WindowCreated(_window);
				}

				void D2DResourceManager::WindowCreated(PassRefPtr<IWindow> window)
				{
					RefPtr<IWindow> _window = window;
					RefPtr<D2DRenderTarget> renderTarget = adoptRef(new D2DRenderTarget(_window));
					renderTargets.insert(renderTarget);
					GetStorage()->GetDirect2DProvider()->SetBindedRenderTarget(_window, renderTarget);
				}

				void D2DResourceManager::WindowDestroying(PassRefPtr<IWindow> window)
				{
					RefPtr<IWindow> _window = window;
					RefPtr<D2DRenderTarget> renderTarget = GetStorage()->GetDirect2DProvider()->GetBindedRenderTarget(_window);
					GetStorage()->GetDirect2DProvider()->SetBindedRenderTarget(_window, nullptr);
					renderTargets.erase(renderTarget);
				}

				PassRefPtr<D2DTextFormatPackage> D2DResourceManager::CreateDirect2DTextFormat(const Font& font)
				{
					return textFormats.Create(font);
				}

				void D2DResourceManager::DestroyDirect2DTextFormat(const Font& font)
				{
					textFormats.Destroy(font);;
				}

				PassRefPtr<CharMeasurer> D2DResourceManager::CreateDirect2DCharMeasurer(const Font& font)
				{
					return charMeasurers.Create(font);
				}

				void D2DResourceManager::DestroyDirect2DCharMeasurer(const Font& font)
				{
					charMeasurers.Destroy(font);
				}
			}

			namespace allocator
			{
				CComPtr<IDWriteTextFormat> CachedTextFormatAllocator::CreateDirect2DFont(const Font& font)
				{
					CComPtr<IDWriteFactory> dwriteFactory = GetStorage()->GetDirect2DProvider()->GetDirectWriteFactory();
					CComPtr<IDWriteTextFormat> format;
					HRESULT hr = dwriteFactory->CreateTextFormat(
						font.fontFamily,
						NULL,
						(font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL),
						(font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL),
						DWRITE_FONT_STRETCH_NORMAL,
						(FLOAT)font.size,
						_T(""),
						&format);
					if (SUCCEEDED(hr))
					{
						format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
						return format;
					}
					else
					{
						return nullptr;
					}
				}

				PassRefPtr<D2DTextFormatPackage> CachedTextFormatAllocator::CreateInternal(const Font& font)
				{
					RefPtr<D2DTextFormatPackage> textFormat = adoptRef(new D2DTextFormatPackage);
					textFormat->textFormat = CreateDirect2DFont(font);
					textFormat->trimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
					textFormat->trimming.delimiter = 0;
					textFormat->trimming.delimiterCount = 0;

					IDWriteInlineObject* ellipseInlineObject;
					HRESULT hr = GetStorage()->
						GetDirect2DProvider()->
						GetDirectWriteFactory()->
						CreateEllipsisTrimmingSign(textFormat->textFormat, &ellipseInlineObject);
					if (SUCCEEDED(hr))
					{
						textFormat->ellipseInlineObject.Attach(ellipseInlineObject);
					}
					return textFormat;
				}

				CachedCharMeasurerAllocator::D2DCharMeasurer::D2DCharMeasurer(CComPtr<IDWriteTextFormat> _font, cint _size) : CharMeasurer(_size)
					, size(_size)
					, font(_font)
				{

				}

				CSize CachedCharMeasurerAllocator::D2DCharMeasurer::MeasureInternal(wchar_t character, PassRefPtr<IGraphicsRenderTarget> renderTarget)
				{
					CSize charSize(0, 0);
					CComPtr<IDWriteTextLayout> textLayout;
					HRESULT hr = GetStorage()->
						GetDirect2DProvider()->
						GetDirectWriteFactory()->
						CreateTextLayout(
						&character,
						1,
						font,
						0,
						0,
						&textLayout);
					if (SUCCEEDED(hr))
					{
						DWRITE_TEXT_METRICS metrics;
						hr = textLayout->GetMetrics(&metrics);
						if (SUCCEEDED(hr))
						{
							charSize = CSize((cint)ceil(metrics.widthIncludingTrailingWhitespace), (cint)ceil(metrics.height));
						}
					}
					return charSize;
				}

				cint CachedCharMeasurerAllocator::D2DCharMeasurer::MeasureWidthInternal(wchar_t character, PassRefPtr<IGraphicsRenderTarget> renderTarget)
				{
					return MeasureInternal(character, renderTarget).cx;
				}

				cint CachedCharMeasurerAllocator::D2DCharMeasurer::GetRowHeightInternal(PassRefPtr<IGraphicsRenderTarget> renderTarget)
				{
					return MeasureInternal(_T(' '), renderTarget).cy;
				}

				PassRefPtr<CharMeasurer> CachedCharMeasurerAllocator::CreateInternal(const Font& value)
				{
					return adoptRef(new D2DCharMeasurer(CachedTextFormatAllocator::CreateDirect2DFont(value), value.size));
				}

				CComPtr<ID2D1SolidColorBrush> CachedSolidBrushAllocator::CreateInternal(const CColor& color)
				{
					CComPtr<ID2D1RenderTarget> renderTarget = guiRenderTarget->GetDirect2DRenderTarget();
					CComPtr<ID2D1SolidColorBrush> brush;
					HRESULT hr = renderTarget->CreateSolidColorBrush(GetD2DColor(color), &brush);
					if (SUCCEEDED(hr))
					{
						return brush;
					}
					else
					{
						return nullptr;
					}
				}

				CComPtr<ID2D1LinearGradientBrush> CachedLinearBrushAllocator::CreateInternal(const pair<CColor, CColor>& colors)
				{
					CComPtr<ID2D1RenderTarget> renderTarget = guiRenderTarget->GetDirect2DRenderTarget();
					CComPtr<ID2D1GradientStopCollection> stopCollection;
					{
						D2D1_GRADIENT_STOP stops[2];
						stops[0].color = GetD2DColor(colors.first);
						stops[0].position = 0.0f;
						stops[1].color = GetD2DColor(colors.second);
						stops[1].position = 1.0f;

						HRESULT hr = renderTarget->CreateGradientStopCollection(
							stops,
							2,
							D2D1_GAMMA_2_2,
							D2D1_EXTEND_MODE_CLAMP,
							&stopCollection);
						if (FAILED(hr))
							return nullptr;
					}

					CComPtr<ID2D1LinearGradientBrush> brush;
					{
						D2D1_POINT_2F points[2] = { { 0, 0 }, { 0, 0 } };
						HRESULT hr = renderTarget->CreateLinearGradientBrush(
							D2D1::LinearGradientBrushProperties(points[0], points[1]),
							stopCollection,
							&brush);
						if (FAILED(hr))
							return nullptr;
					}
					return brush;
				}
			}
		}
	}
}