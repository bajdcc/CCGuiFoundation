#ifndef CC_TEMPLATE
#define CC_TEMPLATE

#include "stdafx.h"
#include "cc_base.h"
#include "WTF\PassRefPtr.h"

namespace cc
{
	namespace base
	{
		template<typename T>
		class Null
		{
		private:
			T object;
			bool empty;

		public:
			Null()
				: empty(true)
			{
			}

			Null(const T& value)
				: empty(false), object(value)
			{
			}

			Null& operator=(const T& value)
			{
				empty = false;
				object = value;
				return(*this);
			}

			Null& operator=(const Null& value)
			{
				empty = value.empty;
				if (!empty)
					object = value.object;
				return(*this);
			}

			bool operator==(const Null& value)
			{
				if (empty && value.empty)
					return true;
				if (empty || value.empty)
					return false;
				return object == value.object;
			}

			bool operator!=(const Null& value)
			{
				return !(*this == value);
			}

			operator bool() const
			{
				return !empty;
			}

			operator const T&() const
			{
				return object;
			}

			const T& operator~() const
			{
				return object;
			}

			const T& operator*() const
			{
				return object;
			}
		};

		template <class T>
		class RawPtr
		{
		public:
			RawPtr() : m_ptr(nullptr){}
			RawPtr(T* ptr) : m_ptr(ptr){}
			RawPtr(const PassRefPtr<T>& ptr){ m_ptr = ptr.get(); }
			~RawPtr(){ m_ptr = nullptr; }

			RawPtr& operator=(T* ptr)
			{
				m_ptr = ptr;
				return *this;
			}
			RawPtr& operator=(const PassRefPtr<T>& ptr)
			{
				m_ptr = ptr.get();
				return *this;
			}
			T* operator->() const
			{
				ASSERT(m_ptr);
				return m_ptr;
			}
			T* get() { return m_ptr; }
			T& operator*() const { return *m_ptr; }
			T* operator~() const { return m_ptr; }
			bool operator!() const { return !m_ptr; }
			bool operator!=(const PassRefPtr<T>& ptr) const { return m_ptr != ptr.get(); }
			bool operator==(const PassRefPtr<T>& ptr) const { return m_ptr == ptr.get(); }
			bool operator!=(const RawPtr& ptr) const { return m_ptr != ptr.m_ptr; }
			bool operator==(const RawPtr& ptr) const { return m_ptr == ptr.m_ptr; }
			bool operator<(const RawPtr& ptr) const { return m_ptr < ptr.m_ptr; }
			bool operator>(const RawPtr& ptr) const { return m_ptr > ptr.m_ptr; }
			operator T*() { return m_ptr; }
			template<class X> operator PassRefPtr<X>() { return dynamic_cast<X*>(m_ptr); }
			template<class X> operator X*() { return dynamic_cast<X*>(m_ptr); }

		private:
			T* m_ptr;
		};
	}
}

#endif