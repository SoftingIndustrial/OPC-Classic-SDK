package com.softing.opc.client.api;

import java.io.Serializable;

import com.softing.opc.client.impl.jna.OTClient;
import com.sun.jna.NativeLong;

public class AddressSpaceElement implements Serializable{

	protected NativeLong objectElementHandle_ = new NativeLong(0);
	protected String name_ = "";
	protected String qualifiedName_ = "";

	protected void finalize() throws Throwable {		
		dispose();
	}

	public AddressSpaceElement(){

	}

	protected AddressSpaceElement(String aName, String aQualifiedName, NativeLong aHandle){
		name_ = aName;
		qualifiedName_ = aQualifiedName;
		objectElementHandle_ = aHandle;
	}
	
	private static final long serialVersionUID = 7526471155622776147L;

	public void dispose(){
		if (objectElementHandle_.longValue() != 0)
		{
			OTClient.INSTANCE.OTCReleaseAddressSpaceElement(objectElementHandle_, 0);
			objectElementHandle_.setValue(0);
		}
	}

	public NativeLong getHandle(){
		return objectElementHandle_;
	}


	public String getName(){
		return "";
	}
	

	public void setName(String name){
		this.name_ = name;
	}

	public String getQualifiedName(){
		return qualifiedName_;
	}
	
	public void setQualifiedName(String qualifiedName){
		this.qualifiedName_ = qualifiedName;
	}

}