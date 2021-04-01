namespace DemoClient.DockWindows
{
    partial class PropertyWindow
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PropertyWindow));
            this.propertyGrid = new System.Windows.Forms.PropertyGrid();
            this.panelBottom = new System.Windows.Forms.Panel();
            this.linkLabelExecute = new System.Windows.Forms.LinkLabel();
            this.panelBottom.SuspendLayout();
            this.SuspendLayout();
            // 
            // propertyGrid
            // 
            this.propertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid.HelpVisible = false;
            this.propertyGrid.Location = new System.Drawing.Point(0, 3);
            this.propertyGrid.Name = "propertyGrid";
            this.propertyGrid.Size = new System.Drawing.Size(320, 390);
            this.propertyGrid.TabIndex = 0;
            this.propertyGrid.ToolbarVisible = false;
            // 
            // panelBottom
            // 
            this.panelBottom.Controls.Add(this.linkLabelExecute);
            this.panelBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panelBottom.Location = new System.Drawing.Point(0, 393);
            this.panelBottom.Name = "panelBottom";
            this.panelBottom.Size = new System.Drawing.Size(320, 32);
            this.panelBottom.TabIndex = 4;
            // 
            // linkLabelExecute
            // 
            this.linkLabelExecute.ActiveLinkColor = System.Drawing.Color.Firebrick;
            this.linkLabelExecute.AutoSize = true;
            this.linkLabelExecute.LinkColor = System.Drawing.Color.Brown;
            this.linkLabelExecute.Location = new System.Drawing.Point(3, 7);
            this.linkLabelExecute.Name = "linkLabelExecute";
            this.linkLabelExecute.Size = new System.Drawing.Size(78, 13);
            this.linkLabelExecute.TabIndex = 0;
            this.linkLabelExecute.TabStop = true;
            this.linkLabelExecute.Text = "Apply Changes";
            this.linkLabelExecute.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelExecute_LinkClicked);
            // 
            // PropertyWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(320, 428);
            this.Controls.Add(this.propertyGrid);
            this.Controls.Add(this.panelBottom);
            this.HideOnClose = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "PropertyWindow";
            this.Padding = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.TabText = "Properties";
            this.Text = "Properties";
            this.panelBottom.ResumeLayout(false);
            this.panelBottom.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PropertyGrid propertyGrid;
        private System.Windows.Forms.Panel panelBottom;
        private System.Windows.Forms.LinkLabel linkLabelExecute;
    }
}
