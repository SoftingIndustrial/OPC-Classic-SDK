using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.Collections;
using System.IO;
using DemoClient.DockWindows;
using DemoClient.Helper;

namespace DemoClient.TabControls
{
    /// <summary>
    /// Hanlder called when a new item is added
    /// </summary>
    /// <param name="itemName"></param>
    public delegate void AddDaItemHandler(string itemName, DaSubscription subscription);
    public delegate void AddDaItemsHandler(string itemName, DaSubscription subscription);

    public partial class AddressSpaceControl : UserControl
    {
        #region Constants 

        private const int BROWSE_INDEX = 0;
        private const int NODE_INDEX = 1;
        private const int TAG_INDEX = 2;
        private const int PROP_INDEX = 3;

        private const string AREA_SPACE = " - Area Space";
        private const string EVENT_CATEGORIES = " - Event Categories";

        #endregion

        #region Private Members

        ExecutionOptions executionOptions = new ExecutionOptions();        

        //save dialog window
        SaveFileDialog saveDlg = new SaveFileDialog();

        private bool canExpand = true;
        private bool canCollapse = true;

        private bool isPropertiesEnabled = false;

        private DaAddressSpaceElement daRootSpaceElem = null;

        #endregion

        #region Properties

        // selected daSubscription        
        DaSubscription daSubscription = null;
        public DaSubscription DaSubscription
        {
            get { return daSubscription; }
            set { daSubscription = value; }
        }

        private ServiceWindow serviceWindow = null;
        public ServiceWindow ServiceWindow
        {
            get { return serviceWindow; }
            set { serviceWindow = value; }
        }

        private PropertyWindow propertyWindow = null;
        public PropertyWindow PropertyWindow
        {
            get { return propertyWindow; }
            set { propertyWindow = value; }
        }

        private WeifenLuo.WinFormsUI.Docking.DockPanel dockPanel;
        public WeifenLuo.WinFormsUI.Docking.DockPanel MainDockPanel
        {
            get { return dockPanel; }
            set { dockPanel = value; }
        }

        #endregion

        #region Delegates

        //event handler on item added
        public event AddDaItemHandler AddDaItem;
        public event AddDaItemsHandler AddDaItems;

        public event EnableButtonsHandler EnableButtons;


        #endregion

        #region Private and Protected Methods

        /// <summary>
        /// Fill Browse data
        /// </summary>
        /// <param name="treeNode"></param>
        private void ExpandTreeNode(TreeNode treeNode, TreeNode rootNode)
        { 
            if (rootNode == null) return;
            if (rootNode.Tag == null) return;

            this.browseTreeView.Cursor = Cursors.WaitCursor;
            if (Type.ReferenceEquals(rootNode.Tag.GetType(), typeof(DaSession)))
            {               
                GetDaData(treeNode,rootNode, rootNode.Tag);
            }
            else if (Type.ReferenceEquals(rootNode.Tag.GetType(), typeof(AeSession)))
            {
                AeSession session = rootNode.Tag as AeSession;
                if (session == null || treeNode.Tag ==null)
                {
                    this.browseTreeView.Cursor = Cursors.Default;
                    return;
                }
                if (treeNode == browseTreeView.Nodes[1])
                {                    
                    GetAeEventCategoriesData(treeNode, session);
                }
                else
                {                   
                    GetAeAreaSpaceData(treeNode, session);
                }
            }
            this.browseTreeView.Cursor = Cursors.Default;
        }       

