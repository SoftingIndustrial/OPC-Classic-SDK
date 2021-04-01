using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using DemoClient.Helper;

namespace DemoClient.DockWindows
{
    public partial class PropertyWindow : ToolWindow
    {
        #region Private Members

        private DaItem daItem = null;
        private DaSubscription daSubscription = null;
        private DaSession daSession = null;
        private AeSubscription aeSubscription = null;
        private AeSession aeSession = null;

        #endregion

        #region Private and Protected Methods

        /// <summary>
        /// Set DaProperty data
        /// </summary>
        /// <param name="propertiesColl"></param>
        /// <param name="property"></param>
        /// <param name="category"></param>
        private void SetPropertyData(ref Helper.CustomProperties propertiesColl, DaProperty property, string category)
        {
            Helper.CustomProperty customProperty = new DemoClient.Helper.CustomProperty("Item Id",
                    "System.String", category, null, property.ItemId);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propertiesColl.Properties.Add(customProperty);

            customProperty = new DemoClient.Helper.CustomProperty("Property Id",
                     "System.String", category, null, property.Id);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propertiesColl.Properties.Add(customProperty);

            customProperty = new DemoClient.Helper.CustomProperty("Property Name",
                     "System.String", category, null, property.Name);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propertiesColl.Properties.Add(customProperty);

            customProperty = new DemoClient.Helper.CustomProperty("Value",
                   "System.Object", category, null, property.ValueQT.Data);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propertiesColl.Properties.Add(customProperty);

            customProperty = new DemoClient.Helper.CustomProperty("Data Type",
                   "System.String", category, null, property.DataType);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propertiesColl.Properties.Add(customProperty);

            customProperty = new DemoClient.Helper.CustomProperty("Description",
                   "System.String", category, null, property.Description);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propertiesColl.Properties.Add(customProperty);
        }

