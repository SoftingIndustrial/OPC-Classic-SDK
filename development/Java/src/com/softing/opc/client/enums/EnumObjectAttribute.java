package com.softing.opc.client.enums;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import com.softing.opc.client.impl.jna.OTClient;

public enum EnumObjectAttribute {

	/**
	 * All the attributes available for a <see cref="DaSession"/> object.
	 */
	DASESSION_ALL (OTClient.OTC_ATTRIBUTE_DASESSION_ALL),
	/**
	 * The name of the client for a <see cref="DaSession"/> object.
	 */
	DASESSION_CLIENTNAME (OTClient.OTC_ATTRIBUTE_DASESSION_CLIENTNAME),
	/**
	 * The locale identifier of a <see cref="DaSession"/> object.
	 */
	DASESSION_LCID (OTClient.OTC_ATTRIBUTE_DASESSION_LCID),
	/**
	 * The url of a <see cref="DaSession"/> object.
	 */
	DASESSION_URL (OTClient.OTC_ATTRIBUTE_DASESSION_URL),
	/**
	 * The forced OPC specification of a <see cref="DaSession"/> object.
	 */
	DASESSION_FORCEDOPCSPECIFICATION (OTClient.OTC_ATTRIBUTE_DASESSION_FORCEDOPCSPECIFICATION),
	/**
	 * The supported Opc specification of a <see cref="DaSession"/> object.
	 */
	DASESSION_SUPPORTEDOPCSPECIFICATION (OTClient.OTC_ATTRIBUTE_DASESSION_SUPPORTEDOPCSPECIFICATION),
	/**
	 * The execution context in which a Data Access server is to be run.
	 */
	DASESSION_CLASSCONTEXT (OTClient.OTC_ATTRIBUTE_DASESSION_COM_CLSCTX),
	/**
	 * All attributes of a <see cref="DaSubscription"/> object.
	 */
	DASUBSCRIPTION_ALL (OTClient.OTC_ATTRIBUTE_DASUBSCRIPTION_ALL),
	/**
	 * The name of a <see cref="DaSubscription"/> object.
	 */
	DASUBSCRIPTION_NAME (OTClient.OTC_ATTRIBUTE_DASUBSCRIPTION_NAME),
	/**
	 * The update rate of a <see cref="DaSubscription"/> object.
	 */
	DASUBSCRIPTION_UPDATERATE (OTClient.OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE),
	/**
	 * The deadband of a <see cref="DaSubscription"/> object.
	 */
	DASUBSCRIPTION_DEADBAND (OTClient.OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND),
	/**
	 * The timebias attribute  of a Data Access Subscription.
	 */
	DASUBSCRIPTION_TIMEBIAS (OTClient.OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS),
	/**
	 * The locale ID of a <see cref="DaSubscription"/> object.
	 */
	DASUBSCRIPTION_LCID (OTClient.OTC_ATTRIBUTE_DASUBSCRIPTION_LCID),
	/**
	 * The keep alive time of a <see cref="DaSubscription"/> object.
	 */
	DASUBSCRIPTION_KEEPALIVETIME (OTClient.OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME),
	/**
	 * All the attributes of a <see cref="DaItem"/> object.
	 */
	DAITEM_ALL (OTClient.OTC_ATTRIBUTE_DAITEM_ALL),
	/**
	 * The requested datatype of a <see cref="DaItem"/> object.
	 */
	DAITEM_REQUESTED_DATATYPE (OTClient.OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE),
	/**
	 * The deadband of a <see cref="DaItem"/> object.
	 */
	DAITEM_DEADBAND (OTClient.OTC_ATTRIBUTE_DAITEM_DEADBAND),
	/**
	 * The native datatype of a <see cref="DaItem"/> object.
	 */
	DAITEM_NATIVE_DATATYPE (OTClient.OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE),
	/**
	 * The access rights of a <see cref="DaItem"/> object.
	 */
	DAITEM_ACCESSRIGHTS (OTClient.OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS),
	/**
	 * The Engineering Unit Type of a <see cref="DaItem"/> object.
	 */
	DAITEM_EUTYPE (OTClient.OTC_ATTRIBUTE_DAITEM_EUTYPE),
	/**
	 * The Engineering Unit Information of a <see cref="DaItem"/> object.
	 */
	DAITEM_EUINFO (OTClient.OTC_ATTRIBUTE_DAITEM_EUINFO),
	/**
	 * The identifier of a <see cref="DaItem"/> object.
	 */
	DAITEM_ID (OTClient.OTC_ATTRIBUTE_DAITEM_ID),
	/**
	 * The path of a <see cref="DaItem"/> object.
	 */
	DAITEM_PATH (OTClient.OTC_ATTRIBUTE_DAITEM_PATH),
	/**
	 * All attributes of an <see cref="AeSession"/> object.
	 */
	AESESSION_ALL (OTClient.OTC_ATTRIBUTE_AESESSION_ALL),
	/**
	 * The client name of an <see cref="AeSession"/> object.
	 */
	AESESSION_CLIENTNAME (OTClient.OTC_ATTRIBUTE_AESESSION_CLIENTNAME),
	/**
	 * The locale ID of an <see cref="AeSession"/> object.
	 */
	AESESSION_LCID (OTClient.OTC_ATTRIBUTE_AESESSION_LCID),
	/**
	 * The url of an <see cref="AeSession"/> object.
	 */
	AESESSION_URL (OTClient.OTC_ATTRIBUTE_AESESSION_URL),
	/**
	 * The execution context in which the Alarms and Events Server is to be run.
	 */
	AESESSION_CLASSCONTEXT (OTClient.OTC_ATTRIBUTE_AESESSION_COM_CLSCTX),
	/**
	 * All the attributes of an <see cref="AeSubscription"/> object.
	 */
	AESUBSCRIPTION_ALL (OTClient.OTC_ATTRIBUTE_AESUBSCRIPTION_ALL),
	/**
	 * The buffertime of an <see cref="AeSubscription"/> object.
	 */
	AESUBSCRIPTION_BUFFERTIME (OTClient.OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME),
	/**
	 * The maxsize of an <see cref="AeSubscription"/> object.
	 */
	AESUBSCRIPTION_MAXSIZE (OTClient.OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE),
	/**
	 * The filters of an <see cref="AeSubscription"/> object.
	 */
	AESUBSCRIPTION_FILTER (OTClient.OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER),
	/**
	 * The returned attributtes of an <see cref="AeSubscription"/> object.
	 */
	AESUBSCRIPTION_RETURNED_ATTRIBUTES (OTClient.OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES);
	
	private long objectAttribute_;
	private static final Map<Long, EnumObjectAttribute> idToInstance;
    static{ 
        HashMap<Long, EnumObjectAttribute> temp = new HashMap<Long, EnumObjectAttribute>(); 
        for (EnumObjectAttribute value : values()) { 
            temp.put(value.getLong(), value); 
        } 
        idToInstance = Collections.unmodifiableMap(temp); 
      }
    
	private EnumObjectAttribute(long oAttr){
		this.objectAttribute_ = oAttr;
	}
	
	public long getLong() { return objectAttribute_; }
	
	//changed at i4t request 16.11.2009 (TOS)
	/*
	public static EnumObjectAttribute getEnum(long oa)
	{
		for(EnumObjectAttribute var : EnumObjectAttribute.values())
		{
			if(var.getLong() == oa)
			{
				return var;
			}
		}
		return null;
	}*/
	
    public static EnumObjectAttribute getEnum(long objAttr_){ 
        return idToInstance.get(objAttr_); 
    }
}