        /// <summary>
        /// Get Da BrowseData
        /// </summary>
        private void GetDaData(TreeNode node, TreeNode rootNode, object daSessionObject)
        {
            if (node.ImageIndex == TAG_INDEX) return;
            DaSession session = daSessionObject as DaSession;
            if (session == null) return;
            if (daRootSpaceElem == null) return;
           
            DaAddressSpaceElement[] addressSpaceElements = null;

            //get branches
            DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
            browseOptions.RetrieveItemId = true;
           
            DaAddressSpaceElement currentAddSpaceElem = null;
            if (node.Tag == null) return;
            if (node.Tag is DaSession)
            {
                currentAddSpaceElem = daRootSpaceElem;
            }
            if (node.Tag is DaAddressSpaceElement)
            {
                currentAddSpaceElem = node.Tag as DaAddressSpaceElement;
            }
            if(currentAddSpaceElem != null)
            {
                browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;
                if (ResultCode.SUCCEEDED(currentAddSpaceElem.Browse(browseOptions,
                                                                    out addressSpaceElements,
                                                                    executionOptions)))
                {
                    node.Nodes.Clear();
                    for (int i = 0; i < addressSpaceElements.Length; i++)
                    {
                        TreeNode newNode = new TreeNode(addressSpaceElements[i].Name, NODE_INDEX, NODE_INDEX);
                        newNode.Name = addressSpaceElements[i].Name;
                        newNode.Tag = addressSpaceElements[i];
                        node.Nodes.Add(newNode);
                        newNode.Nodes.Add(new TreeNode(""));
                    }

                }
                //get the items
                //browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.DA_ITEM;
                //GetTagNode(browseOptions, currentAddSpaceElem, node);

                //get leaves
                browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.LEAF;
                GetTagNode(browseOptions, currentAddSpaceElem,node);
            }
        }

        /// <summary>
        /// Get Da_Items or Leafs, according to the browse options
        /// </summary>
        /// <param name="browseOptions"></param>
        /// <param name="addrSpaceElem"></param>
        /// <param name="node"></param>
        /// <param name="session"></param>
        private void GetTagNode(DaAddressSpaceElementBrowseOptions browseOptions, DaAddressSpaceElement addrSpaceElem, TreeNode node)
        {
            DaGetPropertiesOptions propertyGetOptions = new DaGetPropertiesOptions();
            propertyGetOptions.WhatPropertyData = EnumPropertyData.ALL;

            DaAddressSpaceElement[] addressSpaceElements = null;
            if (ResultCode.SUCCEEDED(addrSpaceElem.Browse(browseOptions,
                                                                 out addressSpaceElements,
                                                                 executionOptions)))
            {
                for (int i = 0; i < addressSpaceElements.Length; i++)
                {
                    TreeNode newNode = new TreeNode(addressSpaceElements[i].Name, TAG_INDEX, TAG_INDEX);
                    newNode.Name = addressSpaceElements[i].Name;
                    newNode.Tag = addressSpaceElements[i];
                    node.Nodes.Add(newNode);

                    //get properties
                    AddProperties(addressSpaceElements[i], propertyGetOptions, newNode);
                }
                AddProperties(addrSpaceElem, propertyGetOptions, node);
            }
 
        }

        /// <summary>
        /// Get Properties
        /// </summary>
        /// <param name="session"></param>
        /// <param name="itemId"></param>
        /// <param name="node"></param>
        private void AddProperties(DaAddressSpaceElement addSpaceElem, DaGetPropertiesOptions propertyGetOptions, TreeNode node)
        {
            //get properties
            DaProperty[] properties = null;
            if (ResultCode.SUCCEEDED(addSpaceElem.GetDaProperties(new DaGetPropertiesOptions(),
                                                                            out properties,
                                                                            executionOptions)))
            {
                if (properties == null) return;
                for (int j = 0; j < properties.Length; j++)
                {
                    if (properties[j].Id > 99)
                    {
                        TreeNode proNode = new TreeNode(properties[j].Description, PROP_INDEX, PROP_INDEX);
                        proNode.Name = properties[j].Description;
                        proNode.Tag = properties[j];
                        node.Nodes.Add(proNode);
                    }
                }
            }
        }

