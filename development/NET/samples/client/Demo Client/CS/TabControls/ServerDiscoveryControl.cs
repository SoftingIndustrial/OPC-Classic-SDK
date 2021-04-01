using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using Softing.OPCToolbox.Client;
using System.Runtime.InteropServices;
using Softing.OPCToolbox;

namespace DemoClient.TabControls
{
    /// <summary>
    /// Handler called when a browse data has been added
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    /// <param name="val"></param>
    public delegate void ServerBrowseAddHandler(object sender, string strUrl, EnumOPCSpecification opcSpecification);

    public partial class ServerDiscoveryControl : UserControl
    {
        #region Enumerations

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

        public enum PLATFORM_ID
        {
            PLATFORM_ID_DOS = 300,
            PLATFORM_ID_OS2 = 400,
            PLATFORM_ID_NT = 500,
            PLATFORM_ID_OSF = 600,
            PLATFORM_ID_VMS = 700
        }

        private enum OPCSpecification
        {
            DataAccess = 1,
            AlarmEvent
        }

        #endregion

        #region Structures

        [StructLayout(LayoutKind.Sequential)]
        public struct SERVER_INFO_101
        {
            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)]
            public UInt32 sv101_platform_id;
            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPWStr)]
            public string sv101_name;
            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)]
            public UInt32 sv101_version_major;
            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)]
            public UInt32 sv101_version_minor;
            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)]
            public UInt32 sv101_type;
            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPWStr)]
            public string sv101_comment;
        };

        #endregion        

        #region Constants

        private const uint ERROR_SUCCESS = 0;
        private const uint ERROR_MORE_DATA = 234;

        //tree node constant names
        private const string LOCAL = "Local";
        private const string REMOTE = "Remote";
        private const string DATA_ACCESS = "Data Access";
        private const string ALARM_AND_EVENTS = "Alarms and Events";

        private const string LOCALHOST_IPADDR = "localhost";

        private const string OPC_SPECIFICATION_AE = "opcae";
        private const string OPC_SPECIFICATION_DA = "opcda";
        private const string OPC_SPECIFICATION_XMLDA = "http://";

        //imagelist network index 
        private const int LOCALPC_INDEX = 0;
        private const int REMOTE_INDEX = 1;
        private const int REMOTESRV_INDEX = 2;
        private const int REMOTEPC_INDEX = 3;
        private const int DATA_INDEX = 4;
        private const int OPCSRV_INDEX = 5;
        private const int PROPERTIES_INDEX = 6;


        #endregion

        #region Win32 Methods

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

        #region Private Members

        private bool canExpand = true;
        private bool canCollapse = true;

        #endregion

        #region Delegates

        public event ServerBrowseAddHandler ServerBrowseAdd;
        public event EnableButtonsHandler EnableButtons;

        #endregion

        #region Private and Protected Methods

        #region Servers list

        /// <summary>
        /// Returns the Server List
        /// </summary>
        /// <param name="ServerType">server type</param>
        /// <param name="domain">domain</param>
        /// <returns></returns>
        private static ArrayList GetServerList(SV_101_TYPES ServerType, string domain)
        {
            int entriesRead = 0, totalEntries = 0;
            ArrayList allServers = new ArrayList();

            do
            {
                // Buffer to store the available servers
                // Filled by the NetServerEnum function
                IntPtr buf = new IntPtr();

                SERVER_INFO_101 server;
                int ret = NetServerEnum(null, 101, out buf, -1,
                    ref entriesRead, ref totalEntries,
                    ServerType, domain, 0);

                // if the function returned any data, fill the tree view
                if (ret == ERROR_SUCCESS ||
                    ret == ERROR_MORE_DATA ||
                    entriesRead > 0)
                {
                    Int32 ptr = buf.ToInt32();

                    for (int i = 0; i < entriesRead; i++)
                    {
                        // cast pointer to a SERVER_INFO_101 structure
                        server = (SERVER_INFO_101)Marshal.PtrToStructure(new IntPtr(ptr), typeof(SERVER_INFO_101));

                        ptr += Marshal.SizeOf(server);

                        // add the machine name and comment to the arrayList. 
                        //You could return the entire structure here if desired
                        allServers.Add(server);
                    }
                }

                // free the buffer 
                NetApiBufferFree(buf);

            }
            while
                (
                entriesRead < totalEntries &&
                entriesRead != 0
                );

            return allServers;
        }

        #endregion              

        #region Server BrowseData
       
        /// <summary>
        /// Expand node functionality
        /// </summary>
        /// <param name="currentNode"></param>
        private void ExpandTreeNode(TreeNode currentNode)
        {
            string ipAddress = null;

            if (currentNode.Text.Equals(REMOTE))
            {
                //add the domains tree nodes
                currentNode.Nodes.Clear();
                ArrayList domains = GetServerList(SV_101_TYPES.SV_TYPE_DOMAIN_ENUM, null);
                for (int i = 0; i < domains.Count; i++)
                {
                    string domain = "";
                    if (!string.IsNullOrEmpty(((SERVER_INFO_101)domains[i]).sv101_name))
                    {
                        domain = ((SERVER_INFO_101)domains[i]).sv101_name.Substring(0, 1).ToUpper() + ((SERVER_INFO_101)domains[i]).sv101_name.Substring(1, ((SERVER_INFO_101)domains[i]).sv101_name.Length - 1).ToLower();
                    }
                    TreeNode domainNode = new TreeNode(domain, REMOTESRV_INDEX, REMOTESRV_INDEX);
                    currentNode.Nodes.Add(domainNode);
                    domainNode.Nodes.Add(new TreeNode("", REMOTEPC_INDEX, REMOTEPC_INDEX));
                }
                return;
            }

            if (currentNode.Parent != null)
            {
                //add data nodes for localhost
                if (currentNode.Parent.Text.Equals(LOCAL))
                {
                    ipAddress = LOCALHOST_IPADDR;
                    if (currentNode.Text.Equals(DATA_ACCESS))
                    {

                        CreateTreeBrowse(ipAddress, OPCSpecification.DataAccess, currentNode);
                        return;
                    }
                    if (currentNode.Text.Equals(ALARM_AND_EVENTS))
                    {
                        CreateTreeBrowse(ipAddress, OPCSpecification.AlarmEvent, currentNode);
                        return;
                    }
                }

                //add host for remote node
                if (currentNode.Parent.Text.Equals(REMOTE))
                {
                    //get the hosts
                    if (currentNode.Nodes[0].ImageIndex == REMOTEPC_INDEX)
                    {
                        currentNode.Nodes.Clear();
                        ArrayList hosts = GetServerList(SV_101_TYPES.SV_TYPE_WORKSTATION, currentNode.Text.ToLower());
                        if (hosts == null)
                        {
                            return;
                        }
                        for (int j = 0; j < hosts.Count; j++)
                        {
                            string host = "";
                            if (!string.IsNullOrEmpty(((SERVER_INFO_101)hosts[j]).sv101_name))
                            {
                                host = ((SERVER_INFO_101)hosts[j]).sv101_name.Substring(0, 1).ToUpper() + ((SERVER_INFO_101)hosts[j]).sv101_name.Substring(1, ((SERVER_INFO_101)hosts[j]).sv101_name.Length - 1).ToLower();
                            }
                            TreeNode hostNode = new TreeNode(host, REMOTEPC_INDEX, REMOTEPC_INDEX);
                            currentNode.Nodes.Add(hostNode);

                            hostNode.Nodes.Add(new TreeNode(DATA_ACCESS, DATA_INDEX, DATA_INDEX));
                            hostNode.Nodes[0].Nodes.Add(new TreeNode("", REMOTEPC_INDEX, REMOTEPC_INDEX));

                            hostNode.Nodes.Add(new TreeNode(ALARM_AND_EVENTS, DATA_INDEX, DATA_INDEX));
                            hostNode.Nodes[1].Nodes.Add(new TreeNode("", REMOTEPC_INDEX, REMOTEPC_INDEX));
                        }
                        return;
                    }
                }
                //add data nodes for remote PC
                if (currentNode.Parent.Parent != null)
                {
                    if (currentNode.Parent.Parent.Parent != null &&
                        currentNode.Parent.Parent.Parent.Text.Trim().Equals(REMOTE))
                    {
                        ipAddress = currentNode.Parent.Text.ToLower();
                        if (currentNode.Text.Equals(DATA_ACCESS))
                        {
                            CreateTreeBrowse(ipAddress, OPCSpecification.DataAccess, currentNode);
                            return;
                        }
                        if (currentNode.Text.Equals(ALARM_AND_EVENTS))
                        {
                            CreateTreeBrowse(ipAddress, OPCSpecification.AlarmEvent, currentNode);
                            return;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Create tre browse
        /// </summary>
        /// <param name="ipAddress"></param>
        /// <param name="specification"></param>
        /// <param name="currentNode"></param>
        private void CreateTreeBrowse(string ipAddress, OPCSpecification specification, TreeNode currentNode)
        {
            currentNode.Nodes.Clear();

            if (specification == OPCSpecification.AlarmEvent)
            {
                //add alarm and event servers
                ServerBrowserData[] serverData = GetServerBrowseData(ipAddress, Softing.OPCToolbox.EnumOPCSpecification.AE10);                
                if (serverData == null) return;
                List<ServerBrowserData> orderData = new List<ServerBrowserData>();
                orderData.AddRange(serverData);
                orderData.Sort(new ServerBrowseDataComparer());
                AddBrowserData(orderData.ToArray(), currentNode);
            }
            else
            {
                //combine the 3 data access
                Dictionary<string, ServerBrowserData> serverData = new Dictionary<string, ServerBrowserData>();
                List<ServerBrowserData> orderData = new List<ServerBrowserData>();

                ServerBrowserData[] serverDataDA10 = GetServerBrowseData(ipAddress, Softing.OPCToolbox.EnumOPCSpecification.DA10);
                if (serverDataDA10 == null) return;

                ServerBrowserData[] serverDataDA20 = GetServerBrowseData(ipAddress, Softing.OPCToolbox.EnumOPCSpecification.DA20);
                if (serverDataDA20 == null) return;


                ServerBrowserData[] serverDataDA30 = GetServerBrowseData(ipAddress, Softing.OPCToolbox.EnumOPCSpecification.DA30);
                if (serverDataDA30 == null) return;

                //build server data list
                for (int i = 0; i < serverDataDA10.Length; i++)
                {
                    if (!serverData.ContainsKey(serverDataDA10[i].Url))
                    {
                        serverData.Add(serverDataDA10[i].Url, serverDataDA10[i]);
                    }

                }
                for (int i = 0; i < serverDataDA20.Length; i++)
                {
                    if (!serverData.ContainsKey(serverDataDA20[i].Url))
                    {
                        serverData.Add(serverDataDA20[i].Url, serverDataDA20[i]);
                    }
                }
                for (int i = 0; i < serverDataDA30.Length; i++)
                {
                    if (!serverData.ContainsKey(serverDataDA30[i].Url))
                    {
                        serverData.Add(serverDataDA30[i].Url, serverDataDA30[i]);
                    }
                }
                orderData.AddRange(serverData.Values);
                orderData.Sort(new ServerBrowseDataComparer());
                AddBrowserData(orderData.ToArray(), currentNode);
            }
        }

        /// <summary>
        /// Get the Server Browse Data
        /// </summary>
        /// <param name="ipAddress"></param>
        /// <param name="specification"></param>
        /// <returns></returns>
        private static ServerBrowserData[] GetServerBrowseData(string ipAddress, Softing.OPCToolbox.EnumOPCSpecification specification)
        {
            ServerBrowserData[] serverData = null;
            if (string.IsNullOrEmpty(ipAddress)) return null;

            Softing.OPCToolbox.Client.ServerBrowser serverBrowser = new ServerBrowser(ipAddress);
            ExecutionOptions executionOptions = new ExecutionOptions();
            try
            {
                if (ResultCode.SUCCEEDED(serverBrowser.Browse(
                                                                  specification,
                                                                  EnumServerBrowserData.ALL,
                                                                  out serverData,
                                                                  executionOptions)))
                {
                    return serverData;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, DemoClient.Properties.Resources.DEMO_CLIENT, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return null;
        }

        /// <summary>
        /// Add alarms and events data
        /// </summary>
        /// <param name="serverData"></param>
        /// <param name="currentNode"></param>
        private void AddBrowserData(ServerBrowserData[] serverData, TreeNode currentNode)
        {
            for (int i = 0; i < serverData.Length; i++)
            {
                if (serverData[i] == null) continue;
                AddChildNodes(serverData[i], currentNode);
            }
        }

        /// <summary>
        /// Add childs nodes
        /// </summary>
        /// <param name="serverData"></param>
        /// <param name="currentNode"></param>
        private void AddChildNodes(ServerBrowserData serverData, TreeNode currentNode)
        {
            //add server treeNode
            TreeNode serverNode = null;
            if (!string.IsNullOrEmpty(serverData.Description))
            {
                serverNode = new TreeNode(serverData.Description, OPCSRV_INDEX, OPCSRV_INDEX);
                serverNode.Tag = serverData;
                currentNode.Nodes.Add(serverNode);
            }

            //add server treeNode properties
            if (!string.IsNullOrEmpty(serverData.ClsId))
            {
                TreeNode clsIDNode = new TreeNode(serverData.ClsId, PROPERTIES_INDEX, PROPERTIES_INDEX);
                clsIDNode.Tag = serverData;
                serverNode.Nodes.Add(clsIDNode);
            }
            if (!string.IsNullOrEmpty(serverData.ProgId))
            {
                TreeNode progIDNode = new TreeNode(serverData.ProgId, PROPERTIES_INDEX, PROPERTIES_INDEX);
                progIDNode.Tag = serverData;
                serverNode.Nodes.Add(progIDNode);
            }
            if (!string.IsNullOrEmpty(serverData.ProgIdVersionIndependent))
            {
                TreeNode vprogIDNode = new TreeNode(serverData.ProgIdVersionIndependent, PROPERTIES_INDEX, PROPERTIES_INDEX);
                vprogIDNode.Tag = serverData;
                serverNode.Nodes.Add(vprogIDNode);
            }
            
        }

        #endregion

        #region Add Session

        /// <summary>
        /// Add Session
        /// </summary>
        private void AddSession()
        {
            string strURL = urlTextBox.Text.Trim();
            if (strURL.Contains(OPC_SPECIFICATION_AE))
            {
                if (ServerBrowseAdd != null)
                {
                    ServerBrowseAdd(null, strURL, EnumOPCSpecification.AE10);
                }
            }
            else if (strURL.Contains(OPC_SPECIFICATION_DA))
            {
                if (ServerBrowseAdd != null)
                {
                    ServerBrowseAdd(null, strURL, EnumOPCSpecification.DA10);
                }
            }
            else if (strURL.Contains(OPC_SPECIFICATION_XMLDA))
            {
                if (ServerBrowseAdd != null)
                {
                    ServerBrowseAdd(null, strURL, EnumOPCSpecification.XMLDA10);
                }
            }
            if (EnableButtons != null)
            {
                EnableButtons(false, false, false, false, true);
            }
        }

        #endregion

        #endregion

        #region Public Methods

        #region Constructor

        public ServerDiscoveryControl()
        {
            InitializeComponent();

            //create the treeView main roots
            CreateTreeMainRoots();
        }

        #endregion   
    
        #region Create Main Roots

        public void CreateTreeMainRoots()
        {
            serversTreeView.Nodes.Clear();
            //add local treenode and his childrens
            TreeNode localNode = new TreeNode(LOCAL, LOCALPC_INDEX, LOCALPC_INDEX);
            serversTreeView.Nodes.Add(localNode);

            TreeNode dataAccessNode = new TreeNode(DATA_ACCESS, DATA_INDEX, DATA_INDEX);
            localNode.Nodes.Add(dataAccessNode);
            dataAccessNode.Nodes.Add(new TreeNode("", LOCALPC_INDEX, LOCALPC_INDEX));

            TreeNode alarmEventsNode = new TreeNode(ALARM_AND_EVENTS, DATA_INDEX, DATA_INDEX);
            localNode.Nodes.Add(alarmEventsNode);
            alarmEventsNode.Nodes.Add(new TreeNode("", LOCALPC_INDEX, LOCALPC_INDEX));

            //add remote treenode
            TreeNode remoteNode = new TreeNode(REMOTE, REMOTE_INDEX, REMOTE_INDEX);
            serversTreeView.Nodes.Add(remoteNode);
            remoteNode.Nodes.Add(new TreeNode("", REMOTE_INDEX, REMOTE_INDEX));
        }

        #endregion

        #endregion

        #region Event Handlers

        /// <summary>
        /// Add ServerData nodes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void serversTreeView_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            e.Cancel = !canExpand;
            TreeNode currentNode = e.Node;
            if (currentNode == null)
            {
                return;
            }
            serversTreeView.Cursor = Cursors.WaitCursor;
            ExpandTreeNode(currentNode);
            canExpand = true;
            serversTreeView.Cursor = Cursors.Default;
        }

        /// <summary>
        /// Before collapse
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void serversTreeView_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
        {
            e.Cancel = !canCollapse;
            canCollapse = true;
        }

        /// <summary>
        /// Fill/empty url text
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void serversTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag != null)
            {
                ServerBrowserData browswData = (ServerBrowserData)e.Node.Tag;
                urlTextBox.Text = browswData.Url;
            }
            else
            {
                urlTextBox.Text = "";
            }
        }

        /// <summary>
        /// Event triggered on mouse click
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void serversTreeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeViewHitTestInfo hit = serversTreeView.HitTest(e.Location);
            if (hit.Location == TreeViewHitTestLocations.PlusMinus)
            {
                canExpand = true;
                canCollapse = true;
            }
            else
            {
                canExpand = false;
                canCollapse = false;
            }
        }

        /// <summary>
        /// Add session
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void addServerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AddSession();
           
        }

        /// <summary>
        /// Add session
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void addServerButton_Click(object sender, EventArgs e)
        {
            AddSession();
        }

        /// <summary>
        /// Add session
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void urlTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                AddSession();
            }
        }

        /// <summary>
        /// Add session
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void serversTreeView_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeViewHitTestInfo hit = serversTreeView.HitTest(e.Location);
            if (hit.Location == TreeViewHitTestLocations.Label)
            {
                AddSession();
            }
            canExpand = true;
            canCollapse = true;
            
        }
        
        /// <summary>
        /// Enable/Disable events
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ServerDiscoveryControl_Enter(object sender, EventArgs e)
        {
            if (EnableButtons != null)
            {
                EnableButtons(false, false, false, false, true);
            }
        }

        #endregion

        #region BrowseData EventArgs Class

        /// <summary>
        /// Boxes event's parameters passed by BrowseDataHandler
        /// </summary>
        public class BrowseDataEventArgs : EventArgs
        {
            #region Properties

            private ServerBrowserData browseData;
            public ServerBrowserData BrowseData
            {
                get { return browseData; }
                //no setter
            }
            #endregion

            #region Constructor

            public BrowseDataEventArgs(ServerBrowserData browse)
            {
                browseData = browse;
            }

            #endregion
        }

        #endregion

        #region Browse Data Comparer

        public class ServerBrowseDataComparer : IComparer<ServerBrowserData>
        {
            #region IComparer<ServerBrowserData> Methods

            public int Compare(ServerBrowserData x, ServerBrowserData y)
            {
                return x.Description.CompareTo(y.Description);
            }

            #endregion
        }

        #endregion     

       
       

    }    
}
