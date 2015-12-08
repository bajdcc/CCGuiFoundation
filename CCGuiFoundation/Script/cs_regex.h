#ifndef CS_REGEX
#define CS_REGEX

#include "stdafx.h"
#include "../GUI/cc_base.h"
#include "cs_inst.h"

using namespace std;
using namespace cc::base;

/*******************************************************************************
正则表达式语法：
	语法说明：
	  1：字符集
		[A1[-A2]A3[-A4]]，					例如[a-zA-Z_]，本例代表字母和下划线
		[^A1[-A2]A3[-A4]]，					例如[^a-zA-Z_]，本例代表除字母和下划线外的字符
		A1,\A1，							例如a代表字符a，\d代表[0-9]
		
	  2：转义字符：
		\^									^
		\$									$

		\{									{
		\}									}
		\[									[
		\]									]
		\(									(
		\)									)
		\?									?
		\+									+
		\-									-
		\*									*
		\|									|
		\\									\
		\<									<
		\>									>

		\n									#10
		\r									#13
		\t									Tab

		\d									[0-9]			【不可用在[]内】
		\D									[^0-9]			【不可用在[]内】
		\s									[ \n\r\t]		【不可用在[]内】
		\S									[^ \n\r\t]		【不可用在[]内】
		\w									[0-9a-zA-Z_]	【不可用在[]内】
		\W									[^0-9a-zA-Z_]	【不可用在[]内】

		\hHH, \oOOO, \uUUUU					字符

		^									字符串开始		【不可用在[]内】
		$									字符串结尾		【不可用在[]内】
	  3：循环
		{m}									循环m次
		{m,}								循环m至无穷多次
		{m,n}								循环m至n次
		*									{0,}
		+									{1,}
		?									{0,1}
								【扩展语法】在循环标志后加"?"代表非贪婪循环
											譬如"ABCDE"匹配\w+\w+得到"ABCD","E"，但是匹配\w+?\w+则得到"A","BCDE"
	  4：功能
	    a|b									匹配a或者b的其中一个
		(?Pattern)				【扩展语法】匹配后获取
		(?:Pattern)							匹配后不获取
		(?=Pattern)				【扩展语法】正向预查，Pattern不能包含扩展语法
		(?!Pattern)				【扩展语法】负向预查，Pattern不能包含扩展语法
		其中Pattern可以是
		  表达式							普通表达式
		  <name>表达式						为表达式命名
		  <name>							如果name曾经被用来命名，则取命名的表达式，【不可递归】
		  <#name>表达式			【扩展语法】匹配后存放进name号存储区
		  <$name>				【扩展语法】匹配字符串是否等于name号存储区的某个记录
*******************************************************************************/

namespace cc
{
	namespace script
	{
		namespace regex
		{
			namespace constants
			{
				/************************************************************************/
				/* 枚举                                                                 */
				/************************************************************************/

				template <typename _Enum>
				struct Cs_EnumToString
				{
					static const TCHAR* _To_string()
					{
						return NULL;
					}
				};

				enum Cse_RegexCharset
				{
					csrec_charset,						/*匹配字符集*/
					csrec_string_begin,					/*匹配字符串起始位置*/
					csrec_string_end					/*匹配字符串终结位置*/
				};

				template <>
				struct Cs_EnumToString<Cse_RegexCharset>
				{
					static const TCHAR* _To_string(Cse_RegexCharset _Enum)
					{
						switch (_Enum)
						{
							case csrec_charset:
								return _T("字符集");
							case csrec_string_begin:
								return _T("字符串起始位置");
							case csrec_string_end:
								return _T("字符串结束位置");
							default:
								return NULL;
						}
					}
				};

				enum Cse_RegexFunc
				{
					csref_catch,						/*获取*/
					csref_uncatch,						/*不获取*/
					csref_positive,						/*正向预查*/
					csref_negative						/*反向预查*/
				};

				template <>
				struct Cs_EnumToString<Cse_RegexFunc>
				{
					static const TCHAR* _To_string(Cse_RegexFunc _Enum)
					{
						switch (_Enum)
						{
							case csref_catch:
								return _T("捕获");
							case csref_uncatch:
								return _T("不捕获");
							case csref_positive:
								return _T("正向预查");
							case csref_negative:
								return _T("反向预查");
							default:
								return NULL;
						}
					}
				};

				enum Cse_RegexContent
				{
					csrec_named,						/*命名表达式*/
					csrec_storage,						/*指定管道存储*/
					csrec_match,						/*指定管道匹配*/
					csrec_normal						/*普通*/
				};

				template <>
				struct Cs_EnumToString<Cse_RegexContent>
				{
					static const TCHAR* _To_string(Cse_RegexContent _Enum)
					{
						switch (_Enum)
						{
							case csrec_named:
								return _T("命名管道");
							case csrec_storage:
								return _T("管道存储");
							case csrec_match:
								return _T("管道匹配");
							case csrec_normal:
								return _T("普通");
							default:
								return NULL;
						}
					}
				};

				enum Cse_RegexSection
				{
					csres_sequence,						/*序列*/
					csres_branch						/*分支*/
				};

				template <>
				struct Cs_EnumToString<Cse_RegexSection>
				{
					static const TCHAR* _To_string(Cse_RegexSection _Enum)
					{
						switch (_Enum)
						{
							case csres_sequence:
								return _T("序列");
							case csres_branch:
								return _T("分支");
							default:
								return NULL;
						}
					}
				};

				enum Cse_RegexError
				{	// identify error
					cserr_none,
					cserr_null,
					cserr_ctype,
					cserr_escape,
					cserr_brack,
					cserr_paren,
					cserr_brace,
					cserr_badbrace,
					cserr_range,
					cserr_space,
					cserr_badrepeat,
					cserr_syntax,
					cserr_incomplete,
					cserr_unsupported,
					cserr_named_null,
					cserr_named_recursive_call,
					cserr_named_redeclared,
					cserr_named_undeclared,
				};

				template <>
				struct Cs_EnumToString<Cse_RegexError>
				{
					static const TCHAR* _To_string(Cse_RegexError _Enum)
					{
						switch (_Enum)
						{
							case cserr_ctype:
								return _T("非法字符");
							case cserr_null:
								return _T("正则表达式为空");
							case cserr_escape:
								return _T("非法的转义字符");
							case cserr_brack:
								return _T("中括号不匹配");
							case cserr_paren:
								return _T("小括号不匹配");
							case cserr_brace:
								return _T("大括号不匹配");
							case cserr_badbrace:
								return _T("数字范围不正确");
							case cserr_range:
								return _T("字符范围不正确");
							case cserr_space:
								return _T("内存不足");
							case cserr_badrepeat:
								return _T("谓词使用错误");
							case cserr_syntax:
								return _T("语法错误");
							case cserr_incomplete:
								return _T("正则表达式不完整");
							case cserr_unsupported:
								return _T("非扩展模式不支持此功能");
							case cserr_named_null:
								return _T("命名管道为空");
							case cserr_named_recursive_call:
								return _T("命名管道递归引用");
							case cserr_named_redeclared:
								return _T("命名管道重复声明");
							case cserr_named_undeclared:
								return _T("命名管道未定义");
							default:
								return NULL;
						}
					}
				};
			}

			/************************************************************************/
			/* 访问器                                                               */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexCharset;
			template <typename _Elem>
			class Cs_RegexLoop;
			template <typename _Elem>
			class Cs_RegexFunc;
			template <typename _Elem>
			class Cs_RegexSection;

			namespace constants
			{
				template <typename _Class>
				struct Cs_ClassToString
				{
					static const TCHAR* _To_string()
					{
						return NULL;
					}
				};

				template <typename _Elem>
				struct Cs_ClassToString<Cs_RegexCharset<_Elem>>
				{
					static const TCHAR* _To_string()
					{
						return _T("字符集");
					}

					static const TCHAR* _Reverse(bool _Rev)
					{
						return _Rev ? _T("取反") : _T("普通");
					}
				};

				template <typename _Elem>
				struct Cs_ClassToString<Cs_RegexLoop<_Elem>>
				{
					static const TCHAR* _To_string()
					{
						return _T("循环");
					}

