#include "stdafx.h"
#include "OpcServer.h"
#include "ServerCommon.h"

OpcServer* instance = NULL;


OpcServer* getOpcServer(void)
{
	return instance;
} // end getOpcServer


void createOpcServer(void)
{
	if (instance == NULL)
	{
		instance = new OpcServer();
	} // end if
} // end createOpcServer


void destroyOpcServer(void)
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	} // end if
} // end destroyOpcServer


long API_CALL handleShutdownRequest(void)
{
	//  TODO: add your Appliction termination code here
	return S_OK;
} // end handleShutdownRequest


OpcServer::OpcServer(void)
{
	m_pDaElement = NULL;
} // end constructor


OpcServer::~OpcServer(void)
{
} // end destructor


long OpcServer::initialize(void)
{
	//determine path of exe, config file has same path
	TCHAR exePathC [MAX_PATH];
	GetModuleFileName(NULL, exePathC, MAX_PATH);
	tstring exePath = exePathC;
	//matches Windows and Unix paths
	size_t found = exePath.find_last_of(_T("/\\"));
	m_configFile = exePath.substr(0, found);
#ifdef TBC_OS_WINDOWS
	m_configFile += _T("\\ConfigFile.csv");
#endif
#ifdef TBC_OS_LINUX
	m_configFile += _T("/ConfigFile.csv");
#endif

	if (!SUCCEEDED(initializeMaps()))
	{
		return S_FALSE;
	}

	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidDa(_T("{83D89A9C-D1BB-4429-A276-1827CF40B421}"));
	getApp()->setProgIdDa(_T("Softing.PerfClient.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.PerfClient.DA"));
	getApp()->setDescription(_T("Softing PerfClient OPC Server"));
	getApp()->setIpPortHTTP(8095);
	getApp()->setUrlDa(_T("/OPC/DA"));
	getApp()->setMajorVersion(4);
	getApp()->setMinorVersion(47);
	getApp()->setPatchVersion(1);
	getApp()->setBuildNumber(0);
	getApp()->setVendorInfo(_T("Softing Industrial Automation GmbH"));
	getApp()->setMinUpdateRateDa(1000);
	getApp()->setClientCheckPeriod(30000);
	getApp()->setAddressSpaceDelimiter(_T('/'));
	getApp()->setPropertyDelimiter(_T('.'));
	getApp()->ShutdownRequest = handleShutdownRequest;
	// proceed with initializing of the Toolkit tracing mechanism
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		_T("Trace.txt"),
		-1,
		2);
	return S_OK;
} // end initialize

long OpcServer::initializeMaps()
{
	// Data types
	m_mapDataType[_T("UI1")] = VT_UI1;
	m_mapDataType[_T("UI2")] = VT_UI2;
	m_mapDataType[_T("UI4")] = VT_UI4;
	m_mapDataType[_T("INT")] = VT_I2;
	m_mapDataType[_T("CHAR")] = VT_I1;
	m_mapDataType[_T("REAL")] = VT_R4;
	m_mapDataType[_T("WORD")] = VT_UI2;
	m_mapDataType[_T("BYTE")] = VT_UI1;
	m_mapDataType[_T("BOOL")] = VT_BOOL;
	m_mapDataType[_T("DWORD")] = VT_UI4;
	m_mapDataType[_T("DINT")] = VT_I4;
	m_mapDataType[_T("DATE_AND_TIME")] = VT_DATE;
	m_mapDataType[_T("STRING")] = VT_BSTR;
	m_mapDataType[_T("NONE")] = VT_EMPTY;
	m_mapDataType[_T("FLOAT")] = VT_R4;
	// Array types
	//m_mapArrayType[_T("false")] = VT_SCALAR;
	//m_mapArrayType[_T("true")] = VT_ARRAY;
	//m_mapArrayType[""] = OSF_AT_Matrix;
	//Access levels
	m_mapAccessLevel[_T("NONE")] = EnumAccessRights_READABLE;
	m_mapAccessLevel[_T("R")] = EnumAccessRights_READABLE;
	m_mapAccessLevel[_T("W")] = EnumAccessRights_WRITEABLE;
	m_mapAccessLevel[_T("RW")] = EnumAccessRights_READWRITEABLE;
	return S_OK;
}// end initializeMaps

void OpcServer::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
} // end setServiceName

long OpcServer::prepare(MyCreator* creator)
{
	long result = S_OK;

	// TODO - binary license activation
	// Fill in your binary license activation keys here
	//
	// NOTE: you can activate one or all of the features at the same time

	// activate the COM-DA Server feature
	// result = getApp()->activate(EnumFeature_DA_SERVER, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));
	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the XML-DA Server Feature
	//result = getApp()->activate(EnumFeature_XMLDA_SERVER, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	// END TODO - binary license activation
	// a valid creator must be provided to the initialize
	result = getApp()->initialize(creator);

	if (!SUCCEEDED(result))
	{
		return result;
	}

	return result;
} // end prepare

long OpcServer::start(void)
{
	return getApp()->start();
} // end start

long OpcServer::ready(void)
{
	return getApp()->ready();
} // end ready

long OpcServer::stop(void)
{
	return getApp()->stop();
} // end stop

