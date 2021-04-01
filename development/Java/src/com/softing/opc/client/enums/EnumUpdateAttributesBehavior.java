package com.softing.opc.client.enums;

public enum EnumUpdateAttributesBehavior {

	/**
	 * The attributes update is made automatically,without the user to request it
	 * explicitely.
	 */
	IMPLICIT_UPDATE (0x01),
	/**
	 * The attributes update is made at the user request.
	 */
	EXPLICIT_UPDATE (0x02);
	
	private final int behaviour_;
	
	private EnumUpdateAttributesBehavior(int updateAttributesBehaviour){
		this.behaviour_ = updateAttributesBehaviour;
	}
	
	public int getInt() { return behaviour_; }
}