using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace DAObjectAttributes
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
		private ExecutionOptions m_executionOptions = null;
		private MyDaItem[] m_itemList = null;
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

			m_daSession.RemoveDaSubscription(m_daSubscription);
			GetApplication().RemoveDaSession(m_daSession);
			GetApplication().Terminate();
			m_daSession = null;
			m_daSubscription = null;
			m_daItem1 = null;
			m_daItem2 = null;
			m_daItem3 = null;
			m_executionOptions = null;
			m_itemList = null;
		}   //	end Terminate


		public int InitializeDaObjects()
		{
			int connectResult = (int)EnumResultCode.E_FAIL;
			m_itemList = new MyDaItem[3];
			m_executionOptions = new ExecutionOptions();

			try
			{

				//	TODO add your server URL here
				//	this is the server url for Softing OPCToolbox Demo Server
				//	first choice is COM-DA 
				string url = "opcda://localhost/Softing.OPCToolboxDemo_ServerDA/{2e565242-b238-11d3-842d-0008c779d775}";

				//uncomment if you need an XML-DA access
				//	string url = "http://localhost:8079/OPC/DA";

				m_daSession = new MyDaSession(url);

				m_daSubscription = new MyDaSubscription(500, m_daSession);

				m_daItem1 = new MyDaItem("maths.sin", m_daSubscription);
				m_itemList[0] = m_daItem1;

				m_daItem2 = new MyDaItem("time.local.second", m_daSubscription);
				m_itemList[1] = m_daItem2;

				m_daItem3 = new MyDaItem("increment.UI1", m_daSubscription);
				m_itemList[2] = m_daItem3;

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


		public void PrintDaSession(DaSession session)
		{

			System.Console.WriteLine("Session's Attributes:");
			System.Console.WriteLine("	URL: " + session.Url);
			System.Console.WriteLine("	ClientName: " + session.ClientName);
			System.Console.WriteLine("	LocaleID: " + session.LocaleId);
			System.Console.WriteLine("	SupportedOPCSpecification: " + session.SupportedOpcSpecification);
			System.Console.WriteLine("	Forced OPC Specification: " + session.ForcedOpcSpecification);
			System.Console.WriteLine("	Class context: " + session.ClassContext);
			System.Console.WriteLine();
		}// end PrintDaSession

		public void PrintDaSubscription(DaSubscription subscription)
		{

			System.Console.WriteLine("Subscription's Attributes:");
			System.Console.WriteLine("	Name: " + subscription.Name);
			System.Console.WriteLine("	RequestedUpdateRate: " + subscription.RequestedUpdateRate);
			System.Console.WriteLine("	RevisedUpdateRate: " + subscription.RevisedUpdateRate);
			System.Console.WriteLine("	Deadband: " + subscription.Deadband);
			System.Console.WriteLine("	TimeBias: " + subscription.TimeBias);
			System.Console.WriteLine("	RequestedKeepAliveTime: " + subscription.RequestedKeepAliveTime);
			System.Console.WriteLine("	RevisedKeepAliveTime: " + subscription.RevisedKeepAliveTime);
			System.Console.WriteLine("	LocaleID: " + subscription.LocaleId);
			System.Console.WriteLine();

		} //end PrintDaSubscription

		public void PrintDaItem(DaItem item)
		{

			System.Console.WriteLine("Item's Attributes:");
			System.Console.WriteLine("	ID: " + item.Id);
			System.Console.WriteLine("	Path: " + item.Path);
			System.Console.WriteLine("	NativeDatatype: " + item.NativeDatatype.ToString());
			System.Console.WriteLine("	AccessRights: " + item.AccessRights.ToString());
			System.Console.WriteLine("	RequestedDatatype: " + item.RequestedDatatype);
			System.Console.WriteLine("	Deadband: " + item.Deadband);
			System.Console.WriteLine("	EUType: " + item.EUType);
			if (item.EUInfo != null)
			{
				ValueData euValue;
				euValue = new ValueData(item.EUInfo);
				System.Console.WriteLine("	EUInfo: " + euValue.ToString());
			}
			System.Console.WriteLine();
		}// end PrintDaItem

		public void PrintInitialState()
		{
			System.Console.WriteLine("--- Initial object attributes ---");
			PrintDaSession(m_daSession);
			PrintDaSubscription(m_daSubscription);
			PrintDaItem(m_daItem1);
			PrintDaItem(m_daItem2);
			PrintDaItem(m_daItem3);
		}// end PrintInitialState

		public void ChangeSession()
		{
			int operationResult;
			int[] results;

			System.Console.WriteLine("--- Change the session's LCID and client name ---");
			System.Console.WriteLine();

			m_daSession.LocaleId = "de-DE";
			m_daSession.ClientName = "ObjectAttributes Sample";

			// write changed data to the server 
			operationResult = m_daSession.SetAttributesToServer(
				new EnumObjectAttribute[] { EnumObjectAttribute.DASESSION_LCID, EnumObjectAttribute.DASESSION_CLIENTNAME },
				out results,
				m_executionOptions);

			System.Console.WriteLine("Update session's attributes to server - result " + operationResult);

			// get it from the server to check 
			operationResult = m_daSession.GetAttributesFromServer(
				new EnumObjectAttribute[] { EnumObjectAttribute.DASESSION_LCID, EnumObjectAttribute.DASESSION_CLIENTNAME },
				out results,
				m_executionOptions);

			System.Console.WriteLine("Update session's attributes from server - result " + operationResult);

			PrintDaSession(m_daSession);
		} // end ChangeSession

		public void ChangeSubscription()
		{
			int operationResult;
			int[] results;

			System.Console.WriteLine("--- Change the subscription's name, update rate, deadband and keep alive time ---");
			System.Console.WriteLine();

			m_daSubscription.Name = "ObjectAttributes Subscription";
			m_daSubscription.RequestedUpdateRate = 1000;
			m_daSubscription.RequestedKeepAliveTime = 4000;
			m_daSubscription.Deadband = 10.0F;

			// write changed data to the server 
			operationResult = m_daSubscription.SetAttributesToServer(
				new EnumObjectAttribute[]{
						EnumObjectAttribute.DASUBSCRIPTION_NAME,
						EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE,
						EnumObjectAttribute.DASUBSCRIPTION_DEADBAND,
						EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME},
				out results,
				m_executionOptions);

			System.Console.WriteLine("Update subscription's attributes to server - result " + operationResult);

			// get it from the server to check 
			operationResult = m_daSubscription.GetAttributesFromServer(
				new EnumObjectAttribute[]{
						EnumObjectAttribute.DASUBSCRIPTION_NAME,
						EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE,
						EnumObjectAttribute.DASUBSCRIPTION_DEADBAND,
						EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME},
				out results,
				m_executionOptions);

			System.Console.WriteLine("Update subscription's attributes from server - result " + operationResult);

			System.Console.WriteLine();
			PrintDaSubscription(m_daSubscription);
		}// end ChangeSubscription

		public void ChangeItemDatatype()
		{
			System.Console.WriteLine("--- Change requested datatype of the items ---");

			string a = string.Empty;
			int operationResult;
			int[] results;
			m_daItem1.RequestedDatatype = a.GetType();
			m_daItem2.RequestedDatatype = a.GetType();
			m_daItem3.RequestedDatatype = a.GetType();

			// write requested datatypes to the server 
			operationResult = m_daSubscription.SetDaItemAttributesToServer(
				m_itemList,
				new EnumObjectAttribute[1] { EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE },
				out results,
				m_executionOptions);

			System.Console.WriteLine("Update item's attributes to server - result " + operationResult);

			// get it from the server to check 
			m_daSubscription.GetDaItemAttributesFromServer(
				m_itemList,
				new EnumObjectAttribute[1] { EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE },
				out results,
				m_executionOptions);

			System.Console.WriteLine("Update item's attributes from server - result " + operationResult);

			PrintDaItem(m_daItem1);
			PrintDaItem(m_daItem2);
			PrintDaItem(m_daItem3);
		}// end ChangeItemDatatype

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