long OpcServer::terminate(void)
{
	long result = getApp()->terminate();
	releaseApp();
	return result;
} // end terminate

long OpcServer::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
} // end processCommandLine


long OpcServer::buildAddressSpace(void)
{
	m_creator = (MyCreator*)getApp()->getCreator();
	m_daRoot = getApp()->getDaAddressSpaceRoot();

	if (!SUCCEEDED(parseConfigFile(m_configFile)))
	{
		return S_FALSE;
	}

	return S_OK;
} // end buildAddressSpace


void OpcServer::changeSimulationValues(void)
{
	MyDaAddressSpaceElement* workingElement  = (MyDaAddressSpaceElement*) m_daRoot;
	std::vector<AddressSpaceElement*> rootChildren;
	std::vector<AddressSpaceElement*>::iterator it_rootChildren;
	rootChildren = workingElement->getChildren();

	for (it_rootChildren = rootChildren.begin(); it_rootChildren < rootChildren.end(); it_rootChildren++)
	{
		workingElement = (MyDaAddressSpaceElement*) *it_rootChildren;

		if (workingElement->getHasChildren())
		{
			processValueChangeNode(workingElement);
		}
		else
		{
			processValueChangeLeaf(workingElement);
		}
	}
} // end changeSimulationValues

void OpcServer::trace(
	EnumTraceLevel aLevel,
	EnumTraceGroup aMask,
	const TCHAR* anObjectID,
	const TCHAR* aMessage,
	...)
{
	getApp()->trace(aLevel, aMask, anObjectID, aMessage);
} // end trace

long OpcServer::parseConfigFile(tstring nsConfigFileName)
{
	tstring line;
	tstring cell;
	tstring parentId;
	tstring idString;
	tstring nameString;
	tstring dataTypeString;
	tstring accessLevelString;
	tstring nodeClassString;
	tstring arrayTypeString;
	tstring isChangingString;
	tstring arraySizeString;
	tstring initialValueString;
	bool isComment = false;
	bool stopParsing = false;
	int inChar;
	int i;
	size_t endOfCell;
	size_t startOfCell;
	//TODO: This is dirty, find a better way for unicode filename open
	//open file
#ifdef UNICODE
	WCHAR filebuf[250];
#endif
#ifndef UNICODE
	char filebuf[250];
#endif
	unsigned int ii;

	for (ii = 0; ii < 250 && ii < nsConfigFileName.length(); ii++)
	{
		filebuf[ii] = nsConfigFileName.at(ii);
	}

	filebuf[ii] = _T('\0');
#ifdef UNICODE
	std::wifstream nsConfigFile(_wfopen(filebuf, _T("r,ccs=UNICODE")));
#endif
#ifndef UNICODE
	std::ifstream nsConfigFile(fopen(filebuf, _T("r")));
#endif

	if (!nsConfigFile.is_open())
	{
		_tprintf(_T("Cannot open namespace file. Namespace not built up.\n"));
		trace(EnumTraceLevel_ERR,
			  EnumTraceGroup_ALL,
			  _T("_parseConfigFile"),
			  _T("Cannot open config file"));
		return S_FALSE;
	}

	getline(nsConfigFile, line);

	while (!nsConfigFile.eof())
	{
		inChar = line[0];

		switch (inChar)
		{
		case _T('#'):
			//ignore comment line
			isComment = true;
			break;

		case _T('\t'):

			//ignore first tab in line
			try
			{
				line = line.substr(1);
			}
			catch (std::exception&)
			{
				trace(EnumTraceLevel_WRN,
					  EnumTraceGroup_ALL,
					  _T("_parseConfigFile"),
					  _T("Error in parsing namespace file. Line seems to be empty, node not added"));
				break;
			}

		default:
			break;
		}// end switch line prepare

		// start line parsing
		i = 0;
		startOfCell = 0;
		stopParsing = false;
		parentId = _T("");
		endOfCell = line.find(_T('\t'), startOfCell);

		while (endOfCell != tstring::npos && !isComment && !stopParsing)
		{
			try
			{
				switch (i)
				{
				case 0:
					idString = line.substr(startOfCell, endOfCell - startOfCell);
					//replace delimiter
					//delimiterPosition = idString.find(_T('/'),0);
					//while (delimiterPosition != tstring::npos)
					//{
					//  idString[delimiterPosition] = _T('.');
					//  delimiterPosition = idString.find(_T('/'), delimiterPosition);
					//}
					break;

				case 1:
					nameString = line.substr(startOfCell, endOfCell - startOfCell);
					break;

				case 2:
					cell = line.substr(startOfCell, endOfCell - startOfCell);
					dataTypeString = cell.c_str();
					break;

				case 3:
					cell = line.substr(startOfCell, endOfCell - startOfCell);
					accessLevelString = cell.c_str();
					break;

				case 4:
					cell = line.substr(startOfCell, endOfCell - startOfCell);
					arrayTypeString = cell.c_str();
					break;

				case 5:
					cell = line.substr(startOfCell, endOfCell - startOfCell);
					nodeClassString = cell.c_str();
					break;

				case 6:
					cell = line.substr(startOfCell, endOfCell - startOfCell);
					isChangingString = cell.c_str();
					break;

				case 7:
					initialValueString = line.substr(startOfCell, endOfCell - startOfCell);
					break;

				case 8:
					arraySizeString = line.substr(startOfCell, endOfCell - startOfCell);
					break;

				case 9:
					//TODO: do smth with the changing rate
					break;

				case 10:
					//Do not consider the Root Node in configuration
					//Virtual Root Node is used and the configuration Root Node
					//does not appear in the NodeId
					break;

				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
					cell = line.substr(startOfCell, endOfCell - startOfCell);

					//building up parentId finished
					if (cell.compare(_T("")) == 0)
					{
						//remove last added '.'
						if (parentId.length() > 0)
						{
							parentId = parentId.substr(0, parentId.length() - 1);
						}

						stopParsing = true;
					}
					//continue building up parentId
					else
					{
						parentId.append(cell);
						//parentId.append(_T("."));
						parentId.append(_T("/"));
					}

					break;

				default:
					break;
				}// end switch variable assignment

				startOfCell = endOfCell + 1;
				i++;
				endOfCell = line.find(_T('\t'), startOfCell);
			} //end try
			catch (std::exception&)
			{
				trace(EnumTraceLevel_ERR,
					  EnumTraceGroup_ALL,
					  _T("_parseConfigFile"),
					  _T("Error in parsing namespace file. Copy or find operation not successful"));
				return S_FALSE;
			}
		}// end while line handling

		if (!isComment)
		{
			m_currentLine[0] = idString;
			m_currentLine[1] = nameString;
			m_currentLine[2] = dataTypeString;
			m_currentLine[3] = accessLevelString;
			m_currentLine[4] = arrayTypeString;
			m_currentLine[5] = nodeClassString;
			m_currentLine[6] = isChangingString;
			m_currentLine[7] = initialValueString;
			m_currentLine[8] = arraySizeString;
			m_currentLine[9] = parentId;

			if (!SUCCEEDED(addAddressSpaceElement()))
			{
				trace(EnumTraceLevel_ERR,
					  EnumTraceGroup_ALL,
					  _T("_parseConfigFile"),
					  _T("Error in adding address space element"));
				return S_FALSE;
			}
		}// end if isComment

		isComment = false;
		getline(nsConfigFile, line);
	}// end while EOF

	return S_OK;
}// end parseConfigFile

