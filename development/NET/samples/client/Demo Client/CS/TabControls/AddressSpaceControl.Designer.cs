namespace DemoClient.TabControls
{
    partial class AddressSpaceControl
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AddressSpaceControl));
            this.browseTreeView = new System.Windows.Forms.TreeView();
            this.imageListAddressSpace = new System.Windows.Forms.ImageList(this.components);
            this.contextMenuStripAll = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addItemsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.writeItemsIdToFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.readPropertiesToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStripProperties = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addItemToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.readToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.writeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.readPropertiesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStripAll.SuspendLayout();
            this.contextMenuStripProperties.SuspendLayout();
            this.SuspendLayout();
            // 
            // browseTreeView
            // 
            this.browseTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.browseTreeView.HideSelection = false;
            this.browseTreeView.ImageIndex = 0;
            this.browseTreeView.ImageList = this.imageListAddressSpace;
            this.browseTreeView.Location = new System.Drawing.Point(0, 0);
            this.browseTreeView.Name = "browseTreeView";
            this.browseTreeView.SelectedImageIndex = 0;
            this.browseTreeView.Size = new System.Drawing.Size(584, 464);
            this.browseTreeView.TabIndex = 0;
            this.browseTreeView.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.browseTreeView_NodeMouseDoubleClick);
            this.browseTreeView.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.browseTreeView_BeforeExpand);
            this.browseTreeView.BeforeCollapse += new System.Windows.Forms.TreeViewCancelEventHandler(this.browseTreeView_BeforeCollapse);
            this.browseTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.browseTreeView_AfterSelect);
            this.browseTreeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.browseTreeView_NodeMouseClick);
            // 
            // imageListAddressSpace
            // 
            this.imageListAddressSpace.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListAddressSpace.ImageStream")));
            this.imageListAddressSpace.TransparentColor = System.Drawing.Color.Transparent;
            this.imageListAddressSpace.Images.SetKeyName(0, "BrowseNode.ico");
            this.imageListAddressSpace.Images.SetKeyName(1, "Node.ico");
            this.imageListAddressSpace.Images.SetKeyName(2, "Tag.ico");
            this.imageListAddressSpace.Images.SetKeyName(3, "Prop.ico");
            // 
            // contextMenuStripAll
            // 
            this.contextMenuStripAll.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addItemsToolStripMenuItem,
            this.writeItemsIdToFileToolStripMenuItem,
            this.toolStripSeparator4,
            this.readPropertiesToolStripMenuItem1});
            this.contextMenuStripAll.Name = "contextMenuStripAll";
            this.contextMenuStripAll.Size = new System.Drawing.Size(184, 76);
            // 
            // addItemsToolStripMenuItem
            // 
            this.addItemsToolStripMenuItem.Name = "addItemsToolStripMenuItem";
            this.addItemsToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
            this.addItemsToolStripMenuItem.Text = "Add Items";
            this.addItemsToolStripMenuItem.ToolTipText = "Add items of the selected object to the subscription";
            this.addItemsToolStripMenuItem.Click += new System.EventHandler(this.addItemsToolStripMenuItem_Click);
            // 
            // writeItemsIdToFileToolStripMenuItem
            // 
            this.writeItemsIdToFileToolStripMenuItem.Name = "writeItemsIdToFileToolStripMenuItem";
            this.writeItemsIdToFileToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
            this.writeItemsIdToFileToolStripMenuItem.Text = "Write ItemIds to File";
            this.writeItemsIdToFileToolStripMenuItem.Click += new System.EventHandler(this.writeItemsIdToFileToolStripMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(180, 6);
            // 
            // readPropertiesToolStripMenuItem1
            // 
            this.readPropertiesToolStripMenuItem1.Image = global::DemoClient.Properties.Resources.properties;
            this.readPropertiesToolStripMenuItem1.Name = "readPropertiesToolStripMenuItem1";
            this.readPropertiesToolStripMenuItem1.Size = new System.Drawing.Size(183, 22);
            this.readPropertiesToolStripMenuItem1.Text = "Read Properties";
            this.readPropertiesToolStripMenuItem1.ToolTipText = "Show properties of the selected object";
            this.readPropertiesToolStripMenuItem1.Click += new System.EventHandler(this.readPropertiesToolStripMenuItem1_Click);
            // 
            // contextMenuStripProperties
            // 
            this.contextMenuStripProperties.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addItemToolStripMenuItem,
            this.toolStripSeparator1,
            this.readToolStripMenuItem,
            this.writeToolStripMenuItem,
            this.toolStripSeparator2,
            this.readPropertiesToolStripMenuItem});
            this.contextMenuStripProperties.Name = "contextMenuStripAll";
            this.contextMenuStripProperties.Size = new System.Drawing.Size(157, 104);
            // 
            // addItemToolStripMenuItem
            // 
            this.addItemToolStripMenuItem.Name = "addItemToolStripMenuItem";
            this.addItemToolStripMenuItem.Size = new System.Drawing.Size(156, 22);
            this.addItemToolStripMenuItem.Text = "Add Item";
            this.addItemToolStripMenuItem.ToolTipText = "Add an item object";
            this.addItemToolStripMenuItem.Click += new System.EventHandler(this.addItemToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(153, 6);
            // 
            // readToolStripMenuItem
            // 
            this.readToolStripMenuItem.Image = global::DemoClient.Properties.Resources.service;
            this.readToolStripMenuItem.Name = "readToolStripMenuItem";
            this.readToolStripMenuItem.Size = new System.Drawing.Size(156, 22);
            this.readToolStripMenuItem.Text = "Read";
            this.readToolStripMenuItem.ToolTipText = "Read the selected item values";
            this.readToolStripMenuItem.Click += new System.EventHandler(this.readToolStripMenuItem_Click);
            // 
            // writeToolStripMenuItem
            // 
            this.writeToolStripMenuItem.Image = global::DemoClient.Properties.Resources.service;
            this.writeToolStripMenuItem.Name = "writeToolStripMenuItem";
            this.writeToolStripMenuItem.Size = new System.Drawing.Size(156, 22);
            this.writeToolStripMenuItem.Text = "Write";
            this.writeToolStripMenuItem.ToolTipText = "Write the selected item values";
            this.writeToolStripMenuItem.Click += new System.EventHandler(this.writeToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(153, 6);
            // 
            // readPropertiesToolStripMenuItem
            // 
            this.readPropertiesToolStripMenuItem.Image = global::DemoClient.Properties.Resources.properties;
            this.readPropertiesToolStripMenuItem.Name = "readPropertiesToolStripMenuItem";
            this.readPropertiesToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F7;
            this.readPropertiesToolStripMenuItem.Size = new System.Drawing.Size(156, 22);
            this.readPropertiesToolStripMenuItem.Text = "Properties ";
            this.readPropertiesToolStripMenuItem.ToolTipText = "Show properties of the selected object";
            this.readPropertiesToolStripMenuItem.Click += new System.EventHandler(this.readPropertiesToolStripMenuItem_Click);
            // 
            // AddressSpaceControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.browseTreeView);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "AddressSpaceControl";
            this.Size = new System.Drawing.Size(584, 464);
            this.Enter += new System.EventHandler(this.AddressSpaceControl_Enter);
            this.contextMenuStripAll.ResumeLayout(false);
            this.contextMenuStripProperties.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView browseTreeView;
        private System.Windows.Forms.ImageList imageListAddressSpace;
        private System.Windows.Forms.ContextMenuStrip contextMenuStripAll;
        private System.Windows.Forms.ContextMenuStrip contextMenuStripProperties;
        private System.Windows.Forms.ToolStripMenuItem addItemToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem readToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem writeToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem readPropertiesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addItemsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem writeItemsIdToFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem readPropertiesToolStripMenuItem1;
    }
}
