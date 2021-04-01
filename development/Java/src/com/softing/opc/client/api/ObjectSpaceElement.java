package com.softing.opc.client.api;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.softing.opc.client.enums.EnumExecutionType;
import com.softing.opc.client.enums.EnumObjectAttribute;
import com.softing.opc.client.enums.EnumObjectState;
import com.softing.opc.client.enums.EnumResultCode;
import com.softing.opc.client.enums.EnumTraceGroup;
import com.softing.opc.client.enums.EnumTraceLevel;
import com.softing.opc.client.enums.ResultCode;
import com.softing.opc.client.impl.jna.OTCExecutionOptions;
import com.softing.opc.client.impl.jna.OTClient;
import com.sun.jna.*;
import com.sun.jna.ptr.NativeLongByReference;

public class ObjectSpaceElement {
	final Logger logger = LoggerFactory.getLogger(ObjectSpaceElement.class);
	
	private NativeLong objectHandle_ = new NativeLong(0);
	private Object handleJanitor_ = new Object();
	private final int index_;
	
	//protected static int objectIndex_ = 0;
	protected static AtomicInteger objectIndex_ = new AtomicInteger(0);
	
	public final int MAX_AGE_DEVICE = 0;
	public final int MAX_AGE_CACHE = 0xFFFFFFFF;


	public ObjectSpaceElement(){
		//this is necessary for hashing
		//index_ = ObjectSpaceElement.objectIndex_++; //ASSIGNMENT NOT THREAD SAFE\
		index_ = objectIndex_.incrementAndGet();
	}

	protected NativeLong getHandle(){
		NativeLong objectHandle = new NativeLong(0);				
		synchronized(handleJanitor_){
			objectHandle = objectHandle_;
		}
		return objectHandle;
	}
	
	protected void setHandle(NativeLong value){
		synchronized(handleJanitor_){
			objectHandle_ = value;
		}
	}


	public boolean getValid(){
		if (getHandle().longValue() != 0){
			return true;
		}
		return false;
	}


	public EnumObjectState getCurrentState(){

		java.nio.ByteBuffer currentState = java.nio.ByteBuffer.allocateDirect(1);
		currentState.put(0, (byte)0);
		java.nio.ByteBuffer targetState = java.nio.ByteBuffer.allocateDirect(1);
		targetState.put(0, (byte)0);
		try{										
			NativeLong res = OTClient.INSTANCE.OTCGetState(getHandle(), currentState, targetState);
			
			if (ResultCode.FAILED(res.intValue())){
				logger.error("getCurrentState() - Result code: " + res.intValue());
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.CurrentState.Get", 
					"result code is " + res.intValue());
				*/
			}
		}
		catch(Exception exc) {
			logger.error("getCurrentState() - " + exc.toString(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"ObjectSpaceElement.CurrentState.Get", 
				exc.toString());
			*/
		}
		return EnumObjectState.getEnum(currentState.get());
	}

	public EnumObjectState getTargetState(){

		java.nio.ByteBuffer currentState = java.nio.ByteBuffer.allocateDirect(1);
		currentState.put(0, (byte)0);
		java.nio.ByteBuffer targetState = java.nio.ByteBuffer.allocateDirect(1);
		targetState.put(0, (byte)0);
		try{										
			NativeLong res = OTClient.INSTANCE.OTCGetState(getHandle(), currentState, targetState);
			
			if (ResultCode.FAILED(res.intValue())){
				logger.error("getTargetState() - Result code: " + res.intValue());
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.CurrentState.Get", 
					"result code is " + res.intValue());
				*/
			}
		}
		catch(Exception exc) {
			logger.error("getTargetState() - " + exc.toString(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"ObjectSpaceElement.CurrentState.Get", 
				exc.toString());
			*/
		}
		return EnumObjectState.getEnum(targetState.getInt());
	}
	
