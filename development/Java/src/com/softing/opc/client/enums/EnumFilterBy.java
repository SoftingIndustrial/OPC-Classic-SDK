package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;

public enum EnumFilterBy {

	/**
	 * The server supports filtering based on the values for the event type(condition
	 * related,simple,tracking related);
	 */
	EVENT (OTClient.OT_FILTER_BY_EVENT),
	/**
	 * The server supports filtering based on the values for the event category;
	 */
	CATEGORY (OTClient.OT_FILTER_BY_CATEGORY),
	/**
	 * The server supports filtering based on the values for the severity;
	 */
	SEVERITY (OTClient.OT_FILTER_BY_SEVERITY),
	/**
	 * The server supports filtering based on the values for the area(nodes in the
	 * eventarea);
	 */
	AREA (OTClient.OT_FILTER_BY_AREA),
	/**
	 * The server supports filtering based on the values for the sources(leaves in the
	 * eventarea);
	 */
	SOURCE (OTClient.OT_FILTER_BY_SOURCE);
	
	private int filterId;
	
	private EnumFilterBy(int filterBy){
		this.filterId = filterBy;
	}
	
	public int getInt() { return filterId; }
}