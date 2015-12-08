#ifndef CC_DEF
#define CC_DEF

typedef signed __int8			cint8_t;
typedef unsigned __int8			cuint8_t;
typedef signed __int16			cint16_t;
typedef unsigned __int16		cuint16_t;
typedef signed __int32			cint32_t;
typedef unsigned __int32		cuint32_t;
typedef signed __int64			cint64_t;
typedef unsigned __int64		cuint64_t;

#ifdef WTF_CPU_X86_64
typedef cint64_t				int;
typedef cint64_t				csint;
typedef cuint64_t				cuint;
#else
typedef cint32_t				cint;
typedef cint32_t				csint;
typedef cuint32_t				cuint;
#endif

// std
typedef std::basic_stringstream<TCHAR, std::char_traits<TCHAR>,
	std::allocator<TCHAR> > _tstringstream;
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>,
	std::allocator<TCHAR> > _tstring;
typedef std::basic_regex<TCHAR> _tregex;
typedef std::match_results<const TCHAR *> _tcmatch;
typedef std::match_results<_tstring::const_iterator> _tsmatch;

// char
#ifdef _UNICODE
static const size_t CharClassSize = 0x10000;
#else
static const size_t CharClassSize = 0x100;
#endif // _UNICODE


#endif