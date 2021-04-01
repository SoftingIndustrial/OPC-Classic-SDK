package com.softing.opc.client.api;

import java.nio.ByteBuffer;
import java.util.Hashtable;
import java.util.Date;

import com.softing.opc.client.api.DaSession;
import com.softing.opc.client.enums.EnumFeature;
import com.softing.opc.client.enums.EnumObjectAttribute;
import com.softing.opc.client.enums.EnumObjectState;
import com.softing.opc.client.enums.EnumObjectType;
import com.softing.opc.client.enums.EnumResultCode;
import com.softing.opc.client.enums.EnumTraceGroup;
import com.softing.opc.client.enums.EnumTraceLevel;
import com.softing.opc.client.enums.EnumUpdateAttributesBehavior;
import com.softing.opc.client.enums.ResultCode;
import com.softing.opc.client.impl.NameDecoder;
import com.softing.opc.client.impl.Trace;
import com.softing.opc.client.impl.jna.OTCAddressSpaceBrowseOptions;
import com.softing.opc.client.impl.jna.OTCCallbackFunctions;
import com.softing.opc.client.impl.jna.OTCDAPropertyData;
import com.softing.opc.client.impl.jna.OTCGetDAPropertiesOptions;
import com.softing.opc.client.impl.jna.OTCInitData;
import com.softing.opc.client.impl.jna.OTCServerStatus;
import com.softing.opc.client.impl.jna.OTClient;
import com.softing.opc.client.impl.jna.OTObjectData;
import com.softing.opc.client.impl.jna.VARIANT;
import com.softing.opc.client.impl.jna.OTCObjectContext.ByValue;
import com.softing.opc.client.impl.jna.OTObjectData.ByReference;
import com.softing.opc.client.impl.jni.JNIOTCObjectContext;
import com.softing.opc.client.impl.jni.JNIOTCallbackFunctions;
import com.softing.opc.client.impl.jni.JNIOTClient;
import com.softing.opc.client.impl.jni.JNIOTObjectData;
import com.softing.opc.client.impl.jni.JNIOTValueData;

