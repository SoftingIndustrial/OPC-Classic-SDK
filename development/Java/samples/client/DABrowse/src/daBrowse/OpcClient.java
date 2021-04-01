package daBrowse;

import com.softing.opc.client.api.Application;
import com.softing.opc.client.api.ExecutionOptions;
import com.softing.opc.client.enums.EnumObjectState;
import com.softing.opc.client.enums.EnumResultCode;
import com.softing.opc.client.enums.EnumTraceGroup;
import com.softing.opc.client.enums.EnumTraceLevel;
import com.softing.opc.client.enums.ResultCode;
import com.softing.opc.client.enums.EnumFeature;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public class OpcClient {

	final Logger logger = LoggerFactory.getLogger(OpcClient.class);
	
	private MyDaSession m_daSession = null;
	private ExecutionOptions m_executionOptions;
	
	public Application getApplication()
	{
		return Application.INSTANCE;
	}
	
	public int Initialize()
	{

		int result = EnumResultCode.S_OK.toInt();
		//	TODO - design time license activation
		//	Fill in your design time license activation keys here
		//
		//	NOTE: you can activate one or all of the features at the same time

		//	activate the COM-DA Client feature
		//result = getApplication().activate(EnumFeature.DA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
		if (!ResultCode.SUCCEEDED(result))
		{
			return result;
		}

		//	activate the XML-DA Client Feature
		//	result = getApplication().activate(EnumFeature.XMLDA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
		if (!ResultCode.SUCCEEDED(result))
		{
			return result;
		}

		
		//	END TODO - design time license activation
	
		//	proceed with the OPC Toolbox core initialization
		result = getApplication().initialize();
		
		if (ResultCode.SUCCEEDED(result))
		{
			//	enable toolbox internal initialization
			getApplication().enableTracing(					
				EnumTraceGroup.ALL,
				EnumTraceGroup.ALL,
				EnumTraceGroup.ALL,
				EnumTraceGroup.ALL,
				"Trace.txt",					
				1000000,
				2);
		}	//	end if

		return result;

	}

	public void Terminate()
	{

		if (m_daSession.getCurrentState() != EnumObjectState.DISCONNECTED)
		{
			m_daSession.disconnect(new ExecutionOptions());
		}
		
		getApplication().removeDaSession(m_daSession);

		getApplication().terminate();
		m_daSession = null;

	}
	
	public int InitializeDaObjects()
	{
		int connectResult = EnumResultCode.E_FAIL.toInt();
		m_executionOptions = new ExecutionOptions();


		try{
			
			//	TODO add your server URL here
			//	this is the server url for Softing OPCToolbox Demo Server
			//	first choice is COM-DA 
			
			//WITHOUT TUNNELING			
			//String url = "opcda:///ifm.OPC.OPCServer.DA.1";
			
			//WITH TUNNELING
			String url= "tpda://localhost:57765";
			
			m_daSession = new MyDaSession(url);
				
			if ( !m_daSession.getValid())
			{
				return connectResult;
			}									
			connectResult = m_daSession.connect(true, false, m_executionOptions);	
		}
		catch(Exception exc)
		{
			logger.error("InitializeDAObjects() - " + exc.getMessage());
			/*
			GetApplication().Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.USER,
				"OpcClient::InitializaDaObjects",
				exc.getMessage());
			*/
		}	//	end try...catch

		return connectResult;
	}	//	end InitializeDaObjects
	
	public MyDaSession GetSession()
	{
		return m_daSession;
	}
	
	public ExecutionOptions GetExecutionOptions()
	{
		return m_executionOptions;
	}
	
	public void Trace(
			EnumTraceLevel traceLevel,
			EnumTraceGroup traceGroup,
			String objectID,
			String message) 
		{
			Application.INSTANCE.trace(
				traceLevel,
				traceGroup,
				objectID,
				message);
		}	//	end Trace

	public String getServiceName()
	{
		return Application.INSTANCE.getServiceName();
	}
	
	public void setServiceName(String value)
	{
		Application.INSTANCE.setServiceName(value);
	}
}
