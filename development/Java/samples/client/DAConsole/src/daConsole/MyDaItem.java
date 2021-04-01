package daConsole;

import com.softing.opc.client.api.DaItem;
import com.softing.opc.client.api.ValueQT;
import com.softing.opc.client.enums.EnumObjectState;
import com.softing.opc.client.enums.ResultCode;


public class MyDaItem extends DaItem{
	
	public MyDaItem(String itemId, MyDaSubscription parentSubscription)
	{
		super(itemId, parentSubscription);
	}
	
	protected void onValueChange(ValueQT aValue, int result){
		if (this.getId() == "maths.sin")
		{
			System.out.println("Value changed!");
			System.out.println( this.getId()+ "-" + aValue.toString());		
		}		
	}
	
	protected void onStateChange(EnumObjectState newState){
		System.out.println( this.toString() + " " + this.getId() + " State Changed - " + newState);
	}

	protected void onPerformStateTransition(int executionContext, int result){
		if(ResultCode.SUCCEEDED(result))
		{                   
			System.out.println( this.toString() + " " + this.getId() + " Performed state transition - "  + executionContext );
		}
		else
		{
			System.out.println(this.toString() + "  Performed state transition failed! Result: " + result);
		}
	}
	
	
}
