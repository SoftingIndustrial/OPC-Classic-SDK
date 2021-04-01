using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Client;
using DemoClient.Helper;
using System.Xml.Serialization;
using System.IO;

namespace DemoClient.DockWindows
{
    public delegate void EnableWriteBarHandler(bool enable);
    public delegate void EnableButtonsHandler(bool enableDelete, bool enableProperties, bool enableAddItem, bool enableAddSubscription, bool enableRefresh);
    public delegate void RefreshWindowHandler(bool refreshMenu);

    public partial class MainWindow : DockContent
    {
        #region Constants

        private const string DATA_ACCESS = "Data Access";
        private const string ALARM_AND_EVENTS = "Alarms and Events";
        private const string SUBSCRIPTION = "Subscription";

        //image list cons indexes
        private const int ROOT = 0;
        private const int ACTIVATED = 1;
        private const int DISCONNECTED = 2;
        private const int CONNECTED = 3;
        private const int CONN_ACTIV_0 = 4;
        private const int CONN_ACTIV_1 = 5;
        private const int CONN_ACTIV_2 = 6;
        private const int CONN_ACTIV_3 = 7;
        private const int DISCONN_ACTIV_0 = 8;
        private const int DISCONN_ACTIV_1 = 9;
        private const int DISCONN_ACTIV_2 = 10;
        private const int DISCONN_ACTIV_3 = 11;
        private const int DISCONN_CONN_0 = 12;
        private const int DISCONN_CONN_1 = 13;
        private const int DISCONN_CONN_2 = 14;
        private const int DISCONN_CONN_3 = 15;

        public const uint MAX_AGE_DEVICE = 0;

        #endregion

        #region Private Members

        ExecutionOptions executionOptions = new ExecutionOptions();

        private Dictionary<object, ObjectCollection> updateBrowsersDict = new Dictionary<object, ObjectCollection>();

        private bool isWriteBarEnabled = false;
        private bool isDeleteEnabled = false;
        private bool isPropertiesEnabled = false;

        private string currentFilePath = "";

        private List<FilesStruct> lastSavedFiles = null;

        #endregion       

        #region Public Static Members

        public static bool AnyChange = false;

        #endregion

        #region Properties

        private Softing.OPCToolbox.Client.Application clientAppInstance = null;
        public Softing.OPCToolbox.Client.Application ClientAppInstance
        {
            get { return clientAppInstance; }
            set
            {
                if (clientAppInstance == null) clientAppInstance = value;
            }
        }

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
        
        public event EnableWriteBarHandler EnableWriteBar;

        public event EnableButtonsHandler EnableButtons;

        public event RefreshWindowHandler RefreshWindow;

        #endregion

        #region Private and Protected Methods

        #region Disable/Enable Controls

        /// <summary>
        /// Disable tabs on loading
        /// </summary>
        private void DisableTabs()
        {
            serverDiscoveryControl.Enabled = true;
            addressSpaceControl.Enabled = false;
            itemsControl.Enabled = false;
            eventsControl.Enabled = false;
            conditionsControl.Enabled = false;
        }

        /// <summary>
        /// Enable/Disable Address Space
        /// </summary>
        private void EnableDisableData()
        {
            TreeNode node = serversGroupsTreeView.SelectedNode;
            if (node == null) return;
            if (node.Tag == null) return;
            ShowSpecificContextMenu(node.Tag);
        }

        #region Context Menus Disable/Enable Items

        /// <summary>
        /// Show context menu related to the selected treeNode
        /// </summary>
        /// <param name="obj"></param>
        private void ShowSpecificContextMenu(object obj)
        {
            //fill address space
            FillAddressSpace(obj);
            if (Type.ReferenceEquals(obj.GetType(), typeof(DaSession)))
            {
                serversGroupsTreeView.ContextMenuStrip = sessionContextMenuStrip;
                DaSession daSession = (DaSession)obj;
                if (daSession == null) return;
                switch (daSession.CurrentState)
                {
                    case EnumObjectState.CONNECTED:
                        flatToolStripMenuItem.Enabled = false;
                        deepToolStripMenuItem.Enabled = true;
                        disconnectToolStripMenuItem.Enabled = true;
                        break;
                    case EnumObjectState.DISCONNECTED:
                        disconnectToolStripMenuItem.Enabled = false;
                        flatToolStripMenuItem.Enabled = true;
                        deepToolStripMenuItem.Enabled = true;
                        break;
                }
                return;
            }
            if (Type.ReferenceEquals(obj.GetType(), typeof(AeSession)))
            {
                serversGroupsTreeView.ContextMenuStrip = sessionContextMenuStrip;
                AeSession aeSession = (AeSession)obj;
                if (aeSession == null) return;
                switch (aeSession.CurrentState)
                {
                    case EnumObjectState.CONNECTED:
                        flatToolStripMenuItem.Enabled = false;
                        deepToolStripMenuItem.Enabled = true;
                        disconnectToolStripMenuItem.Enabled = true;
                        break;
                    case EnumObjectState.DISCONNECTED:
                        disconnectToolStripMenuItem.Enabled = false;
                        flatToolStripMenuItem.Enabled = true;
                        deepToolStripMenuItem.Enabled = true;
                        break;
                }
                return;
            }
            if (Type.ReferenceEquals(obj.GetType(), typeof(DaSubscription)))
            {
                serversGroupsTreeView.ContextMenuStrip = daSubscriptionContextMenuStrip;
                DaSubscription daSubcription = (DaSubscription)obj;
                if (daSubcription == null) return;
                switch (daSubcription.CurrentState)
                {
                    case EnumObjectState.ACTIVATED:
                        daFlatConnectToolStripMenuItem.Enabled = true;
                        daDeepConnectToolStripMenuItem.Enabled = true;
                        daFlatActivateToolStripMenuItem.Enabled = false;
                        daDeepActivateToolStripMenuItem.Enabled = true;
                        daDisconnectToolStripMenuItem.Enabled = true;
                        daRefreshToolStripMenuItem.Enabled = true;
                        break;
                    case EnumObjectState.CONNECTED:
                        daFlatConnectToolStripMenuItem.Enabled = false;
                        daDeepConnectToolStripMenuItem.Enabled = true;
                        daFlatActivateToolStripMenuItem.Enabled = true;
                        daDeepActivateToolStripMenuItem.Enabled = true;
                        daDisconnectToolStripMenuItem.Enabled = true;
                        daRefreshToolStripMenuItem.Enabled = false;
                        break;
                    case EnumObjectState.DISCONNECTED:
                        daFlatConnectToolStripMenuItem.Enabled = true;
                        daDeepConnectToolStripMenuItem.Enabled = true;
                        daFlatActivateToolStripMenuItem.Enabled = true;
                        daDeepActivateToolStripMenuItem.Enabled = true;
                        daDisconnectToolStripMenuItem.Enabled = false;
                        daRefreshToolStripMenuItem.Enabled = false;
                        break;
                }
                return;
            }
            if (Type.ReferenceEquals(obj.GetType(), typeof(AeSubscription)))
            {
                serversGroupsTreeView.ContextMenuStrip = aeSubsciptionContextMenuStrip;
                AeSubscription aeSubcription = (AeSubscription)obj;
                if (aeSubcription == null) return;
                switch (aeSubcription.CurrentState)
                {
                    case EnumObjectState.ACTIVATED:
                        aeFlatConnectToolStripMenuItem.Enabled = true;
                        aeDeepConnectToolStripMenuItem.Enabled = true;
                        aeFlatActivateToolStripMenuItem.Enabled = false;
                        aeDeepActivateToolStripMenuItem.Enabled = true;
                        aeDisconnectToolStripMenuItem.Enabled = true;
                        break;
                    case EnumObjectState.CONNECTED:
                        aeFlatConnectToolStripMenuItem.Enabled = false;
                        aeDeepConnectToolStripMenuItem.Enabled = true;
                        aeFlatActivateToolStripMenuItem.Enabled = true;
                        aeDeepActivateToolStripMenuItem.Enabled = true;
                        aeDisconnectToolStripMenuItem.Enabled = true;
                        break;
                    case EnumObjectState.DISCONNECTED:
                        aeFlatConnectToolStripMenuItem.Enabled = true;
                        aeDeepConnectToolStripMenuItem.Enabled = true;
                        aeFlatActivateToolStripMenuItem.Enabled = true;
                        aeDeepActivateToolStripMenuItem.Enabled = true;
                        aeDisconnectToolStripMenuItem.Enabled = false;
                        break;
                }
                return;
            }
        }

        /// <summary>
        /// Fill Address Space
        /// </summary>
        private void FillAddressSpace(object obj)
        {
            if (Type.ReferenceEquals(obj.GetType(), typeof(DaSession)))
            {
                DaSession daSession = (DaSession)obj;
                if (daSession == null) return;
                if (daSession.CurrentState == EnumObjectState.DISCONNECTED)
                {
                    addressSpaceControl.EmptyBrowse();
                    addressSpaceControl.Enabled = false;
                }
                else
                {
                    addressSpaceControl.Enabled = true;
                    addressSpaceControl.BuildDaBrowseTree(daSession, null);
                }
                return;
            }
            if (Type.ReferenceEquals(obj.GetType(), typeof(AeSession)))
            {
                AeSession aeSession = (AeSession)obj;
                if (aeSession == null) return;
                if (aeSession.CurrentState == EnumObjectState.DISCONNECTED)
                {
                    addressSpaceControl.EmptyBrowse();
                    addressSpaceControl.Enabled = false;
                }
                else
                {
                    addressSpaceControl.Enabled = true;
                    addressSpaceControl.BuildAeBrowseTree(aeSession);
                }
                return;
            }
            if (Type.ReferenceEquals(obj.GetType(), typeof(DaSubscription)))
            {
                DaSubscription daSubcription = (DaSubscription)obj;
                if (daSubcription == null) return;
                if (daSubcription.DaSession.CurrentState == EnumObjectState.DISCONNECTED)
                {
                    addressSpaceControl.EmptyBrowse();
                    addressSpaceControl.Enabled = false;
                }
                else
                {
                    addressSpaceControl.Enabled = true;
                    addressSpaceControl.BuildDaBrowseTree(daSubcription.DaSession, daSubcription);
                }
                return;
            }
            if (Type.ReferenceEquals(obj.GetType(), typeof(AeSubscription)))
            {
                serversGroupsTreeView.ContextMenuStrip = aeSubsciptionContextMenuStrip;
                AeSubscription aeSubcription = (AeSubscription)obj;
                if (aeSubcription == null) return;
                if (aeSubcription.AeSession.CurrentState == EnumObjectState.DISCONNECTED)
                {
                    addressSpaceControl.EmptyBrowse();
                    addressSpaceControl.Enabled = false;
                }
                else
                {
                    addressSpaceControl.Enabled = true;
                    addressSpaceControl.BuildAeBrowseTree(aeSubcription.AeSession);
                }
                return;
            }
        }

        #endregion

        #endregion

        #region Hook Events

        /// <summary>
        /// Add event handlers
        /// </summary>
        private void HookEvents()
        {            
            serverDiscoveryControl.ServerBrowseAdd -= new DemoClient.TabControls.ServerBrowseAddHandler(serverDiscoveryControl_ServerBrowseAdd);
            serverDiscoveryControl.ServerBrowseAdd += new DemoClient.TabControls.ServerBrowseAddHandler(serverDiscoveryControl_ServerBrowseAdd);

            addressSpaceControl.AddDaItem -= new DemoClient.TabControls.AddDaItemHandler(addressSpaceControl_AddDaItem);
            addressSpaceControl.AddDaItem += new DemoClient.TabControls.AddDaItemHandler(addressSpaceControl_AddDaItem);

            addressSpaceControl.AddDaItems -= new DemoClient.TabControls.AddDaItemsHandler(addressSpaceControl_AddDaItems);
            addressSpaceControl.AddDaItems += new DemoClient.TabControls.AddDaItemsHandler(addressSpaceControl_AddDaItems);
        }
       
        #endregion

        #region Unhook Events

        /// <summary>
        /// Remove event handlers
        /// </summary>
        private void UnhookEvents()
        {
            serverDiscoveryControl.ServerBrowseAdd -= new DemoClient.TabControls.ServerBrowseAddHandler(serverDiscoveryControl_ServerBrowseAdd);
            addressSpaceControl.AddDaItem -= new DemoClient.TabControls.AddDaItemHandler(addressSpaceControl_AddDaItem);
            addressSpaceControl.AddDaItems -= new DemoClient.TabControls.AddDaItemsHandler(addressSpaceControl_AddDaItems);

        }

        #endregion

        #region Change State Images

        /// <summary>
        /// Add DisconnectConnect Timer
        /// </summary>
        /// <returns></returns>
        private CustomTimer AddDisconnectConnect(TreeNode node)
        {
            CustomTimer timer = new CustomTimer(node);
            timer.Interval = 200;
            timer.Tick -= new EventHandler(DisconnectConnectTimer);
            timer.Tick += new EventHandler(DisconnectConnectTimer);
            timer.Enabled = true;
            return timer;
        }

        /// <summary>
        /// Add DisconnectConnect Timer
        /// </summary>
        /// <returns></returns>
        private CustomTimer AddConnectActive(TreeNode node)
        {
            CustomTimer timer = new CustomTimer(node);
            timer.Interval = 200;
            timer.Tick -= new EventHandler(ConnectActivateTimer);
            timer.Tick += new EventHandler(ConnectActivateTimer);
            timer.Enabled = true;
            return timer;
        }

        /// <summary>
        /// Add DisconnectActivate Timer
        /// </summary>
        /// <returns></returns>
        private CustomTimer AddDisconnectActivate(TreeNode node)
        {
            CustomTimer timer = new CustomTimer(node);
            timer.Interval = 200;
            timer.Tick -= new EventHandler(DisconnActivateTimer);
            timer.Tick += new EventHandler(DisconnActivateTimer);
            timer.Enabled = true;
            return timer;
        }

        /// <summary>
        /// Disconnect Activate
        /// </summary>
        private void DisconnActivateTimer(object sender, EventArgs e)
        {
            CustomTimer timer = (CustomTimer)sender;
            if (timer == null) return;
            ChangeDisconnActivState(timer.Node);
        }

        /// <summary>
        /// Connect Activate
        /// </summary>
        private void ConnectActivateTimer(object sender, EventArgs e)
        {
            CustomTimer timer = (CustomTimer)sender;
            if (timer == null) return;
            ChangeConnActivState(timer.Node);
        }

        /// <summary>
        /// Disconnect Connect
        /// </summary>
        private void DisconnectConnectTimer(object sender, EventArgs e)
        {
            CustomTimer timer = (CustomTimer)sender;
            if (timer == null) return;
            ChangeDiconnConnState(timer.Node);
        }     

        /// <summary>
        /// Change State
        /// </summary>
        /// <param name="selectedNode"></param>
        /// <param name="deep"></param>
        private void ChangeDiconnConnState(TreeNode selectedNode)
        {
            SwitchDisconnConnIndex(selectedNode);
        }

        /// <summary>
        /// Change Image Index
        /// </summary>
        /// <param name="selectedNode"></param>
        private void SwitchDisconnConnIndex(TreeNode selectedNode)
        {
            switch (selectedNode.ImageIndex)
            {
                case DISCONNECTED:
                    SetNodeImage(selectedNode, DISCONN_CONN_0, false);
                    break;
                case DISCONN_CONN_0:
                    SetNodeImage(selectedNode, DISCONN_CONN_1, false);
                    break;
                case DISCONN_CONN_1:
                    SetNodeImage(selectedNode, DISCONN_CONN_2, false);
                    break;
                case DISCONN_CONN_2:
                    SetNodeImage(selectedNode, DISCONN_CONN_3, false);
                    break;
                case DISCONN_CONN_3:
                    SetNodeImage(selectedNode, DISCONN_CONN_0, false);
                    break;                
            }
        }


        // <summary>
        /// Change State
        /// </summary>
        /// <param name="selectedNode"></param>
        /// <param name="deep"></param>
        private void ChangeConnActivState(TreeNode stateNode)
        {
            SwitchConnActivIndex(stateNode);          
        }

