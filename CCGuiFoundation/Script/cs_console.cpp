#include "stdafx.h"
#include "GUI\cc_exception.h"
#include "GUI\cc_console.h"
#include "cs_inst.h"
#include "cs_pool.h"
#include "cs_regex.h"

using namespace cc::exception;
using namespace cc::script;
using namespace cc::presentation::windows;
using namespace cc::presentation::element;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			void Console::ConsoleThread::ConsoleThreadCallback()
			{
				using cs_regex_ts = cc::script::regex::Cs_RegexAlgorithm_To_string<TCHAR>;
				using cs_regex_s = cc::script::regex::Cs_RegexAlgorithm_Semantic<TCHAR>;
				bool ext = true;
				try
				{
					Wait();
					CString str;
					while (true)
					{
						SetColorToDefault();
						Output(_T(">> "));
						SetColor(CColor(Gdiplus::Color::Gray));
						Input(str);
						SetColor(CColor(Gdiplus::Color::Orange));
						Output(_T("Result: \n"));
						SetColor(CColor(Gdiplus::Color::Brown));
						do
						{
							_tstring input = str;
							_tstringstream ss;
							if (input.compare(_T("e")) == 0)
							{
								ext = !ext;
								ss << _T("Regex extend mode = ") << boolalpha << ext << endl;
								Output(ss.str());
							}
							else
							{
								cs_regex_s s;
								bool success;
								ss << _T("Analysis = ") << boolalpha << (success = s._Regex_analysis(input, ext)) << endl;
								if (success)
								{
									cs_regex_ts ts;
									Output(ts._To_string(s._Get_result()));
									Output(_T("\n"));
								}
								else
								{
									SetColor(CColor(Gdiplus::Color::Red));
									Output(_T("Error\n"));
									ss.str();
									ss << _T("Location: ") << s._Get_error().line() << endl;
									ss << _T("Message: ") << s._Get_error().what() << endl;
									ss << _T("Reason: ") << cs_regex_s::Csex_RegexError::_To_string(s._Get_error().code()) << endl;
									Output(ss.str());
								}
							}
						} while (0);
					}
				}
				catch (const runtime_thread_interrupt& e)
				{
					ATLTRACE(atlTraceException, 0, "%s", e.what());
				}
			}
		}
	}
}