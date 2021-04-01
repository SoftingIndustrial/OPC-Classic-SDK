package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCGetDAPropertiesOptions extends Structure {
	/// get properties stored in the address space element
	public byte m_getLocalFromAddressSpaceElement;
	/// only used if !m_getLocalFromAddressSpaceElement
	public byte m_whatPropertyData;
	public byte dummy1;
	public byte dummy2;
	public com.sun.jna.NativeLong m_propertyCount;
	public com.sun.jna.ptr.NativeLongByReference m_pPropertyIDs;
	public com.sun.jna.ptr.PointerByReference m_pPropertyNames;
	public OTCGetDAPropertiesOptions() {
		super();
	}
	/**
	 * @param m_getLocalFromAddressSpaceElement get properties stored in the address space element<br>
	 * @param m_whatPropertyData only used if !m_getLocalFromAddressSpaceElement
	 */
	public OTCGetDAPropertiesOptions(byte m_getLocalFromAddressSpaceElement, byte m_whatPropertyData, com.sun.jna.NativeLong m_propertyCount, com.sun.jna.ptr.NativeLongByReference m_pPropertyIDs, com.sun.jna.ptr.PointerByReference m_pPropertyNames) {
		super();
		this.m_getLocalFromAddressSpaceElement = m_getLocalFromAddressSpaceElement;
		this.m_whatPropertyData = m_whatPropertyData;
		this.m_propertyCount = m_propertyCount;
		this.m_pPropertyIDs = m_pPropertyIDs;
		this.m_pPropertyNames = m_pPropertyNames;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCGetDAPropertiesOptions)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCGetDAPropertiesOptions)clone));
	}
	
	OTCGetDAPropertiesOptions setupClone (OTCGetDAPropertiesOptions clone)
	{
		clone.m_getLocalFromAddressSpaceElement = this.m_getLocalFromAddressSpaceElement;
		clone.m_whatPropertyData = this.m_whatPropertyData;
		clone.m_propertyCount = this.m_propertyCount;
		clone.m_pPropertyIDs = this.m_pPropertyIDs;
		clone.m_pPropertyNames = this.m_pPropertyNames;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCGetDAPropertiesOptions clone() { return setupClone(new OTCGetDAPropertiesOptions()); }
	public static class ByReference extends OTCGetDAPropertiesOptions implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCGetDAPropertiesOptions implements com.sun.jna.Structure.ByValue {}
}