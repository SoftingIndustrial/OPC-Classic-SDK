package daConsole;

import com.softing.opc.client.api.DaSession;
import com.softing.opc.client.api.ServerStatus;
import com.softing.opc.client.api.ValueQT;
import com.softing.opc.client.enums.EnumObjectState;
import com.softing.opc.client.enums.ResultCode;


public class MyDaSession extends DaSession {

	public MyDaSession(String url)
	{
		super(url);
	}
	
	protected void onStateChange(EnumObjectState newState){
		System.out.println( this.toString() + " State Changed - " + newState);
	}
	
	protected void onPerformStateTransition(int executionContext, int result){
		if(ResultCode.SUCCEEDED(result))
		{
			System.out.println( this.toString() + " Performed state transition - "  + executionContext );
		}
		else
		{
			System.out.println(this.toString() + "  Performed state transition failed! Result: " + result);
		}
	}
	
	protected boolean onShutdown(String reason){
		System.out.println();
		System.out.println(this.toString() + " Shutdown - " + reason);
		System.out.println();
		return true; // reconnection will be performed
	}
	
	protected void onReadComplete(int executionContext, String[] itemIDs, String[] itemPaths, ValueQT[] values, int[] results, int result){
		if(ResultCode.SUCCEEDED(result))
		{
			System.out.println();
			System.out.println(this.toString() + " asynchronous read succeeded! ");
			for (int i = 0; i< itemIDs.length;i++)
			{
				if (ResultCode.SUCCEEDED(results[i]))
				{
					System.out.println( itemIDs[i] + "-" + values[i].toString());						
				}
				else
				{
					System.out.println(" Session asynchronous read failed for item " + " Item: " + itemIDs[i] + " [ResultID: " + results[i] + " ]");
				}
			}
		}
		else
		{
			System.out.println(" Session asynchronous read failed! Result: " + result);
		}
	}


	protected void onWriteComplete(int executionContext, String[] itemIDs, String[] itemPaths, ValueQT[] values, int[] results, int result){
		if(ResultCode.SUCCEEDED(result))
		{
			System.out.println();
			System.out.println(this.toString() + " asynchronous write succeeded! ");
			for(int i = 0 ; i< itemIDs.length; i++)
			{
				if (ResultCode.SUCCEEDED(results[i]))
				{
					System.out.println( itemIDs[i] + "-" + values[i].toString());						
				}
				else
				{
					System.out.println(" Session asynchronous write for item " + itemIDs[i]+ " [ResultID: " + results[i] + " ]");
				}				
			}
		}
		else
		{
			System.out.println("Session asynchronous write failed! Result:" + result);
		}
	}
	
	protected void onGetServerStatus(int executionContext, ServerStatus serverStatus, int result){
		if(ResultCode.SUCCEEDED(result))
		{
			System.out.println(this);
			System.out.println("Server Status");
			System.out.println("	Vendor info: " + serverStatus.getVendorInfo());
			System.out.println("	Product version: " + serverStatus.getProductVersion());
			System.out.println("	State: " + serverStatus.getState());
			System.out.println("	Start time: "+ serverStatus.getStartTime());
			System.out.println("	Last update time: "+ serverStatus.getLastUpdateTime());
			System.out.println("	Current time: "+ serverStatus.getCurrentTime());
			System.out.println("	GroupCount: " + serverStatus.getGroupCount());
			System.out.println("	Bandwidth: " + serverStatus.getBandwidth());
			for (int i = 0; i< serverStatus.getSupportedLcIds().length; i++)
			{
				System.out.println("	Supported LCID: " + serverStatus.getSupportedLcIds()[i]);
			}
			System.out.println("	Status info: " + serverStatus.getStatusInfo());
		}
		else
		{
			System.out.println("Asynchronous get server status failed! Result: " + result);
		}
	}
	
}
