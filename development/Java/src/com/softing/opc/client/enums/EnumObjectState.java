package com.softing.opc.client.enums;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

public enum EnumObjectState {

	/**
	 * There is no connection between a server and a client.
	 */
	DISCONNECTED (0x00),
	/**
	 * Client is connected to the server but they don't comunicate with each other.
	 */
	CONNECTED (0x01),
	/**
	 * Client and server communicate with each other via a callback connection.
	 */
	ACTIVATED (0x03);
	
	private final int objectState_;
	private static final Map<Integer, EnumObjectState> idToInstance;
    static{ 
        HashMap<Integer, EnumObjectState> temp = new HashMap<Integer, EnumObjectState>(); 
        for (EnumObjectState value : values()) { 
            temp.put(value.getInt(), value); 
        } 
        idToInstance = Collections.unmodifiableMap(temp); 
      }
    
	private EnumObjectState(int state){
		this.objectState_ = state;
	}
	
	public int getInt() { return objectState_; }
	
	//changed at i4t request 16.11.2009 (TOS)
	/*
	public static EnumObjectState getEnum(int objectState_)
	{
		for(EnumObjectState var : EnumObjectState.values())
		{
			if(var.getInt() == objectState_)
			{
				return var;
			}
		}
		return null;
	}*/
    public static EnumObjectState getEnum(int objState_){ 
        return idToInstance.get(objState_); 
    }
}