#include "stdafx.h"
#include "Function.h"
#include "OpcServer.h"
#include "EUEngineeringUnits.h"
#include <math.h>

extern unsigned long CAT_ID_DA_WRITE_SIMPLE;
extern unsigned long CAT_ID_DA_WRITE_TRACKING;

void VariableProperty::createAngle(DemoDaAddressSpaceElement* pElement,
								   tstring name,
								   tstring description,
								   tstring itemId,
								   tagEnumAccessRights accessRights)
{
	if (pElement)
	{
		DaProperty* angleProperty = new DaProperty();
		angleProperty->setId(6020);
		angleProperty->setName(name);
		angleProperty->setDescription(description);
		angleProperty->setItemId(itemId);
		angleProperty->setDatatype(VT_I2);
		angleProperty->setAccessRights(accessRights);
		pElement->addProperty(angleProperty);
	}
}

void VariableProperty::setAnalogEngineeringUnits(DemoDaAddressSpaceElement* pElement,
												 double lowLimit,
												 double highLimit,
												 tagEnumAccessRights accessRights)
{
	if (pElement)
	{
		DateTime now;
		now.now();
	
		tstring typeStr = _T("EUType");
		tstring typeItemId = _T("7");
		Variant aVariant((long)EnumEUType_ANALOG);
		ValueQT aValueQT(aVariant, EnumQuality_GOOD, now);
		DaConstantProperty* euType = new DaConstantProperty(
			7,
			typeStr,
			typeItemId,
			aValueQT);
		pElement->addProperty(euType);
		
		tstring highStr = _T("High EU");
		tstring highDescriptionStr = _T("High EU Description");
		tstring highItemId = _T("102");
		DaProperty* euHigh = new DaProperty();
		euHigh->setId(102);
		euHigh->setName(highStr);
		euHigh->setDescription(highDescriptionStr);
		euHigh->setItemId(highItemId);
		euHigh->setDatatype(VT_R8);
		euHigh->setAccessRights(accessRights);
		pElement->addProperty(euHigh);
		tstring lowStr = _T("Low EU");
		tstring lowDescriptionStr = _T("Low EU Description");
		tstring lowItemId = _T("103");
		DaProperty* euLow = new DaProperty();
		euLow->setId(103);
		euLow->setName(lowStr);
		euLow->setDescription(lowDescriptionStr);
		euLow->setItemId(lowItemId);
		euLow->setDatatype(VT_R8);
		euLow->setAccessRights(accessRights);
		pElement->addProperty(euLow);
	}
}

double VariableProperty::convertToRadian(double angle)
{
	double radianAngle = 0.0;
	// calc radian angle
	static double pi = 3.1415926535;
	radianAngle = (2 * pi) * ((double)angle / 360.0);
	return radianAngle;
}

SimulationVariable::SimulationVariable()
{
	tstring name = _T("automatic change");
	setName(name);
	setIoMode(EnumIoMode_REPORT);
	setDatatype(VT_BOOL);
	setAccessRights(EnumAccessRights_READWRITEABLE);
}

void SimulationVariable::handleReadRequest(DaRequest* pRequest)
{
	if (pRequest == NULL)
	{
		return;
	}

	SimulationVariable* pElement = NULL;
	pElement = dynamic_cast<SimulationVariable*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		ValueQT cacheValue;
		pElement->getCacheValue(cacheValue);
		pRequest->setValue(cacheValue);
		pRequest->setResult(S_OK);
	}
	else
	{
		//should never get here
		pRequest->setResult(E_FAIL);
	}
}

