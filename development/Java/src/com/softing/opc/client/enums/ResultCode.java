package com.softing.opc.client.enums;

public class ResultCode {

	static final int ERROR_MASK = 0x80000000;
	static final int SUCCESS_MASK = 0x00000000;
	
	public ResultCode(){}
	
	static public boolean SUCCEEDED(int res) 
	{
		return ((res & ERROR_MASK) == SUCCESS_MASK);
	}
	
	static public boolean SUCCEEDED(EnumResultCode res) {
		return (((res.toInt()) & ERROR_MASK) == SUCCESS_MASK);
	}
	
	static public boolean FAILED(int res) {
		return ((res & ERROR_MASK ) == ERROR_MASK);
	}
	
	static public boolean FAILED(EnumResultCode res) {
		return (((res.toInt()) & ERROR_MASK ) == ERROR_MASK);
	}		
}
