using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;
using System.IO;

namespace PerfClient
{
	public class OpcServer
	{

		#region Constructor
		//-----------------
		public OpcServer()
		{

		}   //	end constructor
			//--
		#endregion

		#region Protected Attributes
		//--------------------------

		protected ShutdownHandler m_shutdownRequest = null;
		protected MyDaAddressSpaceElement m_daSimulationElement = null;
		protected MyDaAddressSpaceElement m_daElement = null;
		protected Random m_random = new Random(1);
		protected MyCreator m_creator = null;
		protected DaAddressSpaceRoot m_daRoot = null;
		protected string m_configFile = null;
		protected string[] m_currentLine = null;
		protected Hashtable m_parentHash = null;

		//--
		#endregion

		#region Public Properties
		//-----------------------

		public string ServiceName
		{
			get
			{
				return Application.Instance.ServiceName;
			}
			set
			{
				Application.Instance.ServiceName = value;
			}
		}   //	end SetServiceName

		//--
		#endregion

		#region Public Methods
		//-----------------------

		public static int Shutdown()
		{
			return (int)EnumResultCode.S_OK;
		}   //	end Shutdown

		public int Initialize()
		{
			m_configFile = "ConfigFile.csv";
			m_currentLine = new string[10];
			m_parentHash = new Hashtable();


			try
			{
				Application.Instance.VersionOtb = 447;
				Application.Instance.AppType = EnumApplicationType.EXECUTABLE;

				Application.Instance.ClsidDa = "{3AD03AF9-AC2B-4D1D-81F9-194D03DD1A4E}";
				Application.Instance.ProgIdDa = "Softing.PerfClient_NET.DA.1";
				Application.Instance.VerIndProgIdDa = "Softing.PerfClient_NET.DA";
				Application.Instance.Description = "Softing PerfClient NET OPC Server";
				Application.Instance.MajorVersion = 4;
				Application.Instance.MinorVersion = 47;
				Application.Instance.PatchVersion = 1;
				Application.Instance.BuildNumber = 0;
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH";
				Application.Instance.MinUpdateRateDa = 1000;
				Application.Instance.ClientCheckPeriod = 30000;
				Application.Instance.AddressSpaceDelimiter = '/';
				Application.Instance.PropertyDelimiter = '.';

				Application.Instance.ShutdownRequest += new Softing.OPCToolbox.Server.ShutdownHandler(Shutdown);
			}
			catch (Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer::Initialize",
					exc.ToString());

				return (int)EnumResultCode.E_FAIL;
			}   //	end try...catch
			return (int)EnumResultCode.S_OK;
		}   //	end Initialize

		public int Start()
		{
			return Application.Instance.Start();
		}   //	end Start

		public int Stop()
		{
			return Application.Instance.Stop();
		}   //	end Stop

		public int Ready()
		{
			return Application.Instance.Ready();
		}   //	end Ready

		public int Terminate()
		{
			return Application.Instance.Terminate();
		}   //	end Terminate

		public int Prepare(MyCreator aMyCreator)
		{
			int result = (int)EnumResultCode.S_OK;
			//	TODO - binary license activation
			//	Fill in your binary license activation keys here
			//
			//	NOTE: you can activate one or all of the features at the same time

			//	activate the COM-DA Server feature
			//	result = Application.Instance.Activate(EnumFeature.DA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}
			//	END TODO - binary license activation

			result = Application.Instance.Initialize(aMyCreator);

			if (ResultCode.SUCCEEDED(result))
			{
				Application.Instance.EnableTracing(
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					"Trace.txt",
					100000000,
					0);
			}   //	end if

			return result;
		}   //	end Prepare

		public int ProcessCommandLine(string commandLine)
		{
			return Application.Instance.ProcessCommandLine(commandLine);
		}   //	end ProcessCommandLine

		public int BuildAddressSpace()
		{
			m_creator = (MyCreator)Application.Instance.Creator;
			m_daRoot = Application.Instance.DaAddressSpaceRoot;

			if (!ResultCode.SUCCEEDED(parseConfigFile()))
			{
				return (int)EnumResultCode.S_FALSE;
			}

			return (int)EnumResultCode.S_OK;


			//try
			//{
			//    MyCreator creator = (MyCreator)Application.Instance.Creator;

			//    //	DA				
			//    DaAddressSpaceRoot daRoot = Application.Instance.DaAddressSpaceRoot;
			//    m_daSimulationElement = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
			//    m_daSimulationElement.Name = "simulation";
			//    m_daSimulationElement.AccessRights = EnumAccessRights.READWRITEABLE;
			//    m_daSimulationElement.Datatype = typeof(System.Int32);				
			//    m_daSimulationElement.IoMode = EnumIoMode.POLL;				
			//    daRoot.AddChild(m_daSimulationElement);

			//    DaProperty property = new DaProperty();			
			//    property.Id = (int)EnumPropertyId.ITEM_DESCRIPTION;
			//    property.Name = "Description";
			//    property.Description = "Element Description";
			//    property.ItemId = property.Name;
			//    property.Datatype = typeof(string);
			//    property.AccessRights = EnumAccessRights.READABLE;
			//    m_daSimulationElement.AddProperty(property);

			//    m_daSimulationElement.ValueChanged(
			//        new ValueQT(
			//            m_random.Next(5000),
			//            EnumQuality.GOOD,
			//            DateTime.Now));				
			//}
			//catch(Exception exc)
			//{
			//    Trace(
			//        EnumTraceLevel.ERR,
			//        EnumTraceGroup.USER1,
			//        "OpcServer:BuildAddressSpace",
			//        exc.ToString());
			//    return (int)EnumResultCode.E_FAIL;
			//}	//	end try...catch

		}   //	end BuildAddressSpace

