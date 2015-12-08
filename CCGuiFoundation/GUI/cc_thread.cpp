#include "stdafx.h"
#include "cc_thread.h"

namespace cc
{
	namespace threading
	{
		spin_mutex::spin_mutex()
		{
			flag.clear();
		}

		void spin_mutex::lock()
		{
			while (flag.test_and_set(std::memory_order_acquire));
		}

		void spin_mutex::unlock()
		{
			flag.clear(std::memory_order_release);
		}

		semaphore::semaphore(int value /*= 1*/)
			: count(value)
			, wakeups(0)
		{
		}

		bool semaphore::wait()
		{
			std::unique_lock<std::mutex> lock(mutex);
			if (--count < 0) { // count is not enough ?
				condition.wait(lock, [&]()->bool{ return wakeups > 0; }); // suspend and wait ...
				--wakeups;  // ok, me wakeup !
			}
			return true;
		}

		bool semaphore::wait_for(const std::chrono::milliseconds& ms)
		{
			std::unique_lock<std::mutex> lock(mutex);
			bool ret = false;
			if (--count < 0) { // count is not enough ?
				ret = condition.wait_for(lock, ms, [&]()->bool{ return wakeups > 0; }); // suspend and wait ...
				--wakeups;  // ok, me wakeup !
			}
			return ret;
		}

		void semaphore::signal()
		{
			std::unique_lock<std::mutex> lock(mutex);
			if (++count <= 0) { // have some thread suspended ?
				++wakeups;
				condition.notify_one(); // notify one !
			}
		}

		bool thread_pool_lite::queue(const std::function<void(void*)>& proc, void* argument)
		{
			auto* arg = new argument_type;
			arg->func = proc;
			arg->argument = argument;
			if (QueueUserWorkItem(&ThreadPoolQueueProc, arg, WT_EXECUTEDEFAULT))
			{
				return true;
			}
			else
			{
				delete arg;
				return false;
			}
		}

		bool thread_pool_lite::queue(const std::function<void()>& proc)
		{
			auto* func = new std::function<void()>(proc);
			if (QueueUserWorkItem(&ThreadPoolQueueFunc, func, WT_EXECUTEDEFAULT))
			{
				return true;
			}
			else
			{
				delete func;
				return false;
			}
		}

		DWORD WINAPI ThreadPoolQueueProc(void* argument)
		{
			typedef thread_pool_lite::argument_type argument_type;
			std::auto_ptr<argument_type> proc(static_cast<argument_type*>(argument));
			proc->func(proc->argument);
			return 0;
		}

		DWORD WINAPI ThreadPoolQueueFunc(void* argument)
		{
			typedef std::function<void()> func_type;
			std::auto_ptr<func_type> proc(static_cast<func_type*>(argument));
			(*proc)();
			return 0;
		}
	}
}
