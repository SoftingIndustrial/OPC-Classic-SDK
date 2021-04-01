package com.softing.opc.client.api;


import java.util.ArrayList;
import java.io.Serializable;
import java.util.Hashtable;

import com.softing.opc.client.enums.EnumExecutionType;
import com.softing.opc.client.enums.EnumObjectAttribute;
import com.softing.opc.client.enums.EnumResultCode;
import com.softing.opc.client.enums.EnumTraceGroup;
import com.softing.opc.client.enums.EnumTraceLevel;
import com.softing.opc.client.enums.EnumUpdateAttributesBehavior;
import com.softing.opc.client.enums.ResultCode;
import com.softing.opc.client.impl.NameDecoder;
import com.softing.opc.client.impl.jna.FILETIME;
import com.softing.opc.client.impl.jna.OTCExecutionOptions;
import com.softing.opc.client.impl.jna.OTCObjectAttributes;
import com.softing.opc.client.impl.jna.OTClient;
import com.softing.opc.client.impl.jna.OTValueData;
import com.softing.opc.client.impl.jna.VARIANT;

import com.softing.opc.client.impl.jni.*;

import com.sun.jna.*;
import com.sun.jna.ptr.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class DaSubscription extends ObjectSpaceElement implements Serializable {

	final Logger logger = LoggerFactory.getLogger(DaSubscription.class);
	private static final long serialVersionUID = 7526471155622776147L;
	private final OTCObjectAttributes objectAttributes_ = new OTCObjectAttributes();
	private final Object attributesJanitor_ = new Object();
	
	protected Hashtable itemList_ = new Hashtable();
	protected DaSession session_ = null;


	public DaSubscription(){

	}

	public DaSubscription(int updateRate, DaSession parentSession){
		itemList_ = new Hashtable();
		
		//sets the parentSession of the newly created subscription
		session_ = parentSession;

		try{
			int res = EnumResultCode.E_FAIL.toInt();		

			res = parentSession.addDaSubscription(updateRate, this);

			if(ResultCode.FAILED(res)){							
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.Constructor", 
					"The subscription could not be added to the session! Result: " + res);
				*/
				logger.error("Constructor - The subscription could not be added to the session! Result: " + res);
			}	
		}
		catch(Exception exc){
			logger.error("Constructor - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.Constructor",
				exc.getMessage());
			*/				
		}	
	}

	/**
	 * Gets the DaSession object to which the subscription belongs.
	 */
	public DaSession getDaSession(){
		return session_;
	}

	/**
	 * Sets the DaSession object to which the subscription belongs.
	 */
	public void setDaSession(DaSession session){
		session_ = session;
	}

	/**
	 * Gets the list with the items belonging to the subscription.
	 */
	public DaItem[] getItemList(){
		synchronized(itemList_)
		{
			DaItem[] itemVector = new DaItem[itemList_.size()];
			int i = 0;
	
			for(Object daItem : itemList_.values())
			{
				itemVector[i] = (DaItem)daItem;
				i++;
			}
			return itemVector;
		}
	}


	public int getRequestedUpdateRate(){
		int updateRate = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE.getLong()));
						
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
							logger.error("getRequestedUpdateRate() - Update attribute DASUBSCRIPTION_UPDATERATE from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSUbscription.RequestedUpdateRate.Get", 
								"Update attribute DASUBSCRIPTION_UPDATERATE from server failed!Result: " + updateResult);
							*/
						}	//	end if
					}	//	end if

					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE.getLong()),
											objectAttributes).intValue();

					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						updateRate = objectAttributes.m_daSubscriptionRequestedUpdateRate.intValue();

					}// end if
					else{
						logger.error("getRequestedUpdateRate() - Get attribute DASUBSCRIPTION_UPDATERATE from server failed. Result: " + getAttrsResult);
						/*															
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSUbscription.RequestedUpdateRate.Get", 
							"Get attribute DASUBSCRIPTION_UPDATERATE from server failed!Result: " + getAttrsResult);
						*/
					} //end else								
				}// end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getRequestedUpdateRate() - " + exc.getMessage());
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.RequestedUpdateRate.Get", 
				exc.getMessage());
			*/
		}
		return updateRate;
	}


	public void setRequestedUpdateRate(int value){
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){
					objectAttributes_.m_daSubscriptionRequestedUpdateRate = new NativeLong(value);
					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE.getLong()),
											objectAttributes_).intValue();	
					if(ResultCode.FAILED(setAttrsResult)){
						logger.error("setRequestedUpdateRate() - Set attribute DASUBSCRIPTION_UPDATERATE failed. Result: " + setAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.RequestedUpdateRate.Set",
							"Set attribute DASUBSCRIPTION_UPDATERATE failed! Result: " + setAttrsResult );
						*/	
					} // end if
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE.getLong()));
						
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
							logger.error("setRequestedUpdateRate() - Set attribute DASUBSCRIPTION_UPDATERATE failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.RequestedUpdateRate.Set",
								"Set attribute DASUBSCRIPTION_UPDATERATE failed! Result: " + updateResult );
							*/	
						}
					}// end if
				}// end lock
			}
			else{
				logger.error("setRequestedUpdateRate() - Set RequestedUpdateRate failed. Subscription is not added to a session");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSubscription.RequestedUpdateRate.Set",
					"Set RequestedUpdateRate failed! Subscription is not added to a session");
				*/
			}// end if...else
		}
		catch(Exception exc){
			logger.error("setRequestedUpdateRate() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.RequestedUpdateRate.Set", 
				exc.getMessage());
			*/
		}
	}


	public int getRevisedUpdateRate(){
		int revisedUpdateRate = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				int result = -1;
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE.getLong()));
						
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
							logger.error("getRevisedUpdateRate() - Update attribute DASUBSCRIPTION_UPDATERATE from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.RevisedUpdateRate.Get", 
								"Update attribute DASUBSCRIPTION_UPDATERATE from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE.getLong()),
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						revisedUpdateRate = objectAttributes.m_daSubscriptionRevisedUpdateRate.intValue();
					}// end if
					else{
						logger.error("getRevisedUpdateRate() - Get attribute DASUBSCRIPTION_UPDATERATE from server failed. Result: " + getAttrsResult);
						/*																
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.RevisedUpdateRate.Get", 
							"Get attribute DASUBSCRIPTION_UPDATERATE from server failed!Result: " + getAttrsResult);
						*/
						
					} //end else	
				}// end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getRevisedUpdateRate() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.RevisedUpdateRate.Get", 
				exc.getMessage());
			*/
		}
		return revisedUpdateRate;
	}


	public int getRequestedKeepAliveTime(){
		int requestedKeepAliveTime = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				int result = -1;
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME.getLong()));
						
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
							logger.error("getRequestedKeepAliveTime() - Update attribute DASUBSCRIPTION_KEEPALIVETIME from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.RequestedKeepAliveTime.Get", 
								"Update attribute DASUBSCRIPTION_KEEPALIVETIME from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME.getLong()), 
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						requestedKeepAliveTime = objectAttributes.m_daSubscriptionRequestedKeepAliveTime.intValue();
					}// end if
					else{
						logger.error("getRequestedKeepAliveTime() - Get attribute DASUBSCRIPTION_KEEPALIVETIME from server failed. Result: " + getAttrsResult);
						/*																
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.RequestedKeepAliveTime.Get", 
							"Get attribute DASUBSCRIPTION_KEEPALIVETIME from server failed!Result: " + getAttrsResult);
						*/
					} //end else	
				}// end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getRequestedKeepAliveTime() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.RequestedKeepAliveTime.Get", 
				exc.getMessage());
			*/
		}
		return requestedKeepAliveTime;
	}

	/**
	 * Sets the requested maximum period between updates sent to the client. Clients can set 
	 * the keep-alive time for a subscription to cause the server to provide client callbacks 
	 * on the subscription when there are no new events to report. Clients can then be assured 
	 * of the health of the server and subscription without resorting to pinging the server 
	 * with calls to GetStatus. Using this facility, a client can expect a callback (data or 
	 * keep-alive) within the specified keep-alive.
	 */
	public void setRequestedKeepAliveTime(int value){
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){
					objectAttributes_.m_daSubscriptionRequestedKeepAliveTime = new NativeLong(value);
					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME.getLong()),
											objectAttributes_).intValue();
					if(ResultCode.FAILED(setAttrsResult)){	
						logger.error("setRequestedKeepAliveTime() - Set attribute DASUBSCRIPTION_KEEPALIVETIME failed. Result: " + setAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSession.LocaleId.Set",
							"Set attribute DASUBSCRIPTION_KEEPALIVETIME failed! Result: " + setAttrsResult );
						*/	
					} // end if
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME.getLong()));
						
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
							logger.error("setRequestedKeepAliveTime() - Set attribute DASUBSCRIPTION_KEEPALIVETIME failed Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.RequestedKeepAliveTime.Get",
								"Set attribute DASUBSCRIPTION_KEEPALIVETIME failed! Result: " + updateResult );
							*/	
						}
					}
				}// end lock
			}
			else{
				logger.error("setRequestedKeepAliveTime() - Set RequestedKEepAliveTime failed. Subscription is not added to a session ");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSubscription.RequestedKeepAliveTime.Get",
					"Set RequestedKEepAliveTime failed! Subscription is not added to a session" );
				*/	
			}	//	end if...else
		}
		catch(Exception exc){
			logger.error("setRequestedKeepAliveTime() - " + exc.getMessage(), exc) ;
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.RequestedKeepAliveTime.Set", 
				exc.getMessage());
			*/
		}
	}


	public int getRevisedKeepAliveTime(){
		int revisedKeepAliveTime = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				int result = -1;
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME.getLong()));
						
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
							logger.error("getRevisedKeepAliveTime() - Update attribute DASUBSCRIPTION_KEEPALIVETIME from server failed. Result: " + updateResult) ;
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.RevisedKeepAliveTime.Get", 
								"Update attribute DASUBSCRIPTION_KEEPALIVETIME from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME.getLong()), 
						objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						revisedKeepAliveTime = objectAttributes.m_daSubscriptionRevisedKeepAliveTime.intValue();
					}
					else{
						logger.error("getRevisedKeepAliveTime() - Get attribute DASUBSCRIPTION_KEEPALIVETIME from server failed. Result: " + getAttrsResult) ;
						/*																
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.RevisedKeepAliveTime.Get", 
							"Get attribute DASUBSCRIPTION_KEEPALIVETIME from server failed!Result: " + getAttrsResult);
						*/
					} //end else	
				}// end lock
			}//end if
		}
		catch(Exception exc){
			logger.error("getRevisedKeepAliveTime() - " + exc.getMessage(), exc) ;
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.RevisedKeepAliveTime.Get", 
				exc.getMessage());
			*/
		}
		return revisedKeepAliveTime;
	}

	/**
	 * Gets a symbolic name for the subscription object that describes its
	 * semantic significance (e.g "Temperatures_on_Floor_1")
	 */
	public String getName(){
		String subscriptionName = "";
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				int result = -1;
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_NAME.getLong()));
						
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
							logger.error("getName() - Update attribute DASUBSCRIPTION_NAME from server failed. Result: " + updateResult) ;
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.Name.Get", 
								"Update attribute DASUBSCRIPTION_NAME from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_NAME.getLong()), 
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{	
						//subscriptionName = Marshal.PtrToStringUni(objectAttributes.m_daSubscriptionName);
						subscriptionName = objectAttributes.m_daSubscriptionName.getString(0, true);
						OTClient.INSTANCE.OTFreeMemory(objectAttributes.m_daSubscriptionName);
					}		
					else{	
						logger.error("getName() - Get attribute DASUBSCRIPTION_NAME from server failed. Result: " + getAttrsResult) ;
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.Name.Get", 
							"Get attribute DASUBSCRIPTION_NAME from server failed!Result: " + getAttrsResult);
						*/
					} //end else	
				}// end lock
			}
			else{
			}	//	end if...else
		}
		catch(Exception exc){
			logger.error("getName() - " + exc.getMessage(), exc) ;
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.Name.Get", 
				exc.getMessage());
			*/
		}	//	end try...catch
		return subscriptionName;
	}


	public void setName(String value){
		try{
			if(this.getHandle().longValue() != 0){
				if (!value.equals("")){
					OTCExecutionOptions options = new OTCExecutionOptions() ;
					int result = -1;
					synchronized(attributesJanitor_){	
						Pointer pStr = OTClient.INSTANCE.OTSysAllocString(new WString(value));
						//ByteByReference bbr = new ByteByReference();
						//bbr.setPointer(pStr);
						objectAttributes_.m_daSubscriptionName = pStr;
						
						//objectAttributes_.m_daSubscriptionName = Marshal.StringToCoTaskMemUni(value);
						int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
												this.getHandle(),
												new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_NAME.getLong()),
												objectAttributes_).intValue();
						if(ResultCode.FAILED(setAttrsResult)){
							logger.error("setName() - Set attribute DASUBSCRIPTION_NAME failed. Result: " + setAttrsResult) ;
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.Name.Set",
								"Set attribute DASUBSCRIPTION_NAME failed! Result: " + setAttrsResult );
							*/	
						} // end if
						if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
							
							//prepare what attributes
							NativeLongByReference whatAttributes = new NativeLongByReference();
							whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_NAME.getLong()));
							
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
								logger.error("setName() - Set attribute DASUBSCRIPTION_NAME failed. Result: " + updateResult) ;
								/*
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT, 
									"DaSubscription.Name.Set",
									"Set attribute DASUBSCRIPTION_NAME failed! Result: " + updateResult );
								*/	
							}	//	end if
						}	//	end if
						OTClient.INSTANCE.OTSysFreeString(pStr);
						//Marshal.FreeCoTaskMem(m_objectAttributes.m_daSubscriptionName);
					}//end lock
				}// end if	
			}// end if
			else{
				logger.error("setName() - Set Name failed. Subscription is not added to a session") ;
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSubscription.Name.Set",
					"Set Name failed! Subscription is not added to a session");
				*/
			}	//	end if...else

		}
		catch(Exception exc){
			//exc.printStackTrace();
			//Marshal.FreeCoTaskMem(m_objectAttributes.m_daSubscriptionName);
			OTClient.INSTANCE.OTSysFreeString(objectAttributes_.m_daSubscriptionName);
			Application.INSTANCE.trace(EnumTraceLevel.ERR,EnumTraceGroup.CLIENT,"DaSubscription.Name.Set", exc.getMessage());
		}
	}

	public float getDeadband(){
		float deadband = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				int result = -1;
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_DEADBAND.getLong()));
						
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
							logger.error("getDeadband() - Update attribute DASUBSCRIPTION_DEADBAND from server failed. Result: " + updateResult) ;
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.Deadband.Get", 
								"Update attribute DASUBSCRIPTION_DEADBAND from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_DEADBAND.getLong()), 
						objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{
						deadband = objectAttributes.m_daSubscriptionDeadband;
					}//end if
					else{
						logger.error("getDeadband() - Get attribute DASUBSCRIPTION_DEADBAND from server failed. Result: " + getAttrsResult) ;
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.Deadband.Get", 
							"Get attribute DASUBSCRIPTION_DEADBAND from server failed!Result: " + getAttrsResult);
						*/
					} //end else	
				}// end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getDeadband() - " + exc.getMessage(), exc) ;
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.Deadband.Get", 
				exc.getMessage());
			*/
		}
		return deadband;
	}


	public void setDeadband(float value){
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){	
					objectAttributes_.m_daSubscriptionDeadband = value;
					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_DEADBAND.getLong()),
						objectAttributes_).intValue();
					if(ResultCode.FAILED(setAttrsResult)){
						logger.error("setDeadband() - Set attribute DASUBSCRIPTION_DEADBAND failed. Result: " + setAttrsResult) ;
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.Deadband.Set",
							"Set attribute DASUBSCRIPTION_DEADBAND failed! Result: " + setAttrsResult );
						*/	
					} // end if
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_DEADBAND.getLong()));
						
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
							logger.error("setDeadband() - Set attribute DASUBSCRIPTION_DEADBAND failed. Result: " + updateResult) ;
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.Deadband.Set",
								"Set attribute DASUBSCRIPTION_DEADBAND failed! Result: " + updateResult );
							*/	
						}	//	end if
					}// end if
				}// end lock
			}
			else{
				logger.error("setDeadband() - Set Deadband failed. Subscription is not added to a session") ;
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSubscription.Deadband.Set",
					"Set Deadband failed! Subscription is not added to a session ");
				*/
			}// end if...else
		}
		catch(Exception exc){
			logger.error("setDeadband() - " + exc.getMessage(), exc) ;
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.Deadband.Set", 
				exc.getMessage());
			*/
		}
	}


	public int getTimeBias(){
		int timeBias = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				int result = -1;
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_TIMEBIAS.getLong()));
						
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
							logger.error("getTimeBias() - Update attribute DASUBSCRIPTION_TIMEBIAS from server failed. Result: " + updateResult) ;
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.TimeBias.Get", 
								"Update attribute DASUBSCRIPTION_TIMEBIAS from server failed!Result: " + updateResult);
							*/
						}	//	end if
					}	//	end lock
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_TIMEBIAS.getLong()), 
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						timeBias = objectAttributes.m_daSubscriptionTimeBias.intValue();							
					}// end if
					else{
						logger.error("getTimeBias() - Get attribute DASUBSCRIPTION_TIMEBIAS from server failed. Result: " + getAttrsResult) ;
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.TimeBias.Get",
							"Get attribute DASUBSCRIPTION_TIMEBIAS from server failed!Result: " + getAttrsResult);
						*/
						
					} //end else	
				}// end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getTimeBias() - " + exc.getMessage(), exc) ;
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.Timebias.Get", 
				exc.getMessage());
			*/
		}
		return timeBias;
	}


	public void setTimeBias(int value){
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){
					objectAttributes_.m_daSubscriptionTimeBias = new NativeLong(value);
					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_TIMEBIAS.getLong()),
											objectAttributes_).intValue();
					if(ResultCode.FAILED(setAttrsResult)){
						logger.error("setTimeBias() - Set attribute DASUBSCRIPTION_TIMEBIAS failed. Result: " + setAttrsResult) ;
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.TimeBias.Set",
							"Set attribute DASUBSCRIPTION_TIMEBIAS failed! Result: " + setAttrsResult );
						*/	
					} // end if
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_TIMEBIAS.getLong()));
						
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
							logger.error("setTimeBias() - Set attribute DASUBSCRIPTION_TIMEBIAS failed. Result: " + updateResult) ;
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.TimeBias.Set",
								"Set attribute DASUBSCRIPTION_TIMEBIAS failed! Result: " + updateResult );
							*/	
						}	//	end if
					}	//	end if
				}// end lock
			}// end if
			else{
				logger.error("setTimeBias() - Set TimeBias failed. Subscription is not added to a session") ;
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSubscription.TimeBias.Set",
					"Set TimeBias failed! Subscription is not added to a session ");
				*/
			}	//	end if...else
		}
		catch(Exception exc){
			logger.error("setTimeBias() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Da.Subscription.TimeBias.Set", 
				exc.getMessage());
			*/
		}	//	end try...catch
	}


	public String getLocaleId(){
		int localeID = 0;
		String cultureName = "";
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_LCID.getLong()));
						
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
							logger.error("getLocaleId() - Update attribute DASUBSCRIPTION_LCID from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.LocaleId.Get", 
								"Update attribute DASUBSCRIPTION_LCID from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_LCID.getLong()), 
						objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						localeID = objectAttributes.m_daSubscriptionLCID.intValue();
						switch(localeID){
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
								java.util.Locale lc = NameDecoder.getWindowsLocale((int)localeID);
								//culture = new CultureInfo((int)localeID);
								cultureName = lc.toString();		
								break;
						}														
					}// end if
					else{
						logger.error("getLocaleId() - Get attribute DASUBSCRIPTION_LCID from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.LocaleId.Get", 
							"Get attribute DASUBSCRIPTION_LCID from server failed!Result: " + getAttrsResult);
						*/
					} //end else	
				}// end lock
			}// end if
		}
		/*catch(ArgumentException exc){
			Application.Instance.Trace(
				EnumTraceLevel.INF,
				EnumTraceGroup.CLIENT, 
				"DaSubscription.LocaleId.Get", 
				exc.Message);
			cultureName = Integer.toString(localeID);
		}*/
		catch(Exception exc){
			logger.error("getLocaleId() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Da.Subscription.LocaleID.Get", 
				exc.getMessage());
			*/
		}
		return cultureName;
	}


	public void setLocaleId(String value){
		try{
			int lcid;
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

					objectAttributes_.m_daSubscriptionLCID = new NativeLong(lcid);

					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_LCID.getLong()),
											objectAttributes_).intValue();
					if(ResultCode.FAILED(setAttrsResult)){
						logger.error("setLocaleId() - Set attribute DASUBSCRIPTION_LCID failed. Result: " + setAttrsResult);
						/*
						Application.Instance.Trace(
							
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaSubscription.LocaleId.Set",
							"Set attribute DASUBSCRIPTION_LCID failed! Result: " + setAttrsResult );
						*/	
					} // end if
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DASUBSCRIPTION_LCID.getLong()));
						
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
							logger.error("setLocaleId() - Set attribute DASUBSCRIPTION_LCID failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaSubscription.LocaleId.Set",
								"Set attribute DASUBSCRIPTION_LCID failed! Result: " + updateResult );
							*/		
						}
					}// end if	
				}// end lock
			}
			else{
				logger.error("setLocaleId() - Set LocaleId failed. Subscription is not added to a session");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaSubscription.LocaleId.Set",
					"Set LocaleId failed! Subscription is not added to a session ");
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
				"DaSubscription.LocaleId.Set", 
				exc.getMessage());
			*/
		}
	}


	protected int addDaItem(String itemId, DaItem anItem){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			if(anItem != null){
				NativeLongByReference itemHandle = new NativeLongByReference();
				
				res = OTClient.INSTANCE.OTCAddDAItem(
						this.getHandle(),
						new WString(itemId),
						new NativeLong(anItem.getHashCode()),
						itemHandle).intValue();
				
				if (ResultCode.SUCCEEDED(res)){

					anItem.setDaSubscription(this);
					anItem.setHandle(itemHandle.getValue());
					//Hashtable synchronizedItemList = Hashtable.Synchronized(m_itemList);
					itemList_.put(anItem.getHashCode(), anItem);				
				}
				else{
					logger.error("AddDaItem() - Adding item failed! Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.AddDaItem",
						" Adding item failed! Result: " + res);
					*/
				}
			}
			else{
				res = EnumResultCode.E_INVALIDARG.toInt();
				logger.error("AddDaItem() - The item argument is invalid");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.AddDaItem",
					" The item argument is invalid");
				*/
			}
		}
		catch(Exception exc){
			logger.error("AddDaItem() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.AddDaItem", 
				exc.getMessage());
			*/
		}
		return res;
	}


	protected int addDaItem(DaItem anItem){
		int res = EnumResultCode.E_FAIL.toInt();

		try{
			if(anItem != null){
				NativeLongByReference itemHandle = new NativeLongByReference();
				
				res = OTClient.INSTANCE.OTCAddDAItem(
					this.getHandle(),
					new WString(anItem.getId()), 
					new NativeLong(anItem.getHashCode()), 
					itemHandle).intValue();

				if(ResultCode.SUCCEEDED(res)){

					anItem.setDaSubscription(this);
					anItem.setHandle(itemHandle.getValue());
					itemList_.put(anItem.getHashCode(), anItem);			
				}
				else{
					logger.error("AddDaItem() -  Adding item failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.AddDaItem",
						" Adding item failed! Result: " + res);
					*/
				}
			}
			else{
				res = EnumResultCode.E_INVALIDARG.toInt();
				logger.error("AddDaItem() -  The item argument is not valid");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.AddDaItem", 
					" The item argument is invalid");
				*/
			}
		}
		catch(Exception exc){
			logger.error("AddDaItem() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.AddDaItem", 
				exc.getMessage());
			*/
		}
		return res;	
	}


	public int removeDaItem(DaItem anItem){
		int res = EnumResultCode.E_FAIL.toInt();
		try{
			if(anItem != null){
				res = OTClient.INSTANCE.OTCRemoveDAItem(((DaItem)itemList_.get(anItem.getHashCode())).getHandle()).intValue();
				if(ResultCode.SUCCEEDED(res)){
					itemList_.remove(anItem.getHashCode());
				}
				else{
					logger.error("RemoveDaItem() - Removing item failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.RemoveDaItem",
						" Removing item failed! Result: " + res);
					*/
				}
			}
			else{
				res = EnumResultCode.E_INVALIDARG.toInt();
				logger.error("RemoveDaItem() - The argument is not valid");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.RemoveDaItem", 
					"The argument is invalid!");
				*/
			}
		}
		catch(Exception exc){
			logger.error("RemoveDaItem() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.RemoveDaItem",
				exc.getMessage());
			*/
		}
		return res;
	}


	public int read(int maxAge, DaItem[] items, ArrayList<ValueQT> values, ArrayList<Integer> results, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();

		
		if(items == null 
				|| values == null 
				|| results == null)
		{
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("Read() - Null Reference parameters passed to the method");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.Read", 
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
			
			//itemHandles
			long[] itemHandles  = new long[items.length];
			for(int i = 0 ; i < items.length; i++)
			{
				itemHandles[i] = ((DaItem)items[i]).getHandle().longValue();
			}
			ArrayList<JNIOTValueData> jnivalues = new ArrayList<JNIOTValueData>();
			res = (int)JNIOTClient.INSTANCE.OTCRead(
				this.getHandle().longValue(),
				(long)items.length,
				itemHandles,
				null,
				null,
				(long)maxAge,
				values,
				results,
				options);

			if(options.m_executionType == (byte)EnumExecutionType.SYNCHRONOUS.getInt()){
				// the read succedeed
				if (ResultCode.SUCCEEDED(res)){
					//nothing
				}	
				else{
					logger.error("Read() - Reading items using subscription failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Read",
						" Reading items using subscription failed! Result: " + res);
					*/
				}
			}
		}
		catch(Exception exc){
			logger.error("Read() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.Read", 
				exc.getMessage());
			*/
		}
		return res;
	}


	public int write(DaItem[] items, ValueQT[] values, ArrayList<Integer> results, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();

		if(results == null)
		{
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("Write() - Null reference passed passed to the Results parameter");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.Write", 
				" Null reference passed passed to the Results parameter");
			*/	
			return res;
		}
		
		results.clear();
		
		if (values == null)
		{
			logger.error("Write() - Values cannot be null");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSubscription.Write", 
				"Values cannot be null!");
			*/
			return res;
		}	//	end if

		if (items == null)
		{
			logger.error("Write() - Items cannot be null");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaSubscription.Write", 
				"Items cannot be null!");
			*/
			return res;
		}	//	end if

		try
		{
			JNIOTCExecutionOptions options = new JNIOTCExecutionOptions();
			
			if(executionOptions != null){
				options.m_executionType = (byte)executionOptions.getExecutionType().getInt();
				options.m_executionContext = executionOptions.getExecutionContext();
			}
			else{
				options.m_executionType = (byte)EnumExecutionType.SYNCHRONOUS.getInt();
				options.m_executionContext = 0;
			}
		
			//itemHandles
			long[] itemHandles  = new long[items.length];
			for(int i = 0 ; i < items.length; i++)
			{
				itemHandles[i] = ((DaItem)items[i]).getHandle().longValue();
			}

			ArrayList<ValueQT> jnivalues = new ArrayList<ValueQT>();
			for(int i = 0; i < items.length; i++)
			{	
				jnivalues.add(values[i]);
			}
		

			
			res = (int)JNIOTClient.INSTANCE.OTCWrite(
				this.getHandle().longValue(),
				(long)items.length,
				itemHandles,
				null,
				null,
				jnivalues,
				results,
				options);

			if (ResultCode.FAILED(res)){
				logger.error("Write() - Write items using subscription failed. Result: " + res);
				/*
				Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Write",
						" Write items using subscription failed! Result: " + res);
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
				"DaSubscription.Write", 
				exc.getMessage());
			*/
		}
		return res;
	}


	public int validateDaItems(DaItem[] items, ArrayList<Integer> results, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		if(results == null)
		{
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("ValidateDaItems() - Null reference passed passed to the Results parameter");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.ValidateDaItems", 
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

			//itemHandles
			java.nio.ByteBuffer bb = java.nio.ByteBuffer.allocateDirect(items.length * NativeLong.SIZE);
			//java.nio.LongBuffer lb = bb.asLongBuffer();
			Pointer itemHandles = Native.getDirectBufferPointer(bb);
			for(int i = 0 ; i < items.length; i++)
			{
				NativeLongByReference nlbr = new NativeLongByReference();
				nlbr.setPointer(itemHandles.share(i*NativeLong.SIZE));
				nlbr.setValue(((DaItem)items[i]).getHandle());
			}
			//results
			java.nio.ByteBuffer bbr = java.nio.ByteBuffer.allocateDirect(items.length  * NativeLong.SIZE);
			Pointer localResults = Native.getDirectBufferPointer(bbr);
			
			/*IntPtr[] itemHandles = new IntPtr[items.Length];
			for (int i=0 ;i<items.Length;i++) {
				itemHandles[i] = ((DaItem)items[i]).Handle;
			}*/
			res = OTClient.INSTANCE.OTCValidateDAItems(
				this.getHandle(),
				new NativeLong(items.length),
				itemHandles,
				localResults,
				options).intValue();

			//getting the results
			//results = new ArrayList<Integer>();
			for(int i=0; i<items.length; i++)
			{
				NativeLongByReference nlbr = new NativeLongByReference();
				nlbr.setPointer(localResults.share(i*NativeLong.SIZE));
				results.add(new Integer(nlbr.getValue().intValue()));
			}
			bbr.clear();
			if (ResultCode.FAILED(res)){
				logger.error("ValidateDaItems() - Validating item failed. Result: " + res);
				/*
				Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.ValidateDaItems",
						" Validating item failed! Result: " + res);
				*/
			}
		}
		catch(Exception exc){
			logger.error("ValidateDaItems() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.ValidateDaItems", 
				exc.getMessage());
			*/
		}
		return res;		
	}


	public int getDaItemAttributesFromServer(DaItem[] items, EnumObjectAttribute[] whatAttributes, ArrayList<Integer> results, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		if(results == null)
		{
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("GetDaItemAttributesFromServer() - Null reference passed passed to the Results parameter");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.GetDaItemAttributesFromServer", 
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

			//itemHandles
			java.nio.ByteBuffer bb = java.nio.ByteBuffer.allocateDirect(items.length * NativeLong.SIZE);
			//java.nio.LongBuffer lb = bb.asLongBuffer();
			Pointer itemHandles = Native.getDirectBufferPointer(bb);
			for(int i = 0 ; i < items.length; i++)
			{
				NativeLongByReference nlbr = new NativeLongByReference();
				nlbr.setPointer(itemHandles.share(i*NativeLong.SIZE));
				nlbr.setValue(((DaItem)items[i]).getHandle());
			}
			NativeLongByReference nlbritemHandles = new NativeLongByReference();
			nlbritemHandles.setPointer(itemHandles);
			
			//Attributes to change
			java.nio.ByteBuffer bba = java.nio.ByteBuffer.allocateDirect(whatAttributes.length * NativeLong.SIZE);
			java.nio.LongBuffer lba = bba.asLongBuffer();
			Pointer pWhatAttributesToChange = Native.getDirectBufferPointer(lba);
			for(int i = 0 ; i < items.length; i++)
			{
				NativeLongByReference nlbr = new NativeLongByReference();
				nlbr.setPointer(pWhatAttributesToChange.share(i*NativeLong.SIZE));
				nlbr.setValue(new NativeLong(whatAttributes[i].getLong()));
			}
			NativeLongByReference nlbrAttributes = new NativeLongByReference();
			nlbrAttributes.setPointer(pWhatAttributesToChange);
			
			//results
			java.nio.ByteBuffer bbr = java.nio.ByteBuffer.allocateDirect(items.length  * NativeLong.SIZE);
			//java.nio.LongBuffer lbr = bbr.asLongBuffer();
			Pointer localResults = Native.getDirectBufferPointer(bbr);
			NativeLongByReference nlbrlocalResults = new NativeLongByReference();
			nlbrlocalResults.setPointer(localResults);
			
			res = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
				this.getHandle(),
				new NativeLong(items.length),
				nlbritemHandles,
				(byte)1,
				new NativeLong(whatAttributes.length),
				nlbrAttributes,
				nlbrlocalResults,
				options).intValue();

			//getting the results
			//results = new ArrayList<Integer>();
			for(int i=0; i<items.length; i++)
			{
				NativeLongByReference nlbr = new NativeLongByReference();
				nlbr.setPointer(localResults.share(i*NativeLong.SIZE));
				results.add(new Integer(nlbr.getValue().intValue()));
			}
			
			if (ResultCode.FAILED(res)){
				logger.error("GetDaItemAttributesFromServer() - Getting attributes from server failed. Result: " + res);
				/*
				Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.GetDaItemAttributesFromServer",
						" Getting attributes from server failed! Result: " + res);
				*/
			}
			bb.clear();
			bba.clear();
			bbr.clear();
		}

		catch(Exception exc){
			logger.error("GetDaItemAttributesFromServer() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.GetDaItemAttributesFromServer", 
				exc.getMessage());
			*/
		}
		return res;
	}


	public int setDaItemAttributesToServer(DaItem[] items, EnumObjectAttribute[] whatAttributes, ArrayList<Integer> results, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		
		if(results == null)
		{
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("SetDaItemAttributesToServer() - Null reference passed passed to the Results parameter");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.SetDaItemAttributesToServer", 
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
		
			//itemHandles
			java.nio.ByteBuffer bb = java.nio.ByteBuffer.allocateDirect(items.length * NativeLong.SIZE);
			//java.nio.LongBuffer lb = bb.asLongBuffer();
			Pointer itemHandles = Native.getDirectBufferPointer(bb);
			for(int i = 0 ; i < items.length; i++)
			{
				NativeLongByReference nlbr = new NativeLongByReference();
				nlbr.setPointer(itemHandles.share(i*NativeLong.SIZE));
				nlbr.setValue(((DaItem)items[i]).getHandle());
			}
			NativeLongByReference nlbritemHandles = new NativeLongByReference();
			nlbritemHandles.setPointer(itemHandles);
			
			//Attributes to change
			java.nio.ByteBuffer bba = java.nio.ByteBuffer.allocateDirect(whatAttributes.length * NativeLong.SIZE);
			//java.nio.LongBuffer lba = bba.asLongBuffer();
			Pointer pWhatAttributesToChange = Native.getDirectBufferPointer(bba);
			for(int i = 0 ; i < items.length; i++)
			{
				NativeLongByReference nlbr = new NativeLongByReference();
				nlbr.setPointer(pWhatAttributesToChange.share(i*NativeLong.SIZE));
				nlbr.setValue(new NativeLong(whatAttributes[i].getLong()));
			}
			NativeLongByReference nlbrAttributes = new NativeLongByReference();
			nlbrAttributes.setPointer(pWhatAttributesToChange);
			
			//results
			java.nio.ByteBuffer bbr = java.nio.ByteBuffer.allocateDirect(items.length  * NativeLong.SIZE);
			//java.nio.LongBuffer lbr = bbr.asLongBuffer();
			Pointer localResults = Native.getDirectBufferPointer(bbr);
			NativeLongByReference nlbrlocalResults = new NativeLongByReference();
			nlbrlocalResults.setPointer(localResults);
			
			res = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
				this.getHandle(),
				new NativeLong(items.length),
				nlbritemHandles,
				(byte)0,
				new NativeLong(whatAttributes.length),
				nlbrAttributes,
				nlbrlocalResults,
				options).intValue();

			//getting the results
			//results = new ArrayList<Integer>();
			for(int i=0; i<items.length; i++)
			{
				NativeLongByReference nlbr = new NativeLongByReference();
				nlbr.setPointer(localResults.share(i*NativeLong.SIZE));
				results.add(new Integer(nlbr.getValue().intValue()));
			}
			bb.clear();
			bba.clear();
			bbr.clear();
			if (ResultCode.FAILED(res)){
				logger.error("SetDaItemAttributesToServer() - Setting attributes to server failed. Result: " + res);
				/*
				Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.SetDaItemAttributesToServer",
						" Setting attributes to server failed! Result: " + res);
				*/
			}
		}
		catch(Exception exc){
			logger.error("SetDaItemAttributesToServer() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.SetDaItemAttributesToServer", 
				exc.getMessage());
			*/
		}
		return res;
	}


	public int refresh(int maxAge, ExecutionOptions executionOptions){
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

			res = OTClient.INSTANCE.OTCRefreshDAGroup(
					this.getHandle(),
					new NativeLong(maxAge),
					options).intValue();		

			if (ResultCode.FAILED(res)){
				logger.error("Refresh() -  Refreshing subscription failed. Result: " + res);
				/*
				Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Refresh",
						" Refreshing subscription failed! Result: " + res);
				*/
			}
		}
		catch(Exception exc){
			logger.error("Refresh() -  " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaSubscription.Refresh", 
				exc.getMessage());
			*/
		}
		return res;
	}


	protected DaItem find(int position){
		return (DaItem)itemList_.get(position);
	}


	protected void onDataChange(DaItem[] items, ValueQT[] values, int[] results){
		//override it
	}


	protected void onReadComplete(int executionContext, DaItem[] items, ValueQT[] values, int[] results, int result){
		//override it
	}


	protected void onWriteComplete(int executionContext, DaItem[] items, ValueQT[] values, int[] results, int result){
		//override it
	}


	protected void onValidateDaItems(int executionContext, DaItem[] items, int[] results, int result){
		//override it
	}


	protected void OnGetDaItemAttributesFromServer(int executionContext, DaItem[] items, EnumObjectAttribute[] whatAttributes, int[] results, int result){
		//override it
	}


	protected void OnSetDaItemAttributesToServer(int executionContext, DaItem[] items, EnumObjectAttribute[] whatAttributes, int[] results, int result){
		//override it
	}



}