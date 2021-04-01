package com.softing.opc.client.enums;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import com.softing.opc.client.impl.jna.OTClient;

//see enums.cs
public enum EnumQuality {
	
	GOOD (OTClient.OT_QUALITY_GOOD),
	GOOD_LOCAL_OVERRIDE (OTClient.OT_QUALITY_LOCAL_OVERRIDE),
	BAD (OTClient.OT_QUALITY_BAD),
	BAD_CONFIG_ERROR (OTClient.OT_QUALITY_CONFIG_ERROR),
	BAD_NOT_CONNECTED (OTClient.OT_QUALITY_NOT_CONNECTED),
	BAD_DEVICE_FAILURE (OTClient.OT_QUALITY_DEVICE_FAILURE),
	BAD_SENSOR_FAILURE (OTClient.OT_QUALITY_SENSOR_FAILURE),
	BAD_LAST_KNOWN (OTClient.OT_QUALITY_LAST_KNOWN),
	BAD_COMM_FAILURE (OTClient.OT_QUALITY_COMM_FAILURE),
	BAD_OUT_OF_SERVICE (OTClient.OT_QUALITY_OUT_OF_SERVICE),
	BAD_WAITING_FOR_INITIAL_DATA (OTClient.OT_QUALITY_WAITING_FOR_INITIAL_DATA),
	UNCERTAIN (OTClient.OT_QUALITY_UNCERTAIN),
	UNCERTAIN_LAST_USABLE (OTClient.OT_QUALITY_LAST_USABLE),
	UNCERTAIN_SENSOR_CAL (OTClient.OT_QUALITY_SENSOR_CAL),
	UNCERTAIN_EGU_EXCEEDED (OTClient.OT_QUALITY_EGU_EXCEEDED),
	UNCERTAIN_SUB_NORMAL (OTClient.OT_QUALITY_SUB_NORMAL),
	QUALITY_NOT_SET (OTClient.OT_QUALITY_NOT_SET);
	
	private final short quality;
	
	private static final Map<Short, EnumQuality> idToInstance;
    static{ 
        HashMap<Short, EnumQuality> temp = new HashMap<Short, EnumQuality>(); 
        for (EnumQuality value : values()) { 
            temp.put(value.toShort(), value); 
        } 
        idToInstance = Collections.unmodifiableMap(temp); 
      }
    
	EnumQuality(int value)
	{
		quality = (short)value;
	}
	
	public short toShort()
	{
		return quality;
	}
	
	//changed at i4t request 16.11.2009 (TOS)
	/*
	public static EnumQuality getEnum(short quality_)
	{
		for(EnumQuality var : EnumQuality.values())
		{
			if(var.toShort() == quality_)
			{
				return var;
			}
		}
		return null;
	}*/
    public static EnumQuality getEnum(short quality_){ 
        return idToInstance.get(quality_); 
    }
    
}
