package com.softing.opc.client.impl.jni;

import java.util.ArrayList;

import com.softing.opc.client.impl.jna.OTValueData;
import com.softing.opc.client.api.ValueQT;


public class JNIOTClient {
	static {
		System.loadLibrary("JOTB");
		//System.loadLibrary("JNIOTClient");
		//System.load("E:\\work\\Eclipse_JNI\\OPC_Tunneling_JNI\\dll\\JNIOTClient.dll");
	}
	
	public static final JNIOTClient INSTANCE = new JNIOTClient();
	
	/**
	 * Original signature : <code>LONG OTCWrite(OTObjectHandle, ULONG, OTObjectHandle*, OTChar**, OTChar**, OTValueData*, LONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:730</i>
	 */
	//public native long OTCWrite(long objectHandle, long count, long[] pItemHandles, String[] pItemIDs, String[] pItemPaths, ArrayList pValues, ArrayList pResults, JNIOTCExecutionOptions executionOptions);
	public native long OTCWrite(long objectHandle, long count, long[] pItemHandles, String[] pItemIDs, String[] pItemPaths, ArrayList<ValueQT> pValues, ArrayList<Integer> pResults, JNIOTCExecutionOptions executionOptions);
	/**
	 * Original signature : <code>LONG OTCRead(OTObjectHandle, ULONG, OTObjectHandle*, OTChar**, OTChar**, ULONG, OTValueData*, LONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:728</i>
	 */
	//public native long OTCRead(long objectHandle, long count, long[] pItemHandles, String[] pItemIDs, String[] pItemPaths, long maxAge, ArrayList pValues, ArrayList pResults, JNIOTCExecutionOptions pExecutionOptions);
	public native long OTCRead(long objectHandle, long count, long[] pItemHandles, String[] pItemIDs, String[] pItemPaths, long maxAge, ArrayList<ValueQT> pValues, ArrayList<Integer> pResults, JNIOTCExecutionOptions pExecutionOptions);
	
	/**
	 * clears the persistent structures stored as globals in the JNI dll.
	 * These structures are used for mapping function callbacks from OTBD.dll back to the Java application.
	 */
	public native long OTCTerminate();
	
	public native long OTCAdvise(JNIOTCallbackFunctions callbackFunctions);
	
	/*
	public interface OTCOnDataChange {
		void invoke(JNIOTCObjectContext objectContext, long count, JNIOTObjectData[] dataArray, ValueQT[] valuesArray , long[] results);
	}
	
	public interface OTCOnReadComplete{
		void invoke(long executionContext, JNIOTCObjectContext objectContext, long result, long count, JNIOTObjectData[] itemDataArray, String[] itemIDs, String[] itemPathsArray, ValueQT[] valuesArray, long[] resultsArray);
	}
	
	public interface OTCOnWriteComplete {
		void invoke(long executionContext, JNIOTCObjectContext objectContext, long result, long count, JNIOTObjectData[] itemDataArray, String[] itemIDs, String[] itemPathsArray, ValueQT[] valuesArray, long[] resultsArray);
	}
	*/
	
	private JNIOTClient(){};
}