import com.sun.jna.*;
import com.sun.jna.ptr.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public final class Application {

	final Logger logger = LoggerFactory.getLogger(Application.class);
	private final Hashtable daSessionList_ = new Hashtable();
	//private final Hashtable aeSessionList_ = new Hashtable();
	private String badQualityValueString_ = "???";
	private EnumUpdateAttributesBehavior updateAttributesBehavior_ = EnumUpdateAttributesBehavior.IMPLICIT_UPDATE;
	//protected final Hashtable<Integer, ServerBrowser> serverBrowserList_ = new Hashtable<Integer, ServerBrowser>();
	private static boolean isInit_ = false;
	private static boolean isAdvised_ = false;
	private static OTCCallbackFunctions otnCCallBacks_ = new OTCCallbackFunctions();
	private static JNIOTCallbackFunctions jniOTNCCallBacks_ = new JNIOTCallbackFunctions();
	private final OTCInitData initData_ = new OTCInitData();
	private ByteBuffer bbInitDataServiceName = null;
	public static Application INSTANCE = new Application();

	private Application(){
		initData_.m_versionOTC	= 422;
	}

	public void releaseApplication(){
		//neimplmented in .NET version
	}

	public short getVersionOtb(){
		return initData_.m_versionOTC;
	}
	
	public void setVersionOtb(short value){
		initData_.m_versionOTC = value;
	}


	public String getServiceName(){
		return initData_.m_serviceName.getString(0, true);
	}
	

	public void setServiceName(String value){
		try
		{
			byte[] bytes = value.getBytes("UTF-16LE");
			bbInitDataServiceName = ByteBuffer.allocateDirect(bytes.length + 1);
			bbInitDataServiceName.put(bytes);
			bbInitDataServiceName.put(bytes.length, (byte)0);
			Pointer p = Native.getDirectBufferPointer(bbInitDataServiceName);
			//ByteByReference bbr = new ByteByReference();
			//bbr.setPointer(p);
			initData_.m_serviceName = p;
		}
		catch(Exception exc)
		{
			logger.error("setServiceName() - " + exc.getMessage(), exc);
		}
	}

	public DaSession[] getDaSessionList(){
		DaSession[] sessionVector = new DaSession[daSessionList_.size()];
		int i = 0;
		for(Object daSession : daSessionList_.values())
		{
			sessionVector[i] = (DaSession)daSession;
			i++;
		}
		return sessionVector;
	}


	/*public AeSession[] getAeSessionList(){
		return null;
	}*/


	public String getBadQualityValueString(){
		return badQualityValueString_;
	}
	

	public void setBadQualityValueString(String value){
		badQualityValueString_ = value;
	}


	public EnumUpdateAttributesBehavior getUpdateAttributesBehavior(){
		return updateAttributesBehavior_;
	}
	
	public void setUpdateAttributesBehaviour(EnumUpdateAttributesBehavior value){
		updateAttributesBehavior_ = value;
	}


	protected int advise(){
		int res = EnumResultCode.E_FAIL.toInt();

		try{
			otnCCallBacks_.m_OTOnTrace					 	 = handleTrace;
			otnCCallBacks_.m_OTCOnStateChange				 = handleStateChange;
			//OTNCCallBacks_.m_OTCOnDataChange				 = handleDataChange;
			//OTNCCallBacks_.m_OTCOnReadComplete			 = handleReadComplete; //the handlers are linked via JNI calls
			//OTNCCallBacks_.m_OTCOnWriteComplete			 = handleWriteComplete;
			otnCCallBacks_.m_OTCOnPerfromStateTransition	 = handlePerformStateTransition;
			otnCCallBacks_.m_OTCOnServerShutdown			 = handleServerShutdown;
			otnCCallBacks_.m_OTCOnUpdateAttributes		 	 = handleUpdateAttributes;
			otnCCallBacks_.m_OTCOnUpdateDAItemAttributes	 = handleUpdateDaItemAttributes;
			otnCCallBacks_.m_OTCOnValidateDAItems			 = handleValidateDaItems;
			otnCCallBacks_.m_OTCOnGetServerStatus			 = handleGetServerStatus;
			otnCCallBacks_.m_OTCOnBrowseAddressSpace		 = handleBrowseAddressSpace;
			otnCCallBacks_.m_OTCOnGetDAProperties			 = handleGetDaProperties;
	
			//OTNCCallBacks_.m_OTCOnReceivedEvents			 = handleEventsReceived;
			//OTNCCallBacks_.m_OTCOnRefreshAEConditions		 = handleRefreshConditions;
			//OTNCCallBacks_.m_OTCOnAcknowledgeAEConditions	 = handleAcknowledgeConditions;	
			//OTNCCallBacks_.m_OTCOnQueryAvailableAEFilters	 = handleQueryAvailableAeFilters;
			//OTNCCallBacks_.m_OTCOnQueryAECategories		 = handleQueryAeCategories;
			//OTNCCallBacks_.m_OTCOnQueryAEAttributes		 = handleQueryAeAttributes;
			//OTNCCallBacks_.m_OTCOnQueryAEConditions		 = handleQueryAeConditions;				
			//OTNCCallBacks_.m_OTCOnQueryAESubConditions	 = handleQueryAeSubConditions;
			//OTNCCallBacks_.m_OTCOnQueryAESourceConditions	 = handleQuerySourceConditions;
			//OTNCCallBacks_.m_OTCOnGetAEConditionState		 = handleGetConditionState;
			//OTNCCallBacks_.m_OTCOnEnableAEConditions		 = handleEnableAeConditions;
			otnCCallBacks_.m_OTCOnGetErrorString			 = handleGetErrorString;
			otnCCallBacks_.m_OTCOnLogon						 = handleLogon;
			otnCCallBacks_.m_OTCOnLogoff					 = handleLogoff;
			  
			if (!isAdvised_)
			{
				//set the callbacks via JNA (everything except HandleReadComplete, HandleWriteComplete, HandlePerformStateTransition 
				res =	OTClient.INSTANCE.OTCAdvise(otnCCallBacks_).intValue();	
				jniOTNCCallBacks_.onDataChangeCallbackClass = "Lcom/softing/opc/client/api/Application;";
				jniOTNCCallBacks_.onDataChangeCallbackMethod = "jniHandleDataChange";
				jniOTNCCallBacks_.onReadCompleteCallbackClass = "Lcom/softing/opc/client/api/Application;";
				jniOTNCCallBacks_.onReadCompleteCallbackMethod = "jniHandleReadComplete";
				jniOTNCCallBacks_.onWriteCompleteCallbackClass = "Lcom/softing/opc/client/api/Application;";
				jniOTNCCallBacks_.onWriteCompleteCallbackMethod = "jniHandleWriteComplete";
				int resJNI = (int)JNIOTClient.INSTANCE.OTCAdvise(jniOTNCCallBacks_);
				if(ResultCode.SUCCEEDED(res) 
						&& ResultCode.SUCCEEDED(resJNI))
				{
					isAdvised_ = true;
				}
				else
				{
					logger.error("Advise() - Application advise failed. Result: " + res);
					//Application.Instance.Trace(EnumTraceLevel.ERR,
					//	EnumTraceGroup.CLIENT,
					//	"Application.Advise",
					//	" Application advise failed! Result: " + res);
				}
			}
		}
		catch(Exception exc)
		{
			//exc.printStackTrace();
			logger.error("Advise() - " + exc.getMessage(), exc);
			/*Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Application.Advise",
				exc.getMessage());
			*/
		}
	return res;
	}


	protected int addDaSession(String url, DaSession aDaSession){
		int res = EnumResultCode.E_FAIL.toInt();
		try
		{
			if(aDaSession != null)
			{
				com.sun.jna.ptr.NativeLongByReference sessionHandleReference =  new NativeLongByReference();
				
				res = OTClient.INSTANCE.OTCAddDASession(
					new WString(url),
					new NativeLong(aDaSession.getHashCode()), 
					sessionHandleReference).intValue();	

				if(ResultCode.SUCCEEDED(res))
				{
					aDaSession.setHandle(sessionHandleReference.getValue());						
					daSessionList_.put(aDaSession.getHashCode(),aDaSession);							
				}
				else
				{
					logger.error("AddDaSession() - Add session failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Application.AddDaSession", 
						" Add session failed! Result: " + res);
					*/
				}
			}	
			else
			{
				res = EnumResultCode.E_INVALIDARG.toInt();
				logger.error("AddDaSession() - The session argument is invalid. It cannot be null.");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application.AddDaSession", 
					" The session argument is invalid! It cannot be null!");
				*/
			}	//	end if...else
		}
		catch(Exception exc)
		{
			logger.error("AddDaSession() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Application.AddDaSession",
				exc.getMessage());
			*/
		}
		return res;
	}


	OTClient.OTOnTrace handleTrace = new OTClient.OTOnTrace() {
		
		@Override
		public void invoke(ByteByReference traceString, short level,
				NativeLong mask, ByteByReference objId, ByteByReference text) {
			try
			{
				//System.out.println( traceString.getPointer().getString(0));
				String strMsg = "OTBD.dll trace - " + traceString.getPointer().getString(0, true); 
				if(level == (short)EnumTraceLevel.DEB.toByte())
				{
					logger.debug(strMsg);
				}
				else if(level == (short)EnumTraceLevel.ERR.toByte())
				{
					logger.error(strMsg);
				}
				else if(level == (short)EnumTraceLevel.WRN.toByte())
				{
					logger.warn(strMsg);
				}
				else if(level == (short)EnumTraceLevel.INF.toByte())
				{
					logger.info(strMsg);
				}
			}
			catch(Exception exc)
			{
				//exc.printStackTrace();
				//Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "Application. HandleTrace",  exc.getMessage());
			}
			
		}
	};


	OTClient.OTCOnStateChange handleStateChange = new OTClient.OTCOnStateChange() {
		
		@Override
		public void invoke(ByValue objectContext, byte state) {
			try
			{
				EnumObjectState currentState = EnumObjectState.DISCONNECTED;
				switch (state)
				{
					case 0: currentState = EnumObjectState.DISCONNECTED;
						break;
					case 1: currentState = EnumObjectState.CONNECTED;
						break;
					case 3: currentState = EnumObjectState.ACTIVATED;
						break;
					default: currentState = EnumObjectState.DISCONNECTED;
						break;
				}		

				if(objectContext.m_objectType.longValue() == EnumObjectType.DASESSION.getLong() )
				{
					DaSession currentSession = (DaSession)daSessionList_.get(objectContext.m_objectData.m_userData.intValue());
					if(currentSession != null) {
						currentSession.onStateChange(currentState);
					}
				}
				if(objectContext.m_objectType.longValue() == EnumObjectType.DASUBSCRIPTION.getLong())
				{
					DaSession currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
					if(currentSession != null)
					{
						
						DaSubscription currentSubscription = currentSession.find(objectContext.m_objectData.m_userData.intValue());
						if(currentSubscription != null) {
							currentSubscription.onStateChange(currentState);
						}
					}
				}
				if(objectContext.m_objectType.longValue() == EnumObjectType.DAITEM.getLong() )
				{
					DaSession currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
					if(currentSession != null)
					{
						
						DaSubscription currentSubscription = currentSession.find(objectContext.m_subscriptionData.m_userData.intValue());
						if(currentSubscription != null)
						{
							
							DaItem currentItem = currentSubscription.find(objectContext.m_objectData.m_userData.intValue());
							if (currentItem != null) {
								currentItem.onStateChange(currentState);
							}
						}
					}
				}	
			}
			catch(Exception exc)
			{
				logger.error("HandleStateChange() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleStateChange", 
					exc.getMessage());
				*/
			}
		}
	};
		
	public static void jniHandleDataChange(JNIOTCObjectContext objectContext, long count,
			JNIOTObjectData[] dataArray, ValueQT[] valuesArray,
			long[] results) {
		
		try
		{
			synchronized(Application.INSTANCE)
			{
				DaSession currentSession = null;
				DaSubscription currentSubscription = null;
				DaItem[] items = new DaItem[(int)count];	
				int[] intResults = new int[(int)count];
				if(objectContext.m_objectType == EnumObjectType.DASUBSCRIPTION.getLong() ){
					currentSession = (DaSession)Application.INSTANCE.daSessionList_.get((int)objectContext.m_sessionData.m_userData);
					if( currentSession != null){
						currentSubscription = currentSession.find((int)objectContext.m_objectData.m_userData);
						if(currentSubscription != null){
							for(int i = 0;i < count; i++){
								items[i] = currentSubscription.find((int)dataArray[i].m_userData);								
								intResults[i] = (int)results[i];
							} //end for																					
							
							currentSubscription.onDataChange(
								items,
								valuesArray,
								intResults);		
						}
					}
				}	
			}
		}
		catch(Exception exc){
			exc.printStackTrace();
			Application.INSTANCE.logger.error("HandleDataChange() - " + exc.getMessage(), exc);	
		}
	}
	
	OTClient.OTCOnDataChange handleDataChange = new OTClient.OTCOnDataChange() {
		
		@Override
		public void invoke(com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, NativeLong count,
				com.softing.opc.client.impl.jna.OTObjectData.ByReference pItemData, com.softing.opc.client.impl.jna.OTValueData.ByReference pValues,
				NativeLongByReference pResults) {
			try{

				DaSession currentSession = null;
				DaSubscription currentSubscription = null;
				DaItem[] items = new DaItem[count.intValue()];	
				ValueQT[]newValues = new ValueQT[count.intValue()];	
				int[] results = new int[count.intValue()]; 
					
				com.softing.opc.client.impl.jna.OTObjectData[] itemData = (com.softing.opc.client.impl.jna.OTObjectData[])pItemData.toArray(count.intValue());
				com.softing.opc.client.impl.jna.OTValueData[] values = (com.softing.opc.client.impl.jna.OTValueData[])pValues.toArray(count.intValue());
				
				if(objectContext.m_objectType.longValue() == EnumObjectType.DASUBSCRIPTION.getLong() ){
					currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
					if( currentSession != null){
						
						currentSubscription = currentSession.find(objectContext.m_objectData.m_userData.intValue());
						if(currentSubscription != null){
						
							for(int i = 0;i < count.intValue(); i++){
								items[i] = currentSubscription.find(itemData[i].m_userData.intValue());								
								newValues[i] = new ValueQT(values[i]);
								results[i] = pResults.getPointer().getNativeLong(i* Native.LONG_SIZE).intValue();
							} //end for																					
							
							currentSubscription.onDataChange(
								items,
								newValues,
								results);		
						}//end if
					} //end if
				} //end if															
			}
			catch(Exception exc){
				logger.error("HandleDataChange() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleDataChange", 
					exc.getMessage());
				*/	
			}
		}
	};


	OTClient.OTCOnUpdateAttributes handleUpdateAttributes = new OTClient.OTCOnUpdateAttributes() {
		
		@Override
		public void invoke(NativeLong executionContext, ByValue objectContext,
				NativeLong result, byte fromServer, NativeLong attributesCount,
				NativeLongByReference pWhatAttributes,
				NativeLongByReference pResults) {
			try{
				
				EnumObjectAttribute[] attributes = new EnumObjectAttribute[attributesCount.intValue()];
				int[] results = new int[attributesCount.intValue()];
				for(int ii = 0; ii < attributesCount.intValue(); ii++){
					int intAttributeValue = pWhatAttributes.getPointer().getNativeLong(ii* Native.LONG_SIZE).intValue();
					attributes[ii] = EnumObjectAttribute.getEnum(intAttributeValue);		
					results[ii] = pResults.getPointer().getNativeLong(ii* Native.LONG_SIZE).intValue();
				}// for
		
				if(objectContext.m_objectType.longValue() == EnumObjectType.DASESSION.getLong() ){
					DaSession currentSession = (DaSession)daSessionList_.get(objectContext.m_objectData.m_userData.intValue());
					if(currentSession != null){
						if(fromServer == 1){
							currentSession.onGetAttributesFromServer(
								executionContext.intValue(),
								attributes,
								results,
								result.intValue());
						}
						if(fromServer == 0){
							currentSession.onSetAttributesToServer(
								executionContext.intValue(),
								attributes,
								results,
								result.intValue());
						}
					} //end if
				}
				if(objectContext.m_objectType.longValue() == EnumObjectType.DASUBSCRIPTION.getLong())
				{
					DaSession currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
					if(currentSession != null){
						
						DaSubscription currentSubscription = currentSession.find(objectContext.m_objectData.m_userData.intValue());
						if(currentSubscription != null){
							if(fromServer == 1){
								currentSubscription.onGetAttributesFromServer(
									executionContext.intValue(),
									attributes,
									results,
									result.intValue());
							}
							if(fromServer == 0){
								currentSubscription.onSetAttributesToServer(
									executionContext.intValue(),
									attributes,
									results,
									result.intValue());
							}
						} //end if
					} //end if
				}				
			}catch(Exception exc){
				logger.error("HandleUpdateAttributes() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleUpdateAttributes", 
					exc.getMessage());
				*/
			}
			
		}
	};

	OTClient.OTCOnValidateDAItems handleValidateDaItems = new OTClient.OTCOnValidateDAItems() {
		
		@Override
		public void invoke(NativeLong executionContext, ByValue objectContext,
				NativeLong result, NativeLong itemCount, com.softing.opc.client.impl.jna.OTObjectData.ByReference pObjectData,
				NativeLongByReference pResults) {
			try{	
				com.softing.opc.client.impl.jna.OTObjectData[] objectDataArray = (com.softing.opc.client.impl.jna.OTObjectData[])pObjectData.toArray(itemCount.intValue());
				DaSession currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
				int[] results = new int[itemCount.intValue()];
				
				if (currentSession != null){
					
					DaSubscription currentSubscription = currentSession.find(objectContext.m_objectData.m_userData.intValue());
					if(currentSubscription != null){
						DaItem[] items = new DaItem[itemCount.intValue()];
					
						for(int i = 0; i < itemCount.intValue(); i++){
							items[i] = currentSubscription.find(objectDataArray[i].m_userData.intValue());
							results[i] = pResults.getPointer().getNativeLong(i* Native.LONG_SIZE).intValue();
						}
						currentSubscription.onValidateDaItems(
							executionContext.intValue(),
							items,
							results,
							result.intValue());
					}
				}
				
			}catch(Exception exc){
				logger.error("HandleValidateDaItems() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleValidateDaItems",
					exc.getMessage());
				*/
			}
			
		}
	};

	public static void jniHandleReadComplete(long executionContext,
			JNIOTCObjectContext objectContext, long result, long count,
			JNIOTObjectData[] itemDataArray, String[] itemIDs,
			String[] itemPathsArray, ValueQT[] valuesArray,
			long[] resultsArray) {

		try
		{
			synchronized(Application.INSTANCE)
			{
				DaSession currentSession;
				DaSubscription currentSubscription;
				
				int[] results  = new int[(int)count];
				DaItem[] items = new DaItem[(int)count];
				for(int i = 0; i < count; i++)
				{
					results[i] = (int)resultsArray[i];
				}
				
				if(objectContext.m_objectType == EnumObjectType.DASESSION.getLong() ){
					currentSession = (DaSession)Application.INSTANCE.daSessionList_.get((int)objectContext.m_objectData.m_userData);
					if(currentSession != null){
						currentSession.onReadComplete(
							(int)executionContext,
							itemIDs,
							itemPathsArray,
							valuesArray,
							results,
							(int)result);
					}
				}
				if(objectContext.m_objectType == EnumObjectType.DASUBSCRIPTION.getLong()){
					currentSession = (DaSession)Application.INSTANCE.daSessionList_.get((int)objectContext.m_sessionData.m_userData);
					if(currentSession != null){
						
						currentSubscription = currentSession.find((int)objectContext.m_objectData.m_userData);
						if(currentSubscription != null){
						
							for(int i = 0; i < count; i++){
								items[i] = currentSubscription.find((int)itemDataArray[i].m_userData);									
							} //end for
						} //end if
						currentSubscription.onReadComplete(
							(int)executionContext,
							items,
							valuesArray,
							results,
							(int)result);
					} //end if
				}
			}		
		}
		catch(Exception exc){
			Application.INSTANCE.logger.error("HandleReadComplete() - " + exc.getMessage(), exc);
		}

	}

	
	OTClient.OTCOnReadComplete handleReadComplete = new OTClient.OTCOnReadComplete() {
		@Override
		public void invoke(NativeLong executionContext, ByValue objectContext,
				NativeLong result, NativeLong count, ByReference pItemData,
				PointerByReference pItemIDs, PointerByReference pItemPaths,
				com.softing.opc.client.impl.jna.OTValueData.ByReference pValues,
				NativeLongByReference pResults) {
			
			try{				
				DaSession currentSession;
				DaSubscription currentSubscription;
				DaItem[] items = new DaItem[count.intValue()];
				ValueQT[] newValues = new ValueQT[count.intValue()]; 
				int[] results = new int[count.intValue()];
				
				String[] itemIdArray = null;
				String[] itemPathsArray = null;
				
				if(pItemIDs != null)
				{
					itemIdArray = new String[count.intValue()];
				}
				if(pItemPaths != null)
				{
					itemPathsArray = new String[count.intValue()];
				}
				
				com.softing.opc.client.impl.jna.OTObjectData[] itemDataArray = null;
				if(pItemData != null)
				{
					itemDataArray = (com.softing.opc.client.impl.jna.OTObjectData[])pItemData.toArray(count.intValue());
				}

				com.softing.opc.client.impl.jna.OTValueData[] valuesArray = (com.softing.opc.client.impl.jna.OTValueData[])pValues.toArray(count.intValue());
				
				//creates the values array
				for (int i = 0; i< count.intValue(); i++){
					newValues[i] = new ValueQT(valuesArray[i]);
					results[i] = pResults.getPointer().getNativeLong(i* Native.LONG_SIZE).intValue();
					if(pItemIDs != null)
					{
						itemIdArray[i] = pItemIDs.getValue().getString(i * Native.POINTER_SIZE, true);
					}
					if(pItemPaths != null)
					{
						itemPathsArray[i] = pItemPaths.getValue().getString(i * Native.POINTER_SIZE, true);
					}
				}
				
				if(objectContext.m_objectType.longValue() == EnumObjectType.DASESSION.getLong() ){
					currentSession = (DaSession)daSessionList_.get(objectContext.m_objectData.m_userData.intValue());
					if(currentSession != null){
						currentSession.onReadComplete(
							executionContext.intValue(),
							itemIdArray,
							itemPathsArray,
							newValues,
							results,
							result.intValue());
					}
				}
				if(objectContext.m_objectType.longValue() == EnumObjectType.DASUBSCRIPTION.getLong()){
					currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
					if(currentSession != null){
						
						currentSubscription = currentSession.find(objectContext.m_objectData.m_userData.intValue());
						if(currentSubscription != null){
						
							for(int i = 0; i < count.intValue(); i++){
								items[i] = currentSubscription.find(itemDataArray[i].m_userData.intValue());									
							} //end for
						} //end if
						currentSubscription.onReadComplete(
							executionContext.intValue(),
							items,
							newValues,
							results,
							result.intValue());
					} //end if
				}	
			}catch(Exception exc){
				logger.error("HandleReadComplete() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleReadComplete", 
					exc.getMessage());
				*/
			}
		}
	};
		
	public static void jniHandleWriteComplete(long executionContext,
			JNIOTCObjectContext objectContext, long result, long count,
			JNIOTObjectData[] itemDataArray, String[] itemIDs,
			String[] itemPathsArray, ValueQT[] valuesArray,
			long[] resultsArray) {

		try
		{		
			synchronized(Application.INSTANCE)
			{
				DaSession currentSession;
				DaSubscription currentSubscription;
				DaItem[] items = new DaItem[(int)count];
				int[] results = new int[(int)count];
				for(int i = 0; i < count; i++)
				{
					results[i] = (int)resultsArray[i];
				}
				
				if(objectContext.m_objectType == EnumObjectType.DASESSION.getLong() ){
					currentSession = (DaSession)Application.INSTANCE.daSessionList_.get((int)objectContext.m_objectData.m_userData);
					if(currentSession != null){
						currentSession.onWriteComplete(
							(int)executionContext,
							itemIDs,
							itemPathsArray,
							valuesArray,
							results,
							(int)result);
					} //end if
				}
				if(objectContext.m_objectType == EnumObjectType.DASUBSCRIPTION.getLong()){
					currentSession = (DaSession)Application.INSTANCE.daSessionList_.get((int)objectContext.m_sessionData.m_userData);
					if(currentSession != null)
					{							
						currentSubscription = currentSession.find((int)objectContext.m_objectData.m_userData);
						if(currentSubscription != null){
							
							for(int i = 0; i < count; i++){
								items[i] = currentSubscription.find((int)itemDataArray[i].m_userData);									
							}
							currentSubscription.onWriteComplete(
								(int)executionContext,
								items,
								valuesArray,
								results,
								(int)result);
						}
					} //end if
				}
			}
		}
		catch(Exception exc){
			Application.INSTANCE.logger.error("HandleWriteComplete() - " + exc.getMessage(), exc);
		}
		
	}
	
	OTClient.OTCOnWriteComplete handleWriteComplete = new OTClient.OTCOnWriteComplete() {
		
		@Override
		public void invoke(NativeLong executionContext, ByValue objectContext,
				NativeLong result, NativeLong count, ByReference pItemData,
				PointerByReference pItemIDs, PointerByReference pItemPaths,
				com.softing.opc.client.impl.jna.OTValueData.ByReference pValues,
				NativeLongByReference pResults) {
			
			try{				
				DaSession currentSession;
				DaSubscription currentSubscription;
				DaItem[] items = new DaItem[count.intValue()];
				ValueQT[] newValues = new ValueQT[count.intValue()]; 	
				com.softing.opc.client.impl.jna.OTValueData[] valuesArray = null;
				if(pValues != null)
				{
					valuesArray = (com.softing.opc.client.impl.jna.OTValueData[])pValues.toArray(count.intValue());
				}
				int[] results = new int[count.intValue()];
				String[] itemIdArray = null;
				String[] itemPathsArray = null;
				com.softing.opc.client.impl.jna.OTObjectData[] itemDataArray = null;
				if(pItemIDs != null)
				{
					itemIdArray = new String[count.intValue()];
				}
				if(pItemPaths != null)
				{
					itemPathsArray = new String[count.intValue()];
				}
				if(pItemData != null)
				{
					itemDataArray = (com.softing.opc.client.impl.jna.OTObjectData[])pItemData.toArray(count.intValue());
				}
				
				
				if(pValues != null){
					for (int i = 0; i < count.intValue(); i++){
						newValues[i] = new ValueQT(valuesArray[i]);	
						results[i] = pResults.getPointer().getNativeLong(i* Native.LONG_SIZE).intValue();
						if(pItemIDs != null)
						{
							itemIdArray[i] = pItemIDs.getValue().getString(i * Native.POINTER_SIZE, true);
						}
						if(pItemPaths != null)
						{
							itemPathsArray[i] = pItemPaths.getValue().getString(i * Native.POINTER_SIZE, true);
						}
					}
				}else{
					newValues = null;
				}	//	end if...else

				if(objectContext.m_objectType.longValue() == EnumObjectType.DASESSION.getLong() ){
					currentSession = (DaSession)daSessionList_.get(objectContext.m_objectData.m_userData.intValue());
					if(currentSession != null){
						currentSession.onWriteComplete(
							executionContext.intValue(),
							itemIdArray,
							itemPathsArray,
							newValues,
							results,
							result.intValue());
					} //end if
				}
				if(objectContext.m_objectType.intValue() == EnumObjectType.DASUBSCRIPTION.getLong()){
					currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
					if(currentSession != null)
					{							
						currentSubscription = currentSession.find(objectContext.m_objectData.m_userData.intValue());
						if(currentSubscription != null){
							
							for(int i = 0; i < count.intValue(); i++){
								items[i] = currentSubscription.find(itemDataArray[i].m_userData.intValue());									
							}
							currentSubscription.onWriteComplete(
								executionContext.intValue(),
								items,
								newValues,
								results,
								result.intValue());
						}
					} //end if
				}
			} //end if		
			catch(Exception exc){
				logger.error("HandleWriteComplete() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleWriteComplete", 
					exc.getMessage());
				*/
			}
		}
	};

	OTClient.OTCOnServerShutdown handleServerShutdown = new OTClient.OTCOnServerShutdown() {
		
		@Override
		public byte invoke(com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData,
				ByteByReference reason) {
			
			boolean res = true;//after server shutsdown the reconnection will be performed
			try{
				DaSession daSession = (DaSession)daSessionList_.get(sessionData.m_userData.intValue());
				if(daSession != null){
					 res = daSession.onShutdown(reason.getPointer().getString(0, true));
				}

			}catch(Exception exc){
				logger.error("HandleServerShutdown() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleServerShutdown", 
					exc.getMessage());
				*/				
			}
			return res?(byte)1:(byte)0;	
		}
	};

	OTClient.OTCOnGetServerStatus handleGetServerStatus = new OTClient.OTCOnGetServerStatus() {
		
		@Override
		public void invoke(NativeLong executionContext,
				com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, NativeLong result,
				OTCServerStatus currentStatus) {
			try{
				int state = currentStatus.m_state.intValue();
				Date startTime = currentStatus.m_startTime.toDate();
				Date lastUpdateTime = currentStatus.m_lastUpdateTime.toDate();
				Date currentTime = currentStatus.m_currentTime.toDate();

				int[] supportedLCIDs = currentStatus.m_supportedLCIDs.getPointer().getIntArray(0, currentStatus.m_countLCIDs.intValue());
				String[] LCIDs = new String[currentStatus.m_countLCIDs.intValue()];

				for (int i = 0; i < currentStatus.m_countLCIDs.intValue(); i++){						
					if(supportedLCIDs[i] == 2048)
					{
						LCIDs[i] = "System Default";
					}
					else if(supportedLCIDs[i] == 1024)
					{
						LCIDs[i] = "User Default";
					}
					else if(supportedLCIDs[i] == 0)
					{
						LCIDs[i] = "Neutral";
					}
					else
					{
						java.util.Locale lc = NameDecoder.getWindowsLocale((int)supportedLCIDs[i]);
						LCIDs[i] = lc.toString();
					}					
				}
				ServerStatus serverStatus = new ServerStatus(
					state,
					startTime,
					currentTime,
					currentStatus.m_vendorInfo.getString(0, true),
					currentStatus.m_productVersion.getString(0, true),
					LCIDs,
					lastUpdateTime,
					currentStatus.m_groupCount.intValue(),
					currentStatus.m_bandwidth.intValue(),
					currentStatus.m_statusInfo.getString(0, true));				
			
				
				DaSession currentDaSession = (DaSession)daSessionList_.get(sessionData.m_userData.intValue());
				if(currentDaSession != null){	
					currentDaSession.onGetServerStatus(executionContext.intValue(), serverStatus, result.intValue());
				}	
			
			}catch(Exception exc){
				logger.error("HandleGetServerStatus() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application.HandleGetServerStatus", 
					exc.getMessage());
				*/
			}
		}
	};

	OTClient.OTCOnPerfromStateTransition handlePerformStateTransition = new OTClient.OTCOnPerfromStateTransition() {
		
		@Override
		public void invoke(NativeLong executionContext, ByValue objectContext,
				NativeLong result) {
			try{

				if(objectContext.m_objectType.intValue() == EnumObjectType.DASESSION.getLong() ){
					DaSession currentSession = (DaSession)daSessionList_.get(objectContext.m_objectData.m_userData.intValue());
					if(currentSession != null){
						currentSession.onPerformStateTransition(executionContext.intValue(), result.intValue());
					} //end if
				}

				if(objectContext.m_objectType.longValue() == EnumObjectType.DASUBSCRIPTION.getLong()){
					DaSession currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
					if(currentSession != null){
						
						DaSubscription currentSubscription = currentSession.find(objectContext.m_objectData.m_userData.intValue());
						if(currentSubscription != null){
							currentSubscription.onPerformStateTransition(executionContext.intValue(), result.intValue());
						} //end if
					}
				}
				if(objectContext.m_objectType.longValue() == EnumObjectType.DAITEM.getLong() ){
					DaSession currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
					if(currentSession != null){
					
						DaSubscription currentSubscription = currentSession.find(objectContext.m_subscriptionData.m_userData.intValue());
						if(currentSubscription != null){
							
							DaItem currentItem = currentSubscription.find(objectContext.m_objectData.m_userData.intValue());
							if(currentItem != null){
								currentItem.onPerformStateTransition(executionContext.intValue(),result.intValue());
							}
						} //end if
					} //end if
				}
			}
			catch(Exception exc){
				//exc.printStackTrace();
				logger.error("HandlePerformStateTransition() - " + exc.getMessage(), exc);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandlePerformStateTransition", 
					exc.getMessage());
				*/
			}
		}
	};

	OTClient.OTCOnUpdateDAItemAttributes handleUpdateDaItemAttributes = new OTClient.OTCOnUpdateDAItemAttributes() {
		
		@Override
		public void invoke(NativeLong executionContext, ByValue objectContext,
				NativeLong result, NativeLong itemCount, OTObjectData.ByReference pItemData,
				byte fromServer, NativeLong attributesCount,
				NativeLongByReference pWhatAttributes,
				NativeLongByReference pResults) {
			
			try{	
				DaItem[] items = new DaItem[itemCount.intValue()];
				DaSession currentSession = null;
				DaSubscription currentSubscription = null;
				int[] results = new int[attributesCount.intValue()];
				
				com.softing.opc.client.impl.jna.OTObjectData[] itemDataArray = (com.softing.opc.client.impl.jna.OTObjectData[])pItemData.toArray(itemCount.intValue());
				
				
				if(objectContext.m_objectType.longValue() == EnumObjectType.DASUBSCRIPTION.getLong()){
					currentSession = (DaSession)daSessionList_.get(objectContext.m_sessionData.m_userData.intValue());
					if(currentSession != null){
						
						currentSubscription = currentSession.find(objectContext.m_objectData.m_userData.intValue());
						if(currentSubscription != null){
							
							for(int i = 0; i< itemCount.intValue(); i++){
								items[i] = currentSubscription.find(itemDataArray[i].m_userData.intValue());									
							}
						} //end if
					} //end if
				}				
				EnumObjectAttribute[] whatOtherAttributes = new EnumObjectAttribute[attributesCount.intValue()];

				for(int j = 0; j< attributesCount.intValue(); j++){
					int intAttributeValue = pWhatAttributes.getPointer().getNativeLong(j* Native.LONG_SIZE).intValue();
					whatOtherAttributes[j] = EnumObjectAttribute.getEnum(intAttributeValue);
					results[j] = pResults.getPointer().getNativeLong(j* Native.LONG_SIZE).intValue();
				}
				if(currentSubscription != null){
					if(fromServer == 1){
						currentSubscription.OnGetDaItemAttributesFromServer(
							executionContext.intValue(),
							items,
							whatOtherAttributes,
							results,
							result.intValue());
					}
					if(fromServer == 0){
						currentSubscription.OnSetDaItemAttributesToServer(
							executionContext.intValue(),
							items,
							whatOtherAttributes,
							results,
							result.intValue());
					}
				} //end if
			} 	
			catch(Exception exc){
				logger.error("HandleUpdateDaItemAttributes() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleUpdateDaItemAttributes", 
					exc.getMessage());
				*/
			}
			
		}
	};

	OTClient.OTCOnBrowseAddressSpace handleBrowseAddressSpace = new OTClient.OTCOnBrowseAddressSpace() {
		
		@Override
		public void invoke(NativeLong executionContext,
				com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, NativeLong result,
				NativeLong addressSpaceElementHandle,
				ByteByReference addressSpaceElementID,
				ByteByReference addressSpaceElementPath,
				OTCAddressSpaceBrowseOptions pBrowseOptions,
				NativeLong addressSpaceElementDataCount,
				com.softing.opc.client.impl.jna.OTCAddressSpaceElementData.ByReference pAddressSpaceElement) {
			try{	
								
				DaSession currentDaSession = (DaSession)daSessionList_.get(sessionData.m_userData.intValue());
				if(currentDaSession != null)
				{		
					if ( result.intValue() == EnumResultCode.S_OK.toInt())
					{
						com.softing.opc.client.impl.jna.OTCAddressSpaceElementData[] addressSpaceElementArray = (com.softing.opc.client.impl.jna.OTCAddressSpaceElementData[])pAddressSpaceElement.toArray(addressSpaceElementDataCount.intValue());
						
						DaAddressSpaceElement[] addressSpaceElements = new DaAddressSpaceElement[addressSpaceElementDataCount.intValue()];	
						for (int i = 0; i< addressSpaceElementDataCount.intValue(); i++){
							addressSpaceElements[i] = new DaAddressSpaceElement(
									addressSpaceElementArray[i].m_name.getString(0, true),
									addressSpaceElementArray[i].m_itemID.getString(0, true),
									addressSpaceElementArray[i].m_itemPath.getString(0, true),
									addressSpaceElementArray[i].m_objectHandle,
									null);							
						}
						currentDaSession.onBrowse(
							executionContext.intValue(),
							addressSpaceElementHandle,
							addressSpaceElementID.getPointer().getString(0, true),
							addressSpaceElementPath.getPointer().getString(0, true),
							addressSpaceElements,
							result.intValue());
					}
					else
					{
						currentDaSession.onBrowse(
							executionContext.intValue(),
							addressSpaceElementHandle,
							"",
							"",
							null,
							result.intValue());
					}					

				} //end if
			}catch(Exception exc){
				logger.error("HandleBrowseAddressSpace() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleBrowseAddressSpace", 
					exc.getMessage());
				*/
			}
		}
	};



	OTClient.OTCOnGetDAProperties handleGetDaProperties = new OTClient.OTCOnGetDAProperties() {
		
		@Override
		public void invoke(NativeLong executionContext,
				com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, NativeLong result,
				NativeLong addressSpaceElementHandle,
				ByteByReference addressSpaceElementID,
				ByteByReference addressSpaceElementPath,
				OTCGetDAPropertiesOptions pGetPropertiesOptions,
				NativeLong propertyDataCount, OTCDAPropertyData.ByReference pPropertyData) {
			
			try{	
				DaProperty[] someProperties = new DaProperty[propertyDataCount.intValue()];						
				
				DaSession currentSession = (DaSession)daSessionList_.get(sessionData.m_userData.intValue());	

				if (currentSession != null)
				{
					OTCDAPropertyData myData;
					for(int i = 0; i < propertyDataCount.intValue(); i++)
					{
						myData = new OTCDAPropertyData(pPropertyData.getPointer().share(i* new OTCDAPropertyData().size()));
						
						someProperties[i] = new DaProperty(
							myData.m_ID.intValue(),
							myData.m_name.getString(0, true),
							myData.m_itemID.getString(0, true),
							myData.m_itemPath.getString(0, true),
							myData.m_description.getString(0, true),
							VARIANT.getSysType(myData.m_datatype),
							new ValueQT(myData.m_value),
							myData.m_result.intValue());
					} //end for

					currentSession.onGetDaProperties(
						executionContext.intValue(),
						addressSpaceElementHandle,
						addressSpaceElementID.getPointer().getString(0, true),
						addressSpaceElementPath.getPointer().getString(0, true),
						someProperties,
						result.intValue());
				} //end if											
			}
			catch(Exception exc){
				logger.error("HandleGetDaProperties() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleGetDaProperties", 
					exc.getMessage());
				*/
			}
			
		}
	};




	OTClient.OTCOnGetErrorString handleGetErrorString = new OTClient.OTCOnGetErrorString() {
		@Override
		public void invoke(NativeLong executionContext,
				com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, NativeLong result,
				NativeLong errorCode, ByteByReference errorString) {
			try{
				DaSession aDaSession = (DaSession)daSessionList_.get(sessionData.m_userData.intValue());
							
				if(aDaSession != null)
				{
					aDaSession.onGetErrorString(
						executionContext.intValue(),
						errorCode.intValue(),
						errorString.getPointer().getString(0, true),
						result.intValue());
				}
			}
			catch(Exception exc){
				logger.error("HandleGetErrorString() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleGetErrorString", 
					exc.getMessage());
				*/
			}
		}
	};

	OTClient.OTCOnLogon handleLogon = new OTClient.OTCOnLogon() {
		@Override
		public void invoke(NativeLong executionContext,
				com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, NativeLong result,
				ByteByReference userName, ByteByReference password) {
			try{		
				DaSession aSession = (DaSession)daSessionList_.get(sessionData.m_userData.intValue());
		
				if(aSession != null)
				{
					aSession.onLogon(
						executionContext.intValue(),
						userName.getPointer().getString(0, true),
						password.getPointer().getString(0, true),
						result.intValue());
				}//end if
			}//try
			catch(Exception exc){
				logger.error("HandleLogon() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleLogon", 
					exc.getMessage());
				*/
			}//end catch
			
		}
	};

	OTClient.OTCOnLogoff handleLogoff = new OTClient.OTCOnLogoff() {
		
		@Override
		public void invoke(NativeLong executionContext,
				com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, NativeLong result) {
			try{
				DaSession aSession = (DaSession)daSessionList_.get(sessionData.m_userData.intValue());
		
				if(aSession != null){
					aSession.onLogoff(executionContext.intValue(), result.intValue());
				}//end if
			}//try
			catch(Exception exc){
				logger.error("HandleLogoff() - " + exc.getMessage(), exc);
				//exc.printStackTrace();
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application. HandleLogoff", 
					exc.getMessage());
				*/
			}//catch
		}
	};


	public int activate(EnumFeature feature, String key){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			res = OTClient.INSTANCE.OTActivate((byte)feature.toShort(), new WString(key), (byte)0);

			logger.info("Activate() - Result of activating application:" + res);
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Application.Activate", 
				" Result of activating application:" + res);
			*/
		}
		catch(Exception exc){
			logger.error("Activate() - " + res, exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Application.Activate", 
				exc.getMessage());
			*/			
		}
		return res;
	}


	public int forceDemoVersion(){
		return OTClient.INSTANCE.OTActivate((byte)0, new WString(""), (byte)1);
	}


	public int initialize(){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			if (!isInit_){

				res = OTClient.INSTANCE.OTCInitialize(initData_).intValue();	
				if(ResultCode.SUCCEEDED(res)){
					isInit_ = true;						
				}	
				else{
					logger.error("Initialize() - Application initialization failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Application. Initialize", 
						" Application initialization failed! Result: " + res);
					*/
				}
			}
			advise();
		}
		catch(Exception exc){
			logger.error("Initialize() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Application. Initialize", 
				exc.getMessage());
			*/			
		}
		return res;
	}


	public int processCommandLine(String aCommandLine){
		return OTClient.INSTANCE.OTCProcessCommandLine(new WString(aCommandLine)).intValue();
	}


	public int removeDaSession(DaSession aDaSession){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			if(aDaSession != null){
				res = OTClient.INSTANCE.OTCRemoveDASession(aDaSession.getHandle()).intValue();
				if(ResultCode.SUCCEEDED(res)){
					daSessionList_.remove(aDaSession.getHashCode());
				}
				else{
					logger.error("RemoveDasession() - The session could not be removed from the application! Result: " + res);
					/*
					Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application.RemoveDasession", 
					"The session could not be removed from the application! Result: " + res);
				    */
				}	//	end if...else
			}
			else{
				res = EnumResultCode.E_INVALIDARG.toInt();
				logger.error("RemoveDasession() - The argument is invalid ! The session cannot be null.");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application.RemoveDasession", 
					"The argument is invalid! The session cannot be null!");
				*/
			}
		}
		catch(Exception exc){
			logger.error("RemoveDasession() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Application.RemoveDasession",
				exc.getMessage());
			*/				
		}
		return res;
	}


	//public int RemoveAeSession(AeSession anAeSession){
	//	return 0;
	//}


	public int terminate(){
		int res = EnumResultCode.E_FAIL.toInt();
		try{

			otnCCallBacks_.m_OTOnTrace					 	 = null;
			otnCCallBacks_.m_OTCOnStateChange				 = null;
			otnCCallBacks_.m_OTCOnDataChange				 = null;
			otnCCallBacks_.m_OTCOnReadComplete			 	 = null;
			otnCCallBacks_.m_OTCOnWriteComplete			 	 = null;
			otnCCallBacks_.m_OTCOnPerfromStateTransition	 = null;
			otnCCallBacks_.m_OTCOnServerShutdown			 = null;
			otnCCallBacks_.m_OTCOnUpdateAttributes		 	 = null;
			otnCCallBacks_.m_OTCOnUpdateDAItemAttributes	 = null;
			otnCCallBacks_.m_OTCOnValidateDAItems			 = null;
			otnCCallBacks_.m_OTCOnGetServerStatus			 = null;
			otnCCallBacks_.m_OTCOnBrowseAddressSpace		 = null;
			otnCCallBacks_.m_OTCOnGetDAProperties			 = null;
			otnCCallBacks_.m_OTCOnBrowseServer				 = null;	
			otnCCallBacks_.m_OTCOnReceivedEvents			 = null;
			otnCCallBacks_.m_OTCOnGetErrorString			 = null;
			otnCCallBacks_.m_OTCOnLogon						 = null;
			otnCCallBacks_.m_OTCOnLogoff					 = null;

			res = OTClient.INSTANCE.OTCTerminate().intValue();
			int resJNI = (int)JNIOTClient.INSTANCE.OTCTerminate();
			daSessionList_.clear();
			//serverBrowserList_.clear();
			if(ResultCode.SUCCEEDED(res) 
					&& ResultCode.SUCCEEDED(resJNI)){
				if(isInit_) 
					isInit_ = false;
				if(isAdvised_) 
					isAdvised_ = false;
			} 
			else{
				logger.error("Terminate() - Terminate application failed. Result: " + res);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Application.Terminate", 
					" Terminate application failed! Result: " + res);
				*/
			}

		}catch(Exception exc){
			logger.error("Terminate() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Application.Terminate", 
				exc.getMessage());
			*/			
		}
		return res;
	}


	public void trace(EnumTraceLevel traceLevel, EnumTraceGroup traceMask, String objectId, String message){
		Trace.WriteLine(traceLevel.toByte(),traceMask.toLong(),objectId,message);
	}


	public void enableTracing(EnumTraceGroup errorLevelMask, EnumTraceGroup warningLevelMask, EnumTraceGroup infoLevelMask, EnumTraceGroup debugLevelMask, String fileName, int fileMaxSize, int maximumBackups){

		if (fileName != null && !fileName.equals("")){
			
			Trace.setEnableTraceToFile(true);
			Trace.setMaximumBackups(maximumBackups);
			Trace.setFileMaxSize(1000000);				
			Trace.setFileName(fileName);
		}
		else{
			Trace.setEnableTraceToFile(false);
		}			
					
		Trace.setDebugLevelMask(debugLevelMask.toLong());
		Trace.setErrorLevelMask(errorLevelMask.toLong());
		Trace.setInfoLevelMask(infoLevelMask.toLong());
		Trace.setWarningLevelMask(warningLevelMask.toLong());
	}

}