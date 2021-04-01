package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCAEReturnedAttributesData extends Structure {
	public com.sun.jna.NativeLong m_categoryID;
	public com.sun.jna.NativeLong m_countAttributeIDs;
	public com.sun.jna.ptr.NativeLongByReference m_attributeIDs;
	public OTCAEReturnedAttributesData() {
		super();
	}
	public OTCAEReturnedAttributesData(com.sun.jna.NativeLong m_categoryID, com.sun.jna.NativeLong m_countAttributeIDs, com.sun.jna.ptr.NativeLongByReference m_attributeIDs) {
		super();
		this.m_categoryID = m_categoryID;
		this.m_countAttributeIDs = m_countAttributeIDs;
		this.m_attributeIDs = m_attributeIDs;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCAEReturnedAttributesData)clone));

	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCAEReturnedAttributesData)clone));
	}
	
	OTCAEReturnedAttributesData setupClone (OTCAEReturnedAttributesData clone)
	{
		clone.m_categoryID = this.m_categoryID;
		clone.m_countAttributeIDs = this.m_countAttributeIDs;
		clone.m_attributeIDs = this.m_attributeIDs;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCAEReturnedAttributesData clone() { return setupClone(new OTCAEReturnedAttributesData()); }
	public static class ByReference extends OTCAEReturnedAttributesData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCAEReturnedAttributesData implements com.sun.jna.Structure.ByValue {}
}
