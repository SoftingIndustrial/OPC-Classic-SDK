#include "OSCompat.h"
#include "ClientServerStatus.h"

using namespace SoftingOPCToolboxClient;

ServerStatus::ServerStatus()
{
	m_state             = 0;
	m_vendorInfo        = _T("");
	m_productVersion    = _T("");
	m_groupCount        = 0;
	m_bandwidth         = 0;
	m_statusInfo        = _T("");
} //end constructor
ServerStatus::ServerStatus(
	unsigned long state,
	DateTime startTime,
	DateTime currentTime,
	tstring vendorInfo,
	tstring productVersion,
	std::vector<tstring> supportedLcIds,
	DateTime lastUpdateTime,
	long groupCount,
	long bandwidth,
	tstring statusInfo)
{
	m_state          = state;
	m_startTime      = startTime;
	m_currentTime    = currentTime;
	m_vendorInfo     = vendorInfo;
	m_productVersion = productVersion;
	m_supportedLcIds = supportedLcIds;
	m_lastUpdateTime = lastUpdateTime,
	m_groupCount     = groupCount;
	m_bandwidth      = bandwidth;
	m_statusInfo     = statusInfo;
} //end constructor
ServerStatus::~ServerStatus()
{
} //end destructor

unsigned long ServerStatus::getState()
{
	return m_state;
} //end getState

DateTime ServerStatus::getStartTime()
{
	return m_startTime;
} //end getStartTime

DateTime ServerStatus::getCurrentTime()
{
	return m_currentTime;
} //end getCurrentTime

tstring& ServerStatus::getVendorInfo()
{
	return m_vendorInfo;
} //end getVendorInfo

tstring& ServerStatus::getProductVersion()
{
	return m_productVersion;
} //end getProductVersion

std::vector<tstring>& ServerStatus::getSupportedLcIds()
{
	return m_supportedLcIds;
} //end getSupportedLcIds

DateTime ServerStatus::getLastUpdateTime()
{
	return m_lastUpdateTime;
} //end getLastUpdateTime

long ServerStatus::getGroupCount()
{
	return m_groupCount;
} //end getGroupCount

long ServerStatus::getBandwidth()
{
	return m_bandwidth;
} //end getBandwidth

tstring& ServerStatus::getStatusInfo()
{
	return m_statusInfo;
} //end getStatusInfo