void SimulationVariable::handleWriteRequest(DaRequest* pRequest)
{
	if (pRequest == NULL)
	{
		return;
	}

	SimulationVariable* pElement = NULL;
	pElement = dynamic_cast<SimulationVariable*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		ValueQT* pValue = pRequest->getValue();
		pRequest->setResult(pElement->valueChanged(*pValue));
		g_simulationEnabled = pValue->getData().boolVal;
#ifdef TBC_OS_WINDOWS

		if (g_simulationChanged)
		{
			SetEvent(g_simulationChanged);
		}

#endif
#ifdef TBC_OS_LINUX
		g_events.signal(1);
#endif
	}
	else
	{
		//should never get here
		pRequest->setResult(E_FAIL);
	}
}

void SimulationVariable::init()
{
	DateTime now;
	now.now();
	Variant aVariant(true);
	ValueQT val(aVariant, EnumQuality_GOOD, now);
	valueChanged(val);
}

SleepIntervalVariable::SleepIntervalVariable()
{
	tstring name = _T("automatic change period");
	setName(name);
	setIoMode(EnumIoMode_REPORT);
	setDatatype(VT_UI4);
	setAccessRights(EnumAccessRights_READWRITEABLE);
}

void SleepIntervalVariable::init()
{
	DateTime now;
	now.now();
	unsigned int defaultValue = 1000;
	Variant aVariant(defaultValue);
	ValueQT val(aVariant, EnumQuality_GOOD, now);
	valueChanged(val);
}

void SleepIntervalVariable::handleReadRequest(DaRequest* pRequest)
{
	if (pRequest == NULL)
	{
		return;
	}

	SleepIntervalVariable* pElement = NULL;
	pElement = dynamic_cast<SleepIntervalVariable*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		ValueQT cacheValue;
		pElement->getCacheValue(cacheValue);
		pRequest->setValue(cacheValue);
		pRequest->setResult(S_OK);
	}
	else
	{
		//should never get here
		pRequest->setResult(E_FAIL);
	}
}

void SleepIntervalVariable::handleWriteRequest(DaRequest* pRequest)
{
	if (pRequest == NULL)
	{
		return;
	}

	SleepIntervalVariable* pElement = NULL;
	pElement = dynamic_cast<SleepIntervalVariable*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		ValueQT* pValue = pRequest->getValue();
		pRequest->setResult(pElement->valueChanged(*pValue));
		g_simulationThreadSleepInterval = pValue->getData().intVal;
#ifdef TBC_OS_WINDOWS

		if (g_simulationChanged)
		{
			SetEvent(g_simulationChanged);
		}

#endif
#ifdef TBC_OS_LINUX
		g_events.signal(1);
#endif
	}
	else
	{
		//should never get here
		pRequest->setResult(E_FAIL);
	}
}

Function::Function(IN tstring tagName)
{
	m_angle = 0;
	setName(tagName);
	setIoMode(EnumIoMode_REPORT);
	setAccessRights(EnumAccessRights_READABLE);
	setDatatype(VT_R8);
	// create angle property
	VariableProperty::createAngle(this,
								  tstring(_T("Angle")),
								  tstring(_T("Angle Description")),
								  tstring(_T("Angle")),
								  EnumAccessRights_READWRITEABLE);
	//create High Eu, Low EU,
	VariableProperty::setAnalogEngineeringUnits(this, -1.0, 1.0, EnumAccessRights_READABLE);
}

void Function::handleReadRequest(IN DaRequest* pRequest)
{
	if (pRequest == NULL)
	{
		return;
	}

	Function* pElement = NULL;
	pElement = dynamic_cast<Function*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		if (pRequest->getPropertyId() == 0)
		{
			// get address space element value take the Toolkit cache value
			ValueQT cacheValue;
			pElement->getCacheValue(cacheValue);
			pRequest->setValue(cacheValue);
			pRequest->setResult(S_OK);
		}
		else
		{
			//  the element's property will handle this request
			pElement->getPropertyValue(pRequest);
		}   //  end if ... else
	}
	else
	{
		// should never get here
		pRequest->setResult(E_FAIL);
	}
} // handleRequest

