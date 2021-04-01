#ifndef _H_EU_ENGINEERING_H
#define _H_EU_ENGINEERING_H

#include "Da/ServerDaRequest.h"
#include "Da/ServerDaProperty.h"

using namespace SoftingOPCToolboxServer;

// Engineering value property
class EUEngineeringUnits : public DaProperty
{

protected:
	ValueQT m_value;

public:
	EUEngineeringUnits(
		int anId,
		tstring& aName,
		tstring& anItemId,
		ValueQT& aValue);
	virtual ~EUEngineeringUnits();

	ValueQT& getValue();
	void setValue(ValueQT value);

};  //  end EUEngineeringUnits

#endif
