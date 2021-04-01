#ifndef _DATAPOINTELEMENT_H_
#define _DATAPOINTELEMENT_H_

#include "BaseDaElement.h"

using namespace SoftingOPCToolboxServer;



//-----------------------------------------------------------------------------
// DataPointTypeProperty
//-----------------------------------------------------------------------------
class DataPointElement : public BaseDaElement
{
public:
	DataPointElement(
		tstring& aName,
		int aDataTypeId,
		tstring& aDescription) :
		BaseDaElement()
	{
		setName(aName);
		setHasChildren(false);
		setIoMode(EnumIoMode_POLL);
		setAccessRights(EnumAccessRights_READWRITEABLE);
		setDatatype(VT_UI4);
		DateTime time;
		time.now();
		ValueQT value(Variant(aDescription.c_str()), EnumQuality_GOOD, time);
		addProperty(new DaConstantProperty(101, tstring(_T("Description")), tstring(_T("101")), value));
		addProperty(new DataPointTypeProperty((aDataTypeId), tstring(_T("DataPointType")), tstring(_T("DataPointType"))));
	} // end ctr


	virtual ~DataPointElement(
		void)
	{
	} // end dtor

	virtual long valueChanged(
		int aDataCv,
		int anInternalData,
		const tstring& anAddress,
		const tstring& aTextDescription)
	{
		ValueQT cacheValue;
		getCacheValue(cacheValue);

		if (cacheValue.getQuality() == EnumQuality_GOOD &&
			cacheValue.getData().intVal == aDataCv)
		{
			return (long)EnumResultCode_S_OK;
		} // end if

		DateTime time;
		time.now();
		cacheValue = ValueQT(Variant(aDataCv), EnumQuality_GOOD, time);
		// Notify value changed to the DA Element
		BaseDaElement::valueChanged(cacheValue);
		// Create a Data change event and fire it
		Variant cacheValueVariant = cacheValue.getData();
		DataEvent* dataEvent = new DataEvent(getItemId(), cacheValueVariant, anInternalData, anAddress, aTextDescription);
		tstringstream toString;
		toString << aDataCv;
		dataEvent->setMessage(toString.str());
		dataEvent->setSeverity(500);
		dataEvent->setActorId(tstring(_T("OPC User")));
		return dataEvent->fire();
	} // end valueChanged

	virtual unsigned int executeCommand(
		const tstring& anAddress,
		const tstring& aCommand,
		const tstring& aDescription)
	{
		return valueChanged(_tstoi(aCommand.c_str()), 0, anAddress, aDescription);
	} // end executeCommand

	virtual void simulate(
		void)
	{
		if (m_simulationOn == true)
		{
			int simulationCV = 0;
			ValueQT cacheValue;
			getCacheValue(cacheValue);
			simulationCV = cacheValue.getData().intVal + 1;

			if (simulationCV > 5)
			{
				simulationCV = 0;
			} // end if

			valueChanged(simulationCV, 0, tstring(_T("simulation Address")), tstring(_T("simulation Text Description")));
		} // end if

		BaseDaElement::simulate();
	} // end simulate
}; // end DataPointElement

#endif