void Function::handleWriteRequest(DaRequest* pRequest)
{
	if (pRequest == NULL)
	{
		return;
	}

	Function* pElement = NULL;
	pElement = dynamic_cast<Function*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		if (pRequest->getPropertyId() == 0)
		{
			ValueQT* pValue = pRequest->getValue();
			pRequest->setResult(pElement->valueChanged(*pValue));
		}
		else if ((pRequest->getPropertyId() == 6020))
		{
			ValueQT* writeValue = pRequest->getValue();
			pElement->setValue(writeValue);
			pRequest->setResult(S_OK);
		}
		else
		{
			//should not get here for the moment
			pRequest->setResult(E_FAIL);
		}
	}
	else
	{
		//should never get here
		pRequest->setResult(E_FAIL);
	}
}

void Function::simulation(void)
{
}

void Function::setValue(ValueQT* pValue)
{
}

SinFunction::SinFunction() : Function(_T("sin"))
{
}

void SinFunction::simulation(void)
{
	m_angle++;

	if (m_angle >= 360)
	{
		m_angle = 0;
	}

	double value = ::sin(VariableProperty::convertToRadian((double)m_angle));
	DateTime now;
	now.now();
	Variant aVariant(value);
	ValueQT val(aVariant, EnumQuality_GOOD, now);
	this->valueChanged(val);
}

void SinFunction::setValue(ValueQT* pValue)
{
	if (pValue)
	{
		m_angle = pValue->getData().iVal;
		double value = ::sin(VariableProperty::convertToRadian((double)m_angle));
		DateTime now;
		now.now();
		Variant aVariant(value);
		ValueQT val(aVariant, EnumQuality_GOOD, now);
		this->valueChanged(val);
	}
}

CosFunction::CosFunction() : Function(_T("cos"))
{
}

void CosFunction::simulation(void)
{
	m_angle++;

	if (m_angle >= 360)
	{
		m_angle = 0;
	}

	double value = ::cos(VariableProperty::convertToRadian((double)m_angle));
	DateTime now;
	now.now();
	Variant aVariant(value);
	ValueQT val(aVariant, EnumQuality_GOOD, now);
	this->valueChanged(val);
}

void CosFunction::setValue(ValueQT* pValue)
{
	if (pValue)
	{
		m_angle = pValue->getData().iVal;
		double value = ::cos(VariableProperty::convertToRadian((double)m_angle));
		DateTime now;
		now.now();
		Variant aVariant(value);
		ValueQT val(aVariant, EnumQuality_GOOD, now);
		this->valueChanged(val);
	}
}

TanFunction::TanFunction() : Function(_T("tan"))
{
}

void TanFunction::simulation(void)
{
	m_angle++;

	if (m_angle >= 360)
	{
		m_angle = 0;
	}

	double value = ::tan(VariableProperty::convertToRadian((double)m_angle));
	DateTime now;
	now.now();
	Variant aVariant(value);
	ValueQT val(aVariant, EnumQuality_GOOD, now);
	this->valueChanged(val);
}

void TanFunction::setValue(ValueQT* pValue)
{
	if (pValue)
	{
		m_angle = pValue->getData().iVal;
		double value = ::tan(VariableProperty::convertToRadian((double)m_angle));
		DateTime now;
		now.now();
		Variant aVariant(value);
		ValueQT val(aVariant, EnumQuality_GOOD, now);
		this->valueChanged(val);
	}
}

AlarmSimulation::AlarmSimulation(tstring name , enum AlarmType alarmType)
{
	setName(name);
	setAccessRights(EnumAccessRights_READWRITEABLE);
	m_alarmType = alarmType;
	setDatatype(VT_BOOL);
	setIoMode(EnumIoMode_POLL);
}

void AlarmSimulation::init()
{
	DateTime time;
	time.now();
	Variant theValue;
	theValue.setBOOL(FALSE);
	ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
	valueChanged(aValueQT);
}

