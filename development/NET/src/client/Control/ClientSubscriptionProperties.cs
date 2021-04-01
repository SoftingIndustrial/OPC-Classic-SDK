using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Softing.OPCToolbox.Client.Control
{
	/// <summary>
	/// Collects data about a <see cref="ControlDaSubscription"/> object. It is used in the configuration phase, when a new subscription is added
	/// to the configuration or an existing one is changed.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="SubscriptionProperties"]/doc/*'
	/// />
	public class SubscriptionProperties : System.Windows.Forms.Form
	{
		#region Private Attributes
		//-------------------------

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox subscriptionNameTextBox;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox subscriptionUpdateRateTextBox;
		private System.Windows.Forms.CheckBox activeCheckBox;
		private System.Windows.Forms.Button okButton;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private ControlDaSession m_session;
		private ListView m_listView;
		private bool m_isAddingSubscription = false;
		private bool m_isEditingSubscription = false;
		private System.Windows.Forms.Button cancelButton;
		//the position in the m_listView where to edit the subscription;
		//it is assigned in the constructor
		private int m_position;
		private ExecutionOptions m_executionOptions = new ExecutionOptions();
		//-
		#endregion

		#region Constructors
		//--------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="SubscriptionProperties"/> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="SubscriptionProperties"]/constructor[@name="SubscriptionProperties"]/doc/*'
		/// />
		public SubscriptionProperties()
		{
			InitializeComponent();
#if NET20
		CheckForIllegalCrossThreadCalls = false;
#endif
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="SubscriptionProperties"/> class with the value indicated by a <see cref="ControlDaSession">Session</see> object.
		/// </summary>
		/// <param name="aSession">the current session</param>
		/// <param name="aListView">the listview in InfoCollector that needs to be modified</param>
		internal SubscriptionProperties(ControlDaSession aSession, ListView aListView)
		{
			m_session = aSession;
			m_listView = aListView;

			InitializeComponent();

			this.activeCheckBox.Checked = true;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="SubscriptionProperties"/> class with the value indicated by a <see cref="ControlDaSession">Session</see> object.
		/// </summary>
		/// <param name="aSession">the current session</param>
		/// <param name="aListView">the listview in InfoCollector that needs to be modified</param>
		/// <param name="position">the position on the listview the subscription must be added or edited</param>
		/// <param name="active">shows whether the current selected subscription is active or not</param>
		internal SubscriptionProperties(ControlDaSession aSession, ListView aListView, int position, bool active)
		{
			m_session = aSession;
			m_listView = aListView;
			m_position = position;

			InitializeComponent();

			if (active)
			{
				this.activeCheckBox.Checked = true;
			}
			else
			{
				this.activeCheckBox.Checked = false;
			}
			this.subscriptionNameTextBox.Text = aListView.Items[position].SubItems[0].Text;
			this.subscriptionUpdateRateTextBox.Text = aListView.Items[position].SubItems[2].Text;
		}

		//-
		#endregion

		#region Properties
		//-----------------

		internal bool IsAddingSubscription
		{
			get { return m_isAddingSubscription; }
			set { m_isAddingSubscription = value; }
		}

		internal bool IsEditingSubscription
		{
			get { return m_isEditingSubscription; }
			set { m_isEditingSubscription = value; }
		}

		//-
		#endregion

		#region Private Methods
		//---------------------

		private void AddItemsInListView(ControlDaSubscription controlSubscription)
		{
			if (IsAddingSubscription)
			{
				ListViewItem newItem = new ListViewItem(controlSubscription.StoredName);
				newItem.Tag = controlSubscription;
				m_listView.Items.Add(newItem);
				if (controlSubscription.IsActivated)
				{
					newItem.SubItems.Add("true");
				}
				else
				{
					newItem.SubItems.Add("false");
				}
				newItem.SubItems.Add(controlSubscription.StoredUpdateRate.ToString());
				newItem.SubItems.Add(controlSubscription.ItemList.Length.ToString());
			}
			if (IsEditingSubscription)
			{
				ListViewItem item = m_listView.Items[m_position];
				item.SubItems[0].Text = controlSubscription.Name;
				bool active = controlSubscription.IsActivated;
				if (active)
				{
					item.SubItems[1].Text = "true";
				}
				else
				{
					item.SubItems[1].Text = "false";
				}

				item.SubItems[2].Text = controlSubscription.StoredUpdateRate.ToString();
			}
		}

		//-
		#endregion

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="SubscriptionProperties"]/method[@name="Dispose"]/doc/*'
		/// />
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof (SubscriptionProperties));
			this.label1 = new System.Windows.Forms.Label();
			this.subscriptionNameTextBox = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.subscriptionUpdateRateTextBox = new System.Windows.Forms.TextBox();
			this.activeCheckBox = new System.Windows.Forms.CheckBox();
			this.okButton = new System.Windows.Forms.Button();
			this.cancelButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			//
			// label1
			//
			this.label1.Location = new System.Drawing.Point(16, 8);
			this.label1.Name = "label1";
			this.label1.TabIndex = 6;
			this.label1.Text = "Subscription Name";
			//
			// subscriptionNameTextBox
			//
			this.subscriptionNameTextBox.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.subscriptionNameTextBox.Location = new System.Drawing.Point(8, 32);
			this.subscriptionNameTextBox.Name = "subscriptionNameTextBox";
			this.subscriptionNameTextBox.Size = new System.Drawing.Size(304, 20);
			this.subscriptionNameTextBox.TabIndex = 1;
			this.subscriptionNameTextBox.Text = "";
			//
			// label2
			//
			this.label2.Location = new System.Drawing.Point(8, 56);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(160, 23);
			this.label2.TabIndex = 7;
			this.label2.Text = "Subscription Update Rate";
			//
			// subscriptionUpdateRateTextBox
			//
			this.subscriptionUpdateRateTextBox.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.subscriptionUpdateRateTextBox.Location = new System.Drawing.Point(8, 80);
			this.subscriptionUpdateRateTextBox.Name = "subscriptionUpdateRateTextBox";
			this.subscriptionUpdateRateTextBox.Size = new System.Drawing.Size(304, 20);
			this.subscriptionUpdateRateTextBox.TabIndex = 2;
			this.subscriptionUpdateRateTextBox.Text = "";
			//
			// activeCheckBox
			//
			this.activeCheckBox.Location = new System.Drawing.Point(8, 112);
			this.activeCheckBox.Name = "activeCheckBox";
			this.activeCheckBox.Size = new System.Drawing.Size(56, 24);
			this.activeCheckBox.TabIndex = 3;
			this.activeCheckBox.Text = "active";
			//
			// okButton
			//
			this.okButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.okButton.Location = new System.Drawing.Point(72, 144);
			this.okButton.Name = "okButton";
			this.okButton.Size = new System.Drawing.Size(70, 25);
			this.okButton.TabIndex = 4;
			this.okButton.Text = "OK";
			this.okButton.Click += new System.EventHandler(this.okButton_Click);
			//
			// cancelButton
			//
			this.cancelButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(192, 144);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.Size = new System.Drawing.Size(70, 25);
			this.cancelButton.TabIndex = 8;
			this.cancelButton.Text = "Cancel";
			this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
			//
			// SubscriptionProperties
			//
			this.AcceptButton = this.okButton;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.cancelButton;
			this.ClientSize = new System.Drawing.Size(320, 182);
			this.ControlBox = false;
			this.Controls.Add(this.cancelButton);
			this.Controls.Add(this.okButton);
			this.Controls.Add(this.activeCheckBox);
			this.Controls.Add(this.subscriptionUpdateRateTextBox);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.subscriptionNameTextBox);
			this.Controls.Add(this.label1);
			this.Icon = ((System.Drawing.Icon) (resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "SubscriptionProperties";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Subscription Properties";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.SubscriptionProperties_Closing);
			this.ResumeLayout(false);
		}

		#endregion

		#region Buttons
		//-------------

		private void okButton_Click(object sender, System.EventArgs e)
		{
			try
			{
				//when editing if I don't change yhe name of the current subscription it shouldn't be a problem
				ControlDaSubscription currentSelectedSubscription = (ControlDaSubscription) m_listView.Items[m_position].Tag;
				bool found = false;
				if (subscriptionNameTextBox.Text != string.Empty)
				{
					//a subscription with the same name with another one cannot exist
					foreach (ControlDaSubscription subscription in m_session.SubscriptionList)
					{
						if ((subscription.StoredName == subscriptionNameTextBox.Text) &&
						    (subscription.GetHashCode() != currentSelectedSubscription.GetHashCode()))
						{
							found = true;
						} //	end if
					} //	end foreach
					if (found)
					{
						MessageBox.Show(
							"The server already has a subscription named " + subscriptionNameTextBox.Text +
							"!Please introduce a different subscription name.",
							"Warning");
						return;
					} //	end if
				} //	end if
				else
				{
					if (IsAddingSubscription)
					{
						MessageBox.Show("Please introduce a subscription name.",
						                "Warning");
						return;
					}
				}
				if (!found)
				{
					if (IsAddingSubscription)
					{
						if (subscriptionUpdateRateTextBox.Text != string.Empty)
						{
							ControlDaSubscription controlSubscription = new ControlDaSubscription(
								Convert.ToUInt32(subscriptionUpdateRateTextBox.Text),
								m_session);
							if (controlSubscription.Valid)
							{
								controlSubscription.Name = subscriptionNameTextBox.Text;
								controlSubscription.StoredName = controlSubscription.Name;
								controlSubscription.StoredUpdateRate = controlSubscription.RequestedUpdateRate;

								controlSubscription.Connect(true, false, m_executionOptions);

								if (activeCheckBox.Checked)
								{
									controlSubscription.IsActivated = true;
								} //	end if
								else
								{
									controlSubscription.IsActivated = false;
								} //	end else

								if (m_listView.InvokeRequired)
								{
									AddItemsToSubscriptionListView aislv = new AddItemsToSubscriptionListView(AddItemsInListView);
									m_listView.BeginInvoke(aislv, new object[] {controlSubscription});
								} //	end if
								else
								{
									ListViewItem newItem = new ListViewItem(controlSubscription.StoredName);
									newItem.Tag = controlSubscription;
									m_listView.Items.Add(newItem);
									if (controlSubscription.IsActivated)
									{
										newItem.SubItems.Add("true");
									}
									else
									{
										newItem.SubItems.Add("false");
									}

									newItem.SubItems.Add(controlSubscription.StoredUpdateRate.ToString());
									newItem.SubItems.Add(controlSubscription.ItemList.Length.ToString());
								} //	end else
							} //	end if
							//reset the indicator
							IsAddingSubscription = false;

							this.Close();
						} //	end if
						else
						{
							MessageBox.Show("Please introduce an update rate.",
							                "Warning");
						} //	end else
					} //	end if adding a subscription

					if (IsEditingSubscription)
					{
						//get the subscription currently selected
						ControlDaSubscription currentSubscription = (ControlDaSubscription) m_listView.Items[m_position].Tag;

						//modify the subscription's data
						currentSubscription.Name = subscriptionNameTextBox.Text;
						if (subscriptionUpdateRateTextBox.Text != string.Empty)
						{
							currentSubscription.RequestedUpdateRate = Convert.ToUInt32(subscriptionUpdateRateTextBox.Text);
						}
						currentSubscription.StoredName = currentSubscription.Name;
						currentSubscription.StoredUpdateRate = currentSubscription.RequestedUpdateRate;

						currentSubscription.Connect(false, false, m_executionOptions);

						if (activeCheckBox.Checked)
						{
							currentSubscription.IsActivated = true;
						}
						else
						{
							currentSubscription.IsActivated = false;
						}

						if (m_listView.InvokeRequired)
						{
							AddItemsToSubscriptionListView aislv = new AddItemsToSubscriptionListView(AddItemsInListView);
							m_listView.BeginInvoke(aislv, new object[] {currentSubscription});
						}
						else
						{
							ListViewItem item = m_listView.Items[m_position];
							item.SubItems[0].Text = currentSubscription.Name;
							bool active = currentSubscription.IsActivated;
							item.SubItems[1].Text = active.ToString().ToLower();
							item.SubItems[2].Text = currentSubscription.StoredUpdateRate.ToString();
						}
						//set the new subscription in the list
						m_listView.Items[m_position].Tag = currentSubscription;

						//reset the indicator
						IsEditingSubscription = false;

						this.Close();
					} //	end if editing a subscription
				} //	end if
			}

			catch (System.FormatException exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"SubscriptionProperties.ok_button.Click",
					exc.ToString());
				MessageBox.Show(" The update rate must have numeric format!");
				this.subscriptionUpdateRateTextBox.Text = String.Empty;
			}
			catch (Exception ex)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"SubscriptionProperties.ok_button.Click",
					ex.ToString());
			}
		}

		private void SubscriptionProperties_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			subscriptionNameTextBox.Text = string.Empty;
			subscriptionUpdateRateTextBox.Text = string.Empty;
		}

		private void cancelButton_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		//-
		#endregion
	}

	internal delegate void AddItemsToSubscriptionListView(ControlDaSubscription subscription);
}