        /// <summary>
        /// Get Ae AreaSpace Data
        /// </summary>
        private void GetAeAreaSpaceData(TreeNode node, AeSession session)
        {           

            AddressSpaceElementBrowseOptions browseOptions = new AddressSpaceElementBrowseOptions();
            browseOptions.ElementTypeFilter = Softing.OPCToolbox.Client.EnumAddressSpaceElementType.BRANCH;

            AddressSpaceElement elem = null;
            if(!node.Equals(browseTreeView.Nodes[0]))
            {
                elem = node.Tag as AddressSpaceElement;
            }
            AddressSpaceElement[] addressSpaceElements = null;
            if (ResultCode.SUCCEEDED(session.Browse((AddressSpaceElement)elem, 
                                                                        browseOptions, 
                                                                        out addressSpaceElements, 
                                                                        executionOptions)))
            {
                node.Nodes.Clear();
                for (int i = 0; i < addressSpaceElements.Length; i++)
                {
                    TreeNode newNode = new TreeNode(addressSpaceElements[i].Name, NODE_INDEX, NODE_INDEX);
                    newNode.Name = addressSpaceElements[i].Name;
                    newNode.Tag = addressSpaceElements[i];
                    node.Nodes.Add(newNode);
                    newNode.Nodes.Add(new TreeNode(""));
                    string[] conditions = null;
                    string sourcePath = GetSourcePath(newNode);
                    if (ResultCode.SUCCEEDED(session.QueryAeSourceConditions(sourcePath, out conditions, executionOptions)))
                    {
                        for (int j = 0; j < conditions.Length; j++)
                        {
                            TreeNode conditionNode = new TreeNode(conditions[j], PROP_INDEX, PROP_INDEX);
                            conditionNode.Name = conditions[j];
                            conditionNode.Tag = String.Empty;
                            newNode.Nodes.Add(conditionNode);
                        }
                    }
                }
            }
            AddressSpaceElementBrowseOptions browseOpt = new AddressSpaceElementBrowseOptions();
            browseOpt.ElementTypeFilter = Softing.OPCToolbox.Client.EnumAddressSpaceElementType.LEAF;
            if (ResultCode.SUCCEEDED(session.Browse((AddressSpaceElement)elem, 
                                                                            browseOpt, 
                                                                            out addressSpaceElements, 
                                                                            executionOptions)))
            {
                for (int i = 0; i < addressSpaceElements.Length; i++)
                {
                    TreeNode newNode = new TreeNode(addressSpaceElements[i].Name, TAG_INDEX, TAG_INDEX);
                    newNode.Name = addressSpaceElements[i].Name;
                    newNode.Tag = addressSpaceElements[i];
                    node.Nodes.Add(newNode);
                    string[] conditions = null;
                    string sourcePath = GetSourcePath(newNode);
                    if (ResultCode.SUCCEEDED(session.QueryAeSourceConditions(sourcePath, out conditions, executionOptions)))
                    {
                        for (int j = 0; j < conditions.Length; j++)
                        {
                            TreeNode conditionNode = new TreeNode(conditions[j], PROP_INDEX, PROP_INDEX);
                            conditionNode.Name = conditions[j];
                            conditionNode.Tag = String.Empty;
                            newNode.Nodes.Add(conditionNode);
                        }
                    }
                }			
            }
		
        }

        /// <summary>
        /// Get Ae Event Categories Data
        /// </summary>
        private void GetAeEventCategoriesData(TreeNode node, AeSession session)
        {
            node.Nodes.Clear();

            //query categories for each event type
            AeCategory[] categories = null;
            session.QueryAeCategories(out categories, executionOptions);

            for (int i = 0; i < categories.Length; i++)
            {               

                TreeNode categoryNode = new TreeNode(categories[i].Description, NODE_INDEX, NODE_INDEX);
                categoryNode.Name = categories[i].Description;
                node.Nodes.Add(categoryNode);

                //Query categories' attributes
                AeAttribute[] attributes = null;
                categories[i].QueryAeAttributes(out attributes, executionOptions);

                for (int j = 0; j < attributes.Length; j++)
                {
                    TreeNode attributeNode = new TreeNode(attributes[j].Description, TAG_INDEX, TAG_INDEX);
                    attributeNode.Name = attributes[j].Description;
                    categoryNode.Nodes.Add(attributeNode);
                }

                //Query ConditionNames
                string[] conditionNames = null;
                categories[i].QueryAeConditionNames(out conditionNames,executionOptions);

                for (int j = 0; j < conditionNames.Length; j++)
                {
                    TreeNode conditionNameNode = new TreeNode(conditionNames[j], PROP_INDEX, PROP_INDEX);
                    conditionNameNode.Name = conditionNames[j];
                    categoryNode.Nodes.Add(conditionNameNode);

                }
            }           
        }

        /// <summary>
        /// Get Source Path
        /// </summary>
        /// <param name="node"></param>
        /// <returns></returns>
        private string GetSourcePath(TreeNode node)
        {
            string sourcePath = String.Empty;
            ArrayList names = new ArrayList();
            int count = 0;
            while (node.Parent != null)
            {                
                if (node.Tag is AddressSpaceElement)
                {
                    names.Add(((AddressSpaceElement)node.Tag).Name);
                }
                node = node.Parent;
                count++;
            }
            for (int i = names.Count - 1; i > 0; i--)
            {
                sourcePath += names[i];
                sourcePath += ".";
            }
            sourcePath += names[0];
            return sourcePath;
        }        

