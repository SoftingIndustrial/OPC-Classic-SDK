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
    public partial class ServiceWindow : ToolWindow
    {

        #region Constants

        public const uint MAX_AGE_DEVICE = 0;

        #endregion

        #region Private Members

        private DaItem item = null;

        private DaSession session = null;

        //flag set to true if the window is dealing with a DaItem
        private bool isDaItem = false;


        #endregion

        #region Private and Protected Methods

        /// <summary>
        /// Fill Read Service Data
        /// </summary>
        /// <param name="itemId"></param>
        /// <param name="hasReadRights"></param>
        /// <param name="itemValues"></param>
        /// <returns></returns>
        private Helper.CustomProperties FillReadService(string itemId, bool hasReadRights, ValueQT itemValues)
        {
            //display the read values
            Helper.CustomProperties readServiceColl = new DemoClient.Helper.CustomProperties();
            Helper.CustomProperty customProperty = new DemoClient.Helper.CustomProperty("Item Id",
                    "System.String", null, null, itemId);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            readServiceColl.Properties.Add(customProperty);
            if (hasReadRights && itemValues != null)
            {
                customProperty = new DemoClient.Helper.CustomProperty("Value",
                     "System.String", null, null, ValueConverter.ArrayToString(itemValues));
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                readServiceColl.Properties.Add(customProperty);
                customProperty = new DemoClient.Helper.CustomProperty("Quality",
                    "System.String", null, null, itemValues.Quality.ToString());
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                readServiceColl.Properties.Add(customProperty);
                customProperty = new DemoClient.Helper.CustomProperty("Time Stamp",
                    "System.String", null, null, ValueConverter.DateTimeOptions(itemValues.TimeStamp));
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                readServiceColl.Properties.Add(customProperty);
            }
            else
            {
                //there are no read values, fiil data with empty strings
                customProperty = new DemoClient.Helper.CustomProperty("Value",
                     "System.String", null, null, "");
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                readServiceColl.Properties.Add(customProperty);
                customProperty = new DemoClient.Helper.CustomProperty("Quality",
                    "System.String", null, null, "");
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                readServiceColl.Properties.Add(customProperty);
                customProperty = new DemoClient.Helper.CustomProperty("Time Stamp",
                    "System.String", null, null, "");
                customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                readServiceColl.Properties.Add(customProperty);
            }
            return readServiceColl;

            
        }

        /// <summary>
        /// Fill Empty Data
        /// </summary>
        /// <param name="itemId"></param>
        private Helper.CustomProperties FillEmptyValueQT(string itemId, Type itemDataType)
        {
            Helper.DataTypes dataTypeHelper = new DataTypes();

            Helper.CustomProperties writeServiceColl = new DemoClient.Helper.CustomProperties();
            Helper.CustomProperty customProperty = new DemoClient.Helper.CustomProperty("Item Id",
                    "System.String", null, null, itemId);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            writeServiceColl.Properties.Add(customProperty);

            //the DaItem VALUE is not an array
            customProperty = new DemoClient.Helper.CustomProperty("Value",
                 "System.String", null, null, "");
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            writeServiceColl.Properties.Add(customProperty);

            if (itemDataType == null)
            {
                customProperty = new DemoClient.Helper.CustomProperty("Data Type",
                     dataTypeHelper.RequiredDataTypes.Values.GetType().ToString(), null, null, "",
                     typeof(Helper.DataTypeEditor), typeof(Helper.DataTypeEditor));
            }
            else
            {
                dataTypeHelper.Type = itemDataType;
                customProperty = new DemoClient.Helper.CustomProperty("Data Type",
                     dataTypeHelper.RequiredDataTypes.Values.GetType().ToString(), null, null, dataTypeHelper.NativeDataType,
                     typeof(Helper.DataTypeEditor), typeof(Helper.DataTypeEditor));
            }
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            writeServiceColl.Properties.Add(customProperty);

            customProperty = new DemoClient.Helper.CustomProperty("Start Index",
                 "System.String", null, null, "");
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            writeServiceColl.Properties.Add(customProperty);

            return writeServiceColl;
        }

        /// <summary>
        /// Fill Write Service
        /// </summary>
        /// <param name="itemId"></param>
        /// <param name="hasWriteAccess"></param>
        /// <param name="itemValue"></param>
        private Helper.CustomProperties FillWriteService(string itemId, Type itemDataType, bool hasWriteAccess, ValueQT itemValues)
        {
            Helper.DataTypes dataTypeHelper = new DataTypes();

            Helper.CustomProperties writeServiceColl = new DemoClient.Helper.CustomProperties();
            Helper.CustomProperty customProperty = new DemoClient.Helper.CustomProperty("Item Id",
                    "System.String", null, null, itemId);
            customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
            writeServiceColl.Properties.Add(customProperty);

            //create the DaItem value
            if (!itemValues.Data.GetType().IsArray)
            {
                //the DaItem VALUE is not an array
                customProperty = new DemoClient.Helper.CustomProperty("Value",
                     "System.String", null, null, ValueConverter.ArrayToString(itemValues));
                if (!hasWriteAccess)
                {
                    customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                }
                writeServiceColl.Properties.Add(customProperty);

                dataTypeHelper.Type = itemDataType;
                customProperty = new DemoClient.Helper.CustomProperty("Data Type",
                     dataTypeHelper.RequiredDataTypes.Values.GetType().ToString(), null, null, dataTypeHelper.NativeDataType,
                     typeof(Helper.DataTypeEditor), typeof(Helper.DataTypeEditor));
                if (!hasWriteAccess)
                {
                    customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                }
                writeServiceColl.Properties.Add(customProperty);

                customProperty = new DemoClient.Helper.CustomProperty("Start Index",
                     "System.String", null, null, "");
                if (!hasWriteAccess)
                {
                    customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                }
                writeServiceColl.Properties.Add(customProperty);
            }
            else
            {
                //DaItem VALUE is an Array
                customProperty = new DemoClient.Helper.CustomProperty("Value",
                     "System.String", null, null, ValueConverter.DataToString(itemValues));
                if (!hasWriteAccess)
                {
                    customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                }
                writeServiceColl.Properties.Add(customProperty);

                Array dataArray = itemValues.Data as Array;

                if (dataArray.GetLowerBound(0) == 0)
                {
                    dataTypeHelper.Type = itemDataType;
                    customProperty = new DemoClient.Helper.CustomProperty("Data Type",
                         dataTypeHelper.RequiredDataTypes.Values.GetType().ToString(), null, null, dataTypeHelper.NativeDataType,
                         typeof(Helper.DataTypeEditor), typeof(Helper.DataTypeEditor));
                }
                else
                {
                    string dataType = dataArray.GetType().ToString();
                    dataType = dataType.Remove(dataType.IndexOf('*'));
                    dataTypeHelper.Type = System.Type.GetType(dataType);
                    customProperty = new DemoClient.Helper.CustomProperty("Data Type",
                         dataTypeHelper.RequiredDataTypes.Values.GetType().ToString(), null, null, dataTypeHelper.NativeDataType,
                         typeof(Helper.DataTypeEditor), typeof(Helper.DataTypeEditor));
                }
                if (!hasWriteAccess)
                {
                    customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                }
                writeServiceColl.Properties.Add(customProperty);

                customProperty = new DemoClient.Helper.CustomProperty("Start Index",
                     "System.Int32", null, null, dataArray.GetLowerBound(0));
                if (!hasWriteAccess)
                {
                    customProperty.Attributes = new Attribute[] {
												ReadOnlyAttribute.Yes
											};
                }
                writeServiceColl.Properties.Add(customProperty);                
            }
            return writeServiceColl;
        }

        #endregion

        #region Public Methods

        #region Constructor

        public ServiceWindow()
        {
            InitializeComponent();
        }

        #endregion

        #region Read DaItem

        /// <summary>
        /// Fill ReadDaItem Window with data
        /// </summary>
        /// <param name="daItem"></param>
        /// <param name="itemValues"></param>
        /// <returns></returns>
        public bool ReadDaItem(DaItem daItem, out  ValueQT itemValues)
        {
            //read the item values
            this.panelBottom.Visible = false; 
            isDaItem = true;
     

            int itemResult;
            int result = daItem.Read(100, out itemValues, out itemResult, new ExecutionOptions());
            if (ResultCode.SUCCEEDED(result) && ResultCode.SUCCEEDED(itemResult))
            {
                bool hasReadRights = false;
                if(daItem.AccessRights != EnumAccessRights.WRITEABLE)
                {
                    hasReadRights = true;
                }
                this.propertyGrid.SelectedObject = FillReadService(daItem.Id, hasReadRights, itemValues);
                return true;
            }
            return false;
        }

        #endregion

        #region Write DaItem

        /// <summary>
        /// Fill WriteDa Service Window with data
        /// </summary>
        /// <param name="daItem"></param>
        /// <returns></returns>
        public bool WriteDaItem(DaItem daItem)
        {
            //Set DaItem for write            
            this.panelBottom.Visible = true;
            this.item = daItem;
            isDaItem = true;

            //read the items values
            ValueQT itemValues;
            int itemResult;
            int result = daItem.Read(100, out itemValues, out itemResult, new ExecutionOptions());
            if (ResultCode.SUCCEEDED(result) && ResultCode.SUCCEEDED(itemResult))
            {
                //the item value is null
                if (itemValues == null)
                {
                    this.propertyGrid.SelectedObject = FillEmptyValueQT(daItem.Id, daItem.NativeDatatype);
                    return false;
                }
                bool hasWriteAccess = true;
                if(daItem.AccessRights == EnumAccessRights.READABLE)
                {
                    hasWriteAccess = false;
                }
                this.propertyGrid.SelectedObject = FillWriteService(daItem.Id, daItem.NativeDatatype, hasWriteAccess, itemValues);
                return true;
            }
            return false;            
        }

        /// <summary>
        /// Execute Write Item
        /// </summary>
        private void ExecuteWriteItem()
        {
            //Get the new values
            Helper.DataTypes dataTypeHelper = new DataTypes();            
            Helper.CustomProperties writeServiceColl = propertyGrid.SelectedObject as Helper.CustomProperties;
            if (writeServiceColl == null) return;
            string strItemId = "";
            string strData = "";
            string strDataType = "";
            string strStartIndex = "";
            foreach (Helper.CustomProperty customProperty in writeServiceColl.Properties)
            {
                if (customProperty == null) return;
                if (customProperty.Name.Equals("Item Id"))
                {
                    strItemId = customProperty.DefaultValue.ToString();
                }
                if (customProperty.Name.Equals("Value"))
                {
                    strData = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Data Type"))
                {
                    strDataType = customProperty.DefaultValue.ToString();
                }
                else if (customProperty.Name.Equals("Start Index"))
                {
                    strStartIndex = customProperty.DefaultValue.ToString();
                }
            }

            //Create the new ValueQT
            ValueQT itemValue = new ValueQT();
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
            if (!dataType.IsArray)
            {
                //data type is not array
                object objData = null;
                ConvertToType(dataType, out objData, strData);
                itemValue.SetData(objData, EnumQuality.GOOD, DateTime.Now);
                if (item != null)
                {
                    SetWriteItemToDaItem(itemValue, dataType);
                }
                else if (item == null )
                {
                    SetWriteItemToAddSpaceElem(strItemId, itemValue, dataType);
                }
            }
            else
            {
                //data type is array
                string typeStr = dataType.FullName;
                Type arrayType = System.Type.GetType(typeStr.Substring(0, typeStr.IndexOf('[')));
                int startIndex = 0;
                if (int.TryParse(strStartIndex, out startIndex))
                {
                    string[] objDatas = strData.Trim().Split(';');
                    if (objDatas == null) return;
                    Array array = null;
                    if (startIndex > 0)
                    {
                        //non-zero bound array
                        array = Array.CreateInstance(arrayType, new int[] { objDatas.Length }, new int[] { startIndex });
                    }
                    else
                    {
                        //zero boud array
                        array = Array.CreateInstance(arrayType, objDatas.Length);
                    }
                    int strIndex = 0;
                    for (int index = array.GetLowerBound(0);
                                    index <= array.GetUpperBound(0);
                                    index++)
                    {
                        object objData = null;
                        ConvertToType(arrayType, out objData, objDatas[strIndex]);
                        array.SetValue(objData, index);
                        strIndex++;
                    }
                    itemValue.SetData(array, EnumQuality.GOOD, DateTime.Now);
                    if (item != null)
                    {                        
                        SetWriteItemToDaItem(itemValue, dataType);
                    }
                    else if (item == null)
                    {
                        SetWriteItemToAddSpaceElem(strItemId,itemValue, dataType);
                    }
                }
            }
        }

        /// <summary>
        /// Write DaItem
        /// </summary>
        /// <param name="itemValue"></param>
        /// <param name="dataType"></param>
        private void SetWriteItemToDaItem(ValueQT itemValue, Type dataType)
        {
            //Write DaItem Values
            ExecutionOptions executionOptions = new ExecutionOptions();
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            int result;
            item.RequestedDatatype = dataType;
            this.item.Write(itemValue, out result, executionOptions);
        }

        /// <summary>
        /// Convert the inputDaa to the its corresponding value type
        /// </summary>
        /// <param name="dataType"></param>
        /// <param name="objData"></param>
        /// <param name="strData"></param>
        private void ConvertToType(System.Type dataType, out object objData, string strData)
        {
            objData = null;
            switch (dataType.FullName)
                {
                    case "System.SByte":
                        {
                            SByte sByteValue = new sbyte();
                            if (SByte.TryParse(strData, out sByteValue))
                            {
                                objData = sByteValue;
                            }
                        }
                        break;
                    case "System.Byte":
                        {
                            Byte byteValue = new byte();
                            if (Byte.TryParse(strData, out byteValue))
                            {
                                objData = byteValue;
                            }
                        }
                        break;
                    case "System.Int16":
                        {
                            Int16 int16Value = new Int16();
                            if (Int16.TryParse(strData, out int16Value))
                            {
                                objData = int16Value;
                            }
                        }
                        break;
                    case "System.UInt16":
                        {
                            UInt16 uInt16Value = new UInt16();
                            if (UInt16.TryParse(strData, out uInt16Value))
                            {
                                objData = uInt16Value;
                            }
                        }
                        break;
                    case "System.Int32":
                        {
                            Int32 int32Value = new Int32();
                            if (Int32.TryParse(strData, out int32Value))
                            {
                                objData = int32Value;
                            }

                        }
                        break;
                    case "System.UInt32":
                        {
                            UInt32 uInt32Value = new UInt32();
                            if (UInt32.TryParse(strData, out uInt32Value))
                            {
                                objData = uInt32Value;
                            }

                        }
                        break;
                    case "System.Int64":
                        {
                            Int64 int64Value = new Int64();
                            if (Int64.TryParse(strData, out int64Value))
                            {
                                objData = int64Value;
                            }

                        }
                        break;
                    case "System.UInt64":
                        {
                            UInt64 uInt64Value = new UInt64();
                            if (UInt64.TryParse(strData, out uInt64Value))
                            {
                                objData = uInt64Value;
                            }

                        }
                        break;
                    case "System.Single":
                        {
                            Single singleValue = new Single();
                            if (Single.TryParse(strData, out singleValue))
                            {
                                objData = singleValue;
                            }
                        }
                        break;
                    case "System.Double":
                        {
                            Double doubleValue = new Double();
                            if (Double.TryParse(strData, out doubleValue))
                            {
                                objData = doubleValue;
                            }
                        }
                        break;
                    case "System.Decimal":
                        {
                            Decimal decimalValue = new Decimal();
                            if (Decimal.TryParse(strData, out decimalValue))
                            {
                                objData = decimalValue;
                            }
                        }
                        break;
                    case "System.Boolean":
                        {
                            Boolean boolValue = new Boolean();
                            if (Boolean.TryParse(strData, out boolValue))
                            {
                                objData = boolValue;
                            }
                        }
                        break;
                    case "System.String":
                        {
                            objData = strData;
                        }
                        break;
                    case "System.DateTime":
                        {
                            DateTime dateTimeValue = new DateTime();
                            if (DateTime.TryParse(strData, out dateTimeValue))
                            {
                                objData = dateTimeValue;
                            }
                        }
                        break;
                }
        }

        #endregion  

        #region Read/Write Address Space Element

        /// <summary>
        /// Read the Address Space ValueQT's
        /// </summary>
        /// <param name="session"></param>
        /// <param name="itemId"></param>
        /// <param name="itemValues"></param>
        /// <returns></returns>
        public bool ReadAddresSpaceItem(DaSession session, string itemId, out ValueQT[] itemValues)
        {
            //read the item values
            this.panelBottom.Visible = false; 
            isDaItem = false;

            int[] itemResults;            
            int result = session.Read(MAX_AGE_DEVICE, new string[] { itemId }, null, out itemValues, out itemResults, null);
            if (itemResults.Length == 0) return false;
            if (ResultCode.SUCCEEDED(result) && ResultCode.SUCCEEDED(itemResults[0]))
            {
                if (itemValues == null) return false;
                if (itemValues.Length == 0) return false;
                this.propertyGrid.SelectedObject = FillReadService(itemId, true, itemValues[0]);
                return true;
            }
            return false;
        }

        /// <summary>
        /// Write Address Space Element
        /// </summary>
        /// <param name="session"></param>
        /// <param name="itemId"></param>
        public void WriteAddresSpaceItem(DaSession session, string itemId, object selectedObject, bool isAddSpaceElem)
        {
            Helper.DataTypes dataTypeHelper = new DataTypes();
            this.panelBottom.Visible = true;
            
            isDaItem = false;            
            this.session = session;

            ValueQT[] itemValues;
            int[] itemResults;            
            int result = session.Read(MAX_AGE_DEVICE, new string[] { itemId }, null, out itemValues, out itemResults, null);
            if (itemResults.Length == 0) return ;
            if (itemValues.Length == 0) return;

            Type itemDataType = typeof(short);
            EnumAccessRights itemAccessRighs = EnumAccessRights.READWRITEABLE;
            bool hasWriteAccess = true;
            if (ResultCode.SUCCEEDED(result) && ResultCode.SUCCEEDED(itemResults[0]))
            {

                DaGetPropertiesOptions propertyGetOptions = new DaGetPropertiesOptions();
                propertyGetOptions.WhatPropertyData = EnumPropertyData.ALL;

                DaProperty[] properties = null;
                if (isAddSpaceElem)
                {
                    if (ResultCode.SUCCEEDED(session.GetDaProperties(itemId,
                                                                              null,
                                                                              propertyGetOptions,
                                                                              out properties,
                                                                              null)))
                    {
                        if (properties == null)
                        {
                            this.propertyGrid.SelectedObject = FillEmptyValueQT(itemId, null);
                            return;
                        }
                        if (properties.Length < 5)
                        {
                            this.propertyGrid.SelectedObject = FillEmptyValueQT(itemId, null);
                            return;
                        }
                        itemDataType = DataTypeConverter.GetSysType((short)properties[0].ValueQT.Data);
                        itemAccessRighs = ObjToAccesssRights(properties[4].ValueQT.Data);
                    }
                }
                else
                {
                    itemDataType = (selectedObject as DaProperty).DataType;
                }

                //the item value is null
                if (itemValues[0] == null)
                {
                    this.propertyGrid.SelectedObject = FillEmptyValueQT(itemId, itemDataType);
                    return;
                }
               
                if (itemAccessRighs == EnumAccessRights.READABLE)
                {
                    hasWriteAccess = false;
                }
            }
            this.propertyGrid.SelectedObject = FillWriteService(itemId, itemDataType, hasWriteAccess, itemValues[0]);     
 
        }

        /// <summary>
        /// Get 
        /// </summary>
        /// <param name="strAccessRights"></param>
        /// <returns></returns>
        private EnumAccessRights ObjToAccesssRights(object objAccessRights)
        {
            switch ((int)objAccessRights)
            {
                case 1:
                    return EnumAccessRights.READABLE;
                case 2:
                    return EnumAccessRights.WRITEABLE;
                case 3:
                    return EnumAccessRights.READWRITEABLE;
            }
            return EnumAccessRights.READWRITEABLE;
        }

        /// <summary>
        /// Set Write AddressSpaceElement
        /// </summary>
        /// <param name="itemId"></param>
        /// <param name="itemValue"></param>
        /// <param name="dataType"></param>
        private void SetWriteItemToAddSpaceElem(string itemId, ValueQT itemValue, Type dataType)
        {
            if (session == null) return;
             //Write DaItem Values
            ExecutionOptions executionOptions = new ExecutionOptions();
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            int[] result;
            session.Write(new string[] { itemId }, null, new ValueQT[] { itemValue }, out result, executionOptions);
        }

        #endregion

        #endregion

        #region Event Handlers

        /// <summary>
        /// Write DaItem
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void linkLabelExecute_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            ExecuteWriteItem();
            MainWindow.AnyChange = true;
        }

        #endregion
      
    }
}
