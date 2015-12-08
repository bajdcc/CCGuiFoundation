#include "stdafx.h"
#include "cs_inst.h"

namespace cc
{
	namespace script
	{
		_tstringstream& operator << (_tstringstream& _S, Cs_InstType _In)
		{
			switch (_In)
			{
				case i_int:
					_S << (_T("int"));
					break;
				case i_float:
					_S << (_T("float"));
					break;
				case i_string:
					_S << (_T("string"));
					break;
				case i_local_ref:
					_S << (_T("local_ref"));
					break;
				case i_fixed_ref:
					_S << (_T("fixed_ref"));
					break;
				case i_field_ref:
					_S << (_T("field_ref"));
					break;
				case i_ref:
					_S << (_T("ref"));
					break;
				case i_func:
					_S << (_T("func"));
					break;
				case i_neg:
					_S << (_T("neg"));
					break;
				case i_add:
					_S << (_T("add"));
					break;
				case i_sub:
					_S << (_T("sub"));
					break;
				case i_mul:
					_S << (_T("mul"));
					break;
				case i_div:
					_S << (_T("div"));
					break;
				case i_idiv:
					_S << (_T("idiv"));
					break;
				case i_mod:
					_S << (_T("mod"));
					break;
				case i_and:
					_S << (_T("and"));
					break;
				case i_or:
					_S << (_T("or"));
					break;
				case i_xor:
					_S << (_T("xor"));
					break;
				case i_not:
					_S << (_T("not"));
					break;
				case i_less:
					_S << (_T("less"));
					break;
				case i_larger:
					_S << (_T("larger"));
					break;
				case i_less_equal:
					_S << (_T("less_equal"));
					break;
				case i_larger_equal:
					_S << (_T("larger_equal"));
					break;
				case i_equal:
					_S << (_T("equal"));
					break;
				case i_not_equal:
					_S << (_T("not_equal"));
					break;
				case i_join:
					_S << (_T("join"));
					break;
				case i_copy:
					_S << (_T("copy"));
					break;
				case i_length:
					_S << (_T("length"));
					break;
				case i_index:
					_S << (_T("index"));
					break;
				case i_index_rm:
					_S << (_T("index_rm"));
					break;
				case i_index_ref:
					_S << (_T("index_ref"));
					break;
				case i_range:
					_S << (_T("range"));
					break;
				case i_range_rm:
					_S << (_T("range_rm"));
					break;
				case i_range_ref:
					_S << (_T("range_ref"));
					break;
				case i_rangerev:
					_S << (_T("rangerev"));
					break;
				case i_rangerev_rm:
					_S << (_T("rangerev_rm"));
					break;
				case i_rangerev_ref:
					_S << (_T("rangerev_ref"));
					break;
				case i_block:
					_S << (_T("block"));
					break;
				case i_block_rm:
					_S << (_T("block_rm"));
					break;
				case i_block_ref:
					_S << (_T("block_ref"));
					break;
				case i_invoke:
					_S << (_T("invoke"));
					break;
				case i_invoke_arr:
					_S << (_T("invoke_arr"));
					break;
				case i_assign:
					_S << (_T("assign"));
					break;
				case i_array:
					_S << (_T("array"));
					break;
				case i_push:
					_S << (_T("push"));
					break;
				case i_pop:
					_S << (_T("pop"));
					break;
				case i_jump:
					_S << (_T("jump"));
					break;
				case i_jump_true:
					_S << (_T("jump_true"));
					break;
				case i_jump_false:
					_S << (_T("jump_false"));
					break;
				case i_result:
					_S << (_T("result"));
					break;
				case i_make_ctor:
					_S << (_T("make_ctor"));
					break;
				case i_ctor_base:
					_S << (_T("ctor_base"));
					break;
				case i_call_ctor:
					_S << (_T("call_ctor"));
					break;
				case i_element:
					_S << (_T("element"));
					break;
				case i_field:
					_S << (_T("field"));
					break;
				case i_remove:
					_S << (_T("remove"));
					break;
				case i_copy_stack:
					_S << (_T("copy_stack"));
					break;
				case i_insert_env:
					_S << (_T("insert_env"));
					break;
				case i_using_env:
					_S << (_T("using_env"));
					break;
				case i_is_bool:
					_S << (_T("is_bool"));
					break;
				case i_is_int:
					_S << (_T("is_int"));
					break;
				case i_is_numeric:
					_S << (_T("is_numeric"));
					break;
				case i_is_value:
					_S << (_T("is_value"));
					break;
				case i_is_func:
					_S << (_T("is_func"));
					break;
				case i_is_env:
					_S << (_T("is_env"));
					break;
				case i_is_external:
					_S << (_T("is_external"));
					break;
				case i_is_array:
					_S << (_T("is_array"));
					break;
				case i_is_ctor:
					_S << (_T("is_ctor"));
					break;
				case i_is_null:
					_S << (_T("is_null"));
					break;
				case i_is_fromCtor:
					_S << (_T("is_fromCtor"));
					break;
				case i_raise_error:
					_S << (_T("raise_error"));
					break;
				case i_unknown:
					_S << (_T("unknown"));
					break;
				default:
					break;
			}
			return _S;
		}