					static const TCHAR* _Lazy(bool _La)
					{
						return _La ? _T("非贪婪") : _T("贪婪");
					}

					static _tstring _Infinite(bool _Inf, cint _Min, cint _Max)
					{
						_tstringstream _S;
						_S << _Min << (TCHAR)meta_comma;
						if (_Inf)
							_S << _T("无穷大");
						else
							_S << _Max;
						return _S.str();
					}
				};

				template <typename _Elem>
				struct Cs_ClassToString<Cs_RegexFunc<_Elem>>
				{
					static const TCHAR* _To_string()
					{
						return _T("功能");
					}
				};


				template <typename _Elem>
				struct Cs_ClassToString<Cs_RegexSection<_Elem>>
				{
					static const TCHAR* _To_string()
					{
						return _T("部分");
					}
				};
			}

			/************************************************************************/
			/* 关于正则表达式语法树的递归算法接口                                   */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexIRecursiveAlgorithm;

			/************************************************************************/
			/* 语法树                                                               */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexTraitsBase;
			template <typename _Elem>
			class Cs_RegexTraits;
			template <typename _Elem, typename _Traits = Cs_RegexTraits<_Elem>>
			class Cs_RegexBase;

			template <typename _Elem>
			class Cs_RegexCharClass;

			template <typename _Elem>
			class Cs_RegexTraitsBase
			{
			};

			template <>
			class Cs_RegexTraitsBase<char>
			{
			public:
				static const size_t char_size = 0x100;
			};

			template <>
			class Cs_RegexTraitsBase<wchar_t>
			{
			public:
				static const size_t char_size = 0x10000;
			};

			enum Meta_type
			{	// meta character representations for parser
				meta_space = _T(' '),
				meta_lpar = _T('('),
				meta_rpar = _T(')'),
				meta_dlr = _T('$'),
				meta_caret = _T('^'),
				meta_dot = _T('.'),
				meta_star = _T('*'),
				meta_plus = _T('+'),
				meta_query = _T('?'),
				meta_lsq = _T('['),
				meta_rsq = _T(']'),
				meta_sharp = _T('#'),
				meta_lt = _T('<'),
				meta_gt = _T('>'),
				meta_bar = _T('|'),
				meta_esc = _T('\\'),
				meta_dash = _T('-'),
				meta_lbr = _T('{'),
				meta_rbr = _T('}'),
				meta_comma = _T(','),
				meta_colon = _T(':'),
				meta_equal = _T('='),
				meta_exc = _T('!'),
				meta_quote = _T('\''),
				meta_eos = -1,
				meta_nl = _T('\n'),
				meta_cr = _T('\r'),
				meta_bsp = _T('\b'),
				meta_tab = _T('\t'),
				meta_chr = 0,

				esc_bsl = _T('\\'),
				esc_word = _T('w'),
				esc_not_word = _T('W'),
				esc_ctrl_a = _T('a'),
				esc_ctrl_b = _T('b'),
				esc_ctrl_f = _T('f'),
				esc_ctrl_n = _T('n'),
				esc_ctrl_r = _T('r'),
				esc_ctrl_t = _T('t'),
				esc_ctrl_v = _T('v'),
				esc_ctrl = _T('c'),
				esc_hex = _T('x'),
				esc_uni = _T('u'),
				esc_oct = _T('o')
			};

			static const char Meta_map[] = {	// array of meta chars
				meta_lpar, meta_rpar, meta_dlr, meta_caret,
				meta_dot, meta_star, meta_plus, meta_query,
				meta_lsq, meta_rsq, meta_bar, meta_esc,
				meta_dash, meta_lbr, meta_rbr, meta_comma,
				meta_colon, meta_equal, meta_exc, meta_nl,
				meta_cr, meta_bsp, meta_lt, meta_gt,
				meta_sharp,
				0 };

			template <typename _Elem>
			class Cs_RegexTraits : public Cs_RegexTraitsBase<_Elem>
			{
			public:
				using Cs_RegexCharRange = std::tuple<_Elem, _Elem>;
				using Cs_RegexCharSet = std::set<Cs_RegexCharRange>;
				using Cs_RegexString = std::basic_string<_Elem, std::char_traits<_Elem>, std::allocator<_Elem>>;
				using Cs_RegexCString = ATL::CStringT<_Elem, ATL::StrTraitATL<_Elem, ATL::ChTraitsCRT<_Elem>>>;
				using Cs_RegexPtr = WTF::RefPtr<Cs_RegexBase<_Elem>>;
				using Cs_RegexPassPtr = WTF::PassRefPtr<Cs_RegexBase<_Elem>>;
				using Cs_RegexList = std::vector<Cs_RegexPtr>;
				using Cs_RegexStringList = std::vector<Cs_RegexString>;
				using Cs_RegexStringSet = std::set<Cs_RegexString>;
				using Cs_RegexCharEsc = Cs_RegexStringSet;
				using Cs_RegexBitSet = std::bitset<CharClassSize>;

				typedef Cs_RegexTraitsBase<_Elem> Cs_RegexTraitsX;

				typedef constants::Cse_RegexCharset Cse_RegexCharset;
				typedef constants::Cse_RegexFunc Cse_RegexFunc;
				typedef constants::Cse_RegexContent Cse_RegexContent;
				typedef constants::Cse_RegexSection Cse_RegexSection;
				typedef constants::Cse_RegexError Cse_RegexError;

				using _Std_Regex_traits = _Regex_traits<_Elem>;

				using _Cs_RegexBase = Cs_RegexBase<_Elem>;
				using _Cs_RegexCharset = Cs_RegexCharset<_Elem>;
				using _Cs_RegexLoop = Cs_RegexLoop<_Elem>;
				using _Cs_RegexFunc = Cs_RegexFunc<_Elem>;
				using _Cs_RegexSection = Cs_RegexSection<_Elem>;
				using _Cs_RegexCharClass = Cs_RegexCharClass<_Elem>;
				using _Cs_RegexIRecursiveAlgorithm = Cs_RegexIRecursiveAlgorithm<_Elem>;

				using Csex_RegexCharset = constants::Cs_EnumToString<Cse_RegexCharset>;
				using Csex_RegexFunc = constants::Cs_EnumToString<Cse_RegexFunc>;
				using Csex_RegexContent = constants::Cs_EnumToString<Cse_RegexContent>;
				using Csex_RegexSection = constants::Cs_EnumToString<Cse_RegexSection>;
				using Csex_RegexError = constants::Cs_EnumToString<Cse_RegexError>;

				using Csx_RegexCharset = constants::Cs_ClassToString<_Cs_RegexCharset>;
				using Csx_RegexLoop = constants::Cs_ClassToString<_Cs_RegexLoop>;
				using Csx_RegexFunc = constants::Cs_ClassToString<_Cs_RegexFunc>;
				using Csx_RegexSection = constants::Cs_ClassToString<_Cs_RegexSection>;

				using Cs_RegexStringListMap = std::multimap<Cs_RegexString, Cs_RegexString>;
				using Cs_RegexNamedRefMap = std::multimap<Cs_RegexString, _Cs_RegexFunc*>;
				using Cs_RegexExpressionMap = std::map<Cs_RegexString, RefPtr<_Cs_RegexBase>>;

				/* 字符串转换 */
				static _tstring _Conv(const Cs_RegexString& _Regex_str);
				static Cs_RegexString _Convi(const _tstring& _Regex_str);
				/* 出错 */
				static void _Throw(constants::Cse_RegexError _Err, cint _Loc = -1);
				static void _Throw(const Cs_RegexString& _Regex_str, constants::Cse_RegexError _Err, cint _Loc = -1);

				/* 把字符区间转换为字符串 */
				_tstring _Char_range(const Cs_RegexCharRange& _Ra);
				/* 把字符集转换为字符串 */
				_tstring _Char_set(const Cs_RegexCharSet& _Cs);
				/* 打印转义字符集合 */
				_tstring _Char_esc(const Cs_RegexCharEsc& _Cs, const _Elem& _Ch);
				/* 字符输出 */
				_tstring _Char_out(const _Elem& _Ch);
			};