        /// <summary>
        /// Add items
        /// </summary>
        /// <param name="itemName"></param>
        private void AddItems()
        {
            TreeNode selectedNode = browseTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;
            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaAddressSpaceElement)))
            {
                if (AddDaItems != null)
                {
                    AddDaItems((selectedNode.Tag as DaAddressSpaceElement).ItemId, daSubscription);
                }
            }
        }

        /// <summary>
        /// Show context menu
        /// </summary>
        /// <param name="treeNode"></param>
        private void ShowProperContextMenu(TreeNode treeNode)
        {
            browseTreeView.ContextMenuStrip = null;
            bool enableAddItem = false;

            //show proper context menu if Dassion node selected
            if (treeNode == null) return;
            if (treeNode.Tag == null) return;

            if (browseTreeView.Nodes.Count > 0)
            {
                if (browseTreeView.Nodes[0].Tag == null) return;
                if (Type.ReferenceEquals(browseTreeView.Nodes[0].Tag.GetType(), typeof(DaSession)))
                {
                    //tag node or property node selected
                    if (treeNode.SelectedImageIndex == TAG_INDEX ||
                        treeNode.SelectedImageIndex == PROP_INDEX)
                    {
                        if (treeNode.SelectedImageIndex == TAG_INDEX)
                        {
                            readPropertiesToolStripMenuItem.Text = "Read Properties";
                            readPropertiesToolStripMenuItem.Image = DemoClient.Properties.Resources.properties;
                            readPropertiesToolStripMenuItem.ShortcutKeys = Keys.None;
                        }
                        else
                        {
                            readPropertiesToolStripMenuItem.Text = "Properties";
                            readPropertiesToolStripMenuItem.Image = DemoClient.Properties.Resources.properties;
                            readPropertiesToolStripMenuItem.ShortcutKeys = Keys.F7;
                        }
                        browseTreeView.ContextMenuStrip = contextMenuStripProperties;                                               
                    }
                    else
                    {
                        //primaty node selected
                        browseTreeView.ContextMenuStrip = contextMenuStripAll;
                    }
                    enableAddItem = true;
                }
                if (treeNode.SelectedImageIndex != BROWSE_INDEX)
                {
                    isPropertiesEnabled = true;
                    if (EnableButtons != null)
                    {
                        EnableButtons(false, isPropertiesEnabled, enableAddItem, false, true);
                    }
                }
                else
                {
                    isPropertiesEnabled = false;
                    enableAddItem = false;
                    if (EnableButtons != null)
                    {
                        EnableButtons(false, isPropertiesEnabled, enableAddItem, false, true);
                    }
                }
            }
        }

        /// <summary>
        /// Read Item
        /// </summary>
        private void ReadItem()
        {
            TreeNode selectedNode = browseTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;

            //get the session
            if (browseTreeView.Nodes.Count == 0) return;
            TreeNode rootNode = browseTreeView.Nodes[0];
            if (rootNode == null) return;
            if (rootNode.Tag == null) return;
            if (Type.ReferenceEquals(rootNode.Tag.GetType(), typeof(DaSession)))
            {
                DaSession session = rootNode.Tag as DaSession;
                if (session == null) return;

                if (serviceWindow == null) return;                
                
                //get the itemId
                string itemId = null;
                if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaAddressSpaceElement)))
                {
                    itemId = (selectedNode.Tag as DaAddressSpaceElement).ItemId;
                }
                else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaProperty)))
                {
                    itemId = (selectedNode.Tag as DaProperty).Id.ToString();
                }
                if (String.IsNullOrEmpty(itemId)) return;

                ValueQT[] itemValues;
                bool result = serviceWindow.ReadAddresSpaceItem(session,itemId, out itemValues);
                serviceWindow.Show(this.dockPanel);
            }

        }

        /// <summary>
        /// Write Item
        /// </summary>
        private void WriteItem()
        {
            TreeNode selectedNode = browseTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;

            //get the session
            if (browseTreeView.Nodes.Count == 0) return;
            TreeNode rootNode = browseTreeView.Nodes[0];
            if (rootNode == null) return;
            if (rootNode.Tag == null) return;
            if (Type.ReferenceEquals(rootNode.Tag.GetType(), typeof(DaSession)))
            {
                DaSession session = rootNode.Tag as DaSession;
                if (session == null) return;

                if (serviceWindow == null) return;

                //get the itemId
                string itemId = null;
                bool isAddrSpaceElement = true;
                if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaAddressSpaceElement)))
                {
                    itemId = (selectedNode.Tag as DaAddressSpaceElement).ItemId;
                }
                else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaProperty)))
                {
                    itemId = (selectedNode.Tag as DaProperty).Id.ToString();
                    isAddrSpaceElement = false;
                }
                if (String.IsNullOrEmpty(itemId)) return;
                serviceWindow.WriteAddresSpaceItem(session, itemId, selectedNode.Tag, isAddrSpaceElement);
                serviceWindow.Show(this.dockPanel);
            }

        }

        /// <summary>
        /// Write Da Value
        /// </summary>
        /// <param name="strValue"></param>
        private void WriteValue(string strValue)
        {
            TreeNode selectedNode = browseTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;

            //get the session
            if (browseTreeView.Nodes.Count == 0) return;
            TreeNode rootNode = browseTreeView.Nodes[0];
            if (rootNode == null) return;
            if (rootNode.Tag == null) return;
            if (Type.ReferenceEquals(rootNode.Tag.GetType(), typeof(DaSession)))
            {
                DaSession session = rootNode.Tag as DaSession;
                if (session == null) return;

                //get the itemId
                string itemId = null;
                Type dataType = typeof(System.Byte);
                int startIndex = 0;
                if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaAddressSpaceElement)))
                {
                    itemId = (selectedNode.Tag as DaAddressSpaceElement).ItemId;
                    DaAddressSpaceElement addSpaceElem = selectedNode.Tag as DaAddressSpaceElement;                    
                    //Get Data Type                    
                    DaProperty[] properties = null;
                    DaGetPropertiesOptions propertyGetOptions = new DaGetPropertiesOptions();
                    propertyGetOptions.WhatPropertyData = EnumPropertyData.ALL;
                    if (ResultCode.SUCCEEDED(session.GetDaProperties(itemId,
                                                                              null,
                                                                              propertyGetOptions,
                                                                              out properties,
                                                                              null)))
                    {
                        if (properties == null)
                        {
                            dataType = typeof(System.Byte);
                        }
                        if (properties.Length < 5)
                        {
                            dataType = typeof(System.Byte);
                        }
                        dataType = DataTypeConverter.GetSysType((short)properties[0].ValueQT.Data);
                        if (dataType.IsArray)
                        {
                            
                            startIndex = ((properties[1].ValueQT.Data) as Array).GetLowerBound(0);
                        }
                    }
                }
                else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaProperty)))
                {
                    itemId = (selectedNode.Tag as DaProperty).Id.ToString();
                    dataType = (selectedNode.Tag as DaProperty).DataType;
                    if (dataType.IsArray)
                    {
                        startIndex = (((selectedNode.Tag as DaProperty).ValueQT.Data) as Array).GetLowerBound(0);
                    }

                }
                if (String.IsNullOrEmpty(itemId)) return;
                if (dataType == null) return;

                ValueQT itemValue = new ValueQT();

                if (!dataType.IsArray)
                {
                    //data type is not array
                    object objData = null;
                    ConvertToType(dataType, out objData, strValue);
                    itemValue.SetData(objData, EnumQuality.GOOD, DateTime.Now);
                }
                else
                {
                    //data type is array
                    string typeStr = dataType.FullName;
                    Type arrayType = System.Type.GetType(typeStr.Substring(0, typeStr.IndexOf('[')));                    
                    string[] objDatas = strValue.Trim().Split(';');
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
                }

                //Write DaItem Values
                ExecutionOptions executionOptions = new ExecutionOptions();
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;


                ValueQT[] itemValues = new ValueQT[] { itemValue };
                int[] results;
                session.Write(new string[] { itemId }, null, itemValues, out results, executionOptions);
            }
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

        /// <summary>
        /// ReadItemProperties
        /// </summary>
        private void ReadProperties()
        {
            TreeNode selectedNode = browseTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;

            //get the session
            if (browseTreeView.Nodes.Count == 0) return;
            TreeNode rootNode = browseTreeView.Nodes[0];
            if (rootNode == null) return;
            if (rootNode.Tag == null) return;
            if (Type.ReferenceEquals(rootNode.Tag.GetType(), typeof(DaSession)))
            {
                DaSession session = rootNode.Tag as DaSession;
                if (session == null) return;

                if (serviceWindow == null) return;

                if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaAddressSpaceElement)))
                {
                    //get AddressSpaceElement Properties List
                    DaAddressSpaceElement addressSpaceElem = selectedNode.Tag as DaAddressSpaceElement;
                    if (addressSpaceElem == null) return;
                    propertyWindow.ReadAddressSpaceElemProperties(session, addressSpaceElem.ItemId);
                    propertyWindow.Show(dockPanel);
                }
                else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaProperty)))
                {
                   //get DaProperty Properties
                    DaProperty property = selectedNode.Tag as DaProperty;
                    if (property == null) return;
                    propertyWindow.DaProperty(property);
                    propertyWindow.Show(dockPanel);                    
                }
            }
        }

        #endregion

        #region Public Methods

        #region Constructor

        public AddressSpaceControl()
        {
            InitializeComponent();
           
        }

        #endregion

        #region BuildBrowseTree

        /// <summary>
        /// Empty Browse
        /// </summary>
        public void EmptyBrowse()
        {
            browseTreeView.Nodes.Clear();
        }

        /// <summary>
        /// Build DaBrowse
        /// </summary>
        /// <param name="daSession"></param>
        public void BuildDaBrowseTree(DaSession daSession, DaSubscription subsciption)
        {
            daSubscription = subsciption;
            ShowProperContextMenu(browseTreeView.SelectedNode);
            if (browseTreeView.Nodes.Count > 0)
            {
                if (browseTreeView.Nodes[0].Text.Equals(daSession.Url))
                    return;
            }            
            browseTreeView.Nodes.Clear();
            TreeNode treeRootNode = new TreeNode(daSession.Url, BROWSE_INDEX, BROWSE_INDEX);
            treeRootNode.Name = daSession.Url;
            DaAddressSpaceElement daAddressSpaceElement = new DaAddressSpaceElement();
            daAddressSpaceElement.Session = daSession;
            daRootSpaceElem = daAddressSpaceElement;
            browseTreeView.Nodes.Add(treeRootNode);
            treeRootNode.Nodes.Add(new TreeNode(""));
            treeRootNode.Tag = daSession;
            if (Options.SortDa)
            {
                browseTreeView.Sort();
            }
            else
            {
                browseTreeView.Sorted = false;
            }
 
        }

        /// <summary>
        /// Build AeBrowse
        /// </summary>
        /// <param name="aeSession"></param>
        public void BuildAeBrowseTree(AeSession aeSession)
        {
            if (browseTreeView.Nodes.Count > 0)
            {
                if (browseTreeView.Nodes[0].Text.Equals(aeSession.Url + AREA_SPACE))
                    return;
            }
            daSubscription = null;
            browseTreeView.Nodes.Clear();
            TreeNode treeRootNode = new TreeNode(aeSession.Url + AREA_SPACE, BROWSE_INDEX, BROWSE_INDEX);
            treeRootNode.Name = aeSession.Url + AREA_SPACE;
            browseTreeView.Nodes.Add(treeRootNode);
            treeRootNode.Nodes.Add(new TreeNode(""));
            treeRootNode.Tag = aeSession;

            treeRootNode = new TreeNode(aeSession.Url + EVENT_CATEGORIES, BROWSE_INDEX, BROWSE_INDEX);
            treeRootNode.Name = aeSession.Url + EVENT_CATEGORIES;
            browseTreeView.Nodes.Add(treeRootNode);
            treeRootNode.Nodes.Add(new TreeNode(""));
            treeRootNode.Tag = aeSession;
            if (Options.SortAe)
            {
                browseTreeView.Sort();
            }
            else
            {
                browseTreeView.Sorted = false;
            }

        }
       
        #endregion

        #region Add Item

        /// <summary>
        /// Add item
        /// </summary>
        /// <param name="itemName"></param>
        public void AddItem()
        {
            TreeNode selectedNode = browseTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;

            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaAddressSpaceElement)))
            {
                if (AddDaItem != null)
                {
                    AddDaItem((selectedNode.Tag as DaAddressSpaceElement).ItemId, daSubscription);
                }
            }
            else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaProperty)))
            {
                if (AddDaItem != null)
                {
                    AddDaItem((selectedNode.Tag as DaProperty).ItemId, daSubscription);
                }
            }
        }

        #endregion

        #region Write Da Value

        public void WriteDaValue(string strValue)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                WriteValue(strValue);
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        #endregion

        #region Show Properties

        public void ShowProperties()
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ReadProperties();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        #endregion

        #endregion

        #region Event Handlers

        /// <summary>
        /// Fill Browse data
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void browseTreeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeViewHitTestInfo hit = browseTreeView.HitTest(e.Location);
            if (hit.Location == TreeViewHitTestLocations.PlusMinus)
            {               
                    canExpand = true;
                    canCollapse = true;
            }
            else
            {
                canExpand = false;
                canCollapse = false;
            }
        }

        /// <summary>
        /// Action on selecting a node
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void browseTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            ShowProperContextMenu(e.Node);            
        }

        /// <summary>
        /// Add all child items
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void addItemsToolStripMenuItem_Click(object sender, EventArgs e)
        {
           AddItems();
           MainWindow.AnyChange = true;
        }

        /// <summary>
        /// Write all
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void writeItemsIdToFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveDlg.DefaultExt = "*.txt";
            saveDlg.Title = "Write ItemId's to File";
            saveDlg.RestoreDirectory = true;
            saveDlg.Filter = "Text files|*.txt|All files|*.*";
            saveDlg.ShowDialog();

            //save the file
            if (!String.IsNullOrEmpty(saveDlg.FileName))
            {
                System.IO.FileStream fs =
                   (System.IO.FileStream)saveDlg.OpenFile();
                StreamWriter writer = new StreamWriter(fs);
                TreeNode selectedNode = browseTreeView.SelectedNode;
                if (selectedNode == null) return;
                if (selectedNode.Tag == null) return;
                if (Type.ReferenceEquals(browseTreeView.Nodes[0].Tag.GetType(), typeof(DaSession)))
                {
                    GetDaData(selectedNode, browseTreeView.Nodes[0], browseTreeView.Nodes[0].Tag);
                }
                foreach(TreeNode node in selectedNode.Nodes)
                {
                    writer.WriteLine(String.Format("{0}.{1}",selectedNode.Text,node.Text));          
                }
                writer.Flush();
                writer.Close();
                fs.Close();                
            }

           string[] filesList = Directory.GetFiles("c:");
            foreach(string file in filesList)
            {
                System.IO.FileInfo fi = new System.IO.FileInfo(file);
               
            }

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void readPropertiesToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ReadProperties();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// Add selected Item
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void addItemToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AddItem();
            MainWindow.AnyChange = true;
        }

        /// <summary>
        /// Add item(s) on double click
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void browseTreeView_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
             TreeViewHitTestInfo hit = browseTreeView.HitTest(e.Location);
             if (hit.Location == TreeViewHitTestLocations.Label)
             {
                 AddItem();
             }
             canExpand = true;
             canCollapse = true;
        }

        /// <summary>
        /// Enable/Disable expand of the tree node
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void browseTreeView_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            e.Cancel = !canExpand;
            if (browseTreeView.Nodes.Count > 0)
            {
                ExpandTreeNode(e.Node, browseTreeView.Nodes[0]);
                canExpand = true;
            }          
        }

        /// <summary>
        /// Collapse tree
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void browseTreeView_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
        {
            e.Cancel = !canCollapse;
            canCollapse = true;
        }

        /// <summary>
        /// Read Item Properties
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void readPropertiesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ReadProperties();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }      

        /// <summary>
        /// Address Space Element Read
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void readToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ReadItem();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// Address Space Element Write
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void writeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                WriteItem();
                MainWindow.AnyChange = true;
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// Enable/Disable buttons
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void AddressSpaceControl_Enter(object sender, EventArgs e)
        {
            if (browseTreeView.Nodes.Count > 0)
            {
                if (browseTreeView.Nodes[0].Tag == null) return;
                if (Type.ReferenceEquals(browseTreeView.Nodes[0].Tag.GetType(), typeof(DaSession)))
                {
                    if (browseTreeView.SelectedNode != null && browseTreeView.SelectedNode.SelectedImageIndex != BROWSE_INDEX)
                    {
                        isPropertiesEnabled = true;
                        if (EnableButtons != null)
                        {
                            EnableButtons(false, isPropertiesEnabled, true, false, true);
                        }
                    }
                }
            }

        }

        #endregion                  
       
    }
}