	public void setTargetState(byte value)
	{
		try{
			byte targetState = (byte)value;
			OTClient.INSTANCE.OTCChangeTargetState(getHandle(), targetState, (byte)1);
		}
		catch(Exception exc){
			logger.error("setTargetState() - " + exc.toString(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"ObjectSpaceElement.TargetState.Set", 
				exc.toString());
			*/
		}	
	}


	public int getHashCode(){
		//return this.hashCode();
		return this.index_;
	}


	public int performStateTransition(boolean deep, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			OTCExecutionOptions options = new OTCExecutionOptions();

			if(executionOptions != null){
				options.m_executionType = (byte)(executionOptions.getExecutionType().getInt());
				options.m_executionContext = new NativeLong(executionOptions.getExecutionContext());
			}
			else{
				options.m_executionType = (byte)EnumExecutionType.SYNCHRONOUS.getInt();
				options.m_executionContext = new NativeLong(0);
			}
			
			if (deep == false){
				// performs the state transition for the object itself
				res = OTClient.INSTANCE.OTCPerformStateTransition(getHandle(), (byte)0, options).intValue();
			}
			else{
				// performs the state transition for the whole subtree of the object
				res = OTClient.INSTANCE.OTCPerformStateTransition(getHandle(), (byte)1, options).intValue();
			}
		}catch(Exception exc){
			logger.error("PerformStateTransition() - " + exc.toString(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"ObjectSpaceElement.PerformStateTransition", 
				exc.toString());
			*/			
		}
		return res;
	}


	public int connect(boolean deep, boolean active, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			byte byteDeep = deep ? (byte)1: (byte)0;
			byte byteState = active ? (byte)EnumObjectState.ACTIVATED.getInt(): (byte)EnumObjectState.CONNECTED.getInt();
			
			OTClient.INSTANCE.OTCChangeTargetState(getHandle(), byteState, byteDeep);
			res = performStateTransition(deep, executionOptions);	
			
		}catch(Exception exc){
			logger.error("Connect() - " + exc.toString(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"ObjectSpaceElement.Connect", 
				exc.toString());
			*/			
		}
		return res;
	}


	public int disconnect(ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			OTCExecutionOptions options = new OTCExecutionOptions();

			if(executionOptions != null){
				options.m_executionType = (byte)executionOptions.getExecutionType().getInt();
				options.m_executionContext = new NativeLong(executionOptions.getExecutionContext());
			}
			else{
				options.m_executionType = (byte)EnumExecutionType.SYNCHRONOUS.getInt();
				options.m_executionContext = new NativeLong(0);
			}
			
			EnumObjectState m_targetState = EnumObjectState.DISCONNECTED;
			OTClient.INSTANCE.OTCChangeTargetState(getHandle(),(byte)m_targetState.getInt(), (byte)1);
			res = OTClient.INSTANCE.OTCPerformStateTransition(getHandle(), (byte)1, options).intValue();
		}
		catch(Exception exc){
			logger.error("Disconnect() - " + exc.toString(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"ObjectSpaceElement.Disconnect", 
				exc.toString());
			*/			
		}		
		return res;
	}

	public int getSetAttributesFromServer(boolean bGet, EnumObjectAttribute[] whatAttributes, ArrayList<Integer> results, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		
		if(results == null)
		{
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("GetSetAttributesFromServer() - Null reference passed passed to the Results parameter");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"ObjectSpaceElement.GetSetAttributesFromServer", 
				" Null reference passed passed to the Results parameter");
			*/	
			return res;
		}
		
		results.clear();
		try{
			OTCExecutionOptions options = new OTCExecutionOptions();

			if(executionOptions != null){
				options.m_executionType = (byte)executionOptions.getExecutionType().getInt();
				options.m_executionContext = new NativeLong(executionOptions.getExecutionContext());
			}
			else{
				options.m_executionType = (byte)EnumExecutionType.SYNCHRONOUS.getInt();
				options.m_executionContext = new NativeLong(0);
			}
			
			int count = whatAttributes.length;
			
			//allocate space for what atributes to change
			java.nio.ByteBuffer bbAttributes = java.nio.ByteBuffer.allocateDirect(count * NativeLong.SIZE);
			//java.nio.LongBuffer lbAttributes = bbAttributes.asLongBuffer();
			Pointer attributesPointer = Native.getDirectBufferPointer(bbAttributes);
			NativeLongByReference attributesLongByReference = new NativeLongByReference();
			for(int i = 0; i < count; i++)
			{
				Pointer p = attributesPointer.share(i*NativeLong.SIZE);
				NativeLongByReference attributei = new NativeLongByReference();
				attributei.setPointer(p);
				attributei.setValue(new NativeLong(whatAttributes[i].getLong()));
			}
			
			//allocate space for the results
			java.nio.ByteBuffer bbResults = java.nio.ByteBuffer.allocateDirect(count * NativeLong.SIZE);
			//java.nio.LongBuffer lbResults = bbResults.asLongBuffer();
			Pointer resultsPointer = Native.getDirectBufferPointer(bbResults);
			NativeLongByReference resultsLongByReference = new NativeLongByReference();
			resultsLongByReference.setPointer(resultsPointer);
			
			res =  OTClient.INSTANCE.OTCUpdateAttributes(
				getHandle(),
				bGet?(byte)1:(byte)0,
				new NativeLong(count),
				attributesLongByReference,
				resultsLongByReference,
				options).intValue();
			
			//results = new ArrayList<Integer>();
			for(int i = 0; i < count; i++)
			{
				Pointer p = resultsPointer.share(i*NativeLong.SIZE);
				NativeLongByReference resulti = new NativeLongByReference();
				resulti.setPointer(p);
				results.add( new Integer(resulti.getValue().intValue()));
			}
			bbAttributes.clear();
			bbResults.clear();
		}
		catch(Exception exc){
			//exc.printStackTrace();
			String function = bGet ? "ObjectSpaceElement.GetAttributesFromServer": "ObjectSpaceElement.SetAttributesFromServer";
			logger.error(function + "() - Null reference passed to the Results parameter", exc);
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				function, 
				exc.toString());
			*/				
		}
		return res;
	}
	
	//changed the parameter int[] results to ArrayList<Integer> in order to make it an OUT parameter
	public int getAttributesFromServer(EnumObjectAttribute[] whatAttributes, ArrayList<Integer> results, ExecutionOptions executionOptions){
		return getSetAttributesFromServer(true, whatAttributes, results, executionOptions);
	}


	public int setAttributesToServer(EnumObjectAttribute[] whatAttributes, ArrayList<Integer> results, ExecutionOptions executionOptions){
		return getSetAttributesFromServer(false, whatAttributes, results, executionOptions);
	}


	protected void onGetAttributesFromServer(int executionContext, EnumObjectAttribute[] whatAttributes, int[] results, int result){
	} 

	protected void onSetAttributesToServer(int executionContext, EnumObjectAttribute[] whatAttributes, int[] results, int result){
	}

	protected void onStateChange(EnumObjectState newState){
	}

	protected void onPerformStateTransition(int executionContext, int result){
	}

}
