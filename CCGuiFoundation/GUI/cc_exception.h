#ifndef CC_EXCEPTION
#define CC_EXCEPTION

#include "stdafx.h"

namespace cc
{
	namespace exception
	{
		class runtime_thread_interrupt : public std::logic_error
		{
		public:
			explicit runtime_thread_interrupt(const std::string& _Message, std::thread::id _Tid);
			virtual const char * what() const throw();

		private:
			std::thread::id _MyTid;
			std::string _MyMessage;
		};
	}
}

#endif