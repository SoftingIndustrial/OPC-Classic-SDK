#include "OSCompat.h"
#include "ClientObjectSpaceElement.h"
#include "ClientApplication.h"

#include "OTClient.h"

using namespace SoftingOPCToolboxClient;


//-----------------------------------------------------------------------------
//	static members initialization region
//
unsigned long ObjectSpaceElement::m_objectIndex = 1;

ObjectSpaceElement::ObjectSpaceElement()
{
	m_index = (ObjectSpaceElement::m_objectIndex)++;
	m_objectHandle = 0;
} // end constructor


ObjectSpaceElement::~ObjectSpaceElement()
{
} // end destructor

BOOL ObjectSpaceElement::getValid()
{
	if (this->getHandle() != 0)
	{
		return TRUE;
	}   //  end if

	return FALSE;
}   //  end getValid

EnumObjectState ObjectSpaceElement::getTargetState()
{
	unsigned char currentState = 0;
	unsigned char targetState = 0;
	OTCGetState(
		m_objectHandle,
		&currentState,
		&targetState);
	return (EnumObjectState)targetState;
} // end getTargetState

EnumObjectState ObjectSpaceElement::getCurrentState()
{
	unsigned char currentState = 0;
	unsigned char targetState = 0;
	OTCGetState(
		m_objectHandle,
		&currentState,
		&targetState);
	return (EnumObjectState)currentState;
} // end CurrentState

void ObjectSpaceElement::setTargetState(EnumObjectState aState)
{
	OTCChangeTargetState(
		m_objectHandle,
		(unsigned char)aState,
		TRUE);
} //end setTargetState

long ObjectSpaceElement::performStateTransition(
	BOOL deep,
	ExecutionOptions* someExecutionOptions)
{
	OTCExecutionOptions options;
	long result = E_FAIL;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext  = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext  = 0;
		options.m_executionType     = EnumExecutionType_SYNCHRONOUS;
	} //end else

	if (deep)
	{
		result = OTCPerformStateTransition(
					 m_objectHandle,
					 TRUE,
					 &options);
	} //end if
	else
	{
		result = OTCPerformStateTransition(
					 m_objectHandle,
					 FALSE,
					 &options);
	} //end else

	return result;
}// end performStateTransition

long ObjectSpaceElement::connect(
	BOOL deep,
	BOOL active,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;

	if (deep)
	{
		if (active)
		{
			OTCChangeTargetState(
				m_objectHandle,
				EnumObjectState_ACTIVATED,
				TRUE);
			result = performStateTransition(
						 deep,
						 someExecutionOptions);
		} // end if

		if (!active)
		{
			OTCChangeTargetState(
				m_objectHandle,
				EnumObjectState_CONNECTED,
				TRUE);
			result = performStateTransition(
						 deep,
						 someExecutionOptions);
		} //end if
	} // end if

	if (!deep)
	{
		if (active)
		{
			OTCChangeTargetState(
				m_objectHandle,
				EnumObjectState_ACTIVATED,
				FALSE);
			result = performStateTransition(
						 deep,
						 someExecutionOptions);
		} // end if

		if (!active)
		{
			OTCChangeTargetState(
				m_objectHandle,
				EnumObjectState_CONNECTED,
				FALSE);
			result = performStateTransition(
						 deep,
						 someExecutionOptions);
		} //end if
	} // end if

	return result;
}


long ObjectSpaceElement::disconnect(ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	OTCChangeTargetState(
		m_objectHandle,
		EnumObjectState_DISCONNECTED,
		TRUE);
	result = performStateTransition(
				 TRUE,
				 someExecutionOptions);
	return result;
}

