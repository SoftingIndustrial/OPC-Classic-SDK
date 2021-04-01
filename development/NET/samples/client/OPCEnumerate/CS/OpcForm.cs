using System;
using System.Collections;
using System.Windows.Forms;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Client;
using System.Runtime.InteropServices;

namespace OPCEnumerate
{
	/// <summary>
	/// Summary description for OpcForm.
	/// </summary>
	public class OpcForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required variable.
		/// </summary>

		#region Private Attributes

		private System.ComponentModel.Container components = null;
		private OutProc m_outProc = null;
		private OpcClient m_opcClient = null;
		private System.Windows.Forms.TreeView tvServers;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox txtUrl;


		private const uint ERROR_SUCCESS = 0;
		private const uint ERROR_MORE_DATA = 234;
		private enum SV_101_TYPES : uint
		{
			SV_TYPE_WORKSTATION = 0x00000001,
			SV_TYPE_SERVER = 0x00000002,
			SV_TYPE_SQLSERVER = 0x00000004,
			SV_TYPE_DOMAIN_CTRL = 0x00000008,
			SV_TYPE_DOMAIN_BAKCTRL = 0x00000010,
			SV_TYPE_TIME_SOURCE = 0x00000020,
			SV_TYPE_AFP = 0x00000040,
			SV_TYPE_NOVELL = 0x00000080,
			SV_TYPE_DOMAIN_MEMBER = 0x00000100,
			SV_TYPE_PRINTQ_SERVER = 0x00000200,
			SV_TYPE_DIALIN_SERVER = 0x00000400,
			SV_TYPE_XENIX_SERVER = 0x00000800,
			SV_TYPE_SERVER_UNIX = 0x00000800,
			SV_TYPE_NT = 0x00001000,
			SV_TYPE_WFW = 0x00002000,
			SV_TYPE_SERVER_MFPN = 0x00004000,
			SV_TYPE_SERVER_NT = 0x00008000,
			SV_TYPE_POTENTIAL_BROWSER = 0x00010000,
			SV_TYPE_BACKUP_BROWSER = 0x00020000,
			SV_TYPE_MASTER_BROWSER = 0x00040000,
			SV_TYPE_DOMAIN_MASTER = 0x00080000,
			SV_TYPE_SERVER_OSF = 0x00100000,
			SV_TYPE_SERVER_VMS = 0x00200000,
			SV_TYPE_WINDOWS = 0x00400000,
			SV_TYPE_DFS = 0x00800000,
			SV_TYPE_CLUSTER_NT = 0x01000000,
			SV_TYPE_TERMINALSERVER = 0x02000000,
			SV_TYPE_CLUSTER_VS_NT = 0x04000000,
			SV_TYPE_DCE = 0x10000000,
			SV_TYPE_ALTERNATE_XPORT = 0x20000000,
			SV_TYPE_LOCAL_LIST_ONLY = 0x40000000,
			SV_TYPE_DOMAIN_ENUM = 0x80000000,
			SV_TYPE_ALL = 0xFFFFFFFF
		};
		[StructLayout(LayoutKind.Sequential)]
		private struct SERVER_INFO_101
		{
			[MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)]
			public UInt32 sv101_platform_id;
			[MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPWStr)]
			public string sv101_name;
			[MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)] public UInt32 sv101_version_major;
			[MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)] public UInt32 sv101_version_minor;
			[MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)] public UInt32 sv101_type;
			[MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPWStr)] public string sv101_comment;
		};
		private enum PLATFORM_ID
		{
			PLATFORM_ID_DOS = 300,
			PLATFORM_ID_OS2 = 400,
			PLATFORM_ID_NT = 500,
			PLATFORM_ID_OSF = 600,
			PLATFORM_ID_VMS = 700
		}
		private ExecutionOptions m_executionOptions = new ExecutionOptions();
		private System.Windows.Forms.ImageList imageList1;

		#endregion

		#region Private Methods

		private void CreateMainTree()
		{

			TreeNode local = new TreeNode("Local", 1, 1);
			this.tvServers.Nodes.Add(local);

			TreeNode da1 = new TreeNode("Data Access V1", 4, 4);
			local.Nodes.Add(da1);
			da1.Nodes.Add(new TreeNode("", 1, 1));

			TreeNode da2 = new TreeNode("Data Access V2", 4, 4);
			local.Nodes.Add(da2);
			da2.Nodes.Add(new TreeNode("", 1, 1));

			TreeNode da3 = new TreeNode("Data Access V3", 4, 4);
			local.Nodes.Add(da3);
			da3.Nodes.Add(new TreeNode("", 1, 1));

			TreeNode ae = new TreeNode("Alarms and Events", 4, 4);
			local.Nodes.Add(ae);
			ae.Nodes.Add(new TreeNode("", 1, 1));

			TreeNode remote = new TreeNode("Remote", 6, 6);
			this.tvServers.Nodes.Add(remote);

			ArrayList domains = OpcForm.GetServerList(SV_101_TYPES.SV_TYPE_DOMAIN_ENUM, null);

			for (int i = 0; i < domains.Count; i++)
			{
				TreeNode domain = new TreeNode(((SERVER_INFO_101)domains[i]).sv101_name.ToString(), 3, 3);
				remote.Nodes.Add(domain);
				domain.Nodes.Add(new TreeNode("", 1, 1));
			}


		}

		private void CreateBrowseTree(string ipAddress, EnumOPCSpecification specification, TreeNode parent)
		{
			try
			{
				parent.Nodes.Clear();

				Softing.OPCToolbox.Client.ServerBrowser serverBrowser = new ServerBrowser(ipAddress);
				ServerBrowserData[] serverData = null;

				if (ResultCode.SUCCEEDED(serverBrowser.Browse(
					specification,
					EnumServerBrowserData.ALL,
					out serverData,
					m_executionOptions)))
				{

					for (int i = 0; i < serverData.Length; i++)
					{
						TreeNode aNode = null;

						if (serverData[i].Description != String.Empty)
						{
							aNode = new TreeNode(serverData[i].Description, 7, 7);
							aNode.Tag = serverData[i].Url;
							parent.Nodes.Add(aNode);
						}
						if (serverData[i].ClsId != String.Empty)
						{
							TreeNode clsID = new TreeNode(serverData[i].ClsId, 5, 5);
							clsID.Tag = serverData[i].Url;
							aNode.Nodes.Add(clsID);
						}

						if (serverData[i].ProgId != String.Empty)
						{
							TreeNode progID = new TreeNode(serverData[i].ProgId, 5, 5);
							progID.Tag = serverData[i].Url;
							aNode.Nodes.Add(progID);
						}

						if (serverData[i].ProgIdVersionIndependent != String.Empty)
						{
							TreeNode vprogID = new TreeNode(serverData[i].ProgIdVersionIndependent, 5, 5);
							vprogID.Tag = serverData[i].Url;
							aNode.Nodes.Add(vprogID);
						}

					}//for													
				}//if
				else
				{
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
		}

		private static ArrayList GetServerList(OpcForm.SV_101_TYPES ServerType, string domain)
		{
			int entriesread = 0, totalentries = 0;
			ArrayList alServers = new ArrayList();

			do
			{
				// Buffer to store the available servers
				// Filled by the NetServerEnum function
				IntPtr buf = new IntPtr();

				SERVER_INFO_101 server;
				int ret = NetServerEnum(null, 101, out buf, -1,
					ref entriesread, ref totalentries,
					ServerType, domain, 0);

				// if the function returned any data, fill the tree view
				if (ret == ERROR_SUCCESS ||
					ret == ERROR_MORE_DATA ||
					entriesread > 0)
				{
					Int32 ptr = buf.ToInt32();

					for (int i = 0; i < entriesread; i++)
					{
						// cast pointer to a SERVER_INFO_101 structure
						server = (SERVER_INFO_101)Marshal.PtrToStructure(new IntPtr(ptr), typeof(SERVER_INFO_101));

						ptr += Marshal.SizeOf(server);

						// add the machine name and comment to the arrayList. 
						//You could return the entire structure here if desired
						alServers.Add(server);
					}
				}

				// free the buffer 
				NetApiBufferFree(buf);

			}
			while
				(
				entriesread < totalentries &&
				entriesread != 0
				);

			return alServers;
		}

		#endregion

		#region //Extern methods
		[DllImport("netapi32.dll", EntryPoint = "NetServerEnum")]
		private static extern int NetServerEnum([MarshalAs(UnmanagedType.LPWStr)]string servername,
			int level,
			out IntPtr bufptr,
			int prefmaxlen,
			ref int entriesread,
			ref int totalentries,
			SV_101_TYPES servertype,
			[MarshalAs(UnmanagedType.LPWStr)]string domain,
			int resume_handle);

		[DllImport("netapi32.dll", EntryPoint = "NetApiBufferFree")]
		private static extern int
			NetApiBufferFree(IntPtr buffer);

		#endregion


		public OpcForm(OutProc anOutProc)
		{
			InitializeComponent();

			try
			{

				m_outProc = anOutProc;

				m_opcClient = m_outProc.OpcClient;
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
			this.tvServers = new System.Windows.Forms.TreeView();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.label1 = new System.Windows.Forms.Label();
			this.txtUrl = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// tvServers
			// 
			this.tvServers.ImageList = this.imageList1;
			this.tvServers.Location = new System.Drawing.Point(24, 16);
			this.tvServers.Name = "tvServers";
			this.tvServers.Size = new System.Drawing.Size(824, 424);
			this.tvServers.TabIndex = 0;
			this.tvServers.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvServers_AfterSelect);
			this.tvServers.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.tvServers_BeforeExpand);
			// 
			// imageList1
			// 
			this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(32, 456);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(32, 23);
			this.label1.TabIndex = 1;
			this.label1.Text = "URL";
			// 
			// txtUrl
			// 
			this.txtUrl.Location = new System.Drawing.Point(72, 456);
			this.txtUrl.Name = "txtUrl";
			this.txtUrl.Size = new System.Drawing.Size(776, 20);
			this.txtUrl.TabIndex = 2;
			this.txtUrl.Text = "";
			// 
			// OpcForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(872, 510);
			this.Controls.Add(this.txtUrl);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.tvServers);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "OpcForm";
			this.Text = "OPCEnumerate";
			this.Load += new System.EventHandler(this.OpcForm_Load);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main()
		{
			OutProc outProc = new OutProc();

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

		private void OpcForm_Load(object sender, System.EventArgs e)
		{

			CreateMainTree();

		}

		private void tvServers_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			if (e.Node.Tag != null)
				this.txtUrl.Text = e.Node.Tag.ToString().TrimEnd();
		}

		private void tvServers_BeforeExpand(object sender, System.Windows.Forms.TreeViewCancelEventArgs e)
		{

			tvServers.Cursor = System.Windows.Forms.Cursors.WaitCursor;
			string ipAddress = null;

			if (e.Node.Text.Equals("Data Access V1"))
			{
				if (e.Node.Parent.Text.Equals("Local"))
				{
					ipAddress = "localhost";
					CreateBrowseTree(ipAddress, EnumOPCSpecification.DA10, e.Node);
				}
			}
			if (e.Node.Text.Equals("Data Access V2"))
			{
				if (e.Node.Parent.Text.Equals("Local"))
				{
					ipAddress = "localhost";
					CreateBrowseTree(ipAddress, EnumOPCSpecification.DA20, e.Node);
				}
			}
			if (e.Node.Text.Equals("Data Access V3"))
			{
				if (e.Node.Parent.Text.Equals("Local"))
				{
					ipAddress = "localhost";
					CreateBrowseTree(ipAddress, EnumOPCSpecification.DA30, e.Node);
				}
			}
			if (e.Node.Text.Equals("Alarms and Events"))
			{
				if (e.Node.Parent.Text.Equals("Local"))
				{
					ipAddress = "localhost";
					CreateBrowseTree(ipAddress, EnumOPCSpecification.AE10, e.Node);
				}
			}

			if (e.Node.Parent != null)
			{
				if (e.Node.Parent.Text.Equals("Remote"))
				{
					if (e.Node.Nodes[0].ImageIndex == 1)
					{
						e.Node.Nodes.Clear();
						ArrayList hosts = OpcForm.GetServerList(SV_101_TYPES.SV_TYPE_WORKSTATION, e.Node.Text.ToLower());
						for (int j = 0; j < hosts.Count; j++)
						{
							TreeNode host = new TreeNode(((SERVER_INFO_101)hosts[j]).sv101_name.ToLower(), 0, 0);
							e.Node.Nodes.Add(host);
							host.Nodes.Add(new TreeNode("Data Access V1", 4, 4));
							host.Nodes[0].Nodes.Add(new TreeNode("", 0, 0));
							host.Nodes.Add(new TreeNode("Data Access V2", 4, 4));
							host.Nodes[1].Nodes.Add(new TreeNode("", 0, 0));
							host.Nodes.Add(new TreeNode("Data Access V3", 4, 4));
							host.Nodes[2].Nodes.Add(new TreeNode("", 0, 0));
							host.Nodes.Add(new TreeNode("Alarms and Events", 4, 4));
							host.Nodes[3].Nodes.Add(new TreeNode("", 0, 0));
						}
					}
				}
				if (e.Node.Parent.Parent != null)
				{
					if (e.Node.Parent.Parent.Parent != null)
					{
						if (e.Node.Parent.Parent.Parent.Text.Trim().Equals("Remote"))
						{
							ipAddress = e.Node.Parent.Text.ToLower();
							if (e.Node.Text.Equals("Data Access V1"))
							{
								CreateBrowseTree(ipAddress, EnumOPCSpecification.DA10, e.Node);
							}
							if (e.Node.Text.Equals("Data Access V2"))
							{
								CreateBrowseTree(ipAddress, EnumOPCSpecification.DA20, e.Node);
							}
							if (e.Node.Text.Equals("Data Access V3"))
							{
								CreateBrowseTree(ipAddress, EnumOPCSpecification.DA30, e.Node);
							}
							if (e.Node.Text.Equals("Alarms and Events"))
							{
								CreateBrowseTree(ipAddress, EnumOPCSpecification.AE10, e.Node);
							}
						}
					}
				}
			}
			tvServers.Cursor = System.Windows.Forms.Cursors.Default;

		}

	}   //	end class
}   //	end namespace