		Cs_Inst::Cs_Inst()
		{

		}
		Cs_Inst::Cs_Inst(Cs_InstType _In, _tstring _Pr, cint _Li)
			: _Ins(_In)
			, _Param(_Pr)
			, _Line(_Li)
			, _Int_param(0)
		{

		}


		Cs_Func::Cs_Func(Cs_InstPage* _Pa)
			: _Owner_page(_Pa)
			, _Var_param_cnt(false)
		{

		}

		void Cs_Func::RegisterLabel(_tstring _La)
		{
			_Labels.insert(make_pair(_La, (cint)_Inst_list.size()));
		}

		void Cs_Func::ProcessLabel()
		{
			for (auto & Inst : _Inst_list)
			{
				switch (Inst._Ins)
				{
					case i_jump:
					case i_jump_true:
					case i_jump_false:
						Inst._Int_param = _Labels[Inst._Param];
						break;
					case i_int:
					case i_func:
					case i_invoke:
					case i_array:
					case i_copy_stack:
						Inst._Int_param = _ttoi(Inst._Param.c_str());
						break;
				}
			}
		}


		Cs_InstPage::Cs_InstPage()
		{

		}

		_tstringstream& operator << (_tstringstream& ss, const Cs_InstPage& inst)
		{
			auto EntryIndex = find(inst._Funcs.begin(), inst._Funcs.end(), ~inst._Entry);
			if (EntryIndex != inst._Funcs.end())
			{
				ss << _T("Entry") << _T(':') << _T(' ') << *EntryIndex << endl;
			}
			else
			{
				ss << _T("Entry") << _T(':') << _T(' ') << _T("Unknown") << endl;
			}
			cuint i = 0;
			for (auto & Func : inst._Funcs)
			{
				ss << endl;
				ss << _T("func") << _T(' ') << i;
				if (Func->_Var_param_cnt)
					ss << _T(' ') << _T("varparam");
				ss << endl;
				for (auto & Param : Func->_Params)
				{
					ss << _T(' ') << _T(' ') << Param << endl;
				}
				ss << _T("begin") << endl;
				cuint j = 0;
				for (auto & InstList : Func->_Inst_list)
				{
					for (auto & Label : Func->_Labels)
					{
						if (Label.second == j)
						{
							ss << Label.first << _T(' ') << _T(':') << endl;
						}
					}
					if (j < Func->_Inst_list.size())
					{
						ss << _T(' ') << _T(' ') << _T('[') << _T("Line") << _T(':') << _T(' ')
							<< (InstList._Line + 1) << _T("]") << _T('\t') 
							<< InstList._Ins
							<< InstList._Param
							<< endl;
					}
					j++;
				}
				ss << _T("end") << endl;
				i++;
			}
			return ss;
		}

		void Cs_InstPage::ProcessLabel()
		{
			for (auto & Func : _Funcs)
			{
				Func->ProcessLabel();
			}
		}

	}
}
