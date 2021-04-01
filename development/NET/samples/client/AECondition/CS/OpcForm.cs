using System;
using System.Windows.Forms;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Client;

namespace AECondition
{
	/// <summary>
	/// Summary description for OpcForm.
	/// </summary>
	public class OpcForm : Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private OutProc m_outProc = null;
		private OpcClient m_opcClient = null;
		private Panel panel1;
		private ColumnHeader conditionStateColumnHeader;
		private ColumnHeader sourceColumnHeader;
		private ColumnHeader nameColumnHeader;
		private ColumnHeader severityColumnHeader;
		private ColumnHeader messageColumnHeader;
		private ColumnHeader timeColumnHeader;
		private ColumnHeader actorIDcolumnHeader;
		private ColumnHeader subConditionColumnHeader;
		private Button acknowledgeButton;
		private Button getConditionStateButton;
		private Button enableConditionsButton;
		private Button disableConditionsButton;
		private Panel panel2;
		private GroupBox conditionStateGroupBox;
		private GroupBox conditionListGroupBox;
		public RichTextBox conditionStateRichTextBox;
		public ListView conditionlistView;
		private MyAeSession m_session = null;
		private MyAeSubscription m_subscription = null;
		private ExecutionOptions m_executionOptions = null;

		public OpcForm()
		{
			InitializeComponent();
		}   //	end constructor

