package daConsole;

import java.util.*;

import com.softing.opc.client.api.Application;
import com.softing.opc.client.api.DaAddressSpaceElementBrowseOptions;
import com.softing.opc.client.api.ExecutionOptions;
import com.softing.opc.client.api.ServerStatus;
import com.softing.opc.client.api.ValueQT;
import com.softing.opc.client.enums.EnumExecutionType;
import com.softing.opc.client.enums.EnumObjectState;
import com.softing.opc.client.enums.EnumQuality;
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
	private MyDaSubscription m_daSubscription = null;
	private MyDaItem m_daItem1 = null;
	private MyDaItem m_daItem2 = null;
	private MyDaItem m_daItem3 = null;
	private MyDaItem m_daItem4 = null;
	private MyDaItem m_daItem5 = null;
	private MyDaItem m_daItem6 = null;
	private MyDaItem m_daItem7 = null;
	private MyDaItem m_daItem8 = null;
	private MyDaItem m_daItem9 = null;
	private MyDaItem m_daItem10 = null;
	private MyDaItem m_daItem11 = null;
	
	private MyDaItem m_daItem12 = null;
	private MyDaItem m_daItem13 = null;
	private MyDaItem m_daItem14 = null;
	private MyDaItem m_daItem15 = null;
	private MyDaItem m_daItem16 = null;
	private MyDaItem m_daItem17 = null;
	private MyDaItem m_daItem18 = null;
	private MyDaItem m_daItem19 = null;
	private MyDaItem m_daItem20 = null;
	private MyDaItem m_daItem21 = null;
	
	private MyDaItem[] m_itemList;
	private String [] m_itemIds;
	private ArrayList<Integer> m_results; 
	private ArrayList<ValueQT> m_values;
	private ExecutionOptions m_executionOptions;
	
	public OpcClient(){}
	
	public Application getApplication()
	{
		return Application.INSTANCE;
	}
	
	public int initialize()
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

	}	//	end Initialize
	
	public int processCommandLine(String commandLine)
	{
		//	forward the command line arguments to the OPC Toolbox core internals
		return Application.INSTANCE.processCommandLine(commandLine);
	}	//	end ProcessCommandLine
	
	public void terminate()
	{

		if (m_daSubscription.getCurrentState() != EnumObjectState.DISCONNECTED)
		{
			m_daSubscription.disconnect(new ExecutionOptions());
		}
		if (m_daSession.getCurrentState() != EnumObjectState.DISCONNECTED)
		{
			m_daSession.disconnect(new ExecutionOptions());
		}

		m_daSubscription.removeDaItem(m_daItem1);
		m_daSubscription.removeDaItem(m_daItem2);
		m_daSubscription.removeDaItem(m_daItem3);
		m_daSubscription.removeDaItem(m_daItem4);
		m_daSubscription.removeDaItem(m_daItem5);
		m_daSubscription.removeDaItem(m_daItem6);
		m_daSubscription.removeDaItem(m_daItem7);
		m_daSubscription.removeDaItem(m_daItem8);
		m_daSubscription.removeDaItem(m_daItem9);
		m_daSubscription.removeDaItem(m_daItem10);
		m_daSubscription.removeDaItem(m_daItem11);
				
		m_daSubscription.removeDaItem(m_daItem12);
		m_daSubscription.removeDaItem(m_daItem13);
		m_daSubscription.removeDaItem(m_daItem14);
		m_daSubscription.removeDaItem(m_daItem15);
		m_daSubscription.removeDaItem(m_daItem16);
		m_daSubscription.removeDaItem(m_daItem17);
		m_daSubscription.removeDaItem(m_daItem18);
		m_daSubscription.removeDaItem(m_daItem19);
		m_daSubscription.removeDaItem(m_daItem20);
		m_daSubscription.removeDaItem(m_daItem21);
		
		m_daSession.removeDaSubscription(m_daSubscription);
		
		getApplication().removeDaSession(m_daSession);

		getApplication().terminate();
		m_daSession = null;
		m_daSubscription = null;
		m_daItem1 = null;
		m_daItem2 = null;
		m_daItem3 = null;
		m_daItem4 = null;
		m_daItem5 = null;
		m_daItem6 = null;
		m_daItem7 = null;
		m_daItem8 = null;
		m_daItem9 = null;
		m_daItem10 = null;
		m_daItem11 = null;
		m_daItem12 = null;
		m_daItem13 = null;
		m_daItem14 = null;
		m_daItem15 = null;
		m_daItem16 = null;
		m_daItem17 = null;
		m_daItem18 = null;
		m_daItem19 = null;
		m_daItem20 = null;
		m_daItem21 = null;

	}	//	end Terminate
	
	public int initializeDaObjects()
	{

		int connectResult = EnumResultCode.E_FAIL.toInt();

		m_executionOptions = new ExecutionOptions();
		m_executionOptions.setExecutionType(EnumExecutionType.ASYNCHRONOUS);
		m_executionOptions.setExecutionContext(0);

		m_itemList = new MyDaItem[21];
		///increment.BSTR
		//increment.UI1 array
		m_itemIds = new String[] {"maths.sin", "time.local.second", "IOP.static.I1 array", "IOP.static.BSTR", "IOP.static.I2", "IOP.static.I4", "IOP.static.I8","IOP.static.R4","IOP.static.R8","IOP.static.BOOL","IOP.static.DATE", "IOP.static.I1", "IOP.static.BOOL","IOP.static.I2 array", "IOP.static.BSTR array", "IOP.static.BOOL array", "IOP.static.I4 array", "IOP.static.DATE array", "IOP.static.I8 array", "IOP.static.R4 array", "IOP.static.R8 array"};

		try{
			
			//	TODO add your server URL here
			//	this is the server url for Softing OPCToolbox Demo Server
			//	first choice is COM-DA 
			
			//WITHOUT TUNNELING
			//String url = "opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}";
			
			//WITH TUNNELING
			String url= "tpda://localhost:56567";

			m_daSession = new MyDaSession(url);
			
			if ( !m_daSession.getValid())
			{
				return connectResult;
			}
			
			m_daSubscription = new MyDaSubscription(1000, m_daSession);

			if (! m_daSubscription.getValid())
			{
				return connectResult;
			}
			
			m_daItem1 = new MyDaItem("maths.sin", m_daSubscription);
				
			if (! m_daItem1.getValid())
			{
				return connectResult;
			}
			m_itemList[0] = m_daItem1;

			m_daItem2 = new MyDaItem("time.local.second", m_daSubscription);
			
			if (! m_daItem2.getValid())
			{
				return connectResult;
			}
			m_itemList[1] = m_daItem2;

			m_daItem3 = new MyDaItem("IOP.static.I1 array", m_daSubscription);
			if (! m_daItem3.getValid())
			{
				return connectResult;
			}
			m_itemList[2] = m_daItem3;
								
			m_daItem4 = new MyDaItem("IOP.static.BSTR", m_daSubscription);					
			if (! m_daItem4.getValid())
			{
				return connectResult;
			}

			m_itemList[3] = m_daItem4;
			
			m_daItem5 = new MyDaItem("IOP.static.I2", m_daSubscription);					
			if (! m_daItem5.getValid())
			{
				return connectResult;
			}

			m_itemList[4] = m_daItem5;
				
			m_daItem6 = new MyDaItem("IOP.static.I4", m_daSubscription);					
			if (! m_daItem6.getValid())
			{
				return connectResult;
			}

			m_itemList[5] = m_daItem6;
			
			m_daItem7 = new MyDaItem("IOP.static.I8", m_daSubscription);					
			if (! m_daItem7.getValid())
			{
				return connectResult;
			}

			m_itemList[6] = m_daItem7;
			
			m_daItem8 = new MyDaItem("IOP.static.R4", m_daSubscription);					
			if (! m_daItem8.getValid())
			{
				return connectResult;
			}

			m_itemList[7] = m_daItem8;
			
			m_daItem9 = new MyDaItem("IOP.static.R8", m_daSubscription);					
			if (! m_daItem9.getValid())
			{
				return connectResult;
			}

			m_itemList[8] = m_daItem9;
			
			m_daItem10 = new MyDaItem("IOP.static.BOOL", m_daSubscription);					
			if (! m_daItem10.getValid())
			{
				return connectResult;
			}

			m_itemList[9] = m_daItem10;
			
			m_daItem11 = new MyDaItem("IOP.static.DATE", m_daSubscription);					
			if (! m_daItem11.getValid())
			{
				return connectResult;
			}
			m_itemList[10] = m_daItem11;
			
			m_daItem12 = new MyDaItem("IOP.static.I1", m_daSubscription);					
			if (! m_daItem12.getValid())
			{
				return connectResult;
			}
			m_itemList[11] = m_daItem12;
			
			m_daItem13 = new MyDaItem("IOP.static.BOOL", m_daSubscription);					
			if (! m_daItem13.getValid())
			{
				return connectResult;
			}
			m_itemList[12] = m_daItem13;
			
			m_daItem14 = new MyDaItem("IOP.static.I2 array", m_daSubscription);					
			if (! m_daItem14.getValid())
			{
				return connectResult;
			}
			m_itemList[13] = m_daItem14;
			
			m_daItem15 = new MyDaItem("IOP.static.BSTR array", m_daSubscription);					
			if (! m_daItem15.getValid())
			{
				return connectResult;
			}
			m_itemList[14] = m_daItem15;
			
			m_daItem16 = new MyDaItem("IOP.static.BOOL array", m_daSubscription);					
			if (! m_daItem16.getValid())
			{
				return connectResult;
			}
			m_itemList[15] = m_daItem16;
			
			m_daItem17 = new MyDaItem("IOP.static.I4 array", m_daSubscription);					
			if (! m_daItem17.getValid())
			{
				return connectResult;
			}
			m_itemList[16] = m_daItem17;
			
			m_daItem18 = new MyDaItem("IOP.static.DATE array", m_daSubscription);					
			if (! m_daItem18.getValid())
			{
				return connectResult;
			}
			m_itemList[17] = m_daItem18;
			
			m_daItem19 = new MyDaItem("IOP.static.I8 array", m_daSubscription);					
			if (! m_daItem19.getValid())
			{
				return connectResult;
			}
			m_itemList[18] = m_daItem19;
			
			m_daItem20 = new MyDaItem("IOP.static.R4 array", m_daSubscription);					
			if (! m_daItem20.getValid())
			{
				return connectResult;
			}
			m_itemList[19] = m_daItem20;
			
			m_daItem21 = new MyDaItem("IOP.static.R8 array", m_daSubscription);					
			if (! m_daItem21.getValid())
			{
				return connectResult;
			}
			m_itemList[20] = m_daItem21;
			
			connectResult = m_daSession.connect(true, false, m_executionOptions);	
		}
		catch(Exception exc)
		{
			logger.error("InitializeDAObjetcs() - " + exc.getMessage());
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
	
	public String readItem()
	{
		String message = "";

		try
		{
			int readCount = 1;
			MyDaItem[] m_itemList = new MyDaItem[readCount];
			m_itemList[0] = m_daItem1;

			ArrayList<ValueQT> m_values = new ArrayList<ValueQT>();
			ArrayList<Integer> m_results = new ArrayList<Integer>();

			if (ResultCode.SUCCEEDED(
				m_daSubscription.read(
				0,
				m_itemList,
				m_values,
				m_results,
				null)))
			{
				message += " \nRead item synchronously using subscription \n";

				for (int i = 0; i< m_values.size();i++)
				{
					if (ResultCode.SUCCEEDED(m_results.get(i)))
					{
						message += " " + m_itemList[i].getId() + " - ";
						message += m_values.get(i).toString() + "\n\n";
					}
					else
					{
						message += "Read failed for item " + m_itemList[i].getId() + "\n\n";
					}	//	end if...else
				}	//	end for
			}
			else
			{
				message += " Subscription synchronous read failed!" + "\n\n";
			}	//	end if...else
		}
		catch(Exception exc)
		{
			logger.error("ReadItem() - " + exc.getMessage());
			/*
			GetApplication().Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.USER,
				"OpcClient::ReadItem",
				exc.getMessage());
			*/
		}	//	end try...catch
		return message;
	}	//	end ReadItem
	
	public void trace(
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
	
	public void activateSession(boolean sync)
	{
		System.out.println();
		if (sync)
		{
			int result = this.m_daSession.connect(true, true, new ExecutionOptions());		
			if (ResultCode.FAILED(result))
			{
				System.out.println(" Session activate failed!");
			}
		}// end if
		else
		{
			m_daSession.connect(true, true, m_executionOptions);	
			m_executionOptions.setExecutionContext(m_executionOptions.getExecutionContext() + 1);		
		} // end else
	} // end ActivateSession


	public void connectSession(boolean sync)
	{
		System.out.println();
		if (sync)
		{
			int result = m_daSession.connect(true, false, new ExecutionOptions());														
			if (ResultCode.FAILED(result))
			{
				System.out.println(" Session connect failed!");
			}
		} // end if
		else
		{
			m_daSession.connect(true, false, m_executionOptions);
			m_executionOptions.setExecutionContext(m_executionOptions.getExecutionContext() + 1);		
		} // end else
	} // end ConnectSession


	public void disconnectSession(boolean sync)
	{
		System.out.println();
		if (sync)
		{
			int result = m_daSession.disconnect(new ExecutionOptions());
			if (ResultCode.FAILED(result))
			{							
				System.out.println(" Session disconnect failed!");						
			}
		} // end if
		else 
		{
			m_daSession.disconnect(m_executionOptions);
			m_executionOptions.setExecutionContext(m_executionOptions.getExecutionContext() + 1);	
		} // end else

	} //end DisconnectSession
	
	public void readItems(boolean sync)
	{
		ValueQT itemValues = new ValueQT();
		Integer itemResult = -1;
		if (sync)
		{
			int readResult = m_daItem1.Read(100, itemValues, itemResult, new ExecutionOptions());
			if(ResultCode.SUCCEEDED(readResult))
			{
				System.out.println("Item synchronous read succeded");
				System.out.println( m_daItem1.getId() + "-" + itemValues.toString());										
			}
			else
			{	
				System.out.println("Synchronous item read failed! " + " Item: " + m_daItem1.getId() + " Result: " + readResult);
			}

			System.out.println();
			int result = EnumResultCode.E_FAIL.toInt();

			m_values = new ArrayList<ValueQT>();
			m_results = new ArrayList<Integer>();
				
			if (ResultCode.SUCCEEDED(result = m_daSubscription.read(100, m_itemList, m_values, m_results, new ExecutionOptions())))
			{
				System.out.println(" Subscription synchronous read succeeded");
				for (int i = 0; i< m_itemList.length;i++)
				{
					if (m_results.get(i) >= 0)
					{
						System.out.println(m_itemList[i].getId() + "-" + m_values.get(i).toString());
					}
					else
					{
						System.out.println(" Item read failed! " + " Item: " + m_itemList[i].getId() + " [ResultID: " + m_results.get(i) + " ]");
					}
				}
			}
			else
			{
				System.out.println(" Synchronous subscription read failed! Result: " + result);
			}

			System.out.println();
		}				
		else 
		{
			m_values = new ArrayList<ValueQT>();
			m_results = new ArrayList<Integer>();				
			m_daItem5.Read(100, itemValues, itemResult, m_executionOptions);
			
			m_executionOptions.setExecutionContext(m_executionOptions.getExecutionContext() + 1);
							
			m_daSubscription.read(100, m_itemList, m_values, m_results, m_executionOptions);
			m_executionOptions.setExecutionContext(m_executionOptions.getExecutionContext() + 1);

		} // end else

	} //end ReadItems
	
	public void writeItems(boolean sync)
	{
		Date writeDateTime = new Date();
		if (sync)
		{
			ValueQT[] inValues = new ValueQT[19];
													
			Byte[] v1 = new Byte[5];
			v1[0] = 7; v1[1] = 8; v1[2] = 9; v1[3] = 10; v1[4] =11; 
			inValues[0] = new ValueQT( v1, EnumQuality.QUALITY_NOT_SET, writeDateTime);

			String v2 = "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ";
			inValues[1] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[2] = new ValueQT(new Short((short)1969), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[3] = new ValueQT(new Integer(654456), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[4] = new ValueQT(new Long(77777777), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[5] = new ValueQT(new Float(89.654), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[6] = new ValueQT(new Double(3.14159265358979323846264338), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[7] = new ValueQT(new Boolean(true), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[8] = new ValueQT(new Date(), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			
			inValues[9] = new ValueQT(new Byte((byte)9), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[10] = new ValueQT(new Boolean(true), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			Short[] sarray = new Short[] {new Short((short)23), new Short((short)47)};
			inValues[11] = new ValueQT(sarray , EnumQuality.QUALITY_NOT_SET, writeDateTime);
			
			String[] strarray = new String[] {new String("Mando"), new String("Diao")};
			inValues[12] = new ValueQT(strarray , EnumQuality.QUALITY_NOT_SET, writeDateTime);
			
			Boolean[] barray = new Boolean[] {new Boolean(false), new Boolean(true)};
			inValues[13] = new ValueQT(barray , EnumQuality.QUALITY_NOT_SET, writeDateTime);
			Integer[] iarray = new Integer[] {new Integer(123), new Integer(456)};
			inValues[14] = new ValueQT(iarray , EnumQuality.QUALITY_NOT_SET, writeDateTime);
			
			Date[] darray = new Date[] {new Date(), new Date()};
			inValues[15] = new ValueQT(darray , EnumQuality.QUALITY_NOT_SET, writeDateTime);
			
			Long[] larray = new Long[] {new Long(888888888888L), new Long(999999999999L)};
			inValues[16] = new ValueQT(larray , EnumQuality.QUALITY_NOT_SET, writeDateTime);
			
			Float[] farray = new Float[] {new Float(0.888888888888), new Float(0.999999999999)};
			inValues[17] = new ValueQT(farray , EnumQuality.QUALITY_NOT_SET, writeDateTime);
			Double[] dblarray = new Double[] {new Double(0.1324354657), new Double(0.978675534211)};
			inValues[18] = new ValueQT(dblarray , EnumQuality.QUALITY_NOT_SET, writeDateTime);
			
			int result = EnumResultCode.E_FAIL.toInt();

			MyDaItem[] inItems = new MyDaItem[19];
			inItems[0] = m_daItem3;
			inItems[1] = m_daItem4;	
			inItems[2] = m_daItem5;	
			inItems[3] = m_daItem6;
			inItems[4] = m_daItem7;	
			inItems[5] = m_daItem8;	
			inItems[6] = m_daItem9;
			inItems[7] = m_daItem10;	
			inItems[8] = m_daItem11;
			
			inItems[9] = m_daItem12;	
			inItems[10] = m_daItem13;
			inItems[11] = m_daItem14;
			inItems[12] = m_daItem15;
			inItems[13] = m_daItem16;
			inItems[14] = m_daItem17;
			inItems[15] = m_daItem18;
			inItems[16] = m_daItem19;
			inItems[17] = m_daItem20;
			inItems[18] = m_daItem21;
			
			ArrayList<Integer> inResults = new ArrayList<Integer>();

			if (ResultCode.SUCCEEDED(result = m_daSubscription.write(inItems, inValues, inResults, new ExecutionOptions())))
			{	
						
				System.out.println(" Subscription synchronous write succeeded");
				for (int i = 0; i< inItems.length;i++)
				{
					if (inResults.get(i) >= 0)
					{
						System.out.println( inItems[i] + "-" + inValues[i].toString());	
						System.out.println();
					}
					else
					{
						System.out.println(" Item synchronous write failed!  " + inItems[i].getId()  + " [ResultID: " + inResults.get(i) + " ]");
					}	
				}
			}
			else
			{
				System.out.println("Subscription synchronous write failed! Result: " + result);
			}		
		} // end if
		else 
		{
			
			ValueQT[] inValues = new ValueQT[9];
			Byte[] v1 = new Byte[5];
			v1[0] = 7; v1[1] = 8; v1[2] = 9; v1[3] = 10; v1[4] = 11; 
			inValues[0] = new ValueQT(v1, EnumQuality.QUALITY_NOT_SET, writeDateTime);
			String v2 = "SOFTING";
			inValues[1] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[2] = new ValueQT(new Short((short)666), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[3] = new ValueQT(new Integer(10000), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[4] = new ValueQT(new Long(23232323), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[5] = new ValueQT(new Float(65.432), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[6] = new ValueQT(new Double(2.190489838784), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[7] = new ValueQT(new Boolean(false), EnumQuality.QUALITY_NOT_SET, writeDateTime);
			inValues[8] = new ValueQT(new Date(), EnumQuality.QUALITY_NOT_SET, writeDateTime);			
			
			
			MyDaItem[] inItems = new MyDaItem[9];
			inItems[0] = m_daItem3;
			inItems[1] = m_daItem4;
			inItems[2] = m_daItem5;	
			inItems[3] = m_daItem6;
			inItems[4] = m_daItem7;	
			inItems[5] = m_daItem8;	
			inItems[6] = m_daItem9;
			inItems[7] = m_daItem10;	
			inItems[8] = m_daItem11;
			
			
			ArrayList<Integer> inResults = new ArrayList<Integer>();

			m_daSubscription.write(inItems, inValues, inResults, m_executionOptions);
			m_executionOptions.setExecutionContext(m_executionOptions.getExecutionContext() + 1);
		} // end else

	} //end WriteItems
	
	public void readUsingSession(boolean sync)
	{
		if (sync)
		{
			int result = EnumResultCode.E_FAIL.toInt();
			// in case of a XML-DA server use a valid item paths array instead of "null"
			m_values = new ArrayList<ValueQT>();
			m_results = new ArrayList<Integer>();
			
			if (ResultCode.SUCCEEDED(result = m_daSession.read(0, m_itemIds, null, m_values, m_results, new ExecutionOptions())))
			{
				System.out.println(" Session synchronous read succeeded!");
				for (int i = 0; i< m_itemList.length;i++)
				{
					if (m_results.get(i) >= 0)
					{
						System.out.println( m_itemIds[i] + "-" + m_values.get(i).toString());	
					}
					else
					{
						System.out.println(" Synchronous read failed for item " + m_itemList[i].getId() + " [ResultID: " + m_results.get(i) + " ]");
					}
				}
			}
			else
			{
				System.out.println(" Session synchronous read failed! Result: " + result);
			}
			System.out.println();
		} // end if
		else 
		{
			// in case of a XML-DA server use a valid item paths array instead of "null"
			m_values = new ArrayList<ValueQT>();
			m_results = new ArrayList<Integer>();
			
			m_daSession.read(0, m_itemIds, null, m_values, m_results, m_executionOptions);
			m_executionOptions.setExecutionContext(m_executionOptions.getExecutionContext() + 1);	
		} // end else

	} //end ReadUsingSession
	
	public void writeUsingSession(boolean sync)
	{
		Date writeDateTime = new Date();
		if (sync)
		{
			ValueQT[] inValues = new ValueQT[2];
			ArrayList<Integer> inResults = new ArrayList<Integer>();
			String[] inItemIds = new String[]{"IOP.static.I1 array","IOP.static.BSTR"};
			//String[] inItemIds = new String[]{"IOP.static.BSTR"};
			int result = EnumResultCode.E_FAIL.toInt();

			Byte[] v2 = new Byte[5];
			v2[0] = 7; v2[1] = 8; v2[2] = 9; v2[3] = 10; v2[4] =11; 
			inValues[0] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);

			String v3 = "SOFTING";
			inValues[1] = new ValueQT(v3, EnumQuality.QUALITY_NOT_SET, writeDateTime);
							
			// in case of a XML-DA server use a valid item paths array instead of "null"
			if (ResultCode.SUCCEEDED(result = m_daSession.write(inItemIds, null, inValues, inResults, new ExecutionOptions())))
			{
				System.out.println(" Session synchronous write succeeded");
				for (int i = 0; i< inItemIds.length;i++)
				{
					if (inResults.get(i) >= 0)
					{
						System.out.println( inItemIds[i] + "-" + inValues[i].toString());	
						System.out.println();
					}
					else
					{
						System.out.println(" Item synchronous write failed! " + " Item: " + inItemIds[i] + " [ResultID: " + inResults.get(i) + " ]");
					}
				}
			}
			else
			{
				System.out.println(" Session synchronous write failed! Result: " + result);
			}
			System.out.println();
		} // end if
		else 
		{
			ValueQT[] inValues = new ValueQT[2];
			ArrayList<Integer> inResults = new ArrayList<Integer>();
						
			String[] inItemIds = new String[]{"IOP.static.I1 array", "IOP.static.BSTR"};		
			//String[] inItemIds = new String[]{"IOP.static.BSTR"};

			Byte[] v2 = new Byte[5];
			v2[0] = 7; v2[1] = 8; v2[2] = 9; v2[3] = 10; v2[4] =11; 
			inValues[0] = new ValueQT(v2, EnumQuality.QUALITY_NOT_SET, writeDateTime);

			String v3 = "SOFTING";
			inValues[1] = new ValueQT(v3, EnumQuality.QUALITY_NOT_SET, writeDateTime);								

			// in case of a XML-DA server use a valid item paths array instead of "null"
			m_daSession.write(inItemIds, null, inValues, inResults, m_executionOptions);
			m_executionOptions.setExecutionContext(m_executionOptions.getExecutionContext() + 1);
		} // end else

	} //end WriteUsingSession
	
	
	public void getServerStatus(boolean sync)
	{
		System.out.println();
		ServerStatus newServerStatus = new ServerStatus();
		ServerStatus serverStatus = new ServerStatus();
		if (sync)
		{
			int result = EnumResultCode.E_FAIL.toInt();

			if (ResultCode.SUCCEEDED(result = m_daSession.getStatus(serverStatus, new ExecutionOptions()))) 
			{
								
				System.out.println("Server Status");
				System.out.println("	Vendor info: " + serverStatus.getVendorInfo());
				System.out.println("	Product version: " + serverStatus.getProductVersion());
				System.out.println("	State: " + serverStatus.getState());
				System.out.println("	Start time: "+ serverStatus.getStartTime().toString());
				System.out.println("	Last update time: "+ serverStatus.getLastUpdateTime().toString());
				System.out.println("	Current time: "+ serverStatus.getCurrentTime().toString());
				System.out.println("	GroupCount: " + serverStatus.getGroupCount());
				System.out.println("	Bandwidth: " + serverStatus.getBandwidth());
				for (int i = 0; i< serverStatus.getSupportedLcIds().length; i++)
				{			
					System.out.println("	Supported LcId: " + serverStatus.getSupportedLcIds()[i]);
				}
				System.out.println("	Status info: " + serverStatus.getStatusInfo());
			}	
			else 
			{
				System.out.println("Synchronous get status failed! Result: " + result);
			}
		} // end if
		else 
		{
			m_daSession.getStatus(newServerStatus, m_executionOptions);
			m_executionOptions.setExecutionContext(m_executionOptions.getExecutionContext() + 1);
		} // end else

	} //end GetServerStatus
	
	
	public void activateConnectionMonitor()
	{
		int result = EnumResultCode.E_FAIL.toInt();
		if(ResultCode.SUCCEEDED( result = m_daSession.activateConnectionMonitor(true, 5000, 0, 10000, 300000)))
		{
			System.out.println();
			System.out.println("Activated connection monitor");
			System.out.println();
		}
		else
		{
			System.out.println("Activate connection monitor failed! Result " + result);
		}	
	} //end ActivateConnectionMonitor
	
	public void deactivateConnectionMonitor()
	{
		int result = EnumResultCode.E_FAIL.toInt();
		if(ResultCode.SUCCEEDED(result = m_daSession.activateConnectionMonitor(false, 0, 0, 0, 0)))
		{
			System.out.println();
			System.out.println("Deactivated connection monitor");
			System.out.println();
		}
		else
		{
			System.out.println("Deactivate connection monitor failed! Result: " + result);
		}		
	} //end DeactivateConnectionMonitor	
	public String getServiceName()
	{
		return Application.INSTANCE.getServiceName();
	}
	
	public void setServiceName(String value)
	{
		Application.INSTANCE.setServiceName(value);
	}
}
