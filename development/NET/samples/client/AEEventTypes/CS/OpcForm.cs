using System;
using System.Windows.Forms;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Client;

namespace AEEventTypes
{
	/// <summary>
	/// Summary description for OpcForm.
	/// </summary>
	public class OpcForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private OutProc m_outProc = null;
		private OpcClient m_opcClient = null;
		private MyAeSession m_session = null;
		private System.Windows.Forms.TreeView eventsTreeView;
		private System.Windows.Forms.ImageList imageList1;
		ExecutionOptions m_QC_executionOptions = new ExecutionOptions();
		ExecutionOptions m_QA_executionOptions = new ExecutionOptions();
		ExecutionOptions m_QCN_executionOptions = new ExecutionOptions();
		ExecutionOptions m_QSCN_executionOptions = new ExecutionOptions();
		ExecutionOptions m_AF_executionOptions = new ExecutionOptions();
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.RichTextBox filtersTextBox;


		public OpcForm(OutProc anOutProc)
		{
			InitializeComponent();

			try
			{

				m_outProc = anOutProc;

				m_opcClient = m_outProc.OpcClient;

				m_session = m_opcClient.m_aeSession;

				//create the events tree
				TreeNode root = new TreeNode(m_session.Url + " - Event Space ", 0, 0);
				eventsTreeView.Nodes.Add(root);

				//query categories for each event type
				AeCategory[] categories = null;
				m_session.QueryAeCategories(out categories, m_QC_executionOptions);

				for (int i = 0; i < categories.Length; i++)
				{
					TreeNode category = new TreeNode(categories[i].Description, 1, 1);
					root.Nodes.Add(category);

					//Query categories' attributes
					AeAttribute[] attributes = null;
					categories[i].QueryAeAttributes(out attributes, m_QA_executionOptions);

					for (int ii = 0; ii < attributes.Length; ii++)
					{
						TreeNode attribute = new TreeNode(attributes[ii].Description, 2, 2);
						category.Nodes.Add(attribute);
					}
					//Query ConditionNames
					string[] conditionNames = null;
					categories[i].QueryAeConditionNames(out conditionNames, m_QCN_executionOptions);

					for (int ii = 0; ii < conditionNames.Length; ii++)
					{
						TreeNode conditionName = new TreeNode(conditionNames[ii], 3, 3);
						category.Nodes.Add(conditionName);

						//Query SubConditionNames
						string[] subConditionNames = null;
						categories[i].QueryAeSubConditionNames(conditionNames[ii], out subConditionNames, m_QSCN_executionOptions);

						for (int iii = 0; iii < subConditionNames.Length; iii++)
						{
							Console.WriteLine(" The category " + categories[ii].Description + " has for the condition " + conditionNames[ii] + " the subcondition " + subConditionNames[iii]);
						}
					}
				}
				//Query available filters
				EnumFilterBy availableFilters;
				m_session.QueryAvailableAeFilters(out availableFilters, m_AF_executionOptions);

				if ((availableFilters & EnumFilterBy.AREA) == EnumFilterBy.AREA)
				{
					filtersTextBox.AppendText("Filter by Area \n");
				}
				if ((availableFilters & EnumFilterBy.CATEGORY) == EnumFilterBy.CATEGORY)
				{
					filtersTextBox.AppendText("Filter by Category \n");
				}
				if ((availableFilters & EnumFilterBy.EVENT) == EnumFilterBy.EVENT)
				{
					filtersTextBox.AppendText("Filter by Event \n");
				}
				if ((availableFilters & EnumFilterBy.SEVERITY) == EnumFilterBy.SEVERITY)
				{
					filtersTextBox.AppendText("Filter by Severity \n");
				}
				if ((availableFilters & EnumFilterBy.SOURCE) == EnumFilterBy.SOURCE)
				{
					filtersTextBox.AppendText("Filter by Source \n");
				}

			}
			catch (Exception exc)
			{
				MessageBox.Show(exc.ToString());
			}   //	end try...catch
		}   //	end constructor

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
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(OpcForm));
			this.eventsTreeView = new System.Windows.Forms.TreeView();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.label1 = new System.Windows.Forms.Label();
			this.filtersTextBox = new System.Windows.Forms.RichTextBox();
			this.SuspendLayout();
			// 
			// eventsTreeView
			// 
			this.eventsTreeView.ImageList = this.imageList1;
			this.eventsTreeView.Location = new System.Drawing.Point(0, 0);
			this.eventsTreeView.Name = "eventsTreeView";
			this.eventsTreeView.Size = new System.Drawing.Size(640, 528);
			this.eventsTreeView.TabIndex = 0;
			// 
			// imageList1
			// 
			this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label1.Location = new System.Drawing.Point(8, 536);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(152, 16);
			this.label1.TabIndex = 1;
			this.label1.Text = "Available Filters";
			// 
			// filtersTextBox
			// 
			this.filtersTextBox.AutoWordSelection = true;
			this.filtersTextBox.Location = new System.Drawing.Point(0, 568);
			this.filtersTextBox.Name = "filtersTextBox";
			this.filtersTextBox.Size = new System.Drawing.Size(640, 80);
			this.filtersTextBox.TabIndex = 2;
			this.filtersTextBox.Text = "";
			// 
			// OpcForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(640, 654);
			this.Controls.Add(this.filtersTextBox);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.eventsTreeView);
			this.Name = "OpcForm";
			this.Text = "AE Event Types";
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

				//	initialize the AE client simulation
				result |= opcClient.InitializeAeObjects();

				OpcForm opcForm = new OpcForm(outProc);

				System.Windows.Forms.Application.Run(opcForm);

				opcClient.Terminate();
				opcClient = null;
			}
			catch (Exception exc)
			{
				MessageBox.Show(exc.ToString());
			}   //	end try...catch

		}   //	end Main

	}   //	end class
}   //	end namespace
