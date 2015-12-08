#include "stdafx.h"
#include "cs_regfa.h"

namespace cc
{
	namespace script
	{
		namespace regex
		{
			Cs_RegexNFA::Cs_RegexNFA()
			{

			}

			void Cs_RegexNFA::Init(_Cs_AtmEdge* _Data)
			{
				_Data->_Data._Action = constants::csree_epsilon;
				_Data->_Data._Param = 0;
				_Data->_Data._Ins.clear();
			}

			void Cs_RegexNFA::Init(_Cs_AtmStatus* _Data)
			{
				_Data->_Data._Final_status = false;
			}
		}
	}
}