namespace DemoClient.TabControls
{
    partial class ServerDiscoveryControl
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ServerDiscoveryControl));
            this.urlLlabel = new System.Windows.Forms.Label();
            this.serversTreeView = new System.Windows.Forms.TreeView();
            this.contextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addServerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.serversImageList = new System.Windows.Forms.ImageList(this.components);
            this.urlTextBox = new System.Windows.Forms.TextBox();
            this.addServerButton = new System.Windows.Forms.Button();
            this.contextMenuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // urlLlabel
            // 
            this.urlLlabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.urlLlabel.AutoSize = true;
            this.urlLlabel.Location = new System.Drawing.Point(3, 447);
            this.urlLlabel.Name = "urlLlabel";
            this.urlLlabel.Size = new System.Drawing.Size(35, 13);
            this.urlLlabel.TabIndex = 1;
            this.urlLlabel.Text = "URL: ";
            // 
            // serversTreeView
            // 
            this.serversTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.serversTreeView.ContextMenuStrip = this.contextMenuStrip;
            this.serversTreeView.HideSelection = false;
            this.serversTreeView.ImageIndex = 0;
            this.serversTreeView.ImageList = this.serversImageList;
            this.serversTreeView.ItemHeight = 18;
            this.serversTreeView.Location = new System.Drawing.Point(0, 0);
            this.serversTreeView.Margin = new System.Windows.Forms.Padding(0);
            this.serversTreeView.Name = "serversTreeView";
            this.serversTreeView.SelectedImageIndex = 0;
            this.serversTreeView.Size = new System.Drawing.Size(584, 439);
            this.serversTreeView.TabIndex = 0;
            this.serversTreeView.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.serversTreeView_NodeMouseDoubleClick);
            this.serversTreeView.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.serversTreeView_BeforeExpand);
            this.serversTreeView.BeforeCollapse += new System.Windows.Forms.TreeViewCancelEventHandler(this.serversTreeView_BeforeCollapse);
            this.serversTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.serversTreeView_AfterSelect);
            this.serversTreeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.serversTreeView_NodeMouseClick);
            // 
            // contextMenuStrip
            // 
            this.contextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addServerToolStripMenuItem});
            this.contextMenuStrip.Name = "contextMenuStrip";
            this.contextMenuStrip.Size = new System.Drawing.Size(140, 26);
            // 
            // addServerToolStripMenuItem
            // 
            this.addServerToolStripMenuItem.Name = "addServerToolStripMenuItem";
            this.addServerToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.addServerToolStripMenuItem.Text = "Add Server";
            this.addServerToolStripMenuItem.ToolTipText = "Add selected server object";
            this.addServerToolStripMenuItem.Click += new System.EventHandler(this.addServerToolStripMenuItem_Click);
            // 
            // serversImageList
            // 
            this.serversImageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("serversImageList.ImageStream")));
            this.serversImageList.TransparentColor = System.Drawing.Color.Transparent;
            this.serversImageList.Images.SetKeyName(0, "localPC.ico");
            this.serversImageList.Images.SetKeyName(1, "remote.ico");
            this.serversImageList.Images.SetKeyName(2, "remoteServers.ico");
            this.serversImageList.Images.SetKeyName(3, "remotePC.ico");
            this.serversImageList.Images.SetKeyName(4, "Node.ico");
            this.serversImageList.Images.SetKeyName(5, "Tag.ico");
            this.serversImageList.Images.SetKeyName(6, "Prop.ico");
            // 
            // urlTextBox
            // 
            this.urlTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.urlTextBox.Location = new System.Drawing.Point(41, 444);
            this.urlTextBox.Name = "urlTextBox";
            this.urlTextBox.Size = new System.Drawing.Size(462, 20);
            this.urlTextBox.TabIndex = 1;
            this.urlTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.urlTextBox_KeyDown);
            // 
            // addServerButton
            // 
            this.addServerButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.addServerButton.Location = new System.Drawing.Point(509, 442);
            this.addServerButton.Name = "addServerButton";
            this.addServerButton.Size = new System.Drawing.Size(75, 23);
            this.addServerButton.TabIndex = 2;
            this.addServerButton.Text = "Add Server";
            this.addServerButton.UseVisualStyleBackColor = true;
            this.addServerButton.Click += new System.EventHandler(this.addServerButton_Click);
            // 
            // ServerDiscoveryControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.addServerButton);
            this.Controls.Add(this.urlTextBox);
            this.Controls.Add(this.urlLlabel);
            this.Controls.Add(this.serversTreeView);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "ServerDiscoveryControl";
            this.Size = new System.Drawing.Size(584, 464);
            this.Enter += new System.EventHandler(this.ServerDiscoveryControl_Enter);
            this.contextMenuStrip.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label urlLlabel;
        private System.Windows.Forms.TextBox urlTextBox;
        private System.Windows.Forms.Button addServerButton;
        private System.Windows.Forms.ImageList serversImageList;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem addServerToolStripMenuItem;
        private System.Windows.Forms.TreeView serversTreeView;

    }
}
