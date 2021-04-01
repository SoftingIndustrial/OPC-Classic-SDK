namespace DemoClient.TabControls
{
    partial class ConditionsUserControl
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
            this.conditionsListView = new System.Windows.Forms.ListView();
            this.sourceColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.conditionColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.severityColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.messageColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.timeColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.actorIDColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.subconditionColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.sessionColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.SuspendLayout();
            // 
            // conditionsListView
            // 
            this.conditionsListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.sourceColumnHeader,
            this.conditionColumnHeader,
            this.severityColumnHeader,
            this.messageColumnHeader,
            this.timeColumnHeader,
            this.actorIDColumnHeader,
            this.subconditionColumnHeader,
            this.sessionColumnHeader});
            this.conditionsListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.conditionsListView.FullRowSelect = true;
            this.conditionsListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.conditionsListView.Location = new System.Drawing.Point(0, 0);
            this.conditionsListView.Margin = new System.Windows.Forms.Padding(0);
            this.conditionsListView.Name = "conditionsListView";
            this.conditionsListView.Size = new System.Drawing.Size(584, 464);
            this.conditionsListView.TabIndex = 2;
            this.conditionsListView.UseCompatibleStateImageBehavior = false;
            this.conditionsListView.View = System.Windows.Forms.View.Details;
            // 
            // sourceColumnHeader
            // 
            this.sourceColumnHeader.Text = "Source";
            this.sourceColumnHeader.Width = 80;
            // 
            // conditionColumnHeader
            // 
            this.conditionColumnHeader.Text = "Condition";
            // 
            // severityColumnHeader
            // 
            this.severityColumnHeader.Text = "Severity";
            // 
            // messageColumnHeader
            // 
            this.messageColumnHeader.Text = "Message";
            // 
            // timeColumnHeader
            // 
            this.timeColumnHeader.Text = "Time";
            // 
            // actorIDColumnHeader
            // 
            this.actorIDColumnHeader.Text = "Actor ID";
            this.actorIDColumnHeader.Width = 55;
            // 
            // subconditionColumnHeader
            // 
            this.subconditionColumnHeader.Text = "Subcondition";
            this.subconditionColumnHeader.Width = 77;
            // 
            // sessionColumnHeader
            // 
            this.sessionColumnHeader.Text = "Session";
            // 
            // ConditionsUserControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.conditionsListView);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "ConditionsUserControl";
            this.Size = new System.Drawing.Size(584, 464);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView conditionsListView;
        private System.Windows.Forms.ColumnHeader sourceColumnHeader;
        private System.Windows.Forms.ColumnHeader conditionColumnHeader;
        private System.Windows.Forms.ColumnHeader messageColumnHeader;
        private System.Windows.Forms.ColumnHeader timeColumnHeader;
        private System.Windows.Forms.ColumnHeader actorIDColumnHeader;
        private System.Windows.Forms.ColumnHeader sessionColumnHeader;
        private System.Windows.Forms.ColumnHeader subconditionColumnHeader;
        private System.Windows.Forms.ColumnHeader severityColumnHeader;
    }
}
