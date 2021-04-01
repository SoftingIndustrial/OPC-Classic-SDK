package com.softing.opc.client.enums;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import com.softing.opc.client.impl.jna.OTClient;


//see enums.cs
public enum EnumAccessRights {
	
	READABLE (OTClient.OT_ACCESSRIGHTS_READABLE),
	WRITEABLE (OTClient.OT_ACCESSRIGHTS_WRITEABLE),
	READWRITEABLE (0x03);
	
	private static final Map<Short, EnumAccessRights> idToInstance;
    static{ 
        HashMap<Short, EnumAccessRights> temp = new HashMap<Short, EnumAccessRights>(); 
        for (EnumAccessRights value : values()) { 
            temp.put(value.toShort(), value); 
        } 
        idToInstance = Collections.unmodifiableMap(temp); 
      }
    
	private final short accessRights;
	
	EnumAccessRights(int value)
	{
		accessRights = (short)value;
	}
	
	public short toShort()
	{
		return accessRights;
	}
	
	//changed at i4t request 16.11.2009 (TOS)
	/*public static EnumAccessRights getEnum(int accessRights_)
	{
		for(EnumAccessRights var : EnumAccessRights.values())
		{
			if(var.toShort() == accessRights_)
			{
				return var;
			}
		}
		return null;
	}*/
	
    public static EnumAccessRights getEnum(int accessRights_){ 
        return idToInstance.get(accessRights_); 
    } 

}