			template <typename _Elem>
			_tstring Cs_RegexTraits<_Elem>::_Char_range(const Cs_RegexCharRange& _Ra)
			{
				_tstringstream _S;
				_Elem _Lower = get<0>(_Ra), _Upper = get<1>(_Ra);
				if (_Lower == _Upper)
				{
					_S << (TCHAR)meta_lsq << _Char_out(_Lower) << (TCHAR)meta_comma << cint(_Lower) << (TCHAR)meta_rsq;
				}
				else
				{
					_S << (TCHAR)meta_lsq << _Char_out(_Lower) << (TCHAR)meta_comma << cint(_Lower) << (TCHAR)meta_rsq
						<< (TCHAR)meta_dash
						<< (TCHAR)meta_lsq << _Char_out(_Upper) << (TCHAR)meta_comma << cint(_Upper) << (TCHAR)meta_rsq;
				}
				return _S.str();
			}

			template <typename _Elem>
			_tstring Cs_RegexTraits<_Elem>::_Char_set(const Cs_RegexCharSet& _Cs)
			{
				_tstringstream _S;
				auto comma = false;
				for (auto & _Ch_set : _Cs)
				{
					if (comma)
						_S << (TCHAR)meta_comma;
					_S << _Char_range(_Ch_set);
					if (!comma)
						comma = true;
				}
				return _S.str();
			}

			template <typename _Elem>
			_tstring Cs_RegexTraits<_Elem>::_Char_esc(const Cs_RegexCharEsc& _Ss, const _Elem& _Ch)
			{
				_tstringstream _S;
				auto comma = false;
				for (auto & _Sle : _Ss)
				{
					if (comma)
						_S << (TCHAR)meta_comma;
					_S << _Ch << _Sle;
					if (!comma)
						comma = true;
				}
				return _S.str();
			}

			template <typename _Elem>
			_tstring Cs_RegexTraits<_Elem>::_Char_out(const _Elem& _Ch)
			{
				_tstringstream _S;
				if (_istprint(_Ch))
				{
					_S << (TCHAR)meta_quote << _Ch << (TCHAR)meta_quote;
				}
				else
				{
					_S << (TCHAR)meta_esc << (TCHAR)esc_hex << hex	<< (cint)_Ch;
				}
				return _S.str();
			}

			template <typename _Elem>
			_tstring Cs_RegexTraits<_Elem>::_Conv(const Cs_RegexString& _Regex_str)
			{
				Cs_RegexCString _Regex_cstr(_Regex_str.c_str());
				CString _Cstr(_Regex_cstr);
				return _Cstr.GetBuffer();
			}

			template <typename _Elem>
			typename Cs_RegexTraits<_Elem>::Cs_RegexString
				Cs_RegexTraits<_Elem>::_Convi(const _tstring& _Regex_str)
			{
				CString _Regex_cstr(_Regex_str.c_str());
				Cs_RegexCString _Cstr(_Regex_cstr);
				return _Cstr.GetBuffer();
			}

			template <typename _Elem>
			void Cs_RegexTraits<_Elem>::_Throw(constants::Cse_RegexError _Err, cint _Loc /*= -1*/)
			{
				_RAISE(Cs_RegexError(_T(""), _Err, _Loc));
			}

			template <typename _Elem>
			void Cs_RegexTraits<_Elem>::_Throw(const Cs_RegexString& _Regex_str, constants::Cse_RegexError _Err, cint _Loc /*= -1*/)
			{
				_RAISE(Cs_RegexError(_Conv(_Regex_str), _Err, _Loc));
			}

			template <typename _Elem, typename _Traits /*= Cs_RegexTraits<_Elem>*/>
			class Cs_RegexBase : public Cs_RegexTraits<_Elem>, public Cs_Interface
			{
			public:
				virtual void		_Visit(_Cs_RegexIRecursiveAlgorithm* _Al) = 0;
			};

			template <typename _Elem>
			class Cs_RegexIRecursiveAlgorithm : public Cs_RegexTraits<_Elem>
			{
			public:
				virtual void		_Visit_begin(_Cs_RegexCharset*	_Node, bool& _Visit_children, bool& _Run_end) = 0;
				virtual void		_Visit_begin(_Cs_RegexLoop*		_Node, bool& _Visit_children, bool& _Run_end) = 0;
				virtual void		_Visit_begin(_Cs_RegexFunc*		_Node, bool& _Visit_children, bool& _Run_end) = 0;
				virtual void		_Visit_begin(_Cs_RegexSection*	_Node, bool& _Visit_children, bool& _Run_end) = 0;
				virtual void		_Visit_end(_Cs_RegexCharset*	_Node) = 0;
				virtual void		_Visit_end(_Cs_RegexLoop*		_Node) = 0;
				virtual void		_Visit_end(_Cs_RegexFunc*		_Node) = 0;
				virtual void		_Visit_end(_Cs_RegexSection*	_Node) = 0;
			};

			/************************************************************************/
			/* 字符集                                                               */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexCharset : public Cs_RegexBase<_Elem>
			{
			public:
				Cs_RegexCharset();

				void				_Visit(_Cs_RegexIRecursiveAlgorithm* _Al);

				Cs_RegexCharSet		_Chr_set;				/*字符集*/
				bool				_Neg;					/*是否取余集*/
				Cse_RegexCharset	_Type;					/*类型*/
				ctype_base::mask	_Mask;					/*掩码*/
				Cs_RegexCharEsc		_Esc;					/*转义字符集合*/
			};

			template <typename _Elem>
			Cs_RegexCharset<_Elem>::Cs_RegexCharset()
				: _Type(constants::csrec_charset)
				, _Neg(false)
				, _Mask(0)
			{

			}

			template <typename _Elem>
			void Cs_RegexCharset<_Elem>::_Visit(_Cs_RegexIRecursiveAlgorithm* _Al)
			{
				bool _Visit_children = true;
				bool _Run_end = true;
				_Al->_Visit_begin(this, _Visit_children, _Run_end);
				if (_Run_end)
				{
					_Al->_Visit_end(this);
				}
			}

			/************************************************************************/
			/* 循环                                                                 */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexLoop : public Cs_RegexBase<_Elem>
			{
			public:
				Cs_RegexLoop();

				void				_Visit(_Cs_RegexIRecursiveAlgorithm* _Al);

				Cs_RegexPtr			_Exp;					/*循环体*/
				cint				_Min;					/*最少循环次数*/
				cint				_Max;					/*最多循环次数*/
				bool				_Rep;					/*是否无限循环*/
				bool				_Greedy;				/*是否忽略优先量词*/
			};

			template <typename _Elem>
			Cs_RegexLoop<_Elem>::Cs_RegexLoop()
				: _Min(0)
				, _Max(0)
				, _Rep(true)
				, _Greedy(true)
			{

			}

			template <typename _Elem>
			void Cs_RegexLoop<_Elem>::_Visit(_Cs_RegexIRecursiveAlgorithm* _Al)
			{
				bool _Visit_children = true;
				bool _Run_end = true;
				_Al->_Visit_begin(this, _Visit_children, _Run_end);
				if (_Visit_children && _Exp)
				{
					_Exp->_Visit(_Al);
				}
				if (_Run_end)
				{
					_Al->_Visit_end(this);
				}
			}

			/************************************************************************/
			/* 功能                                                                 */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexFunc : public Cs_RegexBase<_Elem>
			{
			public:
				Cs_RegexFunc();

				void				_Visit(_Cs_RegexIRecursiveAlgorithm* _Al);

				Cs_RegexPtr			_Exp;					/*子表达式*/
				Cs_RegexBase*		_Ref_source;			/*被引用目标*/
				Cse_RegexFunc		_Func;					/*附加功能*/
				Cse_RegexContent	_Cont;					/*附加属性*/
				Cs_RegexString		_Pat;					/*模式名称*/
			};

			template <typename _Elem>
			Cs_RegexFunc<_Elem>::Cs_RegexFunc()
				: _Func(constants::csref_uncatch)
				, _Cont(constants::csrec_normal)
				, _Ref_source(nullptr)
			{

			}

			template <typename _Elem>
			void Cs_RegexFunc<_Elem>::_Visit(_Cs_RegexIRecursiveAlgorithm* _Al)
			{
				bool _Visit_children = true;
				bool _Run_end = true;
				_Al->_Visit_begin(this, _Visit_children, _Run_end);
				if (_Visit_children && _Exp)
				{
					_Exp->_Visit(_Al);
				}
				if (_Run_end)
				{
					_Al->_Visit_end(this);
				}
			}

