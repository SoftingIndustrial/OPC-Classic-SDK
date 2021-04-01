using System;
using System.Windows.Forms;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Client;

namespace AEBrowse
{
	/// <summary>
	/// Summary description for OpcForm.
	/// </summary>
	public class OpcForm : Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private Panel panel1;
		private ImageList imageList1;
		private TreeView addressSpaceTreeView;
		private System.ComponentModel.Container components;
		MyAeSession m_session = null;
		private ExecutionOptions m_executionOptions;
		public static int index = 0;
		private OutProc m_outProc = null;
		private OpcClient m_opcClient = null;

		public OpcForm(OutProc anOutProc)
		{
			InitializeComponent();

			try
			{

				m_executionOptions = new ExecutionOptions();

				m_outProc = anOutProc;

				m_opcClient = m_outProc.OpcClient;

				m_session = m_opcClient.getSession();

				TreeNode treeRoot = new TreeNode(m_session.Url + " - Area space", 0, 0);
				addressSpaceTreeView.Nodes.Add(treeRoot);
				treeRoot.Nodes.Add(new TreeNode(""));
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
			components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(OpcForm));
			panel1 = new Panel();
			addressSpaceTreeView = new TreeView();
			imageList1 = new ImageList(components);
			panel1.SuspendLayout();
			SuspendLayout();
			// 
			// panel1
			// 
			panel1.Controls.Add(addressSpaceTreeView);
			panel1.Dock = DockStyle.Fill;
			panel1.Location = new System.Drawing.Point(0, 0);
			panel1.Name = "panel1";
			panel1.Size = new System.Drawing.Size(712, 518);
			panel1.TabIndex = 0;
			// 
			// addressSpaceTreeView
			// 
			addressSpaceTreeView.Dock = DockStyle.Fill;
			addressSpaceTreeView.ImageList = imageList1;
			addressSpaceTreeView.Location = new System.Drawing.Point(0, 0);
			addressSpaceTreeView.Name = "addressSpaceTreeView";
			addressSpaceTreeView.Size = new System.Drawing.Size(712, 518);
			addressSpaceTreeView.TabIndex = 0;
			addressSpaceTreeView.BeforeExpand += new TreeViewCancelEventHandler(AddressSpaceTreeViewBeforeExpand);
			// 
			// imageList1
			// 
			imageList1.ImageSize = new System.Drawing.Size(16, 16);
			imageList1.ImageStream = ((ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// Form1
			// 
			AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			ClientSize = new System.Drawing.Size(712, 518);
			Controls.Add(panel1);
			Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			Name = "OpcForm";
			Text = "Browse Area Space";
			panel1.ResumeLayout(false);
			ResumeLayout(false);

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

		private void AddressSpaceTreeViewBeforeExpand(object sender, TreeViewCancelEventArgs e)
		{

			addressSpaceTreeView.Cursor = Cursors.WaitCursor;

			TreeNode rootNode = e.Node;
			AddressSpaceElementBrowseOptions browseOptions = new AddressSpaceElementBrowseOptions();
			browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;

			AddressSpaceElement[] addressSpaceElements = null;
			if (ResultCode.SUCCEEDED(m_session.Browse((AddressSpaceElement)rootNode.Tag, browseOptions, out addressSpaceElements, m_executionOptions)))
			{
				rootNode.Nodes.Clear();
				for (int i = 0; i < addressSpaceElements.Length; i++)
				{
					TreeNode node = new TreeNode(addressSpaceElements[i].Name, 1, 1);
					node.Tag = addressSpaceElements[i];
					rootNode.Nodes.Add(node);
					node.Nodes.Add(new TreeNode(""));
					string[] conditions = null;
					string sourcePath = addressSpaceElements[i].QualifiedName;
					if (ResultCode.SUCCEEDED(m_session.QueryAeSourceConditions(sourcePath, out conditions, m_executionOptions)))
					{

						for (int j = 0; j < conditions.Length; j++)
						{
							TreeNode condition = new TreeNode(conditions[j], 3, 3);
							condition.Tag = String.Empty;
							node.Nodes.Add(condition);

						}// end for									
					}// end if
				}// end for
			}// end if					
			AddressSpaceElementBrowseOptions browseOptions1 = new AddressSpaceElementBrowseOptions();
			browseOptions1.ElementTypeFilter = EnumAddressSpaceElementType.LEAF;
			if (ResultCode.SUCCEEDED(m_session.Browse((AddressSpaceElement)rootNode.Tag, browseOptions1, out addressSpaceElements, m_executionOptions)))
			{
				for (int i = 0; i < addressSpaceElements.Length; i++)
				{
					TreeNode node = new TreeNode(addressSpaceElements[i].Name, 2, 2);
					node.Tag = addressSpaceElements[i];
					rootNode.Nodes.Add(node);
					string[] conditions = null;
					string sourcePath = addressSpaceElements[i].QualifiedName;
					if (ResultCode.SUCCEEDED(m_session.QueryAeSourceConditions(sourcePath, out conditions, m_executionOptions)))
					{

						for (int j = 0; j < conditions.Length; j++)
						{
							TreeNode condition = new TreeNode(conditions[j], 3, 3);
							condition.Tag = String.Empty;
							node.Nodes.Add(condition);
						}// end for									
					}//end if
				}// end for			
			}//	end if 

			addressSpaceTreeView.Cursor = Cursors.Default;

		} // end AddressSpaceTreeViewBeforeExpand

	}   //	end class
}   //	end namespace
