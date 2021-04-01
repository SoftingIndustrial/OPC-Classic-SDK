package com.softing.opc.client.api;

public class DaProperty {

	private Object valueJanitor_ = new Object();
	
	protected  int id_ = 0;
	protected  String name_ = "";
	protected  String itemId_ = "";
	protected  String itemPath_ = "";
	protected  String description_ = "";
	protected  Class dataType_;
	protected  ValueQT value_;
	protected  int result_ = -1;



	public DaProperty(){
		id_ = 0;
		name_ = null;
		itemId_ = null;
		itemPath_ = null;
		description_= null;
		dataType_ = null;
		value_ = null;
		result_ = -1;
	}


	protected DaProperty(int anId, String aName, String anItemId, String anItemPath, String aDescription, Class aDataType, ValueQT aValue, int aResult){
		id_ = anId;
		name_ = aName;
		itemId_ = anItemId;
		itemPath_ = anItemPath;
		description_ = aDescription;
		dataType_ = aDataType;
		value_ = aValue;
		result_ = aResult;
	}


	public int getId(){
		return id_;
	}


	public String getName(){
		return name_;
	}


	public String getItemId(){
		return itemId_;
	}


	public String getItemPath(){
		return itemPath_;
	}


	public String getDescription(){
		return description_;
	}


	public Class getDataType(){
		return dataType_;
	}


	public ValueQT getValueQT(){
		ValueQT res = null;
		synchronized(valueJanitor_){
			res = this.value_;
		}
		return res;
	}


	public int getResult(){
		return result_;
	}

}