		public void SetOutProc(OutProc anOutProc)
		{
			try
			{
				m_outProc = anOutProc;
				m_opcClient = m_outProc.OpcClient;
				m_session = m_opcClient.GetSession();
				m_subscription = m_opcClient.GetSubscription();
				m_executionOptions = m_opcClient.GetExecutionOptions();
			}
			catch (Exception exc)
			{
				MessageBox.Show(exc.ToString());
			}   //	end try...catch
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
				{
					components.Dispose();
				}
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(OpcForm));
			this.panel1 = new Panel();
			this.panel2 = new Panel();
			this.conditionStateGroupBox = new GroupBox();
			this.conditionStateRichTextBox = new RichTextBox();
			this.getConditionStateButton = new Button();
			this.conditionListGroupBox = new GroupBox();
			this.conditionlistView = new ListView();
			this.conditionStateColumnHeader = new ColumnHeader();
			this.sourceColumnHeader = new ColumnHeader();
			this.nameColumnHeader = new ColumnHeader();
			this.severityColumnHeader = new ColumnHeader();
			this.messageColumnHeader = new ColumnHeader();
			this.timeColumnHeader = new ColumnHeader();
			this.actorIDcolumnHeader = new ColumnHeader();
			this.subConditionColumnHeader = new ColumnHeader();
			this.acknowledgeButton = new Button();
			this.disableConditionsButton = new Button();
			this.enableConditionsButton = new Button();
			this.panel1.SuspendLayout();
			this.panel2.SuspendLayout();
			this.conditionStateGroupBox.SuspendLayout();
			this.conditionListGroupBox.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.panel2);
			this.panel1.Dock = DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(904, 750);
			this.panel1.TabIndex = 0;
			// 
			// panel2
			// 
			this.panel2.Controls.Add(this.conditionStateGroupBox);
			this.panel2.Controls.Add(this.conditionListGroupBox);
			this.panel2.Location = new System.Drawing.Point(8, 8);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(896, 736);
			this.panel2.TabIndex = 11;
			// 
			// conditionStateGroupBox
			// 
			this.conditionStateGroupBox.Controls.Add(this.conditionStateRichTextBox);
			this.conditionStateGroupBox.Controls.Add(this.getConditionStateButton);
			this.conditionStateGroupBox.Dock = DockStyle.Bottom;
			this.conditionStateGroupBox.Location = new System.Drawing.Point(0, 368);
			this.conditionStateGroupBox.Name = "conditionStateGroupBox";
			this.conditionStateGroupBox.Size = new System.Drawing.Size(896, 368);
			this.conditionStateGroupBox.TabIndex = 1;
			this.conditionStateGroupBox.TabStop = false;
			this.conditionStateGroupBox.Text = "AE Condition State";
			// 
			// conditionStateRichTextBox
			// 
			this.conditionStateRichTextBox.BackColor = System.Drawing.SystemColors.Window;
			this.conditionStateRichTextBox.Location = new System.Drawing.Point(8, 24);
			this.conditionStateRichTextBox.Name = "conditionStateRichTextBox";
			this.conditionStateRichTextBox.ReadOnly = true;
			this.conditionStateRichTextBox.Size = new System.Drawing.Size(752, 336);
			this.conditionStateRichTextBox.TabIndex = 5;
			this.conditionStateRichTextBox.Text = "";
			// 
			// getConditionStateButton
			// 
			this.getConditionStateButton.Location = new System.Drawing.Point(768, 40);
			this.getConditionStateButton.Name = "getConditionStateButton";
			this.getConditionStateButton.Size = new System.Drawing.Size(120, 40);
			this.getConditionStateButton.TabIndex = 7;
			this.getConditionStateButton.Text = "Get Condition State";
			this.getConditionStateButton.Click += new System.EventHandler(this.GetConditionStateButtonOnClick);
			// 
			// conditionListGroupBox
			// 
			this.conditionListGroupBox.Controls.Add(this.conditionlistView);
			this.conditionListGroupBox.Controls.Add(this.acknowledgeButton);
			this.conditionListGroupBox.Controls.Add(this.disableConditionsButton);
			this.conditionListGroupBox.Controls.Add(this.enableConditionsButton);
			this.conditionListGroupBox.Dock = DockStyle.Top;
			this.conditionListGroupBox.Location = new System.Drawing.Point(0, 0);
			this.conditionListGroupBox.Name = "conditionListGroupBox";
			this.conditionListGroupBox.Size = new System.Drawing.Size(896, 352);
			this.conditionListGroupBox.TabIndex = 0;
			this.conditionListGroupBox.TabStop = false;
			this.conditionListGroupBox.Text = "AE Condition List";
			// 
			// conditionlistView
			// 
			this.conditionlistView.Columns.AddRange(new ColumnHeader[] {
																								this.conditionStateColumnHeader,
																								this.sourceColumnHeader,
																								this.nameColumnHeader,
																								this.severityColumnHeader,
																								this.messageColumnHeader,
																								this.timeColumnHeader,
																								this.actorIDcolumnHeader,
																								this.subConditionColumnHeader});
			this.conditionlistView.FullRowSelect = true;
			this.conditionlistView.Location = new System.Drawing.Point(8, 24);
			this.conditionlistView.MultiSelect = false;
			this.conditionlistView.Name = "conditionlistView";
			this.conditionlistView.Size = new System.Drawing.Size(760, 320);
			this.conditionlistView.TabIndex = 0;
			this.conditionlistView.View = View.Details;
			// 
			// conditionStateColumnHeader
			// 
			this.conditionStateColumnHeader.Text = "ConditionState";
			this.conditionStateColumnHeader.Width = 83;
			// 
			// sourceColumnHeader
			// 
			this.sourceColumnHeader.Text = "Source";
			this.sourceColumnHeader.Width = 148;
			// 
			// nameColumnHeader
			// 
			this.nameColumnHeader.Text = "Condition Name";
			this.nameColumnHeader.Width = 98;
			// 
			// severityColumnHeader
			// 
			this.severityColumnHeader.Text = "Severity";
			this.severityColumnHeader.Width = 61;
			// 
			// messageColumnHeader
			// 
			this.messageColumnHeader.Text = "Message";
			this.messageColumnHeader.Width = 92;
			// 
			// timeColumnHeader
			// 
			this.timeColumnHeader.Text = "Time";
			this.timeColumnHeader.Width = 105;
			// 
			// actorIDcolumnHeader
			// 
			this.actorIDcolumnHeader.Text = "ActorID";
			// 
			// subConditionColumnHeader
			// 
			this.subConditionColumnHeader.Text = "SubconditionName";
			this.subConditionColumnHeader.Width = 108;
			// 
			// acknowledgeButton
			// 
			this.acknowledgeButton.Location = new System.Drawing.Point(776, 56);
			this.acknowledgeButton.Name = "acknowledgeButton";
			this.acknowledgeButton.Size = new System.Drawing.Size(112, 40);
			this.acknowledgeButton.TabIndex = 8;
			this.acknowledgeButton.Text = "Acknowledge conditions";
			this.acknowledgeButton.Click += new System.EventHandler(this.AcknowledgeButtonOnClick);
			// 
			// disableConditionsButton
			// 
			this.disableConditionsButton.Location = new System.Drawing.Point(776, 168);
			this.disableConditionsButton.Name = "disableConditionsButton";
			this.disableConditionsButton.Size = new System.Drawing.Size(112, 40);
			this.disableConditionsButton.TabIndex = 10;
			this.disableConditionsButton.Text = "Disable conditions";
			this.disableConditionsButton.Click += new System.EventHandler(this.DisableConditionsButtonOnClick);
			// 
			// enableConditionsButton
			// 
			this.enableConditionsButton.Location = new System.Drawing.Point(776, 112);
			this.enableConditionsButton.Name = "enableConditionsButton";
			this.enableConditionsButton.Size = new System.Drawing.Size(112, 40);
			this.enableConditionsButton.TabIndex = 9;
			this.enableConditionsButton.Text = "Enable conditions";
			this.enableConditionsButton.Click += new System.EventHandler(this.EnableConditionsButtonOnClick);
			// 
			// AEConditions
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(904, 750);
			this.Controls.Add(this.panel1);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "AEConditions";
			this.Text = "AE Conditions";
			//this.Closing += new System.ComponentModel.CancelEventHandler(m_opcClient.Form1_Closing);
			this.panel1.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.conditionStateGroupBox.ResumeLayout(false);
			this.conditionListGroupBox.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main()
		{
			OutProc outProc = new OutProc();
			int result = (int)EnumResultCode.S_OK;
			try
			{
				outProc.CreateOpcClient();
				OpcClient opcClient = outProc.OpcClient;

				//	initialize the client instance
				if (!ResultCode.SUCCEEDED(opcClient.Initialize()))
				{
					opcClient = null;
					return;
				}   //	end if

				OpcForm opcForm = new OpcForm();

				//	initialize the AE client simulation
				result |= opcClient.InitializeAeObjects();

				opcClient.SetForm(opcForm);

				opcForm.SetOutProc(outProc);
				System.Windows.Forms.Application.Run(opcForm);

				opcClient.Terminate();
				opcClient = null;
			}
			catch (Exception exc)
			{
				MessageBox.Show(exc.ToString());
			}   //	end try...catch

		}   //	end Main

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

		public void ChangeItem(AeCondition condition, string state)
		{

			ListViewItem item = new ListViewItem(state);
			item.Tag = condition;
			conditionlistView.Items.Add(item);
			item.SubItems.Add(condition.SourcePath);
			item.SubItems.Add(condition.ConditionName);
			item.SubItems.Add(condition.Severity.ToString());
			item.SubItems.Add(condition.Message);
			item.SubItems.Add(condition.OccurenceTime.ToString("hh:mm:ss.fff"));
			item.SubItems.Add(condition.ActorId);
			item.SubItems.Add(condition.SubConditionName);

		}
		public void RemoveItems()
		{

			conditionlistView.Items.Clear();

		}

		private void AcknowledgeButtonOnClick(object sender, System.EventArgs e)
		{

			int[] results = null;
			m_session.AcknowledgeAeConditions("aUser", "AECondition", m_subscription.AeConditionList, out results, m_executionOptions);

		}

		private void EnableConditionsButtonOnClick(object sender, System.EventArgs e)
		{

			foreach (AeCondition condition in m_subscription.AeConditionList)
			{
				string source = condition.SourcePath;
				m_session.EnableAeConditionsBySource(new string[] { source }, true, m_executionOptions);
			}//end foreach			
		}

		private void DisableConditionsButtonOnClick(object sender, System.EventArgs e)
		{

			foreach (AeCondition condition in m_subscription.AeConditionList)
			{
				string source = condition.SourcePath;
				m_session.EnableAeConditionsBySource(new string[] { source }, false, m_executionOptions);

			}//end foreach	
		}

		private void GetConditionStateButtonOnClick(object sender, System.EventArgs e)
		{
			if (conditionlistView.SelectedItems.Count != 0)
			{
				AeCondition condition = ((AeCondition)conditionlistView.SelectedItems[0].Tag);
				AeConditionState conditionState = null;
				this.conditionStateRichTextBox.Text = null;
				int res = m_session.GetAeConditionState(condition.SourcePath, condition.ConditionName, null, out conditionState, m_executionOptions);
				if (ResultCode.SUCCEEDED(res) && (m_executionOptions.ExecutionType == EnumExecutionType.SYNCHRONOUS))
				{
					this.conditionStateRichTextBox.Text += " Source Path: " + condition.SourcePath + "\n";
					this.conditionStateRichTextBox.Text += " Condition Name: " + condition.ConditionName + "\n";
					this.conditionStateRichTextBox.Text += " State: " + GetState(conditionState.State) + "\n";
					this.conditionStateRichTextBox.Text += " Quality: " + conditionState.Quality + "\n";
					this.conditionStateRichTextBox.Text += " Active Time: " + conditionState.ConditionActiveTime + "\n";
					this.conditionStateRichTextBox.Text += " Inactive Time: " + conditionState.ConditionInactiveTime + "\n";
					this.conditionStateRichTextBox.Text += " AcknowledgeTime: " + conditionState.ConditionAckTime + "\n";
					this.conditionStateRichTextBox.Text += " AcknowledgerComment: " + conditionState.AcknowledgerComment + "\n";
					this.conditionStateRichTextBox.Text += " AcknowledgerID: " + conditionState.AcknowledgerId + "\n";
					this.conditionStateRichTextBox.Text += " Active subcondition time: " + conditionState.SubConditionActiveTime + "\n";
					this.conditionStateRichTextBox.Text += " Active subcondition name: " + conditionState.ActiveSubConditionName + "\n";
					this.conditionStateRichTextBox.Text += " Active subcondition definition: " + conditionState.ActiveSubConditionDefinition + "\n";
					this.conditionStateRichTextBox.Text += " Active subcondition description: " + conditionState.ActiveSubConditionDescription + "\n";
					this.conditionStateRichTextBox.Text += " Active subcondition severity: " + conditionState.ActiveSubConditionSeverity + "\n";
					this.conditionStateRichTextBox.Text += " Number of subconditions: " + conditionState.SubConditionsNames.Length + "\n";
					for (int i = 0; i < conditionState.SubConditionsNames.Length; i++)
					{
						this.conditionStateRichTextBox.Text += "	Subcondition name: " + conditionState.SubConditionsNames[i] + "\n";
						this.conditionStateRichTextBox.Text += "	Subcondition definition: " + conditionState.SubConditionsDefinitions[i] + "\n";
						this.conditionStateRichTextBox.Text += "	Subcondition description: " + conditionState.SubConditionsDescriptions[i] + "\n";
						this.conditionStateRichTextBox.Text += "	Subcondition severity: " + conditionState.SubConditionsSeverities[i] + "\n";
					}//end for
				}//end if				
			}
			else
			{
				MessageBox.Show(" Select a condition in the condition list ! ", "Select condition");
			} //ens if ... else
		}

		public delegate void ChangeConditionListView(AeCondition condition, string state);
		public delegate void RemoveItemsFromListView();

	}   //	end class
}   //	end namespace
