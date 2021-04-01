using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using DemoClient.DockWindows;
using DemoClient.Helper;

namespace DemoClient.TabControls
{
    #region Enums

    public enum ShownItems
    {
        All = 1,
        Session,
        Subscription
    }

    #endregion

    public delegate void EnableButtonsHandler(bool enableDelete, bool enableProperties, bool enableAddItem, bool enableAddSubscription, bool enableRefresh);

    public partial class ItemsControl : UserControl
    {
        #region Constants

        private const int ACTIVATED = 0;
        private const int DISCONNECTED = 1;
        private const int CONNECTED = 2;
        private const int CONN_ACTIV_0 = 3;
        private const int CONN_ACTIV_1 = 4;
        private const int CONN_ACTIV_2 = 5;
        private const int CONN_ACTIV_3 = 6;
        private const int DISCONN_ACTIV_0 = 7;
        private const int DISCONN_ACTIV_1 = 8;
        private const int DISCONN_ACTIV_2 = 9;
        private const int DISCONN_ACTIV_3 = 10;
        private const int DISCONN_CONN_0 = 11;
        private const int DISCONN_CONN_1 = 12;
        private const int DISCONN_CONN_2 = 13;
        private const int DISCONN_CONN_3 = 14;

        #endregion

        #region Private Members

        private bool isDeleteEnabled = false;
        private bool isPropertiesEnabled = false;

        #endregion

        #region Properties

        private PropertyWindow propertyWindow = null;
        public PropertyWindow PropertyWindow
        {
            get { return propertyWindow; }
            set { propertyWindow = value; }
        }

        private ServiceWindow serviceWindow = null;
        public ServiceWindow ServiceWindow
        {
            get { return serviceWindow; }
            set { serviceWindow = value; }
        }

        private WeifenLuo.WinFormsUI.Docking.DockPanel dockPanel;
        public WeifenLuo.WinFormsUI.Docking.DockPanel MainDockPanel
        {
            get { return dockPanel; }
            set { dockPanel = value; }
        }

        #endregion

        #region Delegates

        internal event EnableButtonsHandler EnableButtons;

        #endregion

        #region Private and Protected Methods

        #region Set Image

        /// <summary>
        /// Set item icon
        /// </summary>
        /// <param name="enumObjectState"></param>
        /// <param name="listViewItem"></param>
        private void SetItemImage(EnumObjectState enumObjectState, ListViewItem listViewItem)
        {
            switch (enumObjectState)
            {
                case EnumObjectState.ACTIVATED:
                    listViewItem.StateImageIndex = ACTIVATED;
                    listViewItem.ImageIndex = ACTIVATED;
                    break;
                case EnumObjectState.CONNECTED:
                    listViewItem.StateImageIndex = CONNECTED;
                    listViewItem.ImageIndex = CONNECTED;
                    break;
                case EnumObjectState.DISCONNECTED:
                    listViewItem.StateImageIndex = DISCONNECTED;
                    listViewItem.ImageIndex = DISCONNECTED;
                    break;
            }
        }

        /// <summary>
        /// Set intermediare image
        /// </summary>
        /// <param name="imageIndex"></param>
        /// <param name="listViewItem"></param>
        private void SetIntermediareImage(int imageIndex, ListViewItem listViewItem)
        {
            listViewItem.StateImageIndex = imageIndex;
            listViewItem.ImageIndex = imageIndex;
        }

        #endregion

        #region Delete Items

