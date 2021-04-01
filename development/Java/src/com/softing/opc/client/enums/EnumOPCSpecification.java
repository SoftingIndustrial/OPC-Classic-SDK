package com.softing.opc.client.enums;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import com.softing.opc.client.impl.jna.OTClient;

//see enums.cs

public enum EnumOPCSpecification {

	DEFAULT (OTClient.OT_OPCSPECIFICATION_DEFAULT),
	DA10 (OTClient.OT_OPCSPECIFICATION_DA10),
	DA20 (OTClient.OT_OPCSPECIFICATION_DA20),
	DA30 (OTClient.OT_OPCSPECIFICATION_DA30),
	AE10 (OTClient.OT_OPCSPECIFICATION_AE10),
	XMLDA10 (OTClient.OT_OPCSPECIFICATION_XMLDA10),
	ALL (OTClient.OT_OPCSPECIFICATION_ALL);
	
	private final short opcSpecification;
	private static final Map<Short, EnumOPCSpecification> idToInstance;
    static{ 
        HashMap<Short, EnumOPCSpecification> temp = new HashMap<Short, EnumOPCSpecification>(); 
        for (EnumOPCSpecification value : values()) { 
            temp.put(value.toShort(), value); 
        } 
        idToInstance = Collections.unmodifiableMap(temp); 
      }
    
	EnumOPCSpecification(int value)
	{
		opcSpecification = (short)value;
	}
	
	public short toShort()
	{
		return opcSpecification;
	}
	//changed at i4t request 16.11.2009 (TOS)
	/*
	public static EnumOPCSpecification getEnum(short opcSpecification_)
	{
		for(EnumOPCSpecification var : EnumOPCSpecification.values())
		{
			if(var.toShort() == opcSpecification_)
			{
				return var;
			}
		}
		return null;
	}*/
    public static EnumOPCSpecification getEnum(short opcSpecification_){ 
        return idToInstance.get(opcSpecification_); 
    }
    
	
}