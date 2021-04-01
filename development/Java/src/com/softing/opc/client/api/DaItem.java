package com.softing.opc.client.api;

import java.io.Serializable;
import java.util.Calendar;
import java.util.Date;
import java.util.ArrayList;
import java.util.TimeZone;

import com.softing.opc.client.enums.EnumAccessRights;
import com.softing.opc.client.enums.EnumEUType;
import com.softing.opc.client.enums.EnumExecutionType;
import com.softing.opc.client.enums.EnumObjectAttribute;
import com.softing.opc.client.enums.EnumQuality;
import com.softing.opc.client.enums.EnumResultCode;
import com.softing.opc.client.enums.EnumTraceGroup;
import com.softing.opc.client.enums.EnumTraceLevel;
import com.softing.opc.client.enums.EnumUpdateAttributesBehavior;
import com.softing.opc.client.enums.ResultCode;
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

public class DaItem extends ObjectSpaceElement implements Serializable {

	final Logger logger = LoggerFactory.getLogger(DaItem.class);
	
	private final OTCObjectAttributes objectAttributes_ = new OTCObjectAttributes();
	private final Object attributesJanitor_ = new Object();
	protected DaSubscription subscription_ = null;
	protected ValueQT valueQT_ = null;
	protected final Object valueQTJanitor_ = new Object();
	
	private static final long serialVersionUID = 7526471155622776147L;


