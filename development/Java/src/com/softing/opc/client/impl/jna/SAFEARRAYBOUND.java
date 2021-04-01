package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class SAFEARRAYBOUND extends Structure {
	public com.sun.jna.NativeLong cElements;
	public com.sun.jna.NativeLong lLbound;
	public SAFEARRAYBOUND() {
		super();
	}
	
	public SAFEARRAYBOUND(Pointer p)
	{
		super();
		this.useMemory(p);
		this.read();
	}
	
	public SAFEARRAYBOUND(com.sun.jna.NativeLong cElements, com.sun.jna.NativeLong lLbound) {
		super();
		this.cElements = cElements;
		this.lLbound = lLbound;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((SAFEARRAYBOUND)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((SAFEARRAYBOUND)clone));
	}
	
	SAFEARRAYBOUND setupClone (SAFEARRAYBOUND clone)
	{
		clone.cElements = this.cElements;
		clone.lLbound = this.lLbound;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public SAFEARRAYBOUND clone() { return setupClone(new SAFEARRAYBOUND()); }
	public static class ByReference extends SAFEARRAYBOUND implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends SAFEARRAYBOUND implements com.sun.jna.Structure.ByValue {}
}
