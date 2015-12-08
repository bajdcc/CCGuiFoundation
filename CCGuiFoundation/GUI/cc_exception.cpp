#include "stdafx.h"
#include "cc_exception.h"

namespace cc
{
	namespace exception
	{
		runtime_thread_interrupt::runtime_thread_interrupt(const std::string& _Message, std::thread::id _Tid)
			: logic_error(_Message), _MyTid(_Tid)
		{
			std::stringstream ss;
			ss << std::endl
				<< "## Runtime Thread Interruption ##" << std::endl
				<< "## Thread ID: " << std::hex << _MyTid << std::endl
				<< "## Message: " << logic_error::what() << std::endl;
			_MyMessage = ss.str();
		}

		const char * runtime_thread_interrupt::what() const throw()
		{
			return _MyMessage.c_str();
		}
	}
}