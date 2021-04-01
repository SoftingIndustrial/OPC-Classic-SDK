namespace DemoClient.TabControls
{
    partial class ItemsControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ItemsControl));
            this.itemsListView = new System.Windows.Forms.ListView();
            this.itemColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.valueColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.qualityColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.timeStampColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.sessionColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.subscriptionColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.imageListItems = new System.Windows.Forms.ImageList(this.components);
            this.contextMenuStripItems = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.activateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.connectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.disconnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.readToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.writeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.propertiesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStripItems.SuspendLayout();
            this.SuspendLayout();
            // 
            // itemsListView
            // 
            this.itemsListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.itemColumnHeader,
            this.valueColumnHeader,
            this.qualityColumnHeader,
            this.timeStampColumnHeader,
            this.sessionColumnHeader,
            this.subscriptionColumnHeader});
            this.itemsListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.itemsListView.FullRowSelect = true;
            this.itemsListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.itemsListView.HideSelection = false;
            this.itemsListView.Location = new System.Drawing.Point(0, 0);
            this.itemsListView.Name = "itemsListView";
            this.itemsListView.Size = new System.Drawing.Size(584, 464);
            this.itemsListView.SmallImageList = this.imageListItems;
            this.itemsListView.TabIndex = 0;
            this.itemsListView.UseCompatibleStateImageBehavior = false;
            this.itemsListView.View = System.Windows.Forms.View.Details;
            this.itemsListView.MouseCaptureChanged += new System.EventHandler(this.itemsListView_MouseCaptureChanged);
            this.itemsListView.SelectedIndexChanged += new System.EventHandler(this.itemsListView_SelectedIndexChanged);
            // 
            // itemColumnHeader
            // 
            this.itemColumnHeader.Text = "Item";
            this.itemColumnHeader.Width = 90;
            // 
            // valueColumnHeader
            // 
            this.valueColumnHeader.Text = "Value";
            // 
            // qualityColumnHeader
            // 
            this.qualityColumnHeader.Text = "Quality";
            // 
            // timeStampColumnHeader
            // 
            this.timeStampColumnHeader.Text = "Time Stamp";
            this.timeStampColumnHeader.Width = 90;
            // 
            // sessionColumnHeader
            // 
            this.sessionColumnHeader.Text = "Session";
            this.sessionColumnHeader.Width = 95;
            // 
            // subscriptionColumnHeader
            // 
            this.subscriptionColumnHeader.Text = "Subscription";
            this.subscriptionColumnHeader.Width = 76;
            // 
            // imageListItems
            // 
            this.imageListItems.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListItems.ImageStream")));
            this.imageListItems.TransparentColor = System.Drawing.Color.Transparent;
            this.imageListItems.Images.SetKeyName(0, "Green.ico");
            this.imageListItems.Images.SetKeyName(1, "Red.ico");
            this.imageListItems.Images.SetKeyName(2, "yellow.ico");
            this.imageListItems.Images.SetKeyName(3, "green_yellow1.ico");
            this.imageListItems.Images.SetKeyName(4, "green_yellow2.ico");
            this.imageListItems.Images.SetKeyName(5, "green_yellow3.ico");
            this.imageListItems.Images.SetKeyName(6, "green_yellow4.ico");
            this.imageListItems.Images.SetKeyName(7, "red_green1.ico");
            this.imageListItems.Images.SetKeyName(8, "red_green2.ico");
            this.imageListItems.Images.SetKeyName(9, "red_green3.ico");
            this.imageListItems.Images.SetKeyName(10, "red_green4.ico");
            this.imageListItems.Images.SetKeyName(11, "red_yellow1.ico");
            this.imageListItems.Images.SetKeyName(12, "red_yellow2.ico");
            this.imageListItems.Images.SetKeyName(13, "red_yellow3.ico");
            this.imageListItems.Images.SetKeyName(14, "red_yellow4.ico");
            // 
            // contextMenuStripItems
            // 
            this.contextMenuStripItems.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteToolStripMenuItem,
            this.stateToolStripMenuItem,
            this.readToolStripMenuItem,
            this.writeToolStripMenuItem,
            this.toolStripSeparator1,
            this.propertiesToolStripMenuItem});
            this.contextMenuStripItems.Name = "contextMenuStripItems";
            this.contextMenuStripItems.Size = new System.Drawing.Size(154, 120);
            // 
            // deleteToolStripMenuItem
            // 
            this.deleteToolStripMenuItem.Image = global::DemoClient.Properties.Resources.delete;
            this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
            this.deleteToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.deleteToolStripMenuItem.Text = "Delete";
            this.deleteToolStripMenuItem.ToolTipText = "Delete the selected items";
            this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteToolStripMenuItem_Click);
            // 
            // stateToolStripMenuItem
            // 
            this.stateToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.activateToolStripMenuItem,
            this.connectToolStripMenuItem,
            this.disconnectToolStripMenuItem});
            this.stateToolStripMenuItem.Name = "stateToolStripMenuItem";
            this.stateToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.stateToolStripMenuItem.Text = "State";
            // 
            // activateToolStripMenuItem
            // 
            this.activateToolStripMenuItem.Image = global::DemoClient.Properties.Resources.activate;
            this.activateToolStripMenuItem.Name = "activateToolStripMenuItem";
            this.activateToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.activateToolStripMenuItem.Text = "Activate";
            this.activateToolStripMenuItem.ToolTipText = "Activate object";
            this.activateToolStripMenuItem.Click += new System.EventHandler(this.activateToolStripMenuItem_Click);
            // 
            // connectToolStripMenuItem
            // 
            this.connectToolStripMenuItem.Image = global::DemoClient.Properties.Resources.connect;
            this.connectToolStripMenuItem.Name = "connectToolStripMenuItem";
            this.connectToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.connectToolStripMenuItem.Text = "Connect";
            this.connectToolStripMenuItem.ToolTipText = "Conenct object";
            this.connectToolStripMenuItem.Click += new System.EventHandler(this.connectToolStripMenuItem_Click);
            // 
            // disconnectToolStripMenuItem
            // 
            this.disconnectToolStripMenuItem.Image = global::DemoClient.Properties.Resources.disconnect;
            this.disconnectToolStripMenuItem.Name = "disconnectToolStripMenuItem";
            this.disconnectToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.disconnectToolStripMenuItem.Text = "Disconnect";
            this.disconnectToolStripMenuItem.ToolTipText = "Disconnect object";
            this.disconnectToolStripMenuItem.Click += new System.EventHandler(this.disconnectToolStripMenuItem_Click);
            // 
            // readToolStripMenuItem
            // 
            this.readToolStripMenuItem.Image = global::DemoClient.Properties.Resources.service;
            this.readToolStripMenuItem.Name = "readToolStripMenuItem";
            this.readToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.readToolStripMenuItem.Text = "Read";
            this.readToolStripMenuItem.ToolTipText = "Read the selected item values";
            this.readToolStripMenuItem.Click += new System.EventHandler(this.readToolStripMenuItem_Click);
            // 
            // writeToolStripMenuItem
            // 
            this.writeToolStripMenuItem.Image = global::DemoClient.Properties.Resources.service;
            this.writeToolStripMenuItem.Name = "writeToolStripMenuItem";
            this.writeToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.writeToolStripMenuItem.Text = "Write";
            this.writeToolStripMenuItem.ToolTipText = "Write the selected item values";
            this.writeToolStripMenuItem.Click += new System.EventHandler(this.writeToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(150, 6);
            // 
            // propertiesToolStripMenuItem
            // 
            this.propertiesToolStripMenuItem.Image = global::DemoClient.Properties.Resources.properties;
            this.propertiesToolStripMenuItem.Name = "propertiesToolStripMenuItem";
            this.propertiesToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F7;
            this.propertiesToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.propertiesToolStripMenuItem.Text = "Properties";
            this.propertiesToolStripMenuItem.ToolTipText = "Show the properties of the selected item";
            this.propertiesToolStripMenuItem.Click += new System.EventHandler(this.propertiesToolStripMenuItem_Click);
            // 
            // ItemsControl
            // 
            this.Controls.Add(this.itemsListView);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "ItemsControl";
            this.Size = new System.Drawing.Size(584, 464);
            this.Enter += new System.EventHandler(this.ItemsControl_Enter);
            this.contextMenuStripItems.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView itemsListView;
        private System.Windows.Forms.ColumnHeader itemColumnHeader;
        private System.Windows.Forms.ColumnHeader valueColumnHeader;
        private System.Windows.Forms.ColumnHeader qualityColumnHeader;
        private System.Windows.Forms.ColumnHeader timeStampColumnHeader;
        private System.Windows.Forms.ColumnHeader sessionColumnHeader;
        private System.Windows.Forms.ColumnHeader subscriptionColumnHeader;
        private System.Windows.Forms.ContextMenuStrip contextMenuStripItems;
        private System.Windows.Forms.ToolStripMenuItem deleteToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem stateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem activateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem connectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem disconnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem readToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem writeToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem propertiesToolStripMenuItem;
        private System.Windows.Forms.ImageList imageListItems;
    }
}