long OpcServer::addAddressSpaceElement(void)
{
	int mapSize;
	VARENUM dataType;
	EnumAccessRights accessLevel;
	MyDaAddressSpaceElement* parent;
	bool isChanging;
	//determine data type
	mapSize = (int) m_mapDataType.size();
	dataType = m_mapDataType[m_currentLine[2]];

	if (mapSize < (int) m_mapDataType.size())
	{
		trace(EnumTraceLevel_ERR,
			  EnumTraceGroup_ALL,
			  _T("_addAddressSpaceElement"),
			  _T("Error in adding address space element. Key not found in data type map"));
		return S_FALSE;
	}

	//determine access level
	mapSize = (int) m_mapAccessLevel.size();
	accessLevel = m_mapAccessLevel[m_currentLine[3]];

	if (mapSize < (int) m_mapAccessLevel.size())
	{
		trace(EnumTraceLevel_ERR,
			  EnumTraceGroup_ALL,
			  _T("_addAddressSpaceElement"),
			  _T("Error in adding address space element. Key not found in access level map"));
		return S_FALSE;
	}

	//determine changing property
	if (m_currentLine[6].compare(_T("true")) == 0)
	{
		isChanging = true;
	}
	else
	{
		isChanging = false;
	}

	//set address space element properties
	m_pDaElement = (MyDaAddressSpaceElement*)m_creator->createMyDaAddressSpaceElement();
	m_pDaElement->setName(m_currentLine[1]);
	m_pDaElement->setAccessRights(accessLevel);

	//is array?
	if (m_currentLine[4].compare(_T("true")) == 0)
	{
		m_pDaElement->setDatatype((VARENUM)(dataType | VT_ARRAY));
	}
	else
	{
		m_pDaElement->setDatatype(dataType);
	}

	m_pDaElement->setItemId(m_currentLine[0]);
	m_pDaElement->setIsChanging(isChanging);

	// set node/leaf property
	if (m_currentLine[5].compare(_T("true")) == 0)
	{
		m_pDaElement->setIoMode(EnumIoMode_REPORT);
		m_pDaElement->setHasChildren(true);
	}
	else
	{
		m_pDaElement->setIoMode(EnumIoMode_REPORT);
		m_pDaElement->setHasChildren(false);
	}

	//direct child of root node
	if (m_currentLine[9].compare(_T("")) == 0)
	{
		if (!m_daRoot->addChild(m_pDaElement))
		{
			trace(EnumTraceLevel_ERR,
				  EnumTraceGroup_ALL,
				  _T("_addAddressSpaceElement"),
				  _T("Error in adding address space element. Child can not be added to root"));
			return S_FALSE;
		}
	}
	//find parent in corresponding map
	else
	{
		mapSize = (int) m_mapParent.size();
		parent = m_mapParent[m_currentLine[9]];

		if (mapSize < (int) m_mapParent.size())
		{
			trace(EnumTraceLevel_ERR,
				  EnumTraceGroup_ALL,
				  _T("_addAddressSpaceElement"),
				  _T("Error in adding address space element. Parent not found in map"));
			return S_FALSE;
		}
		//add child to parent found
		else
		{
			if (!parent->addChild(m_pDaElement))
			{
				trace(EnumTraceLevel_ERR,
					  EnumTraceGroup_ALL,
					  _T("_addAddressSpaceElement"),
					  _T("Error in adding address space element. Child can not be added to map determinded parent"));
				return S_FALSE;
			}
		}
	}

	//add node with children to parent map
	if (m_pDaElement->getHasChildren())
	{
		m_mapParent[m_currentLine[0]] = m_pDaElement;
	}

	// set initial value
	if (!m_pDaElement->getHasChildren())
	{
		Variant aVariant;
		unsigned short tempUShort;
		signed short tempShort;
		unsigned int tempUInt;
		signed int tempInt;
		signed long tempLong;
		unsigned long tempULong;
		signed long long tempLongLong;
		unsigned long long tempULongLong;
		double tempDouble;
		double tempDATE;
		unsigned long arraySize;
		tstringstream tsArray(m_currentLine[8]);
		tsArray >> arraySize;
		//unsigned long arraySize = atol(m_currentLine[8].c_str());
		tstring initialValue;
		initialValue.assign(m_currentLine[7]);
		DateTime time;
		time.now();
		Variant dateVariant;
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = arraySize;
		tstringstream ts;

		switch (m_pDaElement->getDatatype())
		{
		case VT_BOOL:
			if (initialValue.length() > 0 && initialValue.compare(_T("true")) == 0)
			{
				aVariant.setBOOL(true);
			}
			else
			{
				aVariant.setBOOL(false);
			}

			break;

		case VT_I1:
			if (initialValue.length() > 0)
			{
				if ((SCHAR_MIN <= initialValue[0]) && (initialValue[0] <= SCHAR_MAX))
				{
					aVariant.setI1((signed char) initialValue[0]);
				}
				else
				{
					aVariant.init(VT_I1);
				}
			}
			else
			{
				aVariant.init(VT_I1);
			}

			break;

		case VT_UI1:
			if (initialValue.length() > 0)
			{
				if ((0 <= initialValue[0]) && (initialValue[0] <= UCHAR_MAX))
				{
					aVariant.setUI1((unsigned char) initialValue[0]);
				}
				else
				{
					aVariant.init(VT_UI1);
				}
			}
			else
			{
				aVariant.init(VT_UI1);
			}

			break;

		case VT_I2:
			ts << initialValue;
			ts >> tempShort;

			if (initialValue.length() > 0)
			{
				if (SHRT_MIN <= tempShort && tempShort <= SHRT_MAX)
				{
					aVariant.setI2((signed short) tempShort);
				}
				else
				{
					aVariant.init(VT_I2);
				}
			}
			else
			{
				aVariant.init(VT_I2);
			}

			break;

		case VT_UI2:
			ts << initialValue;
			ts >> tempUShort;

			if (initialValue.length() > 0)
			{
				if (0 <= tempUShort && tempUShort <= USHRT_MAX)
				{
					aVariant.setUI2((unsigned short) tempUShort);
				}
				else
				{
					aVariant.init(VT_UI2);
				}
			}
			else
			{
				aVariant.init(VT_UI2);
			}

			break;

		case VT_I4:
			ts << initialValue;
			ts >> tempLong;

			if (initialValue.length() > 0)
			{
				if (LONG_MIN <= tempLong && tempLong <= LONG_MAX)
				{
					aVariant.setI4((long) tempLong);
				}
				else
				{
					aVariant.init(VT_I4);
				}
			}
			else
			{
				aVariant.init(VT_I4);
			}

			break;

		case VT_UI4:
			ts << initialValue;
			ts >> tempULong;

			if (initialValue.length() > 0)
			{
				if (0 <= tempULong && tempULong <= ULONG_MAX)
				{
					aVariant.setUI4((unsigned long) tempULong);
				}
				else
				{
					aVariant.init(VT_UI4);
				}
			}
			else
			{
				aVariant.init(VT_UI4);
			}

			break;

		case VT_I8:
			ts << initialValue;
			ts >> tempLongLong;

			if (initialValue.length() > 0)
			{
				if (LLONG_MIN <= tempLongLong && tempLongLong <= LLONG_MAX)
				{
					aVariant.setI8((long long) tempLongLong);
				}
				else
				{
					aVariant.init(VT_I8);
				}
			}
			else
			{
				aVariant.init(VT_I8);
			}

			break;

		case VT_UI8:
			ts << initialValue;
			ts >> tempULongLong;

			if (initialValue.length() > 0)
			{
				if (0 <= tempULongLong && tempULongLong <= ULLONG_MAX)
				{
					aVariant.setUI8((unsigned long long) tempULongLong);
				}
				else
				{
					aVariant.init(VT_UI8);
				}
			}
			else
			{
				aVariant.init(VT_UI8);
			}

			break;

		case VT_R4:
			ts << initialValue;
			ts >> tempDouble;

			if (initialValue.length() > 0)
			{
				if (FLT_MIN <= tempDouble && tempDouble <= FLT_MAX)
				{
					aVariant.setR4((float) tempDouble);
				}
				else
				{
					aVariant.init(VT_R4);
				}
			}
			else
			{
				aVariant.init(VT_R4);
			}

			break;

		case VT_R8:
			ts << initialValue;
			ts >> tempDouble;

			if (initialValue.length() > 0)
			{
				if (DBL_MIN <= tempDouble && tempDouble <= DBL_MAX)
				{
					aVariant.setR8((double) tempDouble);
				}
				else
				{
					aVariant.init(VT_R8);
				}
			}
			else
			{
				aVariant.init(VT_R8);
			}

			break;

		case VT_INT:
			ts << initialValue;
			ts >> tempInt;

			if (initialValue.length() > 0)
			{
				if (INT_MIN <= tempInt && tempInt <= INT_MAX)
				{
					aVariant.setINT((int) tempInt);
				}
				else
				{
					aVariant.init(VT_INT);
				}
			}
			else
			{
				aVariant.init(VT_INT);
			}

			break;

		case VT_UINT:
			ts << initialValue;
			ts >> tempUInt;

			if (initialValue.length() > 0)
			{
				if (0 <= tempUInt && tempUInt <= UINT_MAX)
				{
					aVariant.setUINT((unsigned int) tempUInt);
				}
				else
				{
					aVariant.init(VT_UINT);
				}
			}
			else
			{
				aVariant.init(VT_UINT);
			}

			break;

		case VT_DATE:
			ts << initialValue;
			ts >> tempDouble;
			tempDATE = 0.0001;

			if (initialValue.length() > 0)
			{
				if (DBL_MIN <= tempDouble && tempDouble <= DBL_MAX)
				{
					aVariant.setDATE(tempDouble);
				}
				else
				{
					aVariant.init(VT_DATE);
				}
			}
			else
			{
				aVariant.init(VT_DATE);
			}

			break;

		case VT_CY:
			//TODO
			//not used atm
			break;

		case VT_BSTR:
			if (initialValue.length() > 0)
			{
				aVariant.setBSTR(initialValue.c_str());
			}
			else
			{
				aVariant.init(VT_BSTR);
			}

		case VT_VARIANT:
			//TODO
			//not used atm
			break;

		case VT_ARRAY | VT_I1:
		{
			void* byteArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_I1);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&byteArray);

			for (unsigned long i = 0; i < arraySize; i++)
			{
				char* pVal = ((char*)byteArray) + i;

				if ((SCHAR_MIN <= initialValue[0]) && (initialValue[0] <= SCHAR_MAX))
				{
					*pVal = (char) initialValue[0];
				}

				*pVal = (char)i;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_UI1:
		{
			void* uByteArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_UI1);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&uByteArray);

			for (unsigned long j = 0; j < arraySize; j++)
			{
				unsigned char* pVal = ((unsigned char*)uByteArray) + j;

				if ((0 <= initialValue[0]) && (initialValue[0] <= UCHAR_MAX))
				{
					*pVal = (unsigned char)initialValue[0];
				}

				*pVal = (unsigned char)j;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_I2:
		{
			void* shortArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_I2);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempShort;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&shortArray);

			for (unsigned long k = 0; k < arraySize; k++)
			{
				short* pVal = ((short*)shortArray) + k;

				if (initialValue.length() > 0 && SHRT_MIN <= tempShort && tempShort <= SHRT_MAX)
				{
					*pVal = (short) tempShort;
				}

				*pVal = (short)k;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_UI2:
		{
			void* uShortArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_UI2);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempUShort;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&uShortArray);

			for (unsigned long l = 0; l < arraySize; l++)
			{
				unsigned short* pVal = ((unsigned short*)uShortArray) + l;

				if (initialValue.length() > 0 && 0 <= tempUShort && tempUShort <= USHRT_MAX)
				{
					*pVal = (unsigned short) tempUShort;
				}

				*pVal = (unsigned short)l;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_I4:
		{
			void* longArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_I4);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempLong;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&longArray);

			for (unsigned long m = 0; m < arraySize; m++)
			{
				long* pVal = ((long*)longArray) + m;

				if (initialValue.length() > 0 && LONG_MIN <= tempLong && tempLong <= LONG_MAX)
				{
					*pVal = (long) tempLong;
				}

				*pVal = (long)m;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_UI4:
		{
			void* uLongArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_UI4);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempULong;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&uLongArray);

			for (unsigned long n = 0; n < arraySize; n++)
			{
				unsigned long* pVal = ((unsigned long*)uLongArray) + n;

				if (initialValue.length() > 0 && 0 <= tempULong && tempULong <= ULONG_MAX)
				{
					*pVal = tempULong;
				}

				*pVal = n;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_R4:
		{
			void* floatArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_R4);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempDouble;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&floatArray);

			for (unsigned long o = 0; o < arraySize; o++)
			{
				float* pVal = ((float*)floatArray) + o;

				if (initialValue.length() > 0 && FLT_MIN <= tempDouble && tempDouble <= FLT_MAX)
				{
					*pVal = (float) tempDouble;
				}

				*pVal = (float)o;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_BOOL:
		{
			void* boolArray = NULL;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_BOOL);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&boolArray);

			for (unsigned long q = 0; q < arraySize; q++)
			{
				bool* pVal = ((bool*)boolArray) + q;

				if (q % 2 == 0)
				{
					*pVal = true;
				}
				else
				{
					*pVal = false;
				} // end if .. else
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_BSTR:
		{
			void* strArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_BSTR);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&strArray);

			for (unsigned long r = 0; r < arraySize; r++)
			{
#ifdef TBC_OS_WINDOWS
				DWORD j = r % 26;
				BSTR* pVal = ((BSTR*)strArray) + r;

				if (initialValue.length() > 0)
				{
					WCHAR buffer[100];

					//strncpy(buffer, initialValue.c_str(), initialValue.length());
					for (int i = (int) initialValue.length(); i < 100; i++)
					{
						buffer[i] = 0;
					}

					*pVal = SysAllocString((WCHAR*)buffer);
				}
				else
				{
					unsigned short wstr[8];
					wstr[0] = L'S';
					wstr[1] = L'o';
					wstr[2] = L'f';
					wstr[3] = L't';
					wstr[4] = L'i';
					wstr[5] = L'n';
					wstr[6] = L'g';
					wstr[7] = 0;
					*pVal = SysAllocString((WCHAR*)wstr);
				}

#endif
#ifdef TBC_OS_LINUX
				DWORD j = r % 26;
				TCHAR** pVal = ((TCHAR**)strArray) + r;

				if (initialValue.length() > 0)
				{
					char buffer[100];
					strncpy(buffer, initialValue.c_str(), initialValue.length());

					for (int i = (int) initialValue.length(); i < 100; i++)
					{
						buffer[i] = 0;
					}

					*pVal = SysAllocString((TCHAR*)buffer);
				}
				else
				{
					TCHAR str[8];
					str[0] = _T('S') + (unsigned short)j;
					str[1] = _T('o') + (unsigned short)(j + 1);
					str[2] = _T('f') + (unsigned short)(j + 2);
					str[3] = _T('t') + (unsigned short)(j + 3);
					str[4] = _T('i') + (unsigned short)(j + 4);
					str[5] = _T('n') + (unsigned short)(j + 5);
					str[6] = _T('g') + (unsigned short)(j + 6);
					str[7] = 0;
					*pVal = OTSysAllocString((TCHAR*)str);
				}

#endif
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_DATE:
		{
			void* dateArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_DATE);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&dateArray);

			for (unsigned long t = 0; t < arraySize; t++)
			{
#ifdef TBC_OS_WINDOWS
				DATE* pVal = ((DATE*)dateArray) + t;
				time.get(*pVal);
#endif
#ifdef TBC_OS_LINUX
				timeval* pVal = ((timeval*)dateArray) + t;
				time.get(pVal);
#endif
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_R8:
		{
			void* doubleArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_R8);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempDouble;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&doubleArray);

			for (unsigned long o = 0; o < arraySize; o++)
			{
				double* pVal = ((double*)doubleArray) + o;

				if (initialValue.length() > 0 && DBL_MIN <= tempDouble && tempDouble <= DBL_MAX)
				{
					*pVal = (double) tempDouble;
				}

				*pVal = (double)o;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_I8:
		{
			void* LLongArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_UI8);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempLongLong;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&LLongArray);

			for (unsigned long n = 0; n < arraySize; n++)
			{
				long long* pVal = ((long long*)LLongArray) + n;

				if (initialValue.length() > 0 && LLONG_MIN <= tempLongLong && tempLongLong <= LLONG_MAX)
				{
					*pVal = tempLongLong;
				}

				*pVal = n;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_UI8:
		{
			void* uLLongArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_UI8);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempULongLong;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&uLLongArray);

			for (unsigned long n = 0; n < arraySize; n++)
			{
				unsigned long long* pVal = ((unsigned long long*)uLLongArray) + n;

				if (initialValue.length() > 0 && 0 <= tempULongLong && tempULongLong <= ULLONG_MAX)
				{
					*pVal = tempULongLong;
				}

				*pVal = n;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_INT:
		{
			void* intArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_INT);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempInt;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&intArray);

			for (unsigned long m = 0; m < arraySize; m++)
			{
				int* pVal = ((int*)intArray) + m;

				if (initialValue.length() > 0 && INT_MIN <= tempInt && tempInt <= INT_MAX)
				{
					*pVal = (int) tempInt;
				}

				*pVal = (int)m;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		case VT_ARRAY | VT_UINT:
		{
			void* uIntArray;
			aVariant.vt = (VARENUM)(VT_ARRAY | VT_UINT);
			aVariant.parray = Variant::safeArrayCreate(aVariant.vt & ~VT_ARRAY, 1, &dim);
			ts << initialValue;
			ts >> tempUInt;
			Variant::SafeArrayAccessData(aVariant.parray, (void**)&uIntArray);

			for (unsigned long m = 0; m < arraySize; m++)
			{
				unsigned int* pVal = ((unsigned int*)uIntArray) + m;

				if (initialValue.length() > 0 && 0 <= tempUInt && tempUInt <= UINT_MAX)
				{
					*pVal = (unsigned int) tempUInt;
				}

				*pVal = (unsigned int)m;
			} // end for

			Variant::SafeArrayUnaccessData(aVariant.parray);
		}
		break;

		default:
			trace(EnumTraceLevel_ERR,
				  EnumTraceGroup_ALL,
				  _T("_addAddressSpaceElement"),
				  _T("Error in setting initial value. Cannot determine data type"));
			return S_FALSE;
			break;
		}

		ValueQT value(aVariant, EnumQuality_GOOD, time);
		m_pDaElement->valueChanged(value);
	}

	return S_OK;
} // end addAddressSpaceElement

