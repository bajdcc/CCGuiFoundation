#ifndef CC_THREAD
#define CC_THREAD

#include "stdafx.h"
#include "cc_base.h"

using namespace cc::base;

namespace cc
{
	namespace threading
	{
		class spin_mutex
		{
			std::atomic_flag flag;
		public:
			spin_mutex();
			spin_mutex(const spin_mutex&) = delete;
			spin_mutex& operator= (const spin_mutex&) = delete;

			void lock();
			void unlock();
		};

		class semaphore : public Object
		{
		public:
			semaphore(int value = 1);

			bool wait();
			bool wait_for(const std::chrono::milliseconds&);
			void signal();

		private:
			int count;
			int wakeups;
			std::mutex mutex;
			std::condition_variable condition;
		};		

		class thread_pool_lite
		{
		private:
			thread_pool_lite() = default;

		public:
			struct argument_type
			{
				std::function<void(void*)> func;
				void* argument;
			};

			static bool queue(const std::function<void()>& proc);
			static bool queue(const std::function<void(void*)>& proc, void* argument);
		};

		DWORD WINAPI ThreadPoolQueueProc(void* argument);
		DWORD WINAPI ThreadPoolQueueFunc(void* argument);
	}
}

#endif