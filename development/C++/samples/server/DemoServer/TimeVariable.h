#ifndef _TIME_VARIABLE_H_
#define _TIME_VARIABLE_H_

#include "DaAddressSpaceElement.h"
#include "Da/ServerDaAddressSpaceElement.h"
#include "Da/ServerDaProperty.h"

using namespace SoftingOPCToolboxServer;

class TimeVariable : public DemoDaAddressSpaceElement
{
public:

	enum TimeType
	{
		second,
		minute,
		hour,
		string,
		array,
		date,
		limitSecond
	};

	enum TimeZone
	{
		local,
		GMT,
		none
	};

	TimeVariable(IN tstring varName);
	TimeVariable(IN tstring varName, IN enum TimeType type, IN enum TimeZone timeZone);

	// request handling
	virtual void handleReadRequest(DaRequest* pRequest);

	virtual void handleWriteRequest(DaRequest* pRequest);

protected:
	enum TimeType m_timeType;
	enum TimeZone m_timeZone;

}; // SODmSTagTime

#endif
