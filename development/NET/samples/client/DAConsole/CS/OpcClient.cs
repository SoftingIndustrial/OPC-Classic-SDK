using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace DaConsole
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
		string m_usr;
		string m_pwd;
		#region Private Attributes
		//------------------------

		private MyDaSession m_daSession = null;
		private MyDaSubscription m_daSubscription = null;
		private MyDaItem m_daItem1 = null;
		private MyDaItem m_daItem2 = null;
		private MyDaItem m_daItem3 = null;
		private MyDaItem m_daItem4 = null;
		private MyDaItem m_daItem5 = null;
		private MyDaItem m_daItem6 = null;

		private MyDaItem[] m_itemList;
		private string[] m_itemIds;
		private int[] m_results;
		private ValueQT[] m_values;
		private ExecutionOptions m_executionOptions;
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

			//	proceed with the OPC Toolkit core initialization
			result = GetApplication().Initialize();

			if (ResultCode.SUCCEEDED(result))
			{
				//	enable toolkit internal initialization
				GetApplication().EnableTracing(
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					"Trace.txt",
					1000000,
					0);
			}   //	end if

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

			return result;

		}   //	end Initialize

		public int ProcessCommandLine(string commandLine)
		{
			//	forward the command line arguments to the OPC Toolkit core internals
			return Application.Instance.ProcessCommandLine(commandLine);
		}   //	end ProcessCommandLine


		public void Terminate()
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
			m_daSubscription.RemoveDaItem(m_daItem5);
			m_daSubscription.RemoveDaItem(m_daItem6);

			m_daSession.RemoveDaSubscription(m_daSubscription);

			GetApplication().RemoveDaSession(m_daSession);

			GetApplication().Terminate();
			m_daSession = null;
			m_daSubscription = null;
			m_daItem1 = null;
			m_daItem2 = null;
			m_daItem3 = null;
			m_daItem4 = null;
			m_daItem5 = null;
			m_daItem6 = null;
		}   //	end Terminate


		public int InitializeDaObjects()
		{

			int connectResult = (int)EnumResultCode.E_FAIL;

			m_executionOptions = new ExecutionOptions();
			m_executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
			m_executionOptions.ExecutionContext = 0;

			m_itemList = new MyDaItem[6];
			m_itemIds = new string[] { "maths.sin", "time.local.second", "increment.UI1 array", "increment.BSTR", "secure.I2", "IOP.static.I1 array" };

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

				m_daItem5 = new MyDaItem("secure.I2", m_daSubscription);
				if (!m_daItem5.Valid)
				{
					return connectResult;
				}

				m_itemList[4] = m_daItem5;

				m_daItem6 = new MyDaItem("IOP.static.I1 array", m_daSubscription);
				if (!m_daItem6.Valid)
				{
					return connectResult;
				}

				m_itemList[5] = m_daItem6;



				connectResult = m_daSession.Connect(true, false, m_executionOptions);
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

		public string ReadItem()
		{
			string message = String.Empty;

			try
			{
				uint readCount = 1;
				MyDaItem[] m_itemList = new MyDaItem[readCount];
				m_itemList[0] = m_daItem1;

				ValueQT[] m_values = null;
				int[] m_results = null;

				if (ResultCode.SUCCEEDED(
					m_daSubscription.Read(
					0,
					m_itemList,
					out m_values,
					out m_results,
					null)))
				{
					message += " \nRead item synchronously using subscription \n";

					for (int i = 0; i < m_values.Length; i++)
					{
						if (ResultCode.SUCCEEDED(m_results[i]))
						{
							message += " " + m_itemList[i].Id + " - ";
							message += m_values[i].ToString() + "\n\n";
						}
						else
						{
							message += "Read failed for item " + m_itemList[i].Id + "\n\n";
						}   //	end if...else
					}   //	end for
				}
				else
				{
					message += " Subscription synchronous read failed!" + "\n\n";
				}   //	end if...else
			}
			catch (Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::ReadItem",
					exc.ToString());
			}   //	end try...catch
			return message;
		}   //	end ReadItem


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
			System.Console.WriteLine();
			if (sync)
			{
				int result = this.m_daSession.Connect(true, true, new ExecutionOptions());
				if (ResultCode.FAILED(result))
				{
					System.Console.WriteLine(" Session activate failed!");
				}
			}// end if
			else
			{
				m_daSession.Connect(true, true, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else
		} // end ActivateSession


		public void ConnectSession(bool sync)
		{
			System.Console.WriteLine();
			if (sync)
			{
				int result = m_daSession.Connect(true, false, new ExecutionOptions());
				if (ResultCode.FAILED(result))
				{
					System.Console.WriteLine(" Session connect failed!");
				}
			} // end if
			else
			{
				m_daSession.Connect(true, false, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else
		} // end ConnectSession


		public void DisconnectSession(bool sync)
		{
			System.Console.WriteLine();
			if (sync)
			{
				int result = m_daSession.Disconnect(new ExecutionOptions());
				if (ResultCode.FAILED(result))
				{
					System.Console.WriteLine(" Session disconnect failed!");
				}
			} // end if
			else
			{
				m_daSession.Disconnect(m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else

		} //end DisconnectSession


		public void ReadItems(bool sync)
		{
			ValueQT itemValues;
			int itemResult;
			if (sync)
			{
				int readResult = m_daItem1.Read(100, out itemValues, out itemResult, new ExecutionOptions());
				if (ResultCode.SUCCEEDED(readResult))
				{
					System.Console.WriteLine("Item synchronous read succeeded");
					System.Console.WriteLine(String.Format("{0,-19} {1} {2,-50} ", m_daItem1.Id, "-", itemValues.ToString()));
				}
				else
				{
					System.Console.WriteLine("Synchronous item read failed! " + " Item: " + m_daItem2.Id + " Result: " + readResult);
				}

				System.Console.WriteLine();
				int result = (int)EnumResultCode.E_FAIL;

				if (ResultCode.SUCCEEDED(result = m_daSubscription.Read(100, m_itemList, out m_values, out m_results, new ExecutionOptions())))
				{
					System.Console.WriteLine(" Subscription synchronous read succeeded");
					for (int i = 0; i < m_itemList.Length; i++)
					{
						if (m_results[i] >= 0)
						{
							System.Console.WriteLine(String.Format("{0,-19} {1} {2,-50} ", m_itemList[i].Id, "-", m_values[i].ToString()));
						}
						else
						{
							System.Console.WriteLine(" Item read failed! " + " Item: " + m_itemList[i].Id + " [ResultID: " + m_results[i] + " ]");
						}
					}
				}
				else
				{
					System.Console.WriteLine(" Synchronous subscription read failed! Result: " + result);
				}
				System.Console.WriteLine();
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
				ValueQT[] inValues = new ValueQT[4];

				SByte[] v1 = new SByte[5];
				v1[0] = 7; v1[1] = 8; v1[2] = 9; v1[3] = 10; v1[4] = 11;
				inValues[0] = new ValueQT(v1, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				System.String v2 = "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ";
				inValues[1] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				int v3 = v1[2];
				inValues[2] = new ValueQT(v3, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				//Prepare bounded array (these are the values to be written)
				int[] zeroBoundArray = new int[] { 1, 2, 3 };
				int lowBound = 1;
				Array lowBoundArray = Array.CreateInstance(typeof(object), new int[] { zeroBoundArray.Length }, new int[] { lowBound });
				zeroBoundArray.CopyTo(lowBoundArray, lowBound);
				//end prepare bounded array

				inValues[3] = new ValueQT(lowBoundArray, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				int result = (int)EnumResultCode.E_FAIL;
				MyDaItem[] inItems = new MyDaItem[4];
				inItems.SetValue(m_daItem3, 0);
				inItems.SetValue(m_daItem4, 1);
				inItems.SetValue(m_daItem5, 2);
				inItems.SetValue(m_daItem6, 3);

				int[] inResults = new int[4];

				if (ResultCode.SUCCEEDED(result = m_daSubscription.Write(inItems, inValues, out inResults, new ExecutionOptions())))
				{

					System.Console.WriteLine(" Subscription synchronous write succeeded");
					for (int i = 0; i < inValues.Length; i++)
					{
						if (inResults[i] >= 0)
						{
							System.Console.WriteLine(inValues[i].ToString());
							System.Console.WriteLine();
						}
						else
						{
							System.Console.WriteLine(" Item synchronous write failed!  " + inItems[i].Id + " [ResultID: " + inResults[i] + " ]");
						}
					}
				}
				else
				{
					System.Console.WriteLine("Subscription synchronous write failed! Result: " + result);
				}
			} // end if
			else
			{
				ValueQT[] inValues = new ValueQT[4];

				SByte[] v1 = new SByte[5];
				v1[0] = 7; v1[1] = 8; v1[2] = 9; v1[3] = 10; v1[4] = 11;
				inValues[0] = new ValueQT(v1, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				System.String v2 = "SOFTING";
				inValues[1] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				int v3 = 6;
				inValues[2] = new ValueQT(v3, EnumQuality.UNCERTAIN, writeDateTime);

				//Prepare bounded array (these are the values to be written)
				int[] zeroBoundArray = new int[] { 4, 5, 6 };
				int lowBound = 1;
				Array lowBoundArray = Array.CreateInstance(typeof(object), new int[] { zeroBoundArray.Length }, new int[] { lowBound }); ;
				zeroBoundArray.CopyTo(lowBoundArray, lowBound);
				//end prepare bounded array

				inValues[3] = new ValueQT(lowBoundArray, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				MyDaItem[] inItems = new MyDaItem[4];
				inItems[0] = m_daItem3;
				inItems[1] = m_daItem4;
				inItems[2] = m_daItem5;
				inItems[3] = m_daItem6;

				int[] inResults = new int[4];

				m_daSubscription.Write(inItems, inValues, out inResults, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else

		} //end WriteItems
        public void ChangeDataRetrievalMode()
        {
            EnumDataRetrievalMode currendMode = m_daSubscription.DataRetreivalMode;
            EnumDataRetrievalMode nextMode = EnumDataRetrievalMode.EnumDataRetrievalMode_ASYNCADVISE;

            System.Console.WriteLine("Previous data retrieval mode was: ");

            switch (currendMode)
            {
                case EnumDataRetrievalMode.EnumDataRetrievalMode_ASYNCADVISE:
                    System.Console.WriteLine("Asynchronous Advises.\nNew data retrieval mode was set: Polled Refresh - XML-DA specific.\nPlease reconnect the subscription.\n");
                    nextMode = EnumDataRetrievalMode.EnumDataRetrievalMode_POLLEDREFRESH;
                    break;
                case EnumDataRetrievalMode.EnumDataRetrievalMode_POLLEDREFRESH:
                    System.Console.WriteLine("Polled Refresh.\nNew data retrieval mode was set: Synchronous reads.\nPlease reconnect the subscription.\n");
                    nextMode = EnumDataRetrievalMode.EnumDataRetrievalMode_SYNCREAD;
                    break;
                case EnumDataRetrievalMode.EnumDataRetrievalMode_SYNCREAD:
                    System.Console.WriteLine("Synchronous reads.\nNew data retrieval mode was set: Asynchronous Advises.\nPlease reconnect the subscription.\n");
                    nextMode = EnumDataRetrievalMode.EnumDataRetrievalMode_ASYNCADVISE;
                    break;
                default:
                    System.Console.WriteLine("Not Set.\nNew data retrieval mode was set: Asynchronous Advises.\nPlease reconnect the subscription.\n");
                    nextMode = EnumDataRetrievalMode.EnumDataRetrievalMode_ASYNCADVISE;
                    break;
            }

            m_daSubscription.DataRetreivalMode = nextMode;
        }

		public void ReadUsingSession(bool sync)
		{
			if (sync)
			{
				int result = (int)EnumResultCode.E_FAIL;
				// in case of a XML-DA server use a valid item paths array instead of "null"
				if (ResultCode.SUCCEEDED(result = m_daSession.Read(0, m_itemIds, null, out m_values, out m_results, new ExecutionOptions())))
				{
					System.Console.WriteLine(" Session synchronous read succeeded!");
					for (int i = 0; i < m_itemList.Length; i++)
					{
						if (m_results[i] >= 0)
						{
							System.Console.WriteLine(String.Format("{0,-19} {1} {2,-50} ", m_itemIds[i], "-", m_values[i].ToString()));
						}
						else
						{
							System.Console.WriteLine(" Synchronous read failed for item " + m_itemList[i].Id + " [ResultID: " + m_results[i] + " ]");
						}
					}
				}
				else
				{
					System.Console.WriteLine(" Session synchronous read failed! Result: " + result);
				}
				System.Console.WriteLine();
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
				ValueQT[] inValues = new ValueQT[4];
				int[] inResults = new int[4];
				string[] inItemIds = new string[] { "increment.UI1 array", "increment.BSTR", "secure.I2", "IOP.static.I1 array" };
				int result = (int)EnumResultCode.E_FAIL;

				SByte[] v2 = new SByte[5];
				v2[0] = 7; v2[1] = 8; v2[2] = 9; v2[3] = 10; v2[4] = 11;
				inValues[0] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				System.String v3 = "SOFTING";
				inValues[1] = new ValueQT(v3, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				int v4 = 10;
				inValues[2] = new ValueQT(v4, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				//Prepare bounded array (these are the values to be written)
				int[] zeroBoundArray = new int[] { 7, 8, 9 };
				int lowBound = 1;
				Array lowBoundArray = Array.CreateInstance(typeof(object), new int[] { zeroBoundArray.Length }, new int[] { lowBound }); ;
				zeroBoundArray.CopyTo(lowBoundArray, lowBound);
				//end prepare bounded array

				inValues[3] = new ValueQT(lowBoundArray, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				// in case of a XML-DA server use a valid item paths array instead of "null"
				if (ResultCode.SUCCEEDED(result = m_daSession.Write(inItemIds, null, inValues, out inResults, new ExecutionOptions())))
				{
					System.Console.WriteLine(" Session synchronous write succeeded");
					for (int i = 0; i < inItemIds.Length; i++)
					{
						if (inResults[i] >= 0)
						{
							System.Console.WriteLine(String.Format("{0,-19} {1} {2,-50} ", inItemIds[i], "-", inValues[i].ToString()));
							System.Console.WriteLine();
						}
						else
						{
							System.Console.WriteLine(" Item synchronous write failed! " + " Item: " + inItemIds[i] + " [ResultID: " + inResults[i] + " ]");
						}
					}
				}
				else
				{
					System.Console.WriteLine(" Session synchronous write failed! Result: " + result);
				}
				System.Console.WriteLine();
			} // end if
			else //Async execution
			{
				ValueQT[] inValues = new ValueQT[4];
				int[] inResults = new int[4];

				string[] inItemIds = new string[] { "increment.UI1 array", "increment.BSTR", "secure.I2", "IOP.static.I1 array" };

				SByte[] v2 = new SByte[5];
				v2[0] = 7; v2[1] = 8; v2[2] = 9; v2[3] = 10; v2[4] = 11;
				inValues[0] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				System.String v3 = "SOFTING";
				inValues[1] = new ValueQT(v3, EnumQuality.QUALITY_NOT_SET, writeDateTime);


				int v4 = 11;
				inValues[2] = new ValueQT(v4, EnumQuality.QUALITY_NOT_SET, writeDateTime);

				//Prepare bounded array (these are the values to be written)
				int[] zeroBoundArray = new int[] { 10, 11, 12 };
				int lowBound = 1;
				Array lowBoundArray = Array.CreateInstance(typeof(object), new int[] { zeroBoundArray.Length }, new int[] { lowBound }); ;
				zeroBoundArray.CopyTo(lowBoundArray, lowBound);
				//end prepare bounded array

				inValues[3] = new ValueQT(lowBoundArray, EnumQuality.QUALITY_NOT_SET, writeDateTime);
				// in case of a XML-DA server use a valid item paths array instead of "null"
				m_daSession.Write(inItemIds, null, inValues, out inResults, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else

		} //end WriteUsingSession


		public void GetServerStatus(bool sync)
		{
			System.Console.WriteLine();
			ServerStatus newServerStatus;
			ServerStatus serverStatus;
			if (sync)
			{
				int result = (int)EnumResultCode.E_FAIL;

				if (ResultCode.SUCCEEDED(result = m_daSession.GetStatus(out serverStatus, new ExecutionOptions())))
				{

					System.Console.WriteLine("Server Status");
					System.Console.WriteLine("	Vendor info: " + serverStatus.VendorInfo);
					System.Console.WriteLine("	Product version: " + serverStatus.ProductVersion);
					System.Console.WriteLine("	State: " + serverStatus.State);
					System.Console.WriteLine("	Start time: " + serverStatus.StartTime.ToString());
					System.Console.WriteLine("	Last update time: " + serverStatus.LastUpdateTime.ToString());
					System.Console.WriteLine("	Current time: " + serverStatus.CurrentTime.ToString());
					System.Console.WriteLine("	GroupCount: " + serverStatus.GroupCount);
					System.Console.WriteLine("	Bandwidth: " + serverStatus.Bandwidth);
					for (int i = 0; i < serverStatus.SupportedLcIds.Length; i++)
					{
						System.Console.WriteLine("	Supported LcId: " + serverStatus.SupportedLcIds[i]);
					}
					System.Console.WriteLine("	Status info: " + serverStatus.StatusInfo);
				}
				else
				{
					System.Console.WriteLine("Synchronous get status failed! Result: " + result);
				}
			} // end if
			else
			{
				m_daSession.GetStatus(out newServerStatus, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			} // end else

		} //end GetServerStatus

		public void ActivateConnectionMonitor()
		{
			int result = (int)EnumResultCode.E_FAIL;
			if (ResultCode.SUCCEEDED(result = m_daSession.ActivateConnectionMonitor(true, 5000, 0, 10000, 300000)))
			{
				System.Console.WriteLine();
				System.Console.WriteLine("Activated connection monitor");
				System.Console.WriteLine();
			}
			else
			{
				System.Console.WriteLine("Activate connection monitor failed! Result " + result);
			}
		} //end ActivateConnectionMonitor


		public void DeactivateConnectionMonitor()
		{
			int result = (int)EnumResultCode.E_FAIL;
			if (ResultCode.SUCCEEDED(result = m_daSession.ActivateConnectionMonitor(false, 0, 0, 0, 0)))
			{
				System.Console.WriteLine();
				System.Console.WriteLine("Deactivated connection monitor");
				System.Console.WriteLine();
			}
			else
			{
				System.Console.WriteLine("Deactivate connection monitor failed! Result: " + result);
			}
		} //end DeactivateConnectionMonitor

		public void setCredentials()
		{
			System.Console.Write("\t User name:");
			m_usr = System.Console.ReadLine();
			System.Console.Write("\t Password:");
			m_pwd = System.Console.ReadLine();
			System.Console.WriteLine("Provided server authentication credentials were set: User name:" + m_usr + " - password" + m_pwd);
		}


		public void setCredentials(string usr, string pwd)
		{
			m_usr = usr;
			m_pwd = pwd;
		}

		//--
		#endregion

		public void LogInSecure(bool sync)
		{
			int result = (int)EnumResultCode.E_ACCESSDENIED;
			EnumObjectState SessionState;
			SessionState = m_daSession.CurrentState;
			if (sync)
			{
				if (SessionState != EnumObjectState.DISCONNECTED)
				{
					result = m_daSession.Logon(m_usr, m_pwd, new ExecutionOptions());
					System.Console.WriteLine("Logging on for user: " + m_usr + "  pass:" + m_pwd + " " + result);
				}
				else
				{
					System.Console.WriteLine("Logon for secure connection failed - session is not connected.");
				}
			}
			else
			{
				m_daSession.Logon(m_usr, m_pwd, m_executionOptions);
				m_executionOptions.ExecutionContext++;
			}
		}

		public void LogOffSecure(bool sync)
		{
			int result = (int)EnumResultCode.E_ACCESSDENIED;
			EnumObjectState SessionState;
			SessionState = m_daSession.CurrentState;
			if (sync)
			{
				if (SessionState != EnumObjectState.DISCONNECTED)
				{
					result = m_daSession.Logoff(new ExecutionOptions());
					System.Console.WriteLine("Logging off for user: " + m_usr + "  pass:" + m_pwd + " " + result);
				}
				else
				{
					System.Console.WriteLine("Logoff for secure connection failed - session is not connected.");
				}
			}
			else
			{
				m_daSession.Logoff(m_executionOptions);
				m_executionOptions.ExecutionContext++;
			}
		}
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

	}   //	end class OpcClient

}   //	end namespace