        /// <summary>
        /// Delete selected DaItems
        /// </summary>
        private void DeleteSelectedDaItems()
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                while (itemsListView.SelectedItems.Count > 0)
                {
                    ListViewItem selectedItem = itemsListView.SelectedItems[0];
                    if (selectedItem == null) continue;
                    if (selectedItem.Tag == null) continue;
                    DaItem daItem = selectedItem.Tag as DaItem;
                    if (daItem == null) continue;
                    daItem.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                    DaSubscription subscription = daItem.DaSubscription;
                    if (subscription == null) continue;
                    if (ResultCode.SUCCEEDED(daItem.Disconnect(null)))
                    {
                        subscription.RemoveDaItem(daItem);
                        selectedItem.Remove();
                    }
                }
            }
        }

        #endregion

        #region Activate/Connect/Disconnect Items

        /// <summary>
        /// Activate SelectedItems
        /// </summary>
        private void ActivateSelectedItems()
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                foreach ( ListViewItem selectedItem in itemsListView.SelectedItems)
                {
                    if (selectedItem == null) continue;
                    if (selectedItem.Tag == null) continue;
                    DaItem daItem = selectedItem.Tag as DaItem;
                    if (daItem == null) continue;
                    daItem.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                    daItem.Connect(false, true, null);

                    if (daItem.CurrentState == EnumObjectState.ACTIVATED)
                    {
                        SetIntermediareImage(ACTIVATED, selectedItem);
                    }
                    else if (daItem.CurrentState == EnumObjectState.CONNECTED)
                    {
                        SetIntermediareImage(CONNECTED, selectedItem);
                    }
                    else if (daItem.CurrentState == EnumObjectState.DISCONNECTED)
                    {
                        SetIntermediareImage(DISCONNECTED, selectedItem);
                    }                    
                    daItem.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                    daItem.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                }
            }
        }

        /// <summary>
        /// Connect SelectedItems
        /// </summary>
        private void ConnectSelectedItems()
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                foreach (ListViewItem selectedItem in itemsListView.SelectedItems)
                {
                    if (selectedItem == null) continue;
                    if (selectedItem.Tag == null) continue;
                    DaItem daItem = selectedItem.Tag as DaItem;
                    if (daItem == null) continue;
                    daItem.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                    daItem.Connect(false, false, null);

                     if (daItem.CurrentState == EnumObjectState.ACTIVATED)
                    {
                        SetIntermediareImage(ACTIVATED, selectedItem);
                    }
                    else if (daItem.CurrentState == EnumObjectState.CONNECTED)
                    {
                        SetIntermediareImage(CONNECTED, selectedItem);
                    }
                    else if (daItem.CurrentState == EnumObjectState.DISCONNECTED)
                    {
                        SetIntermediareImage(DISCONNECTED, selectedItem);
                    }
                    daItem.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                    daItem.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                }
            }
        }

        /// <summary>
        /// Disconnect SelectedItems
        /// </summary>
        private void DisconnectSelectedItems()
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                foreach (ListViewItem selectedItem in itemsListView.SelectedItems)
                {
                    if (selectedItem == null) continue;
                    if (selectedItem.Tag == null) continue;
                    DaItem daItem = selectedItem.Tag as DaItem;
                    if (daItem == null) continue;
                    daItem.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                    daItem.Disconnect(null);
                     if (daItem.CurrentState == EnumObjectState.ACTIVATED)
                    {
                        SetIntermediareImage(ACTIVATED, selectedItem);
                    }
                    else if (daItem.CurrentState == EnumObjectState.CONNECTED)
                    {
                        SetIntermediareImage(CONNECTED, selectedItem);
                    }
                    else if (daItem.CurrentState == EnumObjectState.DISCONNECTED)
                    {
                        SetIntermediareImage(DISCONNECTED, selectedItem);
                    }
                    daItem.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                    daItem.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                }
            }
        }

        #endregion

        #region Show Items

        /// <summary>
        /// Show subscription items
        /// </summary>
        /// <param name="anDaSubscription"></param>
        /// <param name="items"></param>
        private void ShowSubscriptionItems(DaSubscription anDaSubscription, DaItem[] items)
        {
            if (items == null)
            {
                items = anDaSubscription.ItemList;
            }
            ValueQT[] itemValues;
            int[] results;
            if (ResultCode.SUCCEEDED(anDaSubscription.Read(0, items, out itemValues, out results, null)))
            {
                for(int i= 0; i < items.Length; i++)
                {
                    string error;
                    anDaSubscription.DaSession.GetErrorString(results[i], out error, null);
                    if (ResultCode.SUCCEEDED(results[i]))
                    {
                        AddListViewItem(items[i], itemValues[i]);
                    }
                    else
                    {
                        AddListViewItem(items[i], items[i].ValueQT);
                    }
                }
            }
            else
            {
                foreach (DaItem item in items)
                {
                    AddListViewItem(item, item.ValueQT);
                }
            }
        }

        /// <summary>
        /// Show session items
        /// </summary>
        /// <param name="sessions"></param>
        private void ShowSessionItems(DaSession[] sessions)
        {
            if (sessions == null) return;
            foreach (DaSession session in sessions)
            {
                foreach (DaSubscription subscription in session.SubscriptionList)
                {
                    ShowSubscriptionItems(subscription, subscription.ItemList);
                }
            }
        }

        /// <summary>
        /// Show all items
        /// </summary>
        /// <param name="sessions"></param>
        private void ShowAllItems(DaSession[] sessions)
        {
            if (sessions == null) return;
            foreach (DaSession session in sessions)
            {
                foreach (DaSubscription subscription in session.SubscriptionList)
                {
                    ShowSubscriptionItems(subscription, subscription.ItemList);
                }
            }
        }

        /// <summary>
        /// Add List View Items
        /// </summary>
        /// <param name="item"></param>
        /// <param name="itemValuesO"></param>
        private void AddListViewItem(DaItem item, ValueQT itemValues)
        {
            ListViewItem listViewItem = new ListViewItem(item.Id);
            if (item.AccessRights != EnumAccessRights.WRITEABLE && itemValues != null)
            {
                listViewItem.SubItems.Add(ValueConverter.ArrayToString(itemValues));
                listViewItem.SubItems.Add(itemValues.Quality.ToString());
                listViewItem.SubItems.Add(ValueConverter.DateTimeOptions(itemValues.TimeStamp));
            }
            else
            {
                listViewItem.SubItems.Add("");
                listViewItem.SubItems.Add("");
                listViewItem.SubItems.Add("");
            }
            listViewItem.SubItems.Add(item.DaSubscription.DaSession.Url);
            listViewItem.SubItems.Add(item.DaSubscription.Name);
            listViewItem.Tag = item;
            SetItemImage(item.CurrentState, listViewItem);
            itemsListView.Items.Add(listViewItem);
            item.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
            item.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
            
        }

        /// <summary>
        /// Item state changed
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="state"></param>
        private void item_StateChangeCompleted(ObjectSpaceElement obj, EnumObjectState state)
        {
            if (this.InvokeRequired)
            {
                Softing.OPCToolbox.Client.StateChangeEventHandler callback = new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                this.Invoke(callback, new object[] { obj, state});
                return;
            }
            if (obj is DaItem)
            {
                //search item in list and uptade it
                if (itemsListView == null) return;
                foreach (ListViewItem listViewItem in itemsListView.Items)
                {
                    if (listViewItem == null) continue;
                    if (listViewItem.Tag == null) continue;
                    DaItem daItem = listViewItem.Tag as DaItem;
                    if (daItem == null) continue;
                    string itemText = listViewItem.Text;
                    if (daItem == (obj as DaItem))
                    {
                        SetItemImage((obj as DaItem).CurrentState, listViewItem);
                    }
                }
            }
        }

        #endregion

        #region Read/Write Items

        /// <summary>
        /// Read Item
        /// </summary>
        private void ReadItem()
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                ListViewItem firstSelectedItem = itemsListView.SelectedItems[0];
                if (firstSelectedItem == null) return;
                if (firstSelectedItem.Tag == null) return;
                string itemText = firstSelectedItem.Text;
                DaItem daItem = firstSelectedItem.Tag as DaItem;
                if (daItem == null) return;
                if (serviceWindow == null) return;
                ValueQT itemValues;
                bool result = serviceWindow.ReadDaItem(daItem, out itemValues);
                if (result)
                {
                    firstSelectedItem.SubItems.Clear();
                    firstSelectedItem.Text = itemText;
                    firstSelectedItem.Tag = daItem;
                    if (daItem.AccessRights != EnumAccessRights.WRITEABLE && itemValues != null)
                    {
                        firstSelectedItem.SubItems.Add(ValueConverter.ArrayToString(itemValues));
                        firstSelectedItem.SubItems.Add(itemValues.Quality.ToString());
                        firstSelectedItem.SubItems.Add(ValueConverter.DateTimeOptions(itemValues.TimeStamp));
                    }
                    else
                    {
                        firstSelectedItem.SubItems.Add("");
                        firstSelectedItem.SubItems.Add("");
                        firstSelectedItem.SubItems.Add("");
                    }
                    firstSelectedItem.SubItems.Add(daItem.DaSubscription.DaSession.Url);
                    firstSelectedItem.SubItems.Add(daItem.DaSubscription.Name);
                }
                if (result)
                {
                    serviceWindow.Show(this.dockPanel);
                }
            }
        }

        /// <summary>
        /// Write Item
        /// </summary>
        private void WriteItem()
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                ListViewItem firstSelectedItem = itemsListView.SelectedItems[0];
                if (firstSelectedItem == null) return;
                if (firstSelectedItem.Tag == null) return;
                DaItem daItem = firstSelectedItem.Tag as DaItem;
                if (daItem == null) return;
                if (serviceWindow == null) return;
                bool result = serviceWindow.WriteDaItem(daItem);
                if (result)
                {
                    serviceWindow.Show(this.dockPanel);
                }
            }
        }

        /// <summary>
        /// Write Value
        /// </summary>
        /// <param name="strValue"></param>
        private void WriteValue(string strValue)
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                //get the item session
                DaSession session = null;
                string[] itemIds = new string[itemsListView.SelectedItems.Count];
                ValueQT[] itemValues = new ValueQT[itemsListView.SelectedItems.Count];
                for (int i = 0; i < itemsListView.SelectedItems.Count; i++)
                {
                    ListViewItem listViewItem = itemsListView.SelectedItems[i];
                    if (listViewItem == null) return;
                    if (listViewItem.Tag == null) return;
                    DaItem daItem = listViewItem.Tag as DaItem;
                    if (daItem == null) return;  
                    //if no VALUE QT return
                    if (daItem.ValueQT.Data == null) return;
                    if (session == null)
                    {
                        session = daItem.DaSubscription.DaSession;
                    }

                    string itemId = null;
                    Type dataType = typeof(System.Byte);
                    int startIndex = 0;

                    itemId = daItem.Id;
                    dataType = daItem.NativeDatatype;
                    if (dataType.IsArray)
                    {
                        startIndex = ((daItem.ValueQT.Data) as Array).GetLowerBound(0);
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
                    itemIds[i] = itemId;
                    itemValues[i] = itemValue;
                }

                //Write DaItem Values
                ExecutionOptions executionOptions = new ExecutionOptions();
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;

                int[] results;
                session.Write(itemIds, null, itemValues, out results, executionOptions);
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

        #endregion

        #region Item Properties

        /// <summary>
        /// Display item properties
        /// </summary>
        private void ItemProperties()
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                ListViewItem firstSelectedItem = itemsListView.SelectedItems[0];
                if (firstSelectedItem == null) return;
                if (firstSelectedItem.Tag == null) return;
                string itemText = firstSelectedItem.Text;
                DaItem daItem = firstSelectedItem.Tag as DaItem;
                if (daItem == null) return;
                if (propertyWindow == null) return;
                bool result = propertyWindow.DaItem(daItem);
                if (result)
                {
                    daItem.DaSubscription.SetDaItemAttributesToServerCompleted -= new SetDaItemAttributesToServerEventHandler(DaSubscription_SetDaItemAttributesToServerCompleted);
                    daItem.DaSubscription.SetDaItemAttributesToServerCompleted += new SetDaItemAttributesToServerEventHandler(DaSubscription_SetDaItemAttributesToServerCompleted);
                    propertyWindow.Show(this.dockPanel);                    
                }                
            }
        }       

        #endregion       

        #endregion

        #region Public Methods

        #region Constructor

        public ItemsControl()
        {
            InitializeComponent();
        }

        #endregion

        /// <summary>
        /// Add an item
        /// </summary>
        /// <param name="item"></param>
        public void AddItem(DaItem item)
        {
           try
            {
                ValueQT itemValues;
                int itemResult;
                int result = item.Read(100, out itemValues, out itemResult, new ExecutionOptions());
                if (ResultCode.SUCCEEDED(result) && ResultCode.SUCCEEDED(itemResult))
                {
                    AddListViewItem(item, itemValues);                    
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
           
        }       

        /// <summary>
        /// Delete all subscription Da items
        /// </summary>
        /// <param name="daSubscription"></param>
        public void DeleteDaItems(DaSubscription daSubscription)
        {
            foreach (DaItem daItem in daSubscription.ItemList)
            {
                daItem.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(item_StateChangeCompleted);
                ListViewItem listViewItem = new ListViewItem(daItem.Id);
                if (itemsListView.Items.Contains(listViewItem))
                {
                    daItem.Disconnect(null);
                    itemsListView.Items.Remove(listViewItem);
                }
            }
        }

        /// <summary>
        /// Check if there are DaItems
        /// </summary>
        /// <returns></returns>
        public bool IsEmpty()
        {
            if (itemsListView.Items.Count == 0)
                return true;
            return false;
        }
       
        /// <summary>
        /// Add items to the list
        /// </summary>
        public void ShowItems(DaSubscription anDaSubscription, DaItem[] items, ShownItems cond, DaSession[] sessions)
        {
            try
            {
                this.SuspendLayout();
                itemsListView.Items.Clear();
                switch (cond)
                {
                    case ShownItems.All:
                        ShowAllItems(sessions);
                        break;
                    case ShownItems.Session:
                        ShowSessionItems(sessions);
                        break;
                    case ShownItems.Subscription:
                        ShowSubscriptionItems(anDaSubscription, items);
                        break;
                }
                if (itemsListView.Items.Count > 0)
                {
                    this.Enabled = true;
                }
                else
                {
                    this.Enabled = false;
                }
                this.ResumeLayout();
            }
            catch { }
        }

        /// <summary>
        /// Update list view items if are an item form the collection
        /// </summary>
        /// <param name="aDaSubscription"></param>
        /// <param name="items"></param>
        /// <param name="values"></param>
        /// <param name="results"></param>
        public void DataChanged(DaSubscription aDaSubscription, DaItem[] items, ValueQT[] values, int[] results)
        {
            foreach (ListViewItem listViewItem in itemsListView.Items)
            {
                if (listViewItem == null) continue;
                if (listViewItem.Tag == null) continue;
                DaItem daItem = listViewItem.Tag as DaItem;
                if (daItem == null) continue;
                string itemText = listViewItem.Text;
                for (int i = 0; i < items.Length; i++)
                {
                    if (daItem == items[i])
                    {
                        if (ResultCode.SUCCEEDED(results[i]))
                        {
                            listViewItem.SubItems.Clear();
                            listViewItem.Text = itemText;
                            listViewItem.Tag = daItem;
                            if (daItem.AccessRights != EnumAccessRights.WRITEABLE && values[i] != null)
                            {
                                listViewItem.SubItems.Add(ValueConverter.ArrayToString(values[i]));
                                listViewItem.SubItems.Add(values[i].Quality.ToString());
                                listViewItem.SubItems.Add(ValueConverter.DateTimeOptions(values[i].TimeStamp));
                            }
                            else
                            {
                                listViewItem.SubItems.Add("");
                                listViewItem.SubItems.Add("");
                                listViewItem.SubItems.Add("");
                            }
                            listViewItem.SubItems.Add(daItem.DaSubscription.DaSession.Url);
                            listViewItem.SubItems.Add(daItem.DaSubscription.Name);
                        }
                    }
                }
            }
 
        }

        /// <summary>
        /// Write Da Item Value
        /// </summary>
        /// <param name="strValue"></param>
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

        /// <summary>
        /// Show propeties
        /// </summary>
        public void ShowProperties()
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ItemProperties();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// Delete Item
        /// </summary>
        public void DeleteItem()
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                DeleteSelectedDaItems();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// Empty items list
        /// </summary>
        public void EmptyItems()
        {
            itemsListView.Items.Clear();
        }

        #endregion

        #region Event Handlers

        private void itemsListView_MouseCaptureChanged(object sender, EventArgs e)
        {
            if (itemsListView.SelectedItems.Count == 0)
            {
                itemsListView.ContextMenuStrip = null;
            }
            else
            {
                itemsListView.ContextMenuStrip = contextMenuStripItems;
            }
        }

        #region Context Menu Actions

        /// <summary>
        /// Delete Menu Strip
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                DeleteSelectedDaItems();
                MainWindow.AnyChange = true;
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }       

        /// <summary>
        /// Activate DaItem
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void activateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ActivateSelectedItems();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }       

        /// <summary>
        /// Connect DaItem
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void connectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ConnectSelectedItems();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// Disconnect DaItem
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void disconnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                DisconnectSelectedItems();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// Read DaItem
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
        /// Write DaItem
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
            catch(Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// Read DaItem Properties
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void propertiesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ItemProperties();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }       

        #endregion

        #region Setting Item Attributes

        /// <summary>
        /// Display the item new properties
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="executionContext"></param>
        /// <param name="whatAttributes"></param>
        /// <param name="results"></param>
        /// <param name="result"></param>
        void DaSubscription_SetDaItemAttributesToServerCompleted(DaSubscription aDaSubscription, uint executionContext, DaItem[] items, EnumObjectAttribute[] whatAttributes, int[] results, int result)
        {
            if (this.InvokeRequired)
            {
                Softing.OPCToolbox.Client.SetDaItemAttributesToServerEventHandler callback = new Softing.OPCToolbox.Client.SetDaItemAttributesToServerEventHandler(DaSubscription_SetDaItemAttributesToServerCompleted);
                this.Invoke(callback, new object[] { aDaSubscription, executionContext, items, whatAttributes, results, result });
                return;
            }

            if (ResultCode.SUCCEEDED(result))
            {
                for (int i = 0; i < items.Length; i++)
                {
                    if (ResultCode.SUCCEEDED(results[i]))
                    {
                        DaItem item = items[i];
                        foreach (EnumObjectAttribute objAttrib in whatAttributes)
                        {
                            if (objAttrib == EnumObjectAttribute.DAITEM_ID ||
                                objAttrib == EnumObjectAttribute.DAITEM_ALL)
                            {
                                //find the element and reset the ID
                                foreach (ListViewItem listViewItem in itemsListView.Items)
                                {
                                    if (listViewItem == null) continue;
                                    if (listViewItem.Tag == null) return;
                                    DaItem daItem = listViewItem.Tag as DaItem;
                                    if (daItem == null) continue;
                                    if (daItem == item)
                                    {
                                        listViewItem.Text = item.Id;
                                        SetItemImage(daItem.CurrentState, listViewItem);
                                        item.DaSubscription.SetDaItemAttributesToServerCompleted -= new SetDaItemAttributesToServerEventHandler(DaSubscription_SetDaItemAttributesToServerCompleted);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        #endregion

        #region Enable/Disable Buttons

        /// <summary>
        /// Disable/enable buttons
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void itemsListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                isDeleteEnabled = true;
                isPropertiesEnabled = true;
            }
            else 
            {
                isDeleteEnabled = false;
                isPropertiesEnabled = false;
            }
            if (EnableButtons != null)
            {
                EnableButtons(isDeleteEnabled, isPropertiesEnabled, false, false, false);
            }
        }

        /// <summary>
        /// Disable/enable buttons
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ItemsControl_Enter(object sender, EventArgs e)
        {
            if (itemsListView.SelectedItems.Count > 0)
            {
                isDeleteEnabled = true;
                isPropertiesEnabled = true;
            }
            else
            {
                isDeleteEnabled = false;
                isPropertiesEnabled = false;
            }
            if (EnableButtons != null)
            {
                EnableButtons(isDeleteEnabled, isPropertiesEnabled, false, false, false);
            }
        }

        #endregion

        #endregion      
    
       
    }
}
