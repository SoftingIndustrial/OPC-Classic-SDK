using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.IO;

namespace DaConsole_Service
{
	public class OpcClient
	{

		#region Constructor
		//-----------------

		public OpcClient() { }

		//--
		#endregion

		#region Private Members
		//---------------------

		#region Private Attributes
		//------------------------

		private MyDaSession m_daSession = null;
		private MyDaSubscription m_daSubscription = null;
		private MyDaItem m_daItem1 = null;
		private MyDaItem m_daItem2 = null;
		private MyDaItem m_daItem3 = null;
		private MyDaItem m_daItem4 = null;
		private MyDaItem[] m_itemList;
		private string[] m_itemIds;
		private int[] m_results;
		private ValueQT[] m_values;
		private ExecutionOptions m_executionOptions;
		private bool m_areDaObjectsInitialized = false;
		//--
		#endregion


		//--
		#endregion


		#region Public Methods
		//---------------------

		public Application GetApplication()
		{
			return Application.Instance;
		}   //	end GetApplication

		public int Initialize()
		{

			int result = (int)EnumResultCode.S_OK;
			GetApplication().VersionOtb = 447;
			//	TODO - binary license activation
			//	Fill in your binary license activation keys here
			//
			//	NOTE: you can activate one or all of the features at the same time

			//	activate the COM-DA Client feature
			//	result = Application.Instance.Activate(EnumFeature.DA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}

			//	activate the XML-DA Client Feature
			//	result = Application.Instance.Activate(EnumFeature.XMLDA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}
			//	END TODO - binary license activation

			//	proceed with the OPC Toolkit core initialization
			result = GetApplication().Initialize();

			if (ResultCode.SUCCEEDED(result))
			{
				//	enable toolkit internal initialization
				GetApplication().EnableTracing(
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.CLIENT,
					EnumTraceGroup.CLIENT,
					"Trace.txt",
					1000000,
					0);
			}   //	end if

			return result;

		}   //	end Initialize

		public int ProcessCommandLine(string commandLine)
		{
			//	forward the command line arguments to the OPC Toolkit core internals
			return Application.Instance.ProcessCommandLine(commandLine);
		}   //	end ProcessCommandLine


		public void Terminate()
		{
			if (m_areDaObjectsInitialized)
			{

				if (m_daSubscription.CurrentState != EnumObjectState.DISCONNECTED)
				{
					m_daSubscription.Disconnect(new ExecutionOptions());
				}
				if (m_daSession.CurrentState != EnumObjectState.DISCONNECTED)
				{
					m_daSession.Disconnect(new ExecutionOptions());
				}

				m_daSubscription.RemoveDaItem(m_daItem1);
				m_daSubscription.RemoveDaItem(m_daItem2);
				m_daSubscription.RemoveDaItem(m_daItem3);
				m_daSubscription.RemoveDaItem(m_daItem4);

				m_daSession.RemoveDaSubscription(m_daSubscription);

				GetApplication().RemoveDaSession(m_daSession);

			}

			GetApplication().Terminate();
			m_daSession = null;
			m_daSubscription = null;
			m_daItem1 = null;
			m_daItem2 = null;
			m_daItem3 = null;
			m_daItem4 = null;
		}   //	end Terminate


		public int InitializeDaObjects()
		{

			int connectResult = (int)EnumResultCode.E_FAIL;

			m_executionOptions = new ExecutionOptions();
			m_executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
			m_executionOptions.ExecutionContext = 0;

			m_itemList = new MyDaItem[4];
			m_itemIds = new string[] { "maths.sin", "time.local.second", "increment.UI1 array", "increment.BSTR" };

			try
			{

				//	TODO add your server URL here
				//	this is the server url for Softing OPCToolbox Demo Server
				//	first choice is COM-DA 
				string url = "opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}";

				//uncomment if you need an XML-DA access
				//	string url = "http://localhost:8079/OPC/DA";

				m_daSession = new MyDaSession(url);

				if (!m_daSession.Valid)
				{
					return connectResult;
				}

				m_daSubscription = new MyDaSubscription(1000, m_daSession);

				if (!m_daSubscription.Valid)
				{
					return connectResult;
				}

				m_daItem1 = new MyDaItem("maths.sin", m_daSubscription);

				if (!m_daItem1.Valid)
				{
					return connectResult;
				}
				m_itemList[0] = m_daItem1;

				m_daItem2 = new MyDaItem("time.local.second", m_daSubscription);

				if (!m_daItem2.Valid)
				{
					return connectResult;
				}
				m_itemList[1] = m_daItem2;

				m_daItem3 = new MyDaItem("increment.UI1 array", m_daSubscription);
				if (!m_daItem3.Valid)
				{
					return connectResult;
				}
				m_itemList[2] = m_daItem3;

				m_daItem4 = new MyDaItem("increment.BSTR", m_daSubscription);
				if (!m_daItem4.Valid)
				{
					return connectResult;
				}

				m_itemList[3] = m_daItem4;

				m_areDaObjectsInitialized = true;

				connectResult = m_daSession.Connect(true, true, null);
			}
			catch (Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::InitializaDaObjects",
					exc.ToString());
			}   //	end try...catch