long OpcServer::processValueChangeNode(MyDaAddressSpaceElement* nodeToChange)
{
	MyDaAddressSpaceElement* workingElement;
	std::vector<AddressSpaceElement*> nodeChildren;
	std::vector<AddressSpaceElement*>::iterator it_nodeChildren;
	nodeChildren = nodeToChange->getChildren();

	for (it_nodeChildren = nodeChildren.begin(); it_nodeChildren < nodeChildren.end(); it_nodeChildren++)
	{
		workingElement = (MyDaAddressSpaceElement*) *it_nodeChildren;

		if (workingElement->getHasChildren())
		{
			processValueChangeNode(workingElement);
		}
		else
		{
			processValueChangeLeaf(workingElement);
		}
	}

	return S_OK;
} // end processValueChangeNode

long OpcServer::processValueChangeLeaf(MyDaAddressSpaceElement* leafToChange)
{
	ValueQT value;
	DateTime now;
	now.now();

	if (leafToChange->getIsChanging())
	{
		switch (leafToChange->getDatatype())
		{
		case VT_I1:
		{
			leafToChange->getCacheValue(value);
			value.getData().cVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_UI1:
		{
			leafToChange->getCacheValue(value);
			value.getData().bVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_I2:
		{
			leafToChange->getCacheValue(value);
			value.getData().iVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_UI2:
		{
			leafToChange->getCacheValue(value);
			value.getData().uiVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_I4:
		{
			leafToChange->getCacheValue(value);
			value.getData().lVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_UI4:
		{
			leafToChange->getCacheValue(value);
			value.getData().ulVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_INT:
		{
			leafToChange->getCacheValue(value);
			value.getData().intVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_UINT:
		{
			leafToChange->getCacheValue(value);
			value.getData().uintVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_I8:
		{
			leafToChange->getCacheValue(value);
			value.getData().llVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_UI8:
		{
			leafToChange->getCacheValue(value);
			value.getData().ullVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_R4:
		{
			leafToChange->getCacheValue(value);
			value.getData().fltVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_R8:
		{
			leafToChange->getCacheValue(value);
			value.getData().dblVal++;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_BOOL:
		{
			leafToChange->getCacheValue(value);
			value.getData().boolVal = !(value.getData().boolVal);
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_BSTR:
		{
			leafToChange->getCacheValue(value);
			BSTR w = (value.getData().bstrVal);
#ifdef TBC_OS_WINDOWS

			if (w)
			{
				while (*w != 0)
				{
					(*w)++;

					if (*w == OLESTR('Z') + 1)
					{
						*w = OLESTR('A');
					}

					if (*w == OLESTR('z') + 1)
					{
						*w = OLESTR('a');
					}

					if (*w == OLESTR('9') + 1)
					{
						*w = OLESTR('0');
					}

					if (!(((*w >= OLESTR('A')) && (*w <= OLESTR('Z'))) ||
						  ((*w >= OLESTR('a')) && (*w <= OLESTR('z'))) ||
						  ((*w >= OLESTR('0')) && (*w <= OLESTR('9')))))
					{
						*w = L' ';
					}

					w++;
				}
			}

#endif
#ifdef TBC_OS_LINUX

			if (w)
			{
				while (*w != 0)
				{
					(*w)++;

					if (*w == ('Z') + 1)
					{
						*w = ('A');
					}

					if (*w == ('z') + 1)
					{
						*w = ('a');
					}

					if (*w == ('9') + 1)
					{
						*w = ('0');
					}

					if (!(((*w >= ('A')) && (*w <= ('Z'))) ||
						  ((*w >= ('a')) && (*w <= ('z'))) ||
						  ((*w >= ('0')) && (*w <= ('9')))))
					{
						*w = _TCHAR(' ');
					}

					w++;
				}
			}

#endif
			//          value.getData().bstrVal = w;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_DATE:
		{
			leafToChange->getCacheValue(value);
			DATE* pDate = &(value.getData().date);

			if (pDate)
			{
#ifdef TBC_OS_WINDOWS
				*pDate += 1.0 / (24 * 3600);
#else // TBC_OS_WINDOWS
				++pDate->tv_sec;
#endif
			}

			value.getData().date = *pDate;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_CY:
		{
			leafToChange->getCacheValue(value);
			value.getData().cyVal;
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case VT_VARIANT:
		{
			leafToChange->getCacheValue(value);
			ValueQT val(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(val);
		}
		break;

		case (VT_ARRAY | VT_I1):
		case (VT_ARRAY | VT_UI1):
		case (VT_ARRAY | VT_I2):
		case (VT_ARRAY | VT_UI2):
		case (VT_ARRAY | VT_I4):
		case (VT_ARRAY | VT_UI4):
		case (VT_ARRAY | VT_INT):
		case (VT_ARRAY | VT_UINT):
		case (VT_ARRAY | VT_R4):
		case (VT_ARRAY | VT_R8):
		case (VT_ARRAY | VT_BOOL):
		case (VT_ARRAY | VT_BSTR):
		case (VT_ARRAY | VT_DATE):
		case (VT_ARRAY | VT_CY):
		case (VT_ARRAY | VT_VARIANT):
		{
			leafToChange->getCacheValue(value);
			LONG lBound = 0;
			LONG uBound = 0;
			LONG idx;
			void* voidArray = 0;
			Variant::SafeArrayGetLBound(value.getData().parray, 1, &lBound);
			Variant::SafeArrayGetUBound(value.getData().parray, 1, &uBound);
			Variant::SafeArrayAccessData(value.getData().parray, (void**)&voidArray);

			for (idx = lBound; idx <= uBound; idx++)
			{
				VARENUM type = (VARENUM)(value.getData().vt & ~VT_ARRAY);

				switch (type)
				{
				case VT_I1:
				{
					CHAR* cVal = ((CHAR*)voidArray) + idx;
					(*cVal)++;
				}
				break;

				case VT_UI1:
				{
					BYTE* bVal = ((BYTE*)voidArray) + idx;
					(*bVal)++;
				}
				break;

				case VT_I2:
				{
					SHORT* iVal = ((SHORT*)voidArray) + idx;
					(*iVal)++;
				}
				break;

				case VT_UI2:
				{
					USHORT* uiVal = ((USHORT*)voidArray) + idx;
					(*uiVal)++;
				}
				break;

				case VT_I4:
				{
					LONG* lVal = ((LONG*)voidArray) + idx;
					(*lVal)++;
				}
				break;

				case VT_UI4:
				{
					ULONG* ulVal = ((ULONG*)voidArray) + idx;
					(*ulVal)++;
				}
				break;

				case VT_INT:
				{
					int* iVal = ((int*)voidArray) + idx;
					(*iVal)++;
				}
				break;

				case VT_UINT:
				{
					unsigned int* uiVal = ((unsigned int*)voidArray) + idx;
					(*uiVal)++;
				}
				break;

				case VT_R4:
				{
					FLOAT* fltVal = ((FLOAT*)voidArray) + idx;
					(*fltVal)++;
				}
				break;

				case VT_R8:
				{
					DOUBLE* dblVal = ((DOUBLE*)voidArray) + idx;
					(*dblVal)++;
				}
				break;

				case VT_BOOL:
				{
					VARIANT_BOOL* boolVal = ((VARIANT_BOOL*)voidArray) + idx;
					(*boolVal) = !(*boolVal);
				}
				break;

				case VT_BSTR:
				{
					BSTR* bstrVal = ((BSTR*)voidArray) + idx;
					BSTR w = *bstrVal;
#ifdef TBC_OS_WINDOWS

					if (w)
					{
						while (*w != 0)
						{
							(*w)++;

							if (*w == OLESTR('Z') + 1)
							{
								*w = OLESTR('A');
							}

							if (*w == OLESTR('z') + 1)
							{
								*w = OLESTR('a');
							}

							if (*w == OLESTR('9') + 1)
							{
								*w = OLESTR('0');
							}

							if (!(((*w >= OLESTR('A')) && (*w <= OLESTR('Z'))) ||
								  ((*w >= OLESTR('a')) && (*w <= OLESTR('z'))) ||
								  ((*w >= OLESTR('0')) && (*w <= OLESTR('9')))))
							{
								*w = L' ';
							}

							w++;
						}
					}

#endif
#ifdef TBC_OS_LINUX

					if (w)
					{
						while (*w != 0)
						{
							(*w)++;

							if (*w == ('Z') + 1)
							{
								*w = ('A');
							}

							if (*w == ('z') + 1)
							{
								*w = ('a');
							}

							if (*w == ('9') + 1)
							{
								*w = ('0');
							}

							if (!(((*w >= ('A')) && (*w <= ('Z'))) ||
								  ((*w >= ('a')) && (*w <= ('z'))) ||
								  ((*w >= ('0')) && (*w <= ('9')))))
							{
								*w = _TCHAR(' ');
							}

							w++;
						}
					}

#endif
					//                      value.getData().bstrVal = w;
				}
				break;

				case VT_DATE:
				{
					DATE* dateVal = ((DATE*)voidArray) + idx;
					DATE* pDate = dateVal;

					if (pDate)
					{
#ifdef TBC_OS_WINDOWS
						*pDate += 1.0 / (24 * 3600);
#else // TBC_OS_WINDOWS
						++pDate->tv_sec;
#endif
					}
				}
				break;

				case VT_CY:
					//TODO
					// not used atm
					break;

				case VT_VARIANT:
					//TODO
					// not used atm
					break;

				default:
				{
					// nothing to do
				}
				break;
				}
			}

			Variant::SafeArrayUnaccessData(value.getData().parray);
			ValueQT aValueQT(value.getData(), EnumQuality_GOOD, now);
			leafToChange->valueChanged(aValueQT);
		}
		break;

		default:
		{
			//nothing to do
		}
		break;
		}// end switch
	}// end if changing

	return S_OK;
} // end processValueChangeLeaf
