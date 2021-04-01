package com.softing.opc.client.enums;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import com.softing.opc.client.impl.jna.OTClient;

public enum EnumClassContext {
	/**
	 * Not a COM object.
	 */
	NOT_COM (OTClient.OTC_COM_CLSCTX_NOT_COM),
	/**
	 * The code that creates and manages objects of this class is a DLL that runs in
	 * the same process as the caller of the function specifying the class context.
	 */
	INPROC_SERVER (OTClient.OTC_COM_CLSCTX_INPROC_SERVER),
	/**
	 * The EXE code that creates and manages objects of this class runs on same
	 * machine but is loaded in a separate process space.
	 */
	LOCAL_SERVER (OTClient.OTC_COM_CLSCTX_LOCAL_SERVER),
	/**
	 * A remote machine context.
	 */
	REMOTE_SERVER (OTClient.OTC_COM_CLSCTX_REMOTE_SERVER),
	/**
	 * A COM server.
	 */
	SERVER (OTClient.OTC_COM_CLSCTX_SERVER);
	
	private final int contextId;
	private static final Map<Integer, EnumClassContext> idToInstance;
    static{ 
        HashMap<Integer, EnumClassContext> temp = new HashMap<Integer, EnumClassContext>(); 
        for (EnumClassContext value : values()) { 
        	temp.put(value.getInt(), value); 
        } 
        idToInstance = Collections.unmodifiableMap(temp); 
      }
    
	private EnumClassContext(int classContext){
		this.contextId = classContext;
	}
	
	public int getInt() { return contextId; }

	//changed at i4t request - 16.11.2009 (TOS)
	/*
	public static EnumClassContext getEnum(int classContext_)
	{
		for(EnumClassContext var : EnumClassContext.values())
		{
			if(var.getInt() == classContext_)
			{
				return var;
			}
		}
		return null;
	}*/
	
    public static EnumClassContext getEnum(int contextId_){ 
        return idToInstance.get(contextId_); 
    } 
    
}