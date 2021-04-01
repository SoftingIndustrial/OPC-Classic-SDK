package com.softing.opc.client.enums;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import com.softing.opc.client.impl.jna.OTClient;

public enum EnumServerBrowserData {

	/**
	 * A 128-bit value which uniquely identifies a server.
	 */
	CLASSID (OTClient.OTC_SERVERDATA_CLSID),
	/**
	 * The server's ProgID.
	 */
	PROGID (OTClient.OTC_SERVERDATA_PROGID),
	/**
	 * The server's version independent program ID.
	 */
	PROGIDVERSIONINDEPENDENT (OTClient.OTC_SERVERDATA_VERSIONINDEPENDENTPROGID),
	/**
	 * The server's description.
	 */
	DESCRIPTION (OTClient.OTC_SERVERDATA_DESCRIPTION),
	/**
	 * The server's url.
	 */
	URL (OTClient.OTC_SERVERDATA_URL),
	/**
	 * The Opc specifications which the server supports.
	 */
	OPCSPECIFICATIONS (OTClient.OTC_SERVERDATA_OPCSPECIFICATIONS),
	/**
	 * All the above data.
	 */
	ALL (OTClient.OTC_SERVERDATA_ALL);
	
	private final int browserData_;
	
	private static final Map<Integer, EnumServerBrowserData> idToInstance;
    static{ 
        HashMap<Integer, EnumServerBrowserData> temp = new HashMap<Integer, EnumServerBrowserData>(); 
        for (EnumServerBrowserData value : values()) { 
            temp.put(value.getInt(), value); 
        } 
        idToInstance = Collections.unmodifiableMap(temp); 
      }
    
	private EnumServerBrowserData(int serverBrowserData){
		this.browserData_ = serverBrowserData;
	}
	
	public int getInt() { return browserData_; } 
	
	//changed at i4t request 16.11.2009 (TOS)
	/*
	public static EnumServerBrowserData getEnum(int value)
	{
		for(EnumServerBrowserData var : EnumServerBrowserData.values())
		{
			if(var.getInt() == value)
			{
				return var;
			}
		}
		return null;
	}*/
    public static EnumServerBrowserData getEnum(int value){ 
        return idToInstance.get(value); 
    }
}