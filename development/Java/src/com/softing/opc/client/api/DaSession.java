package com.softing.opc.client.api;

import java.io.Serializable;
import java.util.*;

import com.softing.opc.client.enums.EnumClassContext;
import com.softing.opc.client.enums.EnumExecutionType;
import com.softing.opc.client.enums.EnumOPCSpecification;
import com.softing.opc.client.enums.EnumObjectAttribute;
import com.softing.opc.client.enums.EnumResultCode;
import com.softing.opc.client.enums.EnumTraceGroup;
import com.softing.opc.client.enums.EnumTraceLevel;
import com.softing.opc.client.enums.EnumUpdateAttributesBehavior;
import com.softing.opc.client.enums.ResultCode;
import com.softing.opc.client.impl.NameDecoder;
import com.softing.opc.client.impl.jna.FILETIME;
import com.softing.opc.client.impl.jna.OTCAddressSpaceBrowseOptions;
import com.softing.opc.client.impl.jna.OTCAddressSpaceElementData;
import com.softing.opc.client.impl.jna.OTCDAPropertyData;
import com.softing.opc.client.impl.jna.OTCExecutionOptions;
import com.softing.opc.client.impl.jna.OTCGetDAPropertiesOptions;
import com.softing.opc.client.impl.jna.OTCObjectAttributes;
import com.softing.opc.client.impl.jna.OTCServerStatus;
import com.softing.opc.client.impl.jna.OTClient;
import com.softing.opc.client.impl.jna.OTValueData;
import com.softing.opc.client.impl.jna.VARIANT;

import com.softing.opc.client.impl.jni.*;

