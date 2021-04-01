package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCAddressSpaceElementData extends Structure {
	public com.sun.jna.NativeLong m_objectHandle;

	//public com.sun.jna.ptr.ByteByReference m_name;
	public com.sun.jna.Pointer m_name; //UNICODE
	/// OT_ADDRESSSPACETYPE
	public byte m_type;
	public byte dummy1;
	public byte dummy2;
	public byte dummy3;
	/// DA
	//public com.sun.jna.ptr.ByteByReference m_itemID;
	public com.sun.jna.Pointer m_itemID; //UNICODE
	
	//public com.sun.jna.ptr.ByteByReference m_itemPath;
	public com.sun.jna.Pointer m_itemPath; //UNICODE
	
	public OTCAddressSpaceElementData() {
		super();
	}
	
	
	public OTCAddressSpaceElementData(Pointer p)
	{
		super();
		this.useMemory(p);
		this.read();
	}
	
	/**
	 * @param m_type OT_ADDRESSSPACETYPE<br>
	 * @param m_itemID DA
	 */
	/*public OTCAddressSpaceElementData(com.sun.jna.NativeLong m_objectHandle, com.sun.jna.ptr.ByteByReference m_name, byte m_type, com.sun.jna.ptr.ByteByReference m_itemID, com.sun.jna.ptr.ByteByReference m_itemPath) {
		super();
		this.m_objectHandle = m_objectHandle;
		this.m_name = m_name;
		this.m_type = m_type;
		this.m_itemID = m_itemID;
		this.m_itemPath = m_itemPath;
	}*/
	
	public OTCAddressSpaceElementData(com.sun.jna.NativeLong m_objectHandle, Pointer m_name, byte m_type, Pointer m_itemID, Pointer m_itemPath) {
		super();
		this.m_objectHandle = m_objectHandle;
		this.m_name = m_name;
		this.m_type = m_type;
		this.m_itemID = m_itemID;
		this.m_itemPath = m_itemPath;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCAddressSpaceElementData)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCAddressSpaceElementData)clone));
	}
	
	OTCAddressSpaceElementData setupClone (OTCAddressSpaceElementData clone)
	{
		clone.m_objectHandle = this.m_objectHandle;
		clone.m_name = this.m_name;
		clone.m_type = this.m_type;
		clone.m_itemID = this.m_itemID;
		clone.m_itemPath = this.m_itemPath;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCAddressSpaceElementData clone() { return setupClone(new OTCAddressSpaceElementData()); }
	public static class ByReference extends OTCAddressSpaceElementData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCAddressSpaceElementData implements com.sun.jna.Structure.ByValue {}
}

