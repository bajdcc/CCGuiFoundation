#ifndef CC_DEBUG
#define CC_DEBUG

#include "stdafx.h"

namespace cc
{
	namespace debug
	{
		typedef std::map<UINT, CString> MSGMAP;
		void Msg_Init(MSGMAP&);
	}
}

#endif