#ifndef CS_POOL
#define CS_POOL

#include "stdafx.h"
#include "WTF/RefPtr.h"
#include "WTF/AVLTree.h"
#include "../GUI/cc_base.h"

#define POOL_SIZE 256

using namespace std;
using namespace cc::base;

namespace cc
{
	namespace script
	{
		template<typename _Object>
		class Cs_PoolInit
		{
		public:
			void init(_Object* Object)
			{
			};
		};

		template<typename _Object, typename _Init = Cs_PoolInit<_Object>, size_t _Size = POOL_SIZE>
		class Cs_FixedPool : public Cs_Object
		{
// 			static_assert(std::is_pod<_Object>::value, "Object must be POD type");
		public:
			typedef Cs_FixedPool<_Object>	_Pool;
			typedef const _Object*			_Pointer;

			Cs_FixedPool(_Init* _In = nullptr)
			{
				auto size = __max(_Size, 2);
				_Objs.resize(size);
				_Free_obj.resize(size);
				for (size_t i = 0; i < size; i++)
				{
					_Free_obj[i] = i;
				}
				_Initor = _In;
			}

			~Cs_FixedPool()
			{

			}

			size_t used_cnt() const
			{
				return _Objs.size() - _Free_obj.size();
			}

			size_t free_cnt() const
			{
				return _Free_obj.size();
			}

			size_t size() const
			{
				return _Objs.size();
			}

			auto bits() const -> const bitset<_Size>&
			{
				return _Used;
			}

			bool full() const
			{
				return _Free_obj.empty();
			}

			const _Object* data() const
			{
				return _Objs.data();
			}

			_Object* alloc()
			{
				if (!_Free_obj.empty())
				{
					auto Index = _Free_obj.back();
					_Free_obj.pop_back();
					_Used.set(Index);
					return &_Objs[Index];
				}
				else
				{
					return nullptr;
				}
			}

			bool free(_Object* Object)
			{
				if (Object < _Objs.data())
				{
					return false;
				}
				size_t Index = Object - _Objs.data();
				if ((Index >= _Objs.size()) || !_Used.test(Index))
				{
					return false;
				}
				else
				{
					_Used.reset(Index);
					_Free_obj.push_back(Index);
					return true;
				}
			}

			bool operator < (const _Pool& Pool)
			{
				return _Objs.data() < Pool.Objects.data();
			}
			bool operator > (const _Pool& Pool)
			{
				return _Objs.data() > Pool.Objects.data();
			}
			bool operator == (const _Pool& Pool)
			{
				return _Objs.data() == Pool.Objects.data();
			}
			bool operator != (const _Pool& Pool)
			{
				return _Objs.data() != Pool.Objects.data();
			}
			bool operator <= (const _Pool& Pool)
			{
				return _Objs.data() <= Pool.Objects.data();
			}
			bool operator >= (const _Pool& Pool)
			{
				return _Objs.data() >= Pool.Objects.data();
			}

			bool operator < (const _Object* Object)
			{
				return Object >= _Objs.data() + _Objs.size();
			}
			bool operator > (const _Object* Object)
			{
				return Object < _Objs.data();
			}
			bool operator == (const _Object* Object)
			{
				return !(*this < Object || *this > Object);
			}
			bool operator <= (const _Object* Object)
			{
				return Object >= _Objs.data();
			}
			bool operator >= (const _Object* Object)
			{
				return Object > _Objs.data() + _Objs.size();
			}
			bool operator != (const _Object* Object)
			{
				return *this < Object || *this > Object;
			}

			friend _tstringstream& operator << (_tstringstream& _S, const Cs_FixedPool<_Object, _Init, _Size>& _X)
			{
				_S << _T("Size: ") << _X.size()
					<< _T("\tFree: ") << _X.free_cnt()
					<< _T("\tUsed: ") << _X.used_cnt()
					<< _T("\tAddr: ") << (void*)_X.data()
					<< _T("\tBits: ") << _X.bits()
					<< endl;
				return _S;
			}

		protected:
			vector<_Object>			_Objs;
			bitset<_Size>			_Used;
			vector<cint>			_Free_obj;
			RawPtr<_Init>			_Initor;
		};

		template<typename _Object, typename _Init = Cs_PoolInit<_Object>, size_t _Size = POOL_SIZE>
		class Cs_Pool : public Cs_Object
		{
		protected:
			using _PoolObj = Cs_FixedPool<_Object, _Init, _Size>;
			using _PoolPtr = auto_ptr<_PoolObj>;
			using _PoolList = vector<_PoolPtr>;

			typedef typename _PoolObj::_Pointer		_Key;
			typedef _PoolObj*						_Value;

			template <class _key, class _value>
			struct AVLTreeNode {
				_key key;
				_value value;

				AVLTreeNode* less;
				AVLTreeNode* greater;
				int balanceFactor;
			};

			template <class _key, class _value>
			struct AVLTreeAbstractor {
				typedef AVLTreeNode<_key, _value> tree_node;
				typedef tree_node* handle;
				typedef _key key;
				typedef size_t size;
				size pack_size;

				handle get_less(handle h) { return h->less; }
				void set_less(handle h, handle less) { h->less = less; }
				handle get_greater(handle h) { return h->greater; }
				void set_greater(handle h, handle greater) { h->greater = greater; }

				int get_balance_factor(handle h) { return h->balanceFactor; }
				void set_balance_factor(handle h, int bf) { h->balanceFactor = bf; }

				int compare_key_node(const key& k, handle h) { return *h->value == k ? 0 : *h->value > k ? -1 : 1; }
				int compare_node_node(handle ha, handle hb) { return *ha->value == *hb->value ? 0 : *ha->value < *hb->value ? -1 : 1; }

				static handle null() { return nullptr; }
			};

			typedef WTF::AVLTree<AVLTreeAbstractor<typename _Key, typename _Value>> _PoolTree;

		public:
			Cs_Pool()
			{

			}

			~Cs_Pool()
			{
				_PoolTree::Iterator iterator;
				iterator.start_iter_least(_Pool_tree);

				vector<_PoolTree::tree_node*> nodes;

				while (*iterator) {
					nodes.push_back(*iterator);
					++iterator;
				}
				_Pool_tree.purge();

				for (size_t i = 0; i < nodes.size(); ++i)
					delete(nodes[i]);
			}

			_Object* alloc()
			{
				RawPtr<_PoolObj> FreePool;
				for (auto & OwnPool : _Pool_list)
				{
					auto* CurrentPool = OwnPool.get();
					if (!CurrentPool->full())
					{
						FreePool = CurrentPool;
						break;
					}
				}
				if (!FreePool)
				{
					FreePool = new _PoolObj(&_Initor);
					_Pool_list.push_back(_PoolPtr(FreePool));
					auto node = new _PoolTree::tree_node;
					node->key = FreePool->data();
					node->value = FreePool;
					_Pool_tree.insert(node);
				}
				return FreePool->alloc();
			}

			bool free(_Object* Object)
			{
				auto Node = _Pool_tree.search(Object, _PoolTree::EQUAL);
				if (Node)
				{
					return Node->value->free(Object);
				}
				else
				{
					return false;
				}
			}

			friend _tstringstream& operator << (_tstringstream& _S, const Cs_Pool<_Object, _Init, _Size>& _X)
			{
				for (auto & _P : _X.PoolList)
				{
					_S << *_P;
				}
				return _S;
			}

			_Init* _Get_initializer()
			{
				return &_Initor;
			}

		protected:
			_PoolList		_Pool_list;
			_PoolTree		_Pool_tree;
			_Init			_Initor;
		};
	}
}

#endif