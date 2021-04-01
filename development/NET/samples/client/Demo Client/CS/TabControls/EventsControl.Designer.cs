namespace DemoClient.TabControls
{
    partial class EventsControl
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EventsControl));
            this.eventsListView = new System.Windows.Forms.ListView();
            this.sourceColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.severityColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.messageColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.timeColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.actorIDColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.sessionColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.subscriptionColumnHeader = new System.Windows.Forms.ColumnHeader();
            this.imageListEvents = new System.Windows.Forms.ImageList(this.components);
            this.SuspendLayout();
            // 
            // eventsListView
            // 
            this.eventsListView.CausesValidation = false;
            this.eventsListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.sourceColumnHeader,
            this.severityColumnHeader,
            this.messageColumnHeader,
            this.timeColumnHeader,
            this.actorIDColumnHeader,
            this.sessionColumnHeader,
            this.subscriptionColumnHeader});
            this.eventsListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.eventsListView.FullRowSelect = true;
            this.eventsListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.eventsListView.HideSelection = false;
            this.eventsListView.Location = new System.Drawing.Point(0, 0);
            this.eventsListView.MultiSelect = false;
            this.eventsListView.Name = "eventsListView";
            this.eventsListView.ShowGroups = false;
            this.eventsListView.Size = new System.Drawing.Size(584, 464);
            this.eventsListView.SmallImageList = this.imageListEvents;
            this.eventsListView.TabIndex = 1;
            this.eventsListView.UseCompatibleStateImageBehavior = false;
            this.eventsListView.View = System.Windows.Forms.View.Details;
            // 
            // sourceColumnHeader
            // 
            this.sourceColumnHeader.Text = "Source";
            this.sourceColumnHeader.Width = 90;
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
            this.timeColumnHeader.Width = 90;
            // 
            // actorIDColumnHeader
            // 
            this.actorIDColumnHeader.Text = "Actor ID";
            // 
            // sessionColumnHeader
            // 
            this.sessionColumnHeader.Text = "Session";
            this.sessionColumnHeader.Width = 95;
            // 
            // subscriptionColumnHeader
            // 
            this.subscriptionColumnHeader.Text = "Subscription";
            this.subscriptionColumnHeader.Width = 95;
            // 
            // imageListEvents
            // 
            this.imageListEvents.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListEvents.ImageStream")));
            this.imageListEvents.TransparentColor = System.Drawing.Color.Transparent;
            this.imageListEvents.Images.SetKeyName(0, "events.ico");
            // 
            // EventsControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.eventsListView);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "EventsControl";
            this.Size = new System.Drawing.Size(584, 464);
            this.Enter += new System.EventHandler(this.EventsControl_Enter);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView eventsListView;
        private System.Windows.Forms.ColumnHeader sourceColumnHeader;
        private System.Windows.Forms.ColumnHeader severityColumnHeader;
        private System.Windows.Forms.ColumnHeader messageColumnHeader;
        private System.Windows.Forms.ColumnHeader timeColumnHeader;
        private System.Windows.Forms.ColumnHeader sessionColumnHeader;
        private System.Windows.Forms.ColumnHeader subscriptionColumnHeader;
        private System.Windows.Forms.ColumnHeader actorIDColumnHeader;
        private System.Windows.Forms.ImageList imageListEvents;
    }
}
