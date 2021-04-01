using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Softing.OPCToolbox.Client;

namespace DemoClient.TabControls
{
    public partial class EventsControl : UserControl
    {
        #region Constants

        private const string SUBSCRIPTION = "Subscription";

        private const int IMAGE_INDEX = 0;

        #endregion

        #region Delegates

        public event EnableButtonsHandler EnableButtons;

        #endregion

        #region Public Methods

        #region Constructor

        public EventsControl()
        {
            InitializeComponent();

            this.DoubleBuffered = true;
        }

        #endregion

        #region Received Events

        public void ReceivedEvents(AeSubscription anAeSubscription, bool isRefresh, bool lastRefresh, AeEvent[] events)
        {
            try
            {
                this.SuspendLayout();                
                for (int i = 0; i < events.Length; i++)
                {
                    if (events[i].EventType == Softing.OPCToolbox.EnumEventType.SIMPLE ||
                        events[i].EventType == Softing.OPCToolbox.EnumEventType.TRACKING)
                    {
                        ListViewItem item = new ListViewItem(events[i].SourcePath);
                        item.Tag = events[i];
                        item.ImageIndex = IMAGE_INDEX;
                        item.SubItems.Add(events[i].Severity.ToString());
                        item.SubItems.Add(events[i].Message);
                        item.SubItems.Add(events[i].OccurenceTime.ToString());
                        item.SubItems.Add(events[i].ActorId);
                        item.SubItems.Add(anAeSubscription.AeSession.Url);
                        item.SubItems.Add(SUBSCRIPTION);

                        eventsListView.Items.Insert(0,item);
                    }
                }
                this.ResumeLayout();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        #endregion

        /// <summary>
        /// Empty the list
        /// </summary>
        public void EmptyEvents()
        {
            eventsListView.Items.Clear();
        }

        /// <summary>
        /// Check if there are events added
        /// </summary>
        /// <returns></returns>
        public bool IsEmptyEvents()
        {
            if (eventsListView.Items.Count > 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        #endregion

        #region Events Handlers

        /// <summary>
        /// Enable/Disable Buttons
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void EventsControl_Enter(object sender, EventArgs e)
        {
            if (EnableButtons != null)
            {
                EnableButtons(false, false, false, false, false);
            }
        }

        #endregion
    }
}
