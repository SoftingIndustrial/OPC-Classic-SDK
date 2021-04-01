#ifndef _MYWEBTEMPLATE_H_
#define _MYWEBTEMPLATE_H_

#include "ServerWebTemplate.h"
#include <math.h>

using namespace SoftingOPCToolboxServer;



//-----------------------------------------------------------------------------
// MyWebTemplate
//-----------------------------------------------------------------------------
class MyWebTemplate : public WebTemplate
{
public:

	virtual long handleWebTemplate(
		IN TCHAR* templateName,
		IN unsigned long numArgs,
		IN TCHAR* *pArgs,
		OUT tstring& aResult)
	{
		if (_tcsncmp(templateName, _T("CURRENTTIME"), 11) == 0)
		{
			int isGerman = 0;

			if (numArgs == 1)
			{
				isGerman = (_tcscmp(pArgs[0], _T("GERMAN")) == 0);
			}

			DateTime st;
			st.now();

			if (isGerman)
			{
				tstringstream str;
				str << st.day() << _T("/") << st.month() << _T("/") << st.year() << _T(" ");
				str << st.hour() << _T(":") << st.minute() << _T(":") << st.second() << std::endl;
				aResult = str.str();
			}
			else
			{
				aResult = st.toString();
			} // end if .. else

			return S_OK;
		} // end if

		return E_NOTIMPL;
	}   //  end handleWebTemplate

};  //  end MyWebTemplate

#endif