		public void ChangeSimulationValues()
		{
			System.Collections.ArrayList rootChildren;
			rootChildren = m_daRoot.GetChildren();

			foreach (MyDaAddressSpaceElement child in rootChildren)
			{
				if (child.HasChildren)
					ChangeSimulationValuesNode(child);
				else if (child.IsChanging)
				{
					if (!ResultCode.SUCCEEDED(child.changeValue()))
					{
						Trace(EnumTraceLevel.ERR,
								EnumTraceGroup.ALL,
								"OpcServer::ChangeSimulationValues",
								"Error in changing value. Data type unknown");
					}
				}
			}
		}   //	end ChangeSimulationValues

		public void Trace(
			EnumTraceLevel traceLevel,
			EnumTraceGroup traceGroup,
			string objectID,
			string message)
		{
			Application.Instance.Trace(
				traceLevel,
				traceGroup,
				objectID,
				message);
		}   //	end Trace


		//--
		#endregion

		#region Protected Methods
		//--

		protected int parseConfigFile()
		{
			StreamReader nsConfigFile;
			string line;
			bool isComment = false;
			bool stopParsing = false;

			try
			{
				nsConfigFile = new StreamReader(m_configFile);
				while ((line = nsConfigFile.ReadLine()) != null)
				{
					switch (line[0])
					{
						//set comment flag
						case ('#'):
							isComment = true;
							break;
						//prepare line by removing leading tab
						case ('\t'):
							line = line.Substring(1);
							break;
						//empty line is treated as comment        
						case ('\n'):
							isComment = true;
							break;
						default:
							break;
					}// end switch line prepare

					int i = 0;
					int startOfCell = 0;
					int endOfCell = line.IndexOf('\t');
					string parentId = "";
					string parentIdPart = "";
					while (endOfCell != -1 && !isComment && !stopParsing)
					{
						switch (i)
						{
							case 0:
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
								m_currentLine[i] = line.Substring(startOfCell, endOfCell - startOfCell);
								break;
							case 9:
								//do smth with the changing rate
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
								parentIdPart = line.Substring(startOfCell, endOfCell - startOfCell);
								//building up parentId finished	- empty cell detected
								if (parentIdPart == "")
								{
									stopParsing = true;
								}
								//continue building up parentId
								else
								{
									parentId += parentIdPart;
									parentId += "/";
								}
								break;
							default:
								break;
						}// end switch cell
						startOfCell = endOfCell + 1;
						i++;
						endOfCell = line.IndexOf('\t', startOfCell);
					}// end while cells to handle 
					if (!isComment)
					{
						if (stopParsing && parentId.Length != 0)
						{
							parentId = parentId.Substring(0, parentId.Length - 1);
							m_currentLine[9] = parentId;
						}
						else if (stopParsing)
							m_currentLine[9] = "";
						if (!ResultCode.SUCCEEDED(addAddressSpaceElement()))
						{
							Trace(EnumTraceLevel.ERR,
									EnumTraceGroup.ALL,
									"OpcServer::parseConfigFile",
									"Error in adding address space element");
							return (int)EnumResultCode.E_FAIL;
						}
					}
					isComment = false;
					stopParsing = false;
				}// end Readline ->EOF
			}// end try
			catch (Exception e)
			{
				System.Console.WriteLine("File could not be opened or parsing error!");
				Trace(EnumTraceLevel.ERR,
						EnumTraceGroup.ALL,
						"OpcServer::parseConfigFile",
						e.Message);
				return (int)EnumResultCode.E_FAIL;
			}
			return (int)EnumResultCode.S_OK;
		}// end parseConfigFile


