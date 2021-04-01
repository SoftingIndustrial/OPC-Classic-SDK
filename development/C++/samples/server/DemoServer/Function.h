#ifndef _FUNCTIONTAG_H_
#define _FUNCTIONTAG_H_

#include "DaAddressSpaceElement.h"
#include "Da/ServerDaRequest.h"

extern BOOL g_simulationEnabled;
extern unsigned int g_simulationThreadSleepInterval;

#ifdef TBC_OS_WINDOWS
extern HANDLE g_simulationChanged;
#endif

#ifdef TBC_OS_LINUX
#include "PosixEvents.h"
extern PosixEvents g_events;
#endif

class VariableProperty
{
public:

	static void createAngle(DemoDaAddressSpaceElement* pElement,
							tstring name,
							tstring description,
							tstring itemId,
							tagEnumAccessRights accessRights);

	static void setAnalogEngineeringUnits(DemoDaAddressSpaceElement* pElement,
										  double lowLimit,
										  double highLimit,
										  tagEnumAccessRights accessRights);

	static double convertToRadian(double angle);
};

class SimulationVariable : public DemoDaAddressSpaceElement
{
public:
	SimulationVariable();

	void init();

protected:
	virtual void handleReadRequest(IN DaRequest* pRequest);

	virtual void handleWriteRequest(IN DaRequest* pRequest);
};

class SleepIntervalVariable : public DemoDaAddressSpaceElement
{
public:
	SleepIntervalVariable();

	void init();

protected:
	virtual void handleReadRequest(IN DaRequest* pRequest);

	virtual void handleWriteRequest(IN DaRequest* pRequest);

};

// Function class
class Function : public DemoDaAddressSpaceElement
{

public:

	Function(IN tstring tagName);
	virtual void setValue(ValueQT* pValue);
	//  TODO: check
	unsigned int m_angle;

protected:

	// request handling
	virtual void handleReadRequest(IN DaRequest* pRequest);

	virtual void handleWriteRequest(IN DaRequest* pRequest);

	virtual void simulation(void);

};  // end Function

// Sinus function class
class SinFunction : public Function
{

public:

	SinFunction();

	virtual void simulation(void);

	virtual void setValue(ValueQT* pValue);

};  // end SinFunction

// Cosinus function class
class CosFunction : public Function
{

public:

	CosFunction();

	virtual void simulation(void);

	virtual void setValue(ValueQT* pValue);

};  // end CosFunction

// Tan function class
class TanFunction : public Function
{

public:

	TanFunction();

	virtual void simulation(void);

	virtual void setValue(ValueQT* pValue);

};  // end TanFunction


class AlarmSimulation : public DemoDaAddressSpaceElement
{
public:

	enum AlarmType
	{
		simple,
		tracking
	};

	AlarmSimulation(tstring name , enum AlarmType alarmType);

	void init();

	void simulation();

protected:
	virtual void handleReadRequest(IN DaRequest* pRequest);

	virtual void handleWriteRequest(IN DaRequest* pRequest);

public:
	enum AlarmType m_alarmType;
};

class KeyVariable : public DemoDaAddressSpaceElement
{
public:

	KeyVariable();

	void init();

protected:
	virtual void handleReadRequest(IN DaRequest* pRequest);

	virtual void handleWriteRequest(IN DaRequest* pRequest);

};

#endif