			/************************************************************************/
			/* 分支与序列                                                           */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexSection : public Cs_RegexBase<_Elem>
			{
			public:
				Cs_RegexSection();
				Cs_RegexSection(Cse_RegexSection);

				void				_Visit(_Cs_RegexIRecursiveAlgorithm* _Al);

				Cse_RegexSection	_Type;					/*子表达式组织方式*/
				Cs_RegexList		_Sections;				/*子表达式*/
			};

			template <typename _Elem>
			Cs_RegexSection<_Elem>::Cs_RegexSection()
				: _Type(constants::csres_sequence)
			{

			}

			template <typename _Elem>
			Cs_RegexSection<_Elem>::Cs_RegexSection(Cse_RegexSection _Ty)
				: _Type(_Ty)
			{

			}

			template <typename _Elem>
			void Cs_RegexSection<_Elem>::_Visit(_Cs_RegexIRecursiveAlgorithm* _Al)
			{
				bool _Visit_children = true;
				bool _Run_end = true;
				_Al->_Visit_begin(this, _Visit_children, _Run_end);
				if (_Visit_children)
				{
					for (auto & Section : _Sections)
					{
						if (Section)
						{
							Section->_Visit(_Al);
						}
					}
				}
				if (_Run_end)
				{
					_Al->_Visit_end(this);
				}
			}

			/************************************************************************/
			/* 工具                                                                 */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexCharClass : public Cs_RegexTraits<_Elem>, public Cs_Object
			{
			public:
				Cs_RegexCharClass();

				Cs_RegexBitSet		_Chr_class;				/*每一个Char对应的Class，-1为其它类型*/
				Cs_RegexCharSet		_Chr_range;				/*每一个Class对应的范围*/
				ctype_base::mask	_Mask;					/*字符掩码*/
			};

			template <typename _Elem>
			Cs_RegexCharClass<_Elem>::Cs_RegexCharClass()
				: _Chr_class(Cs_RegexTraits::char_size)
				, _Mask(0)
			{

			}

			/************************************************************************/
			/* 错误                                                                 */
			/************************************************************************/

			class Cs_RegexError				
			{	// type of all regular expression exceptions
			public:
				Cs_RegexError()
					: _Err(constants::cserr_none), _Loc(-1)
				{
				}
				
				Cs_RegexError(const _tstring& _Message, constants::Cse_RegexError _Ex, cint _Lx = -1)
					: _Mywhat(_Message), _Err(_Ex), _Loc(_Lx)
				{	// construct
				}

				Cs_RegexError(const Cs_RegexError& _This)
					: _Mywhat(_This._Mywhat), _Err(_This._Err), _Loc(_This._Loc)
				{

				}

				Cs_RegexError& operator = (const Cs_RegexError& _This)
				{
					_Mywhat = (_This._Mywhat);
					_Err = (_This._Err);
					_Loc = (_This._Loc);
					return *this;
				}

				const TCHAR* what() const
				{	// return pointer to message string
					return (_Mywhat.c_str());
				}

				constants::Cse_RegexError code() const
				{	// return stored error code
					return (_Err);
				}

				cint line() const
				{	// return stored error code
					return (_Loc);
				}

			private:
				_tstring _Mywhat;
				constants::Cse_RegexError _Err;
				cint _Loc;
			};

			/************************************************************************/
			/* 构造                                                                 */
			/************************************************************************/

			template <typename _FwdIt, typename _Elem, typename _RxTraits = _Regex_traits<_Elem>>
			class Cs_RegexBuilder : public Cs_RegexTraits<_Elem>
			{	// provides operations used by Cs_RegexParser to build the syntax tree
				Cs_RegexBuilder& operator=(const Cs_RegexBuilder&) = delete;
			public:
				Cs_RegexBuilder(const _RxTraits& _Tr);

				void _Add_bol();
				void _Add_eol();
				void _Add_dot();
				void _Add_char(_Elem _Ch);
				void _Add_class();
				void _Add_char_to_class(_Elem _Ch);
				void _Add_range(_Elem _E0, _Elem _E1);
				void _Add_named_class(ctype_base::mask);
				void _Add_named_class(_FwdIt _First, _FwdIt _Las);
				void _Add_pipe(Cse_RegexContent);
				void _Add_named_pipe(_FwdIt _First, _FwdIt _Last);
				Cs_RegexPassPtr _Add_func(Cse_RegexFunc);
				Cs_RegexPassPtr _Begin_group(bool);
				void _End_group(Cs_RegexPassPtr _Back);
				void _End_sub_group(Cs_RegexPassPtr _Back);
				Cs_RegexPassPtr _Begin_assert_group(bool);
				void _End_assert_group(Cs_RegexPassPtr);
				Cs_RegexPassPtr _Begin_capture_group(bool);
				void _Add_rep(int _Min, int _Max, bool _Greedy);
				void _No_rep();
				void _Negate();
				Cs_RegexPassPtr _End_pattern();

			private:
				template <typename T>
				T* _Conv_ptr(Cs_RegexPassPtr p)
				{
					return dynamic_cast<T*>(p.get());
				}

