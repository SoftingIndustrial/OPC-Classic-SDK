namespace TestClientGui
{
	partial class TestClientForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose( bool disposing )
		{
			if( disposing && ( components != null ) )
			{
				components.Dispose();
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TestClientForm));
			this.treeView1 = new System.Windows.Forms.TreeView();
			this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.insertCopyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.removeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.debugToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.collapseChildrenToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.collapseSuccededToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
			this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
			this.statusStrip1 = new System.Windows.Forms.StatusStrip();
			this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
			this.toolStripProgressBar1 = new System.Windows.Forms.ToolStripProgressBar();
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.toolStripSplitButtonOpenFile = new System.Windows.Forms.ToolStripSplitButton();
			this.toolStripSplitButtonSaveFile = new System.Windows.Forms.ToolStripSplitButton();
			this.saveTestsWithResultsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.saveTestsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.toolStripButtonRun = new System.Windows.Forms.ToolStripButton();
			this.toolStripButtonStop = new System.Windows.Forms.ToolStripButton();
			this.toolStripButtonClearResults = new System.Windows.Forms.ToolStripButton();
			this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
			this.toolStripButtonShowCheckboxes = new System.Windows.Forms.ToolStripButton();
			this.toolStripButtonProperties = new System.Windows.Forms.ToolStripButton();
			this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
			this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
			this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
			this.expandChildrenToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.contextMenuStrip1.SuspendLayout();
			this.toolStripContainer1.BottomToolStripPanel.SuspendLayout();
			this.toolStripContainer1.ContentPanel.SuspendLayout();
			this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
			this.toolStripContainer1.SuspendLayout();
			this.statusStrip1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			this.toolStrip1.SuspendLayout();
			this.SuspendLayout();
			//
			// treeView1
			//
			this.treeView1.ContextMenuStrip = this.contextMenuStrip1;
			this.treeView1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.treeView1.FullRowSelect = true;
			this.treeView1.HideSelection = false;
			this.treeView1.ImageIndex = 0;
			this.treeView1.ImageList = this.imageList1;
			this.treeView1.Location = new System.Drawing.Point(0, 0);
			this.treeView1.Name = "treeView1";
			this.treeView1.SelectedImageIndex = 0;
			this.treeView1.Size = new System.Drawing.Size(354, 560);
			this.treeView1.TabIndex = 0;
			this.treeView1.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterCheck);
			this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
			this.treeView1.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView1_NodeMouseClick);
			//
			// contextMenuStrip1
			//
			this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
			this.insertCopyToolStripMenuItem,
			this.removeToolStripMenuItem,
			this.debugToolStripMenuItem,
			this.collapseChildrenToolStripMenuItem,
			this.collapseSuccededToolStripMenuItem,
			this.expandChildrenToolStripMenuItem});
			this.contextMenuStrip1.Name = "contextMenuStrip1";
			this.contextMenuStrip1.Size = new System.Drawing.Size(173, 158);
			//
			// insertCopyToolStripMenuItem
			//
			this.insertCopyToolStripMenuItem.Name = "insertCopyToolStripMenuItem";
			this.insertCopyToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
			this.insertCopyToolStripMenuItem.Text = "Insert Copy";
			this.insertCopyToolStripMenuItem.Click += new System.EventHandler(this.insertCopyToolStripMenuItem_Click);
			//
			// removeToolStripMenuItem
			//
			this.removeToolStripMenuItem.Name = "removeToolStripMenuItem";
			this.removeToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
			this.removeToolStripMenuItem.Text = "Remove";
			this.removeToolStripMenuItem.Click += new System.EventHandler(this.removeToolStripMenuItem_Click);
			//
			// debugToolStripMenuItem
			//
			this.debugToolStripMenuItem.Name = "debugToolStripMenuItem";
			this.debugToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
			this.debugToolStripMenuItem.Text = "Debug";
			this.debugToolStripMenuItem.Visible = false;
			this.debugToolStripMenuItem.Click += new System.EventHandler(this.debugToolStripMenuItem_Click);
			//
			// collapseChildrenToolStripMenuItem
			//
			this.collapseChildrenToolStripMenuItem.Name = "collapseChildrenToolStripMenuItem";
			this.collapseChildrenToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
			this.collapseChildrenToolStripMenuItem.Text = "Collapse children";
			this.collapseChildrenToolStripMenuItem.Click += new System.EventHandler(this.collapseChildrenToolStripMenuItem_Click);
			//
			// collapseSuccededToolStripMenuItem
			//
			this.collapseSuccededToolStripMenuItem.Name = "collapseSuccededToolStripMenuItem";
			this.collapseSuccededToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
			this.collapseSuccededToolStripMenuItem.Text = "Collapse succeded";
			this.collapseSuccededToolStripMenuItem.Click += new System.EventHandler(this.collapseSuccededToolStripMenuItem_Click);
			//
			// imageList1
			//
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList1.Images.SetKeyName(0, "TaskHS.png");
			this.imageList1.Images.SetKeyName(1, "109_AllAnnotations_Default_16x16_72.png");
			this.imageList1.Images.SetKeyName(2, "109_AllAnnotations_Error_16x16_72.png");
			this.imageList1.Images.SetKeyName(3, "109_AllAnnotations_Warning_16x16_72.png");
			this.imageList1.Images.SetKeyName(4, "List_BulletsHS.png");
			this.imageList1.Images.SetKeyName(5, "List_NumberedHS.png");
			this.imageList1.Images.SetKeyName(6, "PlayHS.png");
			this.imageList1.Images.SetKeyName(7, "book_reportHS.png");
			//
			// propertyGrid1
			//
			this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.propertyGrid1.Location = new System.Drawing.Point(0, 0);
			this.propertyGrid1.Name = "propertyGrid1";
			this.propertyGrid1.Size = new System.Drawing.Size(704, 560);
			this.propertyGrid1.TabIndex = 1;
			//
			// toolStripContainer1
			//
			//
			// toolStripContainer1.BottomToolStripPanel
			//
			this.toolStripContainer1.BottomToolStripPanel.Controls.Add(this.statusStrip1);
			//
			// toolStripContainer1.ContentPanel
			//
			this.toolStripContainer1.ContentPanel.Controls.Add(this.splitContainer1);
			this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(1062, 560);
			this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.toolStripContainer1.LeftToolStripPanelVisible = false;
			this.toolStripContainer1.Location = new System.Drawing.Point(0, 0);
			this.toolStripContainer1.Name = "toolStripContainer1";
			this.toolStripContainer1.RightToolStripPanelVisible = false;
			this.toolStripContainer1.Size = new System.Drawing.Size(1062, 607);
			this.toolStripContainer1.TabIndex = 3;
			this.toolStripContainer1.Text = "toolStripContainer1";
			//
			// toolStripContainer1.TopToolStripPanel
			//
			this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.toolStrip1);
			//
			// statusStrip1
			//
			this.statusStrip1.Dock = System.Windows.Forms.DockStyle.None;
			this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
			this.toolStripStatusLabel1,
			this.toolStripProgressBar1});
			this.statusStrip1.Location = new System.Drawing.Point(0, 0);
			this.statusStrip1.Name = "statusStrip1";
			this.statusStrip1.Size = new System.Drawing.Size(1062, 22);
			this.statusStrip1.TabIndex = 0;
			//
			// toolStripStatusLabel1
			//
			this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
			this.toolStripStatusLabel1.Size = new System.Drawing.Size(39, 17);
			this.toolStripStatusLabel1.Text = "Ready";
			//
			// toolStripProgressBar1
			//
			this.toolStripProgressBar1.Name = "toolStripProgressBar1";
			this.toolStripProgressBar1.Size = new System.Drawing.Size(100, 16);
			//
			// splitContainer1
			//
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.Location = new System.Drawing.Point(0, 0);
			this.splitContainer1.Name = "splitContainer1";
			//
			// splitContainer1.Panel1
			//
			this.splitContainer1.Panel1.Controls.Add(this.treeView1);
			//
			// splitContainer1.Panel2
			//
			this.splitContainer1.Panel2.Controls.Add(this.propertyGrid1);
			this.splitContainer1.Size = new System.Drawing.Size(1062, 560);
			this.splitContainer1.SplitterDistance = 354;
			this.splitContainer1.TabIndex = 0;
			//
			// toolStrip1
			//
			this.toolStrip1.Dock = System.Windows.Forms.DockStyle.None;
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
			this.toolStripSplitButtonOpenFile,
			this.toolStripSplitButtonSaveFile,
			this.toolStripSeparator1,
			this.toolStripButtonRun,
			this.toolStripButtonStop,
			this.toolStripButtonClearResults,
			this.toolStripSeparator2,
			this.toolStripButtonShowCheckboxes,
			this.toolStripButtonProperties});
			this.toolStrip1.Location = new System.Drawing.Point(3, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.Size = new System.Drawing.Size(203, 25);
			this.toolStrip1.TabIndex = 0;
			//
			// toolStripSplitButtonOpenFile
			//
			this.toolStripSplitButtonOpenFile.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripSplitButtonOpenFile.Image = ((System.Drawing.Image)(resources.GetObject("toolStripSplitButtonOpenFile.Image")));
			this.toolStripSplitButtonOpenFile.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripSplitButtonOpenFile.Name = "toolStripSplitButtonOpenFile";
			this.toolStripSplitButtonOpenFile.Size = new System.Drawing.Size(32, 22);
			this.toolStripSplitButtonOpenFile.Text = "Open";
			this.toolStripSplitButtonOpenFile.ButtonClick += new System.EventHandler(this.toolStripSplitButtonOpenFile_ButtonClick);
			//
			// toolStripSplitButtonSaveFile
			//
			this.toolStripSplitButtonSaveFile.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripSplitButtonSaveFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
			this.saveTestsWithResultsToolStripMenuItem,
			this.saveTestsToolStripMenuItem});
			this.toolStripSplitButtonSaveFile.Image = ((System.Drawing.Image)(resources.GetObject("toolStripSplitButtonSaveFile.Image")));
			this.toolStripSplitButtonSaveFile.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripSplitButtonSaveFile.Name = "toolStripSplitButtonSaveFile";
			this.toolStripSplitButtonSaveFile.Size = new System.Drawing.Size(32, 22);
			this.toolStripSplitButtonSaveFile.Text = "Save";
			//
			// saveTestsWithResultsToolStripMenuItem
			//
			this.saveTestsWithResultsToolStripMenuItem.Name = "saveTestsWithResultsToolStripMenuItem";
			this.saveTestsWithResultsToolStripMenuItem.Size = new System.Drawing.Size(188, 22);
			this.saveTestsWithResultsToolStripMenuItem.Text = "Save tests with results";
			this.saveTestsWithResultsToolStripMenuItem.Click += new System.EventHandler(this.saveTestsWithResultsToolStripMenuItem_Click);
			//
			// saveTestsToolStripMenuItem
			//
			this.saveTestsToolStripMenuItem.Name = "saveTestsToolStripMenuItem";
			this.saveTestsToolStripMenuItem.Size = new System.Drawing.Size(188, 22);
			this.saveTestsToolStripMenuItem.Text = "Save tests";
			this.saveTestsToolStripMenuItem.Click += new System.EventHandler(this.saveTestsToolStripMenuItem_Click);
			//
			// toolStripSeparator1
			//
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
			//
			// toolStripButtonRun
			//
			this.toolStripButtonRun.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonRun.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonRun.Image")));
			this.toolStripButtonRun.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButtonRun.Name = "toolStripButtonRun";
			this.toolStripButtonRun.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonRun.Text = "Run";
			this.toolStripButtonRun.Click += new System.EventHandler(this.toolStripButtonRun_Click);
			//
			// toolStripButtonStop
			//
			this.toolStripButtonStop.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonStop.Enabled = false;
			this.toolStripButtonStop.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonStop.Image")));
			this.toolStripButtonStop.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButtonStop.Name = "toolStripButtonStop";
			this.toolStripButtonStop.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonStop.Text = "Stop";
			this.toolStripButtonStop.Click += new System.EventHandler(this.toolStripButtonStop_Click);
			//
			// toolStripButtonClearResults
			//
			this.toolStripButtonClearResults.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonClearResults.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonClearResults.Image")));
			this.toolStripButtonClearResults.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButtonClearResults.Name = "toolStripButtonClearResults";
			this.toolStripButtonClearResults.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonClearResults.Text = "Clear results";
			this.toolStripButtonClearResults.Click += new System.EventHandler(this.toolStripButtonClearResults_Click);
			//
			// toolStripSeparator2
			//
			this.toolStripSeparator2.Name = "toolStripSeparator2";
			this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
			//
			// toolStripButtonShowCheckboxes
			//
			this.toolStripButtonShowCheckboxes.CheckOnClick = true;
			this.toolStripButtonShowCheckboxes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonShowCheckboxes.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonShowCheckboxes.Image")));
			this.toolStripButtonShowCheckboxes.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButtonShowCheckboxes.Name = "toolStripButtonShowCheckboxes";
			this.toolStripButtonShowCheckboxes.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonShowCheckboxes.Text = "Show checkboxes";
			this.toolStripButtonShowCheckboxes.CheckedChanged += new System.EventHandler(this.toolStripButtonShowCheckboxes_CheckedChanged);
			//
			// toolStripButtonProperties
			//
			this.toolStripButtonProperties.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonProperties.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonProperties.Image")));
			this.toolStripButtonProperties.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButtonProperties.Name = "toolStripButtonProperties";
			this.toolStripButtonProperties.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonProperties.Text = "Global properties";
			this.toolStripButtonProperties.Click += new System.EventHandler(this.toolStripButtonProperties_Click);
			//
			// saveFileDialog1
			//
			this.saveFileDialog1.DefaultExt = "xml";
			this.saveFileDialog1.Filter = "XML Tests file|*.xml";
			//
			// backgroundWorker1
			//
			this.backgroundWorker1.WorkerReportsProgress = true;
			this.backgroundWorker1.WorkerSupportsCancellation = true;
			this.backgroundWorker1.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
			this.backgroundWorker1.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.backgroundWorker1_ProgressChanged);
			this.backgroundWorker1.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundWorker1_RunWorkerCompleted);
			//
			// openFileDialog1
			//
			this.openFileDialog1.Filter = "XML Tests file|*.xml";
			//
			// expandChildrenToolStripMenuItem
			//
			this.expandChildrenToolStripMenuItem.Name = "expandChildrenToolStripMenuItem";
			this.expandChildrenToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
			this.expandChildrenToolStripMenuItem.Text = "Expand children";
			this.expandChildrenToolStripMenuItem.Click += new System.EventHandler(this.expandChildrenToolStripMenuItem_Click);
			//
			// TestClientForm
			//
			this.AllowDrop = true;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1062, 607);
			this.Controls.Add(this.toolStripContainer1);
			this.Name = "TestClientForm";
			this.Text = "TestClient for Toolkit V4.30";
			this.Load += new System.EventHandler(this.TestClientForm_Load);
			this.DragDrop += new System.Windows.Forms.DragEventHandler(this.TestClientForm_DragDrop);
			this.DragEnter += new System.Windows.Forms.DragEventHandler(this.TestClientForm_DragEnter);
			this.contextMenuStrip1.ResumeLayout(false);
			this.toolStripContainer1.BottomToolStripPanel.ResumeLayout(false);
			this.toolStripContainer1.BottomToolStripPanel.PerformLayout();
			this.toolStripContainer1.ContentPanel.ResumeLayout(false);
			this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
			this.toolStripContainer1.TopToolStripPanel.PerformLayout();
			this.toolStripContainer1.ResumeLayout(false);
			this.toolStripContainer1.PerformLayout();
			this.statusStrip1.ResumeLayout(false);
			this.statusStrip1.PerformLayout();
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TreeView treeView1;
		private System.Windows.Forms.PropertyGrid propertyGrid1;
		private System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.ToolStripContainer toolStripContainer1;
		private System.Windows.Forms.StatusStrip statusStrip1;
		private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
		private System.Windows.Forms.ToolStripProgressBar toolStripProgressBar1;
		private System.Windows.Forms.SplitContainer splitContainer1;
		private System.Windows.Forms.ToolStrip toolStrip1;
		private System.Windows.Forms.ToolStripSplitButton toolStripSplitButtonOpenFile;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
		private System.Windows.Forms.ToolStripButton toolStripButtonRun;
		private System.Windows.Forms.ToolStripSplitButton toolStripSplitButtonSaveFile;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
		private System.Windows.Forms.ToolStripButton toolStripButtonShowCheckboxes;
		private System.Windows.Forms.ToolStripMenuItem saveTestsWithResultsToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem saveTestsToolStripMenuItem;
		private System.Windows.Forms.SaveFileDialog saveFileDialog1;
		private System.ComponentModel.BackgroundWorker backgroundWorker1;
		private System.Windows.Forms.ToolStripButton toolStripButtonStop;
		private System.Windows.Forms.OpenFileDialog openFileDialog1;
		private System.Windows.Forms.ToolStripButton toolStripButtonProperties;
		private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
		private System.Windows.Forms.ToolStripMenuItem insertCopyToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem removeToolStripMenuItem;
		private System.Windows.Forms.ToolStripButton toolStripButtonClearResults;
		private System.Windows.Forms.ToolStripMenuItem debugToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem collapseChildrenToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem collapseSuccededToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem expandChildrenToolStripMenuItem;
	}
}

