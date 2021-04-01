package com.softing.opc.client.enums;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import com.softing.opc.client.impl.jna.OTClient;

//see enums.cs

public enum EnumEUType {

	NOENUM (OTClient.OTC_DAITEM_EUTYPE_NO),
	ANALOG (OTClient.OTC_DAITEM_EUTYPE_ANALOG),
	ENUMERATED (OTClient.OTC_DAITEM_EUTYPE_ENUMERATED);
	
	private final short euType;
	private static final Map<Short, EnumEUType> idToInstance;
    static{ 
        HashMap<Short, EnumEUType> temp = new HashMap<Short, EnumEUType>(); 
        for (EnumEUType value : values()) { 
            temp.put(value.toShort(), value); 
        } 
        idToInstance = Collections.unmodifiableMap(temp); 
      }
    
	EnumEUType(int value)
	{
		euType = (short)value;
	}
	
	public short toShort()
	{
		return euType;
	}
	//changed at i4t request 16.11.2009 (TOS)
	/*
	public static EnumEUType getEnum(int euType_)
	{
		for(EnumEUType var : EnumEUType.values())
		{
			if(var.toShort() == euType_)
			{
				return var;
			}
		}
		return null;
	}*/
	
    public static EnumEUType getEnum(int euType_){ 
        return idToInstance.get(euType_); 
    } 
    
	
}
