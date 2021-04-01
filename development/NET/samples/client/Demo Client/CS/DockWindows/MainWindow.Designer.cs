namespace DemoClient.DockWindows
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWindow));
            this.serversGroupsTreeView = new System.Windows.Forms.TreeView();
            this.imageListSession = new System.Windows.Forms.ImageList(this.components);
            this.splitter = new System.Windows.Forms.Splitter();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.tabPageSeverDiscovery = new System.Windows.Forms.TabPage();
            this.serverDiscoveryControl = new DemoClient.TabControls.ServerDiscoveryControl();
            this.tabPageAddressSpace = new System.Windows.Forms.TabPage();
            this.addressSpaceControl = new DemoClient.TabControls.AddressSpaceControl();
            this.tabPageItems = new System.Windows.Forms.TabPage();
            this.itemsControl = new DemoClient.TabControls.ItemsControl();
            this.tabPageEvents = new System.Windows.Forms.TabPage();
            this.eventsControl = new DemoClient.TabControls.EventsControl();
            this.tabPageConditions = new System.Windows.Forms.TabPage();
            this.conditionsControl = new DemoClient.TabControls.ConditionsControl();
            this.disconnActivateTimer = new System.Windows.Forms.Timer(this.components);
            this.connActivateTimer = new System.Windows.Forms.Timer(this.components);
            this.disconnConnTimer = new System.Windows.Forms.Timer(this.components);
            this.sessionContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addSubscriptionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.stateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.connectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deepToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.flatToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.disconnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.propertiesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.daSubscriptionContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.daDeleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.daStateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.daActivateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.daDeepActivateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.daFlatActivateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.daConnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.daDeepConnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.daFlatConnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.daDisconnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.daRefreshToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.daPropertiesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aeSubsciptionContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.aeDeleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.aeStateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aeActivateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aeDeepActivateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aeFlatActivateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aeConnectTtoolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aeDeepConnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aeFlatConnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aeDisconnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.aePropertiesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabControl.SuspendLayout();
            this.tabPageSeverDiscovery.SuspendLayout();
            this.tabPageAddressSpace.SuspendLayout();
            this.tabPageItems.SuspendLayout();
            this.tabPageEvents.SuspendLayout();
            this.tabPageConditions.SuspendLayout();
            this.sessionContextMenuStrip.SuspendLayout();
            this.daSubscriptionContextMenuStrip.SuspendLayout();
            this.aeSubsciptionContextMenuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // serversGroupsTreeView
            // 
            this.serversGroupsTreeView.Dock = System.Windows.Forms.DockStyle.Left;
            this.serversGroupsTreeView.HideSelection = false;
            this.serversGroupsTreeView.ImageIndex = 0;
            this.serversGroupsTreeView.ImageList = this.imageListSession;
            this.serversGroupsTreeView.ItemHeight = 18;
            this.serversGroupsTreeView.Location = new System.Drawing.Point(0, 0);
            this.serversGroupsTreeView.Name = "serversGroupsTreeView";
            this.serversGroupsTreeView.SelectedImageIndex = 0;
            this.serversGroupsTreeView.Size = new System.Drawing.Size(211, 566);
            this.serversGroupsTreeView.TabIndex = 0;
            this.serversGroupsTreeView.Enter += new System.EventHandler(this.serversGroupsTreeView_Enter);
            this.serversGroupsTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.serversGroupsTreeView_AfterSelect);
            // 
            // imageListSession
            // 
            this.imageListSession.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListSession.ImageStream")));
            this.imageListSession.TransparentColor = System.Drawing.Color.Transparent;
            this.imageListSession.Images.SetKeyName(0, "GreyTag.ico");
            this.imageListSession.Images.SetKeyName(1, "Green.ico");
            this.imageListSession.Images.SetKeyName(2, "Red.ico");
            this.imageListSession.Images.SetKeyName(3, "yellow.ico");
            this.imageListSession.Images.SetKeyName(4, "green_yellow1.ico");
            this.imageListSession.Images.SetKeyName(5, "green_yellow2.ico");
            this.imageListSession.Images.SetKeyName(6, "green_yellow3.ico");
            this.imageListSession.Images.SetKeyName(7, "green_yellow4.ico");
            this.imageListSession.Images.SetKeyName(8, "red_green1.ico");
            this.imageListSession.Images.SetKeyName(9, "red_green2.ico");
            this.imageListSession.Images.SetKeyName(10, "red_green3.ico");
            this.imageListSession.Images.SetKeyName(11, "red_green4.ico");
            this.imageListSession.Images.SetKeyName(12, "red_yellow1.ico");
            this.imageListSession.Images.SetKeyName(13, "red_yellow2.ico");
            this.imageListSession.Images.SetKeyName(14, "red_yellow3.ico");
            this.imageListSession.Images.SetKeyName(15, "red_yellow4.ico");
            // 
            // splitter
            // 
            this.splitter.Location = new System.Drawing.Point(211, 0);
            this.splitter.Name = "splitter";
            this.splitter.Size = new System.Drawing.Size(3, 566);
            this.splitter.TabIndex = 1;
            this.splitter.TabStop = false;
            // 
            // tabControl
            // 
            this.tabControl.Alignment = System.Windows.Forms.TabAlignment.Bottom;
            this.tabControl.Controls.Add(this.tabPageSeverDiscovery);
            this.tabControl.Controls.Add(this.tabPageAddressSpace);
            this.tabControl.Controls.Add(this.tabPageItems);
            this.tabControl.Controls.Add(this.tabPageEvents);
            this.tabControl.Controls.Add(this.tabPageConditions);
            this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl.ItemSize = new System.Drawing.Size(95, 25);
            this.tabControl.Location = new System.Drawing.Point(214, 0);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(578, 566);
            this.tabControl.TabIndex = 2;
            this.tabControl.SelectedIndexChanged += new System.EventHandler(this.tabControl_SelectedIndexChanged);
            // 
            // tabPageSeverDiscovery
            // 
            this.tabPageSeverDiscovery.Controls.Add(this.serverDiscoveryControl);
            this.tabPageSeverDiscovery.Location = new System.Drawing.Point(4, 4);
            this.tabPageSeverDiscovery.Name = "tabPageSeverDiscovery";
            this.tabPageSeverDiscovery.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageSeverDiscovery.Size = new System.Drawing.Size(570, 533);
            this.tabPageSeverDiscovery.TabIndex = 0;
            this.tabPageSeverDiscovery.Text = "Server Discovery";
            this.tabPageSeverDiscovery.UseVisualStyleBackColor = true;
            // 
            // serverDiscoveryControl
            // 
            this.serverDiscoveryControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.serverDiscoveryControl.Location = new System.Drawing.Point(0, 0);
            this.serverDiscoveryControl.Margin = new System.Windows.Forms.Padding(0);
            this.serverDiscoveryControl.Name = "serverDiscoveryControl";
            this.serverDiscoveryControl.Padding = new System.Windows.Forms.Padding(0, 0, 2, 3);
            this.serverDiscoveryControl.Size = new System.Drawing.Size(570, 533);
            this.serverDiscoveryControl.TabIndex = 0;
            // 
            // tabPageAddressSpace
            // 
            this.tabPageAddressSpace.Controls.Add(this.addressSpaceControl);
            this.tabPageAddressSpace.Location = new System.Drawing.Point(4, 4);
            this.tabPageAddressSpace.Name = "tabPageAddressSpace";
            this.tabPageAddressSpace.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageAddressSpace.Size = new System.Drawing.Size(570, 533);
            this.tabPageAddressSpace.TabIndex = 1;
            this.tabPageAddressSpace.Text = "Address Space";
            this.tabPageAddressSpace.UseVisualStyleBackColor = true;
            // 
            // addressSpaceControl
            // 
            this.addressSpaceControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.addressSpaceControl.DaSubscription = null;
            this.addressSpaceControl.Location = new System.Drawing.Point(0, 0);
            this.addressSpaceControl.MainDockPanel = null;
            this.addressSpaceControl.Margin = new System.Windows.Forms.Padding(0);
            this.addressSpaceControl.Name = "addressSpaceControl";
            this.addressSpaceControl.Padding = new System.Windows.Forms.Padding(0, 0, 2, 3);
            this.addressSpaceControl.PropertyWindow = null;
            this.addressSpaceControl.ServiceWindow = null;
            this.addressSpaceControl.Size = new System.Drawing.Size(570, 533);
            this.addressSpaceControl.TabIndex = 0;
            // 
            // tabPageItems
            // 
            this.tabPageItems.Controls.Add(this.itemsControl);
            this.tabPageItems.Location = new System.Drawing.Point(4, 4);
            this.tabPageItems.Name = "tabPageItems";
            this.tabPageItems.Size = new System.Drawing.Size(570, 533);
            this.tabPageItems.TabIndex = 2;
            this.tabPageItems.Text = "Items";
            this.tabPageItems.UseVisualStyleBackColor = true;
            // 
            // itemsControl
            // 
            this.itemsControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.itemsControl.Location = new System.Drawing.Point(0, 0);
            this.itemsControl.MainDockPanel = null;
            this.itemsControl.Margin = new System.Windows.Forms.Padding(0);
            this.itemsControl.Name = "itemsControl";
            this.itemsControl.Padding = new System.Windows.Forms.Padding(0, 0, 2, 3);
            this.itemsControl.PropertyWindow = null;
            this.itemsControl.ServiceWindow = null;
            this.itemsControl.Size = new System.Drawing.Size(570, 533);
            this.itemsControl.TabIndex = 0;
            // 
            // tabPageEvents
            // 
            this.tabPageEvents.Controls.Add(this.eventsControl);
            this.tabPageEvents.Location = new System.Drawing.Point(4, 4);
            this.tabPageEvents.Name = "tabPageEvents";
            this.tabPageEvents.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageEvents.Size = new System.Drawing.Size(570, 533);
            this.tabPageEvents.TabIndex = 4;
            this.tabPageEvents.Text = "Events";
            this.tabPageEvents.UseVisualStyleBackColor = true;
            // 
            // eventsControl
            // 
            this.eventsControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.eventsControl.Location = new System.Drawing.Point(0, 0);
            this.eventsControl.Margin = new System.Windows.Forms.Padding(0);
            this.eventsControl.Name = "eventsControl";
            this.eventsControl.Padding = new System.Windows.Forms.Padding(0, 0, 2, 3);
            this.eventsControl.Size = new System.Drawing.Size(570, 533);
            this.eventsControl.TabIndex = 0;
            // 
            // tabPageConditions
            // 
            this.tabPageConditions.Controls.Add(this.conditionsControl);
            this.tabPageConditions.Location = new System.Drawing.Point(4, 4);
            this.tabPageConditions.Name = "tabPageConditions";
            this.tabPageConditions.Size = new System.Drawing.Size(570, 533);
            this.tabPageConditions.TabIndex = 3;
            this.tabPageConditions.Text = "Conditions";
            this.tabPageConditions.UseVisualStyleBackColor = true;
            // 
            // conditionsControl
            // 
            this.conditionsControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.conditionsControl.Location = new System.Drawing.Point(0, 0);
            this.conditionsControl.Margin = new System.Windows.Forms.Padding(0);
            this.conditionsControl.Name = "conditionsControl";
            this.conditionsControl.Padding = new System.Windows.Forms.Padding(0, 0, 2, 3);
            this.conditionsControl.Size = new System.Drawing.Size(570, 533);
            this.conditionsControl.TabIndex = 0;
            // 
            // disconnActivateTimer
            // 
            this.disconnActivateTimer.Interval = 200;
            // 
            // connActivateTimer
            // 
            this.connActivateTimer.Interval = 200;
            // 
            // disconnConnTimer
            // 
            this.disconnConnTimer.Interval = 200;
            // 
            // sessionContextMenuStrip
            // 
            this.sessionContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addSubscriptionToolStripMenuItem,
            this.deleteToolStripMenuItem,
            this.toolStripSeparator1,
            this.stateToolStripMenuItem,
            this.toolStripSeparator2,
            this.propertiesToolStripMenuItem});
            this.sessionContextMenuStrip.Name = "daSessionContextMenuStrip";
            this.sessionContextMenuStrip.Size = new System.Drawing.Size(166, 104);
            // 
            // addSubscriptionToolStripMenuItem
            // 
            this.addSubscriptionToolStripMenuItem.Name = "addSubscriptionToolStripMenuItem";
            this.addSubscriptionToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.addSubscriptionToolStripMenuItem.Text = "Add Subscription";
            this.addSubscriptionToolStripMenuItem.ToolTipText = "Add a subscription object";
            this.addSubscriptionToolStripMenuItem.Click += new System.EventHandler(this.addSubscriptionToolStripMenuItem_Click);
            // 
            // deleteToolStripMenuItem
            // 
            this.deleteToolStripMenuItem.Image = global::DemoClient.Properties.Resources.delete;
            this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
            this.deleteToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.deleteToolStripMenuItem.Text = "Delete";
            this.deleteToolStripMenuItem.ToolTipText = "Delete the object and all objects in the subtrees";
            this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(162, 6);
            // 
            // stateToolStripMenuItem
            // 
            this.stateToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connectToolStripMenuItem,
            this.disconnectToolStripMenuItem});
            this.stateToolStripMenuItem.Name = "stateToolStripMenuItem";
            this.stateToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.stateToolStripMenuItem.Text = "State";
            // 
            // connectToolStripMenuItem
            // 
            this.connectToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deepToolStripMenuItem,
            this.flatToolStripMenuItem});
            this.connectToolStripMenuItem.Image = global::DemoClient.Properties.Resources.connect;
            this.connectToolStripMenuItem.Name = "connectToolStripMenuItem";
            this.connectToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.connectToolStripMenuItem.Text = "Connect";
            // 
            // deepToolStripMenuItem
            // 
            this.deepToolStripMenuItem.Name = "deepToolStripMenuItem";
            this.deepToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.deepToolStripMenuItem.Text = "Deep";
            this.deepToolStripMenuItem.ToolTipText = "Connect object and it\'s childs";
            this.deepToolStripMenuItem.Click += new System.EventHandler(this.deepToolStripMenuItem_Click);
            // 
            // flatToolStripMenuItem
            // 
            this.flatToolStripMenuItem.Name = "flatToolStripMenuItem";
            this.flatToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.flatToolStripMenuItem.Text = "Flat";
            this.flatToolStripMenuItem.ToolTipText = "Connect object";
            this.flatToolStripMenuItem.Click += new System.EventHandler(this.flatToolStripMenuItem_Click);
            // 
            // disconnectToolStripMenuItem
            // 
            this.disconnectToolStripMenuItem.Image = global::DemoClient.Properties.Resources.disconnect;
            this.disconnectToolStripMenuItem.Name = "disconnectToolStripMenuItem";
            this.disconnectToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.disconnectToolStripMenuItem.Text = "Disconnect";
            this.disconnectToolStripMenuItem.ToolTipText = "Disconnect object and it\'s childs";
            this.disconnectToolStripMenuItem.Click += new System.EventHandler(this.disconnectToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(162, 6);
            // 
            // propertiesToolStripMenuItem
            // 
            this.propertiesToolStripMenuItem.Image = global::DemoClient.Properties.Resources.properties;
            this.propertiesToolStripMenuItem.Name = "propertiesToolStripMenuItem";
            this.propertiesToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F7;
            this.propertiesToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.propertiesToolStripMenuItem.Text = "Properties";
            this.propertiesToolStripMenuItem.ToolTipText = "Show properties of the selected object";
            this.propertiesToolStripMenuItem.Click += new System.EventHandler(this.propertiesToolStripMenuItem_Click);
            // 
            // daSubscriptionContextMenuStrip
            // 
            this.daSubscriptionContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.daDeleteToolStripMenuItem,
            this.toolStripSeparator3,
            this.daStateToolStripMenuItem,
            this.daRefreshToolStripMenuItem,
            this.toolStripSeparator4,
            this.daPropertiesToolStripMenuItem});
            this.daSubscriptionContextMenuStrip.Name = "daSessionContextMenuStrip";
            this.daSubscriptionContextMenuStrip.Size = new System.Drawing.Size(154, 104);
            // 
            // daDeleteToolStripMenuItem
            // 
            this.daDeleteToolStripMenuItem.Image = global::DemoClient.Properties.Resources.delete;
            this.daDeleteToolStripMenuItem.Name = "daDeleteToolStripMenuItem";
            this.daDeleteToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.daDeleteToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.daDeleteToolStripMenuItem.Text = "Delete";
            this.daDeleteToolStripMenuItem.ToolTipText = "Delete the object and all objects in the subtrees";
            this.daDeleteToolStripMenuItem.Click += new System.EventHandler(this.daDeleteToolStripMenuItem_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(150, 6);
            // 
            // daStateToolStripMenuItem
            // 
            this.daStateToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.daActivateToolStripMenuItem,
            this.daConnectToolStripMenuItem,
            this.daDisconnectToolStripMenuItem});
            this.daStateToolStripMenuItem.Name = "daStateToolStripMenuItem";
            this.daStateToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.daStateToolStripMenuItem.Text = "State";
            // 
            // daActivateToolStripMenuItem
            // 
            this.daActivateToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.daDeepActivateToolStripMenuItem,
            this.daFlatActivateToolStripMenuItem});
            this.daActivateToolStripMenuItem.Image = global::DemoClient.Properties.Resources.activate;
            this.daActivateToolStripMenuItem.Name = "daActivateToolStripMenuItem";
            this.daActivateToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.daActivateToolStripMenuItem.Text = "Activate";
            // 
            // daDeepActivateToolStripMenuItem
            // 
            this.daDeepActivateToolStripMenuItem.Name = "daDeepActivateToolStripMenuItem";
            this.daDeepActivateToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.daDeepActivateToolStripMenuItem.Text = "Deep";
            this.daDeepActivateToolStripMenuItem.ToolTipText = "Activate object and it\'s childs";
            this.daDeepActivateToolStripMenuItem.Click += new System.EventHandler(this.daDeepActivateToolStripMenuItem_Click);
            // 
            // daFlatActivateToolStripMenuItem
            // 
            this.daFlatActivateToolStripMenuItem.Name = "daFlatActivateToolStripMenuItem";
            this.daFlatActivateToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.daFlatActivateToolStripMenuItem.Text = "Flat";
            this.daFlatActivateToolStripMenuItem.ToolTipText = "Activate object";
            this.daFlatActivateToolStripMenuItem.Click += new System.EventHandler(this.daFlatActivateToolStripMenuItem_Click);
            // 
            // daConnectToolStripMenuItem
            // 
            this.daConnectToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.daDeepConnectToolStripMenuItem,
            this.daFlatConnectToolStripMenuItem});
            this.daConnectToolStripMenuItem.Image = global::DemoClient.Properties.Resources.connect;
            this.daConnectToolStripMenuItem.Name = "daConnectToolStripMenuItem";
            this.daConnectToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.daConnectToolStripMenuItem.Text = "Connect";
            // 
            // daDeepConnectToolStripMenuItem
            // 
            this.daDeepConnectToolStripMenuItem.Name = "daDeepConnectToolStripMenuItem";
            this.daDeepConnectToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.daDeepConnectToolStripMenuItem.Text = "Deep";
            this.daDeepConnectToolStripMenuItem.ToolTipText = "Connect object and it\'s childs";
            this.daDeepConnectToolStripMenuItem.Click += new System.EventHandler(this.daDeepConnectToolStripMenuItem_Click);
            // 
            // daFlatConnectToolStripMenuItem
            // 
            this.daFlatConnectToolStripMenuItem.Name = "daFlatConnectToolStripMenuItem";
            this.daFlatConnectToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.daFlatConnectToolStripMenuItem.Text = "Flat";
            this.daFlatConnectToolStripMenuItem.ToolTipText = "Connect objec";
            this.daFlatConnectToolStripMenuItem.Click += new System.EventHandler(this.daFlatConnectToolStripMenuItem_Click);
            // 
            // daDisconnectToolStripMenuItem
            // 
            this.daDisconnectToolStripMenuItem.Image = global::DemoClient.Properties.Resources.disconnect;
            this.daDisconnectToolStripMenuItem.Name = "daDisconnectToolStripMenuItem";
            this.daDisconnectToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.daDisconnectToolStripMenuItem.Text = "Disconnect";
            this.daDisconnectToolStripMenuItem.ToolTipText = "Disconnect object and it\'s childs";
            this.daDisconnectToolStripMenuItem.Click += new System.EventHandler(this.daDisconnectToolStripMenuItem_Click);
            // 
            // daRefreshToolStripMenuItem
            // 
            this.daRefreshToolStripMenuItem.Name = "daRefreshToolStripMenuItem";
            this.daRefreshToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F5;
            this.daRefreshToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.daRefreshToolStripMenuItem.Text = "Refresh ";
            this.daRefreshToolStripMenuItem.ToolTipText = "Refresh the selected subscription object";
            this.daRefreshToolStripMenuItem.Click += new System.EventHandler(this.daRefreshToolStripMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(150, 6);
            // 
            // daPropertiesToolStripMenuItem
            // 
            this.daPropertiesToolStripMenuItem.Image = global::DemoClient.Properties.Resources.properties;
            this.daPropertiesToolStripMenuItem.Name = "daPropertiesToolStripMenuItem";
            this.daPropertiesToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F7;
            this.daPropertiesToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.daPropertiesToolStripMenuItem.Text = "Properties";
            this.daPropertiesToolStripMenuItem.ToolTipText = "Show properties of the selected object";
            this.daPropertiesToolStripMenuItem.Click += new System.EventHandler(this.daPropertiesToolStripMenuItem_Click);
            // 
            // aeSubsciptionContextMenuStrip
            // 
            this.aeSubsciptionContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aeDeleteToolStripMenuItem,
            this.toolStripSeparator5,
            this.aeStateToolStripMenuItem,
            this.toolStripSeparator6,
            this.aePropertiesToolStripMenuItem});
            this.aeSubsciptionContextMenuStrip.Name = "daSessionContextMenuStrip";
            this.aeSubsciptionContextMenuStrip.Size = new System.Drawing.Size(154, 82);
            // 
            // aeDeleteToolStripMenuItem
            // 
            this.aeDeleteToolStripMenuItem.Image = global::DemoClient.Properties.Resources.delete;
            this.aeDeleteToolStripMenuItem.Name = "aeDeleteToolStripMenuItem";
            this.aeDeleteToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.aeDeleteToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.aeDeleteToolStripMenuItem.Text = "Delete";
            this.aeDeleteToolStripMenuItem.ToolTipText = "Delete the object and all objects in the subtrees";
            this.aeDeleteToolStripMenuItem.Click += new System.EventHandler(this.aeDeleteToolStripMenuItem_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(150, 6);
            // 
            // aeStateToolStripMenuItem
            // 
            this.aeStateToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aeActivateToolStripMenuItem,
            this.aeConnectTtoolStripMenuItem,
            this.aeDisconnectToolStripMenuItem});
            this.aeStateToolStripMenuItem.Name = "aeStateToolStripMenuItem";
            this.aeStateToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.aeStateToolStripMenuItem.Text = "State";
            // 
            // aeActivateToolStripMenuItem
            // 
            this.aeActivateToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aeDeepActivateToolStripMenuItem,
            this.aeFlatActivateToolStripMenuItem});
            this.aeActivateToolStripMenuItem.Image = global::DemoClient.Properties.Resources.activate;
            this.aeActivateToolStripMenuItem.Name = "aeActivateToolStripMenuItem";
            this.aeActivateToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.aeActivateToolStripMenuItem.Text = "Activate";
            // 
            // aeDeepActivateToolStripMenuItem
            // 
            this.aeDeepActivateToolStripMenuItem.Name = "aeDeepActivateToolStripMenuItem";
            this.aeDeepActivateToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.aeDeepActivateToolStripMenuItem.Text = "Deep";
            this.aeDeepActivateToolStripMenuItem.ToolTipText = "Activate object and it\'s childs";
            this.aeDeepActivateToolStripMenuItem.Click += new System.EventHandler(this.aeDeepActivateToolStripMenuItem_Click);
            // 
            // aeFlatActivateToolStripMenuItem
            // 
            this.aeFlatActivateToolStripMenuItem.Name = "aeFlatActivateToolStripMenuItem";
            this.aeFlatActivateToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.aeFlatActivateToolStripMenuItem.Text = "Flat";
            this.aeFlatActivateToolStripMenuItem.ToolTipText = "Activate object";
            this.aeFlatActivateToolStripMenuItem.Click += new System.EventHandler(this.aeFlatActivateToolStripMenuItem_Click);
            // 
            // aeConnectTtoolStripMenuItem
            // 
            this.aeConnectTtoolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aeDeepConnectToolStripMenuItem,
            this.aeFlatConnectToolStripMenuItem});
            this.aeConnectTtoolStripMenuItem.Image = global::DemoClient.Properties.Resources.connect;
            this.aeConnectTtoolStripMenuItem.Name = "aeConnectTtoolStripMenuItem";
            this.aeConnectTtoolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.aeConnectTtoolStripMenuItem.Text = "Connect";
            // 
            // aeDeepConnectToolStripMenuItem
            // 
            this.aeDeepConnectToolStripMenuItem.Name = "aeDeepConnectToolStripMenuItem";
            this.aeDeepConnectToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.aeDeepConnectToolStripMenuItem.Text = "Deep";
            this.aeDeepConnectToolStripMenuItem.ToolTipText = "Connect object and it\'s childs";
            this.aeDeepConnectToolStripMenuItem.Click += new System.EventHandler(this.aeDeepConnectToolStripMenuItem_Click);
            // 
            // aeFlatConnectToolStripMenuItem
            // 
            this.aeFlatConnectToolStripMenuItem.Name = "aeFlatConnectToolStripMenuItem";
            this.aeFlatConnectToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.aeFlatConnectToolStripMenuItem.Text = "Flat";
            this.aeFlatConnectToolStripMenuItem.ToolTipText = "Connect object";
            this.aeFlatConnectToolStripMenuItem.Click += new System.EventHandler(this.aeFlatConnectToolStripMenuItem_Click);
            // 
            // aeDisconnectToolStripMenuItem
            // 
            this.aeDisconnectToolStripMenuItem.Image = global::DemoClient.Properties.Resources.disconnect;
            this.aeDisconnectToolStripMenuItem.Name = "aeDisconnectToolStripMenuItem";
            this.aeDisconnectToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.aeDisconnectToolStripMenuItem.Text = "Disconnect";
            this.aeDisconnectToolStripMenuItem.ToolTipText = "Disconnect object and it\'s childs";
            this.aeDisconnectToolStripMenuItem.Click += new System.EventHandler(this.aeDisconnectToolStripMenuItem_Click);
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(150, 6);
            // 
            // aePropertiesToolStripMenuItem
            // 
            this.aePropertiesToolStripMenuItem.Image = global::DemoClient.Properties.Resources.properties;
            this.aePropertiesToolStripMenuItem.Name = "aePropertiesToolStripMenuItem";
            this.aePropertiesToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F7;
            this.aePropertiesToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.aePropertiesToolStripMenuItem.Text = "Properties";
            this.aePropertiesToolStripMenuItem.ToolTipText = "Show properties of the selected object";
            this.aePropertiesToolStripMenuItem.Click += new System.EventHandler(this.aePropertiesToolStripMenuItem_Click);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(792, 566);
            this.Controls.Add(this.tabControl);
            this.Controls.Add(this.splitter);
            this.Controls.Add(this.serversGroupsTreeView);
            this.Name = "MainWindow";
            this.Load += new System.EventHandler(this.MainWindow_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainWindow_FormClosing);
            this.tabControl.ResumeLayout(false);
            this.tabPageSeverDiscovery.ResumeLayout(false);
            this.tabPageAddressSpace.ResumeLayout(false);
            this.tabPageItems.ResumeLayout(false);
            this.tabPageEvents.ResumeLayout(false);
            this.tabPageConditions.ResumeLayout(false);
            this.sessionContextMenuStrip.ResumeLayout(false);
            this.daSubscriptionContextMenuStrip.ResumeLayout(false);
            this.aeSubsciptionContextMenuStrip.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView serversGroupsTreeView;
        private System.Windows.Forms.Splitter splitter;
        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tabPageSeverDiscovery;
        private System.Windows.Forms.TabPage tabPageAddressSpace;
        private System.Windows.Forms.TabPage tabPageItems;
        private System.Windows.Forms.TabPage tabPageConditions;
        private System.Windows.Forms.TabPage tabPageEvents;
        private DemoClient.TabControls.ServerDiscoveryControl serverDiscoveryControl;
        private DemoClient.TabControls.AddressSpaceControl addressSpaceControl;
        private DemoClient.TabControls.ItemsControl itemsControl;
        private DemoClient.TabControls.ConditionsControl conditionsControl;
        private DemoClient.TabControls.EventsControl eventsControl;
        private System.Windows.Forms.ImageList imageListSession;
        private System.Windows.Forms.Timer disconnActivateTimer;
        private System.Windows.Forms.Timer connActivateTimer;
        private System.Windows.Forms.Timer disconnConnTimer;
        private System.Windows.Forms.ContextMenuStrip sessionContextMenuStrip;
        private System.Windows.Forms.ContextMenuStrip daSubscriptionContextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem addSubscriptionToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem deleteToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem stateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem connectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem deepToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem flatToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem disconnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem propertiesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem daDeleteToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem daStateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem daActivateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem daDeepActivateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem daFlatActivateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem daConnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem daDeepConnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem daFlatConnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem daDisconnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem daRefreshToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem daPropertiesToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip aeSubsciptionContextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem aeDeleteToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripMenuItem aeStateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aeActivateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aeDeepActivateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aeFlatActivateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aeConnectTtoolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aeDeepConnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aeFlatConnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aeDisconnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
        private System.Windows.Forms.ToolStripMenuItem aePropertiesToolStripMenuItem;
    }
}
