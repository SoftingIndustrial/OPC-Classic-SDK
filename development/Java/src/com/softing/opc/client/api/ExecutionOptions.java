package com.softing.opc.client.api;

import com.softing.opc.client.enums.EnumExecutionType;

public class ExecutionOptions {

	protected EnumExecutionType executionType_;
	protected int executionContext_;

	public ExecutionOptions(){
		executionContext_ = 0;
		executionType_ = EnumExecutionType.SYNCHRONOUS;
	}

	public ExecutionOptions(EnumExecutionType type, int executionContext){
		this.executionType_ = type;
		this.executionContext_ = executionContext;
	}

	public EnumExecutionType getExecutionType(){
		return executionType_;
	}
	
	public void setExecutionType(EnumExecutionType value){
		executionType_ = value;
	}

	public int getExecutionContext(){
		return executionContext_;
	}
	
	public void setExecutionContext(int value){
		executionContext_ = value;
	}
}
