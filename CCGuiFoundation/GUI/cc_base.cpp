#include "stdafx.h"
#include "cc_base.h"

namespace cc
{
	namespace base
	{
		// CSize
		CSize::CSize()
		{
			cx = cy = 0;
		}
		CSize::CSize(int initCX, int initCY)
		{
			cx = initCX; cy = initCY;
		}
		CSize::CSize(SIZE initSize)
		{
			*(SIZE*)this = initSize;
		}
		CSize::CSize(POINT initPt)
		{
			*(POINT*)this = initPt;
		}
		CSize::CSize(DWORD dwSize)
		{
			cx = (short)LOWORD(dwSize);
			cy = (short)HIWORD(dwSize);
		}
		BOOL CSize::operator==(SIZE size) const
		{
			return (cx == size.cx && cy == size.cy);
		}
		BOOL CSize::operator!=(SIZE size) const
		{
			return (cx != size.cx || cy != size.cy);
		}
		void CSize::operator+=(SIZE size)
		{
			cx += size.cx; cy += size.cy;
		}
		void CSize::operator-=(SIZE size)
		{
			cx -= size.cx; cy -= size.cy;
		}
		CSize CSize::operator+(SIZE size) const
		{
			return CSize(cx + size.cx, cy + size.cy);
		}
		CSize CSize::operator-(SIZE size) const
		{
			return CSize(cx - size.cx, cy - size.cy);
		}
		CSize CSize::operator-() const
		{
			return CSize(-cx, -cy);
		}
		CPoint CSize::operator+(POINT point) const
		{
			return CPoint(cx + point.x, cy + point.y);
		}
		CPoint CSize::operator-(POINT point) const
		{
			return CPoint(cx - point.x, cy - point.y);
		}
		CRect CSize::operator+(const RECT* lpRect) const
		{
			return CRect(lpRect) + *this;
		}
		CRect CSize::operator-(const RECT* lpRect) const
		{
			return CRect(lpRect) - *this;
		}

		CString CSize::ToString()
		{
			CString str;
			str.Format(_T("Size(%d, %d)"), cx, cy);
			return str;
		}

		// CPoint
		CPoint::CPoint()
		{
			x = y = 0;
		}
		CPoint::CPoint(int initX, int initY)
		{
			x = initX; y = initY;
		}
#if !defined(_AFX_CORE_IMPL) || !defined(_AFXDLL) || defined(_DEBUG)
		CPoint::CPoint(POINT initPt)
		{
			*(POINT*)this = initPt;
		}
#endif
		CPoint::CPoint(SIZE initSize)
		{
			*(SIZE*)this = initSize;
		}
		CPoint::CPoint(DWORD dwPoint)
		{
			x = (short)LOWORD(dwPoint);
			y = (short)HIWORD(dwPoint);
		}
		void CPoint::Offset(int xOffset, int yOffset)
		{
			x += xOffset; y += yOffset;
		}
		void CPoint::Offset(POINT point)
		{
			x += point.x; y += point.y;
		}
		void CPoint::Offset(SIZE size)
		{
			x += size.cx; y += size.cy;
		}
		BOOL CPoint::operator==(POINT point) const
		{
			return (x == point.x && y == point.y);
		}
		BOOL CPoint::operator!=(POINT point) const
		{
			return (x != point.x || y != point.y);
		}
		void CPoint::operator+=(SIZE size)
		{
			x += size.cx; y += size.cy;
		}
		void CPoint::operator-=(SIZE size)
		{
			x -= size.cx; y -= size.cy;
		}
		void CPoint::operator+=(POINT point)
		{
			x += point.x; y += point.y;
		}
		void CPoint::operator-=(POINT point)
		{
			x -= point.x; y -= point.y;
		}
		CPoint CPoint::operator+(SIZE size) const
		{
			return CPoint(x + size.cx, y + size.cy);
		}
		CPoint CPoint::operator-(SIZE size) const
		{
			return CPoint(x - size.cx, y - size.cy);
		}
		CPoint CPoint::operator-() const
		{
			return CPoint(-x, -y);
		}
		CPoint CPoint::operator+(POINT point) const
		{
			return CPoint(x + point.x, y + point.y);
		}
		CSize CPoint::operator-(POINT point) const
		{
			return CSize(x - point.x, y - point.y);
		}
		CRect CPoint::operator+(const RECT* lpRect) const
		{
			return CRect(lpRect) + *this;
		}
		CRect CPoint::operator-(const RECT* lpRect) const
		{
			return CRect(lpRect) - *this;
		}