void AlarmSimulation::handleReadRequest(IN DaRequest* pRequest)
{
	if (pRequest == NULL)
	{
		return;
	}

	AlarmSimulation* pElement = NULL;
	pElement = dynamic_cast<AlarmSimulation*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		ValueQT cacheValue;
		pElement->getCacheValue(cacheValue);
		pRequest->setValue(cacheValue);
		pRequest->setResult(S_OK);
	}
	else
	{
		//should never get here
		pRequest->setResult(E_FAIL);
	}
}

void AlarmSimulation::handleWriteRequest(IN DaRequest* pRequest)
{
#ifdef TBC_OS_WINDOWS

	if (pRequest == NULL)
	{
		return;
	}

	AlarmSimulation* pElement = NULL;
	pElement = dynamic_cast<AlarmSimulation*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		ValueQT* pValue = pRequest->getValue();
		pRequest->setResult(pElement->valueChanged(*pValue));
		//now generate the event
		std::vector<Variant> attributeValues(1);
		attributeValues[0] = pValue->getData();
		tstring anActorID(_T("alarm.simulation"));
		tstring message(_T("Boolean value: "));

		if (attributeValues[0].boolVal == 0)
		{
			message += _T("FALSE");
		}
		else
		{
			message += _T("TRUE");
		}

		switch (pElement->m_alarmType)
		{
		case simple:
		{
			tstring name = _T("alarm simulation.fire simple event");
			AeEvent fireSimpleEvent(EnumEventType_SIMPLE, name, CAT_ID_DA_WRITE_SIMPLE);
			fireSimpleEvent.setSeverity(700);
			fireSimpleEvent.setActorId(anActorID);
			fireSimpleEvent.setMessage(message);
			fireSimpleEvent.setAttributeValueList(attributeValues);
			fireSimpleEvent.fire();
		}
		break;

		case tracking:
		{
			tstring name = _T("alarm simulation.fire tracking event");
			AeEvent fireTrackingEvent(EnumEventType_TRACKING, name, CAT_ID_DA_WRITE_TRACKING);
			fireTrackingEvent.setSeverity(750);
			fireTrackingEvent.setActorId(anActorID);
			fireTrackingEvent.setMessage(message);
			fireTrackingEvent.setAttributeValueList(attributeValues);
			fireTrackingEvent.fire();
		}
		break;
		}
	}
	else
	{
		//should never get here
		pRequest->setResult(E_FAIL);
	}

#endif
}

void AlarmSimulation::simulation()
{
}

KeyVariable::KeyVariable()
{
	tstring name = _T("key");
	setName(name);
	setAccessRights(EnumAccessRights_READABLE);
	setDatatype(VT_UI1);
	setIoMode(EnumIoMode_POLL);
}

void KeyVariable::init()
{
	DateTime time;
	time.now();
	Variant theValue;
	theValue.setUI1(0);
	ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
	valueChanged(aValueQT);
}

void KeyVariable::handleReadRequest(IN DaRequest* pRequest)
{
	if (pRequest == NULL)
	{
		return;
	}

	KeyVariable* pElement = NULL;
	pElement = dynamic_cast<KeyVariable*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		ValueQT cacheValue;
		pElement->getCacheValue(cacheValue);
		pRequest->setValue(cacheValue);
		pRequest->setResult(S_OK);
	}
	else
	{
		//should never get here
		pRequest->setResult(E_FAIL);
	}
}

void KeyVariable::handleWriteRequest(IN DaRequest* pRequest)
{
	if (pRequest == NULL)
	{
		return;
	}

	KeyVariable* pElement = NULL;
	pElement = dynamic_cast<KeyVariable*>(pRequest->getAddressSpaceElement());

	if (pElement)
	{
		ValueQT* pValue = pRequest->getValue();
		pRequest->setResult(pElement->valueChanged(*pValue));
	}
	else
	{
		//should never get here
		pRequest->setResult(E_FAIL);
	}
}
