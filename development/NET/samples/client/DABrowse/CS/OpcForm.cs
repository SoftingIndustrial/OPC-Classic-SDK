using System;
using System.Windows.Forms;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Client;

namespace DABrowse
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
		private System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.TreeView itemsTreeView;
		private OutProc m_outProc = null;
		private OpcClient m_opcClient = null;
		private DaGetPropertiesOptions m_propertyGetOptions = new DaGetPropertiesOptions();
		private ExecutionOptions m_executionOptions = null;
		private MyDaSession m_session = null;
		public OpcForm(OutProc anOutProc)
		{
			InitializeComponent();

			try
			{

				m_outProc = anOutProc;

				m_opcClient = m_outProc.OpcClient;

				m_session = m_opcClient.GetSession();

				m_executionOptions = m_opcClient.GetExecutionOptions();

				//create the browse tree root element
				TreeNode treeRoot = new TreeNode(m_session.Url, 0, 0);
				DaAddressSpaceElement rootDaAddressSpaceElement = new DaAddressSpaceElement(EnumAddressSpaceElementType.BRANCH, String.Empty, string.Empty, string.Empty, 0, null);
				rootDaAddressSpaceElement.Session = m_session;
				treeRoot.Tag = rootDaAddressSpaceElement;
				itemsTreeView.Nodes.Add(treeRoot);

				DaAddressSpaceElement[] addressSpaceElements = null;

				#region Browse branches
				//---------------------
				DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;

				//get the root's children
				if (ResultCode.SUCCEEDED(m_session.Browse(
					((DaAddressSpaceElement)treeRoot.Tag).ItemId,
					((DaAddressSpaceElement)treeRoot.Tag).ItemPath,
					browseOptions,
					out addressSpaceElements,
					m_executionOptions)))
				{
					if (addressSpaceElements != null)
					{
						for (int i = 0; i < addressSpaceElements.Length; i++)
						{
							TreeNode node = new TreeNode(addressSpaceElements[i].Name, 1, 1);
							node.Tag = addressSpaceElements[i];
							treeRoot.Nodes.Add(node);
						}//end for
					}
				}//end if		

				//-
				#endregion

				addressSpaceElements = null;

				#region Browse Leaves
				//-------------------                    
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.LEAF;

				//get the root's children
				if (ResultCode.SUCCEEDED(m_session.Browse(
					((DaAddressSpaceElement)treeRoot.Tag).ItemId,
					((DaAddressSpaceElement)treeRoot.Tag).ItemPath,
					browseOptions,
					out addressSpaceElements,
					m_executionOptions)))
				{
					if (addressSpaceElements != null)
					{
						for (int i = 0; i < addressSpaceElements.Length; i++)
						{
							TreeNode node = new TreeNode(addressSpaceElements[i].Name, 2, 2);
							node.Tag = addressSpaceElements[i];
							treeRoot.Nodes.Add(node);
						}//end for
					}   //  end if
				}//end if	
				 //-
				#endregion

				#region GetDaProperties
				//---------------------

				DaProperty[] daProperties = null;
				m_propertyGetOptions.WhatPropertyData = EnumPropertyData.ALL;

				if (ResultCode.SUCCEEDED(rootDaAddressSpaceElement.GetDaProperties(
					m_propertyGetOptions,
					out daProperties,
					m_executionOptions)))
				{
					if (daProperties != null)
					{
						for (int i = 0; i < daProperties.Length; i++)
						{
							TreeNode node = new TreeNode(daProperties[i].Name, 3, 3);
							//add all properties except OPC-specific properties
							if (daProperties[i].Id >= 100)
							{
								treeRoot.Nodes.Add(node);
							}
						}//end for
					}
				}   //  end if

				//-
				#endregion
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
			this.itemsTreeView = new System.Windows.Forms.TreeView();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.SuspendLayout();
			// 
			// itemsTreeView
			// 
			this.itemsTreeView.ImageIndex = 3;
			this.itemsTreeView.ImageList = this.imageList1;
			this.itemsTreeView.Location = new System.Drawing.Point(0, 0);
			this.itemsTreeView.Name = "itemsTreeView";
			this.itemsTreeView.SelectedImageIndex = 3;
			this.itemsTreeView.Size = new System.Drawing.Size(576, 520);
			this.itemsTreeView.TabIndex = 0;
			this.itemsTreeView.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.TreeViewBeforeExpand);
			// 
			// imageList1
			// 
			this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// OpcForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(576, 518);
			this.Controls.Add(this.itemsTreeView);
			this.Name = "OpcForm";
			this.Text = "DABrowse";
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

				//	initialize the DA client simulation
				result |= opcClient.InitializeDaObjects();

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

		private void TreeViewBeforeExpand(object sender, System.Windows.Forms.TreeViewCancelEventArgs e)
		{

			try
			{
				itemsTreeView.Cursor = System.Windows.Forms.Cursors.WaitCursor;

				foreach (TreeNode node in e.Node.Nodes)
				{
					m_propertyGetOptions.WhatPropertyData = EnumPropertyData.ALL;
					DaAddressSpaceElement[] addressSpaceElements = null;

					node.Nodes.Clear();

					if (node.Tag is DaAddressSpaceElement)
					{
						DaAddressSpaceElement element = node.Tag as DaAddressSpaceElement;
						DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();

						#region Branches
						//-----------
						if (m_session.isConnected())
						{
							browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;

							if (ResultCode.SUCCEEDED(m_session.Browse(
								element.ItemId,
								element.ItemPath,
								browseOptions,
								out addressSpaceElements,
								m_executionOptions)))
							{
								if (addressSpaceElements != null)
								{
									for (int i = 0; i < addressSpaceElements.Length; i++)
									{
										TreeNode newNode = new TreeNode(addressSpaceElements[i].Name, 1, 1);
										newNode.Tag = addressSpaceElements[i];
										node.Nodes.Add(newNode);
									}//end for
								}
							}//end if
						}//end if
						 //-
						#endregion

						addressSpaceElements = null;

						#region Leaves
						//-----------
						if (m_session.isConnected())
						{
							browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.LEAF;

							if (ResultCode.SUCCEEDED(m_session.Browse(
								element.ItemId,
								element.ItemPath,
								browseOptions,
								out addressSpaceElements,
								m_executionOptions)))
							{
								if (addressSpaceElements != null)
								{
									for (int i = 0; i < addressSpaceElements.Length; i++)
									{
										TreeNode newNode = new TreeNode(addressSpaceElements[i].Name, 2, 2);
										newNode.Tag = addressSpaceElements[i];
										node.Nodes.Add(newNode);
									}//end for
								}
							}//end if
						}//end if
						 //-
						#endregion

						DaProperty[] daProperties = null;

						#region Properties
						//----------------
						if (m_session.isConnected())
						{
							if (ResultCode.SUCCEEDED(element.GetDaProperties(
								m_propertyGetOptions,
								out daProperties,
								m_executionOptions)))
							{
								if (daProperties != null)
								{
									for (int i = 0; i < daProperties.Length; i++)
									{
										//add all properties except OPC-specific properties
										if (daProperties[i].Id >= 100)
										{
											TreeNode newNode = new TreeNode(daProperties[i].Description, 3, 3);
											node.Nodes.Add(newNode);
										}
									}//end for
								}
							}   //  end if
						}//end if
						 //-
						#endregion

					}// end if 

					node.Collapse();
				}   //  end if
				itemsTreeView.Cursor = System.Windows.Forms.Cursors.Default;
			}
			catch (Exception exc)
			{
				MessageBox.Show(exc.ToString());
			}
		}   //	end treeview_beforeexpand

	}   //	end class
}   //	end namespace