		CString CPoint::ToString()
		{
			CString str;
			str.Format(_T("Point(%d, %d)"), x, y);
			return str;
		}

		// CRect
		CRect::CRect()
		{
			left = top = right = bottom = 0;
		}
		CRect::CRect(int l, int t, int r, int b)
		{
			left = l; top = t; right = r; bottom = b;
		}
		CRect::CRect(const RECT& srXRect)
		{
			::CopyRect(this, &srXRect);
		}
		CRect::CRect(LPCRECT lpSrcRect)
		{
			::CopyRect(this, lpSrcRect);
		}
		CRect::CRect(POINT point, SIZE size)
		{
			right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy;
		}
		CRect::CRect(POINT topLeft, POINT bottomRight)
		{
			left = topLeft.x; top = topLeft.y;
			right = bottomRight.x; bottom = bottomRight.y;
		}
		int CRect::Width() const
		{
			return right - left;
		}
		int CRect::Height() const
		{
			return bottom - top;
		}
		CSize CRect::Size() const
		{
			return CSize(right - left, bottom - top);
		}
		CPoint& CRect::TopLeft()
		{
			return *((CPoint*)this);
		}
		CPoint& CRect::BottomRight()
		{
			return *((CPoint*)this + 1);
		}
		const CPoint& CRect::TopLeft() const
		{
			return *((CPoint*)this);
		}
		const CPoint& CRect::BottomRight() const
		{
			return *((CPoint*)this + 1);
		}
		CPoint CRect::CenterPoint() const
		{
			return CPoint((left + right) / 2, (top + bottom) / 2);
		}
		void CRect::SwapLeftRight()
		{
			SwapLeftRight(LPRECT(this));
		}
		void CRect::SwapLeftRight(LPRECT lpRect)
		{
			LONG temp = lpRect->left; lpRect->left = lpRect->right; lpRect->right = temp;
		}
		CRect::operator LPRECT()
		{
			return this;
		}
		CRect::operator LPCRECT() const
		{
			return this;
		}
		BOOL CRect::IsRectEmpty() const
		{
			return ::IsRectEmpty(this);
		}
		BOOL CRect::IsRectNull() const
		{
			return (left == 0 && right == 0 && top == 0 && bottom == 0);
		}
		BOOL CRect::PtInRect(POINT point) const
		{
			return ::PtInRect(this, point);
		}
		void CRect::SetRect(int x1, int y1, int x2, int y2)
		{
			::SetRect(this, x1, y1, x2, y2);
		}
		void CRect::SetRect(POINT topLeft, POINT bottomRight)
		{
			::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
		}
		void CRect::SetRectEmpty()
		{
			::SetRectEmpty(this);
		}
		void CRect::CopyRect(LPCRECT lpSrcRect)
		{
			::CopyRect(this, lpSrcRect);
		}
		BOOL CRect::EqualRect(LPCRECT lpRect) const
		{
			return ::EqualRect(this, lpRect);
		}
		void CRect::InflateRect(int x, int y)
		{
			::InflateRect(this, x, y);
		}
		void CRect::InflateRect(SIZE size)
		{
			::InflateRect(this, size.cx, size.cy);
		}
		void CRect::DeflateRect(int x, int y)
		{
			::InflateRect(this, -x, -y);
		}
		void CRect::DeflateRect(SIZE size)
		{
			::InflateRect(this, -size.cx, -size.cy);
		}
		void CRect::OffsetRect(int x, int y)
		{
			::OffsetRect(this, x, y);
		}
		void CRect::OffsetRect(POINT point)
		{
			::OffsetRect(this, point.x, point.y);
		}
		void CRect::OffsetRect(SIZE size)
		{
			::OffsetRect(this, size.cx, size.cy);
		}
		BOOL CRect::IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2)
		{
			return ::IntersectRect(this, lpRect1, lpRect2);
		}
		BOOL CRect::UnionRect(LPCRECT lpRect1, LPCRECT lpRect2)
		{
			return ::UnionRect(this, lpRect1, lpRect2);
		}
		void CRect::operator=(const RECT& srXRect)
		{
			::CopyRect(this, &srXRect);
		}
		BOOL CRect::operator==(const RECT& rect) const
		{
			return ::EqualRect(this, &rect);
		}
		BOOL CRect::operator!=(const RECT& rect) const
		{
			return !::EqualRect(this, &rect);
		}
		void CRect::operator+=(POINT point)
		{
			::OffsetRect(this, point.x, point.y);
		}
		void CRect::operator+=(SIZE size)
		{
			::OffsetRect(this, size.cx, size.cy);
		}
		void CRect::operator+=(LPCRECT lpRect)
		{
			InflateRect(lpRect);
		}
		void CRect::operator-=(POINT point)
		{
			::OffsetRect(this, -point.x, -point.y);
		}
		void CRect::operator-=(SIZE size)
		{
			::OffsetRect(this, -size.cx, -size.cy);
		}
		void CRect::operator-=(LPCRECT lpRect)
		{
			DeflateRect(lpRect);
		}
		void CRect::operator&=(const RECT& rect)
		{
			::IntersectRect(this, this, &rect);
		}
		void CRect::operator|=(const RECT& rect)
		{
			::UnionRect(this, this, &rect);
		}
		CRect CRect::operator+(POINT pt) const
		{
			CRect rect(*this); ::OffsetRect(&rect, pt.x, pt.y); return rect;
		}
		CRect CRect::operator-(POINT pt) const
		{
			CRect rect(*this); ::OffsetRect(&rect, -pt.x, -pt.y); return rect;
		}
		CRect CRect::operator+(SIZE size) const
		{
			CRect rect(*this); ::OffsetRect(&rect, size.cx, size.cy); return rect;
		}
		CRect CRect::operator-(SIZE size) const
		{
			CRect rect(*this); ::OffsetRect(&rect, -size.cx, -size.cy); return rect;
		}
		CRect CRect::operator+(LPCRECT lpRect) const
		{
			CRect rect(this); rect.InflateRect(lpRect); return rect;
		}
		CRect CRect::operator-(LPCRECT lpRect) const
		{
			CRect rect(this); rect.DeflateRect(lpRect); return rect;
		}
		CRect CRect::operator&(const RECT& rect2) const
		{
			CRect rect; ::IntersectRect(&rect, this, &rect2);
			return rect;
		}
		CRect CRect::operator|(const RECT& rect2) const
		{
			CRect rect; ::UnionRect(&rect, this, &rect2);
			return rect;
		}
		BOOL CRect::SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2)
		{
			return ::SubtractRect(this, lpRectSrc1, lpRectSrc2);
		}

		void CRect::InflateRect(LPCRECT lpRect)
		{
			left -= lpRect->left;
			top -= lpRect->top;
			right += lpRect->right;
			bottom += lpRect->bottom;
		}

		void CRect::DeflateRect(LPCRECT lpRect)
		{
			left += lpRect->left;
			top += lpRect->top;
			right -= lpRect->right;
			bottom -= lpRect->bottom;
		}

		void CRect::InsetRect(const RECT& srcRect)
		{
			left += srcRect.left;
			top += srcRect.top;
			right -= srcRect.right;
			bottom -= srcRect.bottom;
		}

		BOOL CRect::RestrictRect(const RECT& srcRect)
		{
			CSize offset;

			UINT bFitness = 0;

			if (left < srcRect.left) {
				offset.cx = srcRect.left - left;
			}
			else if (right > srcRect.right) {
				offset.cx = srcRect.right - right;
			}
			else {
				bFitness |= 0x000F;
			}

			if (top < srcRect.top) {
				offset.cy = srcRect.top - top;
			}
			else if (bottom > srcRect.bottom) {
				offset.cy = srcRect.bottom - bottom;
			}
			else {
				bFitness |= 0x00F0;
			}

			OffsetRect(offset);

			return bFitness == 0x00FF ? TRUE : FALSE;
		}

		CString CRect::ToString()
		{
			CString str;
			str.Format(_T("Rect(%d, %d, %d, %d)"), left, top, right, bottom);
			return str;
		}

		CColor::CColor() : r(0), g(0), b(0), a(0xFF)
		{

		}

		CColor::CColor(Gdiplus::ARGB _value) : value(_value)
		{

		}

		CColor::CColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a /*= 0xFF*/)
			: r(_r), g(_g), b(_b), a(_a)
		{

		}

		cint CColor::Compare(CColor color) const
		{
			return value - color.value;
		}

		CColor&& CColor::Parse(const CString& value)
		{
			LPCTSTR code = _T("0123456789ABCDEF");
			if ((value.GetLength() == 7 || value.GetLength() == 9) && value[0] == _T('#'))
			{
				cint index[8] = { 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF };
				for (cint i = 0; i < value.GetLength() - 1; i++)
				{
					index[i] = _tcschr(code, value[i + 1]) - code;
					if (index[i] < 0 || index[i] > 0xF)
					{
						return std::move(CColor());
					}
				}

				CColor c;
				c.r = (unsigned char)(index[0] * 16 + index[1]);
				c.g = (unsigned char)(index[2] * 16 + index[3]);
				c.b = (unsigned char)(index[4] * 16 + index[5]);
				c.a = (unsigned char)(index[6] * 16 + index[7]);
				return std::move(c);
			}
			return std::move(CColor());
		}

		bool CColor::operator==(CColor color) const
		{
			return Compare(color) == 0;
		}

		bool CColor::operator!=(CColor color) const
		{
			return Compare(color) != 0;
		}

		bool CColor::operator<(CColor color) const
		{
			return Compare(color) < 0;
		}

		bool CColor::operator<=(CColor color) const
		{
			return Compare(color) <= 0;
		}

		bool CColor::operator>(CColor color) const
		{
			return Compare(color) > 0;
		}

		bool CColor::operator>=(CColor color) const
		{
			return Compare(color) >= 0;
		}

		CString CColor::ToString() const
		{
			LPCTSTR code = _T("0123456789ABCDEF");
			TCHAR result[] = _T("#00000000");
			result[1] = code[r / 0x10];
			result[2] = code[r % 0x10];
			result[3] = code[g / 0x10];
			result[4] = code[g % 0x10];
			result[5] = code[b / 0x10];
			result[6] = code[b % 0x10];
			if (a == 0xFF)
			{
				result[7] = _T('\0');
			}
			else
			{
				result[7] = code[a / 0x10];
				result[8] = code[a % 0x10];
			}
			return result;
		}

		Font::Font() : size(0)
			, bold(false)
			, italic(false)
			, underline(false)
			, strikeline(false)
			, antialias(true)
			, verticalAntialias(true)
		{

		}

		cint Font::Compare(const Font& value) const
		{
			cint result = 0;

			result = fontFamily.Compare(value.fontFamily);
			if (result != 0) return result;

			result = size - value.size;
			if (result != 0) return result;

			result = (cint)bold - (cint)value.bold;
			if (result != 0) return result;

			result = (cint)italic - (cint)value.italic;
			if (result != 0) return result;

			result = (cint)underline - (cint)value.underline;
			if (result != 0) return result;

			result = (cint)strikeline - (cint)value.strikeline;
			if (result != 0) return result;

			result = (cint)antialias - (cint)value.antialias;
			if (result != 0) return result;

			return 0;
		}

		bool Font::operator>=(const Font& value) const
		{
			return Compare(value) >= 0;
		}

		bool Font::operator>(const Font& value) const
		{
			return Compare(value) > 0;
		}

		bool Font::operator<=(const Font& value) const
		{
			return Compare(value) <= 0;
		}

		bool Font::operator<(const Font& value) const
		{
			return Compare(value) < 0;
		}

		bool Font::operator!=(const Font& value) const
		{
			return Compare(value) != 0;
		}

		bool Font::operator==(const Font& value) const
		{
			return Compare(value) == 0;
		}

		CString Font::ToString() const
		{
			_tstringstream ss;
			ss << "family: " << (LPCTSTR)fontFamily << ", size: " << size
				<< ", B: " << std::boolalpha << bold
				<< ", U: " << std::boolalpha << underline
				<< ", I: " << std::boolalpha << italic
				<< ", S: " << std::boolalpha << strikeline
				<< ", ha: " << std::boolalpha << antialias
				<< ", va: " << std::boolalpha << verticalAntialias;
			return ss.str().c_str();
		}

		Pos::Pos() : row(0), column(0)
		{

		}

		Pos::Pos(cint _row, cint _column) : row(_row), column(_column)
		{

		}

		cint Pos::Compare(const Pos& value) const
		{
			if (row < value.row) return -1;
			if (row > value.row) return 1;
			if (column < value.column) return -1;
			if (column > value.column) return 1;
			return 0;
		}

		bool Pos::operator>=(const Pos& value) const
		{
			return Compare(value) >= 0;
		}

		bool Pos::operator>(const Pos& value) const
		{
			return Compare(value) > 0;
		}

		bool Pos::operator<=(const Pos& value) const
		{
			return Compare(value) <= 0;
		}

		bool Pos::operator<(const Pos& value) const
		{
			return Compare(value) < 0;
		}

		bool Pos::operator!=(const Pos& value) const
		{
			return Compare(value) != 0;
		}

		bool Pos::operator==(const Pos& value) const
		{
			return Compare(value) == 0;
		}

		CString Pos::ToString() const
		{
			CString str;
			str.Format(_T("Pos(%d, %d)"), row, column);
			return str;
		}

		TextRange::TextRange()
		{

		}

		TextRange::TextRange(cint _start, cint _end): start(_start), end(_end)
		{

		}

		bool TextRange::operator>=(const TextRange& range) const
		{
			return start >= range.start;
		}

		bool TextRange::operator>(const TextRange& range) const
		{
			return start > range.start;
		}

		bool TextRange::operator<=(const TextRange& range) const
		{
			return start <= range.start;
		}

		bool TextRange::operator<(const TextRange& range) const
		{
			return start < range.start;
		}

		bool TextRange::operator!=(const TextRange& range) const
		{
			return start != range.start;
		}

		bool TextRange::operator==(const TextRange& range) const
		{
			return start == range.start;
		}

		namespace direct2d
		{
			cint TextLine::BlockSize = 32;

			TextLine::TextLine()
				: text(NULL)
				, att(NULL)
				, availableOffsetCount(0)
				, bufferLength(0)
				, dataLength(0)
			{

			}

			cint TextLine::CalculateBufferLength(cint dataLength)
			{
				if (dataLength < 1)
					dataLength = 1;
				cint bufferLength = dataLength - dataLength % BlockSize;
				if (bufferLength < dataLength)
				{
					bufferLength += BlockSize;
				}
				return bufferLength;
			}

			void TextLine::Initialize()
			{
				Finalize();
				text = new TCHAR[BlockSize];
				att = new CharAtt[BlockSize];
				bufferLength = BlockSize;

				memset(text, 0, sizeof(TCHAR)*bufferLength);
				memset(att, 0, sizeof(CharAtt)*bufferLength);
			}

			void TextLine::Finalize()
			{
				if (text)
				{
					delete[] text;
					text = NULL;
				}
				if (att)
				{
					delete[] att;
					att = NULL;
				}
				availableOffsetCount = 0;
				bufferLength = 0;
				dataLength = 0;
			}

			bool TextLine::IsReady()
			{
				return text && att;
			}

			bool TextLine::Modify(cint start, cint count, LPCTSTR input, cint inputCount, cint colorIndex)
			{
				if (!text || !att || start < 0 || count < 0 || start + count > dataLength || inputCount < 0 || colorIndex < 0)
					return false;

				cint newDataLength = dataLength - count + inputCount;
				cint newBufferLength = CalculateBufferLength(newDataLength);
				if (newBufferLength != bufferLength)
				{
					TCHAR* newText = new TCHAR[newBufferLength];
					memcpy(newText, text, start*sizeof(TCHAR));
					memcpy(newText + start, input, inputCount*sizeof(TCHAR));
					memcpy(newText + start + inputCount, text + start + count, (dataLength - start - count)*sizeof(TCHAR));

					CharAtt* newAtt = new CharAtt[newBufferLength];
					memcpy(newAtt, att, start*sizeof(CharAtt));
					if (colorIndex == 0)
					{
						memset(newAtt + start, 0, inputCount*sizeof(CharAtt));
					}
					else
					{
						for (auto i = 0; i < inputCount; i++)
						{
							newAtt[start + i].rightOffset = 0;
							newAtt[start + i].colorIndex = colorIndex;
						}
					}
					memcpy(newAtt + start + inputCount, att + start + count, (dataLength - start - count)*sizeof(CharAtt));

					delete[] text;
					delete[] att;
					text = newText;
					att = newAtt;
				}
				else
				{
					memmove(text + start + inputCount, text + start + count, (dataLength - start - count)*sizeof(TCHAR));
					memmove(att + start + inputCount, att + start + count, (dataLength - start - count)*sizeof(CharAtt));
					memcpy(text + start, input, inputCount*sizeof(TCHAR));
					if (colorIndex == 0)
					{
						memset(att + start, 0, inputCount*sizeof(CharAtt));
					}
					else
					{
						for (auto i = 0; i < inputCount; i++)
						{
							att[start + i].rightOffset = 0;
							att[start + i].colorIndex = colorIndex;
						}
					}
				}
				dataLength = newDataLength;
				bufferLength = newBufferLength;
				if (availableOffsetCount > start)
				{
					availableOffsetCount = start;
				}

				return true;
			}

			TextLine TextLine::Split(cint index)
			{
				if (index<0 || index>dataLength) return TextLine();
				cint count = dataLength - index;
				TextLine line;
				line.Initialize();
				line.Modify(0, 0, text + index, count, att->colorIndex);
				memcpy(line.att, att + index, count*sizeof(CharAtt));
				Modify(index, count, _T(""), 0, att->colorIndex);
				return line;
			}

			void TextLine::AppendAndFinalize(TextLine& line)
			{
				cint oldDataLength = dataLength;
				Modify(oldDataLength, 0, line.text, line.dataLength, att->colorIndex);
				memcpy(att + oldDataLength, line.att, line.dataLength*sizeof(CharAtt));
				line.Finalize();
			}

			bool TextLine::operator!=(const TextLine& value) const
			{
				return true;
			}

			bool TextLine::operator==(const TextLine& value) const
			{
				return false;
			}

			bool ColorEntry::operator!=(const ColorEntry& value) const
			{
				return true;
			}

			bool ColorEntry::operator==(const ColorEntry& value) const
			{
				return false;
			}

			bool ColorEntryResource::operator!=(const ColorEntryResource& value)
			{
				return true;
			}

			bool ColorEntryResource::operator==(const ColorEntryResource& value)
			{
				return false;
			}
		}

		CellOption::CellOption()
			: composeType(Absolute)
			, absolute(20)
			, percentage(0)
		{

		}

		CellOption CellOption::MinSizeOption()
		{
			CellOption option;
			option.composeType = MinSize;
			return option;
		}

		CellOption CellOption::PercentageOption(double value)
		{
			CellOption option;
			option.composeType = Percentage;
			option.percentage = value;
			return option;
		}

		CellOption CellOption::AbsoluteOption(cint value)
		{
			CellOption option;
			option.composeType = Absolute;
			option.absolute = value;
			return option;
		}

		bool CellOption::operator!=(const CellOption& value)
		{
			return true;
		}

		bool CellOption::operator==(const CellOption& value)
		{
			return false;
		}

		TextEditPreviewStruct::TextEditPreviewStruct()
			: editVersion(0)
			, keyInput(false)
		{

		}

		TextEditNotifyStruct::TextEditNotifyStruct()
			: editVersion(0)
			, keyInput(false)
		{

		}

		TextCaretChangedStruct::TextCaretChangedStruct()
			: editVersion(0)
		{

		}
	}
}
