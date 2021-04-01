#ifndef _DATAPOINTTYPEPROPERTY_H_
#define _DATAPOINTTYPEPROPERTY_H_

#include "Da/ServerDaProperty.h"
#include "OSCompat.h"

using namespace SoftingOPCToolboxServer;

static const int DATAPOINT_TYPE_PROPERTY_ID = 5001;
static tstring DATAPOINT_TYPE_PROPERTY_NAME = _T("DataPointType");

//-----------------------------------------------------------------------------
// DataPointTypeProperty
//-----------------------------------------------------------------------------
class DataPointTypeProperty : public DaProperty
{
public:
	DataPointTypeProperty(
		int aDataPointTypeId) :
		DaProperty()
	{
		setId(DATAPOINT_TYPE_PROPERTY_ID);
		tstringstream toString;
		toString << DATAPOINT_TYPE_PROPERTY_ID;
		setItemId(toString.str());
		setName(datapointTypePropertyName());
		setDescription(datapointTypePropertyName());
		setDatatype(VT_I4);
		setAccessRights(EnumAccessRights_READABLE);
		m_dataPointTypeValue = aDataPointTypeId;
	} // end ctr

	DataPointTypeProperty(
		int aDataPointTypeId,
		tstring& aName) :
		DaProperty()
	{
		setId(DATAPOINT_TYPE_PROPERTY_ID);
		tstringstream toString;
		toString << DATAPOINT_TYPE_PROPERTY_ID;
		setItemId(toString.str());
		setName(aName);
		setDescription(aName);
		setDatatype(VT_I4);
		setAccessRights(EnumAccessRights_READABLE);
		m_dataPointTypeValue = aDataPointTypeId;
	} // end ctr

	DataPointTypeProperty(
		int aDataPointTypeId,
		tstring& aName,
		tstring& aDescription)
	{
		setId(DATAPOINT_TYPE_PROPERTY_ID);
		setName(aName);
		tstringstream toString;
		toString << DATAPOINT_TYPE_PROPERTY_ID;
		setItemId(toString.str());
		setDescription(aDescription);
		setDatatype(VT_I4);
		setAccessRights(EnumAccessRights_READABLE);
		m_dataPointTypeValue = aDataPointTypeId;
	}  // end ctr

	virtual void handleReadRequest(
		DaRequest& aRequest)
	{
		//  Meant to be obverriden, just return failed
		aRequest.setResult(EnumResultCode_S_OK);
		DateTime time;
		time.now();
		ValueQT aValue(Variant(m_dataPointTypeValue), EnumQuality_GOOD, time);
		aRequest.setValue(aValue);
		aRequest.complete();
	}   //  end HandleReadRequest

	tstring& datapointTypePropertyName(
		void)
	{
		return DATAPOINT_TYPE_PROPERTY_NAME;
	} // end datapointTypePropertyName

protected:
	int m_dataPointTypeValue;

};  //  end DataPointTypeProperty

#endif
