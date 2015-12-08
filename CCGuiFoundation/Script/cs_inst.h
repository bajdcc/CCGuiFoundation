#ifndef CS_INST
#define CS_INST

#include "stdafx.h"
#include <unordered_map>
#include "WTF/RefPtr.h"
#include "../GUI/cc_base.h"

using namespace std;
using namespace cc::base;

namespace cc
{
	namespace script
	{
		class Cs_Object : public Object
		{
		};

		class Cs_Interface : public Object
		{
		};

		enum Cs_InstType
		{
			i_int,
			i_float,
			i_string,

			i_local_ref,
			i_fixed_ref,
			i_field_ref,
			i_ref,
			i_func,

			i_neg,
			i_add,
			i_sub,
			i_mul,
			i_div,
			i_idiv,
			i_mod,
			i_and,
			i_or,
			i_xor,
			i_not,
			i_less,
			i_larger,
			i_less_equal,
			i_larger_equal,
			i_equal,
			i_not_equal,
			i_join,
			i_copy,
			i_length,

			i_index,
			i_index_rm,
			i_index_ref,
			i_range,
			i_range_rm,
			i_range_ref,
			i_rangerev,
			i_rangerev_rm,
			i_rangerev_ref,
			i_block,
			i_block_rm,
			i_block_ref,

			i_invoke,
			i_invoke_arr,
			i_assign,
			i_array,

			i_push,
			i_pop,
			i_jump,
			i_jump_true,
			i_jump_false,
			i_result,

			i_make_ctor,
			i_ctor_base,
			i_call_ctor,
			i_element,
			i_field,
			i_remove,
			i_copy_stack,
			i_insert_env,
			i_using_env,

			i_is_bool,
			i_is_int,
			i_is_numeric,
			i_is_value,
			i_is_func,
			i_is_env,
			i_is_external,
			i_is_array,
			i_is_ctor,
			i_is_null,
			i_is_fromCtor,

			i_raise_error,

			i_unknown
		};

		_tstringstream& operator << (_tstringstream& ss, Cs_InstType inst);

		struct Cs_Inst
		{
			Cs_Inst();
			Cs_Inst(Cs_InstType, _tstring, cint);

			Cs_InstType			_Ins;
			_tstring			_Param;
			cint				_Line;
			cint				_Int_param;
		};

		using Cs_InstList = vector<Cs_Inst>;

		class Cs_InstPage;
		class Cs_Func : public Cs_Object
		{
		public:
			Cs_Func(Cs_InstPage*);

			void								RegisterLabel(_tstring);
			void								ProcessLabel();

			vector<_tstring>					_Params;
			vector<cint>						_Param_handles;
			bool								_Var_param_cnt;
			Cs_InstList							_Inst_list;
			unordered_map<_tstring, cint>		_Labels;
			RawPtr<Cs_InstPage>					_Owner_page;
		};

		using Cs_FuncPtr = RefPtr<Cs_Func>;
		using Cs_FuncList = vector<Cs_FuncPtr>;

		class Cs_InstPage : public Cs_Object
		{
		public:
			Cs_InstPage();
			_tstring							ToString();
			void								ProcessLabel();

			Cs_FuncList							_Funcs;
			RawPtr<Cs_Func>						_Entry;
			_tstring							_Code;
		};

		_tstringstream& operator << (_tstringstream&, const Cs_InstPage&);

		using Cs_FreeInsPagePtr = RefPtr<Cs_InstPage>;
	}
}


#endif