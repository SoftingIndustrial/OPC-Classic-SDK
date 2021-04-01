package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class SAFEARRAY extends Structure {
	public short cDims;
	public short fFeatures;
	public com.sun.jna.NativeLong cbElements;
	public com.sun.jna.NativeLong cLocks;
	public com.sun.jna.Pointer pvData;
	public com.softing.opc.client.impl.jna.SAFEARRAYBOUND[] rgsabound = new com.softing.opc.client.impl.jna.SAFEARRAYBOUND[(1)];
	public SAFEARRAY() {
		super();
	}
	
	public SAFEARRAY(Pointer p)
	{
		super();
		this.useMemory(p);
		this.read();
	}
	
	public SAFEARRAY(short cDims, short fFeatures, com.sun.jna.NativeLong cbElements, com.sun.jna.NativeLong cLocks, com.sun.jna.Pointer pvData, com.softing.opc.client.impl.jna.SAFEARRAYBOUND rgsabound[]) {
		super();
		this.cDims = cDims;
		this.fFeatures = fFeatures;
		this.cbElements = cbElements;
		this.cLocks = cLocks;
		this.pvData = pvData;
		if (rgsabound.length != this.rgsabound.length) 
			throw new java.lang.IllegalArgumentException("Wrong array size !");
		this.rgsabound = rgsabound;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((SAFEARRAY)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((SAFEARRAY)clone));
	}
	
	SAFEARRAY setupClone (SAFEARRAY clone)
	{
		clone.cDims = this.cDims;
		clone.fFeatures = this.fFeatures;
		clone.cbElements = this.cbElements;
		clone.cLocks = this.cLocks;
		clone.pvData = this.pvData;
		clone.rgsabound = this.rgsabound;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public SAFEARRAY clone() { return setupClone(new SAFEARRAY()); }
	public static class ByReference extends SAFEARRAY implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends SAFEARRAY implements com.sun.jna.Structure.ByValue {}
}