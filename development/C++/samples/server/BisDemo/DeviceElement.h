#ifndef _DEVICEELEMENT_H_
#define _DEVICEELEMENT_H_

#include "BaseDaElement.h"

using namespace SoftingOPCToolboxServer;



//-----------------------------------------------------------------------------
// DeviceElement
//-----------------------------------------------------------------------------
class DeviceElement : public BaseDaElement
{
public:
	DeviceElement(
		tstring& aName,
		int aDataTypeId,
		tstring& aDescription) : BaseDaElement()
	{
		setAccessRights(EnumAccessRights_READABLE);
		setDatatype(VT_UI4);
		setIoMode(EnumIoMode_REPORT);
		setName(aName);
		setHasChildren(false);
		setIoMode(EnumIoMode_POLL);
		DateTime time;
		time.now();
		ValueQT value(Variant(aDescription.c_str()), EnumQuality_GOOD, time);
		addProperty(new DaConstantProperty(101, tstring(_T("Description")), tstring(_T("Description")), value));
		addProperty(new DataPointTypeProperty(aDataTypeId));
	}   //  end ctr

	virtual ~DeviceElement(
		void)
	{
	} // end dtor

	bool getConnected(
		void)
	{
		ValueQT cacheValue;
		getCacheValue(cacheValue);

		if (cacheValue.getQuality() == EnumQuality_GOOD)
		{
			return (cacheValue.getData().intVal >= 1);
		}
		else
		{
			return false;
		}   //  end if
	} // end getConnected

	void setConnected(
		bool value)
	{
		ValueQT cacheValue;
		getCacheValue(cacheValue);
		Variant cacheValueVariant = cacheValue.getData();

		if (cacheValueVariant.intVal > 1)
		{
			cacheValueVariant.intVal = 1;
		} // end if

		if ((cacheValue.getQuality() == EnumQuality_GOOD) &&
			(cacheValueVariant.intVal >= 1) == value)
		{
			return;
		} // end if

		DateTime time;
		time.now();
		Variant aVariant;

		if (value == true)
		{
			aVariant.setINT(1);
		}
		else
		{
			aVariant.setINT(0);
		} // end if .. else

		cacheValue = ValueQT(aVariant, EnumQuality_GOOD, time);
		//  Notify value changed to the DA Element
		valueChanged(cacheValueVariant.intVal, 0, tstring(_T("simulation Address")), tstring(_T("simulation Text Description")));
		//  Create a State change event and fire it
		DeviceEvent deviceState(getItemId(), cacheValueVariant);
		deviceState.setActorId(tstring(_T("OPC User")));

		if (value == true)
		{
			deviceState.setMessage(tstring(_T("Connected")));
			deviceState.setSeverity(1);
		}
		else
		{
			deviceState.setMessage(tstring(_T("Disconnected")));
			deviceState.setSeverity(1000);
		} // end if .. else

		deviceState.fire();
	} // end setConnected

	virtual void simulate(
		void)
	{
		if (m_simulationOn == true)
		{
			ValueQT cacheValue;
			getCacheValue(cacheValue);
			bool simulationCV = (cacheValue.getData().intVal > 0);
			simulationCV = simulationCV ? false : true;
			setConnected(simulationCV);
		} // end if

		BaseDaElement::simulate();
	} // end simulate

	virtual long valueChanged(
		int aDataCv,
		int anInternalData,
		const tstring& anAddress,
		const tstring& aTextDescription)
	{
		ValueQT cacheValue;
		getCacheValue(cacheValue);
		Variant cacheValueVariant = cacheValue.getData();

		if (cacheValueVariant.intVal > 1)
		{
			cacheValueVariant.intVal = 1;
		} // end if

		if (aDataCv > 1)
		{
			aDataCv = 1;
		} // end if

		if (cacheValue.getQuality() == EnumQuality_GOOD &&
			cacheValueVariant.intVal == aDataCv)
		{
			return (long)EnumResultCode_S_OK;
		} // end if

		DateTime time;
		time.now();
		Variant aVariant;
		aVariant.setINT(aDataCv);
		cacheValue = ValueQT(aVariant, EnumQuality_GOOD, time);
		// Notify value changed to the DA Element
		BaseDaElement::valueChanged(cacheValue);
		// Create a Data change event and fire it
		cacheValueVariant = cacheValue.getData();
		DeviceEvent* deviceEvent = new DeviceEvent(getItemId(), cacheValueVariant);
		deviceEvent->setActorId(tstring(_T("OPC User")));

		if (aDataCv > 0)
		{
			deviceEvent->setMessage(tstring(_T("Connected")));
			deviceEvent->setSeverity(1);
		}
		else
		{
			deviceEvent->setMessage(tstring(_T("Disconnected")));
			deviceEvent->setSeverity(1000);
		} // end if .. else

		return deviceEvent->fire();
	} // end valueChanged

	virtual unsigned int executeCommand(
		const tstring& anAddress,
		const tstring& aCommand,
		const tstring& aDescription)
	{
		return valueChanged(_tstoi(aCommand.c_str()), 0, anAddress, aDescription);
	} // end executeCommand
}; // end DeviceElement

#endif