        /// <summary>
        /// Change Image Index
        /// </summary>
        /// <param name="selectedNode"></param>
        private void SwitchConnActivIndex(TreeNode selectedNode)
        {
            switch (selectedNode.ImageIndex)
            {
                case CONNECTED:
                    SetNodeImage(selectedNode, CONN_ACTIV_0, false);
                    break;
                case CONN_ACTIV_0:
                    SetNodeImage(selectedNode, CONN_ACTIV_1, false);
                    break;
                case CONN_ACTIV_1:
                    SetNodeImage(selectedNode, CONN_ACTIV_2, false);
                    break;
                case CONN_ACTIV_2:
                    SetNodeImage(selectedNode, CONN_ACTIV_3, false);
                    break;
                case CONN_ACTIV_3:
                    SetNodeImage(selectedNode, CONN_ACTIV_0, false);
                    break;              
            }
        }

        /// <summary>
        /// Change State
        /// </summary>
        /// <param name="stateNode"></param>
        /// <param name="deep"></param>
        private void ChangeDisconnActivState(TreeNode stateNode)
        {
            SwitchDisconnActivIndex(stateNode);
        }       

        /// <summary>
        /// Change Image Index
        /// </summary>
        /// <param name="selectedNode"></param>
        private void SwitchDisconnActivIndex(TreeNode selectedNode)
        {
            switch (selectedNode.ImageIndex)
            {
                case DISCONNECTED:
                    SetNodeImage(selectedNode, DISCONN_ACTIV_0, false);
                    break;
                case DISCONN_ACTIV_0:
                    SetNodeImage(selectedNode, DISCONN_ACTIV_1, false);
                    break;
                case DISCONN_ACTIV_1:
                    SetNodeImage(selectedNode, DISCONN_ACTIV_2, false);
                    break;
                case DISCONN_ACTIV_2:
                    SetNodeImage(selectedNode, DISCONN_ACTIV_3, false);
                    break;
                case DISCONN_ACTIV_3:
                    SetNodeImage(selectedNode, DISCONN_ACTIV_0, false);
                    break;                
              }
        }

        #endregion

        #region Manage Session/Subscriptions

        #region Sessions

        /// <summary>
        /// Addd session
        /// </summary>
        private void AddSession(string strUrl, EnumOPCSpecification opcSpecification)
        {
            try
            {
                if (opcSpecification == EnumOPCSpecification.AE10)
                {
                    //add AE session and subscription
                    AddAeSession(strUrl);
                }
                else
                {
                    //add DaSession and Subcription
                    AddDaSession(strUrl);
                }
                AnyChange = true;
            }
            catch (Exception)
            {
            }

        }

        #region Add DaSession

        /// <summary>
        /// Add DaSession
        /// </summary>
        /// <param name="sb"></param>
        private void AddDaSession(string strUrl)
        {
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            DaSession session = new DaSession(strUrl);
            if (session == null) return;
            if (!session.Valid)
            {
                return;
            }

            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);

            DaSubscription subscription = new DaSubscription(1000, session);
            if (subscription == null) return;
            if (!subscription.Valid)
            {
                return;
            }
            subscription.Name = SUBSCRIPTION;

            subscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            subscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);

            subscription.DataChanged -= new DataChangedEventHandler(subscription_DataChanged);
            subscription.DataChanged += new DataChangedEventHandler(subscription_DataChanged);