import com.sun.jna.*;
import com.sun.jna.Native;
import com.sun.jna.NativeLong;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.ByteByReference;
import com.sun.jna.ptr.NativeLongByReference;
import com.sun.jna.ptr.PointerByReference;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public class DaSession extends ObjectSpaceElement implements Serializable {

	final Logger logger = LoggerFactory.getLogger(DaSession.class);
	private static final long serialVersionUID = 7526471155622776147L;
	
	//protected Hashtable<Integer, DaSubscription> subscriptionList_ = new Hashtable<Integer, DaSubscription>();
	private final OTCObjectAttributes objectAttributes_ = new OTCObjectAttributes();
	private final Object attributesJanitor_ = new Object();
	
	protected Hashtable subscriptionList_ = new Hashtable();
	

	public DaSession(){

	}

	public DaSession(String url){
		subscriptionList_ = new Hashtable();
		try{
			int res = EnumResultCode.E_FAIL.toInt();			

			res = Application.INSTANCE.addDaSession(url, this);

			if(ResultCode.FAILED(res)){
				logger.error("Constructor - The session could not be added to the application. Result: " + res);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Constructor", 
					"The session could not be added to the application! Result: " + res);
				*/
			}	
		}
		catch(Exception exc){
			logger.error("Constructor - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSession.Constructor",
				exc.getMessage());
			*/				
		}	
	}


	public DaSubscription[] getSubscriptionList(){
		
		synchronized(subscriptionList_)
		{
			DaSubscription[] subscriptionVector = new DaSubscription[subscriptionList_.size()];
			int i = 0;
	
			for(Object daSubscription : subscriptionList_.values()){
	
				subscriptionVector[i] = (DaSubscription)daSubscription;
				i++;
	
			} //end foreach
	
			return subscriptionVector;
		}
	}


	public String getUrl(){
		String url = "";
		OTCObjectAttributes objectAttributes = new OTCObjectAttributes();	
		try{
			if(this.getHandle().longValue() != 0){
				synchronized(attributesJanitor_)
				{
					OTCExecutionOptions options = new OTCExecutionOptions() ;
					int result = -1;
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_URL.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
							this.getHandle(),
							(byte)1,
							new NativeLong(1),
							whatAttributes,
							results,
							options).intValue();
						
						if(ResultCode.FAILED(updateResult)){
							logger.error("getUrl() - Update attribute DASESSION_URL from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.URL.Get", 
								"Update attribute DASESSION_URL from server failed!Result: " + updateResult);
							*/
						}	//	end if
					}	//	end lock
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASESSION_URL.getLong()),
						objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult)) {
						url = objectAttributes.m_daSessionURL.getString(0, true);
						OTClient.INSTANCE.OTFreeMemory(objectAttributes.m_daSessionURL);
						
						//url = Marshal.PtrToStringUni(objectAttributes.m_daSessionURL);								
						//OTBFunctions.OTFreeMemory(objectAttributes.m_daSessionURL);
					}//	end if
					else{				
						logger.error("getUrl() - Get attribute DASESSION_URL from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSession.URL.Get", 
							"Get attribute DASESSION_URL from server failed!Result: " + getAttrsResult);
						*/
					} //end else
				}//	end lock
			}//end if
		}
		catch(Exception exc){
			logger.error("getUrl() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.URL.Get", 
				exc.getMessage());
			*/
		}
		return url;				
	}


	public String getLocaleId(){
		int lcid = 0;
		String cultureName = "";
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				int result = -1;
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_LCID.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
							this.getHandle(),
							(byte)1,
							new NativeLong(1),
							whatAttributes,
							results,
							options).intValue();
						
						if(ResultCode.FAILED(updateResult)){
							logger.error("getLocaleId() - Update attribute DASESSION_LCID failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.LocaleId.Get",
								"Update attribute DASESSION_LCID failed! Result: " + updateResult);
							*/	
						}
					}// end if
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASESSION_LCID.getLong()),
						objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult)) {
						lcid = objectAttributes.m_daSessionLCID.intValue();
						switch(lcid){
							case 2048:		
								cultureName = "System Default";
								break;
							case 1024:									
								cultureName = "User Default";
								break;
							case 0:
								cultureName = "Neutral";
								break;
							default:
								java.util.Locale lc = NameDecoder.getWindowsLocale((int)lcid);
								cultureName = lc.toString();	
								break;
						}
						
					}//end if				
					else{	
						logger.error("getLocaleId() - Get attribute DASESSION_LCID failed. Error code:" + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSession.LocaleId.Get",
							"Get attribute DASESSION_LCID failed! Error message: " + getAttrsResult );
						*/	
					} //end else
				}//	end lock
			}	//	end if					
		}
		catch(Exception exc){
			logger.error("getLocaleId() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"Da.Session.LocaleID.Get",
				exc.getMessage());
			*/		
		}
		return cultureName;	
	}


	public void setLocaleId(String value){
		int lcid;
		try{
			if(value.equals("System Default"))
			{
					lcid = 2048;
			}
			else if (value.equals("User Default"))
			{
					lcid = 1024;
			}
			else if (value.equals("Neutral"))
			{
					lcid = 0;
			}
			else
			{
					java.util.Locale loc = new java.util.Locale(value);
					lcid = NameDecoder.getLCID(loc);
			}
		
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){		

					objectAttributes_.m_daSessionLCID = new NativeLong(lcid);

					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASESSION_LCID.getLong()),
											objectAttributes_).intValue();
					if(ResultCode.FAILED(setAttrsResult)){
						logger.error("setLocaleId() - Set attribute DASESSION_LCID failed. Result: " + setAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSession.LocaleId.Set",
							"Set attribute DASESSION_LCID failed! Result: " + setAttrsResult );
						*/	
					} // end if
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_LCID.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
												this.getHandle(),
												(byte)0,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("setLocaleId() - Set attribute DASESSION_LCID failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.LocaleId.Set",
								"Set attribute DASESSION_LCID failed! Result: " + updateResult );
							*/	
						}	//	end if
					}	//	end if	
				}//	end lock
			}
			else{
				logger.error("setLocaleId() - Set LocaleId failed. Session is not added to the application");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSession.LocaleId.Set",
					"Set LocaleId failed! Session is not added to the application" );
				*/
			}	//	end if...else
		}
		catch(Exception exc){
			logger.error("setLocaleId() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"Da.Session.LocaleID.Set",
				exc.getMessage());
			*/
		}	//	end try...catch
	}


	public String getClientName(){
		String clientName = "";
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_CLIENTNAME.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
							this.getHandle(),
							(byte)1,
							new NativeLong(1),
							whatAttributes,
							results,
							options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getClientName() - Get attribute DASESSION_CLIENTNAME failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.ClientName.Get",
								"Get attribute DASESSION_CLIENTNAME failed! Result: " + updateResult );
							*/	
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASESSION_CLIENTNAME.getLong()), 
						objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult)) {
						clientName = objectAttributes.m_daSessionClientName.getString(0, true);
						OTClient.INSTANCE.OTFreeMemory(objectAttributes.m_daSessionClientName);
					}	
					else{
						if(ResultCode.FAILED(getAttrsResult)){
							logger.error("getClientName() - Get attribute DASESSION_CLIENTNAME failed. Result: " + getAttrsResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.ClientName.Get",
								"Get attribute DASESSION_CLIENTNAME failed! Result: " + getAttrsResult );
							*/	
						}	//	end if
					}	//	end if...else
				}//end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getClientName() - " + exc.getMessage(), exc) ;
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"Da.Session.ClientName.Get", 
				exc.getMessage());
			*/
		}
		return clientName;
	}
	

	public void setClientName(String value){
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				if (!value.equals("")){
					synchronized(attributesJanitor_){	
						
						Pointer pStr = OTClient.INSTANCE.OTSysAllocString(new WString(value));
						//ByteByReference bbr = new ByteByReference();
						//bbr.setPointer(pStr);
						objectAttributes_.m_daSessionClientName = pStr;
						
						int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
							this.getHandle(),
							new NativeLong(EnumObjectAttribute.DASESSION_CLIENTNAME.getLong()), 
							objectAttributes_).intValue();
						if(ResultCode.FAILED(setAttrsResult)){
							logger.error("setClientName() - Set attribute DASESSION_CLIENTNAME failed. Result: " + setAttrsResult) ;
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.ClientName.Set",
								"Set attribute DASESSION_CLIENTNAME failed! Result: " + setAttrsResult );
							*/	
						}
						if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
						{
							//prepare what attributes
							NativeLongByReference whatAttributes = new NativeLongByReference();
							whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_CLIENTNAME.getLong()));
							
							//prepare results
							NativeLongByReference results = new NativeLongByReference();
							results.setValue(new NativeLong(result));
							
							int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
								this.getHandle(),
								(byte)0,
								new NativeLong(1),
								whatAttributes,
								results,
								options).intValue();
							if(ResultCode.FAILED(updateResult)){
								logger.error("setClientName() - Set attribute DASESSION_CLIENTNAME failed. Result: " + updateResult) ;
								/*
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT, 
									"DaSession.ClientName.Set",
									"Set attribute DASESSION_CLIENTNAME failed! Result: " + updateResult);
								*/	
							}
						}//end if
						OTClient.INSTANCE.OTSysFreeString(pStr);
					}	//	end lock
				}	//	end if
			}
			else{
				logger.error("setClientName() - Set ClientName failed. Session is not added to the application") ;
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSession.ClientName.Set",
					"Set ClientName failed! Session is not added to the application");
				*/	
			}	//	end if...else
		}
		catch(Exception exc){
			logger.error("setClientName() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"Da.Session.ClientName.Set",
				exc.getMessage());
			*/
		}	//	end try...catch
	}
	

	public EnumOPCSpecification getSupportedOpcSpecification(){
		EnumOPCSpecification specification = EnumOPCSpecification.DEFAULT;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_SUPPORTEDOPCSPECIFICATION.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
							this.getHandle(),
							(byte)1,
							new NativeLong(1),
							whatAttributes,
							results,
							options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getSupportedOpcSpecification() - Update attribute DASESSION_SUPPORTEDOPCSPECIFICATION failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.SupportedOPCSpecification.Get",
								"Update attribute DASESSION_SUPPORTEDOPCSPECIFICATION failed! Result: " + updateResult );
							*/	
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASESSION_SUPPORTEDOPCSPECIFICATION.getLong()),
						objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult)) {						
						specification = EnumOPCSpecification.getEnum(objectAttributes.m_daSessionSupportedOPCSpecification);
					}
					else{
						logger.error("getSupportedOpcSpecification() - Get attribute DASESSION_SUPPORTEDOPCSPECIFICATION failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSession.SupportedOPCSpecification.Get",
							"Get attribute DASESSION_SUPPORTEDOPCSPECIFICATION failed! Result: " + getAttrsResult );
						*/									
					}	//	end if...else
				}	//	end lock
			}	//	end if
		}
		catch(Exception exc){
			logger.error("getSupportedOpcSpecification() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"Da.Session.SupportedOPCSpecification.Get",
				exc.getMessage());
			*/
		}	//end try...catch
		return specification;
	}


	public EnumOPCSpecification getForcedOpcSpecification(){
		EnumOPCSpecification specification = EnumOPCSpecification.DEFAULT;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_FORCEDOPCSPECIFICATION.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
							this.getHandle(),
							(byte)1,
							new NativeLong(1),
							whatAttributes,
							results,
							options).intValue();

						if(ResultCode.FAILED(updateResult)){
							logger.error("getForcedOpcSpecification() - Update attribute DASESSION_FORCEDOPCSPECIFICATION failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.ForcedOPCSpecification.Get",
								"Update attribute DASESSION_FORCEDOPCSPECIFICATION failed! Result: " + updateResult );
							*/	
						}	//	end if
					}	//	end if
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASESSION_FORCEDOPCSPECIFICATION.getLong()),
						objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult)) {
						specification = EnumOPCSpecification.getEnum(objectAttributes.m_daSessionForcedOPCSpecification);
					}// end if
					else{
						logger.error("getForcedOpcSpecification() - Update attribute DASESSION_SUPPORTEDOPCSPECIFICATION failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSession.SupportedOPCSpecification.Get",
							"Update attribute DASESSION_SUPPORTEDOPCSPECIFICATION failed! Result: " + getAttrsResult );
						*/									
					}
				}//	end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getForcedOpcSpecification() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSession.ForcedOPCSpecification.Get", 
				exc.getMessage());
			*/
		}
		return specification;
	}


	public void setForcedOpcSpecification(EnumOPCSpecification value){
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){	
					objectAttributes_.m_daSessionForcedOPCSpecification = (byte)value.toShort();
					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASESSION_FORCEDOPCSPECIFICATION.getLong()),
						objectAttributes_).intValue();
					if(ResultCode.FAILED(setAttrsResult)){
						logger.error("setForcedOpcSpecification() - Set attribute DASESSION_FORCEDOPCSPECIFICATION failed. Result: " + setAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSession.ForcedOPCSpecification.Set",
							"Set attribute DASESSION_FORCEDOPCSPECIFICATION failed! Result: " + setAttrsResult );
						*/	
					}
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_FORCEDOPCSPECIFICATION.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
							this.getHandle(),
							(byte)0,
							new NativeLong(1),
							whatAttributes,
							results,
							options).intValue();	
						if(ResultCode.FAILED(updateResult)){
							logger.error("setForcedOpcSpecification() - Update attribute DASESSION_FORCEDOPCSPECIFICATION failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.ForcedOPCSpecification.Set",
								"Update attribute DASESSION_FORCEDOPCSPECIFICATION failed! Result: " + updateResult );
							*/	
						}
					}// end if
				}//	end lock
			}// end if
			else{
				logger.error("setForcedOpcSpecification() - Set ForcedOpcSpecification failed. Session is not added to the application");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSession.ForcedOPCSpecification.Set",
					"Set ForcedOpcSpecification failed! Session is not added to the application");
				*/	
			}	//	end if...else
		}
		catch(Exception exc){
			logger.error("setForcedOpcSpecification() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"Da.Session.ForcedOPCSpecification.Set",
				exc.getMessage());
			*/
		}
	}


	public EnumClassContext getClassContext(){
		int classContext = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				int result = -1;
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_CLASSCONTEXT.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
							this.getHandle(),
							(byte)1,
							new NativeLong(1),
							whatAttributes,
							results,
							options).intValue();
						
						if(ResultCode.FAILED(updateResult)){
							logger.error("getClassContext() - Update attribute DASESSION_CLASSCONTEXT failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.ClassContext.Get",
								"Update attribute DASESSION_CLASSCONTEXT failed! Result: " + updateResult);
							*/	
						}
					}// end if
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASESSION_CLASSCONTEXT.getLong()),
						objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult)) {
						classContext = objectAttributes.m_daSessionComClsctx.intValue();								
					}//end if				
					else{
						logger.error("getClassContext() - Get attribute DASESSION_CLASSCONTEXT failed. Error message: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSession.ClassContext.Get",
							"Get attribute DASESSION_CLASSCONTEXT failed! Error message: " + getAttrsResult );
						*/	
					} //end else
				}//	end lock
			}//end if
		}				
		catch(Exception exc){
			logger.error("getClassContext() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"Da.Session.LocaleID.Get",
				exc.getMessage());
			*/		
		}	//	end try...catch
		return EnumClassContext.getEnum(classContext);
	}


	public void setClassContext(EnumClassContext value){
		try{					
			if (this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){		
					objectAttributes_.m_daSessionComClsctx = new NativeLong(value.getInt());
					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASESSION_CLASSCONTEXT.getLong()),
						objectAttributes_).intValue();
					if(ResultCode.FAILED(setAttrsResult)){
						logger.error("setClassContext() - Set attribute DASESSION_CLASSCONTEXT failed. Result: " + setAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSession.ClassContext.Set",
							"Set attribute DASESSION_CLASSCONTEXT failed! Result: " + setAttrsResult );
						*/	
					} // end if
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASESSION_CLASSCONTEXT.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateAttributes(
							this.getHandle(),
							(byte)0,
							new NativeLong(1),
							whatAttributes,
							results,
							options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("setClassContext() - Set attribute DASESSION_CLASSCONTEXT failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSession.ClassContext.Set",
								"Set attribute DASESSION_CLASSCONTEXT failed! Result: " + updateResult );
							*/	
						}	//	end if
					}	//	end if	
				}//	end lock
			}
			else{
				logger.error("setClassContext() - Set ClassContext failed. Session is not added to the application");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSession.ClassContext.Set",
					"Set ClassContext failed! Session is not added to the application" );
				*/	
			}//end if...else
		}
		catch(Exception exc){
			logger.error("setClassContext() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.ClassContext.Set",
				exc.getMessage());
			*/
		}	//	end try...catch
	}


	protected int addDaSubscription(int updateRate, DaSubscription aSubscription){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			if (aSubscription != null){
				NativeLongByReference groupHandle = new NativeLongByReference();
				
				res = OTClient.INSTANCE.OTCAddDASubscription(
						this.getHandle(),
						new NativeLong(updateRate),
						new NativeLong(aSubscription.getHashCode()),
						groupHandle).intValue();
				
				if (ResultCode.SUCCEEDED(res)){

					aSubscription.setDaSession(this);	
					aSubscription.setHandle(groupHandle.getValue());	
					subscriptionList_.put(aSubscription.getHashCode(),aSubscription);
				}	
				else{
					logger.error("AddDaSubscription() - Adding subscription to the session failed. Result: " + res);
					/*
					Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.AddDaSubscription", 
					"Adding subscription to the session failed! Result: " + res);
					*/
				}
			}
			else{
				res = EnumResultCode.E_INVALIDARG.toInt();
				logger.error("AddDaSubscription() - The subscription argument is invalid");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.AddDaSubscription", 
					" The subscription argument is invalid");
				*/
			}
		}
		catch(Exception exc){
			logger.error("AddDaSubscription() - " + exc.getMessage(), exc);
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSession.AddDaSubscription",
				exc.getMessage());
			*/
			
		}
		return res;
	}


	public int removeDaSubscription(DaSubscription aSubscription){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			if(aSubscription != null){
				res = OTClient.INSTANCE.OTCRemoveDASubscription(aSubscription.getHandle()).intValue();
	
				if(ResultCode.SUCCEEDED(res)){
					subscriptionList_.remove(aSubscription.getHashCode());
				}		
				else{
					logger.error("RemoveDaSubscription() - Removing subscription from the session failed. Result: " + res);
					/*
					Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.RemoveDaSubscription", 
					" Removing subscription from the session failed! Result: " + res);
					*/
				}
			}
			else{
				res = EnumResultCode.E_INVALIDARG.toInt();
				logger.error("RemoveDaSubscription() - The argument is invalid");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSession.RemoveDaSubscription", 
					"The argument is invalid");
				*/
			}
		}
		catch(Exception exc){
			logger.error("RemoveDaSubscription() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.RemoveDaSubscription", 
				exc.getMessage());
			*/			
		}
		return res;
	}


	public int read(int maxAge, String[] itemIDs, String[] itemPaths, ArrayList<ValueQT> values, ArrayList<Integer> results, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		
		if(values == null || results == null)
		{
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("Read() - Null Reference parameters passed to the method");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSession.Read", 
				" Null Reference parameters passed to the method");
			*/	
			return res;
		}
		
		values.clear();
		results.clear();
		
		try{
			JNIOTCExecutionOptions options = new JNIOTCExecutionOptions();

			if(executionOptions != null){
				options.m_executionType = (byte)executionOptions.getExecutionType().getInt();
				options.m_executionContext = executionOptions.getExecutionContext();
			}
			else{
				options.m_executionType = (byte)EnumExecutionType.SYNCHRONOUS.getInt();
				options.m_executionContext = 0;
			}
			
			res = (int)JNIOTClient.INSTANCE.OTCRead(
				this.getHandle().longValue(),
				(long)itemIDs.length,
				null,
				itemIDs,
				itemPaths,
				(long)maxAge,
				values,
				results,
				options);

			if(options.m_executionType == (byte)EnumExecutionType.SYNCHRONOUS.getInt()){
				if (ResultCode.SUCCEEDED(res)){
					//NOTHING
				}
				else{
					logger.error("Read() - Reading items using session failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.Read", 
						" Reading items using session failed! Result: " + res);
					*/
				}
			}// end if execution options
		}
		catch(Exception exc){
			logger.error("Read() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.Read", 
				exc.getMessage());
			*/			
		}
		return res;	
	}


	public int write(String[] itemIDs, String[] itemPaths, ValueQT[] values, ArrayList<Integer> results, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();

		if (values == null || results == null)
		{
			String strMessage = "";
			if(values == null)
			{
				strMessage = "Values cannot be null";
			}
			else if(values == null)
			{
				strMessage = "Parameter Results cannot be null";
			}
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.Write", strMessage
				);
			*/
			logger.error("Write() - " + strMessage);
			
			return res;
		}	//	end if
		
		results.clear();
		
		try{
			JNIOTCExecutionOptions options = new JNIOTCExecutionOptions();

			if(executionOptions != null){
				options.m_executionType = (byte)executionOptions.getExecutionType().getInt();
				options.m_executionContext = executionOptions.getExecutionContext();
			}
			else{
				options.m_executionType = (byte)EnumExecutionType.SYNCHRONOUS.getInt();
				options.m_executionContext = 0;
			}

			ArrayList<ValueQT> jnivalues = new ArrayList<ValueQT>();
			for(int i = 0; i < itemIDs.length; i++)
			{	
				jnivalues.add(values[i]);
			}
			
			res = (int)JNIOTClient.INSTANCE.OTCWrite(
					this.getHandle().longValue(),
					(long)itemIDs.length,
					null,
					itemIDs,
					itemPaths,
					jnivalues,
					results,
					options);
			
			if (ResultCode.FAILED(res)){
				logger.error("Write() - Writing items using subscription failed. Result: " + res);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Write", 
					"Writing items using subscription failed! Result: " + res);
				*/					
			}
		}
		catch(Exception exc){
			logger.error("Write() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.Write", 
				exc.getMessage());
			*/				
		}
		return res;
	}


	public int browse(String rootID, DaAddressSpaceElementBrowseOptions browseOptions, ArrayList<DaAddressSpaceElement> addressSpaceElements, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		
		if(addressSpaceElements == null)
		{
			logger.error("Browse() - Parameter addressSpaceElements IS NULL" );
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.Browse", "Parameter addressSpaceElements cannot be null"
				);
			*/
			return res;
		}
		
		addressSpaceElements.clear();
		OTCAddressSpaceBrowseOptions OTCBrowseOptions = new OTCAddressSpaceBrowseOptions();
		OTCBrowseOptions.m_accessRightsFilter = new NativeLong(0);

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

			if (browseOptions != null){
				OTCBrowseOptions.m_type = (byte)browseOptions.getElementTypeFilter().getInt();
				
				if(browseOptions.getElementNameFilter() != null)
				{
					/*ByteByReference bbrElementNameFilter = new ByteByReference();
					Pointer pElementNameFilter = OTClient.INSTANCE.OTSysAllocString(new WString(browseOptions.getElementNameFilter()));
					bbrElementNameFilter.setPointer(pElementNameFilter);
					OTCBrowseOptions.m_elementNameFilter = bbrElementNameFilter;
					*/
					Pointer pElementNameFilter = OTClient.INSTANCE.OTSysAllocString(new WString(browseOptions.getElementNameFilter()));
					OTCBrowseOptions.m_elementNameFilter = pElementNameFilter;
				}
				
				if(browseOptions.getVendorFilter() != null)
				{
					/*ByteByReference bbrVendorFilter = new ByteByReference();
					Pointer pVendorFilter = OTClient.INSTANCE.OTSysAllocString(new WString(browseOptions.getVendorFilter()));
					bbrVendorFilter.setPointer(pVendorFilter);
					OTCBrowseOptions.m_vendorFilter = bbrVendorFilter;
					*/
					Pointer pVendorFilter = OTClient.INSTANCE.OTSysAllocString(new WString(browseOptions.getVendorFilter()));
					OTCBrowseOptions.m_vendorFilter = pVendorFilter;
				}
				OTCBrowseOptions.m_accessRightsFilter = new NativeLong(browseOptions.getAccessRightsFilter().toShort());
				OTCBrowseOptions.m_datatypeFilter = VARIANT.getType(browseOptions.getDataTypeFilter());
				OTCBrowseOptions.m_maxElements = new NativeLong(browseOptions.getMaxElements());
				OTCBrowseOptions.m_retrieveItemID = (byte)(browseOptions.getIsRetrievingItemID()? 1:0);
				OTCBrowseOptions.m_retrieveProperties = (byte)(browseOptions.getIsReturningProperties()? 1:0);
				OTCBrowseOptions.m_retrievePropertyValues = (byte)(browseOptions.getIsReturningPropertyValues()? 1:0);
			}

			PointerByReference pOTCaddressSpaceElements = new PointerByReference();
			NativeLongByReference addressSpaceElementDataCount = new NativeLongByReference(new NativeLong(0));
			
			Pointer pTemp = pOTCaddressSpaceElements.getValue();
			
			res = OTClient.INSTANCE.OTCBrowseAddressSpace(
				this.getHandle(),
				new NativeLong(0),
				new WString(rootID),
				(WString)null,
				OTCBrowseOptions,
				addressSpaceElementDataCount,
				pOTCaddressSpaceElements,
				options).intValue();
			
			//addressSpaceElements = new DaAddressSpaceElement[addressSpaceElementDataCount.getValue().intValue()];
			
			if (options.m_executionType == (byte)EnumExecutionType.SYNCHRONOUS.getInt()){
				if (ResultCode.SUCCEEDED(res)){

					for (int i = 0; i < addressSpaceElementDataCount.getValue().intValue(); i++)
					{
						if(pOTCaddressSpaceElements.getValue().equals(pTemp))
						{
							// Nothing; the returned pointer is not valid
						}
						else
						{
							OTCAddressSpaceElementData myData = new OTCAddressSpaceElementData(pOTCaddressSpaceElements.getValue().share(i* new OTCAddressSpaceElementData().size()));
							
							addressSpaceElements.add(new DaAddressSpaceElement(
								myData.m_name.getString(0, true),
								myData.m_itemID.getString(0, true),
								myData.m_itemPath.getString(0, true),
								myData.m_objectHandle,
								this));
							if (myData.m_itemID != null && myData.m_itemID!= null)
							{
								OTClient.INSTANCE.OTFreeMemory(myData.m_itemID);
							}
							if (myData.m_name != null && myData.m_name != null)
							{
								OTClient.INSTANCE.OTFreeMemory(myData.m_name);
							}
							if (myData.m_itemPath != null && myData.m_itemPath!=null)
							{
								OTClient.INSTANCE.OTFreeMemory(myData.m_itemPath);
							}
						}
					}
					if (pOTCaddressSpaceElements != null && pOTCaddressSpaceElements.getValue() != null)
					{
						OTClient.INSTANCE.OTFreeMemory(pOTCaddressSpaceElements.getValue());
					}
				}
				else{
					logger.error("Browse() - Browsing server's address space failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.Browse",
						"Browsing server's address space failed! Result: " + res);
					*/	
				}
			}		
			if(OTCBrowseOptions.m_elementNameFilter != null)
			{
				//OTClient.INSTANCE.OTSysFreeString(OTCBrowseOptions.m_elementNameFilter.getPointer());
				OTClient.INSTANCE.OTSysFreeString(OTCBrowseOptions.m_elementNameFilter); //unicode
			}
			if(OTCBrowseOptions.m_vendorFilter != null)
			{
				//OTClient.INSTANCE.OTSysFreeString(OTCBrowseOptions.m_vendorFilter.getPointer());
				OTClient.INSTANCE.OTSysFreeString(OTCBrowseOptions.m_vendorFilter); //UNICODE
			}
		}	
		catch(Exception exc){
			logger.error("Browse() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.Browse", 
				exc.getMessage());
			*/			
		}
		return res;
	}


	public int getDaProperties(String anAddressSpaceElementId, String anAddressSpaceElementPath, DaGetPropertiesOptions aGetPropertyOptions, ArrayList<DaProperty> someDaProperties, ExecutionOptions executionOptions){
		return getDaProperties(null, anAddressSpaceElementId, anAddressSpaceElementPath, aGetPropertyOptions, someDaProperties, executionOptions);
	}


	protected int getDaProperties(DaAddressSpaceElement anAddressSpaceElement, String anAddressSpaceElementId, String anAddressSpaceElementPath, DaGetPropertiesOptions aGetPropertyOptions, ArrayList<DaProperty> someDaProperties, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();

		try{
			//	Setup the Execution Options:
			OTCExecutionOptions options = new OTCExecutionOptions();
			NativeLong aeHandle = null;

			if (anAddressSpaceElement != null)
			{
				aeHandle = anAddressSpaceElement.getHandle();
			}

			if(executionOptions != null)
			{
				options.m_executionType = (byte)(executionOptions.getExecutionType().getInt());
				options.m_executionContext = new NativeLong(executionOptions.getExecutionContext());
			}
			else{
				options.m_executionType = (byte)EnumExecutionType.SYNCHRONOUS.getInt();
				options.m_executionContext = new NativeLong(0);
			}
		
			NativeLongByReference propertiesDataCount = new NativeLongByReference(new NativeLong(0));

			OTCGetDAPropertiesOptions otcPropertyOptions = new OTCGetDAPropertiesOptions();
			
			if (aGetPropertyOptions != null){
				if (aGetPropertyOptions.getPropertyIds() != null){
					
					Pointer p = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(aGetPropertyOptions.getPropertyIds().length * NativeLong.SIZE));
					for(int i=0; i < aGetPropertyOptions.getPropertyIds().length; i++)
					{
						NativeLongByReference nlbri = new NativeLongByReference();
						nlbri.setPointer(p.share(i*NativeLong.SIZE));
						nlbri.setValue(new NativeLong(aGetPropertyOptions.getPropertyIds()[i]));
					}
					NativeLongByReference nlbr = new NativeLongByReference();
					nlbr.setPointer(p);
					otcPropertyOptions.m_pPropertyIDs = nlbr;
				}	//	end if
		
				if (aGetPropertyOptions.getPropertyNames() != null){
					Pointer p = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(aGetPropertyOptions.getPropertyNames().length * Pointer.SIZE));
					PointerByReference pbr = new PointerByReference();
					pbr.setValue(p);
					otcPropertyOptions.m_pPropertyNames = pbr; 
					
					for (int index = 0; index < aGetPropertyOptions.getPropertyNames().length; index++){
						Pointer pi = p.share(index*Pointer.SIZE);
						Pointer pstri = OTClient.INSTANCE.OTSysAllocString(new WString(aGetPropertyOptions.getPropertyNames()[index]));
						pi.setPointer(0, pstri);
					}	//	end for
				}	//	end if
				
				if(aGetPropertyOptions.getPropertyIds() != null){
					otcPropertyOptions.m_propertyCount = new NativeLong(aGetPropertyOptions.getPropertyIds().length);
				}
				else{
					if(aGetPropertyOptions.getPropertyNames() != null){
						otcPropertyOptions.m_propertyCount = new NativeLong(aGetPropertyOptions.getPropertyNames().length);
					}
				}				
				otcPropertyOptions.m_whatPropertyData = (byte)aGetPropertyOptions.getWhatPropertyData().getInt();	
				otcPropertyOptions.m_getLocalFromAddressSpaceElement = (byte)0;
			
			}
			else{
				otcPropertyOptions.m_getLocalFromAddressSpaceElement = (byte)1;
			}

			PointerByReference pPropertyData = new PointerByReference();
			
			Pointer pTemp = pPropertyData.getValue();
			
			res = OTClient.INSTANCE.OTCGetDAProperties(
				this.getHandle(),
				aeHandle,
				new WString(anAddressSpaceElementId), 
				new WString(anAddressSpaceElementPath),
				otcPropertyOptions,
				propertiesDataCount, 
				pPropertyData,
				options).intValue();

			if (ResultCode.SUCCEEDED(res))
			{
				if (options.m_executionType == (byte)EnumExecutionType.SYNCHRONOUS.getInt())
				{
					if (propertiesDataCount.getValue().longValue() > 0)
					{

						//someDaProperties = new DaProperty[propertiesDataCount.getValue().intValue()];
						OTCDAPropertyData fake = new OTCDAPropertyData();
						
						for (int index = 0; index < propertiesDataCount.getValue().intValue(); index++)
						{
							if(pPropertyData.getValue().equals(pTemp))
							{
								//NO property returned
								//someDaProperties.add( new DaProperty());
							}
							else
							{
								OTCDAPropertyData myData = new OTCDAPropertyData(pPropertyData.getValue().share(index*fake.size())); //44 = sizeof OTCDAPropertyData in C
								someDaProperties.add( new DaProperty(
									myData.m_ID.intValue(),
									myData.m_name != null ? myData.m_name.getString(0, true):"",
									myData.m_itemID!= null ? myData.m_itemID.getString(0, true):"",
									myData.m_itemPath != null ? myData.m_itemPath.getString(0, true):"",
									myData.m_description != null ? myData.m_description.getString(0, true):"",
									VARIANT.getSysType(myData.m_datatype),
									new ValueQT(myData.m_value),
									myData.m_result.intValue()));
								
								if (myData.m_name != null )
								{
									OTClient.INSTANCE.OTFreeMemory(myData.m_name);
								}
								if (myData.m_itemID != null)
								{
									OTClient.INSTANCE.OTFreeMemory(myData.m_itemID);
								}
								if (myData.m_itemPath != null)
								{
									OTClient.INSTANCE.OTFreeMemory(myData.m_itemPath);
								}
								if (myData.m_description != null )
								{
									OTClient.INSTANCE.OTFreeMemory(myData.m_description);
								}
								if (myData.m_value.m_pValue != null)
								{
									OTClient.INSTANCE.OTVariantClear(myData.m_value.m_pValue);
									OTClient.INSTANCE.OTFreeMemory(myData.m_value.m_pValue.getPointer());
								}
							}
						}	//	end for			
						if (pPropertyData != null)
						{
							OTClient.INSTANCE.OTFreeMemory(pPropertyData.getValue());
						}
					}	//	end if
				}	//	end if
			}
			else{
				logger.error("GetDaProperties() - Getting properties failed. Result: " + res);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.GetDaProperties",
					"Getting properties failed! Result: " + res);
				*/	
			}	//	end if...else
		}
		catch(Exception exc){
			logger.error("GetDaProperties() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.GetDaProperties", 
				exc.getMessage());
			*/
			someDaProperties.clear();
		}
		return res;
	}


	public int getStatus(ServerStatus serverStatus, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			OTCExecutionOptions options = new OTCExecutionOptions();

			if(executionOptions != null){
				options.m_executionType = (byte)executionOptions.getExecutionType().getInt();
				options.m_executionContext = new NativeLong(executionOptions.getExecutionContext());
			}
			else{
				options.m_executionType = (byte)EnumExecutionType.SYNCHRONOUS.getInt();
				options.m_executionContext = new NativeLong (0);
			}				

			OTCServerStatus.ByReference currentStatus = new OTCServerStatus.ByReference();				
			res =  OTClient.INSTANCE.OTCGetServerStatus(
						this.getHandle(),
						currentStatus,
						options).intValue();
		
			if(options.m_executionType == (byte)EnumExecutionType.SYNCHRONOUS.getInt()){
				if (ResultCode.SUCCEEDED(res))
				{

					int state = currentStatus.m_state.intValue();
					Date startTime = currentStatus.m_startTime.toDate();
					Date lastUpdateTime = currentStatus.m_lastUpdateTime.toDate();
					Date currentTime = currentStatus.m_currentTime.toDate();

					long[] supportedLCIDs = new long[currentStatus.m_countLCIDs.intValue()];
					for(int i = 0; i < supportedLCIDs.length; i++)
					{
						Pointer p = currentStatus.m_supportedLCIDs.getPointer().share(i* NativeLong.SIZE);
						NativeLongByReference nlbr = new NativeLongByReference();
						nlbr.setPointer(p);
						supportedLCIDs[i] = nlbr.getValue().longValue();
					}
					String[] LCIDs = new String[currentStatus.m_countLCIDs.intValue()];
					
					for (int i = 0; i < currentStatus.m_countLCIDs.longValue(); i++)
					{
						//System.out.println("   Supported LCID: " + supportedLCIDs[i]);
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
							if(lc != null)
							{
								LCIDs[i] = lc.toString();
							}
							else
							{
								LCIDs[i] = "Unknown LCID";
							}
						}
					}
					
					serverStatus.setState(state);
					serverStatus.setStartTime(startTime);
					serverStatus.setCurrentTime(currentTime);
					serverStatus.setVendorInfo(new String(currentStatus.m_vendorInfo.getString(0, true)));
					serverStatus.setProductVersion(new String(currentStatus.m_productVersion.getString(0, true)));
					serverStatus.setSupportedLcIds(LCIDs);
					serverStatus.setLastUpdateTime(lastUpdateTime);
					serverStatus.setGroupCount(currentStatus.m_groupCount.intValue());
					serverStatus.setBandwidth(currentStatus.m_bandwidth.intValue());
					serverStatus.setStatusInfo(new String(currentStatus.m_statusInfo.getString(0, true)));
					
					/*serverStatus = new ServerStatus(
						state,
						startTime,
						currentTime,
						currentStatus.m_vendorInfo.getPointer().getString(0, true),
						currentStatus.m_productVersion.getPointer().getString(0, true),
						LCIDs,
						lastUpdateTime,
						currentStatus.m_groupCount.intValue(),
						currentStatus.m_bandwidth.intValue(),
						currentStatus.m_statusInfo.getPointer().getString(0, true));*/
				}
				else
				{
					logger.error("GetStatus() - Getting status failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.GetStatus",
						"Getting status failed! Result: " + res);
					*/
				}
				
				OTClient.INSTANCE.OTFreeMemory(currentStatus.m_vendorInfo);
				
				OTClient.INSTANCE.OTFreeMemory(currentStatus.m_statusInfo);
				
				OTClient.INSTANCE.OTFreeMemory(currentStatus.m_productVersion);
				
				OTClient.INSTANCE.OTFreeMemory(currentStatus.m_supportedLCIDs.getPointer());
				
			}
		}
		catch(Exception exc){
			logger.error("GetStatus() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.GetStatus", 
				exc.getMessage());
			*/				
		}
		return res;
	}


	public int activateConnectionMonitor(boolean activate, int checkCycle, int connect1Attempts, int connect1Cycle, int connect2Cycle){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			byte active = (byte)(activate ? 1 : 0);	
			res = OTClient.INSTANCE.OTCActivateConnectionMonitor(
				this.getHandle(),
				active,
				new NativeLong(checkCycle),
				new NativeLong(connect1Attempts),
				new NativeLong(connect1Cycle),
				new NativeLong(connect2Cycle)).intValue();

			if (ResultCode.FAILED(res))
			{
				logger.error("ActivateConnectionMonitor() - Activating connection monitor failed. Result: " + res);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.ActivateConnectionMonitor",
					"Activating connection monitor failed! Result: " + res);
				*/
			}
		}
		catch(Exception exc){
			logger.error("ActivateConnectionMonitor() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSession.ActivateConnectionMonitor", 
				exc.getMessage());
			*/			
		}
		return res;
	}


	public int getErrorString(int errorCode, StringBuffer errorText, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		errorText = new StringBuffer();
		PointerByReference pErrorString = new PointerByReference();
		
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

			res = OTClient.INSTANCE.OTCGetErrorString(
						this.getHandle(),
						new NativeLong(errorCode),
						pErrorString,
						options).intValue();

			if (ResultCode.SUCCEEDED(res))
			{
				errorText.append(pErrorString.getPointer().getPointer(0).getString(0, true));
				//errorText = Marshal.PtrToStringUni(errorString);
			}
			else
			{
				logger.error("GetErrorString() - Getting error string failed. Result: " + res);
				/*
				Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.GetErrorString",
						"Getting error string failed! Result: " + res);
				*/
			}
			OTClient.INSTANCE.OTFreeMemory(pErrorString.getPointer().getPointer(0));
		}
		catch(Exception exc){
			logger.error("GetErrorString() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSession.GetErrorString", 
				exc.getMessage());
			*/			
		}
		return res;
	}


	public int logon(String userName, String password, ExecutionOptions executionOptions){
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
			res = OTClient.INSTANCE.OTCLogon(
				this.getHandle(),
				new WString(userName),
				new WString(password),
				options).intValue();

			if (ResultCode.FAILED(res))
			{
				logger.error("Logon() - Logging on failed. Result: " + res);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Logon",
					"Logging on failed! Result: " + res);
				*/
			}
		}
		catch(Exception exc){
			logger.error("Logon() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSession.Logon", 
				exc.getMessage());
			*/				
		}
		return res;
	}


	public int logoff(ExecutionOptions executionOptions){
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

			res = OTClient.INSTANCE.OTCLogoff(
				this.getHandle(),
				options).intValue();

			if (ResultCode.FAILED(res))
			{
				logger.error("Logoff() - Logging off failed. Result: " + res);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Logoff",
					"Logging off failed! Result: " + res);
				*/
			}
		}
		catch(Exception exc){
			logger.error("Logoff() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSession.Logoff", 
				exc.getMessage());
			*/					
		}
		return res;
	}


	protected DaSubscription find(int position){
		return (DaSubscription)subscriptionList_.get(position);
	}


	protected void onReadComplete(int executionContext, String[] itemIDs, String[] itemPaths, ValueQT[] values, int[] results, int result){
		//override it
	}


	protected void onWriteComplete(int executionContext, String[] itemIDs, String[] itemPaths, ValueQT[] values, int[] results, int result){
		//override it
	}


	protected boolean onShutdown(String reason){
		//override it
		return false;
	}


	protected void onGetServerStatus(int executionContext, ServerStatus serverStatus, int result){
		//override it
	}


	protected void onBrowse(int executionContext, NativeLong addressSpaceElementHandle, String addressSpaceElementID, String addressSpaceElementPath, DaAddressSpaceElement[] addressSpaceElement, int result){
		//override it
	}


	protected void onGetDaProperties(int executionContext, NativeLong addressSpaceElementHandle, String addressSpaceElementID, String addressSpaceElementPath, DaProperty[] aDaProperty, int result){
		//override it
	}


	protected void onGetErrorString(int executionContext, int errorCode, String errorString, int result){
		//override it
	}


	protected void onLogon(int executionContext, String userName, String password, int result){
		//override it
	}


	protected void onLogoff(int executionContext, int result){
		//override it
	}


	
}