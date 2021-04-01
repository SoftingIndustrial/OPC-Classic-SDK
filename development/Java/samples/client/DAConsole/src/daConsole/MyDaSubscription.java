package daConsole;

import com.softing.opc.client.api.DaItem;
import com.softing.opc.client.api.DaSubscription;
import com.softing.opc.client.api.ValueQT;
import com.softing.opc.client.enums.EnumObjectState;
import com.softing.opc.client.enums.ResultCode;


public class MyDaSubscription extends DaSubscription{


	
	public MyDaSubscription(int updateRate, MyDaSession parentSession)
	{
		super(updateRate, parentSession);
	}
	
	protected void onStateChange(EnumObjectState newState){
		System.out.println( this + " State Changed - " + newState);
	}
	
	protected void onDataChange(DaItem[] items, ValueQT[] values, int[] results){
		System.out.println();
		System.out.println("Subscription id:" + this.getHandle());
		for (int i = 0; i < items.length; i++)
		{
			if(ResultCode.SUCCEEDED(results[i]))
			{	
				System.out.println( items[i].getId() + "-" + values[i].toString());					
			}
			else
			{
				System.out.println("Receiving item value failed! " + " Item: " + items[i].getId() );
			}
		}
		System.out.println();
	}
	
	protected void onReadComplete(int executionContext, DaItem[] items, ValueQT[] values, int[] results, int result){
		if(ResultCode.SUCCEEDED(result))
		{
			System.out.println();
			System.out.println(this + " asynchronous read succeeded! ");

			for(int i = 0 ; i< values.length; i++)
			{
				if (ResultCode.SUCCEEDED(results[i]))
				{
					System.out.println( items[i].getId() + "-" + values[i].toString());						
				}
				else
				{
					System.out.println(" Item read failed!" + items[i].getId() + " [ResultID: " + results[i] + " ]" );
				}
			}
		}
		else
		{
			System.out.println(" Subscription asynchronous read failed! Result: " + result);
		}
	}


	protected void onWriteComplete(int executionContext, DaItem[] items, ValueQT[] values, int[] results, int result){
		if(ResultCode.SUCCEEDED(result))
		{
			System.out.println(this + " asynchronous write succeeded! ");
			for(int i = 0 ; i< items.length; i++)
			{
				if (ResultCode.SUCCEEDED(results[i]))
				{
					System.out.println(" " + items[i].getId() );
				}
				else
				{
					System.out.println(" aSubscription asynchronous write failed! " + items[i].getId() + " [ResultID: " + results[i] + " ]");
				}		
			}
		}
		else
		{
			System.out.println(" Subscription asynchronous write failed! Result: " + result);
		}
	}
	
	protected void onPerformStateTransition(int executionContext, int result){
		if(ResultCode.SUCCEEDED(result))
		{
			System.out.println( this + " Performed state transition - "  + executionContext );
		}
		else
		{
			System.out.println(this + "  Performed state transition failed! Result: " + result);
		}
	}
	
}