			private:
				RefPtr<_Cs_RegexSection> _Cur_exp;
				RefPtr<_Cs_RegexCharset> _Cur_chr;
				RefPtr<_Cs_RegexFunc> _Cur_fun;
				const _RxTraits& _Trait;
			};

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::Cs_RegexBuilder(const _RxTraits& _Tr)
				: _Trait(_Tr)
				, _Cur_exp(adoptRef(new _Cs_RegexSection))
			{
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_bol()
			{	// add bol node
				_Add_class();
				_Cur_chr->_Type = constants::csrec_string_begin;
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_eol()
			{	// add eol node
				_Add_class();
				_Cur_chr->_Type = constants::csrec_string_end;
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_dot()
			{	// add dot node
				_Add_class();
				_Negate();
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_char(_Elem _Ch)
			{	// append character
				_Add_range(_Ch, _Ch);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_class()
			{	// add bracket expression node
				if (!_Cur_chr)
					_Cur_chr = adoptRef(new _Cs_RegexCharset);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_char_to_class(_Elem _Ch)
			{	// add character to bracket expression
				_Add_char(_Ch);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_range(_Elem _E0, _Elem _E1)
			{	// add character range to set
				_Add_class();
				_Cur_chr->_Chr_set.insert(make_pair(_E0, _E1));
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_named_class(ctype_base::mask _Mask)
			{	// add contents of named class to bracket expression
				_Add_class();
				_Cur_chr->_Mask |= _Mask;
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_named_class(_FwdIt _First, _FwdIt _Last)
			{	// add contents of named class to bracket expression				
				_Cur_chr->_Esc.insert(_Trait.transform_primary(_First, _Last));
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_pipe(Cse_RegexContent _Cont)
			{	// add pipe content type
				_Cur_fun->_Cont = _Cont;
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_named_pipe(_FwdIt _First, _FwdIt _Last)
			{	// add pipe name
				_Cur_fun->_Pat = _Trait.transform_primary(_First, _Last);
				_Cur_fun = nullptr;
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			typename Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::Cs_RegexPassPtr
				Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_func(Cse_RegexFunc _Func)
			{	// add func
				_Cur_fun = adoptRef(new _Cs_RegexFunc);
				_Cur_fun->_Func = _Func;
				RefPtr<_Cs_RegexSection> _Old = _Cur_exp;
				_Old->_Sections.push_back(_Cur_fun);
				_Cur_exp = adoptRef(new _Cs_RegexSection);
				_Cur_fun->_Exp = _Cur_exp;
				return (_Old);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			typename Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::Cs_RegexPassPtr
				Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Begin_group(bool _Seq)
			{	// add group node
				RefPtr<_Cs_RegexSection> _Old = _Cur_exp;
				_Cur_exp = adoptRef(new _Cs_RegexSection(_Seq ? constants::csres_sequence : constants::csres_branch));
				_Old->_Sections.push_back(_Cur_exp);
				return (_Old);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_End_group(Cs_RegexPassPtr _Back)
			{	// add end of group node
				RefPtr<_Cs_RegexSection> _Par = _Conv_ptr<_Cs_RegexSection>(_Back), _Tmp = _Cur_exp;
				if (_Par->_Sections.size() == 1 && _Par->_Sections.front() == _Tmp)
				{
					_Par->_Type = _Tmp->_Type;
					_Par->_Sections.clear();
					copy(_Tmp->_Sections.begin(), _Tmp->_Sections.end(), back_inserter(_Par->_Sections));
				}
				_Cur_exp = _Par;
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_End_sub_group(Cs_RegexPassPtr _Back)
			{	// add end of sub group node
				_Cur_exp = _Conv_ptr<_Cs_RegexSection>(_Back);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			typename Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::Cs_RegexPassPtr
				Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Begin_assert_group(bool _Neg)
			{	// add assert node
				return _Add_func(_Neg ? constants::csref_negative : constants::csref_positive);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_End_assert_group(Cs_RegexPassPtr _Nx)
			{	// add end of assert node
				_End_group(_Nx);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			typename Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::Cs_RegexPassPtr
				Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Begin_capture_group(bool _Cap)
			{	// add capture group node
				return _Add_func(_Cap ? constants::csref_catch : constants::csref_uncatch);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Negate()
			{	// set flag
				_Cur_chr->_Neg = true;
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_Add_rep(int _Min, int _Max, bool _Greedy)
			{	// add repeat node
				if (_Cur_chr) // charset repeat
				{
					RefPtr<_Cs_RegexLoop> _Lp = adoptRef(new _Cs_RegexLoop);
					_Lp->_Exp = _Cur_chr;
					_Lp->_Min = _Min;
					_Lp->_Max = _Max;
					_Lp->_Rep = _Min > _Max;
					_Lp->_Greedy = _Greedy;
					_Cur_exp->_Sections.push_back(_Lp);
					_Cur_chr = nullptr;
				}
				else // expression repeat
				{
					RefPtr<_Cs_RegexLoop> _Lp = adoptRef(new _Cs_RegexLoop);
					_Lp->_Exp = _Cur_exp;
					_Lp->_Min = _Min;
					_Lp->_Max = _Max;
					_Lp->_Rep = _Min > _Max;
					_Lp->_Greedy = _Greedy;
					_Cur_exp = adoptRef(new _Cs_RegexSection);
					_Cur_exp->_Sections.push_back(_Lp);
				}
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_No_rep()
			{	// no explicit repeat
				_Cur_exp->_Sections.push_back(_Cur_chr);
				_Cur_chr = nullptr;
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			typename Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::Cs_RegexPassPtr
				Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits>::_End_pattern()
			{	// wrap up
				return _Cur_exp;
			}

			/************************************************************************/
			/* 分析                                                                 */
			/************************************************************************/

			template <typename _FwdIt, typename _Elem, typename _RxTraits = _Regex_traits<_Elem>>
			class Cs_RegexAnalysis : public Cs_RegexTraits<_Elem>
			{	// parse a regular expression
				Cs_RegexAnalysis& operator=(const Cs_RegexAnalysis&) = delete;
			public:
				explicit Cs_RegexAnalysis(const _RxTraits& _Tr, _FwdIt _Pfirst, _FwdIt _Plast, bool _Allow_ext);

			public:
				Cs_RegexPassPtr _Compile();

			private:
				//lexing
				void _Error(constants::Cse_RegexError);
				void _Require_extend();

				bool _Is_esc() const;
				void _Trans();
				void _Next();
				void _Expect(Meta_type, constants::Cse_RegexError);

				//parsing
				int _Do_digits(int _Base, int _Count);
				bool _DecimalDigits();
				void _HexDigits(int);
				void _OctalDigits();
				void _Do_ex_class(Meta_type);
				bool _CharacterClassEscape(bool);
				_Prs_ret _ClassEscape(bool);
				_Prs_ret _ClassAtom();
				void _ClassRanges();
				void _CharacterClass();
				bool _IdentityEscape();
				bool _Do_ffn(_Elem);
				bool _Do_ffnx(_Elem);
				bool _CharacterEscape();
				void _AtomEscape();
				void _Do_group();
				void _Do_capture_group();
				void _Do_noncapture_group();
				void _Do_assert_group(bool);
				bool _Wrapped_disjunction();
				void _Quantifier();
				bool _Alternative();
				void _Pipe();
				void _Disjunction();

			private:
				_FwdIt _Pat;
				_FwdIt _Begin;
				_FwdIt _End;
				cint _Disj_count;
				const _RxTraits& _Trait;
				cint _Val;
				_Elem _Char;
				Meta_type _Mchar;
				bool _Allow_ext;
				Cs_RegexPtr _Res;
				Cs_RegexBuilder<_FwdIt, _Elem, _RxTraits> _St;
			};

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::Cs_RegexAnalysis(
				const _RxTraits& _Tr, _FwdIt _Pfirst, _FwdIt _Plast, bool _Al_ext)
				: _Pat(_Pfirst), _Begin(_Pfirst), _End(_Plast), _Trait(_Tr), _Allow_ext(_Al_ext)
				, _St(_Tr), _Val(0), _Char(0), _Mchar(meta_chr), _Disj_count(0)
			{
				_Trans();
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Error(constants::Cse_RegexError _Err)
			{
				_Throw(_Err, _Pat - _Begin);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Require_extend()
			{
				if (!_Allow_ext)
					_Error(constants::cserr_unsupported);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			typename Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::Cs_RegexPassPtr
				Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Compile()
			{	// compile regular expression
				_Disjunction();
				if (_Pat != _End)
					_Error(constants::cserr_syntax);
				return _St._End_pattern();
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			bool Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Is_esc() const
			{	// assumes _Pat != _End
				_FwdIt _Ch0 = _Pat;
				return ((++_Ch0 != _End) &&
					((*_Ch0 == _Meta_lpar || *_Ch0 == _Meta_rpar) ||
					(*_Ch0 == _Meta_lbr || *_Ch0 == _Meta_rbr) ||
					(*_Ch0 == _Meta_lsq || *_Ch0 == _Meta_rsq)));
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Trans()
			{	// map character to meta-character
				if (_Pat == _End)
					_Mchar = meta_eos, _Char = (_Elem)meta_eos;
				else
				{	// map current character
					_Char = *_Pat;
					_Mchar = _CSTD strchr(Meta_map, _Char) != 0
						? (Meta_type)_Char : meta_chr;
				}
				switch (_Char)
				{	// handle special cases
					case meta_esc:
						if (_Is_esc())
						{	// replace escape sequence
							_FwdIt _Ch0 = _Pat;
							_Mchar = Meta_type(_Char = *++_Ch0);
						}
						break;

					case meta_nl:
						if (_Disj_count == 0)
							_Mchar = meta_bar;
						break;

					case meta_dlr:
					{	// check if $ is special
						_FwdIt _Ch0 = _Pat;
						if (_Allow_ext && ++_Ch0 != _End && *_Ch0 != meta_nl)
							_Mchar = meta_chr;
						break;
					}
				}
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Next()
			{	// advance to next input character
				if (_Pat != _End)
				{	// advance
					if (*_Pat == meta_esc && _Is_esc())
						++_Pat;
					++_Pat;
				}
				_Trans();
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Expect(
				Meta_type _St, constants::Cse_RegexError _Code)
			{	// check whether current meta-character is _St
				if (_Mchar != _St)
					_Error(_Code);
				_Next();
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			int Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Do_digits(
				int _Base, int _Count)
			{	// translate digits to numeric value
				int _Chv;
				_Val = 0;
				regex_traits<_Elem> _Tr;
				while (_Count != 0 && (_Chv = _Tr.value(_Char, _Base)) != -1)
				{	// append next digit
					--_Count;
					_Val *= _Base;
					_Val += _Chv;
					_Next();
				}
				return (_Count);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			bool Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_DecimalDigits()
			{	// check for decimal value
				return (_Do_digits(10, INT_MAX) != INT_MAX);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_HexDigits(int _Count)
			{	// check for _Count hex digits
				if (_Do_digits(16, _Count) != 0)
					_Error(constants::cserr_escape);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_OctalDigits()
			{	// check for up to 3 octal digits
				if (_Do_digits(8, 3) != 0)
					_Error(constants::cserr_escape);
			}

			template<class _FwdIt, class _Elem,	class _RxTraits>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Do_ex_class(Meta_type _End)
			{	// handle delimited expressions within bracket expression
				auto _Errtype =
					(_End == meta_colon ? constants::cserr_ctype
					: constants::cserr_syntax);
				_FwdIt _Beg = _Pat;

				while (_Mchar != meta_colon && _Mchar != meta_eos)
				{	// advance to end delimiter
					_Next();
				}
				if (_Mchar != _End)
					_Error(_Errtype);
				else if (_End == meta_colon)
				{	// handle named character class
					typename _RxTraits::char_class_type _Cls =
						_Trait.lookup_classname(_Beg, _Pat);
					if (!_Cls)
						_Error(constants::cserr_ctype);
					_St._Add_named_class(_Cls);
					_St._Add_named_class(_Beg, _Pat);
				}
				_Next();
				_Expect(meta_rsq, _Errtype);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			bool Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_CharacterClassEscape(bool _Addit)
			{	// check for character class escape
				typename _RxTraits::char_class_type _Cls;
				_FwdIt _Ch0 = _Pat;
				if (_Ch0 == _End
					|| (_Cls = _Trait.lookup_classname(_Pat, ++_Ch0) == 0))
					return (false);

				if (_Addit)
					_St._Add_class();
				_St._Add_named_class(_Cls);
				_St._Add_named_class(_Pat, _Ch0);
				if (_Trait.isctype(_Char, _RxTraits::_Ch_upper))
					_St._Negate();
				_Next();
				return (true);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			_Prs_ret Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_ClassEscape(bool _Addit)
			{	// check for class escape
				if (_CharacterClassEscape(_Addit))
					return (_Prs_set);
				else if (_DecimalDigits())
				{	// check for invalid value
					if (_Val != 0)
						_Error(constants::cserr_escape);
					return (_Prs_chr);
				}
				return (_CharacterEscape() ? _Prs_chr : _Prs_none);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			_Prs_ret Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_ClassAtom()
			{	// check for class atom
				if (_Mchar == meta_esc)
				{	// check for valid escape sequence
					_Next();
					return (_ClassEscape(false));
				}
				else if (_Mchar == meta_lsq)
				{	// check for valid delimited expression
					_Next();
					if (_Mchar == meta_colon)
					{	// handle delimited expression
						Meta_type _St = _Mchar;
						_Next();
						_Do_ex_class(_St);
						return (_Prs_set);
					}
					else
					{	// handle ordinary [
						_Val = meta_lsq;
						return (_Prs_chr);
					}
				}
				else if (_Mchar == meta_rsq || _Mchar == meta_eos)
					return (_Prs_none);
				else
				{	// handle ordinary character
					_Val = _Char;
					_Next();
					return (_Prs_chr);
				}
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_ClassRanges()
			{	// check for valid class ranges
				_Prs_ret _Ret;

				for (;;)
				{	// process characters through end of bracket expression
					if ((_Ret = _ClassAtom()) == _Prs_none)
						return;
					else if (_Ret == _Prs_set)
						;
					else if (_Mchar == meta_dash)
					{	// check for valid range
						_Next();
						_Elem _Chr1 = (_Elem)_Val;
						if ((_Ret = _ClassAtom()) == _Prs_none)
						{	// treat - as ordinary character
							_St._Add_char_to_class((_Elem)_Val);
							_St._Add_char_to_class(meta_dash);
							return;
						}
						else if (_Ret == _Prs_set)
							_Error(constants::cserr_range);	// set follows dash
						if (_Val < _Chr1)
							_Error(constants::cserr_range);
						_St._Add_range(_Chr1, (_Elem)_Val);
					}
					else
						_St._Add_char_to_class((_Elem)_Val);
				}
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_CharacterClass()
			{	// add bracket expression
				_St._Add_class();
				if (_Mchar == meta_caret)
				{	// negate bracket expression
					_St._Negate();
					_Next();
				}
				if (_Allow_ext && _Mchar == meta_rsq)
				{	// insert initial ] when not special
					_St._Add_char_to_class(meta_rsq);
					_Next();
				}
				_ClassRanges();
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			bool Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_IdentityEscape()
			{	// check for valid identity escape
				if (// ECMAScript identity escape characters
					!_Trait.isctype(_Char, _RxTraits::_Ch_alnum)
					&& _Char != '_')
					;
				else
					return (false);
				_Val = _Char;
				_Next();
				return (true);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			bool Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Do_ffn(_Elem _Ch)
			{	// check for limited file format escape character
				if (_Ch == esc_ctrl_b)
					_Val = '\b';
				else if (_Ch == esc_ctrl_f)
					_Val = '\f';
				else if (_Ch == esc_ctrl_n)
					_Val = '\n';
				else if (_Ch == esc_ctrl_r)
					_Val = '\r';
				else if (_Ch == esc_ctrl_t)
					_Val = '\t';
				else if (_Ch == esc_ctrl_v)
					_Val = '\v';
				else
					return (false);
				return (true);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			bool Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Do_ffnx(_Elem _Ch)
			{	// check for the remaining file format escape character
				if (_Ch == esc_ctrl_a)
					_Val = '\a';
				else
					return (false);
				return (true);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			bool Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_CharacterEscape()
			{	// check for valid character escape
				if (_Do_ffn(_Char) || _Do_ffnx(_Char))
					_Next();
				else if (_Char == esc_ctrl)
				{	// handle control escape sequence
					_Next();
					if (!_Trait.isctype(_Char, _RxTraits::_Ch_alpha))
						_Error(constants::cserr_escape);
					_Val = (char)(_Char % 32);
					_Next();
				}
				else if (_Char == esc_hex)
				{	// handle hexadecimal escape sequence
					_Next();
					_HexDigits(2);
				}
				else if (_Char == esc_uni)
				{	// handle unicode escape sequence
					_Next();
					_HexDigits(4);
				}
				else if (_Char == esc_oct)
				{	// handle octal escape sequence
					_Next();
					_OctalDigits();
				}
				else
					return (_IdentityEscape());

				typedef typename make_unsigned<_Elem>::type _Uelem;
				if ((numeric_limits<_Uelem>::max)() < (unsigned int)_Val)
					_Error(constants::cserr_escape);
				_Val = (_Elem)_Val;
				return (true);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_AtomEscape()
			{	// check for valid atom escape
				if (_CharacterEscape())
					_St._Add_char((_Elem)_Val);
				else if (!_CharacterClassEscape(true))
					_Error(constants::cserr_escape);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Do_group()
			{	// add group
				Cs_RegexPtr _Pos1 = _St._Begin_group(true);
				_Disjunction();
				_St._End_group(_Pos1);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Do_capture_group()
			{	// add capture group
				Cs_RegexPtr _Pos1 = _St._Begin_capture_group(true);
				_Pipe();
				_Disjunction();
				_St._End_group(_Pos1);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Do_noncapture_group()
			{	// add non-capture group
				Cs_RegexPtr _Pos1 = _St._Begin_capture_group(false);
				_Pipe();
				_Disjunction();
				_St._End_group(_Pos1);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Do_assert_group(bool _Neg)
			{	// add assert group
				Cs_RegexPtr _Pos1 = _St._Begin_assert_group(_Neg);
				_Pipe();
				_Disjunction();
				_St._End_assert_group(_Pos1);
			}

			template<class _FwdIt, class _Elem, class _RxTraits>
			bool Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Wrapped_disjunction()
			{	// add disjunction inside group
				++_Disj_count;
				if (_Mchar == meta_rpar)
					_Error(constants::cserr_paren);
				else if (_Mchar == meta_query)
				{	// check for valid ECMAScript (?x ... ) group
					_Next();
					Meta_type _Ch = _Mchar;
					_Next();
					if (_Ch == meta_colon)
						_Do_noncapture_group();
					else if (_Ch == meta_exc)
					{	// process assert group, negating
						_Require_extend();
						_Do_assert_group(true);
						--_Disj_count;
						return (false);
					}
					else if (_Ch == meta_equal)
					{	// process assert group
						_Require_extend();
						_Do_assert_group(false);
						--_Disj_count;
						return (false);
					}
					else
						_Error(constants::cserr_syntax);
				}
				else
					_Do_group();
				--_Disj_count;
				return (true);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Quantifier()
			{	// check for quantifier following atom
				int _Min = 0;
				int _Max = -1;
				if (_Mchar == meta_star)
					;
				else if (_Mchar == meta_plus)
					_Min = 1;
				else if (_Mchar == meta_query)
					_Max = 1;
				else if (_Mchar == meta_lbr)
				{	// check for valid bracketed value
					_Next();
					if (!_DecimalDigits())
						_Error(constants::cserr_badbrace);
					_Min = _Val;
					if (_Mchar != meta_comma)
						_Max = _Min;
					else
					{	// check for decimal constant following comma
						_Next();
						if (_Mchar == meta_rbr)
							;
						else if (!_DecimalDigits())
							_Error(constants::cserr_badbrace);
						else
							_Max = _Val;
					}
					if (_Mchar != meta_rbr || (_Max != -1 && _Max < _Min))
						_Error(constants::cserr_badbrace);
				}
				else
				{
					_St._No_rep();
					return;
				}
				_Next();
				if (_Mchar == meta_query)
				{	// add non-greedy repeat node
					_Require_extend();
					_Next();
					_St._Add_rep(_Min, _Max, false);
				}
				else
					_St._Add_rep(_Min, _Max, true);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			bool Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Alternative()
			{	// check for valid alternative
				bool _Found = false;
				for (;;)
				{	// concatenate valid elements
					bool _Quant = true;
					if (_Mchar == meta_eos || _Mchar == meta_bar
						|| (_Mchar == meta_rpar && _Disj_count != 0))
						return (_Found);
					else if (_Mchar == meta_rpar)
						_Error(constants::cserr_paren);
					else if (_Mchar == meta_dot)
					{	// add dot node
						_St._Add_dot();
						_Next();
					}
					else if (_Mchar == meta_esc)
					{	// check for valid escape sequence
						_Next();
						_AtomEscape();
					}
					else if (_Mchar == meta_lsq)
					{	// add bracket expression
						_Next();
						_CharacterClass();
						_Expect(meta_rsq, constants::cserr_brack);
					}
					else if (_Mchar == meta_lpar)
					{	// check for valid group
						_Next();
						_Quant = _Wrapped_disjunction();
						_Expect(meta_rpar, constants::cserr_paren);
					}
					else if (_Mchar == meta_caret)
					{	// add bol node
						_St._Add_bol();
						_Next();
					}
					else if (_Mchar == meta_dlr)
					{	// add eol node
						_St._Add_eol();
						_Next();
					}
					else if (_Mchar == meta_star
						|| _Mchar == meta_plus
						|| _Mchar == meta_query
						|| _Mchar == meta_lbr)
						_Error(constants::cserr_badrepeat);
					else if (_Mchar == meta_rbr)
						_Error(constants::cserr_brace);
					else if (_Mchar == meta_rsq)
						_Error(constants::cserr_brack);
					else
					{	// add character
						_St._Add_char(_Char);
						_Next();
					}
					if (_Quant)
						_Quantifier();
					_Found = true;
				}
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Pipe()
			{	// check for named pipe
				if (_Mchar != meta_lt)
					return;
				_Next();
				if (_Mchar == meta_sharp)
				{	// store results
					_Require_extend();
					_Next();
					_St._Add_pipe(constants::csrec_storage);
				}
				else if (_Mchar == meta_dlr)
				{	// match results
					_Require_extend();
					_Next();
					_St._Add_pipe(constants::csrec_match);
				}
				else
				{	// save pipe name
					_St._Add_pipe(constants::csrec_named);
				}
				_FwdIt _Beg = _Pat;
				while (_Trait.isctype(_Char, _RxTraits::_Ch_alnum)
					|| _Char == '_')
				{	// advance to end delimiter
					_Next();
				}
				if (_Beg == _Pat)
					_Error(constants::cserr_named_null);
				_St._Add_named_pipe(_Beg, _Pat);
				_Expect(meta_gt, constants::cserr_ctype);
			}

			template <typename _FwdIt, typename _Elem, typename _RxTraits /*= _Regex_traits<_Elem>*/>
			void Cs_RegexAnalysis<_FwdIt, _Elem, _RxTraits>::_Disjunction()
			{	// check for valid disjunction
				Cs_RegexPtr _Pos = _St._Begin_group(false);
				Cs_RegexPtr _Pos1 = _St._Begin_group(true);
				_Alternative();
				while (_Mchar == meta_bar)
				{	// append terms as long as we keep finding | characters
					_Next();
					_St._End_sub_group(_Pos1);
					_Pos1 = _St._Begin_group(true);
					_Alternative();
				}
				_St._End_group(_Pos1);
				_St._End_group(_Pos);
			}

			/************************************************************************/
			/* 算法                                                                 */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexAlgorithm_To_string : public Cs_RegexIRecursiveAlgorithm<_Elem>
			{
			public:
				void _Visit_begin(_Cs_RegexCharset* _Node, bool& _Visit_children, bool& _Run_end);
				void _Visit_begin(_Cs_RegexLoop* _Node, bool& _Visit_children, bool& _Run_end);
				void _Visit_begin(_Cs_RegexFunc* _Node, bool& _Visit_children, bool& _Run_end);
				void _Visit_begin(_Cs_RegexSection* _Node, bool& _Visit_children, bool& _Run_end);
				void _Visit_end(_Cs_RegexCharset* _Node);
				void _Visit_end(_Cs_RegexSection* _Node);
				void _Visit_end(_Cs_RegexLoop* _Node);
				void _Visit_end(_Cs_RegexFunc* _Node);

				_tstring _To_string(Cs_RegexPassPtr _Exp);

			protected:
				void _Append_prefix();
				void _Reduce_prefix();

				_tstringstream _Res;
				_tstring _Prefix;
			};

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Visit_begin(_Cs_RegexCharset* _Node, bool& _Visit_children, bool& _Run_end)
			{
				_Res << _Prefix << Csex_RegexCharset::_To_string(_Node->_Type);
				switch (_Node->_Type)
				{
					case constants::csrec_charset:
					{
						_Res << (TCHAR)meta_lsq << Csx_RegexCharset::_Reverse(_Node->_Neg) << (TCHAR)meta_rsq
							<< (TCHAR)meta_colon;
						auto _Es = _Node->_Esc.empty(), _Cs = _Node->_Chr_set.empty();
						if (!_Es)
							_Res << _Char_esc(_Node->_Esc, meta_esc);
						if (!_Es && !_Cs)
							_Res << (TCHAR)meta_tab;
						if (!_Cs)
							_Res << _Char_set(_Node->_Chr_set);
					}
						break;
					case constants::csrec_string_begin:
						break;
					case constants::csrec_string_end:
						break;
				}
				_Res << endl;
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Visit_begin(_Cs_RegexLoop* _Node, bool& _Visit_children, bool& _Run_end)
			{
				_Res << _Prefix << Csx_RegexLoop::_To_string()
					<< (TCHAR)meta_lsq << Csx_RegexLoop::_Lazy(_Node->_Greedy) << (TCHAR)meta_rsq
					<< (TCHAR)meta_lbr << Csx_RegexLoop::_Infinite(_Node->_Rep, _Node->_Min, _Node->_Max) << (TCHAR)meta_rbr;
				_Append_prefix();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Visit_begin(_Cs_RegexFunc* _Node, bool& _Visit_children, bool& _Run_end)
			{
				_Res << _Prefix << Csx_RegexFunc::_To_string()
					<< (TCHAR)meta_lsq << Csex_RegexFunc::_To_string(_Node->_Func) << (TCHAR)meta_rsq
					<< (TCHAR)meta_lsq << Csex_RegexContent::_To_string(_Node->_Cont);
				if (!_Node->_Pat.empty())
					_Res << (TCHAR)meta_colon << _Conv(_Node->_Pat);
				_Res << (TCHAR)meta_rsq;
				_Append_prefix();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Visit_begin(_Cs_RegexSection* _Node, bool& _Visit_children, bool& _Run_end)
			{
				_Res << _Prefix << Csex_RegexSection::_To_string(_Node->_Type);
				_Append_prefix();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Visit_end(_Cs_RegexCharset* _Node)
			{

			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Visit_end(_Cs_RegexLoop* _Node)
			{
				_Reduce_prefix();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Visit_end(_Cs_RegexFunc* _Node)
			{
				_Reduce_prefix();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Visit_end(_Cs_RegexSection* _Node)
			{
				_Reduce_prefix();
			}

			template <typename _Elem>
			_tstring Cs_RegexAlgorithm_To_string<_Elem>::_To_string(Cs_RegexPassPtr _Exp)
			{
				Cs_RegexPtr _Ex = _Exp;
				if (_Ex)
				{
					_Ex->_Visit(this);
					return _Res.str();
				}
				else
				{
					return _T("");
				}
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Append_prefix()
			{
				_Prefix.push_back(meta_tab);
				_Res << (TCHAR)meta_space << (TCHAR)meta_lbr << endl;
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_To_string<_Elem>::_Reduce_prefix()
			{
				_Prefix.pop_back();
				_Res << _Prefix << (TCHAR)meta_rbr << endl;
			}

			template <typename _Elem>
			class Cs_RegexAlgorithm_Semantic : public Cs_RegexIRecursiveAlgorithm<_Elem>
			{
			public:
				Cs_RegexAlgorithm_Semantic();

				void _Visit_begin(_Cs_RegexCharset* _Node, bool& _Visit_children, bool& _Run_end);
				void _Visit_begin(_Cs_RegexLoop* _Node, bool& _Visit_children, bool& _Run_end);
				void _Visit_begin(_Cs_RegexFunc* _Node, bool& _Visit_children, bool& _Run_end);
				void _Visit_begin(_Cs_RegexSection* _Node, bool& _Visit_children, bool& _Run_end);
				void _Visit_end(_Cs_RegexCharset* _Node);
				void _Visit_end(_Cs_RegexSection* _Node);
				void _Visit_end(_Cs_RegexLoop* _Node);
				void _Visit_end(_Cs_RegexFunc* _Node);

				bool _Regex_analysis(Cs_RegexString _Exp, bool _Allow_ext);

				const Cs_RegexError& _Get_error();
				Cs_RegexPassPtr _Get_result();

			protected:
				template <typename _FwdIt>
				void _Regex_analysis_fwd(_FwdIt _First, _FwdIt _Last, bool _Allow_ext)
				{
					_Std_Regex_traits _Tr;
					Cs_RegexAnalysis<_FwdIt, _Elem, _Std_Regex_traits>
						_Al(_Tr, _First, _Last, _Allow_ext);
					_Regex_res = _Al._Compile();
					if (_Empty_exp(_Regex_res))
						_Throw(constants::cserr_null);
				}

				void _Inc();
				void _Dec();
				bool _Empty_exp(Cs_RegexPassPtr _Ex);

				Cs_RegexExpressionMap _Named_exps;
				Cs_RegexNamedRefMap _Named_refs;
				Cs_RegexStringListMap _Ref_graph;
				Cs_RegexStringList _Named_stack;
				Cs_RegexStringList _Top_named_exps;
				cint _Lv_cnt;
				Cs_RegexError _Regex_err;
				Cs_RegexPtr _Regex_res;
			};

			template <typename _Elem>
			Cs_RegexAlgorithm_Semantic<_Elem>::Cs_RegexAlgorithm_Semantic()
				: _Lv_cnt(0)
			{

			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Visit_begin(_Cs_RegexCharset* _Node, bool& _Visit_children, bool& _Run_end)
			{
				_Inc();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Visit_begin(_Cs_RegexLoop* _Node, bool& _Visit_children, bool& _Run_end)
			{
				_Inc();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Visit_begin(_Cs_RegexFunc* _Node, bool& _Visit_children, bool& _Run_end)
			{
				_Inc();
				if (_Node->_Cont == constants::csrec_named)
				{
					if (find(_Named_stack.begin(), _Named_stack.end(), _Node->_Pat) != _Named_stack.end())
					{
						_Throw(_Node->_Pat, constants::cserr_named_recursive_call);
					}
					else
					{
						if (!_Named_stack.empty())
							_Ref_graph.insert(make_pair(_Named_stack.back(), _Node->_Pat));
						else
							_Top_named_exps.push_back(_Node->_Pat);
						_Named_stack.push_back(_Node->_Pat);
						bool _Has_exp = !_Empty_exp(_Node->_Exp);
						bool _Has_pat = _Named_exps.find(_Node->_Pat) != _Named_exps.end();
						if (!_Has_exp)
							_Node->_Exp = nullptr;
						if (_Has_exp && _Has_pat && _Named_exps[_Node->_Pat])
							_Throw(_Node->_Pat, constants::cserr_named_redeclared);
						if (_Has_exp)
							_Named_exps.insert(make_pair(_Node->_Pat, _Node->_Exp));
						else
						{
							if (!_Has_pat)
							{
								_Named_exps.insert(make_pair(_Node->_Pat, nullptr));
							}
							_Named_refs.insert(make_pair(_Node->_Pat, _Node));
						}
					}
				}
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Visit_begin(_Cs_RegexSection* _Node, bool& _Visit_children, bool& _Run_end)
			{
				_Inc();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Visit_end(_Cs_RegexCharset* _Node)
			{
				_Dec();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Visit_end(_Cs_RegexSection* _Node)
			{
				_Dec();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Visit_end(_Cs_RegexLoop* _Node)
			{
				_Dec();
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Visit_end(_Cs_RegexFunc* _Node)
			{
				if (_Node->_Cont == constants::csrec_named)
				{
					_Named_stack.pop_back();
				}
				_Dec();
			}

			template <typename _Elem>
			const Cs_RegexError& Cs_RegexAlgorithm_Semantic<_Elem>::_Get_error()
			{
				return _Regex_err;
			}

			template <typename _Elem>
			typename Cs_RegexAlgorithm_Semantic<_Elem>::Cs_RegexPassPtr
				Cs_RegexAlgorithm_Semantic<_Elem>::_Get_result()
			{
				return _Regex_res;
			}

			template <typename _Elem>
			bool Cs_RegexAlgorithm_Semantic<_Elem>::_Regex_analysis(Cs_RegexString _Exp, bool _Allow_ext)
			{
				try
				{
					_Regex_analysis_fwd(_Exp.begin(), _Exp.end(), _Allow_ext);
					_Regex_res->_Visit(this);
				}
				catch(const Cs_RegexError& _Err)
				{
					_Regex_err = _Err;
				}
				return _Regex_err.code() == constants::cserr_none;
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Inc()
			{
				_Lv_cnt++;
			}

			template <typename _Elem>
			void Cs_RegexAlgorithm_Semantic<_Elem>::_Dec()
			{
				_Lv_cnt--;
				if (_Lv_cnt == 0)
				{
					for (auto & _Named_exp : _Named_exps)
					{
						if (!_Named_exp.second)
						{
							_Throw(_Named_exp.first, constants::cserr_named_undeclared);
						}
					}
					for (auto & _Top_named_exp : _Top_named_exps)
					{
						Cs_RegexStringList _Ref_stack;
						_Ref_stack.push_back(_Top_named_exp);
						for (auto & _Ref_stk : _Ref_stack)
						{
							auto _Cur = _Ref_graph.equal_range(_Ref_stk);
							for (auto _It = _Cur.first; _It != _Cur.second; _It++)
							{
								auto _Ch = _It->second;
								if (find(_Ref_stack.begin(), _Ref_stack.end(), _Ch) != _Ref_stack.end())
								{
									_Throw(_Ch, constants::cserr_named_recursive_call);
								}
								else
								{
									_Ref_stack.push_back(_Ch);
								}
							}
						}
					}
					for (auto & _Named_ref : _Named_refs)
					{
						_Named_ref.second->_Ref_source = _Named_exps[_Named_ref.first].get();
					}
				}
			}

			template <typename _Elem>
			bool Cs_RegexAlgorithm_Semantic<_Elem>::_Empty_exp(Cs_RegexPassPtr _Ex)
			{
				RefPtr<_Cs_RegexSection> _Sec = dynamic_cast<_Cs_RegexSection*>(_Ex.get());
				return _Sec->_Sections.empty();
			}
		}
	}
}

#endif