		protected int addAddressSpaceElement()
		{
			Type dataType;
			EnumAccessRights accessRights;
			MyDaAddressSpaceElement parent;
			bool isChanging;
			bool isArray;
			bool hasChildren;
			//String tempParentID;

			if (m_currentLine[4] == "true")
				isArray = true;
			else
				isArray = false;

			switch (m_currentLine[2])
			{
				case "UI1":
				case "BYTE":
					if (isArray)
						dataType = typeof(System.Byte[]);
					else
						dataType = typeof(System.Byte);
					break;
				case "CHAR":
					if (isArray)
						dataType = typeof(System.SByte[]);
					else
						dataType = typeof(System.SByte);
					break;
				case "UI2":
				case "WORD":
					if (isArray)
						dataType = typeof(System.UInt16[]);
					else
						dataType = typeof(System.UInt16);
					break;
				case "INT":
					if (isArray)
						dataType = typeof(System.Int16[]);
					else
						dataType = typeof(System.Int16);
					break;
				case "UI4":
				case "DWORD":
					if (isArray)
						dataType = typeof(System.UInt32[]);
					else
						dataType = typeof(System.UInt32);
					break;
				case "DINT":
					if (isArray)
						dataType = typeof(System.Int32[]);
					else
						dataType = typeof(System.Int32);
					break;
				case "REAL":
				case "FLOAT":
					if (isArray)
						dataType = typeof(float[]);
					else
						dataType = typeof(float);
					//dataType = typeof(System.Double);
					break;
				case "BOOL":
					if (isArray)
						dataType = typeof(System.Boolean[]);
					else
						dataType = typeof(System.Boolean);
					break;
				case "DATE_AND_TIME":
					if (isArray)
						dataType = typeof(System.DateTime[]);
					else
						dataType = typeof(System.DateTime);
					break;
				case "STRING":
					if (isArray)
						dataType = typeof(System.String[]);
					else
						dataType = typeof(System.String);
					break;
				case "NONE":
					dataType = null;
					break;
				default:
					dataType = null;
					break;
			}//end switch data type

			switch (m_currentLine[3])
			{
				case "NONE":
					accessRights = EnumAccessRights.READABLE;
					break;
				case "R":
					accessRights = EnumAccessRights.READABLE;
					break;
				case "W":
					accessRights = EnumAccessRights.WRITEABLE;
					break;
				case "RW":
					accessRights = EnumAccessRights.READWRITEABLE;
					break;
				default:
					accessRights = EnumAccessRights.READABLE;
					break;
			}// end switch access rights

			if (m_currentLine[6] == "true")
				isChanging = true;
			else
				isChanging = false;

			if (m_currentLine[5] == "true")
				hasChildren = true;
			else
				hasChildren = false;

			m_daElement = (MyDaAddressSpaceElement)m_creator.CreateMyDaAddressSpaceElement();
			m_daElement.ItemId = m_currentLine[0];
			m_daElement.Name = m_currentLine[1];
			m_daElement.AccessRights = accessRights;
			m_daElement.Datatype = dataType;
			m_daElement.IsChanging = isChanging;
			m_daElement.IoMode = EnumIoMode.REPORT;
			m_daElement.HasChildren = hasChildren;

			//determine parent
			if (m_currentLine[9] == "")
			{
				if (!m_daRoot.AddChild(m_daElement))
				{
					Trace(EnumTraceLevel.ERR,
						  EnumTraceGroup.ALL,
						  "OpcServer::addAddressSpaceElement",
						  "Error in adding child to root node");
					return (int)EnumResultCode.E_FAIL;
				}
			}
			else
			{
				//tempParentID = m_currentLine[9];
				//tempParentID.Replace('/', '.');
				if (m_parentHash.ContainsKey(m_currentLine[9]))
				{
					parent = (MyDaAddressSpaceElement)m_parentHash[m_currentLine[9]];
					if (!parent.AddChild(m_daElement))
					{
						Trace(EnumTraceLevel.ERR,
							  EnumTraceGroup.ALL,
							  "OpcServer::addAddressSpaceElement",
							  "Error in adding child to his parent");
						return (int)EnumResultCode.E_FAIL;
					}
				}
				else
				{
					Trace(EnumTraceLevel.ERR,
						EnumTraceGroup.ALL,
						"OpcServer::addAddressSpaceElement",
						"Cannot find parent in hashmap");
					return (int)EnumResultCode.E_FAIL;
				}
			}

			if (m_daElement.HasChildren)
			{
				m_parentHash.Add(m_daElement.ItemId, m_daElement);
			}
			else
			{
				if (!ResultCode.SUCCEEDED(m_daElement.SetInitialValue(m_currentLine[7], m_currentLine[8])))
				{
					Trace(EnumTraceLevel.ERR,
						EnumTraceGroup.ALL,
						"OpcServer::parseConfigFile",
						"Error in setting initial value. Data type unknown");
					return (int)EnumResultCode.E_FAIL;
				}
			}
			return (int)EnumResultCode.S_OK;
		} //end addAddressSpaceElement

		protected void ChangeSimulationValuesNode(MyDaAddressSpaceElement node)
		{
			System.Collections.ArrayList nodeChildren;
			nodeChildren = node.GetChildren();

			foreach (MyDaAddressSpaceElement child in nodeChildren)
			{
				if (child.HasChildren)
					ChangeSimulationValuesNode(child);
				else if (child.IsChanging)
				{
					if (!ResultCode.SUCCEEDED(child.changeValue()))
					{
						Trace(EnumTraceLevel.ERR,
								EnumTraceGroup.ALL,
								"OpcServer::ChangeSimulationValuesNode",
								"Error in changing value. Data type unknown");
					}
				}
			}
		}   //	end ChangeSimulationValuesNode

		//--
		#endregion

	}   //	end OpcServer
}   //	end namespace
