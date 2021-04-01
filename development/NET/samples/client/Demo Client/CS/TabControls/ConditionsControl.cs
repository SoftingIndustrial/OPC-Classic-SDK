using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using DemoClient.Helper;

namespace DemoClient.TabControls
{
    #region Enums

    public enum ShownConditions
    {
        All = 1,
        Session,
        Subscription
    }

    #endregion

    public partial class ConditionsControl : UserControl
    {
        #region Constants

        private const string SUBSCRIPTION = "Subscription";
        private const string AECONDITION = "AeCondition";

        private const int ENA_STATE = 0;
        private const int DIS_STATE = 1;
        private const int ACT_ENA_STATE = 2;
        private const int ACT_DIS_STATE = 3;
        private const int ACT_ENA_ACK_STATE = 4;
        private const int ACT_DIS_ACK_STATE = 5;


        #endregion   

        #region Private Members

        private Helper.ListViewColumnSorter colSorter = new DemoClient.Helper.ListViewColumnSorter();

        #endregion

        #region Delegates

        public event EnableButtonsHandler EnableButtons;

        #endregion

        #region Private and Protected Methods

        /// <summary>
        /// Add all conditions
        /// </summary>
        /// <param name="sessions"></param>
        private void ShowAllConditions(AeSession[] sessions)
        {
            if (sessions == null) return;
            foreach (AeSession session in sessions)
            {
                foreach (AeSubscription subscription in session.SubscriptionList)
                {
                    foreach (AeCondition condition in subscription.AeConditionList)
                    {
                        AddCondition(condition, subscription);
                    }
                }
            }
        }

        /// <summary>
        /// Add all session sunscriptions conditions
        /// </summary>
        /// <param name="sessions"></param>
        private void ShowSessionConditions(AeSession[] sessions)
        {
            if (sessions == null) return;
            foreach (AeSession session in sessions)
            {
                foreach (AeSubscription subscription in session.SubscriptionList)
                {
                    foreach (AeCondition condition in subscription.AeConditionList)
                    {
                        AddCondition(condition, subscription);
                    }
                }
            }
        }

        /// <summary>
        /// Add all subscription conditions
        /// </summary>
        /// <param name="aeSubscription"></param>
        /// <param name="conditions"></param>
        private void ShowSubscriptionConditions(AeSubscription aeSubscription, AeCondition[] conditions)
        {
            if (conditions == null)
            {
                conditions = aeSubscription.AeConditionList;
            }
            foreach (AeCondition condition in conditions)
            {
                AddCondition(condition, aeSubscription);
            }

        }

        /// <summary>
        /// Add Condition
        /// </summary>
        /// <param name="condition"></param>
        /// <param name="subsciption"></param>
        private void AddCondition(AeCondition condition, AeSubscription subsciption)
        {
            try
            {
                ListViewItem item = new ListViewItem(condition.SourcePath);
                item.ImageIndex = SetConditionImage(condition.State);
                condition.AeSession = subsciption.AeSession;
                item.Tag = condition;           
                item.SubItems.Add(condition.ConditionName);
                item.SubItems.Add(condition.Severity.ToString());
                item.SubItems.Add(condition.Message);
                item.SubItems.Add(ValueConverter.DateTimeOptions(condition.OccurenceTime));
                item.SubItems.Add(condition.ActorId);
                item.SubItems.Add(condition.SubConditionName);
                item.SubItems.Add(subsciption.AeSession.Url);
                item.SubItems.Add(SUBSCRIPTION);

                conditionsListView.Items.Add(item);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        /// <summary>
        /// Set condition state image
        /// </summary>
        /// <param name="enumConditionState"></param>
        private int SetConditionImage(Softing.OPCToolbox.EnumConditionState state)
        {
            switch (state)
            {
                case (EnumConditionState)0://disable
                    return DIS_STATE;
                case (EnumConditionState)1: //enable
                    return ENA_STATE;
                case (EnumConditionState)2: //active-disable
                    return ACT_DIS_STATE;
                case (EnumConditionState)3: //activate-enabled
                    return ACT_ENA_STATE;
                case (EnumConditionState)6://activ-acknoledged
                    return ACT_DIS_ACK_STATE;
                case (EnumConditionState)7://activ-enabled-acknoledge
                    return ACT_ENA_ACK_STATE;
            }
            return DIS_STATE;
        }

        #endregion

        #region Public Methods

        #region Constructor

        public ConditionsControl()
        {
            InitializeComponent();

            //set listview sorter
            conditionsListView.ListViewItemSorter = colSorter;
            colSorter.SortColumn = timeColumnHeader.Index;
            colSorter.Order = SortOrder.Descending;
        }

        #endregion

        #region Received Conditions

        public void ConditionsAdd(AeSubscription anAeSubscription, AeCondition[] conditions, ShownConditions cond, AeSession[] sessions)
        {
            try
            {
                this.SuspendLayout();                
                conditionsListView.Items.Clear();
                switch (cond)
                {
                    case ShownConditions.All:
                        ShowAllConditions(sessions);
                        break;
                    case ShownConditions.Session:
                        ShowSessionConditions(sessions);
                        break;
                    case ShownConditions.Subscription:
                        ShowSubscriptionConditions(anAeSubscription, conditions);
                        break;
                }
                conditionsListView.Sort();
                if (conditionsListView.Items.Count > 0)
                {
                    this.Enabled = true;
                }
                else
                {
                    this.Enabled = false;
                }
                this.ResumeLayout();
            }
            catch { }
        }

        /// <summary>
        /// check selected items
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void conditionsListView_MouseCaptureChanged(object sender, EventArgs e)
        {
            if (conditionsListView.SelectedItems.Count == 0)
            {
                conditionsListView.ContextMenuStrip = null;
            }
            else
            {
                conditionsListView.ContextMenuStrip = contextMenuStrip;
            }
        }

        #endregion

        public void EmptyConditions()
        {
            conditionsListView.Items.Clear();
        }

        #endregion

        #region Event Handlers

        private void ConditionsControl_Enter(object sender, EventArgs e)
        {
            if (EnableButtons != null)
            {
                EnableButtons(false, false, false, false, false);
            }
        }

        private void acknowledgeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                conditionsListView.Cursor = Cursors.WaitCursor;
                foreach (ListViewItem item in conditionsListView.SelectedItems)
                {
                    AeCondition condition = item.Tag as AeCondition;

                    int[] results = null;
                    condition.AeSession.AcknowledgeAeConditions(Environment.UserName, AECONDITION, new AeCondition[] { condition }, out results, null);

                }
            }
            catch { }
            finally
            {
                conditionsListView.Cursor = Cursors.Default;
            }
        }


        #endregion
    }
}
