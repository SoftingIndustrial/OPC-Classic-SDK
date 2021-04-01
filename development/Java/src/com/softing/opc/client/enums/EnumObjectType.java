package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;

public enum EnumObjectType {

	/**
	 * Data Access Session.
	 */
	DASESSION (OTClient.OTC_OBJECTTYPE_DASESSION),
	/**
	 * Data Access Subscription.
	 */
	DASUBSCRIPTION (OTClient.OTC_OBJECTTYPE_DASUBSCRIPTION),
	/**
	 * Data Access Item.
	 */
	DAITEM (OTClient.OTC_OBJECTTYPE_DAITEM),
	/**
	 * Alarms and Events Server.
	 */
	AESESSION (OTClient.OTC_OBJECTTYPE_AESESSION),
	/**
	 * Alarms and Events subscription.
	 */
	AESUBSCRIPTION (OTClient.OTC_OBJECTTYPE_AESUBSCRIPTION);
	
	private final long objectTypeId_;
	
	private EnumObjectType(long oTypeId){
		this.objectTypeId_ = oTypeId;
	}
	
	public long getLong() { return objectTypeId_; }
}