long ObjectSpaceElement::getAttributesFromServer(
	std::vector<EnumObjectAttribute> whatAttributes,
	std::vector<long> results,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;
	unsigned long count = (unsigned long) whatAttributes.size();
	unsigned long* pWhatAttributesToChange = new unsigned long[count];
	unsigned long resultCount = (unsigned long) results.size();
	long* pUpdateResults = new long[resultCount];
	unsigned long i;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	for (i = 0; i < count; i++)
	{
		pWhatAttributesToChange[i] = whatAttributes[i];
	}//end for

	for (i = 0; i < resultCount; i++)
	{
		pUpdateResults[i] = results[i];
	}//end for

	result = OTCUpdateAttributes(
				 m_objectHandle,
				 TRUE,
				 count,
				 pWhatAttributesToChange,
				 pUpdateResults,
				 &options);

	if (pUpdateResults)
	{
		for (i = 0; i < resultCount; i++)
		{
			results[i] = pUpdateResults[i];
		}//end for
	} // end if

	delete[] pUpdateResults;
	delete[] pWhatAttributesToChange;
	return result;
}//end getAttributesFromServer

long ObjectSpaceElement::setAttributesToServer(
	std::vector<EnumObjectAttribute> whatAttributes,
	std::vector<long> results,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	result = E_FAIL;
	OTCExecutionOptions options;
	unsigned long attributesCount = (unsigned long) whatAttributes.size();
	unsigned long* pWhatAttributesToChange = new unsigned long[attributesCount];
	unsigned long resultCount = (unsigned long) results.size();
	long* pUpdateResults = new long[resultCount];
	unsigned long i;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	for (i = 0; i < attributesCount; i++)
	{
		pWhatAttributesToChange[i] = whatAttributes[i];
	} //end for

	for (i = 0; i < resultCount; i++)
	{
		pUpdateResults[i] = results[i];
	} // end for

	result = OTCUpdateAttributes(
				 m_objectHandle,
				 FALSE,
				 attributesCount,
				 pWhatAttributesToChange,
				 pUpdateResults,
				 &options);

	for (i = 0; i < resultCount; i++)
	{
		results[i] = pUpdateResults[i];
	}//end for

	delete[] pUpdateResults;
	delete[] pWhatAttributesToChange;
	return result;
} //end setAttributesToServer

void ObjectSpaceElement::handleStateChangeCompleted(EnumObjectState newState)
{
	TCHAR* StateText[] =
	{
		_T("DISCONNECTED"),
		_T("CONNECTED"),
		_T("INVALID"),
		_T("ACTIVATED")
	};
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("ObjectSpaceElement::handleStateChangeCompleted"),
		StateText[newState]);
} // end handleStateChangeCompleted

void ObjectSpaceElement::onStateChange(EnumObjectState newState)
{
	handleStateChangeCompleted(newState);
} // end OnStateChange

void ObjectSpaceElement::handlePerformStateTransitionCompleted(
	unsigned long executionContext,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("ObjectSpaceElement::handlePerformStateTransitionCompleted"),
		_T(""));
} // end onPerormStateTransition

void ObjectSpaceElement::onPerformStateTransition(
	unsigned long executionContext,
	long result)
{
	handlePerformStateTransitionCompleted(
		executionContext,
		result);
}// end OnPerformStateTransition

void ObjectSpaceElement::handleGetAttributesFromServerCompleted(
	unsigned long executionContext,
	std::vector<EnumObjectAttribute>& whatAttributes,
	std::vector<long>& results,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("ObjectSpaceElement::handleGetAttributesFromServer"),
		_T(""));
} //end

//This was internal in C#
void ObjectSpaceElement::onGetAttributesFromServer(
	unsigned long executionContext,
	std::vector<EnumObjectAttribute>& whatAttributes,
	std::vector<long>& results,
	long result)
{
	handleGetAttributesFromServerCompleted(
		executionContext,
		whatAttributes,
		results,
		result);
} //end

void ObjectSpaceElement::handleSetAttributesToServerCompleted(
	unsigned long executionContext,
	std::vector<EnumObjectAttribute>& whatAttributes,
	std::vector<long>& results,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("ObjectSpaceElement::handleSetAttributesToServer"),
		_T(""));
} //end

//This was internal in C#
void ObjectSpaceElement::onSetAttributesToServer(
	unsigned long executionContext,
	std::vector<EnumObjectAttribute>& whatAttributes,
	std::vector<long>& results,
	long result)
{
	handleSetAttributesToServerCompleted(
		executionContext,
		whatAttributes,
		results,
		result);
} //end