	public DaItem(String itemId, DaSubscription parentSubscription){
		Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
		calendar.set(1601, Calendar.JANUARY, 1, 0, 0, 0);
        calendar.set(Calendar.MILLISECOND, 0);
		Date timeStampInitial = calendar.getTime();
		
		this.valueQT_ = new ValueQT(
				null,
				EnumQuality.BAD_WAITING_FOR_INITIAL_DATA,
				timeStampInitial);

		subscription_ = parentSubscription;

		try{
			int res = EnumResultCode.E_FAIL.toInt();			
			res = parentSubscription.addDaItem(itemId, this);
			if(ResultCode.FAILED(res)){
				logger.error("Constructor - The item could not be added to the subscription. Result: " + res);
				/*
				Application.Instance.Trace(
						EnumTraceLevel.ERR, EnumTraceGroup.CLIENT,
						"DaItem.Constructor", 
						"The item could not be added to the subscription! Result:" + res);
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
					"DaItem.Constructor",
					exc.getMessage());
			*/
		}	
	}

	protected DaItem(){

	}



	public ValueQT getValueQT(){
		return valueQT_;
	}

	public void setValueQT(ValueQT valueQt){
		try{
			synchronized (valueQTJanitor_){

				valueQT_ = valueQt;
			}
		}
		catch(Exception exc){
			//exc.printStackTrace();
			logger.error("setValueQT() - " + exc.getMessage(), exc);
			//Application.Instance.Trace(EnumTraceLevel.ERR,EnumTraceGroup.CLIENT,"Da.Item.ValueQT.Set", exc.getMessage());					
		}
	}

	public String getValueToString(){
		try{
			if((getValueQT().getData() != null)
			  && 
			  (this.getValueQT().getQuality() == EnumQuality.GOOD)){
				ValueData vt = new ValueData(getValueQT().getData());
				return vt.toString();
			}
			else{
				if(this.getValueQT().getData() == null){
					return "";
				}
				if(this.getValueQT().getQuality() != EnumQuality.GOOD){
					return Application.INSTANCE.getBadQualityValueString();
				}
			}
		}
		catch(Exception exc){
			logger.error("getValueToString() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			//Application.Instance.Trace(EnumTraceLevel.ERR,EnumTraceGroup.CLIENT,"Da.Item.ValueToString", exc.getMessage());					
		}
		return "";
	}

	public DaSubscription getDaSubscription(){
		return subscription_;
	}

	public void setDaSubscription(DaSubscription subscription){
		subscription_ = subscription;
	}

	public String getId(){
		String ID = "";
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();	
				synchronized(attributesJanitor_){
					
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_ID.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)1,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getId() - Update of DAITEM_ID attribute from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.Id.Get", 
								"Update attribute DAITEM_ID from server failed!Result: " + updateResult);
							*/
						}	//	end if
					}	//	end if
					
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
						this.getHandle(),
						new NativeLong(EnumObjectAttribute.DAITEM_ID.getLong()),
						objectAttributes).intValue();
					
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{				
						ID = objectAttributes.m_daItemID.getString(0, true);			
						OTClient.INSTANCE.OTFreeMemory(objectAttributes.m_daItemID);							
					}// end if
					else{	
						logger.error("getId() - Return of DAITEM_ID attribute from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaItem.Id.Get", 
							"Get attribute DAITEM_ID from server failed!Result: " + getAttrsResult);
						*/
					} //end else	
				}// end lock
			}	//	end if					
		}
		catch(Exception exc){
			logger.error("getId() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Da.Item.ID.Get",
				exc.getMessage());
			*/
		}
		return ID;
	}

	public void setId(String value){
		try{
			if(this.getHandle().longValue() != 0){
				if (!value.equals("")){
					OTCExecutionOptions options = new OTCExecutionOptions() ;
					int result = -1;
					synchronized(attributesJanitor_){
						//string to ByteByReference
						byte[] bytes = value.getBytes("UTF-16LE"); //unicode
						java.nio.ByteBuffer bb = java.nio.ByteBuffer.allocateDirect(bytes.length + 1);
						bb.put(bytes);
						bb.put(bytes.length, (byte)0);

						Pointer p = Native.getDirectBufferPointer(bb);
						
						objectAttributes_.m_daItemID = p;
						int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
												this.getHandle(),
												new NativeLong(EnumObjectAttribute.DAITEM_ID.getLong()),
												objectAttributes_).intValue();	
						if(ResultCode.FAILED(setAttrsResult)){
							logger.error("setId() - Set of DAITEM_ID attribute failed. Result: " + setAttrsResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"Da.Session.LocaleId.Set",
								"Set attribute DAITEM_ID failed! Result: " + setAttrsResult );
							*/	
						} // end if
						
						if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
							
							//prepare itemHandle
							NativeLongByReference itemHandles = new NativeLongByReference();
							itemHandles.setValue(this.getHandle());
							
							//prepare what attributes
							NativeLongByReference whatAttributes = new NativeLongByReference();
							whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_ID.getLong()));
							
							//prepare results
							NativeLongByReference results = new NativeLongByReference();
							results.setValue(new NativeLong(result));
							
							int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)0,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();							
							if(ResultCode.FAILED(updateResult)){
								logger.error("setId() - Set of DAITEM_ID attribute failed. Result: " + updateResult);
								/*
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT, 
									"DaItem.Id.Set",
									"Set attribute DAITEM_ID failed! Result: " + updateResult );
								*/	
							}
						}// end if								
						//TODO: FREE m_objectAttributes.m_daItemID - was previously allocated with ByteBuffer.allocateDirect
						bb.clear();
					}// end lock
				}// end if
			}// end if
			else{
				logger.error("setId() - Set of Id failed. Item not added to the subscription");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaItem.Id.Set",
					"Set Id failed! Item is not added to a subscription." );
				*/
			}	//	end if...else
		}
		catch(Exception exc){	
			logger.error("setId() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Da.Item.ID.Set", 
				exc.getMessage());
			*/					
		}	//	end try...catch
	}

	public String getPath(){
		String path = "";
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();	
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_ID.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)1,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getPath() - Update attribute DAITEM_PATH from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.Path.Get", 
								"Update attribute DAITEM_PATH from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DAITEM_PATH.getLong()),
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{				
						path = objectAttributes.m_daItemPath.getString(0, true);			
						OTClient.INSTANCE.OTFreeMemory(objectAttributes.m_daItemPath);	
						
					}// end if	
					else{	
						logger.error("getPath() - Get attribute DAITEM_PATH from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaItem.Path.Get", 
							"Get attribute DAITEM_PATH from server failed!Result: " + getAttrsResult);
						*/
					} //end else	
				}// end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getPath() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Da.Item.Path.Get", 
				exc.getMessage());
			*/					
		}
		return path;
	}

	public void setPath(String value){
		try{
			if(this.getHandle().longValue() != 0){
				if (!value.equals("")){
					OTCExecutionOptions options = new OTCExecutionOptions() ;
					int result = -1;
					synchronized(attributesJanitor_){
						byte[] bytes = value.getBytes("UTF-16LE"); //UNICODE
						java.nio.ByteBuffer bb = java.nio.ByteBuffer.allocateDirect(bytes.length + 1);
						bb.put(bytes);
						bb.put(bytes.length, (byte)0);
						Pointer p = Native.getDirectBufferPointer(bb);
						//ByteByReference bbr = new ByteByReference();
						//bbr.setPointer(p);
						
						objectAttributes_.m_daItemPath = p;
						int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
												this.getHandle(),
												new NativeLong(EnumObjectAttribute.DAITEM_PATH.getLong()),
												objectAttributes_).intValue();	
						if(ResultCode.FAILED(setAttrsResult)){	
							logger.error("setPath() - Set attribute DAITEM_PATH failed. Result: " + setAttrsResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.Path.Set",
								"Set attribute DAITEM_PATH failed! Result: " + setAttrsResult );
							*/	
						} // end if
						if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
							
							//prepare itemHandle
							NativeLongByReference itemHandles = new NativeLongByReference();
							itemHandles.setValue(this.getHandle());
							
							//prepare what attributes
							NativeLongByReference whatAttributes = new NativeLongByReference();
							whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_PATH.getLong()));
							
							//prepare results
							NativeLongByReference results = new NativeLongByReference();
							results.setValue(new NativeLong(result));
							
							int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
													this.getDaSubscription().getHandle(),
													new NativeLong(1),
													itemHandles,
													(byte)0,
													new NativeLong(1),
													whatAttributes,
													results,
													options).intValue();	
							if(ResultCode.FAILED(updateResult)){
								logger.error("setPath() - Set attribute DAITEM_PATH failed. Result: " + updateResult);
								/*
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT, 
									"DaItem.Path.Set",
									"Set attribute DAITEM_PATH failed! Result: " + updateResult );
								*/	
							}
						}// end if
						//TODO: free m_objectAttributes.m_daItemPath. it was allocated with allocateDirect
						bb.clear();
					}// end lock
				}// end if
			}
			else{
				logger.error("setPath() - failed. Item is not added to a subscription");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaItem.Path.Set",
					"Set Path failed! Item is not added to a subscription.");
				*/	
			}	//	end if..else
		}
		catch(Exception exc){	
			logger.error("setPath() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Da.Item.Path.Set", 
				exc.getMessage());
			*/
		}	//	end try...catch
	}

	public Class getNativeDatatype(){
		short nativeDataType = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();	
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_NATIVE_DATATYPE.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)1,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getNativeDatatype() - Update attribute DAITEM_NATIVE_DATATYPE from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.NativeDatatype.Get", 
								"Update attribute DAITEM_NATIVE_DATATYPE from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DAITEM_NATIVE_DATATYPE.getLong()), 
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						nativeDataType = objectAttributes.m_daItemNativeDatatype;
					}// end if	
					else{
						logger.error("getNativeDatatype() - Get attribute DAITEM_NATIVE_DATATYPE from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaItem.NativeDatatype.Get", 
							"Get attribute DAITEM_NATIVE_DATATYPE from server failed!Result: " + getAttrsResult);
						*/								
					} //end if...else	
				}// end lock
			}	//	end if					
		}
		catch(Exception exc){
			logger.error("getNativeDatatype() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaItem.NativeDatatype.Get",
				exc.getMessage());
			*/
		}
		return VARIANT.getSysType(nativeDataType);
	}

	public Class getRequestedDatatype(){
		short requestedDataType = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();	
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)1,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getRequestedDatatype() - Update attribute DAITEM_REQUESTED_DATATYPE from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.RequestedDatatype.Get", 
								"Update attribute DAITEM_REQUESTED_DATATYPE from server failed!Result: " + updateResult);
							*/
							
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE.getLong()),
											objectAttributes).intValue(); 
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						requestedDataType =  objectAttributes.m_daItemRequestedDatatype;
					}// end if	
					else{	
						logger.error("getRequestedDatatype() - Get attribute DAITEM_REQUESTED_DATATYPE from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaItem.RequestedDatatype.Get", 
							"Get attribute DAITEM_REQUESTED_DATATYPE from server failed!Result: " + getAttrsResult);
						*/								
					} //end else	
				}//end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getRequestedDatatype() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaItem.RequestedDatatype.Get", 
				exc.getMessage());
			*/
		}
		return VARIANT.getSysType(requestedDataType);
	}


	public void setRequestedDatatype(Class value){
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){
					objectAttributes_.m_daItemRequestedDatatype = (short)VARIANT.getType(value);
					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE.getLong()),
											objectAttributes_).intValue();	
					if(ResultCode.FAILED(setAttrsResult)){
						logger.error("setRequestedDatatype() - Set attribute DAITEM_REQUESTED_DATATYPE failed. Result: " + setAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaItem.RequestedDatatype.Set",
							"Set attribute DAITEM_REQUESTED_DATATYPE failed! Result: " + setAttrsResult );
						*/	
					} // end if
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)0,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("setRequestedDatatype() - Set attribute DAITEM_REQUESTED_DATATYPE failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.RequestedDatatype.Set",
								"Set attribute DAITEM_REQUESTED_DATATYPE failed! Result: " + updateResult );
							*/	
						}	//	end if
					}// end if
				}// end lock
			}
			else{
				logger.error("setRequestedDatatype() - Set RequestedDatatype failed. Item is not added to a subscription");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaItem.RequestedDatatype.Set",
					"Set RequestedDatatype failed! Item is not added to a subscription.");
				*/	
			}	// end if...else
		}
		catch(Exception exc){
			logger.error("setRequestedDatatype() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Da.Item.RequestedDatatype.Set", 
				exc.getMessage());
			*/
		}
	}

	public EnumAccessRights getAccessRights(){
		EnumAccessRights accessRights = EnumAccessRights.READWRITEABLE; 
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();	
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_ACCESSRIGHTS.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)1,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getAccessRights() - Update attribute DAITEM_ACCESSRIGHTS from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.AccessRights.Get", 
								"Update attribute DAITEM_ACCESSRIGHTS from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DAITEM_ACCESSRIGHTS.getLong()),
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{					
						accessRights =  EnumAccessRights.getEnum(objectAttributes.m_daItemAccessRights.intValue());	
					}
					else{
						logger.error("getAccessRights() - Get attribute DAITEM_ACCESSRIGHTS from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaItem.AccessRights.Get", 
							"Get attribute DAITEM_ACCESSRIGHTS from server failed!Result: " + getAttrsResult);
						*/								
					} //end if...else	
				}// end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getAccessRights() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaItem.AccessRights.Get", 
				exc.getMessage());
			*/
		}
		return accessRights;
	}

	public float getDeadband(){
		float deadband = 0;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();	
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_DEADBAND.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)1,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getDeadband() - Update attribute DAITEM_DEADBAND from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.Deadband.Get", 
								"Update attribute DAITEM_DEADBAND from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DAITEM_DEADBAND.getLong()),
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						deadband = objectAttributes.m_daItemDeadband;
					}// end if	
					else{
						logger.error("getDeadband() - Get attribute DAITEM_DEADBAND from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaItem.Deadband.Get", 
							"Get attribute DAITEM_DEADBAND from server failed!Result: " + getAttrsResult);
						*/
					} //end else	
				}// end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getDeadband() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Da.Item.Deadband.Get", 
				exc.getMessage());
			*/
		}	//	end try...catch
		return deadband;
	}

	public void setDeadband(float value){
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){	
					objectAttributes_.m_daItemDeadband = value;
					int setAttrsResult = OTClient.INSTANCE.OTCSetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DAITEM_DEADBAND.getLong()),
											objectAttributes_).intValue();	
					if(ResultCode.FAILED(setAttrsResult)){
						logger.error("setDeadband() - Set attribute DAITEM_DEADBAND failed. Result: " + setAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"Da.Session.LocaleId.Set",
							"Set attribute DAITEM_DEADBAND failed! Result: " + setAttrsResult );
						*/	
					} // end if
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_DEADBAND.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)0,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();	
						if(ResultCode.FAILED(updateResult)){
							logger.error("setDeadband() - Set attribute DAITEM_DEADBAND failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"Da.Session.LocaleId.Set",
								"Set attribute DAITEM_DEADBAND failed! Result: " + updateResult );
							*/	
						}
					}// end if
				}// end lock
			}
			else{
				logger.error("setDeadband() - Set Deadband failed. Item is not added to a subscription");
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT, 
					"DaItem.Deadband.Set",
					"Set Deadband failed! Item is not added to a subscription ");
				*/	
			}	//	end if...else
		}
		catch(Exception exc){
			logger.error("setDeadband() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaItem.Deadband.Set", 
				exc.getMessage());
			*/					
		}
	}

	public EnumEUType getEUType(){
		EnumEUType type = EnumEUType.NOENUM;
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();	
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_EUTYPE.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)1,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getEUType() - Update attribute DAITEM_EUTYPE from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.EUType.Get", 
								"Update attribute DAITEM_EUTYPE from server failed!Result: " + updateResult);
							*/
						}
					}
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DAITEM_EUTYPE.getLong()), 
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						type = EnumEUType.getEnum(objectAttributes.m_daItemEuType); 
					}// end if
					else{	
						logger.error("getEUType() - Get attribute DAITEM_EUTYPE from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaItem.EUType.Get", 
							"Get attribute DAITEM_EUTYPE from server failed!Result: " + getAttrsResult);
						*/								
					} //end else	
				}// end lock
			}// end if
		}
		catch(Exception exc){
			logger.error("getEUType() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaItem.EUType.Get", 
				exc.getMessage());
			*/					
		}
		return type;
	}

	public Object getEUInfo(){
		Object info = new Object();
		OTCObjectAttributes objectAttributes = new OTCObjectAttributes();	
		try{
			if(this.getHandle().longValue() != 0){
				OTCExecutionOptions options = new OTCExecutionOptions() ;
				int result = -1;
				synchronized(attributesJanitor_){
					if(Application.INSTANCE.getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE){
						
						//prepare itemHandle
						NativeLongByReference itemHandles = new NativeLongByReference();
						itemHandles.setValue(this.getHandle());
						
						//prepare what attributes
						NativeLongByReference whatAttributes = new NativeLongByReference();
						whatAttributes.setValue(new NativeLong(EnumObjectAttribute.DAITEM_EUINFO.getLong()));
						
						//prepare results
						NativeLongByReference results = new NativeLongByReference();
						results.setValue(new NativeLong(result));
						
						int updateResult = OTClient.INSTANCE.OTCUpdateDAItemAttributes(
												this.getDaSubscription().getHandle(),
												new NativeLong(1),
												itemHandles,
												(byte)1,
												new NativeLong(1),
												whatAttributes,
												results,
												options).intValue();
						if(ResultCode.FAILED(updateResult)){
							logger.error("getEUInfo() - Update attribute DAITEM_EUINFO from server failed. Result: " + updateResult);
							/*
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT, 
								"DaItem.EUInfo.Get", 
								"Update attribute DAITEM_EUINFO from server failed!Result: " + updateResult);
							*/
						}
					}
					Pointer p = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(ValueQT.VARIANT_SIZE));
					objectAttributes.m_daItemEuInfo = new VARIANT(p); 
					
					int getAttrsResult = OTClient.INSTANCE.OTCGetAttributes(
											this.getHandle(),
											new NativeLong(EnumObjectAttribute.DAITEM_EUINFO.getLong()),
											objectAttributes).intValue();
					if(ResultCode.SUCCEEDED(getAttrsResult))
					{						
						//info = Marshal.GetObjectForNativeVariant(objectAttributes.m_daItemEuInfo);
						info = objectAttributes.m_daItemEuInfo.getObject(); 
					}// end if
					else{	
						logger.error("getEUInfo() - Get attribute DAITEM_EUINFO from server failed. Result: " + getAttrsResult);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT, 
							"DaItem.EUInfo.Get", 
							"Get attribute DAITEM_EUINFO from server failed!Result: " + getAttrsResult);
						*/
						
					} //end else	
					OTClient.INSTANCE.OTVariantClear(objectAttributes.m_daItemEuInfo);
					OTClient.INSTANCE.OTFreeMemory(objectAttributes.m_daItemEuInfo.getPointer());
					//Win32Functions.VariantClear(objectAttributes.m_daItemEuInfo);
					//Marshal.FreeCoTaskMem(objectAttributes.m_daItemEuInfo);
				}// end lock
			}// end if
		}
		
		catch(Exception exc){
			logger.error("getEUInfo() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"Da.Item.EUInfo.Get", 
				exc.getMessage());
			*/				
		}
	    return info;	
	}

	public int Read(int maxAge, ValueQT itemValue, Integer result, ExecutionOptions executionOptions){
		
		int res = EnumResultCode.E_FAIL.toInt();
		result = EnumResultCode.E_FAIL.toInt();

		if(itemValue == null || result ==null)
		{
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("Read() - Null Reference parameters passed to the method");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaItem.Read", 
				" Null Reference parameters passed to the method");
			*/	
			return res;
		}
		if (subscription_ == null){
			
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("Read() - The item must belong to a subscription");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaItem.Read", 
				" The item must belong to a subscription");
			*/				
		}
		else{
			
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
				ArrayList<ValueQT> values = new ArrayList<ValueQT>(); //contains JNIOTValueData - can be used a generic type
				ArrayList<Integer> results = new ArrayList<Integer>(); //contains Integers - can be parameterized to <Integer>
				res = (int)JNIOTClient.INSTANCE.OTCRead(
						subscription_.getHandle().longValue(), 
						(long)1, 
						new long[] {this.getHandle().longValue()}, 
						new String[] {this.getId()}, 
						new String[] {this.getPath()}, 
						(long)maxAge, 
						values, 
						results, 
						options);
				
				if(options.m_executionType == (byte)EnumExecutionType.SYNCHRONOUS.getInt()){
					
					if(ResultCode.SUCCEEDED(res)){
						itemValue.setData(values.get(0));
						result = results.get(0);
					}
					else{
						logger.error("Read() - Reading item failed. Result: " + res);
						/*
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaItem.Read", 
							" Reading item failed! Result: " + res);
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
					"DaItem.Read", 
					exc.getMessage());
				*/
			}
		}
		return res;
	}

	public int Write(ValueQT itemValue, Integer result, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		result = EnumResultCode.E_FAIL.toInt();
		if (subscription_ == null){
			
			res = EnumResultCode.E_INVALIDARG.toInt();
			logger.error("Write() - The item must belong to a subscription");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaItem.Write", 
				" The item must belong to a subscription");
			*/				
		}
		else{
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

		
				//prepare value
				ArrayList<ValueQT> values = new ArrayList<ValueQT>();
				values.add(itemValue);
				
				//prepare results
				ArrayList<Integer> results = new ArrayList<Integer>();
				
				res = (int)JNIOTClient.INSTANCE.OTCWrite(
					subscription_.getHandle().longValue(),
					(long)1,
					new long[] {this.getHandle().longValue()},
					new String[] {this.getId()},
					new String[] {this.getPath()},
					values,
					results,
					options); 
		
				if ( ResultCode.SUCCEEDED(res)){
					result = results.get(0);
				}
				else{	
					logger.error("Write() - Writing item failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaItem.Write", 
						"Writing item failed! Result: " + res);
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
					"DaItem.Write", 
					exc.getMessage());
				*/
			}
		}
		return res;
	}

	public int getAttributesFromServer(EnumObjectAttribute[] whatAttributes, ArrayList<Integer> results, ExecutionOptions executionOptions){
		return subscription_.getDaItemAttributesFromServer(
				new DaItem[] { this },
				whatAttributes,
				results,
				executionOptions);
	}

	public int setAttributesToServer(EnumObjectAttribute[] whatAttributes, ArrayList<Integer> results, ExecutionOptions executionOptions){
		return subscription_.setDaItemAttributesToServer(
				new DaItem[] { this },
				whatAttributes,
				results,
				executionOptions);
	}

	protected void onValueChange(ValueQT aValue, int result){
		//override it
	}


}