            //create related nodes     
            TreeNode sessionNode = null;
            bool foundRootNode = false;
            foreach (TreeNode node in serversGroupsTreeView.Nodes)
            {
                if (node.Text == DATA_ACCESS)
                {
                    foundRootNode = true;
                    sessionNode = AddDaSessionNode(node, session, subscription);
                }
            }
            if (!foundRootNode)
            {
                TreeNode dataAccessNode = new TreeNode(DATA_ACCESS, ROOT, ROOT);
                serversGroupsTreeView.Nodes.Add(dataAccessNode);
                sessionNode = AddDaSessionNode(dataAccessNode, session, subscription);
            }
            //expand sesion node
            if (sessionNode != null)
            {
                sessionNode.Expand();
            }
            //Connect to the session          
            session.Connect(true, false, executionOptions);
        }        

        /// <summary>
        /// Add DaSessionNodes
        /// </summary>
        /// <param name="node"></param>
        /// <param name="session"></param>
        /// <param name="subscription"></param>
        private TreeNode AddDaSessionNode(TreeNode node, DaSession session, DaSubscription subscription)
        {
            TreeNode sessionNode = new TreeNode(session.Url, DISCONNECTED, DISCONNECTED);
            sessionNode.Tag = session;
            node.Nodes.Add(sessionNode);

            TreeNode subscriptionNode = new TreeNode(subscription.Name, DISCONNECTED, DISCONNECTED);
            subscriptionNode.Tag = subscription;
            sessionNode.Nodes.Add(subscriptionNode);

            serversGroupsTreeView.SelectedNode = sessionNode;

            //add session timer
            CustomTimer timer = AddDisconnectConnect(sessionNode);
            if (updateBrowsersDict.ContainsKey(session))
            {
                SetNodeImage(sessionNode, DISCONNECTED, false);
                updateBrowsersDict[session].Timer.Enabled = false;
                updateBrowsersDict[session] = new ObjectCollection(sessionNode, timer);
            }
            else
            {
                updateBrowsersDict.Add(session, new ObjectCollection(sessionNode, timer));
            }

            //add subsciption timer
            timer = AddDisconnectConnect(subscriptionNode);
            if (updateBrowsersDict.ContainsKey(subscription))
            {
                SetNodeImage(subscriptionNode, DISCONNECTED, false);
                updateBrowsersDict[subscription].Timer.Enabled = false;
                updateBrowsersDict[subscription] = new ObjectCollection(subscriptionNode, timer);
            }
            else
            {
                updateBrowsersDict.Add(subscription, new ObjectCollection(subscriptionNode, timer));
            }
            return sessionNode;

        }

        #endregion

        #region Add AeSession

        /// <summary>
        /// Add AeSession
        /// </summary>
        /// <param name="sb"></param>
        private void AddAeSession(string strUrl)
        {
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            AeSession session = new AeSession(strUrl);
            if (session == null) return;
            if (!session.Valid)
            {
                return;
            }

            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);

            AeSubscription subscription = new AeSubscription(session);
            if (subscription == null) return;
            if (!subscription.Valid)
            {
                return;
            }

            subscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            subscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);

            subscription.AeEventsReceived -= new AeEventsReceivedEventHandler(subscription_AeEventsReceived);
            subscription.AeEventsReceived += new AeEventsReceivedEventHandler(subscription_AeEventsReceived);

            subscription.AeConditionsChanged -= new AeConditionsChangedEventHandler(subscription_AeConditionsChanged);
            subscription.AeConditionsChanged += new AeConditionsChangedEventHandler(subscription_AeConditionsChanged);

            //create related nodes     
            TreeNode sessionNode = null;
            bool foundRootNode = false;
            foreach (TreeNode node in serversGroupsTreeView.Nodes)
            {
                if (node.Text == ALARM_AND_EVENTS)
                {
                    foundRootNode = true;
                    sessionNode = AddAeSessionNode(node, session, subscription);
                }
            }
            if (!foundRootNode)
            {
                TreeNode alarmEventsNode = new TreeNode(ALARM_AND_EVENTS, ROOT, ROOT);
                serversGroupsTreeView.Nodes.Add(alarmEventsNode);
                sessionNode = AddAeSessionNode(alarmEventsNode, session, subscription);
            }
            //expand sesion node
            if (sessionNode != null)
            {
                sessionNode.Expand();
            }
            //Connect to the session     
            session.Connect(true, false, executionOptions);
            subscription.RefreshAeConditions(executionOptions);

        }       

        /// <summary>
        /// Add AeSessionNodes
        /// </summary>
        /// <param name="node"></param>
        /// <param name="session"></param>
        /// <param name="subscription"></param>
        private TreeNode AddAeSessionNode(TreeNode node, AeSession session, AeSubscription subscription)
        {
            TreeNode sessionNode = new TreeNode(session.Url, DISCONNECTED, DISCONNECTED);
            sessionNode.Tag = session;
            node.Nodes.Add(sessionNode);

            TreeNode subscriptionNode = new TreeNode(SUBSCRIPTION, DISCONNECTED, DISCONNECTED);
            subscriptionNode.Tag = subscription;
            sessionNode.Nodes.Add(subscriptionNode);

            serversGroupsTreeView.SelectedNode = sessionNode;

            //add session timer
            CustomTimer timer = AddDisconnectConnect(sessionNode);
            if (updateBrowsersDict.ContainsKey(session))
            {
                SetNodeImage(sessionNode, DISCONNECTED, false);
                updateBrowsersDict[session].Timer.Enabled = false;
                updateBrowsersDict[session] = new ObjectCollection(sessionNode, timer);
            }
            else
            {
                updateBrowsersDict.Add(session, new ObjectCollection(sessionNode, timer));
            }

            //add subsciption timer
            timer = AddDisconnectConnect(subscriptionNode);
            if (updateBrowsersDict.ContainsKey(subscription))
            {
                SetNodeImage(subscriptionNode, DISCONNECTED, false);
                updateBrowsersDict[subscription].Timer.Enabled = false;
                updateBrowsersDict[subscription] = new ObjectCollection(subscriptionNode, timer);
            }
            else
            {
                updateBrowsersDict.Add(subscription, new ObjectCollection(subscriptionNode, timer));
            }
            return sessionNode;
        }

        #endregion

        #region Connect Sessions

        /// <summary>
        /// Connect all DaSessions
        /// </summary>
        private void ConnectAllDaSessions(TreeNode node)
        {
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            foreach (TreeNode daSessionNode in node.Nodes)
            {
                if (daSessionNode.Tag == null) continue;
                if (daSessionNode.Tag is DaSession)
                {
                    DeepConnectDaSession(daSessionNode);
                }
            }
        }

        /// <summary>
        /// Connect all AeSessions
        /// </summary>
        private void ConnectAllAeSessions(TreeNode node)
        {
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            foreach (TreeNode aeSessionNode in node.Nodes)
            {
                if (aeSessionNode.Tag == null) continue;
                if (aeSessionNode.Tag is AeSession)
                {
                    DeepConnectAeSession(aeSessionNode);
                }
            }
        }

        #endregion

        #region Activate Sessions

        /// <summary>
        /// Connect all DaSessions
        /// </summary>
        private void ActivateAllDaSessions(TreeNode node)
        {
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            foreach (TreeNode daSessionNode in node.Nodes)
            {
                if (daSessionNode.Tag == null) continue;
                if (daSessionNode.Tag is DaSession)
                {
                    DaSession session = (DaSession)daSessionNode.Tag;
                    if (session == null) continue;
                    session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                    session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                    session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                    switch (session.CurrentState)
                    {
                        case EnumObjectState.CONNECTED:
                            SetNodeImage(daSessionNode, CONNECTED, false);
                            if (updateBrowsersDict.ContainsKey(session))
                            {
                                updateBrowsersDict[session].Timer.Enabled = false;
                                updateBrowsersDict.Remove(session);
                            }
                            break;
                        case EnumObjectState.DISCONNECTED:
                            CustomTimer timer = AddDisconnectConnect(daSessionNode);
                            SetNodeImage(daSessionNode, DISCONNECTED, false);
                            if (updateBrowsersDict.ContainsKey(session))
                            {
                                updateBrowsersDict[session].Timer.Enabled = false;
                                updateBrowsersDict[session] = new ObjectCollection(daSessionNode, timer);
                            }
                            else
                            {
                                updateBrowsersDict.Add(session, new ObjectCollection(daSessionNode, timer));
                            }
                            break;
                    }
                    session.Connect(false, false, executionOptions);
                    foreach (TreeNode daSubscriptionNode in daSessionNode.Nodes)
                    {
                        ActivateDaSubscription(daSubscriptionNode, true);
                    }

                }
            }
        }

        /// <summary>
        /// Connect all AeSessions
        /// </summary>
        private void ActivateAllAeSessions(TreeNode node)
        {
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            foreach (TreeNode aeSessionNode in node.Nodes)
            {
                if (aeSessionNode.Tag == null) continue;
                if (aeSessionNode.Tag is AeSession)
                {
                    AeSession session = (AeSession)aeSessionNode.Tag;
                    if (session == null) continue;
                    session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                    session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                    session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                    switch (session.CurrentState)
                    {
                        case EnumObjectState.CONNECTED:
                            SetNodeImage(aeSessionNode, CONNECTED, false);
                            if (updateBrowsersDict.ContainsKey(session))
                            {
                                updateBrowsersDict[session].Timer.Enabled = false;
                                updateBrowsersDict.Remove(session);
                            }
                            break;
                        case EnumObjectState.DISCONNECTED:
                            SetNodeImage(aeSessionNode, DISCONNECTED, false);
                            CustomTimer timer = AddDisconnectConnect(aeSessionNode);
                            if (updateBrowsersDict.ContainsKey(session))
                            {
                                updateBrowsersDict[session].Timer.Enabled = false;
                                updateBrowsersDict[session] = new ObjectCollection(aeSessionNode, timer);
                            }
                            else
                            {
                                updateBrowsersDict.Add(session, new ObjectCollection(aeSessionNode, timer));
                            }
                            break;
                    }
                    session.Connect(false, false, executionOptions);
                    foreach (TreeNode aeSubscriptionNode in aeSessionNode.Nodes)
                    {
                        ActivateAeSubscription(aeSubscriptionNode, true);
                    }
                }
            }
        }

        #endregion

        #region Disconnect Sessions

        /// <summary>
        /// Disconnect all DaSession
        /// </summary>
        /// <param name="node"></param>
        private void DisconnectAllDaSessions(TreeNode node)
        {
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            foreach (TreeNode daSessionNode in node.Nodes)
            {
                if (daSessionNode.Tag == null) continue;
                if (daSessionNode.Tag is DaSession)
                {
                    DisconnectDaSession(daSessionNode);
                }
            }
        }

        /// <summary>
        /// Disconnect all AeSessions
        /// </summary>
        /// <param name="node"></param>
        private void DisconnectAllAeSessions(TreeNode node)
        {
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            foreach (TreeNode aeSessionNode in node.Nodes)
            {
                if (aeSessionNode.Tag == null) continue;
                if (aeSessionNode.Tag is AeSession)
                {
                    DisconnectAeSession(aeSessionNode);
                }
            }
        }

        #endregion

        #endregion       

        #region Session ContextMenu Actions

        public void AddSubscription()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;
            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession)))
            {
                DaSession daSession = (DaSession)selectedNode.Tag;
                if (daSession == null) return;
                //add daSubscription
                DaSubscription subscription = new DaSubscription(1000, daSession);
                if (subscription == null) return;
                if (!subscription.Valid)
                {
                    return;
                }
                TreeNode subscriptionNode = new TreeNode(subscription.Name, DISCONNECTED, DISCONNECTED);
                subscriptionNode.Tag = subscription;
                selectedNode.Nodes.Add(subscriptionNode);                

                subscription.DataChanged -= new DataChangedEventHandler(subscription_DataChanged);
                subscription.DataChanged += new DataChangedEventHandler(subscription_DataChanged);
                if (ResultCode.SUCCEEDED(subscription.Connect(true, true, null)))
                {
                    SetNodeImage(subscriptionNode, ACTIVATED, true);
                }

                serversGroupsTreeView.SelectedNode = subscriptionNode;

                //show properties window
                bool result = propertyWindow.DaSubscription(subscription);
                if (result)
                {
                    subscription.SetAttributesToServerCompleted -= new SetAttributesToServerEventHandler(subscription_SetAttributesToServerCompleted);
                    subscription.SetAttributesToServerCompleted += new SetAttributesToServerEventHandler(subscription_SetAttributesToServerCompleted);
                }
                propertyWindow.Show(dockPanel);


                

            }
            else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
            {
                AeSession aeSession = (AeSession)selectedNode.Tag;
                if (aeSession == null) return;
                //add aeSubscription
                AeSubscription subscription = new AeSubscription(aeSession);
                if (subscription == null) return;
                if (!subscription.Valid)
                {
                    return;
                }

                TreeNode subscriptionNode = new TreeNode(SUBSCRIPTION, DISCONNECTED, DISCONNECTED);
                subscriptionNode.Tag = subscription;
                selectedNode.Nodes.Add(subscriptionNode);

                subscription.AeEventsReceived -= new AeEventsReceivedEventHandler(subscription_AeEventsReceived);
                subscription.AeEventsReceived += new AeEventsReceivedEventHandler(subscription_AeEventsReceived);

                subscription.AeConditionsChanged -= new AeConditionsChangedEventHandler(subscription_AeConditionsChanged);
                subscription.AeConditionsChanged += new AeConditionsChangedEventHandler(subscription_AeConditionsChanged);

                //Connect to the aeSubscription
                if (ResultCode.SUCCEEDED(subscription.Connect(true, true, null)))
                {
                    SetNodeImage(subscriptionNode, ACTIVATED, true);
                }
                //set executions options
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;
                subscription.RefreshAeConditions(executionOptions);

                serversGroupsTreeView.SelectedNode = subscriptionNode;

                //show properties window
                bool result = propertyWindow.AeSubscription(subscription);
                propertyWindow.Show(dockPanel);
            }
        }

        /// <summary>
        /// Delete selected session
        /// </summary>
        private void DeleteSession()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;
            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession)))
            {
                DaSession daSession = (DaSession)selectedNode.Tag;
                if (daSession == null) return;
                DeleteDaSubscriptions(daSession);
                if (updateBrowsersDict.ContainsKey(daSession))
                {
                    updateBrowsersDict[daSession].Timer.Enabled = false;
                    updateBrowsersDict.Remove(daSession);
                }
                clientAppInstance.RemoveDaSession(daSession);
                selectedNode.Remove();
                bool hasChilds = false;
                TreeNode daNode = null;
                foreach (TreeNode node in serversGroupsTreeView.Nodes)
                {
                    if (node.Text == DATA_ACCESS)
                    {
                        daNode = node;
                        if (daNode.Nodes.Count > 0)
                        {
                            hasChilds = true;
                        }
                        break;
                    }
                }
                if (!hasChilds && daNode != null)
                {
                    daNode.Remove();
                }
            }
            else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
            {
                AeSession aeSession = (AeSession)selectedNode.Tag;
                if (aeSession == null) return;
                DeleteAeSubscriptions(aeSession);
                if (updateBrowsersDict.ContainsKey(aeSession))
                {
                    updateBrowsersDict[aeSession].Timer.Enabled = false;
                    updateBrowsersDict.Remove(aeSession);
                }
                clientAppInstance.RemoveAeSession(aeSession);
                selectedNode.Remove();
                bool hasChilds = false;
                TreeNode aeNode = null;
                foreach (TreeNode node in serversGroupsTreeView.Nodes)
                {
                    if (node.Text == ALARM_AND_EVENTS)
                    {
                        aeNode = node;
                        if (aeNode.Nodes.Count > 0)
                        {
                            hasChilds = true;
                        }
                        break;
                    }
                }
                if (!hasChilds && aeNode != null)
                {
                    aeNode.Remove();
                }
            }
            if (serversGroupsTreeView.Nodes.Count == 0)
            {
                addressSpaceControl.EmptyBrowse();
            }
            AnyChange = true;
        }

        /// <summary>
        /// Delete all DaSubscriptions
        /// </summary>
        /// <param name="daSession"></param>
        private void DeleteDaSubscriptions(DaSession daSession)
        {
            //remove all subscriptions
            while (daSession.SubscriptionList.Length > 0)
            {
                DeleteDaItems(daSession.SubscriptionList[0]);
                daSession.SubscriptionList[0].DataChanged -= new DataChangedEventHandler(subscription_DataChanged);
                daSession.RemoveDaSubscription(daSession.SubscriptionList[0]);
            }
        }

        /// <summary>
        /// Delete all AeSubscriptions
        /// </summary>
        /// <param name="aeSession"></param>
        private void DeleteAeSubscriptions(AeSession aeSession)
        {
            //remove all subscriptions
            while (aeSession.SubscriptionList.Length > 0)
            {
                aeSession.SubscriptionList[0].AeEventsReceived -= new AeEventsReceivedEventHandler(subscription_AeEventsReceived);
                aeSession.SubscriptionList[0].AeConditionsChanged -= new AeConditionsChangedEventHandler(subscription_AeConditionsChanged);     
                aeSession.RemoveAeSubscription(aeSession.SubscriptionList[0]);
            }
        }

        /// <summary>
        /// Connect in dept session
        /// </summary>
        private void DeepConnectSession()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;
            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession)))
            {
                DeepConnectDaSession(selectedNode);
                ShowSpecificContextMenu(selectedNode.Tag);
            }
            else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
            {
                DeepConnectAeSession(selectedNode);
                ShowSpecificContextMenu(selectedNode.Tag);
            }
        }

        /// <summary>
        /// Connect in dept DaSession
        /// </summary>
        /// <param name="aeSession"></param>
        private void DeepConnectDaSession(TreeNode daSessionNode)
        {
            DaSession session = (DaSession)daSessionNode.Tag;
            if (session == null) return;
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);

            switch (session.CurrentState)
            {
                case EnumObjectState.CONNECTED:
                    SetNodeImage(daSessionNode, CONNECTED, false);
                    if (updateBrowsersDict.ContainsKey(session))
                    {
                        updateBrowsersDict[session].Timer.Enabled = false;
                        updateBrowsersDict.Remove(session);
                    }
                    break;
                case EnumObjectState.DISCONNECTED:
                    //add session timer    
                    SetNodeImage(daSessionNode, DISCONNECTED, false);
                    CustomTimer timer = AddDisconnectConnect(daSessionNode);
                    if (updateBrowsersDict.ContainsKey(session))
                    {
                        updateBrowsersDict[session].Timer.Enabled = false;
                        updateBrowsersDict[session] = new ObjectCollection(daSessionNode, timer);
                    }
                    else
                    {
                        updateBrowsersDict.Add(session, new ObjectCollection(daSessionNode, timer));
                    }
                    break;
            }
            session.Connect(false, false, executionOptions);
            foreach (TreeNode daSubscriptionNode in daSessionNode.Nodes)
            {
                ConnectDaSubscription(daSubscriptionNode, true);
            }            
        }

        /// <summary>
        /// Connect in dept AeSession
        /// </summary>
        /// <param name="aeSession"></param>
        private void DeepConnectAeSession(TreeNode aeSessionNode)
        {
            AeSession session = (AeSession)aeSessionNode.Tag;
            if (session == null) return;
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            switch (session.CurrentState)
            {
                case EnumObjectState.CONNECTED:
                    SetNodeImage(aeSessionNode, CONNECTED, false);
                    if (updateBrowsersDict.ContainsKey(session))
                    {
                        updateBrowsersDict[session].Timer.Enabled = false;
                        updateBrowsersDict.Remove(session);
                    }
                    break;
                case EnumObjectState.DISCONNECTED:
                    //add session timer
                    SetNodeImage(aeSessionNode, DISCONNECTED, false);
                    CustomTimer timer = AddDisconnectConnect(aeSessionNode);
                    if (updateBrowsersDict.ContainsKey(session))
                    {
                        updateBrowsersDict[session].Timer.Enabled = false;
                        updateBrowsersDict[session] = new ObjectCollection(aeSessionNode, timer);
                    }
                    else
                    {
                        updateBrowsersDict.Add(session, new ObjectCollection(aeSessionNode, timer));
                    }
                    break;
            }
            session.Connect(false, false, executionOptions);
            foreach (TreeNode aeSubscriptionNode in aeSessionNode.Nodes)
            {
                ConnectAeSubscription(aeSubscriptionNode,true);
            }            
        }

        /// <summary>
        /// Connect Flat Session
        /// </summary>
        private void FlatConnectSession()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;
            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession)))
            {
                DaSession daSession = (DaSession)selectedNode.Tag;
                if (daSession == null) return;
                FlatConnectDaSession(selectedNode, daSession);
            }
            else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
            {
                AeSession aeSession = (AeSession)selectedNode.Tag;
                if (aeSession == null) return;
                FlatConnectAeSession(selectedNode, aeSession);
            }
        }

        /// <summary>
        /// Connect DaSession
        /// </summary>
        /// <param name="aeSession"></param>
        private void FlatConnectDaSession(TreeNode daSessionNode, DaSession daSession)
        {
            daSession.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            daSession.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            daSession.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            switch (daSession.CurrentState)
            {
                case EnumObjectState.CONNECTED:
                    SetNodeImage(daSessionNode, CONNECTED, false);
                    if (updateBrowsersDict.ContainsKey(daSession))
                    {
                        updateBrowsersDict[daSession].Timer.Enabled = false;
                        updateBrowsersDict.Remove(daSession);
                    }
                    break;
                case EnumObjectState.DISCONNECTED:
                    //add session timer    
                    SetNodeImage(daSessionNode, DISCONNECTED, false);
                    CustomTimer timer = AddDisconnectConnect(daSessionNode);
                    if (updateBrowsersDict.ContainsKey(daSession))
                    {
                        updateBrowsersDict[daSession].Timer.Enabled = false;
                        updateBrowsersDict[daSession] = new ObjectCollection(daSessionNode, timer);
                    }
                    else
                    {
                        updateBrowsersDict.Add(daSession, new ObjectCollection(daSessionNode, timer));
                    }
                    break;
            }
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            daSession.Connect(false, false, executionOptions);

        }

        /// <summary>
        /// Connect AeSession
        /// </summary>
        /// <param name="aeSession"></param>
        private void FlatConnectAeSession(TreeNode aeSessionNode, AeSession aeSession)
        {
            aeSession.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            aeSession.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            aeSession.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            switch (aeSession.CurrentState)
            {
                case EnumObjectState.CONNECTED:
                    SetNodeImage(aeSessionNode, CONNECTED, false);
                    if (updateBrowsersDict.ContainsKey(aeSession))
                    {
                        updateBrowsersDict[aeSession].Timer.Enabled = false;
                        updateBrowsersDict.Remove(aeSession);
                    }
                    break;
                case EnumObjectState.DISCONNECTED:
                    //add session timer
                    SetNodeImage(aeSessionNode, DISCONNECTED, false);
                    CustomTimer timer = AddDisconnectConnect(aeSessionNode);
                    if (updateBrowsersDict.ContainsKey(aeSession))
                    {
                        updateBrowsersDict[aeSession].Timer.Enabled = false;
                        updateBrowsersDict[aeSession] = new ObjectCollection(aeSessionNode, timer);
                    }
                    else
                    {
                        updateBrowsersDict.Add(aeSession, new ObjectCollection(aeSessionNode, timer));
                    }
                    break;
            }
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            aeSession.Connect(false, false, executionOptions);
        }

        /// <summary>
        /// Disconnect Session
        /// </summary>
        private void DisconnectSession()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;
            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession)))
            {
                DisconnectDaSession(selectedNode);
                ShowSpecificContextMenu(selectedNode.Tag);

            }
            else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
            {
                DisconnectAeSession(selectedNode);
                ShowSpecificContextMenu(selectedNode.Tag);
            }
        }

        /// <summary>
        /// Disconnect DaSession
        /// </summary>
        /// <param name="aeSession"></param>
        private void DisconnectDaSession(TreeNode daSessionNode)
        {
            DaSession session = (DaSession)daSessionNode.Tag;
            if (session == null) return;
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            switch (session.CurrentState)
            {
                case EnumObjectState.CONNECTED:
                    SetNodeImage(daSessionNode, DISCONN_CONN_0, false);
                    CustomTimer timer = AddDisconnectConnect(daSessionNode);
                    if (updateBrowsersDict.ContainsKey(session))
                    {
                        updateBrowsersDict[session].Timer.Enabled = false;
                        updateBrowsersDict[session] = new ObjectCollection(daSessionNode, timer);
                    }
                    else
                    {
                        updateBrowsersDict.Add(session, new ObjectCollection(daSessionNode, timer));
                    }
                    break;
                case EnumObjectState.DISCONNECTED:
                    SetNodeImage(daSessionNode, DISCONNECTED, false);
                    if (updateBrowsersDict.ContainsKey(session))
                    {
                        updateBrowsersDict[session].Timer.Enabled = false;
                        updateBrowsersDict.Remove(session);
                    }
                    break;
            }
            foreach (TreeNode daSubscriptionNode in daSessionNode.Nodes)
            {
                if (daSubscriptionNode.Tag == null) continue;
                if (daSubscriptionNode.Tag == null) continue;
                DaSubscription daSubscription = (DaSubscription)daSubscriptionNode.Tag;
                daSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                daSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                daSubscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                switch (daSubscription.CurrentState)
                {
                    case EnumObjectState.DISCONNECTED:
                        SetNodeImage(daSubscriptionNode, DISCONNECTED, true);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(daSubscription);
                        }
                        break;
                    case EnumObjectState.CONNECTED:
                        SetNodeImage(daSubscriptionNode, DISCONN_CONN_0, true);
                        CustomTimer timer = AddDisconnectConnect(daSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict[daSubscription] = new ObjectCollection(daSubscriptionNode, timer);
                        }
                        else
                        {
                            updateBrowsersDict.Add(daSubscription, new ObjectCollection(daSubscriptionNode, timer));
                        }
                        break;
                    case EnumObjectState.ACTIVATED:
                        SetNodeImage(daSubscriptionNode, DISCONN_ACTIV_0, true);
                        CustomTimer timerSub = AddDisconnectActivate(daSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(daSubscriptionNode))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict[daSubscription] = new ObjectCollection(daSubscriptionNode, timerSub);
                        }
                        else
                        {
                            updateBrowsersDict.Add(daSubscription, new ObjectCollection(daSubscriptionNode, timerSub));
                        }
                        break;
                }
            }
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            session.Disconnect(executionOptions);
        }

        /// <summary>
        /// Disconnect AeSession
        /// </summary>
        /// <param name="aeSession"></param>
        private void DisconnectAeSession(TreeNode aeSessionNode)
        {
            AeSession session = (AeSession)aeSessionNode.Tag;
            if (session == null) return;
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            switch (session.CurrentState)
            {
                case EnumObjectState.DISCONNECTED:
                    SetNodeImage(aeSessionNode, DISCONNECTED, false);
                    if (updateBrowsersDict.ContainsKey(session))
                    {
                        updateBrowsersDict[session].Timer.Enabled = false;
                        updateBrowsersDict.Remove(session);
                    }
                    break;
                case EnumObjectState.CONNECTED:
                    SetNodeImage(aeSessionNode, DISCONN_CONN_0, false);
                    CustomTimer timer = AddDisconnectConnect(aeSessionNode);
                    if (updateBrowsersDict.ContainsKey(session))
                    {
                        updateBrowsersDict[session].Timer.Enabled = false;
                        updateBrowsersDict[session] = new ObjectCollection(aeSessionNode, timer);
                    }
                    else
                    {
                        updateBrowsersDict.Add(session, new ObjectCollection(aeSessionNode, timer));
                    }
                    break;
            }
            foreach (TreeNode aeSubscriptionNode in aeSessionNode.Nodes)
            {
                if (aeSubscriptionNode == null) continue;
                if (aeSubscriptionNode.Tag == null) continue;
                AeSubscription aeSubscription = (AeSubscription)aeSubscriptionNode.Tag;
                aeSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                aeSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                aeSubscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                switch (aeSubscription.CurrentState)
                {
                    case EnumObjectState.DISCONNECTED:
                        SetNodeImage(aeSubscriptionNode, DISCONNECTED, true);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(aeSubscription);
                        }
                        break;
                    case EnumObjectState.CONNECTED:
                        SetNodeImage(aeSubscriptionNode, DISCONN_CONN_0, true);
                        CustomTimer timer = AddDisconnectConnect(aeSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict[aeSubscription] = new ObjectCollection(aeSubscriptionNode, timer);
                        }
                        else
                        {
                            updateBrowsersDict.Add(aeSubscription, new ObjectCollection(aeSubscriptionNode, timer));
                        }
                        break;
                    case EnumObjectState.ACTIVATED:
                        SetNodeImage(aeSubscriptionNode, DISCONN_ACTIV_0, true);
                        CustomTimer timerSubs = AddDisconnectActivate(aeSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict[aeSubscription] = new ObjectCollection(aeSubscriptionNode, timerSubs);
                        }
                        else
                        {
                            updateBrowsersDict.Add(aeSubscription, new ObjectCollection(aeSubscriptionNode, timerSubs));
                        }
                        break;
                }
            }
            //set executions options
            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
            executionOptions.ExecutionContext = 0;

            session.Disconnect(executionOptions);
        }

        /// <summary>
        /// Read Session Properties
        /// </summary>
        private void ReadSessionsProperties()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;
            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession)))
            {
                //DaSession Properties
                DaSession session = selectedNode.Tag as DaSession;
                if(session == null) return;
                bool result = propertyWindow.DaSession(session);
                propertyWindow.Show(dockPanel);

            }
            else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
            {
                //AeSession Properties
                AeSession session = selectedNode.Tag as AeSession;
                if (session == null) return;
                bool result = propertyWindow.AeSession(session);
                propertyWindow.Show(dockPanel);
            }

        }

        #endregion

        #region DaSubscription ContextMenu Actions

        /// <summary>
        /// Delete DaSubscription
        /// </summary>
        private void DeleteDaSubscription()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;
            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSubscription)))
            {
                DaSubscription daSubscription = (DaSubscription)selectedNode.Tag;
                if (daSubscription == null) return;
                DaSession daSession = daSubscription.DaSession;
                if (daSession == null) return;
                if(updateBrowsersDict.ContainsKey(daSubscription))
                {
                    updateBrowsersDict[daSubscription].Timer.Enabled = false;
                    updateBrowsersDict.Remove(daSubscription);
                }
                DeleteDaItems(daSubscription);
                daSession.RemoveDaSubscription(daSubscription);
                selectedNode.Remove();
            }
        }        

        /// <summary>
        /// Activate DaSubscription
        /// </summary>
        private void ActivateDaSubscription(TreeNode daSubscriptionNode,bool deep)
        {
            if (daSubscriptionNode == null) return;
            if (daSubscriptionNode.Tag == null) return;
            if (Type.ReferenceEquals(daSubscriptionNode.Tag.GetType(), typeof(DaSubscription)))
            {
                //set executions options
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;

                DaSubscription daSubscription = (DaSubscription)daSubscriptionNode.Tag;
                if (daSubscription == null) return;
                daSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                daSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                daSubscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                switch (daSubscription.CurrentState)
                {
                    case EnumObjectState.DISCONNECTED:
                        SetNodeImage(daSubscriptionNode, DISCONNECTED, false);
                        CustomTimer timer = AddDisconnectActivate(daSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict[daSubscription] = new ObjectCollection(daSubscriptionNode, timer);
                        }
                        else
                        {
                            updateBrowsersDict.Add(daSubscription, new ObjectCollection(daSubscriptionNode, timer));
                        }
                        break;
                    case EnumObjectState.CONNECTED:
                        CustomTimer timerSubs = AddConnectActive(daSubscriptionNode);
                        SetNodeImage(daSubscriptionNode, CONNECTED, false);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict[daSubscription] = new ObjectCollection(daSubscriptionNode, timerSubs);
                        }
                        else
                        {
                            updateBrowsersDict.Add(daSubscription, new ObjectCollection(daSubscriptionNode, timerSubs));
                        }
                        break;
                    case EnumObjectState.ACTIVATED:
                         SetNodeImage(daSubscriptionNode, ACTIVATED, false);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(daSubscription);
                        }
                        break;
                }
                daSubscription.Connect(deep, true, executionOptions);
            }
        }

        /// <summary>
        /// Connect DaSubcription
        /// </summary>
        /// <param name="deep"></param>
        private void ConnectDaSubscription(TreeNode daSubscriptionNode, bool deep)
        {
            if (daSubscriptionNode == null) return;
            if (daSubscriptionNode.Tag == null) return;
            if (Type.ReferenceEquals(daSubscriptionNode.Tag.GetType(), typeof(DaSubscription)))
            {
                //set executions options
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;

                DaSubscription daSubscription = (DaSubscription)daSubscriptionNode.Tag;
                if (daSubscription == null) return;
                daSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                daSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                daSubscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                switch (daSubscription.CurrentState)
                {
                    case EnumObjectState.DISCONNECTED:
                        //add session timer
                        SetNodeImage(daSubscriptionNode, DISCONNECTED, false);
                        CustomTimer timer = AddDisconnectConnect(daSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict[daSubscription] = new ObjectCollection(daSubscriptionNode, timer);
                        }
                        else
                        {
                            updateBrowsersDict.Add(daSubscription, new ObjectCollection(daSubscriptionNode, timer));
                        }
                        break;
                    case EnumObjectState.CONNECTED:
                        SetNodeImage(daSubscriptionNode, CONNECTED, false);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(daSubscription);
                        }
                        break;
                    case EnumObjectState.ACTIVATED:
                        SetNodeImage(daSubscriptionNode, CONN_ACTIV_0, true);
                        CustomTimer timerSubs = AddConnectActive(daSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict[daSubscription] = new ObjectCollection(daSubscriptionNode, timerSubs);
                        }
                        else
                        {
                            updateBrowsersDict.Add(daSubscription, new ObjectCollection(daSubscriptionNode, timerSubs));
                        }
                        break;
                }
                daSubscription.Connect(deep, false, executionOptions);
            }
        }

        /// <summary>
        /// Disconnect DaSubscription
        /// </summary>
        private void DisconnectDaSubscription(TreeNode daSubscriptionNode)
        {
            if (daSubscriptionNode == null) return;
            if (daSubscriptionNode.Tag == null) return;
            if (Type.ReferenceEquals(daSubscriptionNode.Tag.GetType(), typeof(DaSubscription)))
            {
                //set executions options
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;

                DaSubscription daSubscription = (DaSubscription)daSubscriptionNode.Tag;
                if (daSubscription == null) return;
                daSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                daSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                daSubscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                switch (daSubscription.CurrentState)
                {
                    case EnumObjectState.DISCONNECTED:
                        SetNodeImage(daSubscriptionNode, DISCONNECTED, true);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(daSubscription);
                        }
                        break;
                    case EnumObjectState.CONNECTED:
                        SetNodeImage(daSubscriptionNode, DISCONN_CONN_0, true);
                        CustomTimer timer = AddDisconnectConnect(daSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(daSubscription))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict[daSubscription] = new ObjectCollection(daSubscriptionNode, timer);
                        }
                        else
                        {
                            updateBrowsersDict.Add(daSubscription, new ObjectCollection(daSubscriptionNode, timer));
                        }
                        break;
                    case EnumObjectState.ACTIVATED:
                        SetNodeImage(daSubscriptionNode, DISCONN_ACTIV_0, true);
                        CustomTimer timerSub = AddDisconnectActivate(daSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(daSubscriptionNode))
                        {
                            updateBrowsersDict[daSubscription].Timer.Enabled = false;
                            updateBrowsersDict[daSubscription] = new ObjectCollection(daSubscriptionNode, timerSub);
                        }
                        else
                        {
                            updateBrowsersDict.Add(daSubscription, new ObjectCollection(daSubscriptionNode, timerSub));
                        }
                        break;
                }
                daSubscription.Disconnect(executionOptions);
            }
        }

        /// <summary>
        /// Read DaSubscription Properties
        /// </summary>
        private void ReadDaProperties()
        {
            TreeNode node = serversGroupsTreeView.SelectedNode;
            if (node == null) return;
            if (node.Tag == null) return;
            if (Type.ReferenceEquals(node.Tag.GetType(), typeof(DaSubscription)))
            {
                DaSubscription subscription = node.Tag as DaSubscription;
                if (subscription == null) return;
                bool result = propertyWindow.DaSubscription(subscription);
                if (result)
                {
                    subscription.SetAttributesToServerCompleted -= new SetAttributesToServerEventHandler(subscription_SetAttributesToServerCompleted);
                    subscription.SetAttributesToServerCompleted += new SetAttributesToServerEventHandler(subscription_SetAttributesToServerCompleted);
                }
                propertyWindow.Show(dockPanel);
            }
        }

        /// <summary>
        /// Refresh DaSubscription
        /// </summary>
        private void RefreshDaSubscription()
        {
           TreeNode node = serversGroupsTreeView.SelectedNode;
            if (node == null) return;
            if (node.Tag == null) return;
            if (Type.ReferenceEquals(node.Tag.GetType(), typeof(DaSubscription)))
            {
                DaSubscription subscription = node.Tag as DaSubscription;
                if (subscription == null) return;

                //set executions options
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;
                subscription.Refresh(MAX_AGE_DEVICE, executionOptions);
            }
        }

        #endregion

        #region AeSubcription ContextMenu Actions

        /// <summary>
        /// Delete AeSubscription
        /// </summary>
        private void DeleteAeSubscription()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;
            if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSubscription)))
            {
                AeSubscription aeSubscription = (AeSubscription)selectedNode.Tag;
                if (aeSubscription == null) return;
                AeSession aeSession = aeSubscription.AeSession;
                if (aeSession == null) return;
                if (updateBrowsersDict.ContainsKey(aeSubscription))
                {
                    updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                    updateBrowsersDict.Remove(aeSubscription);
                }
                aeSession.RemoveAeSubscription(aeSubscription);
                selectedNode.Remove();
            }
            AnyChange = true;
        }

        /// <summary>
        /// Activate AeSubscription
        /// </summary>
        private void ActivateAeSubscription(TreeNode aeSubscriptionNode,bool deep)
        {
            if (aeSubscriptionNode == null) return;
            if (aeSubscriptionNode.Tag == null) return;
            if (Type.ReferenceEquals(aeSubscriptionNode.Tag.GetType(), typeof(AeSubscription)))
            {
                //set executions options
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;

                AeSubscription aeSubscription = (AeSubscription)aeSubscriptionNode.Tag;
                if (aeSubscription == null) return;
                aeSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                aeSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                aeSubscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                switch (aeSubscription.CurrentState)
                {
                    case EnumObjectState.DISCONNECTED:
                        SetNodeImage(aeSubscriptionNode, DISCONNECTED, false);
                        CustomTimer timer = AddDisconnectConnect(aeSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict[aeSubscription] = new ObjectCollection(aeSubscriptionNode, timer);
                        }
                        else
                        {
                            updateBrowsersDict.Add(aeSubscription, new ObjectCollection(aeSubscriptionNode, timer));
                        }
                        break;
                    case EnumObjectState.CONNECTED:
                        SetNodeImage(aeSubscriptionNode, CONNECTED, false);
                        CustomTimer timerSubs = AddConnectActive(aeSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict[aeSubscription] = new ObjectCollection(aeSubscriptionNode, timerSubs);
                        }
                        else
                        {
                            updateBrowsersDict.Add(aeSubscription, new ObjectCollection(aeSubscriptionNode, timerSubs));
                        }
                        break;
                    case EnumObjectState.ACTIVATED:
                        SetNodeImage(aeSubscriptionNode, ACTIVATED, false);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(aeSubscription);
                        }
                        break;
                }
                aeSubscription.Connect(deep, true, executionOptions);
            }
        }

        /// <summary>
        /// Connect AeSubcription
        /// </summary>
        /// <param name="deep"></param>
        private void ConnectAeSubscription(TreeNode aeSubscriptionNode, bool deep)
        {
            if (aeSubscriptionNode == null) return;
            if (aeSubscriptionNode.Tag == null) return;
            if (Type.ReferenceEquals(aeSubscriptionNode.Tag.GetType(), typeof(AeSubscription)))
            {
                //set executions options
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;

                AeSubscription aeSubscription = (AeSubscription)aeSubscriptionNode.Tag;
                if (aeSubscription == null) return;
                aeSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                aeSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                aeSubscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                switch (aeSubscription.CurrentState)
                {
                    case EnumObjectState.DISCONNECTED:
                        //add session timer
                        SetNodeImage(aeSubscriptionNode, DISCONNECTED, false);
                        CustomTimer timer = AddDisconnectConnect(aeSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict[aeSubscription] = new ObjectCollection(aeSubscriptionNode, timer);
                        }
                        else
                        {
                            updateBrowsersDict.Add(aeSubscription, new ObjectCollection(aeSubscriptionNode, timer));
                        }
                        break;
                    case EnumObjectState.CONNECTED:
                        SetNodeImage(aeSubscriptionNode, CONNECTED, false);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(aeSubscription);
                        }
                        break;
                    case EnumObjectState.ACTIVATED:
                        SetNodeImage(aeSubscriptionNode, CONN_ACTIV_0, true);
                        CustomTimer timerSubs = AddConnectActive(aeSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict[aeSubscription] = new ObjectCollection(aeSubscriptionNode, timerSubs);
                        }
                        else
                        {
                            updateBrowsersDict.Add(aeSubscription, new ObjectCollection(aeSubscriptionNode, timerSubs));
                        }
                        break;
                }
                aeSubscription.Connect(deep, false, executionOptions);
            }
        }

        /// <summary>
        /// Disconnect AeSubscription
        /// </summary>
        private void DisconnectAeSubscription(TreeNode aeSubscriptionNode)
        {
            if (aeSubscriptionNode == null) return;
            if (aeSubscriptionNode.Tag == null) return;
            if (Type.ReferenceEquals(aeSubscriptionNode.Tag.GetType(), typeof(AeSubscription)))
            {
                //set executions options
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                executionOptions.ExecutionContext = 0;

                AeSubscription aeSubscription = (AeSubscription)aeSubscriptionNode.Tag;
                if (aeSubscription == null) return;
                aeSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                aeSubscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                aeSubscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                switch (aeSubscription.CurrentState)
                {
                    case EnumObjectState.DISCONNECTED:
                        SetNodeImage(aeSubscriptionNode, DISCONNECTED, true);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(aeSubscription);
                        }

                        break;
                    case EnumObjectState.CONNECTED:
                        SetNodeImage(aeSubscriptionNode, DISCONN_CONN_0, true);
                        CustomTimer timer = AddDisconnectConnect(aeSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict[aeSubscription] = new ObjectCollection(aeSubscriptionNode, timer);
                        }
                        else
                        {
                            updateBrowsersDict.Add(aeSubscription, new ObjectCollection(aeSubscriptionNode, timer));
                        }
                        break;
                    case EnumObjectState.ACTIVATED:
                        SetNodeImage(aeSubscriptionNode, DISCONN_ACTIV_0, true);
                        CustomTimer timerSubs = AddDisconnectActivate(aeSubscriptionNode);
                        if (updateBrowsersDict.ContainsKey(aeSubscription))
                        {
                            updateBrowsersDict[aeSubscription].Timer.Enabled = false;
                            updateBrowsersDict[aeSubscription] = new ObjectCollection(aeSubscriptionNode, timerSubs);
                        }
                        else
                        {
                            updateBrowsersDict.Add(aeSubscription, new ObjectCollection(aeSubscriptionNode, timerSubs));
                        }
                        break;
                }
                aeSubscription.Disconnect(executionOptions);
            }
        }

        /// <summary>
        /// Read AeProperties
        /// </summary>
        private void ReadAeProperties()
        {
            TreeNode node = serversGroupsTreeView.SelectedNode;
            if (node == null) return;
            if (node.Tag == null) return;
            if (Type.ReferenceEquals(node.Tag.GetType(), typeof(AeSubscription)))
            {
                AeSubscription subscription = node.Tag as AeSubscription;
                if (subscription == null) return;
                bool result = propertyWindow.AeSubscription(subscription);
                propertyWindow.Show(dockPanel);
            }
        }

        #endregion

        /// <summary>
       /// Set session node adn childrenNodes image indes
       /// </summary>
       /// <param name="sessionNode"></param>
       /// <param name="imageIndex"></param>
        private void SetNodeImage(TreeNode node, int imageIndex, bool updateChilds)
        {
            node.ImageIndex = imageIndex;
            node.SelectedImageIndex = imageIndex;
            if (updateChilds)
            {
                foreach (TreeNode childNode in node.Nodes)
                {
                    childNode.ImageIndex = imageIndex;
                    childNode.SelectedImageIndex = imageIndex;
                }
            }
        }      

        #endregion        

        #region Manage Items

        /// <summary>
        /// Add DaItem
        /// </summary>
        /// <param name="item"></param>
        /// <param name="subscription"></param>
        private void AddDaItem(string item, DaSubscription subscription)
        {
            if (subscription == null)
            {
                TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
                if (selectedNode == null) return;
                if (selectedNode.Tag == null) return;

                DaSession daSession = (DaSession)selectedNode.Tag;
                if (daSession == null) return;

                if (daSession.SubscriptionList.Length == 1)
                {
                    subscription = daSession.SubscriptionList[0];
                }
            }
            //add DaItem
            if (subscription != null)
            {
                //add item                
                DaItem daItem = new DaItem(item, subscription);
                if (!daItem.Valid)
                {
                    return;
                }
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                daItem.Connect(true, true, executionOptions);
            }
            AnyChange = true;
        }

        /// <summary>
        /// Add DaItems
        /// </summary>
        /// <param name="itemName"></param>
        /// <param name="subscription"></param>
        private void AddDaItems(string itemName, DaSubscription subscription)
        {
            //create subscription
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag == null) return;

            DaSession daSession = (DaSession)selectedNode.Tag;
            if (daSession == null) return;

            if (subscription == null)
            {               
                subscription = new DaSubscription(1000, daSession);
                subscription.Name = itemName;
                if (!subscription.Valid) return;

                TreeNode subscriptionNode = new TreeNode(subscription.Name, DISCONNECTED, DISCONNECTED);
                subscriptionNode.Tag = subscription;
                selectedNode.Nodes.Add(subscriptionNode);
                if (ResultCode.SUCCEEDED(subscription.Connect(false, true, null)))
                {
                    SetNodeImage(subscriptionNode, ACTIVATED, false);
                }
            }
            if(subscription != null)
            {
                executionOptions.ExecutionType = EnumExecutionType.SYNCHRONOUS;
                executionOptions.ExecutionContext = 0;

                //get element childs                
                DaGetPropertiesOptions propertyGetOptions = new DaGetPropertiesOptions();
                propertyGetOptions.WhatPropertyData = EnumPropertyData.ALL;
                DaAddressSpaceElement[] addressSpaceElements = null;

                //get branches
                DaAddressSpaceElementBrowseOptions browseOptionsBranch = new DaAddressSpaceElementBrowseOptions();
                browseOptionsBranch.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;
                if (ResultCode.SUCCEEDED(daSession.Browse(itemName,
                                                                    browseOptionsBranch,
                                                                    out addressSpaceElements,
                                                                    executionOptions)))
                {
                    for (int i = 0; i < addressSpaceElements.Length; i++)
                    {
                        //add item                
                        DaItem daItem = new DaItem(addressSpaceElements[i].ItemId, subscription);
                        if (!daItem.Valid)
                        {
                            return;
                        }
                        executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                        daItem.Connect(true, true, executionOptions);
                    }

                }
 
                //get leaves
                DaAddressSpaceElementBrowseOptions browseOptionsLeaf = new DaAddressSpaceElementBrowseOptions();
                browseOptionsLeaf.ElementTypeFilter = EnumAddressSpaceElementType.LEAF;
                if (ResultCode.SUCCEEDED(daSession.Browse(itemName,
                                                                  browseOptionsLeaf,
                                                                  out addressSpaceElements,
                                                                  executionOptions
                                                                                )))
                {
                    for (int i = 0; i < addressSpaceElements.Length; i++)
                    {
                        //add item                
                        DaItem daItem = new DaItem(addressSpaceElements[i].ItemId, subscription);
                        if (!daItem.Valid)
                        {
                            return;
                        }
                        executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                        daItem.Connect(true, true, executionOptions);
                    }                   
                }

                //get properties
                DaProperty[] properties = null;
                if (ResultCode.SUCCEEDED(daSession.GetDaProperties(itemName,
                                                                                   null,
                                                                                   propertyGetOptions,
                                                                                   out properties,
                                                                                   executionOptions)))
                {
                    if (properties == null) return;
                    for (int j = 0; j < properties.Length; j++)
                    {
                        if (properties[j].Id > 99)
                        {
                            //add item                
                            DaItem daItem = new DaItem(properties[j].ItemId, subscription);
                            if (!daItem.Valid)
                            {
                                return;
                            }
                            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                            daItem.Connect(true, true, executionOptions);
                        }
                    }
                }
            }
            AnyChange = true;

        }  

        /// <summary>
        /// Delete DaItems
        /// </summary>
        /// <param name="daSubscription"></param>
        private void DeleteDaItems(DaSubscription daSubscription)
        {
            itemsControl.DeleteDaItems(daSubscription);
            AnyChange = true;
        }

        /// <summary>
        /// Show Items
        /// </summary>
        private void ShowItems()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag != null && ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession)))
            {
                itemsControl.ShowItems(null, null, DemoClient.TabControls.ShownItems.Session, new DaSession[] { (selectedNode.Tag as DaSession) });
            }
            else if (selectedNode.Tag != null && ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSubscription)))
            {
                itemsControl.ShowItems((selectedNode.Tag as DaSubscription), null, DemoClient.TabControls.ShownItems.Subscription, null);
            }
            else
            {
                itemsControl.ShowItems(null, null, DemoClient.TabControls.ShownItems.All, clientAppInstance.DaSessionList);
            }
        }

        #endregion

        #region Manage Conditions

        /// <summary>
        /// Show Conditions
        /// </summary>
        private void ShowConditions()
        {
            TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
            if (selectedNode == null) return;
            if (selectedNode.Tag != null && ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
            {
                    conditionsControl.ConditionsAdd(null, null, DemoClient.TabControls.ShownConditions.Session, new AeSession[] { (selectedNode.Tag as AeSession) });
            }
            else if (selectedNode.Tag != null && ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSubscription)))
            {
                conditionsControl.ConditionsAdd((selectedNode.Tag as AeSubscription), null, DemoClient.TabControls.ShownConditions.Subscription, null);
            }
            else
            {
                conditionsControl.ConditionsAdd(null, null, DemoClient.TabControls.ShownConditions.All, clientAppInstance.AeSessionList);
            }
        }

        #endregion

        #region Closing Window

        /// <summary>
        /// Closing Window
        /// </summary>
        private void ClosingWindow()
        {
            //release all resources - items, conditions, events
            //deasign all events associated with items
            foreach (TreeNode node in serversGroupsTreeView.Nodes)
            {
                if (node.Text == DATA_ACCESS)
                {
                    //set executions options
                    executionOptions.ExecutionType = EnumExecutionType.SYNCHRONOUS;
                    executionOptions.ExecutionContext = 0;

                    foreach (TreeNode daSessionNode in node.Nodes)
                    {
                        if (daSessionNode.Tag == null) continue;
                        if (daSessionNode.Tag is DaSession)
                        {

                            //(daSessionNode.Tag as DaSession).Disconnect(executionOptions);
                            DeleteDaSubscriptions(daSessionNode.Tag as DaSession);
                            if (updateBrowsersDict.ContainsKey(daSessionNode.Tag as DaSession))
                            {
                                updateBrowsersDict[daSessionNode.Tag as DaSession].Timer.Enabled = false;
                                updateBrowsersDict.Remove(daSessionNode.Tag as DaSession);
                            }
                            clientAppInstance.RemoveDaSession(daSessionNode.Tag as DaSession);
                            
                        }
                    }
                }
                else if (node.Text == ALARM_AND_EVENTS)
                {
                    //set executions options
                    executionOptions.ExecutionType = EnumExecutionType.SYNCHRONOUS;
                    executionOptions.ExecutionContext = 0;

                    foreach (TreeNode aeSessionNode in node.Nodes)
                    {
                        if (aeSessionNode.Tag == null) continue;
                        if (aeSessionNode.Tag is AeSession)
                        {
                            //(aeSessionNode.Tag as AeSession).Disconnect(executionOptions);
                            DeleteAeSubscriptions(aeSessionNode.Tag as AeSession);
                            if (updateBrowsersDict.ContainsKey(aeSessionNode.Tag as AeSession))
                            {
                                updateBrowsersDict[aeSessionNode.Tag as AeSession].Timer.Enabled = false;
                                updateBrowsersDict.Remove(aeSessionNode.Tag as AeSession);
                            }
                            clientAppInstance.RemoveAeSession(aeSessionNode.Tag as AeSession);
                        }
                    }
                }
            }
        }      

        #endregion

        #region Serialize/Deserialize

        /// <summary>
        /// Serialize OPCLCient objects
        /// </summary>
        private void SerializeToFile()
        {
            if (String.IsNullOrEmpty(currentFilePath)) return;
            OPCToolboxClient opcClientLayout = new OPCToolboxClient();
            OPCToolboxClientSessions sessionsXML = new OPCToolboxClientSessions();

            List<OPCToolboxClientSessionsDASession> daSessionsXML = new List<OPCToolboxClientSessionsDASession>();
            List<OPCToolboxClientSessionsAESession> aeSessionsXML = new List<OPCToolboxClientSessionsAESession>();

            //buil DA objects
            foreach (DaSession daSession in clientAppInstance.DaSessionList)
            {
                OPCToolboxClientSessionsDASession sessionXML = new OPCToolboxClientSessionsDASession();
                List<OPCToolboxClientSessionsDASessionDASubscription> daSubscriptionsXML = new List<OPCToolboxClientSessionsDASessionDASubscription>();
                foreach (DaSubscription daSubscription in daSession.SubscriptionList)
                {
                    OPCToolboxClientSessionsDASessionDASubscription daSubscriptionXML = new OPCToolboxClientSessionsDASessionDASubscription();
                    List<OPCToolboxClientSessionsDASessionDASubscriptionDAItem> daItemsXML = new List<OPCToolboxClientSessionsDASessionDASubscriptionDAItem>();
                    foreach (DaItem daItem in daSubscription.ItemList)
                    {
                        OPCToolboxClientSessionsDASessionDASubscriptionDAItem daItemXML = new OPCToolboxClientSessionsDASessionDASubscriptionDAItem();
                        daItemXML.ItemId = daItem.Id;
                        daItemXML.ItemPath = daItem.Path;
                        daItemXML.RequestedDatatype = daItem.RequestedDatatype.ToString();
                        daItemXML.Deadband = daItem.Deadband;

                        daItemsXML.Add(daItemXML);
                    }
                    daSubscriptionXML.Deadband = daSubscription.Deadband;
                    daSubscriptionXML.KeepAliveTime = daSubscription.RevisedKeepAliveTime;
                    daSubscriptionXML.Name = daSubscription.Name;
                    daSubscriptionXML.UpdateRate = daSubscription.RevisedUpdateRate;
                    daSubscriptionXML.LocaleId = daSubscription.LocaleId;
                    daSubscriptionXML.Items = daItemsXML;

                    daSubscriptionsXML.Add(daSubscriptionXML);
                }
                sessionXML.ForcedOpcVersion = (OPCToolboxClientSessionsDASessionForcedOpcVersion)System.Enum.Parse(typeof(OPCToolboxClientSessionsDASessionForcedOpcVersion),
                                                                                                                        daSession.ForcedOpcSpecification.ToString());
                sessionXML.ForcedOpcVersionSpecified = true;
                sessionXML.ClientName = daSession.ClientName;
                sessionXML.DcomClsctx = daSession.ClassContext.ToString();
                sessionXML.LocaleId = daSession.LocaleId;
                sessionXML.Url = daSession.Url;
                sessionXML.Subscriptions = daSubscriptionsXML;

                daSessionsXML.Add(sessionXML);
            }
            //build AE objects
            foreach (AeSession aeSession in clientAppInstance.AeSessionList)
            {
                OPCToolboxClientSessionsAESession aeSessionXML = new OPCToolboxClientSessionsAESession();
                List<OPCToolboxClientSessionsAESessionAESubscription> aeSubsciptionsXML = new List<OPCToolboxClientSessionsAESessionAESubscription>();
                foreach (AeSubscription aeSubscription in aeSession.SubscriptionList)
                {
                    OPCToolboxClientSessionsAESessionAESubscription aeSubscriptionXML = new OPCToolboxClientSessionsAESessionAESubscription();
                    aeSubscriptionXML.BufferTime = aeSubscription.RevisedBufferTime;
                    aeSubscriptionXML.MaxSize = aeSubscription.RevisedMaxSize;

                    aeSubsciptionsXML.Add(aeSubscriptionXML);
                }
                aeSessionXML.ClientName = aeSession.ClientName;
                aeSessionXML.DcomClsctx = aeSession.ClassContext.ToString();
                aeSessionXML.LocaleId = aeSession.LocaleId;
                aeSessionXML.Url = aeSession.Url;
                aeSessionXML.Subscriptions = aeSubsciptionsXML;

                aeSessionsXML.Add(aeSessionXML);
            }
            sessionsXML.DASession = daSessionsXML;
            sessionsXML.AESession = aeSessionsXML;

            opcClientLayout.Sessions = sessionsXML;

            XmlSerializer serializer = new XmlSerializer(typeof(OPCToolboxClient));
            TextWriter writer = new StreamWriter(currentFilePath);
            bool succeded = true;
            try
            {
                serializer.Serialize(writer, opcClientLayout);
            }
            catch 
            {
                succeded = false;
            }
            writer.Close();
            if (succeded)
            {
                AnyChange = false;
            }

        }

        /// <summary>
        /// Deserialize objects and add them
        /// </summary>
        private void DeserializeFromFile()
        {
            if (!String.IsNullOrEmpty(currentFilePath))
            {
                //set execution options
                executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;

                // Deserialization
                OPCToolboxClient opcClientLayout = null;
                XmlSerializer serializer = new XmlSerializer(typeof(OPCToolboxClient));
                TextReader reader = new StreamReader(currentFilePath);
                try
                {
                    opcClientLayout = (OPCToolboxClient)serializer.Deserialize(reader);
                }
                catch { }
                reader.Close();

                if (opcClientLayout == null)
                {
                    currentFilePath = "";
                    return;
                }
                AnyChange = false; 
                //empty everyting
                NewFile(false);

                //empty timers dictionary
                updateBrowsersDict.Clear();

                //add the Data Access Root Node
                TreeNode dataAccessRootNode = null;
                if (opcClientLayout.Sessions.DASession.Count > 0)
                {
                    dataAccessRootNode = new TreeNode(DATA_ACCESS, ROOT, ROOT);
                    serversGroupsTreeView.Nodes.Add(dataAccessRootNode);
                }
                
                bool selectedFirstSessionNode = false;

                //add all DA objects
                foreach (OPCToolboxClientSessionsDASession daSessionXML in opcClientLayout.Sessions.DASession)
                {
                    //create DASession
                     DaSession session = CreateDaSession(daSessionXML);
                    //Create and Add tree node
                     if (session == null) continue;
                     TreeNode daSessionNode = CreateDaSessionTreeNode(session);
                     if (daSessionNode == null) continue;
                     dataAccessRootNode.Nodes.Add(daSessionNode);                    
                     if (!selectedFirstSessionNode)
                     {
                         serversGroupsTreeView.SelectedNode = daSessionNode;
                         selectedFirstSessionNode = true;
                     }

                    //connect Session
                     session.Connect(false, false, executionOptions);
                    foreach (OPCToolboxClientSessionsDASessionDASubscription daSubscriptionXML in daSessionXML.Subscriptions)
                    {
                        //create DaSubscription
                        DaSubscription daSubscription = CreateDaSubscription(daSubscriptionXML, session);
                        if (daSubscription == null) continue;
                        //Create and Add tree node
                        TreeNode daSubscriptionNode = CreateDaSubscriptionNode(daSubscription);
                        if (daSubscriptionNode == null) continue;
                        daSessionNode.Nodes.Add(daSubscriptionNode);

                        foreach (OPCToolboxClientSessionsDASessionDASubscriptionDAItem daItemXML in daSubscriptionXML.Items)
                        {
                            //createItem
                            DaItem daItem = CreateDaItem(daItemXML, daSubscription);                            
                        }

                        //activate DaSubscription and Items
                        daSubscription.Connect(true, true, executionOptions);
                    }

                    daSessionNode.Expand();
                }
                if (dataAccessRootNode != null)
                {
                    dataAccessRootNode.Expand();
                }

                //add the Alarm and Events Root Node
                TreeNode alarmEventsRootNode = null;
                if (opcClientLayout.Sessions.AESession.Count > 0)
                {
                    alarmEventsRootNode = new TreeNode(ALARM_AND_EVENTS, ROOT, ROOT);
                    serversGroupsTreeView.Nodes.Add(alarmEventsRootNode);
                }               
                
                //add all AE objects
                foreach (OPCToolboxClientSessionsAESession aeSessionXML in opcClientLayout.Sessions.AESession)
                {
                    //create AESession
                    AeSession session = CreateAeSession(aeSessionXML);
                    //Create and Add tree node
                    if (session == null) continue;
                    TreeNode aeSessionNode = CreateAeSessionTreeNode(session);
                    if (aeSessionNode == null) continue;
                    alarmEventsRootNode.Nodes.Add(aeSessionNode);                    
                    if (!selectedFirstSessionNode)
                    {
                        serversGroupsTreeView.SelectedNode = alarmEventsRootNode;
                        selectedFirstSessionNode = true;
                    }

                    //connect Session
                    session.Connect(false, false, executionOptions);
                    foreach (OPCToolboxClientSessionsAESessionAESubscription aeSubscriptionXML in aeSessionXML.Subscriptions)
                    {
                        //create AeSubscription
                        AeSubscription aeSubscription = CreateAeSubscription(aeSubscriptionXML, session);
                        if (aeSubscription == null) continue;
                        //Create and Add tree node
                        TreeNode aeSubscriptionNode = CreateAeSubscriptionNode(aeSubscription);
                        if (aeSubscriptionNode == null) continue;
                        aeSessionNode.Nodes.Add(aeSubscriptionNode);

                        //activate DaSubscription and Items
                        aeSubscription.Connect(false, true, executionOptions);
                    }

                    aeSessionNode.Expand();
                }
                if (alarmEventsRootNode != null)
                {
                    alarmEventsRootNode.Expand();
                }
                
            }
        }

        /// <summary>
        /// Create AeSubscription Tree Node
        /// </summary>
        /// <param name="subscription"></param>
        /// <returns></returns>
        private TreeNode CreateAeSubscriptionNode(AeSubscription subscription)
        {
            TreeNode subscriptionNode = new TreeNode(SUBSCRIPTION, DISCONNECTED, DISCONNECTED);
            subscriptionNode.Tag = subscription;

            //add subsciption timer
            CustomTimer timer = AddDisconnectActivate(subscriptionNode);
            if (updateBrowsersDict.ContainsKey(subscription))
            {
                SetNodeImage(subscriptionNode, DISCONNECTED, false);
                updateBrowsersDict[subscription].Timer.Enabled = false;
                updateBrowsersDict[subscription] = new ObjectCollection(subscriptionNode, timer);
            }
            else
            {
                updateBrowsersDict.Add(subscription, new ObjectCollection(subscriptionNode, timer));
            }

            return subscriptionNode;
        }

        /// <summary>
        /// Create AeSubscription from XML data
        /// </summary>
        /// <param name="aeSubscriptionXML"></param>
        /// <param name="session"></param>
        /// <returns></returns>
        private AeSubscription CreateAeSubscription(OPCToolboxClientSessionsAESessionAESubscription aeSubscriptionXML, AeSession session)
        {
            AeSubscription subscription = new AeSubscription(session);
            if (subscription == null) return null;
            if (!subscription.Valid)
            {
                return null;
            }

            subscription.RequestedBufferTime = aeSubscriptionXML.BufferTime;
            subscription.RequestedMaxSize = aeSubscriptionXML.MaxSize;

            subscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            subscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            subscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);

            subscription.AeEventsReceived -= new AeEventsReceivedEventHandler(subscription_AeEventsReceived);
            subscription.AeEventsReceived += new AeEventsReceivedEventHandler(subscription_AeEventsReceived);

            subscription.AeConditionsChanged -= new AeConditionsChangedEventHandler(subscription_AeConditionsChanged);
            subscription.AeConditionsChanged += new AeConditionsChangedEventHandler(subscription_AeConditionsChanged);

            return subscription;

        }

        /// <summary>
        /// Create AeSession Tree Node
        /// </summary>
        /// <param name="session"></param>
        /// <returns></returns>
        private TreeNode CreateAeSessionTreeNode(AeSession session)
        {
            TreeNode sessionNode = new TreeNode(session.Url, DISCONNECTED, DISCONNECTED);
            sessionNode.Tag = session;

            //add session timer
            CustomTimer timer = AddDisconnectConnect(sessionNode);
            if (updateBrowsersDict.ContainsKey(session))
            {
                SetNodeImage(sessionNode, DISCONNECTED, false);
                updateBrowsersDict[session].Timer.Enabled = false;
                updateBrowsersDict[session] = new ObjectCollection(sessionNode, timer);
            }
            else
            {
                updateBrowsersDict.Add(session, new ObjectCollection(sessionNode, timer));
            }

            return sessionNode;

        }

        /// <summary>
        /// Create AeSession from XML data
        /// </summary>
        /// <param name="aeSessionXML"></param>
        /// <returns></returns>
        private AeSession CreateAeSession(OPCToolboxClientSessionsAESession aeSessionXML)
        {
            AeSession session = new AeSession(aeSessionXML.Url);
            if (session == null) return null;
            if (!session.Valid)
            {
                return null;
            }
            
            session.ClientName = aeSessionXML.ClientName;
            session.ClassContext = (EnumClassContext)Enum.Parse(typeof(EnumClassContext), aeSessionXML.DcomClsctx);
            session.LocaleId = aeSessionXML.LocaleId;

            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);

            return session;
                   
        }

        /// <summary>
        /// Create DaItem from XML
        /// </summary>
        /// <param name="daItemXML"></param>
        /// <returns></returns>
        private DaItem CreateDaItem(OPCToolboxClientSessionsDASessionDASubscriptionDAItem daItemXML, DaSubscription subscription)
        {
            DaItem daItem = new DaItem(daItemXML.ItemId, subscription);
            if (daItem == null) return null;
            if (!daItem.Valid)
            {
                return null;
            }
            daItem.RequestedDatatype = Type.GetType(daItemXML.RequestedDatatype);
            daItem.Path = daItemXML.ItemPath;
            daItem.Deadband =  float.Parse(daItemXML.Deadband.ToString());

            return daItem;
        }

        /// <summary>
        /// Create DaSubscriprion TreeNode
        /// </summary>
        /// <param name="daSubscription"></param>
        /// <returns></returns>
        private TreeNode CreateDaSubscriptionNode(DaSubscription subscription)
        {
            TreeNode subscriptionNode = new TreeNode(subscription.Name, DISCONNECTED, DISCONNECTED);
            subscriptionNode.Tag = subscription;

            //add subsciption timer
            CustomTimer timer = AddDisconnectActivate(subscriptionNode);
            if (updateBrowsersDict.ContainsKey(subscription))
            {
                SetNodeImage(subscriptionNode, DISCONNECTED, false);
                updateBrowsersDict[subscription].Timer.Enabled = false;
                updateBrowsersDict[subscription] = new ObjectCollection(subscriptionNode, timer);
            }
            else
            {
                updateBrowsersDict.Add(subscription, new ObjectCollection(subscriptionNode, timer));
            }

            return subscriptionNode;
        }

        /// <summary>
        /// Create DaSubscription from XML data
        /// </summary>
        /// <param name="daSubscriptionXML"></param>
        /// <param name="session"></param>
        /// <returns></returns>
        private DaSubscription CreateDaSubscription(OPCToolboxClientSessionsDASessionDASubscription daSubscriptionXML, DaSession session)
        {
            DaSubscription subscription = new DaSubscription(uint.Parse(daSubscriptionXML.UpdateRate.ToString()), session);
            if (subscription == null) return null;
            if (!subscription.Valid)
            {
                return null;
            }
            subscription.Name = daSubscriptionXML.Name;
            subscription.Deadband = float.Parse(daSubscriptionXML.Deadband.ToString());
            subscription.LocaleId = daSubscriptionXML.LocaleId;
            subscription.RequestedKeepAliveTime = daSubscriptionXML.KeepAliveTime;

            subscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            subscription.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            subscription.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);

            subscription.DataChanged -= new DataChangedEventHandler(subscription_DataChanged);
            subscription.DataChanged += new DataChangedEventHandler(subscription_DataChanged);

            return subscription;

        }

        /// <summary>
        /// Create session tree node
        /// </summary>
        /// <param name="session"></param>
        /// <returns></returns>
        private TreeNode CreateDaSessionTreeNode(DaSession session)
        {
            TreeNode sessionNode = new TreeNode(session.Url, DISCONNECTED, DISCONNECTED);
            sessionNode.Tag = session;

            //add session timer
            CustomTimer timer = AddDisconnectConnect(sessionNode);
            if (updateBrowsersDict.ContainsKey(session))
            {
                SetNodeImage(sessionNode, DISCONNECTED, false);
                updateBrowsersDict[session].Timer.Enabled = false;
                updateBrowsersDict[session] = new ObjectCollection(sessionNode, timer);
            }
            else
            {
                updateBrowsersDict.Add(session, new ObjectCollection(sessionNode, timer));
            }

            return sessionNode;
        }

        /// <summary>
        /// Create an DaSession from de XML specidications
        /// </summary>
        /// <param name="daSessionXML"></param>
        /// <returns></returns>
        private DaSession CreateDaSession(OPCToolboxClientSessionsDASession daSessionXML)
        {
            DaSession session = new DaSession(daSessionXML.Url);
            if (session == null) return null;
            if (!session.Valid)
            {
                return null;
            }

            session.ClientName = daSessionXML.ClientName;
            session.ClassContext = (EnumClassContext)System.Enum.Parse(typeof(EnumClassContext), daSessionXML.DcomClsctx);
            session.ForcedOpcSpecification = (EnumOPCSpecification)System.Enum.Parse(typeof(EnumOPCSpecification), daSessionXML.ForcedOpcVersion.ToString());
            session.LocaleId = daSessionXML.LocaleId;

            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
            session.StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
            session.StateChangeCompleted += new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);

            return session;
        }

        #endregion

        #endregion

        #region Public Methods

        #region Constructor

        /// <summary>
        /// Window Constructor
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
        }

        #endregion       

        #region Manage Sessions

        /// <summary>
        /// Connect all Sessions
        /// </summary>
        public void ConnectAllSessions()
        {
            foreach (TreeNode node in serversGroupsTreeView.Nodes)
            {
                if (node.Text == DATA_ACCESS)
                {
                    ConnectAllDaSessions(node);
                }
                else if (node.Text == ALARM_AND_EVENTS)
                {
                    ConnectAllAeSessions(node);
                }
            }
            //enble/disable address space
            EnableDisableData();            
        }       

        /// <summary>
        /// Activate all Sessions
        /// </summary>
        public void ActivateAllSessions()
        {
            foreach (TreeNode node in serversGroupsTreeView.Nodes)
            {
                if (node.Text == DATA_ACCESS)
                {
                    ActivateAllDaSessions(node);
                }
                else if (node.Text == ALARM_AND_EVENTS)
                {
                    ActivateAllAeSessions(node);
                }
            }
            //enble/disable address space
            EnableDisableData();
        }       

        /// <summary>
        /// Disconnect all Sessions
        /// </summary>
        public void DisconnectAllSessions()
        {
            foreach (TreeNode node in serversGroupsTreeView.Nodes)
            {
                if (node.Text == DATA_ACCESS)
                {
                    DisconnectAllDaSessions(node);
                }
                else if (node.Text == ALARM_AND_EVENTS)
                {
                    DisconnectAllAeSessions(node);
                }
            }
            //enble/disable address space
            EnableDisableData();
        }       

        #endregion

        #region WriteDa Value

        public void WriteValue(string strValue)
        {
            if (tabControl.SelectedTab != null)
            {
                if (tabControl.SelectedTab == tabPageAddressSpace)
                {
                    addressSpaceControl.WriteDaValue(strValue);
                }
                else if (tabControl.SelectedTab == tabPageItems)
                {
                    itemsControl.WriteDaValue(strValue);
                }
            }
            AnyChange = true;
        }

        #endregion

        #region Menu/Toolbar Events

        /// <summary>
        /// Show Properties
        /// </summary>
        public void ShowProperties()
        {
            try
            {
                Cursor = Cursors.WaitCursor;
                if (serversGroupsTreeView.Focused)
                {
                    TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
                    if (selectedNode == null) return;
                    if (selectedNode.Tag == null) return;
                    if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession)))
                    {
                        //DaSession Properties
                        DaSession session = selectedNode.Tag as DaSession;
                        if (session == null) return;
                        bool result = propertyWindow.DaSession(session);
                        propertyWindow.Show(dockPanel);

                    }
                    else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
                    {
                        //AeSession Properties
                        AeSession session = selectedNode.Tag as AeSession;
                        if (session == null) return;
                        bool result = propertyWindow.AeSession(session);
                        propertyWindow.Show(dockPanel);
                    }
                    else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSubscription)))
                    {
                        DaSubscription subscription = selectedNode.Tag as DaSubscription;
                        if (subscription == null) return;
                        bool result = propertyWindow.DaSubscription(subscription);
                        if (result)
                        {
                            subscription.SetAttributesToServerCompleted -= new SetAttributesToServerEventHandler(subscription_SetAttributesToServerCompleted);
                            subscription.SetAttributesToServerCompleted += new SetAttributesToServerEventHandler(subscription_SetAttributesToServerCompleted);
                        }
                        propertyWindow.Show(dockPanel);
                    }
                    else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSubscription)))
                    {
                        AeSubscription subscription = selectedNode.Tag as AeSubscription;
                        if (subscription == null) return;
                        bool result = propertyWindow.AeSubscription(subscription);
                        propertyWindow.Show(dockPanel);
                    }
                }
                else if (tabControl.SelectedTab != null)
                {
                    if (addressSpaceControl.Focused || tabControl.SelectedTab == tabPageAddressSpace)
                    {
                        addressSpaceControl.ShowProperties();
                    }
                    else if (itemsControl.Focused || tabControl.SelectedTab == tabPageItems)
                    {
                        itemsControl.ShowProperties();
                    }
                }
            }
            catch
            { }
            finally
            {
                Cursor = Cursors.Default;
            }

        }

        /// <summary>
        /// Delete Action
        /// </summary>
        public void DeleteAction()
        {
            try
            {
                Cursor = Cursors.WaitCursor;
                if (serversGroupsTreeView.Focused)
                {
                    TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
                    if (selectedNode == null) return;
                    if (selectedNode.Tag == null) return;
                    if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession)))
                    {
                        //DaSession Delete
                        DaSession daSession = selectedNode.Tag as DaSession;
                        if (daSession == null) return;
                        DeleteDaSubscriptions(daSession);
                        if (updateBrowsersDict.ContainsKey(daSession))
                        {
                            updateBrowsersDict[daSession].Timer.Enabled = false;
                            updateBrowsersDict.Remove(daSession);
                        }
                        clientAppInstance.RemoveDaSession(daSession);
                        selectedNode.Remove();
                        bool hasChilds = false;
                        TreeNode daNode = null;
                        foreach (TreeNode node in serversGroupsTreeView.Nodes)
                        {
                            if (node.Text == DATA_ACCESS)
                            {
                                daNode = node;
                                if (daNode.Nodes.Count > 0)
                                {
                                    hasChilds = true;
                                }
                                break;
                            }
                        }
                        if (!hasChilds && daNode != null)
                        {
                            daNode.Remove();
                            itemsControl.EmptyItems();
                            itemsControl.Enabled = false;
                        }
                        

                    }
                    else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
                    {
                        //AeSession Delete
                        AeSession aeSession = (AeSession)selectedNode.Tag;
                        if (aeSession == null) return;
                        DeleteAeSubscriptions(aeSession);
                        if (updateBrowsersDict.ContainsKey(aeSession))
                        {
                            updateBrowsersDict[aeSession].Timer.Enabled = false;
                            updateBrowsersDict.Remove(aeSession);
                        }
                        clientAppInstance.RemoveAeSession(aeSession);
                        selectedNode.Remove();
                        bool hasChilds = false;
                        TreeNode aeNode = null;
                        foreach (TreeNode node in serversGroupsTreeView.Nodes)
                        {
                            if (node.Text == ALARM_AND_EVENTS)
                            {
                                aeNode = node;
                                if (aeNode.Nodes.Count > 0)
                                {
                                    hasChilds = true;
                                }
                                break;
                            }
                        }
                        if (!hasChilds && aeNode != null)
                        {
                            aeNode.Remove();
                            conditionsControl.EmptyConditions();
                            conditionsControl.Enabled = false;
                            eventsControl.EmptyEvents();
                            eventsControl.Enabled = false;
                        }

                    }
                    else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSubscription)))
                    {
                        DaSubscription subscription = selectedNode.Tag as DaSubscription;
                        if (subscription == null) return;
                        DaSession daSession = subscription.DaSession;
                        if (daSession == null) return;
                        if (updateBrowsersDict.ContainsKey(subscription))
                        {
                            updateBrowsersDict[subscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(subscription);
                        }
                        DeleteDaItems(subscription);
                        subscription.DataChanged -= new DataChangedEventHandler(subscription_DataChanged);
                        daSession.RemoveDaSubscription(subscription);
                        selectedNode.Remove();
                       
                    }
                    else if (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSubscription)))
                    {
                        AeSubscription subscription = selectedNode.Tag as AeSubscription;
                        if (subscription == null) return;
                        AeSession aeSession = subscription.AeSession;
                        if (aeSession == null) return;
                        if (updateBrowsersDict.ContainsKey(subscription))
                        {
                            updateBrowsersDict[subscription].Timer.Enabled = false;
                            updateBrowsersDict.Remove(subscription);
                        }
                        subscription.AeEventsReceived -= new AeEventsReceivedEventHandler(subscription_AeEventsReceived);
                        subscription.AeConditionsChanged -= new AeConditionsChangedEventHandler(subscription_AeConditionsChanged);     
                
                        aeSession.RemoveAeSubscription(subscription);
                        selectedNode.Remove();
                        
                    }
                    if (serversGroupsTreeView.Nodes.Count == 0)
                    {
                        addressSpaceControl.EmptyBrowse();
                    }
                }
                else if (tabControl.SelectedTab != null)
                {
                    if (tabControl.SelectedTab == tabPageItems)
                    {
                        itemsControl.DeleteItem();
                    }
                }
                AnyChange = true;
            }
            catch
            { }
            finally
            {
                Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// AddItem
        /// </summary>
        public void AddItem()
        {
            addressSpaceControl.AddItem();
            AnyChange = true;
        }

        /// <summary>
        /// Refresh tree
        /// </summary>
        public void RefreshTree()
        {
            if (tabControl.SelectedTab == null) return;
            if (tabControl.SelectedTab == tabPageSeverDiscovery)
            {
                //refresh Servers List
                serverDiscoveryControl.CreateTreeMainRoots();
            }
            else if (tabControl.SelectedTab == tabPageAddressSpace)
            {
                //refresh Address Space List
                TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
                if (selectedNode != null)
                {
                    addressSpaceControl.EmptyBrowse();
                    FillAddressSpace(selectedNode.Tag);
                }
            }
        }        

        #endregion

        #region Sort Option

        public void FillAddressSpace()
        {
             TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
             if (selectedNode != null)
             {
                 addressSpaceControl.EmptyBrowse();
                 FillAddressSpace(selectedNode.Tag);
             }
            
        }

        #endregion

        #region Save/Open/New File

        internal void NewFile(bool emptyFilePath)
        {
            //save the changes
            if (AnyChange)
            {
                DialogResult result = MessageBox.Show("Save changes?", "Demo Client", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
                if (result == DialogResult.Yes)
                {
                    SaveFile();
                }
                if (result == DialogResult.Cancel)
                {
                    return;
                }                
            }
            //refresh the window
            this.Refresh();
            if (RefreshWindow != null)
            {
                RefreshWindow(false);
            }

            //empty the curent data
            this.serversGroupsTreeView.Cursor = Cursors.WaitCursor;
            this.tabControl.Cursor = Cursors.WaitCursor;

            if (emptyFilePath)
            {
                currentFilePath = "";
            }

            //delete all            
            foreach (DaSession session in clientAppInstance.DaSessionList)
            {
                //session.Disconnect(null);
                foreach (DaSubscription daSubscription in session.SubscriptionList)
                {
                    itemsControl.DeleteDaItems(daSubscription);
                    daSubscription.DataChanged -= new DataChangedEventHandler(subscription_DataChanged);
                    daSubscription.Disconnect(null);
                    session.RemoveDaSubscription(daSubscription);
                }
                clientAppInstance.RemoveDaSession(session);
            }

            foreach (AeSession session in clientAppInstance.AeSessionList)
            {
                //session.Disconnect(null);
                foreach (AeSubscription aeSubscription in session.SubscriptionList)
                {
                    aeSubscription.AeEventsReceived -= new AeEventsReceivedEventHandler(subscription_AeEventsReceived);
                    aeSubscription.AeConditionsChanged -= new AeConditionsChangedEventHandler(subscription_AeConditionsChanged);
                    aeSubscription.Disconnect(null);
                    session.RemoveAeSubscription(aeSubscription);
                }
                clientAppInstance.RemoveAeSession(session);
            }
            serversGroupsTreeView.Nodes.Clear();
            addressSpaceControl.EmptyBrowse();
            itemsControl.EmptyItems();
            conditionsControl.EmptyConditions();
            eventsControl.EmptyEvents();
            addressSpaceControl.Enabled = false;
            itemsControl.Enabled = false;
            conditionsControl.Enabled = false;
            eventsControl.Enabled = false;

            propertyWindow.Hide();
            serviceWindow.Hide();
            AnyChange = false;
            this.serversGroupsTreeView.Cursor = Cursors.Default;
            this.tabControl.Cursor = Cursors.Default;
        }

        internal void OpenFile()
        {
            if (AnyChange)
            {
                DialogResult result = MessageBox.Show("Save changes?", "Demo Client", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
                if (result == DialogResult.Yes)
                {
                    SaveFile();
                }
                if (result == DialogResult.Cancel)
                {
                    return;
                }
            }

            OpenFileDialog openFileDlg = new OpenFileDialog();
            openFileDlg.Title = "Open OPC Client File";
            openFileDlg.Filter = "Softing OPC Client Files (*.soc)|*.soc|All files (*.*)|*.*";
            openFileDlg.DefaultExt = "*.soc";
            openFileDlg.RestoreDirectory = true;
            if (openFileDlg.ShowDialog() == DialogResult.OK)
            {
                string fileName = openFileDlg.FileName;
                currentFilePath = fileName;
            }
            if (RefreshWindow != null)
            {
                RefreshWindow(false);
            }
            this.Refresh();
            this.serversGroupsTreeView.Cursor = Cursors.WaitCursor;
            this.tabControl.Cursor = Cursors.WaitCursor;
            DeserializeFromFile();
            this.serversGroupsTreeView.Cursor = Cursors.Default;
            this.tabControl.Cursor = Cursors.Default;
        }

        internal void SaveFile()
        {
            if (String.IsNullOrEmpty(currentFilePath))
            {
                SaveFileDialog saveDlg = new SaveFileDialog();
                saveDlg.DefaultExt = "*.soc";
                saveDlg.Title = "Save OPC Client File";
                saveDlg.RestoreDirectory = true;
                saveDlg.Filter = "Softing OPC Client Files (*.soc)|*.soc|All files (*.*)|*.*";
                if (saveDlg.ShowDialog() == DialogResult.OK &&
                    !String.IsNullOrEmpty(saveDlg.FileName))
                {
                    currentFilePath = saveDlg.FileName;
                }
                //add file in the last files list
                if (lastSavedFiles.Count == 4)
                {
                    lastSavedFiles.RemoveAt(lastSavedFiles.Count - 1);
                }
                lastSavedFiles.Insert(0,new FilesStruct(Path.GetFileNameWithoutExtension(currentFilePath), currentFilePath));
                Options.LastSavedFiles = lastSavedFiles;
            }
            if (RefreshWindow != null)
            {
                RefreshWindow(true);
            }
            this.Refresh();
            this.serversGroupsTreeView.Cursor = Cursors.WaitCursor;
            this.tabControl.Cursor = Cursors.WaitCursor;
            //Save the file
            SerializeToFile();
            this.serversGroupsTreeView.Cursor = Cursors.Default;
            this.tabControl.Cursor = Cursors.Default;
        }

        internal void OpenSavedFile(string filePath)
        {
            currentFilePath = filePath;
            this.serversGroupsTreeView.Cursor = Cursors.WaitCursor;
            this.tabControl.Cursor = Cursors.WaitCursor;
            
            DeserializeFromFile();
            this.serversGroupsTreeView.Cursor = Cursors.Default;
            this.tabControl.Cursor = Cursors.Default;
        }

        internal void SaveAsFile()
        {
            SaveFileDialog saveDlg = new SaveFileDialog();
            saveDlg.DefaultExt = "*.soc";
            saveDlg.Title = "Save OPC Client File";
            saveDlg.RestoreDirectory = true;
            saveDlg.Filter = "Softing OPC Client Files (*.soc)|*.soc|All files (*.*)|*.*";
            if (saveDlg.ShowDialog() == DialogResult.OK &&
                !String.IsNullOrEmpty(saveDlg.FileName))
            {
                currentFilePath = saveDlg.FileName;
            }

            //add file in the last files list
            bool foundFile = false;
            FilesStruct newFile = new FilesStruct(Path.GetFileNameWithoutExtension(currentFilePath), currentFilePath);
            foreach (FilesStruct file in lastSavedFiles)    
            {                
                if (newFile.FilePath == file.FilePath)
                {                    
                    foundFile = true;
                }
            }
            if (foundFile)
            {
                lastSavedFiles.Remove(newFile);
            }
            else if(!foundFile &&lastSavedFiles.Count == 4)
            {
                lastSavedFiles.RemoveAt(lastSavedFiles.Count - 1);
            }
            lastSavedFiles.Insert(0, newFile);
            Options.LastSavedFiles = lastSavedFiles;

            if (RefreshWindow != null)
            {
                RefreshWindow(true);
            }
            this.Refresh();
            this.serversGroupsTreeView.Cursor = Cursors.WaitCursor;
            this.tabControl.Cursor = Cursors.WaitCursor;
            //Save the file
            SerializeToFile();
            this.serversGroupsTreeView.Cursor = Cursors.Default;
            this.tabControl.Cursor = Cursors.Default;
        }

        #endregion

        #endregion

        #region Event Handlers

        #region Loading the Window

        /// <summary>
        /// Loading the window
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainWindow_Load(object sender, EventArgs e)
        {
            //disable all not needed tabs
            DisableTabs();            

            //hook events
            HookEvents();

            itemsControl.ServiceWindow = this.serviceWindow;
            itemsControl.PropertyWindow = this.propertyWindow;
            itemsControl.MainDockPanel = this.dockPanel;

            addressSpaceControl.ServiceWindow = this.serviceWindow;
            addressSpaceControl.PropertyWindow = this.propertyWindow;
            addressSpaceControl.MainDockPanel = this.dockPanel;

            itemsControl.EnableButtons -= new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);
            itemsControl.EnableButtons += new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);

            addressSpaceControl.EnableButtons -= new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);
            addressSpaceControl.EnableButtons += new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);

            serverDiscoveryControl.EnableButtons -= new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);
            serverDiscoveryControl.EnableButtons += new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);

            eventsControl.EnableButtons -= new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);
            eventsControl.EnableButtons += new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);

            conditionsControl.EnableButtons -= new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);
            conditionsControl.EnableButtons += new DemoClient.TabControls.EnableButtonsHandler(EnableDisableButtons);

            lastSavedFiles = Options.LastSavedFiles;
        }
       
        #endregion

        #region Buttons Disbale/Enable

        void EnableDisableButtons(bool enableDelete, bool enableProperties, bool enableAddItem, bool enableAddSubscription, bool enableRefresh)
        {
            if (EnableButtons != null)
            {
                EnableButtons(enableDelete, enableProperties, enableAddItem, enableAddSubscription, enableRefresh);
            }
        }

        #endregion

        #region Browse Data Management

        void serverDiscoveryControl_ServerBrowseAdd(object sender, string strURl, EnumOPCSpecification opcSpecification)
        {
            AddSession(strURl, opcSpecification);            
        }

        #endregion

        #region Times 

        /// <summary>
        /// Change the node icon to simulate work 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void disconnActivateState(object sender, EventArgs e)
        {    
            DisconnActivateTimer(sender,e );
        }       

        /// <summary>
        /// Change the node icon to simulate work 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void connActivateState(object sender, EventArgs e)
        {            
            ConnectActivateTimer(sender, e);
        }            

        /// <summary>
        /// Change the node icon to simulate work 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void disconnConnState(object sender, EventArgs e)
        {       
            DisconnectConnectTimer(sender, e);
        }      

        #endregion        

        #region Sessions Tree Management

        /// <summary>
        /// add selected node to the dabrowse
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void serversGroupsTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            isWriteBarEnabled = false;
            isDeleteEnabled = false;
            isPropertiesEnabled = false;
            bool enableAddSubscription = false;

            if (tabControl.SelectedTab != null)
            {
                if (tabControl.SelectedTab == tabPageItems)
                {
                    ShowItems();
                }
                else if (tabControl.SelectedTab == tabPageConditions)
                {
                    ShowConditions();
                }
            }
            
            //show proper context menu            
            if (e.Node.Tag == null)
            {
                serversGroupsTreeView.ContextMenuStrip = null;
            }
            else
            {
                ShowSpecificContextMenu(e.Node.Tag);
                if ((Type.ReferenceEquals(e.Node.Tag.GetType(), typeof(DaSession)) ||
                     Type.ReferenceEquals(e.Node.Tag.GetType(), typeof(DaSubscription))) && 
                     addressSpaceControl.Enabled)
                {
                    isWriteBarEnabled = true;

                }
                if (Type.ReferenceEquals(e.Node.Tag.GetType(), typeof(DaSession)) ||
                     Type.ReferenceEquals(e.Node.Tag.GetType(), typeof(AeSession)))
                {
                    enableAddSubscription = true;
                }
            }
            if (tabControl.SelectedTab == tabPageItems && itemsControl.Enabled)
            {
                isWriteBarEnabled = true;
            }
            if (EnableWriteBar != null)
            {
                EnableWriteBar(isWriteBarEnabled);
            }

            if (serversGroupsTreeView.SelectedNode.Text != DATA_ACCESS ||
                serversGroupsTreeView.SelectedNode.Text != ALARM_AND_EVENTS)
            {
                isDeleteEnabled = true;
                isPropertiesEnabled = true;
            }
            if (!serversGroupsTreeView.Focused)
            {
                enableAddSubscription = false;
            }
            if (EnableButtons != null)
            {
                EnableButtons(isDeleteEnabled, isPropertiesEnabled, false, enableAddSubscription, false);
            }
            
        }

        /// <summary>
        /// Enable Disable Buttons
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void serversGroupsTreeView_Enter(object sender, EventArgs e)
        {
            isDeleteEnabled = false;
            isPropertiesEnabled = false;
            bool enableAddSubscription = false;

            if (serversGroupsTreeView.SelectedNode != null)
            {
                if (serversGroupsTreeView.SelectedNode.Text != DATA_ACCESS ||
                    serversGroupsTreeView.SelectedNode.Text != ALARM_AND_EVENTS)
                {
                    isDeleteEnabled = true;
                    isPropertiesEnabled = true;
                }
                if (serversGroupsTreeView.SelectedNode.Tag != null)
                {
                    if (Type.ReferenceEquals(serversGroupsTreeView.SelectedNode.Tag.GetType(), typeof(DaSession)) ||
                        (Type.ReferenceEquals(serversGroupsTreeView.SelectedNode.Tag.GetType(), typeof(AeSession))))
                    {
                        enableAddSubscription = true;
                    }
                }
            }
            if (EnableButtons != null)
            {
                EnableButtons(isDeleteEnabled, isPropertiesEnabled, false, enableAddSubscription, false);
            }
        }

        #endregion

        #region Window Closing

        /// <summary>
        /// On closing disconnect all sessions
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            ClosingWindow();
        }
        
        #endregion        

        #region Session ContextMenu

        /// <summary>
        /// Add session subscription
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void addSubscriptionToolStripMenuItem_Click(object sender, EventArgs e)
        {            
            AddSubscription();
            AnyChange = true;
        }       

        /// <summary>
        /// Delete Session
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DeleteSession();
            AnyChange = true;
        }       

        /// <summary>
        /// Connect deep
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void deepToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DeepConnectSession();
        } 

        /// <summary>
        /// Connect Flat
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void flatToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FlatConnectSession();
        }  

        /// <summary>
        /// Disconnect
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void disconnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DisconnectSession();
        }

        /// <summary>
        /// Session properties
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void propertiesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ReadSessionsProperties();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }               

        #endregion    

        #region DaSubscription ContextMenu

        /// <summary>
        /// Delete DaSubscription
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void daDeleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DeleteDaSubscription();
            AnyChange = true;
        }               

        /// <summary>
        /// Activate Subscription
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void daDeepActivateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ActivateDaSubscription(serversGroupsTreeView.SelectedNode, true);
        }       

        /// <summary>
        /// Actovate DaSubscription
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void daFlatActivateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ActivateDaSubscription(serversGroupsTreeView.SelectedNode, false);
        }

        /// <summary>
        /// Connect DaSubcription
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void daDeepConnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ConnectDaSubscription(serversGroupsTreeView.SelectedNode, true);
        }      

        /// <summary>
        /// Connect DaSubsciption
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void daFlatConnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ConnectDaSubscription(serversGroupsTreeView.SelectedNode, false);
        }

        /// <summary>
        /// Disconnect DaSubscription
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void daDisconnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DisconnectDaSubscription(serversGroupsTreeView.SelectedNode);
        }       

        /// <summary>
        /// Refresh DaSubcription
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void daRefreshToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                RefreshDaSubscription();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }
       
        /// <summary>
        /// DaSubscription Properties
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void daPropertiesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ReadDaProperties();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// Update DaSubscription
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="executionContext"></param>
        /// <param name="whatAttributes"></param>
        /// <param name="results"></param>
        /// <param name="result"></param>
        void subscription_SetAttributesToServerCompleted(ObjectSpaceElement obj, uint executionContext, EnumObjectAttribute[] whatAttributes, int[] results, int result)
        {
            if (this.InvokeRequired)
            {
                Softing.OPCToolbox.Client.SetAttributesToServerEventHandler callback = new Softing.OPCToolbox.Client.SetAttributesToServerEventHandler(subscription_SetAttributesToServerCompleted);
                this.Invoke(callback, new object[] { obj, executionContext, whatAttributes, results, result });
                return;
            }

            if (Type.ReferenceEquals(obj.GetType(), typeof(DaSubscription)))
            {
                foreach (TreeNode node in serversGroupsTreeView.Nodes)
                {
                    if (node.Text == DATA_ACCESS)
                    {
                        foreach (TreeNode sessionNode in node.Nodes)
                        {
                            foreach (TreeNode subscriptionNode in sessionNode.Nodes)
                            {
                                if (subscriptionNode == null) continue;
                                if (subscriptionNode.Tag == null) continue;
                                if (Type.ReferenceEquals(subscriptionNode.Tag.GetType(), typeof(DaSubscription)) &&
                                            ((obj as DaSubscription) == (subscriptionNode.Tag as DaSubscription)))
                                {
                                    subscriptionNode.Text = (obj as DaSubscription).Name;
                                }
                            }
                        }
                    }
                 }
            }
        }

        #endregion

        #region AeSubcription ContextMenu

        private void aeDeleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DeleteAeSubscription();
            AnyChange = true;
        }

        private void aeDeepActivateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ActivateAeSubscription(serversGroupsTreeView.SelectedNode, true);
        }

        private void aeFlatActivateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ActivateAeSubscription(serversGroupsTreeView.SelectedNode, false);
        }

        private void aeDeepConnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ConnectAeSubscription(serversGroupsTreeView.SelectedNode, true);
        }

        private void aeFlatConnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ConnectAeSubscription(serversGroupsTreeView.SelectedNode, false);
        }

        private void aeDisconnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DisconnectAeSubscription(serversGroupsTreeView.SelectedNode);
        }

        private void aePropertiesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ReadAeProperties();
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }       

        #endregion

        #region Handle State Changed

        /// <summary>
        /// Handle State Changed
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="state"></param>
        void HandleStateChangedOnAddSession(ObjectSpaceElement obj, EnumObjectState state)
        {
            try
            {
                if (this.InvokeRequired)
                {
                    Softing.OPCToolbox.Client.StateChangeEventHandler callback = new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                    this.Invoke(callback, new object[] { obj, state });
                    return;
                }

                if (Type.ReferenceEquals(obj.GetType(), typeof(DaSession)))
                {
                    switch (state)
                    {
                        case EnumObjectState.CONNECTED:
                            ObjectCollection objCollection = null;
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, CONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);

                            }
                            
                            //Activate to the subscription
                            //set executions options
                            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                            executionOptions.ExecutionContext = 0;

                            if (((DaSession)obj).SubscriptionList.Length > 0)
                            {
                                //set node which state must be updated
                                if (objCollection == null) return;
                                if (objCollection.Node.Nodes.Count > 0)
                                {
                                    SetNodeImage(objCollection.Node.Nodes[0], CONNECTED, false);
                                }                                
                                Timer timer = AddConnectActive(objCollection.Node.Nodes[0]);
                                if (updateBrowsersDict.ContainsKey(((DaSession)obj).SubscriptionList[0]))
                                {
                                    updateBrowsersDict[((DaSession)obj).SubscriptionList[0]].Timer.Enabled = false;
                                    updateBrowsersDict[((DaSession)obj).SubscriptionList[0]] = new ObjectCollection(objCollection.Node.Nodes[0], timer);
                                }
                                else
                                {
                                    updateBrowsersDict.Add(((DaSession)obj).SubscriptionList[0], new ObjectCollection(objCollection.Node.Nodes[0], timer));
                                }
                                ((DaSession)obj).SubscriptionList[0].Connect(true, true, executionOptions);
                            }
                             ShowSpecificContextMenu(obj);
                            ((DaSession)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                            break;
                        default:
                            break;
                    }
                }
                else if (Type.ReferenceEquals(obj.GetType(), typeof(AeSession)))
                {
                    switch (state)
                    {
                        case EnumObjectState.CONNECTED:
                            ObjectCollection objCollection = null;
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, CONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }

                            //Activate to the subscription
                            //set executions options
                            executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
                            executionOptions.ExecutionContext = 0;
                            //set node which state must be updated
                            if (objCollection == null) return;
                            if (objCollection.Node.Nodes.Count > 0)
                            {
                                SetNodeImage(objCollection.Node.Nodes[0], CONNECTED, false);
                            }                            
                            Timer timer = AddConnectActive(objCollection.Node.Nodes[0]);
                            if (updateBrowsersDict.ContainsKey(((AeSession)obj).SubscriptionList[0]))
                            {
                                updateBrowsersDict[((AeSession)obj).SubscriptionList[0]].Timer.Enabled = false;
                                updateBrowsersDict[((AeSession)obj).SubscriptionList[0]] = new ObjectCollection(objCollection.Node.Nodes[0], timer);
                            }
                            else
                            {
                                updateBrowsersDict.Add(((AeSession)obj).SubscriptionList[0], new ObjectCollection(objCollection.Node.Nodes[0], timer));
                            }
                            ((AeSession)obj).SubscriptionList[0].Connect(true, true, executionOptions);
                            ShowSpecificContextMenu(obj);
                            ((AeSession)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                            break;
                        default:
                              break;
                    }
                }
                else if (Type.ReferenceEquals(obj.GetType(), typeof(DaSubscription)))
                {
                    switch (state)
                    {
                        case EnumObjectState.ACTIVATED:
                            ObjectCollection objCollection = null;
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, ACTIVATED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ((DaSubscription)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                             break;
                        default:
                            break;
                    }
                }
                else if (Type.ReferenceEquals(obj.GetType(), typeof(AeSubscription)))
                {
                    switch (state)
                    {
                        case EnumObjectState.ACTIVATED:
                            ObjectCollection objCollection = null;
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, ACTIVATED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ((AeSubscription)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedOnAddSession);
                            break;
                        default:
                            break;
                    }
                }
            }
            catch
            {

            }
        }

        /// <summary>
        /// Handle State Changed
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="state"></param>
        void HandleStateChangedAll(ObjectSpaceElement obj, EnumObjectState state)
        {
            try
            {
                if (this.InvokeRequired)
                {
                    Softing.OPCToolbox.Client.StateChangeEventHandler callback = new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                    this.Invoke(callback, new object[] { obj, state });
                    return;
                }
                if (Type.ReferenceEquals(obj.GetType(), typeof(DaSession)))
                {
                    ObjectCollection objCollection = null;
                    switch (state)
                    {
                        case EnumObjectState.CONNECTED:
                            
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, CONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);
                            ((DaSession)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;

                        case EnumObjectState.DISCONNECTED:
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, DISCONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);
                            ((DaSession)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;
                        default:
                            ShowSpecificContextMenu(obj);
                            break;
                    }
                }
                else if (Type.ReferenceEquals(obj.GetType(), typeof(AeSession)))
                {
                    ObjectCollection objCollection = null;
                    switch (state)
                    {
                        case EnumObjectState.CONNECTED:

                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, CONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);
                            ((AeSession)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;

                        case EnumObjectState.DISCONNECTED:
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, DISCONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);
                            ((AeSession)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;
                        default:
                            ShowSpecificContextMenu(obj);
                            break;
                    }
                }
                else if (Type.ReferenceEquals(obj.GetType(), typeof(DaSubscription)))
                {
                    ObjectCollection objCollection = null;
                    switch (state)
                    {
                        case EnumObjectState.ACTIVATED:

                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, ACTIVATED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);                            

                            ((DaSubscription)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;
                        case EnumObjectState.CONNECTED:
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, CONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);
                            ((DaSubscription)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;

                        case EnumObjectState.DISCONNECTED:
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, DISCONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);
                            ((DaSubscription)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;
                        default:
                            ShowSpecificContextMenu(obj);
                            break;
                    }
                }
                else if (Type.ReferenceEquals(obj.GetType(), typeof(AeSubscription)))
                {
                    ObjectCollection objCollection = null;
                    switch (state)
                    {
                        case EnumObjectState.ACTIVATED:
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, ACTIVATED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);
                            ((AeSubscription)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;
                        case EnumObjectState.CONNECTED:
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, CONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);
                            ((AeSubscription)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;

                        case EnumObjectState.DISCONNECTED:
                            if (updateBrowsersDict.ContainsKey((obj)))
                            {
                                objCollection = updateBrowsersDict[obj];
                                SetNodeImage(objCollection.Node, DISCONNECTED, false);
                                objCollection.Timer.Enabled = false;
                                updateBrowsersDict.Remove(obj);
                            }
                            ShowSpecificContextMenu(obj);
                            ((AeSubscription)obj).StateChangeCompleted -= new Softing.OPCToolbox.Client.StateChangeEventHandler(HandleStateChangedAll);
                            break;
                        default:
                            ShowSpecificContextMenu(obj);
                            break;
                    }
                }
            }
            catch{}
        }
       
        #endregion       

        #region Handle Tab Changes

        private void tabControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            isDeleteEnabled = false;
            isPropertiesEnabled = false;
            bool enableRefresh = false;
            bool enableAddItem = false;
            bool enableAddSubscription = false;

            if (tabControl.SelectedTab == null) return;
            if (tabControl.SelectedTab == tabPageSeverDiscovery)
            {
                isWriteBarEnabled = false;
                enableRefresh = true;
                enableAddSubscription = false;
            }
            else if (tabControl.SelectedTab == tabPageAddressSpace)
            {
                if (addressSpaceControl.Enabled)
                {
                    enableRefresh = true;
                }
                TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
                if (selectedNode == null)
                {
                    if (EnableButtons != null)
                    {
                        EnableButtons(isDeleteEnabled, isPropertiesEnabled, enableAddItem, enableAddSubscription, enableRefresh);
                    }
                    return;
                }
                if (selectedNode.Tag == null)
                {
                    if (EnableButtons != null)
                    {
                        EnableButtons(isDeleteEnabled, isPropertiesEnabled, enableAddItem, enableAddSubscription, enableRefresh);
                    }
                    return;
                }
                FillAddressSpace(selectedNode.Tag);
                if (addressSpaceControl.Enabled && 
                    (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSession))) ||
                    (Type.ReferenceEquals(selectedNode.Tag.GetType(), typeof(DaSubscription))))            
                {
                    isWriteBarEnabled = true;                    
                }
                else
                {
                    isWriteBarEnabled = false;
                }                
            }
            else if (tabControl.SelectedTab == tabPageItems)
            {
                ShowItems();
                if (itemsControl.Enabled)
                {
                    isWriteBarEnabled = true;
                    enableAddItem = true;
                }
                else
                {
                    isWriteBarEnabled = false;
                }
            }
            else if (tabControl.SelectedTab == tabPageEvents)
            {
                isWriteBarEnabled = false;
            }
            else if (tabControl.SelectedTab == tabPageConditions)
            {
                ShowConditions();
                isWriteBarEnabled = false;
            }
            if (EnableWriteBar != null)
            {
                EnableWriteBar(isWriteBarEnabled);
            }

            if (EnableButtons != null)
            {
                EnableButtons(isDeleteEnabled, isPropertiesEnabled, enableAddItem, enableAddSubscription, enableRefresh);
            }
        }

        #endregion

        #region Add DaItems

        void addressSpaceControl_AddDaItem(string itemName, DaSubscription subscription)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                AddDaItem(itemName, subscription);
                AnyChange = true;
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        void addressSpaceControl_AddDaItems(string itemName, DaSubscription subscription)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                AddDaItems(itemName, subscription);
                AnyChange = true;
            }
            catch { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        void subscription_DataChanged(DaSubscription aDaSubscription, DaItem[] items, ValueQT[] values, int[] results)
        {
            //check selected node
            try
            {
                if (this.InvokeRequired)
                {
                    Softing.OPCToolbox.Client.DataChangedEventHandler callback = new Softing.OPCToolbox.Client.DataChangedEventHandler(subscription_DataChanged);
                    this.Invoke(callback, new object[] { aDaSubscription, items, values, results});
                    return;
                }
                itemsControl.DataChanged(aDaSubscription, items, values, results);
            }
            catch { }
        }

        #endregion

        #region Add Conditions

        void subscription_AeConditionsChanged(AeSubscription anAeSubscription, AeCondition[] conditions)
        {
            //check selected node
            try
            {
                if (this.InvokeRequired)
                {
                    Softing.OPCToolbox.Client.AeConditionsChangedEventHandler callback = new Softing.OPCToolbox.Client.AeConditionsChangedEventHandler(subscription_AeConditionsChanged);
                    this.Invoke(callback, new object[] { anAeSubscription, conditions});
                    return;
                }

                TreeNode selectedNode = serversGroupsTreeView.SelectedNode;
                if (selectedNode == null) return;
                if(selectedNode.Tag != null && ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSession)))
                {
                    foreach(AeSubscription subsc in (selectedNode.Tag as AeSession).SubscriptionList)
                    {
                        if(subsc == anAeSubscription)
                        {
                            conditionsControl.ConditionsAdd(subsc, conditions, DemoClient.TabControls.ShownConditions.Session, new AeSession[] { (selectedNode.Tag as AeSession) });
                            break;
                        }
                    }                    
                }
                else if(selectedNode.Tag != null && ReferenceEquals(selectedNode.Tag.GetType(), typeof(AeSubscription)))
                {
                    if (anAeSubscription == (selectedNode.Tag as AeSubscription))
                    {
                        conditionsControl.ConditionsAdd((selectedNode.Tag as AeSubscription), conditions, DemoClient.TabControls.ShownConditions.Subscription, null);
                    }
                }
                else
                {
                    conditionsControl.ConditionsAdd(anAeSubscription, conditions, DemoClient.TabControls.ShownConditions.All, clientAppInstance.AeSessionList);
                }
            }
            catch { }

        }

        #endregion

        #region AeEvents

        void subscription_AeEventsReceived(AeSubscription anAeSubscription, bool isRefresh, bool lastRefresh, AeEvent[] events)
        {
            try
            {
                if (this.InvokeRequired)
                {
                    Softing.OPCToolbox.Client.AeEventsReceivedEventHandler callback = new Softing.OPCToolbox.Client.AeEventsReceivedEventHandler(subscription_AeEventsReceived);
                    this.Invoke(callback, new object[] { anAeSubscription, isRefresh, lastRefresh , events});
                    return;
                }
                eventsControl.ReceivedEvents(anAeSubscription, isRefresh, lastRefresh, events);
                if (!eventsControl.IsEmptyEvents())
                {
                    eventsControl.Enabled = true;
                }
                
            }
            catch { }
        }

        #endregion        

        #endregion

        #region Helper Classes

        #region Objecy Collection Class

        /// <summary>
        /// Class that handels items collections
        /// </summary>
        public class ObjectCollection
        {
            private TreeNode node;
            public TreeNode Node
            {
                get { return node; }
                set { node = value; }
            }

            private Timer timer;
            public Timer Timer
            {
                get { return timer; }
                set { timer = value; }
            }

            public ObjectCollection(TreeNode newNode, Timer newTimer)
            {
                node = newNode;
                timer = newTimer;
            }
         
        }

        #endregion

        #region Custom Timer Class

        public class CustomTimer : Timer
        {
            TreeNode node;
            public TreeNode Node
{
  get { return node; }
  set { node = value; }
}

            public CustomTimer(TreeNode newNode)
            {
                node = newNode;
            }
        }

        #endregion
       
        #endregion                             

    }    
}
