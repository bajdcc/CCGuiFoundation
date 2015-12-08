#ifndef CC_LIST
#define CC_LIST

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_control.h"
#include "cc_element.h"

using namespace cc::base;
using namespace cc::interfaces::windows;
using namespace cc::presentation::control;
using namespace cc::presentation::element;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			namespace helper
			{
				template<class T>
				struct PtrType
				{
					typedef typename RefPtr<T> RefType;
					typedef typename PassRefPtr<T> ParamType;
					typedef typename vector<RefType> ContainerType;
					typedef typename ContainerType::size_type SizeType;
				};

				template<class T, class P = PtrType<T>>
				class ItemsBase : public Object
				{
				public:
					typedef typename P::RefType RefType;
					typedef typename P::ParamType ParamType;
					typedef typename P::SizeType SizeType;
					typedef typename P::ContainerType ContainerType;

				public:
					ItemsBase()
					{
					}

					~ItemsBase()
					{
					}

					bool NotifyUpdate(SizeType start, SizeType count = 1)
					{
						auto count = Count();
						if (start < 0 ||
							start >= count ||
							count <= 0 ||
							start + count > count)
						{
							return false;
						}
						else
						{
							NotifyUpdateInternal(start, count, count);
							return true;
						}
					}

					bool Contains(ParamType _item) const
					{
						RefType item = _item;
						return find(items.begin(), items.end(), item) != items.end();
					}

					SizeType Count() const
					{
						return (SizeType)items.size();
					}

					SizeType Count()
					{
						return (SizeType)items.size();
					}

					ParamType Get(SizeType index) const
					{
						return items.at(index);
					}

					ParamType operator[](SizeType index) const
					{
						return items.at(index);
					}

					Null<SizeType> IndexOf(ParamType _item)const
					{
						RefType item = _item;						
						for (SizeType i = 0; i < items.size(); i++)
						{
							if (Get(i) == item)
								return i;
						}
						return Null<SizeType>();
					}

					SizeType Add(ParamType _item)
					{
						RefType item = _item;
						auto index = Count();
						if (QueryInsert(index, item))
						{
							BeforeInsert(index, item);
							items.push_back(item);
							AfterInsert(index, item);
							NotifyUpdateInternal(index, 0, 1);
							return index;
						}
						else
						{
							return -1;
						}
					}

					SizeType Insert(SizeType index, ParamType _item)
					{
						RefType item = _item;
						if (0 <= index && index <= Count() && QueryInsert(index, item))
						{
							BeforeInsert(index, item);
							items.insert(items.begin() + index, item);
							AfterInsert(index, item);
							NotifyUpdateInternal(index, 0, 1);
							return index;
						}
						else
						{
							return -1;
						}
					}

					bool Set(SizeType index, ParamType _item)
					{
						RefType item = _item;
						if (0 <= index && index < Count())
						{
							if (QueryRemove(index, Get(index)) && QueryInsert(index, item))
							{
								BeforeRemove(index, Get(index));
								items.erase(items.begin() + index);
								AfterRemove(index, 1);

								BeforeInsert(index, item);
								items.Insert(items.begin() + index, item);
								AfterInsert(index, item);

								NotifyUpdateInternal(index, 1, 1);
								return true;
							}
						}
						return false;
					}

					bool Remove(ParamType _item)
					{
						RefType item = _item;
						auto found = find(items.begin(), items.end(), item);
						if (found != items.end())
						{
							BeforeRemove(index, *found);
							items.erase(found);
							AfterRemove(index, 1);
							NotifyUpdateInternal(index, 1, 0);
						}
						return false;
					}

					bool RemoveAt(SizeType index)
					{
						if (0 <= index && index < Count() && QueryRemove(index, Get(index)))
						{
							BeforeRemove(index, Get(index));
							items.erase(items.begin() + index);
							AfterRemove(index, 1);
							NotifyUpdateInternal(index, 1, 0);
							return true;
						}
						return false;
					}

					bool RemoveRange(SizeType index, SizeType count)
					{
						if (count <= 0)
						{
							return false;
						}
						if (0 <= index && index < Count() && index + count <= Count())
						{
							for (SizeType i = 0; i < count; i++)
							{
								if (!QueryRemove(index + 1, Get(index + i)))
								{
									return false;
								}
							}
							for (SizeType i = 0; i < count; i++)
							{
								BeforeRemove(index + i, Get(index + i));
							}
							items.erase(items.begin() + index, items.begin() + index + count);
							AfterRemove(index, count);
							NotifyUpdateInternal(index, count, 0);
							return true;
						}
						return false;
					}

					bool Clear()
					{
						auto count = Count();
						for (SizeType i = 0; i < count; i++)
						{
							if (!QueryRemove(i, Get(i))) return false;
						}
						for (SizeType i = 0; i < count; i++)
						{
							BeforeRemove(i, Get(i));
						}
						items.clear();
						AfterRemove(0, count);
						NotifyUpdateInternal(0, count, 0);
						return true;
					}

				protected:
					virtual void NotifyUpdateInternal(SizeType start, SizeType count, SizeType newCount)
					{
					}

					virtual bool QueryInsert(SizeType index, ParamType value)
					{
						return true;
					}

					virtual void BeforeInsert(SizeType index, ParamType value)
					{
					}

					virtual void AfterInsert(SizeType index, ParamType value)
					{
					}

					virtual bool QueryRemove(SizeType index, ParamType value)
					{
						return true;
					}

					virtual void BeforeRemove(SizeType index, ParamType value)
					{
					}

					virtual void AfterRemove(SizeType index, SizeType count)
					{
					}

					ContainerType items;
				};
			}
		}
	}
}

#endif