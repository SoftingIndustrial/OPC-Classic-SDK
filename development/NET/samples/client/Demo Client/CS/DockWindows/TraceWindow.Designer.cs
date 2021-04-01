namespace DemoClient.DockWindows
{
    partial class TraceWindow
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TraceWindow));
            this.richTextBoxTraces = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // richTextBoxTraces
            // 
            this.richTextBoxTraces.BackColor = System.Drawing.Color.White;
            this.richTextBoxTraces.Dock = System.Windows.Forms.DockStyle.Fill;
            this.richTextBoxTraces.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.richTextBoxTraces.Location = new System.Drawing.Point(0, 0);
            this.richTextBoxTraces.Name = "richTextBoxTraces";
            this.richTextBoxTraces.ReadOnly = true;
            this.richTextBoxTraces.Size = new System.Drawing.Size(760, 266);
            this.richTextBoxTraces.TabIndex = 0;
            this.richTextBoxTraces.Text = "";
            // 
            // TraceWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(760, 266);
            this.Controls.Add(this.richTextBoxTraces);
            this.HideOnClose = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "TraceWindow";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottom;
            this.TabText = "Trace";
            this.Text = "Trace";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RichTextBox richTextBoxTraces;



    }
}
