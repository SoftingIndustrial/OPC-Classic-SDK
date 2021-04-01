using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Threading;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace TestServer
{
	public class OpcServer
	{
		internal static object LockTransactionForWrite = new object();

		#region Constructor
		//-----------------
		public OpcServer()
		{

		}	//	end constructor
		//--
		#endregion

		#region Protected Attributes
		//--------------------------

		protected ShutdownHandler m_shutdownRequest = null;

		protected DataGenerator m_generator = new DataGenerator();
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
		}	//	end SetServiceName


		public Form1 MainForm { get; set; }

		//--
		#endregion

		#region Public Methods
		//-----------------------

		public int Shutdown()
		{
			if (MainForm != null)
			{
				try
				{
					MainForm.Close();
				}
				catch
				{
				}
			}

			return (int)EnumResultCode.S_OK;
		}	//	end Shutdown

		public int Initialize()
		{
			try
			{
				Application.Instance.VersionOtb = 447;
				Application.Instance.AppType = EnumApplicationType.EXECUTABLE;

				Application.Instance.ClsidDa = "{E4D228D9-1442-4DC9-BFB6-026215D5D302}";
				Application.Instance.ProgIdDa = "Softing.COM TestServer.DA.1";
				Application.Instance.VerIndProgIdDa = "Softing.COM TestServer.DA";
				Application.Instance.Description = "Softing COM TestServer OPC Server";
				Application.Instance.IpPortHTTP = 8078;
				Application.Instance.UrlDa = "/OPC/DA";
				Application.Instance.MajorVersion = 4;
				Application.Instance.MinorVersion = 47;
				Application.Instance.PatchVersion = 1;
				Application.Instance.BuildNumber = 0;
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH";
				Application.Instance.MinUpdateRateDa = 100;
				Application.Instance.ClientCheckPeriod = 30000;
				Application.Instance.AddressSpaceDelimiter = '.';
				Application.Instance.PropertyDelimiter = '/';

				Application.Instance.ShutdownRequest += new Softing.OPCToolbox.Server.ShutdownHandler(Shutdown);
			}
			catch(Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer::Initialize",
					exc.ToString());

				return (int)EnumResultCode.E_FAIL;
			}	//	end try...catch
			return (int)EnumResultCode.S_OK;
		}	//	end Initialize

		public int Start()
		{
			return Application.Instance.Start();
		}	//	end Start

		public int Stop()
		{
			return Application.Instance.Stop();
		}	//	end Stop

		public int Ready()
		{
			return Application.Instance.Ready();
		}	//	end Ready

		public int Terminate()
		{
			return Application.Instance.Terminate();
		}	//	end Terminate

		public int Prepare(MyCreator aMyCreator)
		{
			int result = (int)EnumResultCode.S_OK;
			//	TODO - binary license activation
			//	Fill in your binary license activation keys here
			//
			//	NOTE: you can activate one or all of the features at the same time

			//	activate the COM-DA Server feature
            result = Application.Instance.Activate(EnumFeature.DA_SERVER, "0f00-0000-0000-f97a-21d8");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}
            //	END TODO - binary license activation
            //	activate the XML-DA Server Feature
            result = Application.Instance.Activate(EnumFeature.XMLDA_SERVER, "0f20-0000-0000-07ea-d600");
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
					3);
			}	//	end if

			return result;
		}	//	end Prepare

		public int ProcessCommandLine(string commandLine)
		{
			return Application.Instance.ProcessCommandLine(commandLine);
		}	//	end ProcessCommandLine

		public int BuildAddressSpace()
		{
			try
			{
				MyCreator creator = (MyCreator)Application.Instance.Creator;

				//	DA
				DaAddressSpaceRoot daRoot = Application.Instance.DaAddressSpaceRoot;

				#region CTT

				// Create the structure of nodes required for UA CTT Testing

				AddressSpaceElement cttFolder = creator.CreateMyDaAddressSpaceElement();
				cttFolder.Name = "CTT";
				cttFolder.HasChildren = true;
				if (!daRoot.AddChild(cttFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				AddressSpaceElement staticFolder = creator.CreateMyDaAddressSpaceElement();
				staticFolder.Name = "Static";
				staticFolder.HasChildren = true;
				if (!cttFolder.AddChild(staticFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				AddressSpaceElement allProfilesFolder = creator.CreateMyDaAddressSpaceElement();
				allProfilesFolder.Name = "All Profiles";
				allProfilesFolder.HasChildren = true;
				if (!staticFolder.AddChild(allProfilesFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				MyDaAddressSpaceElement tag;

				#region Static Scalar elements for all data Types

				AddressSpaceElement scalarFolder = creator.CreateMyDaAddressSpaceElement();
				scalarFolder.Name = "Scalar";
				scalarFolder.HasChildren = true;
				if (!allProfilesFolder.AddChild(scalarFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Double", typeof(System.Double));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Boolean", typeof(System.Boolean));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Byte", typeof(System.Byte));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Date Time", typeof(System.DateTime));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Float", typeof(System.Single));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Guid", typeof(System.Guid));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int16", typeof(System.Int16));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int32", typeof(System.Int32));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int64", typeof(System.Int64));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("String", typeof(System.String));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Sbyte", typeof(System.SByte));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				// tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("ByteString", typeof(System.Byte[]));
				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("ByteString", typeof(System.Byte[][]));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				// tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.Byte), 10, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt16", typeof(System.UInt16));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt32", typeof(System.UInt32));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt64", typeof(System.UInt64));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("XmlElement", typeof(System.Xml.XmlElement));
				if (!scalarFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomValueQT(tag.Datatype));
				#endregion

				#region Static Array elements for all data Types

				AddressSpaceElement arraysFolder = creator.CreateMyDaAddressSpaceElement();
				arraysFolder.Name = "Arrays";
				arraysFolder.HasChildren = true;
				if (!allProfilesFolder.AddChild(arraysFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				int arrayLength = 10;

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Double", typeof(System.Double[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				// Set the first value of the array to a value smaller than 10e14 for CTT testing
				double[] doubleArrayVal = m_generator.GetRandomArray(typeof(System.Double), false, arrayLength, true) as double[];
				doubleArrayVal[0] %= 10E+14;
				tag.ValueChanged(new ValueQT(doubleArrayVal, EnumQuality.GOOD, DateTime.Now));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Boolean", typeof(System.Boolean[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.Boolean), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Byte", typeof(System.Byte[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.Byte), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Date Time", typeof(System.DateTime[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.DateTime), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Float", typeof(System.Single[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				// Set the first value of the array to a value smaller than 10e5 for CTT testing
				float[] floatArrayVal = m_generator.GetRandomArray(typeof(System.Single), false, arrayLength, true) as float[];
				floatArrayVal[0] %= 0xf10E + 5;
				tag.ValueChanged(new ValueQT(floatArrayVal, EnumQuality.GOOD, DateTime.Now));


				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Guid", typeof(System.Guid[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.Guid), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int16", typeof(System.Int16[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.Int16), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int32", typeof(System.Int32[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.Int32), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int64", typeof(System.Int64[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.Int64), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("String", typeof(System.String[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.String), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Sbyte", typeof(System.SByte[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.SByte), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("ByteString", typeof(System.Byte[][]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.Byte), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt16", typeof(System.UInt16[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.UInt16), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt32", typeof(System.UInt32[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.UInt32), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt64", typeof(System.UInt64[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.UInt64), arrayLength, true));

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("XmlElement", typeof(System.Xml.XmlElement[]));
				if (!arraysFolder.AddChild(tag))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				tag.ValueChanged(m_generator.GetRandomArrayValueQT(typeof(System.Xml.XmlElement), arrayLength, true));

				#endregion

				AddressSpaceElement daFolder = creator.CreateMyDaAddressSpaceElement();
				daFolder.Name = "DA Profile";
				daFolder.HasChildren = true;
				if (!staticFolder.AddChild(daFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				#region DataItem elements for AllProfiles folder

				AddressSpaceElement dataItemFolder = creator.CreateMyDaAddressSpaceElement();
				dataItemFolder.Name = "DataItem";
				dataItemFolder.HasChildren = true;
				if (!daFolder.AddChild(dataItemFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				MyDaAddressSpaceElement dataItem;
				// Sbyte
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("SByte", typeof(System.SByte));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				DaProperty valuePrecision = new DaProperty();
				valuePrecision.Id = 111;
				valuePrecision.Name = "ValuePrecision";
				valuePrecision.Description = "ValuePrecision";
				valuePrecision.ItemId = valuePrecision.Id.ToString();
				valuePrecision.Datatype = typeof(System.Double);
				valuePrecision.AccessRights = EnumAccessRights.READABLE;
				dataItem.AddProperty(valuePrecision);

				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// Byte
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Byte", typeof(System.Byte));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// Int16
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int16", typeof(System.Int16));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// UInt16
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt16", typeof(System.UInt16));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// Int32
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int32", typeof(System.Int32));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// UInt32
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt32", typeof(System.UInt32));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// Int64
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int64", typeof(System.Int64));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// UInt64
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt64", typeof(System.UInt64));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// Double
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Double", typeof(System.Double));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// Float
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Float", typeof(System.Single));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// String
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("String", typeof(System.String));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				// DateTime
				dataItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("DateTime", typeof(System.DateTime));
				if (!dataItemFolder.AddChild(dataItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				dataItem.AddProperty(valuePrecision);
				dataItem.ValueChanged(m_generator.GetRandomValueQT(dataItem.Datatype));

				#endregion

				#region AnalogType elements for AllProfiles folder

				AddressSpaceElement analogTypeFolder = creator.CreateMyDaAddressSpaceElement();
				analogTypeFolder.Name = "AnalogType";
				analogTypeFolder.HasChildren = true;
				if (!daFolder.AddChild(analogTypeFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				MyDaAddressSpaceElement analogTypeItem;

				// Sbyte
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("SByte", typeof(SByte));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				DaConstantProperty euTypeProperty = new DaConstantProperty(
					(int)EnumPropertyId.ITEM_EU_TYPE,
					"EUType",
					"7",
					new ValueQT(EnumEUType.ANALOG, EnumQuality.GOOD, DateTime.Now));
				euTypeProperty.AccessRights = EnumAccessRights.READABLE;
				analogTypeItem.AddProperty(euTypeProperty);

				DaProperty euHighProperty = new DaProperty();
				euHighProperty.Id = (int)EnumPropertyId.HIGH_EU;
				euHighProperty.Name = "High EU";
				euHighProperty.Description = "High EU";
				euHighProperty.ItemId = euHighProperty.Id.ToString();
				euHighProperty.Datatype = typeof(System.Double);
				euHighProperty.AccessRights = EnumAccessRights.READABLE;
				analogTypeItem.AddProperty(euHighProperty);

				DaProperty euLowProperty = new DaProperty();
				euLowProperty.Id = (int)EnumPropertyId.LOW_EU;
				euLowProperty.Name = "Low EU";
				euLowProperty.Description = "Low EU";
				euLowProperty.ItemId = euLowProperty.Id.ToString();
				euLowProperty.Datatype = typeof(System.Double);
				euLowProperty.AccessRights = EnumAccessRights.READABLE;
				analogTypeItem.AddProperty(euLowProperty);

				DaProperty irHighProperty = new DaProperty();
				irHighProperty.Id = (int)EnumPropertyId.HIGH_INSTRUMENT_RANGE;
				irHighProperty.Name = "High InstrumentRange";
				irHighProperty.Description = "High InstrumentRange";
				irHighProperty.ItemId = irHighProperty.Id.ToString();
				irHighProperty.Datatype = typeof(System.Double);
				irHighProperty.AccessRights = EnumAccessRights.READABLE;
				analogTypeItem.AddProperty(irHighProperty);

				DaProperty irLowProperty = new DaProperty();
				irLowProperty.Id = (int)EnumPropertyId.LOW_INSTRUMENT_RANGE;
				irLowProperty.Name = "Low InstrumentRange";
				irLowProperty.Description = "Low InstrumentRange";
				irLowProperty.ItemId = irLowProperty.Id.ToString();
				irLowProperty.Datatype = typeof(System.Double);
				irLowProperty.AccessRights = EnumAccessRights.READABLE;
				analogTypeItem.AddProperty(irLowProperty);

				DaProperty euUnitsProperty = new DaProperty();
				euUnitsProperty.Id = (int)EnumPropertyId.EU_UNITS;
				euUnitsProperty.Name = "EU Units";
				euUnitsProperty.Description = "EU Units";
				euUnitsProperty.ItemId = euUnitsProperty.Id.ToString();
				euUnitsProperty.Datatype = typeof(System.Double);
				euUnitsProperty.AccessRights = EnumAccessRights.READABLE;
				analogTypeItem.AddProperty(euUnitsProperty);

				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				// Byte
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Byte", typeof(Byte));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				// Int16
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int16", typeof(Int16));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				// UInt16
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt16", typeof(UInt16));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				// Int32
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int32", typeof(Int32));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				// UInt32
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt32", typeof(UInt32));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				// Int64
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int64", typeof(Int64));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				// UInt64
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt64", typeof(UInt64));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				// Double
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Double", typeof(Double));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				// Float
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Float", typeof(float));
				if (!analogTypeFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));

				#endregion

				#region AnalogTypeArray elements for AllProfiles folder

				AddressSpaceElement analogTypeArraysFolder = creator.CreateMyDaAddressSpaceElement();
				analogTypeArraysFolder.Name = "AnalogType Arrays";
				analogTypeArraysFolder.HasChildren = true;
				if (!daFolder.AddChild(analogTypeArraysFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				// Int16
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int16", typeof(Int16[]));
				if (!analogTypeArraysFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(new Int16[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, EnumQuality.GOOD, DateTime.Now));

				// Int32
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Int32", typeof(Int32[]));
				if (!analogTypeArraysFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(new Int32[] { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 }, EnumQuality.GOOD, DateTime.Now));

				// UInt16
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt16", typeof(UInt16[]));
				if (!analogTypeArraysFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(new UInt16[] { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 }, EnumQuality.GOOD, DateTime.Now));

				// UInt32
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("UInt32", typeof(UInt32[]));
				if (!analogTypeArraysFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(new UInt32[] { 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 }, EnumQuality.GOOD, DateTime.Now));

				// Float
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Float", typeof(float[]));
				if (!analogTypeArraysFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(new float[] { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f }, EnumQuality.GOOD, DateTime.Now));

				// Double
				analogTypeItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("Double", typeof(Double[]));
				if (!analogTypeArraysFolder.AddChild(analogTypeItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogTypeItem.AddProperty(euTypeProperty);
				analogTypeItem.AddProperty(euHighProperty);
				analogTypeItem.AddProperty(euLowProperty);
				analogTypeItem.AddProperty(irHighProperty);
				analogTypeItem.AddProperty(irLowProperty);
				analogTypeItem.AddProperty(euUnitsProperty);
				analogTypeItem.ValueChanged(new ValueQT(new Double[] { 9.00001, 9.0002, 9.003, 9.04, 9.5, 9.06, 9.007, 9.008, 9.0009 }, EnumQuality.GOOD, DateTime.Now));



				#endregion


				#endregion

				#region Test Module

				AddressSpaceElement testFolder = creator.CreateMyDaAddressSpaceElement();
				testFolder.Name = "Test Folder";
				testFolder.HasChildren = true;
				if (!daRoot.AddChild(testFolder))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				#region CreateItems Command

				// Add Create Items command
				DaCreateItemsCommand createItemsCmd = (DaCreateItemsCommand)creator.CreateMyDaCreateItemsCommand();
				createItemsCmd.Name = "CreateItemsCommand";
				createItemsCmd.HasChildren = true;
				if (!testFolder.AddChild(createItemsCmd))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				createItemsCmd.ValueChanged(new ValueQT(false, EnumQuality.GOOD, DateTime.Now));

				MyDaAddressSpaceElement param;

				// item count parameter
				param = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement(DaCreateItemsCommand.ItemCountParam, DaCreateItemsCommand.ItemCountParamType);
				if (!createItemsCmd.AddChild(param))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				param.ValueChanged(new ValueQT((uint)0, EnumQuality.GOOD, DateTime.Now));

				// item type parameter
				param = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement(DaCreateItemsCommand.ItemTypeParam, DaCreateItemsCommand.ItemTypeParamType);
				if (!createItemsCmd.AddChild(param))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				param.ValueChanged(new ValueQT((uint)0, EnumQuality.GOOD, DateTime.Now));

				// item is analog parameter
				param = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement(DaCreateItemsCommand.ItemIsAnalogParam, DaCreateItemsCommand.ItemIsAnalogParamType);
				param.AccessRights = EnumAccessRights.READWRITEABLE;
				if (!createItemsCmd.AddChild(param))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				param.ValueChanged(new ValueQT(false, EnumQuality.GOOD, DateTime.Now));

				// item set result parameter
				param = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement(DaCreateItemsCommand.ItemSetIdParam, DaCreateItemsCommand.ItemSetIdParamType);
				param.AccessRights = EnumAccessRights.READABLE;
				if (!createItemsCmd.AddChild(param))
				{
					return (int)EnumResultCode.E_FAIL;
				}
				param.ValueChanged(new ValueQT(new string[0], EnumQuality.GOOD, DateTime.Now));

				#endregion

				#region Analog Items

				MyDaAddressSpaceElement analogItem;

				analogItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("AnalogItem_1", typeof(double));
				if (!testFolder.AddChild(analogItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				DaConstantProperty euType = new DaConstantProperty(
					(int)EnumPropertyId.ITEM_EU_TYPE,
					"EUType",
					"7",
					new ValueQT(EnumEUType.ANALOG, EnumQuality.GOOD, DateTime.Now));
				euType.AccessRights = EnumAccessRights.READABLE;
				analogItem.AddProperty(euType);

				DaProperty euHigh = new DaProperty();
				euHigh.Id = (int)EnumPropertyId.HIGH_EU;
				euHigh.Name = "High EU";
				euHigh.Description = "High EU";
				euHigh.ItemId = euHigh.Id.ToString();
				euHigh.Datatype = typeof(System.Double);
				euHigh.AccessRights = EnumAccessRights.READABLE;
				analogItem.AddProperty(euHigh);

				DaProperty euLow = new DaProperty();
				euLow.Id = (int)EnumPropertyId.LOW_EU;
				euLow.Name = "Low EU";
				euLow.Description = "Low EU";
				euLow.ItemId = euLow.Id.ToString();
				euLow.Datatype = typeof(System.Double);
				euLow.AccessRights = EnumAccessRights.READABLE;
				analogItem.AddProperty(euLow);

				analogItem.ValueChanged(new ValueQT(0.0, EnumQuality.GOOD, DateTime.Now));

				analogItem = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement("AnalogItem_2", typeof(double));
				if (!testFolder.AddChild(analogItem))
				{
					return (int)EnumResultCode.E_FAIL;
				}

				analogItem.AddProperty(euType);
				analogItem.AddProperty(euHigh);
				analogItem.AddProperty(euLow);

				analogItem.ValueChanged(new ValueQT(1.0, EnumQuality.GOOD, DateTime.Now));
				#endregion

				#endregion
			}
			catch(Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer:BuildAddressSpace",
					exc.ToString());
				return (int)EnumResultCode.E_FAIL;
			}	//	end try...catch

			return (int)EnumResultCode.S_OK;
		}	//	end BuildAddressSpace

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
		}	//	end Trace


		//--
		#endregion

	}	//	end OpcServer
}	//	end namespace