        /// <summary>
        /// Change item properties
        /// </summary>
        private void ExecuteDaItemProperties()
        {
            if (this.daItem == null) return;

            Helper.DataTypes dataTypeHelper = new DataTypes();
            Helper.CustomProperties propServiceColl = propertyGrid.SelectedObject as Helper.CustomProperties;
            if (propServiceColl == null) return;

            string strItemId = "";
            string strItemPath = "";
            string strDataType = "";
            string strDeadBand = "";
            foreach (Helper.CustomProperty customProperty in propServiceColl.Properties)
            {
                if (customProperty == null) return;
                if (customProperty.Name.Equals("Item Id"))
                {
                    strItemId = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Item Path"))
                {
                    strItemPath = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Requested Data Type"))
                {
                    strDataType = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Deadband"))
                {
                    strDeadBand = customProperty.DefaultValue.ToString();
                }
            }

            //set item new values
            daItem.Id = strItemId;
            daItem.Path = strItemPath;
            //find the data type
            Type dataType = null;
            if (dataTypeHelper.RequiredDataTypes.ContainsValue(strDataType))
            {
                foreach (Type type in dataTypeHelper.RequiredDataTypes.Keys)
                {
                    if (dataTypeHelper.RequiredDataTypes[type] == strDataType)
                    {
                        dataType = type;
                        break;
                    }
                }
            }
            if (dataType == null) return;
            daItem.RequestedDatatype = dataType;
            float deadBand;
            float.TryParse(strDeadBand, out deadBand);
            daItem.Deadband = deadBand;
            int[] itemResults;

            List<EnumObjectAttribute> objAttributes = new List<EnumObjectAttribute>();
            objAttributes.Add(EnumObjectAttribute.DAITEM_ID);
            objAttributes.Add(EnumObjectAttribute.DAITEM_PATH);
            objAttributes.Add(EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE);
            objAttributes.Add(EnumObjectAttribute.DAITEM_DEADBAND);
            objAttributes.Add(EnumObjectAttribute.DAITEM_NATIVE_DATATYPE);
            objAttributes.Add(EnumObjectAttribute.DAITEM_ACCESSRIGHTS);
            objAttributes.Add(EnumObjectAttribute.DAITEM_EUTYPE);
            objAttributes.Add(EnumObjectAttribute.DAITEM_EUINFO);

            ExecutionOptions executionOptions = new ExecutionOptions();
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;
            daItem.DaSubscription.SetDaItemAttributesToServer(new DaItem[] { daItem }, objAttributes.ToArray(), out itemResults, executionOptions);

        }

        /// <summary>
        /// Change DaSubscription properties
        /// </summary>
        private void ExecuteDaSubscriptionProperties()
        {
            if (this.daSubscription == null) return;

            Helper.LocaleId localeId = new LocaleId();
            Helper.CustomProperties propServiceColl = propertyGrid.SelectedObject as Helper.CustomProperties;
            if (propServiceColl == null) return;

            string strName = "";
            string strUpdateRate = "";
            string strLCID = "";
            string strDeadBand = "";
            string strKeepAliveTime = "";
            foreach (Helper.CustomProperty customProperty in propServiceColl.Properties)
            {
                if (customProperty == null) return;
                if (customProperty.Name.Equals("Name"))
                {
                    strName = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Update Rate(ms)"))
                {
                    strUpdateRate = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Deadband"))
                {
                    strDeadBand = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Locale Id"))
                {
                    strLCID = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Keep Alive Time(ms)"))
                {
                    strKeepAliveTime = customProperty.DefaultValue.ToString();
                }  
            }

            //set item new values
            daSubscription.Name = strName;
            uint updateRate;
            uint.TryParse(strUpdateRate, out updateRate);
            daSubscription.RequestedUpdateRate = updateRate;
            float deadBand;
            float.TryParse(strDeadBand, out deadBand);
            daSubscription.Deadband = deadBand;
            daSubscription.LocaleId = strLCID;
            uint keepAliveTime;
            uint.TryParse(strKeepAliveTime, out keepAliveTime);
            daSubscription.RequestedKeepAliveTime = keepAliveTime;

            ExecutionOptions executionOptions = new ExecutionOptions();
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;
            int[] results;
            daSubscription.SetAttributesToServer(new EnumObjectAttribute[] { EnumObjectAttribute.DASUBSCRIPTION_ALL }, out results, executionOptions);
            //set opc version
            foreach (Helper.CustomProperty customProperty in propServiceColl.Properties)
            {
                if (customProperty == null) return;
                if (customProperty.Name.Equals("Revised Update Rate(ms)"))
                {
                    customProperty.DefaultValue = updateRate;
                }
                else if (customProperty.Name.Equals("Revised Keep Alive Time(ms)"))
                {
                    customProperty.DefaultValue = keepAliveTime;
                }
            }
            propertyGrid.SelectedObject = propServiceColl;
        }

        /// <summary>
        /// Change AeSubscription properties
        /// </summary>
        private void ExecuteAeSubscriptionProperties()
        {
            if (this.aeSubscription == null) return;

            Helper.LocaleId localeId = new LocaleId();
            Helper.CustomProperties propServiceColl = propertyGrid.SelectedObject as Helper.CustomProperties;
            if (propServiceColl == null) return;

            string strBufferTime = "";
            string strMaxSize = "";
            
            foreach (Helper.CustomProperty customProperty in propServiceColl.Properties)
            {
                if (customProperty == null) return;
                if (customProperty.Name.Equals("Buffer Time"))
                {
                    strBufferTime = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Max Size"))
                {
                    strMaxSize = customProperty.DefaultValue.ToString();
                }    
            }

            //set item new values
            uint bufferTime;
            uint.TryParse(strBufferTime, out bufferTime);
            aeSubscription.RequestedBufferTime = bufferTime;
            uint maxSize;
            uint.TryParse(strMaxSize, out maxSize);
            aeSubscription.RequestedMaxSize = maxSize;            

            ExecutionOptions executionOptions = new ExecutionOptions();
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;
            int[] results;
            aeSubscription.SetAttributesToServer(new EnumObjectAttribute[] { EnumObjectAttribute.AESUBSCRIPTION_ALL }, out results, executionOptions);
            //set opc version
            foreach (Helper.CustomProperty customProperty in propServiceColl.Properties)
            {
                if (customProperty == null) return;
                if (customProperty.Name.Equals("Revised Buffer Time"))
                {
                    customProperty.DefaultValue = bufferTime;
                }
                else if (customProperty.Name.Equals("Revised Max Size"))
                {
                    customProperty.DefaultValue = maxSize;
                }
            }
            propertyGrid.SelectedObject = propServiceColl;
        }

        /// <summary>
        /// Change AeSession properties
        /// </summary>
        private void ExecuteAeSessionProperties()
        {
            if (this.aeSession == null) return;

            Helper.LocaleId localeId = new LocaleId();
            Helper.CustomProperties propServiceColl = propertyGrid.SelectedObject as Helper.CustomProperties;
            if (propServiceColl == null) return;

            string strLocalId = "";
            string strClassContext = "";
            string strClientName = "";

            foreach (Helper.CustomProperty customProperty in propServiceColl.Properties)
            {
                if (customProperty == null) return;
                if (customProperty.Name.Equals("Locale Id"))
                {
                    strLocalId = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Client Name"))
                {
                    strClientName = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("DCOM Class Context"))
                {
                    strClassContext = customProperty.DefaultValue.ToString();
                }
            }

            //set item new values
            aeSession.LocaleId = strLocalId;
            aeSession.ClientName = strClientName;
            aeSession.ClassContext = GetClassContext(strClassContext);

            ExecutionOptions executionOptions = new ExecutionOptions();
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;
            int[] results;
            aeSession.SetAttributesToServer( new EnumObjectAttribute[] { EnumObjectAttribute.AESESSION_ALL }, out results, executionOptions);
        }       

        /// <summary>
        /// Change DaSession Properties
        /// </summary>
        private void ExecuteDaSessionProperties()
        {
            if (this.daSession == null) return;

            Helper.LocaleId localeId = new LocaleId();
            Helper.CustomProperties propServiceColl = propertyGrid.SelectedObject as Helper.CustomProperties;
            if (propServiceColl == null) return;

            string strLocalId = "";
            string strClassContext = "";
            string strClientName = "";
            string strOpcVersion = "";

            foreach (Helper.CustomProperty customProperty in propServiceColl.Properties)
            {
                if (customProperty == null) return;
                if (customProperty.Name.Equals("Locale Id"))
                {
                    strLocalId = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Client Name"))
                {
                    strClientName = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("DCOM Class Context"))
                {
                    strClassContext = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Forced OPC Version"))
                {
                    strOpcVersion = customProperty.DefaultValue.ToString();
                }
                
            }

            //set item new values
            daSession.LocaleId = strLocalId;
            daSession.ClientName = strClientName;
            daSession.ClassContext = GetClassContext(strClassContext);
            daSession.ForcedOpcSpecification = GetOPCVersion(strOpcVersion);

            ExecutionOptions executionOptions = new ExecutionOptions();
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;
            int[] results;
            daSession.SetAttributesToServer(new EnumObjectAttribute[] { EnumObjectAttribute.DASESSION_ALL }, out results, executionOptions);
            //set opc version
            foreach (Helper.CustomProperty customProperty in propServiceColl.Properties)
            {
                if (customProperty == null) return;
                if (customProperty.Name.Equals("OPC Version"))
                {
                    customProperty.DefaultValue = strOpcVersion;
                }
            }
            propertyGrid.SelectedObject = propServiceColl;
        }

        /// <summary>
        /// Get Context Class Object From String
        /// </summary>
        /// <param name="strClassContext"></param>
        /// <returns></returns>
        private EnumClassContext GetClassContext(string strClassContext)
        {
            switch (strClassContext)
            {
                case "INPROC_SERVER":
                    return EnumClassContext.INPROC_SERVER;
                case "LOCAL_SERVER":
                    return EnumClassContext.LOCAL_SERVER;
                case "NOT_COM":
                    return EnumClassContext.NOT_COM;
                case "REMOTE_SERVER":
                    return EnumClassContext.REMOTE_SERVER;
                case "SERVER":
                    return EnumClassContext.SERVER;
            }
            return EnumClassContext.NOT_COM;
        }


        /// <summary>
        /// Get OPC Version From String
        /// </summary>
        /// <param name="strClassContext"></param>
        /// <returns></returns>
        private EnumOPCSpecification GetOPCVersion(string strOpcVersion)
        {
            switch (strOpcVersion)
            {
                case "AE10":
                    return EnumOPCSpecification.AE10;
                case "DA10":
                    return EnumOPCSpecification.DA10;
                case "DA20":
                    return EnumOPCSpecification.DA20;
                case "DA30":
                    return EnumOPCSpecification.DA30;
                case "DEFAULT":
                    return EnumOPCSpecification.DEFAULT;
                case "XMLDA10":
                    return EnumOPCSpecification.XMLDA10;
            }
            return EnumOPCSpecification.DEFAULT;
        }

        #endregion

        #region Public Methods

        #region Constructor

        /// <summary>
        /// Property Window Constructor
        /// </summary>
        public PropertyWindow()
        {
            InitializeComponent();
        }

        #endregion

        #region DaProperty

        /// <summary>
        /// Display DaProperty
        /// </summary>
        /// <param name="property"></param>
        public void DaProperty(DaProperty property)
        {
            //display the propertyes
            panelBottom.Visible = false;

            Helper.CustomProperties propertiesColl = new DemoClient.Helper.CustomProperties();
            SetPropertyData(ref propertiesColl, property, property.Description);            
            propertyGrid.SelectedObject = propertiesColl;
        }

        #endregion

        #region AddressSpaceElement

        public void ReadAddressSpaceElemProperties(DaSession session, string itemId)
        {
            panelBottom.Visible = false;

            DaGetPropertiesOptions propertyGetOptions = new DaGetPropertiesOptions();
            propertyGetOptions.WhatPropertyData = EnumPropertyData.ALL;
            
            DaProperty[] properties = null;
            if (ResultCode.SUCCEEDED(session.GetDaProperties(itemId,
                                                                      null,
                                                                      propertyGetOptions,
                                                                      out properties,
                                                                      null)))
            {
                if (properties == null) return;

                Helper.CustomProperties propertiesColl = new DemoClient.Helper.CustomProperties();

                for (int i = 0; i < properties.Length; i++)
                {
                    SetPropertyData(ref propertiesColl,properties[i], String.Format("{0} - {1}",properties[i].Id,properties[i].Description));
                }
                propertyGrid.PropertySort = PropertySort.Categorized;                
                propertyGrid.SelectedObject = propertiesColl;
            }
        }

        #endregion

        #region DaSession

        /// <summary>
        /// DaSession Properties
        /// </summary>
        /// <param name="session"></param>
        internal bool DaSession(DaSession session)
        {
            panelBottom.Visible = true;

            this.daItem = null;
            daSubscription = null;
            daSession = session;
            aeSubscription = null;
            aeSession = null;

            Helper.DataTypes dataTypeHelper = new DataTypes();
            this.panelBottom.Visible = true;

            //DASession URL
            Helper.CustomProperties propServiceColl = new DemoClient.Helper.CustomProperties();
            Helper.CustomProperty customProperty = new DemoClient.Helper.CustomProperty("URL",
                    "System.String", "General", null, daSession.Url);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);


            //DaSession Locale ID
            ServerStatus status = null;
            if (ResultCode.SUCCEEDED(daSession.GetStatus(out status, null)))
            {
                LocaleId.SelectedItem = daSession.LocaleId;
                LocaleId.LocaleIdList = status.SupportedLcIds;
                customProperty = new DemoClient.Helper.CustomProperty("Locale Id",
                       "System.String", "General", null, daSession.LocaleId,
                     typeof(Helper.LCIDEditor), typeof(Helper.LCIDEditor));
            }
            else
            {
                customProperty = new DemoClient.Helper.CustomProperty("Locale Id",
                       "System.String", "General", null, daSession.LocaleId);
            }
            propServiceColl.Properties.Add(customProperty);

            //DaSession Client name
            customProperty = new DemoClient.Helper.CustomProperty("Client Name",
                 "System.String", "General", null, daSession.ClientName);
            propServiceColl.Properties.Add(customProperty);

            //DaSession OPC Version
            customProperty = new DemoClient.Helper.CustomProperty("OPC Version",
                    "System.String", "General", null, daSession.SupportedOpcSpecification);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            //DaSession Forced OPC Version
            customProperty = new DemoClient.Helper.CustomProperty("Forced OPC Version",
                    typeof(EnumOPCSpecification), "General", null, daSession.ForcedOpcSpecification);
            propServiceColl.Properties.Add(customProperty);

            
            //DaSession Class Context         
            EnumClassContext classContext = daSession.ClassContext;
            customProperty = new DemoClient.Helper.CustomProperty("DCOM Class Context",
                  typeof(EnumClassContext), "General", null, classContext);
            propServiceColl.Properties.Add(customProperty);

            if (status != null)
            {
                //AeSession Vendor info          
                customProperty = new DemoClient.Helper.CustomProperty("Vendor Info",
                       "System.String", "Status", null, status.VendorInfo);
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Version     
                customProperty = new DemoClient.Helper.CustomProperty("Version",
                       "System.String", "Status", null, status.ProductVersion);
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Start time     
                customProperty = new DemoClient.Helper.CustomProperty("Start Time",
                       "System.String", "Status", null, ValueConverter.DateTimeOptions(status.StartTime));
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Current time    
                customProperty = new DemoClient.Helper.CustomProperty("Current Time",
                       "System.String", "Status", null, ValueConverter.DateTimeOptions(status.CurrentTime));
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Last update time   
                customProperty = new DemoClient.Helper.CustomProperty("Last Update Time",
                       "System.String", "Status", null, ValueConverter.DateTimeOptions(status.LastUpdateTime));
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Status info 
                customProperty = new DemoClient.Helper.CustomProperty("Status Info",
                       "System.String", "Status", null, status.StatusInfo);
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Server state 
                customProperty = new DemoClient.Helper.CustomProperty("Server State",
                       "System.String", "Status", null, status.State);
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

            }

            this.propertyGrid.PropertySort = PropertySort.Categorized;
            this.propertyGrid.SelectedObject = propServiceColl;
            return true;   
        }

        #endregion

        #region AeSession

        /// <summary>
        /// AeSession Properties
        /// </summary>
        /// <param name="session"></param>
        internal bool AeSession(AeSession session)
        {
            panelBottom.Visible = true;

            this.daItem = null;
            daSubscription = null;
            daSession = null;
            aeSubscription = null;
            aeSession = session;

            Helper.DataTypes dataTypeHelper = new DataTypes();
            this.panelBottom.Visible = true;

            //AeSession URL
            Helper.CustomProperties propServiceColl = new DemoClient.Helper.CustomProperties();
            Helper.CustomProperty customProperty = new DemoClient.Helper.CustomProperty("URL",
                    "System.String", "General", null, aeSession.Url);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            
             //AeSession Locale Id
            ServerStatus status = null;
            if (ResultCode.SUCCEEDED(aeSession.GetStatus(out status, null)))
            {
                LocaleId.SelectedItem = aeSession.LocaleId;
                LocaleId.LocaleIdList = status.SupportedLcIds;
                customProperty = new DemoClient.Helper.CustomProperty("Locale Id",
                       "System.String", "General", null, aeSession.LocaleId,
                     typeof(Helper.LCIDEditor), typeof(Helper.LCIDEditor));
            }
            else
            {
                customProperty = new DemoClient.Helper.CustomProperty("Locale Id",
                       "System.String", "General", null, aeSession.LocaleId);
            }
            propServiceColl.Properties.Add(customProperty);


            //AeSession Client name
            customProperty = new DemoClient.Helper.CustomProperty("Client Name",
                 "System.String", "General", null, aeSession.ClientName);       
            propServiceColl.Properties.Add(customProperty);

            //AeSession Available filter criterias
            EnumFilterBy filters;
            if (ResultCode.SUCCEEDED(aeSession.QueryAvailableAeFilters(out filters, null)))
            {
                
                customProperty = new DemoClient.Helper.CustomProperty("Available Filter Criterias",
                    "System.String", "General", null, GetFilter(filters));
            }
            else
            {
                customProperty = new DemoClient.Helper.CustomProperty("Available Filter Criterias",
                      "System.String", "General", null, "");

            }
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            //AeSession Class Context         
            EnumClassContext classContext = aeSession.ClassContext;
            customProperty = new DemoClient.Helper.CustomProperty("DCOM Class Context",
                  typeof(EnumClassContext), "General", null, classContext);
            propServiceColl.Properties.Add(customProperty);

            if (status != null)
            {
                //AeSession Vendor info          
                customProperty = new DemoClient.Helper.CustomProperty("Vendor Info",
                       "System.String", "Status", null, status.VendorInfo);
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Version     
                customProperty = new DemoClient.Helper.CustomProperty("Version",
                       "System.String", "Status", null, status.ProductVersion);
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Start time     
                customProperty = new DemoClient.Helper.CustomProperty("Start Time",
                       "System.String", "Status", null, ValueConverter.DateTimeOptions(status.StartTime));
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Current time    
                customProperty = new DemoClient.Helper.CustomProperty("Current Time",
                       "System.String", "Status", null, ValueConverter.DateTimeOptions(status.CurrentTime));
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Last update time   
                customProperty = new DemoClient.Helper.CustomProperty("Last Update Time",
                       "System.String", "Status", null, ValueConverter.DateTimeOptions(status.LastUpdateTime));
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Status info 
                customProperty = new DemoClient.Helper.CustomProperty("Status Info",
                       "System.String", "Status", null, status.StatusInfo);
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

                //AeSession Server state 
                customProperty = new DemoClient.Helper.CustomProperty("Server State",
                       "System.String", "Status", null, status.State);
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                propServiceColl.Properties.Add(customProperty);

            }

            this.propertyGrid.PropertySort = PropertySort.Categorized;
            this.propertyGrid.SelectedObject = propServiceColl;
            return true;
        }

        /// <summary>
        /// Get the Filter
        /// </summary>
        /// <param name="filter"></param>
        /// <returns></returns>
        private string GetFilter(EnumFilterBy filter)
        {
            StringBuilder filterToString = new StringBuilder();
            if((filter & EnumFilterBy.AREA) == EnumFilterBy.AREA)
            {
                filterToString.Append("AREA   ");
            }
            if ((filter & EnumFilterBy.CATEGORY) == EnumFilterBy.CATEGORY)
            {
                filterToString.Append("CATEGORY  ");
            }
            if ((filter & EnumFilterBy.EVENT) == EnumFilterBy.EVENT)
            {
                filterToString.Append("EVENT  ");
            }
            if ((filter & EnumFilterBy.SEVERITY) == EnumFilterBy.SEVERITY)
            {
                filterToString.Append("SEVERITY  ");
            }
            if ((filter & EnumFilterBy.SOURCE) == EnumFilterBy.SOURCE)
            {
                filterToString.Append("SOURCE  ");
            }
            return filterToString.ToString();
        }

        #endregion

        #region AeSubscription

        /// <summary>
        /// AeSubscription Properties
        /// </summary>
        /// <param name="subscription"></param>
        internal bool AeSubscription(AeSubscription subscription)
        {
            panelBottom.Visible = true;

            this.daItem = null;
            aeSubscription = subscription;
            daSession = null;
            daSubscription = null;
            aeSession = null;

            Helper.DataTypes dataTypeHelper = new DataTypes();
            this.panelBottom.Visible = true;

            //AeSubscription Buffer Time
            Helper.CustomProperties propServiceColl = new DemoClient.Helper.CustomProperties();
            Helper.CustomProperty customProperty = new DemoClient.Helper.CustomProperty("Buffer Time",
                    "System.String", "General", null, aeSubscription.RequestedBufferTime);
            propServiceColl.Properties.Add(customProperty);

            //AeSubscription Revised Buffer Time
            customProperty = new DemoClient.Helper.CustomProperty("Revised Buffer Time",
                    "System.String", "General", null, aeSubscription.RevisedBufferTime);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            //AeSubscription Max Size
            customProperty = new DemoClient.Helper.CustomProperty("Max Size",
                 "System.String", "General", null, aeSubscription.RequestedMaxSize);         
            propServiceColl.Properties.Add(customProperty);

            //AeSubscription Revised Max Size
            customProperty = new DemoClient.Helper.CustomProperty("Revised Max Size",
                  "System.String", "General", null, aeSubscription.RevisedMaxSize);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);           

            this.propertyGrid.PropertySort = PropertySort.Categorized;
            this.propertyGrid.SelectedObject = propServiceColl;
            return true;
        }        

        #endregion

        #region DaSubscription

        /// <summary>
        /// DaSubscription Properties
        /// </summary>
        /// <param name="subscription"></param>
        internal bool DaSubscription(DaSubscription subscription)
        {
            panelBottom.Visible = true;

            this.daItem = null;
            daSubscription = subscription;
            daSession = null;
            aeSubscription = null;
            aeSession = null;

            Helper.DataTypes dataTypeHelper = new DataTypes();
            this.panelBottom.Visible = true;

            //DaSubscription Name
            Helper.CustomProperties propServiceColl = new DemoClient.Helper.CustomProperties();
            Helper.CustomProperty customProperty = new DemoClient.Helper.CustomProperty("Name",
                    "System.String", "General", null, daSubscription.Name);
            propServiceColl.Properties.Add(customProperty);

            //DaSubscription Update Rate
            customProperty = new DemoClient.Helper.CustomProperty("Update Rate(ms)",
                    "System.String", "General", null, daSubscription.RequestedUpdateRate);
            propServiceColl.Properties.Add(customProperty);

            //DaSubscription Revised Update Rate
            customProperty = new DemoClient.Helper.CustomProperty("Revised Update Rate(ms)",
                 "System.String", "General", null, daSubscription.RevisedUpdateRate);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            //DaSubscription Deadband
            customProperty = new DemoClient.Helper.CustomProperty("Deadband",
                  "System.String", "General", null, daSubscription.Deadband.ToString());
            propServiceColl.Properties.Add(customProperty);

            //DaSubscription Time Bias
            customProperty = new DemoClient.Helper.CustomProperty("Time Bias(min)",
                  "System.String", "General", null, daSubscription.TimeBias);
            propServiceColl.Properties.Add(customProperty);

            //DaSubscription Locale Id       
            ServerStatus status;
            if (ResultCode.SUCCEEDED(daSubscription.DaSession.GetStatus(out status, null)))
            {
                LocaleId.SelectedItem = daSubscription.LocaleId;
                LocaleId.LocaleIdList = status.SupportedLcIds;
                customProperty = new DemoClient.Helper.CustomProperty("Locale Id",
                       "System.String", "General", null, daSubscription.LocaleId,
                     typeof(Helper.LCIDEditor), typeof(Helper.LCIDEditor));
            }
            else
            {
                customProperty = new DemoClient.Helper.CustomProperty("Locale Id",
                       "System.String", "General", null, daSubscription.LocaleId);
            }
            propServiceColl.Properties.Add(customProperty);
            

            //DaSubscription Revised Keep Alive Time
            customProperty = new DemoClient.Helper.CustomProperty("Keep Alive Time(ms)",
                  "System.String", "General", null, daSubscription.RequestedKeepAliveTime);
            propServiceColl.Properties.Add(customProperty);

            customProperty = new DemoClient.Helper.CustomProperty("Revised Keep Alive Time(ms)",
                  "System.String", "General", null, daSubscription.RevisedKeepAliveTime);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            this.propertyGrid.PropertySort = PropertySort.Categorized;
            this.propertyGrid.SelectedObject = propServiceColl;
            return true;
        }

        #endregion

        #region DaItem

        /// <summary>
        /// Show DaItem Properties
        /// </summary>
        /// <param name="daItem"></param>
        internal bool DaItem(DaItem item)
        {
            panelBottom.Visible = true;

            this.daItem = item;
            daSubscription = null;
            daSession = null;
            aeSubscription = null;
            aeSession = null;

            Helper.DataTypes dataTypeHelper = new DataTypes();
            this.panelBottom.Visible = true;

            //Item Id
            Helper.CustomProperties propServiceColl = new DemoClient.Helper.CustomProperties();
            Helper.CustomProperty customProperty = new DemoClient.Helper.CustomProperty("Item Id",
                    "System.String", "General", null, daItem.Id);
            propServiceColl.Properties.Add(customProperty);

            //Item Path
            customProperty = new DemoClient.Helper.CustomProperty("Item Path",
                    "System.String", "General", null, daItem.Path);
            propServiceColl.Properties.Add(customProperty);

            //Requested Data Type
            string dataType = daItem.NativeDatatype.FullName;
            if (dataType.Contains("*"))
            {
                dataType = dataType.Remove(dataType.IndexOf('*'));
            }
            dataTypeHelper.Type = System.Type.GetType(dataType);
            customProperty = new DemoClient.Helper.CustomProperty("Requested Data Type",
                 dataTypeHelper.RequiredDataTypes.Values.GetType().ToString(), "General", null, dataTypeHelper.NativeDataType,
                 typeof(Helper.DataTypeEditor), typeof(Helper.DataTypeEditor));
            propServiceColl.Properties.Add(customProperty);

            //Deadband
            customProperty = new DemoClient.Helper.CustomProperty("Deadband",
                  "System.String", "General", null, daItem.Deadband.ToString());
            propServiceColl.Properties.Add(customProperty);

            //Native Data Type
            customProperty = new DemoClient.Helper.CustomProperty("Native Data Type",
                  "System.String", "General", null, dataTypeHelper.NativeDataType);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            //Access Rights
            string strAccesRights = "";
            switch (daItem.AccessRights)
            {
                case EnumAccessRights.READABLE:
                    strAccesRights = "read";
                    break;
                case EnumAccessRights.READWRITEABLE:
                    strAccesRights = "read and write";
                    break;
                case EnumAccessRights.WRITEABLE:
                    strAccesRights = "write";
                    break;
            }
            customProperty = new DemoClient.Helper.CustomProperty("Acces Rights",
                  "System.String", "General", null, strAccesRights);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            //Engineering Units Type
            string strEUType = "";
            switch (daItem.EUType)
            {
                case EnumEUType.ANALOG:
                    strEUType = "analog";
                    break;
                case EnumEUType.ENUMERATED:
                    strEUType = "enumerated";
                    break;
                case EnumEUType.NOENUM:
                    strEUType = "no";
                    break;
            }
            customProperty = new DemoClient.Helper.CustomProperty("EU Type",
                  "System.String", "General", null, strEUType);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            //Engeneering Units Info
            string euInfo = "";
            if (daItem.EUInfo != null)
            {
                euInfo = daItem.EUInfo.ToString();
            }
            customProperty = new DemoClient.Helper.CustomProperty("EU Info",
                  "System.String", "General", null, euInfo);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            propServiceColl.Properties.Add(customProperty);

            this.propertyGrid.SelectedObject = propServiceColl;
            return true;
        }        

        #endregion       

        #endregion


        #region Event Handlers

        private void linkLabelExecute_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            if (daItem != null)
            {
                ExecuteDaItemProperties();
                MainWindow.AnyChange = true;
                return;
            }
            if (daSubscription != null)
            {
                ExecuteDaSubscriptionProperties();
                MainWindow.AnyChange = true;
                return;
            }
            if (daSession != null)
            {
                ExecuteDaSessionProperties();
                MainWindow.AnyChange = true;
                return;
            }
            if (aeSubscription != null)
            {
                ExecuteAeSubscriptionProperties();
                MainWindow.AnyChange = true;
                return;
            }
            if (aeSession != null)
            {
                ExecuteAeSessionProperties();
                MainWindow.AnyChange = true;
                return;
            }
            
        }

        #endregion
    }
}
