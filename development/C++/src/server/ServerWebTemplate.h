#ifndef _SERVERWEBTEMPLATE_H_
#define _SERVERWEBTEMPLATE_H_

#include "../Enums.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


namespace SoftingOPCToolboxServer
{

class TBC_EXPORT WebTemplate
{

public:

	WebTemplate() {};

	virtual ~WebTemplate() {};

	virtual long handleWebTemplate(
		IN TCHAR* aTemplateName,
		IN unsigned long aNumArgs,
		IN TCHAR* *anArgs,
		tstring& aResult)
	{
		return E_NOTIMPL;
	};  //  end handleWebTemplate

};  //  end WebTemplate

}   //  ens namespace SoftingOPCToolboxServer

#pragma pack(pop)
#endif  // _WEBTEMPLATE_H_
