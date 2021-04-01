package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;


public enum EnumAddressSpaceElementType {

	/**
	 * A node in the hierarchical structure of a server's address space. It provides a
	 * logical grouping for the process variables.
	 */
	BRANCH (OTClient.OT_ADDRESSSPACETYPE_BRANCH),
	/**
	 * A process variable which can be accessed via an OPC server.
	 */
	LEAF (OTClient.OT_ADDRESSSPACETYPE_LEAF),
	/**
	 * A process variable which can be accessed via an OPC Data Access Server.
	 */
	DA_ITEM (OTClient.OT_ADDRESSSPACETYPE_ITEM),
	/**
	 * All the types enumerated above.
	 */
	ALL (OTClient.OT_ADDRESSSPACETYPE_ALL);
	
	public final int elementType;
	
	private EnumAddressSpaceElementType(int typeId) {
		this.elementType = typeId;
	}
	
	public int getInt() { return elementType; }
}