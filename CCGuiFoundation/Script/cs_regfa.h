#ifndef CS_REGFA
#define CS_REGFA

#include "stdafx.h"
#include "../GUI/cc_base.h"
#include "cs_atm.h"
#include "cs_regex.h"

#define ENFA_POOL_SIZE 1024

using namespace std;
using namespace cc::base;

namespace cc
{
	namespace script
	{
		namespace regex
		{
			namespace constants
			{
				/************************************************************************/
				/* NFA                                                                  */
				/************************************************************************/

				enum Cse_RegexEdge			// 边类别
				{
					csree_epsilon,			/*无*/
					csree_match,			/*存储序号*/
					csree_charset,			/*字符集*/
					csree_string_begin,		/*无*/
					csree_string_end,		/*无*/
					csree_positive,			/*自动机序号*/
					csree_negative			/*自动机序号*/
				};

				enum Cse_RegexEdgeIns		//构造匹配结果的指令
				{
					csrei_catch,			/*无*/
					csrei_storage,			/*存储序号*/
					csrei_pass,				/*字符数量*/
					csrei_match,			/*存储序号*/
					csrei_end,				/*无*/
					csrei_long_loop,		/*重复序号*/
					csrei_short_loop,		/*重复序号*/
					csrei_endloop			/*无*/
				};

				enum Cse_RegexEdgeRep		//重复指令
				{
					csrer_long,				/*贪婪*/
					csrer_short				/*非贪婪*/
				};
			}

			struct Cs_RegexEdgeIns
			{
				constants::Cse_RegexEdge _Ins;
				cint _Param;
			};

			using Cs_RegexEdgeInsList = vector<Cs_RegexEdgeIns>;

			/************************************************************************/
			/* ENFA                                                                 */
			/************************************************************************/

			struct Cs_RegexNFAEdgeData
			{
				Cs_RegexEdgeInsList _Ins;
				constants::Cse_RegexEdge _Action;
				cint _Param;
			};

			struct Cs_RegexNFAStatusData
			{
				bool _Final_status;
			};

			template <typename _EdgeData, typename _StatusData>
			class Cs_RegexENFA
			{
			public:
				RawPtr<_StatusData> _Init_status;
				RawPtr<_StatusData> _Final_status;
			};

			template <typename _EdgeData, typename _StatusData>
			class Cs_RegexENFABag
			{
				using _Cs_RegexENFA = Cs_RegexENFA<_EdgeData, _StatusData>;
				using _Cs_RegexENFAList = std::vector<_Cs_RegexENFA>;
			public:
				_Cs_RegexENFA		_MainENFA;
				_Cs_RegexENFAList	_SubENFAs;
			};

			class Cs_RegexNFA : public Cs_Automata<Cs_RegexNFAEdgeData, Cs_RegexNFAStatusData, ENFA_POOL_SIZE>
			{
				using _ENFA = Cs_RegexENFA<_Cs_AtmEdge, _Cs_AtmStatus>;
			public:
				Cs_RegexNFA();

				void Init(_Cs_AtmEdge*);
				void Init(_Cs_AtmStatus*);
			};

			/************************************************************************/
			/* FA                                                                   */
			/************************************************************************/

			template <typename _Elem>
			class Cs_RegexFAInfo : public Cs_RegexTraits<_Elem>
			{
				using _Cs_AtmEdge = Cs_RegexNFA::_Cs_AtmEdge;
				using _Cs_AtmStatus = Cs_RegexNFA::_Cs_AtmStatus;
				using _Cs_RegexENFABag = Cs_RegexENFABag<_Cs_AtmEdge, _Cs_AtmStatus>;
				using _Cs_RegexENFAList = _Cs_RegexENFABag::_Cs_RegexENFAList;

			public:
				explicit Cs_RegexFAInfo(Cs_RegexPassPtr _Regex_str);

				Cs_RegexStringList				_Storage_id;			/*存储管道名称表*/
				Cs_RegexExpressionMap			_Named_exp;				/*命名子表达式表*/
				Cs_RegexPtr						_Regex;					/*正则表达式*/

				_Cs_AtmStatus*					_MainNFA;				/*主要NFA*/
				_Cs_RegexENFAList				_SubNFAs;				/*附加NFA*/
			};

			template <typename _Elem>
			Cs_RegexFAInfo<_Elem>::Cs_RegexFAInfo(Cs_RegexPassPtr _Regex_str)
				: _MainNFA(nullptr)
				, _Regex(_Regex_str)
			{
// 				RegexpInformationGenerationAlgorithm _Al;
// 				_Al.Info = this;
// 				Expression->Visit(&_Al);
			}
		}
	}
}

#endif