			return connectResult;
		}   //	end InitializeDaObjects


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


		public void ActivateSession(bool sync)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			streamWriter.WriteLine();
			if (sync)
			{
				int result = this.m_daSession.Connect(true, true, new ExecutionOptions());
				if (ResultCode.FAILED(result))
				{
					streamWriter.WriteLine(" Session activate failed!");
				}
			}// end if
			else
			{
				m_daSession.Connect(true, true, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else
			streamWriter.Flush();
			streamWriter.Close();
		} // end ActivateSession


		public void ConnectSession(bool sync)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			streamWriter.WriteLine();
			if (sync)
			{
				int result = m_daSession.Connect(true, false, new ExecutionOptions());
				if (ResultCode.FAILED(result))
				{
					streamWriter.WriteLine(" Session connect failed!");
				}
			} // end if
			else
			{
				m_daSession.Connect(true, false, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else
			streamWriter.Flush();
			streamWriter.Close();
		} // end ConnectSession


		public void DisconnectSession(bool sync)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			streamWriter.WriteLine();
			if (sync)
			{
				int result = m_daSession.Disconnect(new ExecutionOptions());
				if (ResultCode.FAILED(result))
				{
					streamWriter.WriteLine(" Session disconnect failed!");
				}
			} // end if
			else
			{
				m_daSession.Disconnect(m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else
			streamWriter.Flush();
			streamWriter.Close();
		} //end DisconnectSession


		public void ReadItems(bool sync)
		{
			ValueQT itemValues;
			int itemResult;

			if (sync)
			{
				FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
				StreamWriter streamWriter = new StreamWriter(fs);
				streamWriter.BaseStream.Seek(0, SeekOrigin.End);

				int readResult = m_daItem1.Read(100, out itemValues, out itemResult, new ExecutionOptions());
				if (ResultCode.SUCCEEDED(readResult))
				{
					streamWriter.WriteLine("Item synchronous read succeded");
					streamWriter.WriteLine(String.Format("{0,-19} {1} {2,-50} ", m_daItem1.Id, "-", itemValues.ToString()));
				}
				else
				{
					streamWriter.WriteLine("Synchronous item read failed! " + " Item: " + m_daItem2.Id + " Result: " + readResult);
				}

				streamWriter.WriteLine();
				int result = (int)EnumResultCode.E_FAIL;

				if (ResultCode.SUCCEEDED(result = m_daSubscription.Read(100, m_itemList, out m_values, out m_results, new ExecutionOptions())))
				{
					streamWriter.WriteLine(" Subscription synchronous read succeeded");
					for (int i = 0; i < m_itemList.Length; i++)
					{
						if (m_results[i] >= 0)
						{
							streamWriter.WriteLine(String.Format("{0,-19} {1} {2,-50} ", m_itemList[i].Id, "-", m_values[i].ToString()));
						}
						else
						{
							streamWriter.WriteLine(" Item read failed! " + " Item: " + m_itemList[i].Id + " [ResultID: " + m_results[i] + " ]");
						}
					}
				}
				else
				{
					streamWriter.WriteLine(" Synchronous subscription read failed! Result: " + result);
				}
				streamWriter.WriteLine();
				streamWriter.Flush();
				streamWriter.Close();
			}
			else
			{
				m_daItem1.Read(100, out itemValues, out itemResult, m_executionOptions);
				m_executionOptions.ExecutionContext++;

				m_daSubscription.Read(100, m_itemList, out m_values, out m_results, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else

		} //end ReadItems


		public void WriteItems(bool sync)
		{
			DateTime writeDateTime = new DateTime();
			if (sync)
			{
				ValueQT[] inValues = new ValueQT[2];

				SByte[] v1 = new SByte[5];
				v1[0] = 7; v1[1] = 8; v1[2] = 9; v1[3] = 10; v1[4] = 11;
				inValues[0] = new ValueQT(v1, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				System.String v2 = "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ";
				inValues[1] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);
				int result = (int)EnumResultCode.E_FAIL;

				MyDaItem[] inItems = new MyDaItem[2];
				inItems.SetValue(m_daItem3, 0);
				inItems.SetValue(m_daItem4, 1);

				int[] inResults = new int[2];

				FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
				StreamWriter streamWriter = new StreamWriter(fs);
				streamWriter.BaseStream.Seek(0, SeekOrigin.End);
				if (ResultCode.SUCCEEDED(result = m_daSubscription.Write(inItems, inValues, out inResults, new ExecutionOptions())))
				{
					streamWriter.WriteLine();
					streamWriter.WriteLine(" Subscription synchronous write succeeded");
					for (int i = 0; i < inItems.Length; i++)
					{
						if (inResults[i] >= 0)
						{
							streamWriter.WriteLine(String.Format("{0,-19} {1} {2,-50} ", inItems[i], "-", inValues[i].ToString()));
							streamWriter.WriteLine();
						}
						else
						{
							streamWriter.WriteLine(" Item synchronous write failed!  " + inItems[i].Id + " [ResultID: " + inResults[i] + " ]");
						}
					}
				}
				else
				{
					streamWriter.WriteLine("Subscription synchronous write failed! Result: " + result);
				}
				streamWriter.Flush();
				streamWriter.Close();
			} // end if
			else
			{
				ValueQT[] inValues = new ValueQT[2];

				SByte[] v1 = new SByte[5];
				v1[0] = 7; v1[1] = 8; v1[2] = 9; v1[3] = 10; v1[4] = 11;
				inValues[0] = new ValueQT(v1, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				System.String v2 = "SOFTING";
				inValues[1] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				MyDaItem[] inItems = new MyDaItem[2];
				inItems[0] = m_daItem3;
				inItems[1] = m_daItem4;

				int[] inResults = new int[2];

				m_daSubscription.Write(inItems, inValues, out inResults, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else

		} //end WriteItems



		public void ReadUsingSession(bool sync)
		{
			if (sync)
			{
				int result = (int)EnumResultCode.E_FAIL;
				// in case of a XML-DA server use a valid item paths array instead of "null"

				FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
				StreamWriter streamWriter = new StreamWriter(fs);
				streamWriter.BaseStream.Seek(0, SeekOrigin.End);
				if (ResultCode.SUCCEEDED(result = m_daSession.Read(0, m_itemIds, null, out m_values, out m_results, new ExecutionOptions())))
				{
					streamWriter.WriteLine(" Session synchronous read succeeded!");
					for (int i = 0; i < m_itemList.Length; i++)
					{
						if (m_results[i] >= 0)
						{
							streamWriter.WriteLine(String.Format("{0,-19} {1} {2,-50} ", m_itemIds[i], "-", m_values[i].ToString()));
						}
						else
						{
							streamWriter.WriteLine(" Synchronous read failed for item " + m_itemList[i].Id + " [ResultID: " + m_results[i] + " ]");
						}
					}
				}
				else
				{
					streamWriter.WriteLine(" Session synchronous read failed! Result: " + result);
				}
				streamWriter.WriteLine();
				streamWriter.Flush();
				streamWriter.Close();
			} // end if
			else
			{
				// in case of a XML-DA server use a valid item paths array instead of "null"
				m_daSession.Read(0, m_itemIds, null, out m_values, out m_results, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else

		} //end ReadUsingSession


		public void WriteUsingSession(bool sync)
		{
			DateTime writeDateTime = new DateTime();
			if (sync)
			{
				ValueQT[] inValues = new ValueQT[2];
				int[] inResults = new int[2];
				string[] inItemIds = new string[] { "increment.UI1 array", "increment.BSTR" };
				int result = (int)EnumResultCode.E_FAIL;

				SByte[] v2 = new SByte[5];
				v2[0] = 7; v2[1] = 8; v2[2] = 9; v2[3] = 10; v2[4] = 11;
				inValues[0] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				System.String v3 = "SOFTING";
				inValues[1] = new ValueQT(v3, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
				StreamWriter streamWriter = new StreamWriter(fs);
				streamWriter.BaseStream.Seek(0, SeekOrigin.End);
				// in case of a XML-DA server use a valid item paths array instead of "null"
				if (ResultCode.SUCCEEDED(result = m_daSession.Write(inItemIds, null, inValues, out inResults, new ExecutionOptions())))
				{
					streamWriter.WriteLine(" Session synchronous write succeeded");
					for (int i = 0; i < inItemIds.Length; i++)
					{
						if (inResults[i] >= 0)
						{
							streamWriter.WriteLine(String.Format("{0,-19} {1} {2,-50} ", inItemIds[i], "-", inValues[i].ToString()));
							streamWriter.WriteLine();
						}
						else
						{
							streamWriter.WriteLine(" Item synchronous write failed! " + " Item: " + inItemIds[i] + " [ResultID: " + inResults[i] + " ]");
						}
					}
				}
				else
				{
					streamWriter.WriteLine(" Session synchronous write failed! Result: " + result);
				}
				streamWriter.WriteLine();
				streamWriter.Flush();
				streamWriter.Close();
			} // end if
			else
			{
				ValueQT[] inValues = new ValueQT[2];
				int[] inResults = new int[2];

				string[] inItemIds = new string[] { "increment.UI1 array", "increment.BSTR" };

				SByte[] v2 = new SByte[5];
				v2[0] = 7; v2[1] = 8; v2[2] = 9; v2[3] = 10; v2[4] = 11;
				inValues[0] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				System.String v3 = "SOFTING";
				inValues[1] = new ValueQT(v3, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				// in case of a XML-DA server use a valid item paths array instead of "null"
				m_daSession.Write(inItemIds, null, inValues, out inResults, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else

		} //end WriteUsingSession


		public void GetServerStatus(bool sync)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			streamWriter.WriteLine();
			ServerStatus newServerStatus;
			ServerStatus serverStatus;
			if (sync)
			{
				int result = (int)EnumResultCode.E_FAIL;

				if (ResultCode.SUCCEEDED(result = m_daSession.GetStatus(out serverStatus, new ExecutionOptions())))
				{

					streamWriter.WriteLine("Server Status");
					streamWriter.WriteLine("	Vendor info: " + serverStatus.VendorInfo);
					streamWriter.WriteLine("	Product version: " + serverStatus.ProductVersion);
					streamWriter.WriteLine("	State: " + serverStatus.State);
					streamWriter.WriteLine("	Start time: " + serverStatus.StartTime.ToString());
					streamWriter.WriteLine("	Last update time: " + serverStatus.LastUpdateTime.ToString());
					streamWriter.WriteLine("	Current time: " + serverStatus.CurrentTime.ToString());
					streamWriter.WriteLine("	GroupCount: " + serverStatus.GroupCount);
					streamWriter.WriteLine("	Bandwidth: " + serverStatus.Bandwidth);
					for (int i = 0; i < serverStatus.SupportedLcIds.Length; i++)
					{
						streamWriter.WriteLine("	Supported LcId: " + serverStatus.SupportedLcIds[i]);
					}
					streamWriter.WriteLine("	Status info: " + serverStatus.StatusInfo);
				}
				else
				{
					streamWriter.WriteLine("Synchronous get status failed! Result: " + result);
				}
			} // end if
			else
			{
				m_daSession.GetStatus(out newServerStatus, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else
			streamWriter.Flush();
			streamWriter.Close();

		} //end GetServerStatus

		public void ActivateConnectionMonitor()
		{
			int result = (int)EnumResultCode.E_FAIL;

			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			if (ResultCode.SUCCEEDED(result = m_daSession.ActivateConnectionMonitor(true, 5000, 0, 10000, 300000)))
			{
				streamWriter.WriteLine();
				streamWriter.WriteLine("Activated connection monitor");
				streamWriter.WriteLine();
			}
			else
			{
				streamWriter.WriteLine("Activate connection monitor failed! Result " + result);
			}
			streamWriter.Flush();
			streamWriter.Close();
		} //end ActivateConnectionMonitor


		public void DeactivateConnectionMonitor()
		{
			int result = (int)EnumResultCode.E_FAIL;

			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			if (ResultCode.SUCCEEDED(result = m_daSession.ActivateConnectionMonitor(false, 0, 0, 0, 0)))
			{
				streamWriter.WriteLine();
				streamWriter.WriteLine("Deactivated connection monitor");
				streamWriter.WriteLine();
			}
			else
			{
				streamWriter.WriteLine("Deactivate connection monitor failed! Result: " + result);
			}
			streamWriter.Flush();
			streamWriter.Close();
		} //end DeactivateConnectionMonitor

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
		}   //	end ServiceName

		public string ServiceDescription
		{
			get
			{
				return Application.Instance.ServiceDescription;
			}
			set
			{
				Application.Instance.ServiceDescription = value;
			}
		}   //	end ServiceDescription

		//--
		#endregion

	}   //	end class OpcClient

}   //	end namespace
