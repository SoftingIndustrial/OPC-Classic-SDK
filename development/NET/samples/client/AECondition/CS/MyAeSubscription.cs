using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.Windows.Forms;

namespace AECondition
{
	public class MyAeSubscription : AeSubscription
	{

		#region Constructor
		//-----------------

		public MyAeSubscription(MyAeSession parentSession) : base(parentSession)
		{
			AeConditionsChanged += new AeConditionsChangedEventHandler(HandleAeConditionsChanged);

		}

		//--
		#endregion

		#region Private Members
		//---------------------


		#region Private Attributes
		//------------------------

		private OpcForm m_opcForm = null;

		//--
		#endregion


		//--
		#endregion

		#region Public Methods
		//---------------------

		public void SetForm(OpcForm form)
		{
			m_opcForm = form;
		}

		private string GetState(EnumConditionState state)
		{
			string stateToString = string.Empty;
			if ((state & EnumConditionState.ACTIVE) == EnumConditionState.ACTIVE)
			{
				stateToString += " ACT";
			}
			if ((state & EnumConditionState.ENABLED) == EnumConditionState.ENABLED)
			{
				stateToString += " ENA";
			}
			if ((state & EnumConditionState.ACKED) == EnumConditionState.ACKED)
			{
				stateToString += " ACK";
			}
			if (state == 0)
			{
				stateToString += " DIS";
			}
			return stateToString;
		}

		//--
		#endregion

		#region Public Properties
		//-----------------------


		//--
		#endregion

		#region Handles
		//-----------------------

		private void HandleAeConditionsChanged(AeSubscription anAeSubscription, AeCondition[] conditions)
		{

			if (m_opcForm.conditionlistView.InvokeRequired)
			{
				RemoveItemsFromListView riflw = new RemoveItemsFromListView(m_opcForm.RemoveItems);
				m_opcForm.conditionlistView.BeginInvoke(riflw, null);
			}
			else
			{
				m_opcForm.conditionlistView.Items.Clear();
			}
			string state = String.Empty;
			for (int i = 0; i < conditions.Length; i++)
			{
				AeCondition condition = conditions[i];
				state = GetState(condition.State);

				if (m_opcForm.conditionlistView.InvokeRequired)
				{
					ChangeConditionListView cclw = new ChangeConditionListView(m_opcForm.ChangeItem);
					m_opcForm.conditionlistView.BeginInvoke(cclw, new object[] { condition, state });
				}
				else
				{

					ListViewItem item = new ListViewItem(state);
					item.Tag = condition;
					m_opcForm.conditionlistView.Items.Add(item);
					item.SubItems.Add(condition.SourcePath);
					item.SubItems.Add(condition.ConditionName);
					item.SubItems.Add(condition.Severity.ToString());
					item.SubItems.Add(condition.Message);
					item.SubItems.Add(condition.OccurenceTime.ToString("hh:mm:ss.fff"));
					item.SubItems.Add(condition.ActorId);
					item.SubItems.Add(condition.SubConditionName);
					state = String.Empty;
				}
			}//end for
			System.Diagnostics.Debug.WriteLine("The conditions list has changed ");
		}

		//--
		#endregion

		public delegate void ChangeConditionListView(AeCondition condition, string state);
		public delegate void RemoveItemsFromListView();

	}   //	end class MyAeItem

}   //	end namespace
