package com.softing.opc.client.impl.jna;


import com.sun.jna.*;

public class OTCAddressSpaceBrowseOptions extends Structure {
	/**
	 * filter<br>
	 * OT_ADDRESSSPACETYPE
	 */
	public byte m_type;
	public byte dummy1;
	public byte dummy2;
	public byte dummy3;
	//public com.sun.jna.ptr.ByteByReference m_elementNameFilter;
	public com.sun.jna.Pointer m_elementNameFilter; //UNICODE
	/// DA V1 and V2
	public com.sun.jna.NativeLong m_accessRightsFilter;
	/// DA V1 and V2
	public short m_datatypeFilter;
	public byte dummy4;
	public byte dummy5;
	/// DA V3 and XML-DA
	//public com.sun.jna.ptr.ByteByReference m_vendorFilter;
	public com.sun.jna.Pointer m_vendorFilter; //UNICODE
	/**
	 * retrieve options<br>
	 * DA V3 and XML-DA
	 */
	public com.sun.jna.NativeLong m_maxElements;
	/// retrieve what data
	public byte m_retrieveItemID;
	/// DA V3 and XML-DA
	public byte m_retrieveProperties;
	/// DA V3 and XML-DA
	public byte m_retrievePropertyValues;
	/// DA V2
	public byte m_forceBrowseUp;
	public OTCAddressSpaceBrowseOptions() {
		super();
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCAddressSpaceBrowseOptions)clone));
		
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCAddressSpaceBrowseOptions)clone));
	}
	
	OTCAddressSpaceBrowseOptions setupClone (OTCAddressSpaceBrowseOptions clone)
	{
		clone.m_type = this.m_type;
		clone.m_elementNameFilter = this.m_elementNameFilter;
		clone.m_accessRightsFilter = this.m_accessRightsFilter;
		clone.m_datatypeFilter = this.m_datatypeFilter;
		clone.m_vendorFilter = this.m_vendorFilter;
		clone.m_maxElements = this.m_maxElements;
		clone.m_retrieveItemID = this.m_retrieveItemID;
		clone.m_retrieveProperties = this.m_retrieveProperties;
		clone.m_retrievePropertyValues = this.m_retrievePropertyValues;
		clone.m_forceBrowseUp = this.m_forceBrowseUp;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCAddressSpaceBrowseOptions clone() { return setupClone(new OTCAddressSpaceBrowseOptions()); }
	public static class ByReference extends OTCAddressSpaceBrowseOptions implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCAddressSpaceBrowseOptions implements com.sun.jna.Structure.ByValue {}
}

