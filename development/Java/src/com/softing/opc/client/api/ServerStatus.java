package com.softing.opc.client.api;

import java.util.Date;

public class ServerStatus {

	private  int state = 0;
	private  Date startTime = new Date();
	private  Date currentTime = new Date();
	private  String vendorInfo = "";
	private  String productVersion = "";
	private  String[] supportedLcIds = null;
	private  Date lastUpdateTime = new Date();
	private  int groupCount = 0;
	private  int bandwidth = 0;
	private  String statusInfo = "";
	
	public ServerStatus(){
	}		
	
	protected ServerStatus( int stateValue, Date startTimeValue,
			 Date currentTimeValue, String vendorInfoValue,
			 String productVersionValue, String[] supportedLcIdsValue,
			 Date lastUpdateTimeValue, int groupCountValue,
			 int bandwidthValue,String statusInfoValue){
		state = stateValue;                
		startTime = startTimeValue;      
		currentTime = currentTimeValue;    
		vendorInfo = vendorInfoValue;         
		productVersion = productVersionValue;     
		supportedLcIds = supportedLcIdsValue;      
		lastUpdateTime = lastUpdateTimeValue; 
		groupCount = groupCountValue;           
		bandwidth = bandwidthValue ;            
		statusInfo = statusInfoValue;        
	}
	
	public String getVendorInfo(){
		return vendorInfo;
	}
	
	public void setVendorInfo(String value){
		vendorInfo = value;
	}
	
	public int getState(){
		return state;
	}
	
	public void setState(int value){
		state = value;
	}
	
	public Date getStartTime(){
		return startTime;
	}
	
	public void setStartTime(Date value){
		startTime = value;
	}
	
	public Date getCurrentTime(){
		return currentTime;
	}
	
	public void setCurrentTime(Date value){
		currentTime = value;
	}
	
	public String getProductVersion(){
		return productVersion;
	}
	
	public void setProductVersion(String value){
		productVersion = value;
	}
	
	public String[] getSupportedLcIds(){
		return supportedLcIds;
	}
	
	public void setSupportedLcIds(String[] value){
		supportedLcIds = value;
	}
	
	public Date getLastUpdateTime(){
		return lastUpdateTime;
	}
	
	public void setLastUpdateTime(Date value){
		lastUpdateTime = value;
	}
	
	public int getGroupCount(){
		return groupCount;
	}
	
	public void setGroupCount(int value){
		groupCount = value;
	}
	
	public int getBandwidth(){
		return bandwidth;
	}
	
	public void setBandwidth(int value){
		bandwidth = value;
	}
	
	public String getStatusInfo(){
		return statusInfo;
	}
	
	public void setStatusInfo(String value){
		statusInfo = value;
	}
	
}
