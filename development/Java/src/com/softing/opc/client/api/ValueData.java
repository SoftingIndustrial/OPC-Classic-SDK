package com.softing.opc.client.api;

import com.softing.opc.client.impl.jna.VARIANT;


public class ValueData {

	//protected OTClient.VARIANT data_ = null;
	protected Object data_ = null;
	
	public void finalize() throws Throwable {
	}


	public ValueData(Object aData){
		data_ = aData;
	}

	public ValueData(){

	}


	public Object getObject(){
			return data_;
	}

	/*public String toString(){
		if(data_ == null)
		{
			return "";
		}
				
		return data_.toString();
	}*/
	
	public String toString()
	{
		if(data_ != null)
		{
			if(data_ instanceof Object[])
			{
				StringBuffer strBuf = new StringBuffer();
				strBuf.append("{");
	
				Object[] objArray = (Object[])data_;
				boolean bExists = false;
				for(int i = 0; i < objArray.length; i++)
				{
					if(bExists)
					{
						strBuf.append(",");
					}
					strBuf.append(objArray[i].toString());
					bExists = true;
				}
	
				strBuf.append("}");
				return strBuf.toString();
			}
			else
			{
				return data_.toString();
			}
		}
		else
		{
			return "null";
		}
	}

	protected static short getVartype(Class input){
		return VARIANT.getType(input);
	}

	public static short getVartype(Object input){
		return VARIANT.getType(input);
	}

	protected static Class getSysType(short anInput){
		return VARIANT.getSysType(anInput);
	}

}