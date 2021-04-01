using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Softing.OPCToolbox.Client;
using System.Runtime.InteropServices;

namespace Softing.OPCToolbox.Client.Control
{
	/// <summary>
	/// Permits configuring the DataControl. This form collects information regarding the items whose values are to be visualized using the DataControl.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="InfoCollector"]/doc/*'
	/// />
	public class InfoCollector : System.Windows.Forms.Form
	{
		#region //Extern members
		//----------------------

		[DllImport("netapi32.dll", EntryPoint = "NetServerEnum")]
		private static extern int NetServerEnum([MarshalAs(UnmanagedType.LPWStr)] string servername,
		                                        int level,
		                                        out IntPtr bufptr,
		                                        int prefmaxlen,
		                                        ref int entriesread,
		                                        ref int totalentries,
		                                        SV_101_TYPES servertype,
		                                        [MarshalAs(UnmanagedType.LPWStr)] string domain,
		                                        int resume_handle);

		[DllImport("netapi32.dll", EntryPoint = "NetApiBufferFree")]
		private static extern int
			NetApiBufferFree(IntPtr buffer);

		//-
		#endregion

		#region //Private attributes
		//---------------------------

		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage itemsPage;
		private System.Windows.Forms.GroupBox SubscriptionsGroupBox;
		private System.Windows.Forms.ColumnHeader nameColumn;
		private System.Windows.Forms.ColumnHeader updateRateColumn;
		private System.Windows.Forms.ColumnHeader noItemsColumn;
		private System.Windows.Forms.Button insertButton;
		private System.Windows.Forms.Button deleteButton;
		private System.Windows.Forms.TreeView itemsTreeView;
		private System.Windows.Forms.ListView subscriptionsList;
		private const uint ERROR_MORE_DATA = 234;
		private System.Windows.Forms.ColumnHeader activeColumnHeader;
		private System.Windows.Forms.Button editButton;
		private System.Windows.Forms.GroupBox browseItemsGroupBox;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button addSubscribedItemButton;
		private System.Windows.Forms.Button removeSubscribedItemButton;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox itemIdTextBox;
		private System.Windows.Forms.ColumnHeader itemIdColumnHeader;
		private System.Windows.Forms.ColumnHeader dataTypeColumnHeader;
		private System.Windows.Forms.ColumnHeader AccessrightsColumnHeader;
		private const uint ERROR_SUCCESS = 0;

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
			[MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)] private UInt32 sv101_platform_id;
			[MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPWStr)] public string sv101_name;
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
		} //	the session that will be serialized at the end
		private ControlDaSession m_session = new ControlDaSession(string.Empty);
		private ControlDaSubscription m_selectedSubscription = null;
		private ListViewItem m_selectedSubscriptionItemListView = null;
		//it shows that the last control on which the click was made was the itemsTreeView
		private bool m_isItemsTreeViewSelected = true;
		//the url of the session that was chosen before switching tabs
		private string m_currentSessionUrl = String.Empty;
		//shows whether is the first time the user switches from server tab to da items tab
		private bool m_isFirstTime = true;
		private SubscriptionProperties m_subscriptionProperties;
		private System.Windows.Forms.ListView subscribedItemsListView;
		//indicates if the closing of the current session is intended; it is used when the exception otb.dll not found occurs
		private bool m_closeSession = true;
		//indicates if the reload of the session configuration is to be done
		private bool m_reload = false;
		private Application m_instance = Application.Instance;
		//this saves the initial session, the session that existed before the current configuration took place
		// if the cancel buuton is pressed this willl be considered the current configuration
		private ControlDaSession m_initialSession = null;
		private TabPage serverPage;
		private Panel panel1;
		private TextBox urlTextBox;
		private Label label1;
		private Button serverPageOkButton;
		private Button serverPageCancelButton;
		private TreeView browseServersTreeView;
		private ListView itemsListView;
		private Button removeButton;
		private Button addButton;
		private Button cancelButton;
		private Button okFinalButton;
		private System.Windows.Forms.Splitter splitter1;
		private ExecutionOptions m_executionOptions = new ExecutionOptions();
		//-
		#endregion

		#region //Private methods
		private void CreateMainTree()
		{
			try
			{
				TreeNode local = new TreeNode("Local", 0, 0);
				this.browseServersTreeView.Nodes.Add(local);

				TreeNode da1 = new TreeNode("Data Access V1", 5, 5);
				local.Nodes.Add(da1);
				da1.Nodes.Add(new TreeNode("", 1, 1));

				TreeNode da2 = new TreeNode("Data Access V2", 5, 5);
				local.Nodes.Add(da2);
				da2.Nodes.Add(new TreeNode("", 1, 1));

				TreeNode da3 = new TreeNode("Data Access V3", 5, 5);
				local.Nodes.Add(da3);
				da3.Nodes.Add(new TreeNode("", 1, 1));

				TreeNode remote = new TreeNode("Remote", 1, 1);
				this.browseServersTreeView.Nodes.Add(remote);

				ArrayList domains = InfoCollector.GetServerList(SV_101_TYPES.SV_TYPE_DOMAIN_ENUM, null);

				for (int i = 0; i < domains.Count; i++)
				{
					TreeNode domain = new TreeNode(((SERVER_INFO_101) domains[i]).sv101_name.ToString(), 3, 3);
					remote.Nodes.Add(domain);
					domain.Nodes.Add(new TreeNode("", 1, 1));
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.CreateMainTree", exc.ToString());
			} //	end try...catch
		} //	end CreateMainTree


		private void CreateBrowseTree(
			string ipAddress,
			EnumOPCSpecification specification,
			TreeNode parent)
		{
			try
			{
				parent.Nodes.Clear();
				Softing.OPCToolbox.Client.ServerBrowser serverBrowser = new ServerBrowser(ipAddress);
				ServerBrowserData[] serverData = null;
				if (
					ResultCode.SUCCEEDED(serverBrowser.Browse(specification, EnumServerBrowserData.ALL, out serverData,
					                                          m_executionOptions)))
				{
					for (int i = 0; i < serverData.Length; i++)
					{
						TreeNode aNode = null;
						if (serverData[i].Description != "")
						{
							aNode = new TreeNode(serverData[i].Description, 6, 6);
							aNode.Tag = serverData[i].Url;
							parent.Nodes.Add(aNode);
						}
						if (serverData[i].ClsId != "")
						{
							TreeNode clsID = new TreeNode(serverData[i].ClsId, 7, 7);
							clsID.Tag = serverData[i].Url;
							aNode.Nodes.Add(clsID);
						}
						if (serverData[i].ProgId != "")
						{
							TreeNode progID = new TreeNode(serverData[i].ProgId, 7, 7);
							progID.Tag = serverData[i].Url;
							aNode.Nodes.Add(progID);
						}
						if (serverData[i].ProgIdVersionIndependent != "")
						{
							TreeNode vprogID = new TreeNode(serverData[i].ProgIdVersionIndependent, 7, 7);
							vprogID.Tag = serverData[i].Url;
							aNode.Nodes.Add(vprogID);
						}
					} //	end for
				}
				else
				{
				} //	end if...else
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.CreateBrowseTree", exc.ToString());
			} //	end try...catch
		} //	end CreateBrowseTree


		private static ArrayList GetServerList(
			InfoCollector.SV_101_TYPES ServerType,
			string domain)
		{
			int entriesread = 0;
			int totalentries = 0;
			ArrayList alServers = new ArrayList();
			try
			{
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
							server = (SERVER_INFO_101) Marshal.PtrToStructure(new IntPtr(ptr), typeof (SERVER_INFO_101));
							ptr += Marshal.SizeOf(server);
							// add the machine name and comment to the arrayList.
							//You could return the entire structure here if desired
							alServers.Add(server);
						} //	end for
					} //	end if
					// free the buffer
					NetApiBufferFree(buf);
				} //	end do...while
				while
					(
					entriesread < totalentries &&
					entriesread != 0
					);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.GetServerList", exc.ToString());
			} //	end try...catch
			return alServers;
		} //	end GetServerList


		/// <summary>
		/// SelectBranch
		/// Colors in a given color all the nodes of a branch.
		/// </summary>
		/// <param name="node">the parent node of the branch</param>
		/// <param name="imageIndex">the index of the image to be added to the node</param>
		private void SelectBranch(
			TreeNode node,
			int imageIndex)
		{
			try
			{
				if (node.Nodes.Count != 0)
				{
					foreach (TreeNode aNode in node.Nodes)
						SelectBranch(
							aNode,
							imageIndex);
				} //	end if
				node.ImageIndex = imageIndex;
				node.SelectedImageIndex = imageIndex;
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.SelectBranch", exc.ToString());
			} //	end try...catch
		} //	end SelectBranch

		/// <summary>
		/// Checks if an item belongs to a subscription
		/// </summary>
		/// <param name="aSubscription">the subscription to which the item is supposed to belong</param>
		/// <param name="itemId">the id of the item</param>
		private bool SubscriptionContainsItem(
			ControlDaSubscription aSubscription,
			string itemId)
		{
			try
			{
				DaItem[] items = aSubscription.ItemList;
				for (int i = 0; i < items.Length; i++)
				{
					if (((ControlDaItem) items[i]).StoredId == itemId)
						return true;
				} //	end for
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.SubscriptionContainsItem", exc.ToString());
			} //	end try...catch
			return false;
		} //	end SubscriptionContainsItem


		/// <summary>
		/// In the subscriptionList ListView changes the value of NumberOfItems field
		/// </summary>
		private void ChangeNumberOfItemsField()
		{
			//change the "Number of Items" column in the subscriptionList
			try
			{
				FindSelectedSubscription();
				this.m_selectedSubscriptionItemListView.SubItems[3].Text =
					((ControlDaSubscription) this.subscriptionsList.SelectedItems[0].Tag).ItemList.Length.ToString();
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.ChangeNumberOfItemsField", exc.ToString());
			} //	end try...catch
		} //	end ChangeNumberOfItemsField


		/// <summary>
		/// Adds items to the selected subscription
		/// The items to be added at the subscription are the items of the AddressSpaceElement given as parameter
		/// </summary>
		/// <param name="anAddressSpaceElement">the AddressSpaceElement whose items are to be added to the subscription </param>
		private void AddItemsToSelectedSubscription(DaAddressSpaceElement anAddressSpaceElement)
		{
			try
			{
				System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.WaitCursor;

				FindSelectedSubscription();

				DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.DA_ITEM;
				DaAddressSpaceElement[] addressSpaceElements = null;

				if (ResultCode.SUCCEEDED(
					m_session.Browse(
						anAddressSpaceElement.ItemId,
						anAddressSpaceElement.ItemPath,
						browseOptions,
						out addressSpaceElements,
						m_executionOptions)))
				{
					//disconnect the subscription
					m_selectedSubscription.Disconnect(m_executionOptions);

					for (int i = 0; i < addressSpaceElements.Length; i++)
					{
						ControlDaItem controlItem = new ControlDaItem(
							addressSpaceElements[i].ItemId,
							m_selectedSubscription);
					} //	end for

					m_selectedSubscription.Connect(true, false, m_executionOptions);

					for (int i = 0; i < addressSpaceElements.Length; i++)
					{
						//change the selected listViewItem's tag with the info's of the new subscription
						this.m_selectedSubscriptionItemListView.Tag = m_selectedSubscription;

						string itemID = addressSpaceElements[i].ItemId;
						ControlDaItem controlItem = (ControlDaItem) (GetSubscriptionItems(m_selectedSubscription, itemID))[0];
						//add item to the subscribedItemsList
						ListViewItem newItem = new ListViewItem(controlItem.StoredId);
						newItem.Tag = controlItem;
						subscribedItemsListView.Items.Add(newItem);
						newItem.SubItems.Add(controlItem.NativeDatatype.ToString());
						newItem.SubItems.Add(controlItem.AccessRights.ToString());

						if (this.subscriptionsList.InvokeRequired)
						{
							ChangeNumberOfItemsFieldCallback cnifh = new ChangeNumberOfItemsFieldCallback(ChangeNumberOfItemsField);
							this.subscriptionsList.BeginInvoke(cnifh, null);
						} //	end if
						else
						{
							this.m_selectedSubscriptionItemListView.SubItems[3].Text = this.m_selectedSubscription.ItemList.Length.ToString();
						} //	end else
					} //	end for

					browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;
					addressSpaceElements = null;

					if (ResultCode.SUCCEEDED(
						m_session.Browse(
							anAddressSpaceElement.ItemId,
							anAddressSpaceElement.ItemPath,
							browseOptions,
							out addressSpaceElements,
							m_executionOptions)))
					{
						for (int i = 0; i < addressSpaceElements.Length; i++)
						{
							AddItemsToSelectedSubscription(addressSpaceElements[i]);
						} //	end for
					} //	end if
				} //	end if

				System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.Default;
			}
			catch (Exception exc)
			{
				System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.Default;
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.CreateMainTree", exc.ToString());
			}
		}

		/// <summary>
		/// Adds items to the selected subscription
		/// The items to be added at the subscription are the items of the AddressSpaceElement given as parameter
		/// </summary>
		/// <param name="anAddressSpaceElement">the AddressSpaceElement whose items are to be added to the subscription </param>
		private uint RemoveItemsFromSelectedSubscription(DaAddressSpaceElement anAddressSpaceElement)
		{
			try
			{
				FindSelectedSubscription();
				DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.DA_ITEM;

				DaAddressSpaceElement[] addressSpaceElements = null;
				if (
					ResultCode.SUCCEEDED(m_session.Browse(anAddressSpaceElement.ItemId, anAddressSpaceElement.ItemPath, browseOptions,
					                                      out addressSpaceElements, m_executionOptions)))
				{
					for (int i = 0; i < addressSpaceElements.Length; i++)
					{
						string itemID = addressSpaceElements[i].ItemId;
						ArrayList items = GetSubscriptionItems(m_selectedSubscription, itemID);
						if (items != null)
						{
							for (int ii = 0; ii < items.Count; ii++)
							{
								if (ResultCode.SUCCEEDED(m_selectedSubscription.RemoveDaItem((DaItem) items[ii])))
								{
									//change the selected listViewItem's tag with the info's of the new subscription
									this.m_selectedSubscriptionItemListView.Tag = m_selectedSubscription;
									if (this.subscriptionsList.InvokeRequired)
									{
										ChangeNumberOfItemsFieldCallback cnifh = new ChangeNumberOfItemsFieldCallback(ChangeNumberOfItemsField);
										this.subscriptionsList.BeginInvoke(cnifh, null);
									} //	end if
									else
									{
										this.m_selectedSubscriptionItemListView.SubItems[3].Text =
											this.m_selectedSubscription.ItemList.Length.ToString();
									} //	end else
								} //	end if
							}
						} //	end if
					} //	end for
					browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;
					addressSpaceElements = null;
					if (
						ResultCode.SUCCEEDED(m_session.Browse(anAddressSpaceElement.ItemId, anAddressSpaceElement.ItemPath, browseOptions,
						                                      out addressSpaceElements, m_executionOptions)))
					{
						for (int i = 0; i < addressSpaceElements.Length; i++)
						{
							RemoveItemsFromSelectedSubscription(addressSpaceElements[i]);
						}
					}
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.RemoveItemsFromSelectedSubscription",
				                 exc.ToString());
			}
			return 0;
		}

		/// <summary>
		/// Finds the color of an node
		/// </summary>
		/// <param name="anAddressSpaceElement">the tag of the node whose image index is to be found</param>
		/// <returns></returns>
		private int FindNodeImageIndex(DaAddressSpaceElement anAddressSpaceElement)
		{
			try
			{
				FindSelectedSubscription();
				DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.DA_ITEM;
				DaAddressSpaceElement[] addressSpaceElements = null;
				if (
					ResultCode.SUCCEEDED(m_session.Browse(anAddressSpaceElement.ItemId, anAddressSpaceElement.ItemPath, browseOptions,
					                                      out addressSpaceElements, m_executionOptions)))
				{
					string itemID;
					bool allTheItemsBelongToTheSubscription = true;
					bool atLeastOneItemBelongToTheSubscription = false;
					if (addressSpaceElements.Length == 0)
					{
						//this case must be further examened
						allTheItemsBelongToTheSubscription = false;
						atLeastOneItemBelongToTheSubscription = false;
					}
					else
					{
						allTheItemsBelongToTheSubscription = true;
						atLeastOneItemBelongToTheSubscription = false;
					}
					for (int i = 0; i < addressSpaceElements.Length; i++)
					{
						itemID = addressSpaceElements[i].ItemId;
						if (!SubscriptionContainsItem(m_selectedSubscription, itemID))
						{
							allTheItemsBelongToTheSubscription = false;
						}
						else
						{
							atLeastOneItemBelongToTheSubscription = true;
						}
					}
					//the current node has all the items belonging to the subscription
					if (allTheItemsBelongToTheSubscription)
					{
						return 12;
					}
					else
					{
						//the current node has at least one item added to the subscription
						if (atLeastOneItemBelongToTheSubscription)
						{
							return 15;
						}
						else
						{
							return 14;
						}
					}
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.FindNodeImageIndex", exc.ToString());
			}
			// the node hasn't any items added to the subscription
			return 14;
		}

		/// <summary>
		/// Find what is the selected subscription in the current moment
		/// </summary>
		private void FindSelectedSubscription()
		{
			try
			{
				if ((m_selectedSubscriptionItemListView == null) && (m_selectedSubscription == null))
				{
					m_selectedSubscriptionItemListView = this.subscriptionsList.Items[0];
					m_selectedSubscription = (ControlDaSubscription) m_selectedSubscriptionItemListView.Tag;
				}
				if ((m_selectedSubscriptionItemListView != null) && (m_selectedSubscription == null))
				{
					m_selectedSubscription = (ControlDaSubscription) m_selectedSubscriptionItemListView.Tag;
				}
				if ((m_selectedSubscriptionItemListView == null) && (m_selectedSubscription != null))
				{
					foreach (ListViewItem item in this.subscriptionsList.Items)
					{
						if (((ControlDaSubscription) item.Tag).Name == m_selectedSubscription.Name)
						{
							m_selectedSubscriptionItemListView = item;
							return;
						}
					}
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.FindSelectedSubscription", exc.ToString());
			}
		}

		/// <summary>
		/// Checks if an addressSpaceElement has item of type ITEM
		/// </summary>
		/// <param name="anAddressSpaceElement">the element whose items are to be checked</param>
		/// <returns></returns>
		private bool AddressSpaceElementHasItems(DaAddressSpaceElement anAddressSpaceElement)
		{
			try
			{
				ExecutionOptions options = m_executionOptions;
				options.ExecutionContext = 0;
				options.ExecutionType = EnumExecutionType.ASYNCHRONOUS;

				DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.DA_ITEM;

				DaAddressSpaceElement[] addressSpaceElements = null;

				m_session.Browse(anAddressSpaceElement.ItemId, anAddressSpaceElement.ItemPath, browseOptions,
				                 out addressSpaceElements, options);
				if (addressSpaceElements == null)
				{
					return false;
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.AddressSpaceElementsHasItems",
				                 exc.ToString());
			}
			return true;
		}

		/// <summary>
		/// Checks if an item belongs to a subscription
		/// If yes the item is returned
		/// </summary>
		/// <param name="aSubscription">the subscription for which the item is searched</param>
		/// <param name="itemId">the ID of the item to be searched</param>
		/// <returns></returns>
		private ArrayList GetSubscriptionItems(
			ControlDaSubscription aSubscription,
			string itemId)
		{
			ArrayList items = new ArrayList();
			try
			{
				DaItem[] someItems = aSubscription.ItemList;
				for (int i = 0; i < someItems.Length; i++)
				{
					if (((ControlDaItem) someItems[i]).StoredId == itemId)
					{
						items.Add(someItems[i]);
					}
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.GetSubscriptionsItems", exc.ToString());
			}
			return items;
		}

		/// <summary>
		/// Redraws the ItemsTreeView
		/// </summary>
		private void RefreshBrowseItemsTreeView()
		{
			try
			{
				ClearItemsListView();
				ClearItemsTreeView();

				if ((m_session.CurrentState == EnumObjectState.CONNECTED) || (m_session.CurrentState == EnumObjectState.ACTIVATED))
				{
					TreeNode treeRoot = new TreeNode(m_session.Url, 14, 14);

					//creates the itemsTreeView
					itemsTreeView.Nodes.Add(treeRoot);
					DaAddressSpaceElement root = new DaAddressSpaceElement(EnumAddressSpaceElementType.BRANCH, String.Empty,
					                                                       String.Empty, String.Empty, 0, null);
					root.Name = m_session.Url;
					treeRoot.Tag = root;
					DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
					browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;
					DaAddressSpaceElement[] addressSpaceElements = null;
					if (
						ResultCode.SUCCEEDED(m_session.Browse(String.Empty, String.Empty, browseOptions, out addressSpaceElements,
						                                      m_executionOptions)))
					{
						for (int i = 0; i < addressSpaceElements.Length; i++)
						{
							int imageIndex = FindNodeImageIndex(addressSpaceElements[i]);
							TreeNode node = new TreeNode(addressSpaceElements[i].Name.ToString(), imageIndex, imageIndex);
							node.Tag = addressSpaceElements[i];
							treeRoot.Nodes.Add(node);
						} //	end for
					} //	end if
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.RefreshBrowseItemsTreeView",
				                 exc.ToString());
			}
		}

		/// <summary>
		/// Redraws the SubscribedItemsListView
		/// </summary>
		private void RefreshSubscribedItemsListView()
		{
			try
			{
				if ((m_session.CurrentState == EnumObjectState.CONNECTED) || (m_session.CurrentState == EnumObjectState.ACTIVATED))
				{
					//creates the subscribedItemsTreeView
					int[] results = new int[m_selectedSubscription.ItemList.Length];
					DaItem[] items = m_selectedSubscription.ItemList;

					//validate existing added item
					if (ResultCode.SUCCEEDED(
						m_selectedSubscription.ValidateDaItems(items, out results, m_executionOptions)))
					{
						for (int ii = 0; ii < items.Length; ii++)
						{
							if (ResultCode.SUCCEEDED(results[ii]))
							{
								//add item to the subscribedItemsList
								ListViewItem newItem = new ListViewItem(items[ii].Id);
								newItem.Tag = items[ii];
								subscribedItemsListView.Items.Add(newItem);
								newItem.SubItems.Add(items[ii].NativeDatatype.ToString());
								newItem.SubItems.Add(items[ii].AccessRights.ToString());
							} //	end if
							else
							{
								ListViewItem newItem = new ListViewItem(items[ii].Id);
								newItem.Tag = items[ii];
								newItem.ForeColor = System.Drawing.Color.Red;
								subscribedItemsListView.Items.Add(newItem);
								newItem.SubItems.Add("EMPTY");
								newItem.SubItems.Add(string.Empty);
							} //	end else
						} //	end for
					} //	if validate seccedeed
				} //	if server is connected
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.RefreshSubscribedItemsListView",
				                 exc.ToString());
			}
		}

		private void RemoveAllItems(ListView aListView)
		{
			if (aListView.Items.Count != 0)
			{
				aListView.Items.Clear();
			}
		}

		private void RemoveAllNodes(TreeView aTreeView)
		{
			if (aTreeView.Nodes.Count != 0)
			{
				aTreeView.Nodes.Clear();
			}
		}

		private void ClearSubscriptionList()
		{
			if (subscriptionsList.InvokeRequired)
			{
				RemoveAllItemsFromListView raiflw = new RemoveAllItemsFromListView(RemoveAllItems);
				subscriptionsList.BeginInvoke(raiflw, new object[] {subscriptionsList});
			}
			else
			{
				if (subscriptionsList.Items.Count != 0)
				{
					subscriptionsList.Items.Clear();
				}
			} //	end if...else
		}

		private void ClearSubscribedItemsListView()
		{
			if (subscribedItemsListView.InvokeRequired)
			{
				RemoveAllItemsFromListView raiflw = new RemoveAllItemsFromListView(RemoveAllItems);
				subscribedItemsListView.BeginInvoke(raiflw, new object[] {subscribedItemsListView});
			}
			else
			{
				if (subscribedItemsListView.Items.Count != 0)
				{
					subscribedItemsListView.Items.Clear();
				}
			} //	end if...else
		}

		private void ClearItemsListView()
		{
			if (itemsListView.InvokeRequired)
			{
				RemoveAllItemsFromListView raiflw = new RemoveAllItemsFromListView(RemoveAllItems);
				itemsListView.BeginInvoke(raiflw, new object[] {itemsListView});
			}
			else
			{
				if (itemsListView.Items.Count != 0)
				{
					itemsListView.Items.Clear();
				}
			} //	end if...else
		}

		private void ClearItemsTreeView()
		{
			if (itemsTreeView.InvokeRequired)
			{
				RemoveAllItemsFromTreeView raiflw = new RemoveAllItemsFromTreeView(RemoveAllNodes);
				itemsTreeView.BeginInvoke(raiflw, new object[] {itemsTreeView});
			}
			else
			{
				if (itemsTreeView.Nodes.Count != 0)
				{
					itemsTreeView.Nodes.Clear();
				} //	end if
			} //	end if...else
		}

		private void RemoveItem(ListView fromWhere, ListViewItem whatItem)
		{
			fromWhere.Items.Remove(whatItem);
		}

		private void RemoveNode(TreeView fromWhere, TreeNode whatNode)
		{
			fromWhere.Nodes.Remove(whatNode);
		}

		private void RemoveItemFromListView(ListView aListView, ListViewItem aListViewItem)
		{
			if (aListView.InvokeRequired)
			{
				RemoveItemFromListView riflw = new RemoveItemFromListView(RemoveItem);
				aListView.BeginInvoke(riflw, new object[] {aListView, aListViewItem});
			}
			else
			{
				aListView.Items.Remove(aListViewItem);
			}
		}

		private void RemoveItemFromTreeView(TreeView aTreeView, TreeNode aTreeNode)
		{
			if (aTreeView.InvokeRequired)
			{
				RemoveNodeFromTreeView rnftw = new RemoveNodeFromTreeView(RemoveNode);
				aTreeView.BeginInvoke(rnftw, new object[] {aTreeView, aTreeNode});
			}
			else
			{
				aTreeView.Nodes.Remove(aTreeNode);
			}
		}

		#endregion

		#region //Public Constructors
		//----------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="InfoCollector"/> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="InfoCollector"]/constructor[@name="InfoCollector"]/doc/*'
		/// />
		public InfoCollector(ControlDaSession aSession)
		{
#if NET20
			CheckForIllegalCrossThreadCalls = false;
#endif
			InitializeComponent();
			CreateMainTree();
			m_instance.Initialize();
			m_instance.EnableTracing(
				Softing.OPCToolbox.EnumTraceGroup.ALL,
				Softing.OPCToolbox.EnumTraceGroup.ALL,
				Softing.OPCToolbox.EnumTraceGroup.ALL,
				Softing.OPCToolbox.EnumTraceGroup.ALL,
				System.IO.Path.Combine(Environment.CurrentDirectory, "SoftingDataControlTrace.txt"),
				1000000,
				10);
			try
			{
				if (aSession == null)
				{
					m_instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"InfoCollectorConstructor",
						"Session object given as parameter for the class constructor is null");
				} //	end if
				else
				{
					//the session wasn't previously configured
					if (aSession.StoredUrl == string.Empty)
					{
						m_reload = false;
						m_currentSessionUrl = string.Empty;
					}
					else
					{
						// set the saved session's url in the
						this.urlTextBox.Text = aSession.StoredUrl;
						m_reload = true;
						m_currentSessionUrl = aSession.StoredUrl;

						#region Reload Configuration
						//---------------------------

						m_session = new ControlDaSession(aSession.StoredUrl);

						if (m_session.Valid)
						{
							m_session.Connect(false, true, m_executionOptions);
							m_session.StoredUrl = aSession.StoredUrl;

							foreach (ControlDaSubscription aSubscription in aSession.SubscriptionList)
							{
								ControlDaSubscription newSubscription = new ControlDaSubscription(
									aSubscription.StoredUpdateRate,
									m_session);
								if (newSubscription.Valid)
								{
									newSubscription.Name = aSubscription.StoredName;
									newSubscription.StoredName = aSubscription.StoredName;
									newSubscription.StoredUpdateRate = aSubscription.StoredUpdateRate;
									newSubscription.IsActivated = aSubscription.IsActivated;
								} //	end if

								DaItem[] items = aSubscription.ItemList;
								for (int i = 0; i < items.Length; i++)
								{
									ControlDaItem anItem = new ControlDaItem(
										((ControlDaItem) items[i]).StoredId,
										newSubscription);
								} //	end for

								newSubscription.Connect(true, false, m_executionOptions);

								ListViewItem newItem = new ListViewItem(newSubscription.StoredName);
								//add newly created subscription in subscriptionsList
								subscriptionsList.Items.Add(newItem);

								if (newSubscription.IsActivated)
								{
									newItem.SubItems.Add("true");
								} //	end if
								else
								{
									newItem.SubItems.Add("false");
								} //	end else

								newItem.SubItems.Add(newSubscription.StoredUpdateRate.ToString());
								newItem.SubItems.Add(newSubscription.ItemList.Length.ToString());
								newItem.Tag = newSubscription;

								m_selectedSubscription = newSubscription;
							} //	end foreach subscription

							m_selectedSubscriptionItemListView = this.subscriptionsList.Items[this.subscriptionsList.Items.Count - 1];
						} //	end if adding succeeded
						else
						{
							m_instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT,
								"InfoCollector Constructor - reload",
								"Adding session failed");
						} //	end if...else

						#endregion

						#region Saves this initial configuration
						m_initialSession = new ControlDaSession(aSession.StoredUrl);
						if (m_initialSession.Valid)
						{
							foreach (ControlDaSubscription aSubscription in aSession.SubscriptionList)
							{
								ControlDaSubscription newSubscription = new ControlDaSubscription(
									aSubscription.StoredUpdateRate,
									m_initialSession);
								if (newSubscription.Valid)
								{
									newSubscription.Name = aSubscription.StoredName;
									newSubscription.StoredName = aSubscription.StoredName;
									newSubscription.StoredUpdateRate = aSubscription.StoredUpdateRate;
									newSubscription.IsActivated = aSubscription.IsActivated;
								} //	end if

								DaItem[] items = aSubscription.ItemList;
								for (int i = 0; i < items.Length; i++)
								{
									ControlDaItem anItem = new ControlDaItem(
										((ControlDaItem) items[i]).StoredId,
										newSubscription);
								} //	end for
							} //	end foreach subscription
						} //	end if
						else
						{
							m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, " InfoCollectorConstructor",
							                 "Failed to add initial session to the application");
						}

						#endregion
					} //	end if...else reload configuration

					m_session.m_binaryLicenseDa = aSession.m_binaryLicenseDa;
					m_session.m_binaryLicenseXmlDa = aSession.m_binaryLicenseXmlDa;

					if (m_session.m_binaryLicenseDa != string.Empty)
					{
						m_instance.Activate(EnumFeature.DA_CLIENT, m_session.m_binaryLicenseDa);
					} //	end if

					if (m_session.m_binaryLicenseXmlDa != string.Empty)
					{
						m_instance.Activate(EnumFeature.XMLDA_CLIENT, m_session.m_binaryLicenseXmlDa);
					} //	end if
				} //	end if...else aSession!=null
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollectorConstructor", exc.ToString());
			}
		}

		//-
		#endregion

		#region //Public Properties
		//----------------------

		/// <summary>
		/// Gets or sets the <see cref="ControlDaSession"/> chosen when the DataControl was configured.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="InfoCollector"]/property[@name="InfoCollected"]/doc/*'
		/// />
		[DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Content)]
		public ControlDaSession InfoCollected
		{
			get { return m_session; }
			set { m_session = value; }
		}

		//-
		#endregion

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="InfoCollector"]/method[@name="Dispose"]/doc/*'
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
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof (InfoCollector));
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.serverPage = new System.Windows.Forms.TabPage();
			this.panel1 = new System.Windows.Forms.Panel();
			this.urlTextBox = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.serverPageOkButton = new System.Windows.Forms.Button();
			this.serverPageCancelButton = new System.Windows.Forms.Button();
			this.browseServersTreeView = new System.Windows.Forms.TreeView();
			this.itemsPage = new System.Windows.Forms.TabPage();
			this.browseItemsGroupBox = new System.Windows.Forms.GroupBox();
			this.itemsTreeView = new System.Windows.Forms.TreeView();
			this.okFinalButton = new System.Windows.Forms.Button();
			this.addButton = new System.Windows.Forms.Button();
			this.cancelButton = new System.Windows.Forms.Button();
			this.removeButton = new System.Windows.Forms.Button();
			this.itemsListView = new System.Windows.Forms.ListView();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.itemIdTextBox = new System.Windows.Forms.TextBox();
			this.removeSubscribedItemButton = new System.Windows.Forms.Button();
			this.subscribedItemsListView = new System.Windows.Forms.ListView();
			this.itemIdColumnHeader = new System.Windows.Forms.ColumnHeader();
			this.dataTypeColumnHeader = new System.Windows.Forms.ColumnHeader();
			this.AccessrightsColumnHeader = new System.Windows.Forms.ColumnHeader();
			this.addSubscribedItemButton = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.SubscriptionsGroupBox = new System.Windows.Forms.GroupBox();
			this.editButton = new System.Windows.Forms.Button();
			this.deleteButton = new System.Windows.Forms.Button();
			this.insertButton = new System.Windows.Forms.Button();
			this.subscriptionsList = new System.Windows.Forms.ListView();
			this.nameColumn = new System.Windows.Forms.ColumnHeader();
			this.activeColumnHeader = new System.Windows.Forms.ColumnHeader();
			this.updateRateColumn = new System.Windows.Forms.ColumnHeader();
			this.noItemsColumn = new System.Windows.Forms.ColumnHeader();
			this.splitter1 = new System.Windows.Forms.Splitter();
			this.tabControl1.SuspendLayout();
			this.serverPage.SuspendLayout();
			this.panel1.SuspendLayout();
			this.itemsPage.SuspendLayout();
			this.browseItemsGroupBox.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.SubscriptionsGroupBox.SuspendLayout();
			this.SuspendLayout();
			//
			// imageList1
			//
			this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
			this.imageList1.ImageStream =
				((System.Windows.Forms.ImageListStreamer) (resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			//
			// tabControl1
			//
			this.tabControl1.Controls.Add(this.serverPage);
			this.tabControl1.Controls.Add(this.itemsPage);
			this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl1.Location = new System.Drawing.Point(0, 0);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(600, 481);
			this.tabControl1.TabIndex = 0;
			this.tabControl1.Click += new System.EventHandler(this.tabControl1_Click);
			//
			// serverPage
			//
			this.serverPage.Controls.Add(this.panel1);
			this.serverPage.Location = new System.Drawing.Point(4, 22);
			this.serverPage.Name = "serverPage";
			this.serverPage.Size = new System.Drawing.Size(595, 455);
			this.serverPage.TabIndex = 0;
			this.serverPage.Text = "Server";
			//
			// panel1
			//
			this.panel1.Controls.Add(this.urlTextBox);
			this.panel1.Controls.Add(this.label1);
			this.panel1.Controls.Add(this.serverPageOkButton);
			this.panel1.Controls.Add(this.serverPageCancelButton);
			this.panel1.Controls.Add(this.browseServersTreeView);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(595, 455);
			this.panel1.TabIndex = 5;
			//
			// urlTextBox
			//
			this.urlTextBox.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 (((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.urlTextBox.Location = new System.Drawing.Point(105, 388);
			this.urlTextBox.Name = "urlTextBox";
			this.urlTextBox.Size = new System.Drawing.Size(487, 20);
			this.urlTextBox.TabIndex = 2;
			this.urlTextBox.Text = "";
			//
			// label1
			//
			this.label1.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label1.Location = new System.Drawing.Point(3, 391);
			this.label1.Name = "label1";
			this.label1.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.label1.Size = new System.Drawing.Size(121, 24);
			this.label1.TabIndex = 1;
			this.label1.Text = "  Data Access URL:";
			//
			// serverPageOkButton
			//
			this.serverPageOkButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.serverPageOkButton.Location = new System.Drawing.Point(404, 420);
			this.serverPageOkButton.Name = "serverPageOkButton";
			this.serverPageOkButton.Size = new System.Drawing.Size(84, 23);
			this.serverPageOkButton.TabIndex = 3;
			this.serverPageOkButton.Text = "OK";
			this.serverPageOkButton.Click += new System.EventHandler(this.serverPageOkButton_Click);
			//
			// serverPageCancelButton
			//
			this.serverPageCancelButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.serverPageCancelButton.Location = new System.Drawing.Point(510, 420);
			this.serverPageCancelButton.Name = "serverPageCancelButton";
			this.serverPageCancelButton.Size = new System.Drawing.Size(73, 24);
			this.serverPageCancelButton.TabIndex = 4;
			this.serverPageCancelButton.Text = "Cancel";
			this.serverPageCancelButton.Click += new System.EventHandler(this.serverPageCancelButton_Click);
			//
			// browseServersTreeView
			//
			this.browseServersTreeView.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				    | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.browseServersTreeView.ImageList = this.imageList1;
			this.browseServersTreeView.Location = new System.Drawing.Point(0, 0);
			this.browseServersTreeView.Name = "browseServersTreeView";
			this.browseServersTreeView.Size = new System.Drawing.Size(595, 369);
			this.browseServersTreeView.TabIndex = 0;
			this.browseServersTreeView.AfterSelect +=
				new System.Windows.Forms.TreeViewEventHandler(this.browseServersTreeView_AfterSelect);
			this.browseServersTreeView.BeforeExpand +=
				new System.Windows.Forms.TreeViewCancelEventHandler(this.browseServersTreeView_BeforeExpand);
			//
			// itemsPage
			//
			this.itemsPage.Controls.Add(this.browseItemsGroupBox);
			this.itemsPage.Controls.Add(this.groupBox1);
			this.itemsPage.Controls.Add(this.SubscriptionsGroupBox);
			this.itemsPage.Location = new System.Drawing.Point(4, 22);
			this.itemsPage.Name = "itemsPage";
			this.itemsPage.Size = new System.Drawing.Size(592, 455);
			this.itemsPage.TabIndex = 1;
			this.itemsPage.Text = "DA Items";
			//
			// browseItemsGroupBox
			//
			this.browseItemsGroupBox.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 (((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.browseItemsGroupBox.Controls.Add(this.itemsTreeView);
			this.browseItemsGroupBox.Controls.Add(this.okFinalButton);
			this.browseItemsGroupBox.Controls.Add(this.addButton);
			this.browseItemsGroupBox.Controls.Add(this.cancelButton);
			this.browseItemsGroupBox.Controls.Add(this.removeButton);
			this.browseItemsGroupBox.Controls.Add(this.itemsListView);
			this.browseItemsGroupBox.Location = new System.Drawing.Point(0, 271);
			this.browseItemsGroupBox.Name = "browseItemsGroupBox";
			this.browseItemsGroupBox.Size = new System.Drawing.Size(592, 188);
			this.browseItemsGroupBox.TabIndex = 1;
			this.browseItemsGroupBox.TabStop = false;
			this.browseItemsGroupBox.Text = "BrowseItems";
			//
			// itemsTreeView
			//
			this.itemsTreeView.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
			this.itemsTreeView.ImageList = this.imageList1;
			this.itemsTreeView.Location = new System.Drawing.Point(8, 16);
			this.itemsTreeView.Name = "itemsTreeView";
			this.itemsTreeView.Size = new System.Drawing.Size(246, 135);
			this.itemsTreeView.TabIndex = 0;
			this.itemsTreeView.Click += new System.EventHandler(this.itemsTreeView_Click);
			this.itemsTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.itemsTreeView_AfterSelect);
			this.itemsTreeView.BeforeExpand +=
				new System.Windows.Forms.TreeViewCancelEventHandler(this.itemsTreeView_BeforeExpand);
			//
			// okFinalButton
			//
			this.okFinalButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.okFinalButton.Location = new System.Drawing.Point(445, 155);
			this.okFinalButton.Name = "okFinalButton";
			this.okFinalButton.Size = new System.Drawing.Size(70, 25);
			this.okFinalButton.TabIndex = 3;
			this.okFinalButton.Text = "OK";
			this.okFinalButton.Click += new System.EventHandler(this.okFinalButton_Click);
			//
			// addButton
			//
			this.addButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.addButton.Location = new System.Drawing.Point(517, 16);
			this.addButton.Name = "addButton";
			this.addButton.Size = new System.Drawing.Size(70, 25);
			this.addButton.TabIndex = 1;
			this.addButton.Text = "Add";
			this.addButton.Click += new System.EventHandler(this.addButton_Click);
			//
			// cancelButton
			//
			this.cancelButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(517, 155);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.Size = new System.Drawing.Size(70, 25);
			this.cancelButton.TabIndex = 4;
			this.cancelButton.Text = "Cancel";
			this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
			//
			// removeButton
			//
			this.removeButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.removeButton.Location = new System.Drawing.Point(517, 48);
			this.removeButton.Name = "removeButton";
			this.removeButton.Size = new System.Drawing.Size(70, 25);
			this.removeButton.TabIndex = 2;
			this.removeButton.Text = "Remove";
			this.removeButton.Click += new System.EventHandler(this.removeButton_Click);
			//
			// itemsListView
			//
			this.itemsListView.Anchor = System.Windows.Forms.AnchorStyles.Right;
			this.itemsListView.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.itemsListView.Location = new System.Drawing.Point(260, 16);
			this.itemsListView.Name = "itemsListView";
			this.itemsListView.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.itemsListView.Size = new System.Drawing.Size(251, 135);
			this.itemsListView.SmallImageList = this.imageList1;
			this.itemsListView.TabIndex = 3;
			this.itemsListView.View = System.Windows.Forms.View.List;
			this.itemsListView.Click += new System.EventHandler(this.itemsListView_Click);
			this.itemsListView.DoubleClick += new System.EventHandler(this.itemsListView_DoubleClick);
			//
			// groupBox1
			//
			this.groupBox1.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				    | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this.itemIdTextBox);
			this.groupBox1.Controls.Add(this.removeSubscribedItemButton);
			this.groupBox1.Controls.Add(this.subscribedItemsListView);
			this.groupBox1.Controls.Add(this.addSubscribedItemButton);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Location = new System.Drawing.Point(0, 118);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(592, 149);
			this.groupBox1.TabIndex = 2;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Subscribed Items";
			//
			// itemIdTextBox
			//
			this.itemIdTextBox.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				    | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.itemIdTextBox.Location = new System.Drawing.Point(46, 25);
			this.itemIdTextBox.Name = "itemIdTextBox";
			this.itemIdTextBox.Size = new System.Drawing.Size(465, 20);
			this.itemIdTextBox.TabIndex = 4;
			this.itemIdTextBox.Text = "";
			//
			// removeSubscribedItemButton
			//
			this.removeSubscribedItemButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.removeSubscribedItemButton.Location = new System.Drawing.Point(517, 56);
			this.removeSubscribedItemButton.Name = "removeSubscribedItemButton";
			this.removeSubscribedItemButton.Size = new System.Drawing.Size(70, 25);
			this.removeSubscribedItemButton.TabIndex = 1;
			this.removeSubscribedItemButton.Text = "Remove";
			this.removeSubscribedItemButton.Click += new System.EventHandler(this.removeSubscribedItemButton_Click);
			//
			// subscribedItemsListView
			//
			this.subscribedItemsListView.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				    | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.subscribedItemsListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[]
				{
					this.itemIdColumnHeader,
					this.dataTypeColumnHeader,
					this.AccessrightsColumnHeader
				});
			this.subscribedItemsListView.FullRowSelect = true;
			this.subscribedItemsListView.GridLines = true;
			this.subscribedItemsListView.HideSelection = false;
			this.subscribedItemsListView.Location = new System.Drawing.Point(8, 56);
			this.subscribedItemsListView.Name = "subscribedItemsListView";
			this.subscribedItemsListView.Size = new System.Drawing.Size(503, 87);
			this.subscribedItemsListView.TabIndex = 2;
			this.subscribedItemsListView.View = System.Windows.Forms.View.Details;
			//
			// itemIdColumnHeader
			//
			this.itemIdColumnHeader.Text = "ItemID";
			this.itemIdColumnHeader.Width = 206;
			//
			// dataTypeColumnHeader
			//
			this.dataTypeColumnHeader.Text = "Datatype";
			this.dataTypeColumnHeader.Width = 123;
			//
			// AccessrightsColumnHeader
			//
			this.AccessrightsColumnHeader.Text = "AccessRights";
			this.AccessrightsColumnHeader.Width = 171;
			//
			// addSubscribedItemButton
			//
			this.addSubscribedItemButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.addSubscribedItemButton.Location = new System.Drawing.Point(517, 24);
			this.addSubscribedItemButton.Name = "addSubscribedItemButton";
			this.addSubscribedItemButton.Size = new System.Drawing.Size(70, 25);
			this.addSubscribedItemButton.TabIndex = 0;
			this.addSubscribedItemButton.Text = "Add";
			this.addSubscribedItemButton.Click += new System.EventHandler(this.addSubscribedItemButton_Click);
			//
			// label2
			//
			this.label2.Location = new System.Drawing.Point(4, 29);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(51, 17);
			this.label2.TabIndex = 3;
			this.label2.Text = "Item ID";
			//
			// SubscriptionsGroupBox
			//
			this.SubscriptionsGroupBox.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				    | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.SubscriptionsGroupBox.Controls.Add(this.editButton);
			this.SubscriptionsGroupBox.Controls.Add(this.deleteButton);
			this.SubscriptionsGroupBox.Controls.Add(this.insertButton);
			this.SubscriptionsGroupBox.Controls.Add(this.subscriptionsList);
			this.SubscriptionsGroupBox.Location = new System.Drawing.Point(0, 6);
			this.SubscriptionsGroupBox.Name = "SubscriptionsGroupBox";
			this.SubscriptionsGroupBox.Size = new System.Drawing.Size(592, 121);
			this.SubscriptionsGroupBox.TabIndex = 0;
			this.SubscriptionsGroupBox.TabStop = false;
			this.SubscriptionsGroupBox.Text = "Subscriptions";
			//
			// editButton
			//
			this.editButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.editButton.Location = new System.Drawing.Point(517, 80);
			this.editButton.Name = "editButton";
			this.editButton.Size = new System.Drawing.Size(70, 25);
			this.editButton.TabIndex = 7;
			this.editButton.Text = "Edit";
			this.editButton.Click += new System.EventHandler(this.editButton_Click);
			//
			// deleteButton
			//
			this.deleteButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.deleteButton.Location = new System.Drawing.Point(517, 48);
			this.deleteButton.Name = "deleteButton";
			this.deleteButton.Size = new System.Drawing.Size(70, 25);
			this.deleteButton.TabIndex = 6;
			this.deleteButton.Text = "Delete";
			this.deleteButton.Click += new System.EventHandler(this.deleteButton_Click);
			//
			// insertButton
			//
			this.insertButton.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.insertButton.Location = new System.Drawing.Point(517, 16);
			this.insertButton.Name = "insertButton";
			this.insertButton.Size = new System.Drawing.Size(70, 25);
			this.insertButton.TabIndex = 5;
			this.insertButton.Text = "New";
			this.insertButton.Click += new System.EventHandler(this.insertButton_Click);
			//
			// subscriptionsList
			//
			this.subscriptionsList.Anchor =
				((System.Windows.Forms.AnchorStyles)
				 (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
				   | System.Windows.Forms.AnchorStyles.Right)));
			this.subscriptionsList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[]
				{
					this.nameColumn,
					this.activeColumnHeader,
					this.updateRateColumn,
					this.noItemsColumn
				});
			this.subscriptionsList.FullRowSelect = true;
			this.subscriptionsList.HideSelection = false;
			this.subscriptionsList.Location = new System.Drawing.Point(7, 20);
			this.subscriptionsList.Name = "subscriptionsList";
			this.subscriptionsList.Size = new System.Drawing.Size(504, 91);
			this.subscriptionsList.TabIndex = 0;
			this.subscriptionsList.View = System.Windows.Forms.View.Details;
			this.subscriptionsList.Click += new System.EventHandler(this.subscriptionsList_Click);
			//
			// nameColumn
			//
			this.nameColumn.Text = "Name";
			this.nameColumn.Width = 197;
			//
			// activeColumnHeader
			//
			this.activeColumnHeader.Text = "active";
			this.activeColumnHeader.Width = 70;
			//
			// updateRateColumn
			//
			this.updateRateColumn.Text = "UpdateRate";
			this.updateRateColumn.Width = 121;
			//
			// noItemsColumn
			//
			this.noItemsColumn.Text = "ItemCount";
			this.noItemsColumn.Width = 112;
			//
			// splitter1
			//
			this.splitter1.Location = new System.Drawing.Point(0, 0);
			this.splitter1.Name = "splitter1";
			this.splitter1.Size = new System.Drawing.Size(3, 481);
			this.splitter1.TabIndex = 1;
			this.splitter1.TabStop = false;
			//
			// InfoCollector
			//
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(600, 481);
			this.Controls.Add(this.splitter1);
			this.Controls.Add(this.tabControl1);
			this.Icon = ((System.Drawing.Icon) (resources.GetObject("$this.Icon")));
			this.MinimumSize = new System.Drawing.Size(166, 515);
			this.Name = "InfoCollector";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Softing";
			this.tabControl1.ResumeLayout(false);
			this.serverPage.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			this.itemsPage.ResumeLayout(false);
			this.browseItemsGroupBox.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.SubscriptionsGroupBox.ResumeLayout(false);
			this.ResumeLayout(false);
		}

		#endregion

		#region //BrowseServers
		//---------------------

		private void browseServersTreeView_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			try
			{
				if (e.Node.Tag != null)
					urlTextBox.Text = e.Node.Tag.ToString();
			}
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.browseServersTreeView_AfterSelect",
					exc.ToString());
			}
		}

		private void browseServersTreeView_BeforeExpand(object sender, System.Windows.Forms.TreeViewCancelEventArgs e)
		{
			try
			{
				browseServersTreeView.Cursor = System.Windows.Forms.Cursors.WaitCursor;
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

				if (e.Node.Parent != null)
				{
					if (e.Node.Parent.Text.Equals("Remote"))
					{
						if (e.Node.Nodes[0].ImageIndex == 1)
						{
							e.Node.Nodes.Clear();
							ArrayList hosts = InfoCollector.GetServerList(SV_101_TYPES.SV_TYPE_WORKSTATION, e.Node.Text.ToLower());
							for (int j = 0; j < hosts.Count; j++)
							{
								TreeNode host = new TreeNode(((SERVER_INFO_101) hosts[j]).sv101_name.ToLower(), 4, 4);
								e.Node.Nodes.Add(host);
								host.Nodes.Add(new TreeNode("Data Access V1", 5, 5));
								host.Nodes[0].Nodes.Add(new TreeNode("", 0, 0));
								host.Nodes.Add(new TreeNode("Data Access V2", 5, 5));
								host.Nodes[1].Nodes.Add(new TreeNode("", 0, 0));
								host.Nodes.Add(new TreeNode("Data Access V3", 5, 5));
								host.Nodes[2].Nodes.Add(new TreeNode("", 0, 0));
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
							}
						}
					}
				}
				browseServersTreeView.Cursor = System.Windows.Forms.Cursors.Default;
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.browseServersTreeView_BeforeExpand",
				                 exc.ToString());
			}
		}

		//-
		#endregion

		#region //TabControl
		//-------------------

		private void tabControl1_Click(object sender, System.EventArgs e)
		{
			try
			{
				if ((this.urlTextBox.Text == String.Empty) && (!m_reload))
				{
					return;
				}
				else
				{
					if ((m_currentSessionUrl == urlTextBox.Text) && (!m_reload))
					{
						return;
					}
				}

				if (tabControl1.SelectedTab.Name.Equals("itemsPage"))
				{
					#region //is first time, is reload
					if ((m_reload) && (m_isFirstTime))
					{
						//if the reload must be done but another server is chosen before reload is completed
						if ((m_session.Url != this.urlTextBox.Text) && (this.urlTextBox.Text != String.Empty))
						{
							switch (System.Windows.Forms.MessageBox.Show(
								"You have chosen another server! Do you want to configure this server?",
								"Warning",
								MessageBoxButtons.YesNo,
								System.Windows.Forms.MessageBoxIcon.Warning))
							{
								case (DialogResult.Yes):

									//clears the old configuration
									ClearSubscriptionList();
									ClearSubscribedItemsListView();

									m_session.Disconnect(m_executionOptions);

									foreach (DaSubscription subscription in m_session.SubscriptionList)
									{
										m_session.RemoveDaSubscription(subscription);
									}

									m_instance.RemoveDaSession(m_session);
									m_session.StoredUrl = string.Empty;
									m_session = null;

									m_session = new ControlDaSession(this.urlTextBox.Text);

									#region //Add server
									if (m_session.Valid)
									{
										m_session.Connect(false, false, m_executionOptions);

										if (m_session.CurrentState == EnumObjectState.CONNECTED)
										{
											ControlDaSubscription controlSubscription = new ControlDaSubscription(
												1000,
												m_session);
											if (controlSubscription.Valid)
											{
												controlSubscription.Name = "Subscription1";
												controlSubscription.StoredName = controlSubscription.Name;
												controlSubscription.StoredUpdateRate = controlSubscription.RequestedUpdateRate;
												ListViewItem newItem = new ListViewItem(controlSubscription.StoredName);
												subscriptionsList.Items.Add(newItem);
												controlSubscription.Connect(true, false, m_executionOptions);
												controlSubscription.IsActivated = true;
												newItem.SubItems.Add("true");

												newItem.SubItems.Add(controlSubscription.StoredUpdateRate.ToString());
												newItem.SubItems.Add("0");
												newItem.Tag = controlSubscription;
												m_selectedSubscription = controlSubscription;
												m_selectedSubscriptionItemListView = this.subscriptionsList.Items[0];
											} //	if
										}
									} //	add server succeeded
									else
									{
										m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "tabControl1_Click,is first time, is reload",
										                 "Add DaSession failed!");
										m_isFirstTime = false;
										return;
									}

									#endregion

									#region //Refresh BrowseItemsTreeView
									RefreshBrowseItemsTreeView();

									#endregion

									break;
								case (DialogResult.No):

									#region //Redraw the itemsTreeView and SubscribedItemsListView
									FindSelectedSubscription();

									RefreshBrowseItemsTreeView();

									RefreshSubscribedItemsListView();

									#endregion

									break;
							} //	end switch
							m_isFirstTime = false;
							return;
						} //	end if (m_session.Url != this.urlTextBox.Text)&&(this.urlTextBox.Text != String.Empty)

						//the reload must be done and no other server was chosen
						if (this.urlTextBox.Text == String.Empty)
						{
							ClearSubscribedItemsListView();

							#region //Redraw the itemsTreeView and SubscribedItemsListView
							FindSelectedSubscription();

							RefreshBrowseItemsTreeView();

							RefreshSubscribedItemsListView();

							#endregion

							m_isFirstTime = false;
							return;
						} //	end if

						//if the reload must be done and the same server is chosen before reload is completed
						// do the reload of the saved session
						if ((m_session.Url == this.urlTextBox.Text) && (this.urlTextBox.Text != String.Empty))
						{
							m_isFirstTime = false;
							return;
						}

						m_isFirstTime = false;
						return;
					} //	end if

					#endregion

					#region //is first time,is not reload
					if ((this.urlTextBox.Text != string.Empty) && (!m_reload) && (m_isFirstTime))
					{
						// no reload is necessary;
						if (m_session.Valid)
						{
							m_instance.RemoveDaSession(m_session);
							m_session = null;
						}
						m_session = new ControlDaSession(this.urlTextBox.Text);

						#region //Add server
						//----------------------

						if (m_session.Valid)
						{
							m_session.Connect(
								false,
								false,
								m_executionOptions);

							if (m_session.CurrentState == EnumObjectState.CONNECTED)
							{
								ControlDaSubscription controlSubscription = new ControlDaSubscription(
									1000,
									m_session);
								if (controlSubscription.Valid)
								{
									controlSubscription.Name = "Subscription1";
									controlSubscription.StoredName = controlSubscription.Name;
									controlSubscription.StoredUpdateRate = controlSubscription.RequestedUpdateRate;
									ListViewItem newItem = new ListViewItem(controlSubscription.StoredName);
									subscriptionsList.Items.Add(newItem);
									controlSubscription.Connect(true, true, m_executionOptions);
									controlSubscription.IsActivated = true;
									newItem.SubItems.Add("true");

									newItem.SubItems.Add(controlSubscription.StoredUpdateRate.ToString());
									newItem.SubItems.Add("0");
									newItem.Tag = controlSubscription;
									m_selectedSubscription = controlSubscription;
									m_selectedSubscriptionItemListView = this.subscriptionsList.Items[0];
								} //	if
							} //	end if
						} //	add server succeeded
						else
						{
							m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "tabControl1_Click", "Adding session failed");
							m_isFirstTime = false;
							return;
						}

						#endregion

						#region //Refresh browseItemsTreeView
						RefreshBrowseItemsTreeView();

						#endregion

						m_isFirstTime = false;
						return;
					} //	end urlTextBox != string.Empty

					#endregion

					#region //is not first time
					if ((!m_isFirstTime) && (this.urlTextBox.Text != string.Empty) && (this.urlTextBox.Text != m_session.Url))
					{
						switch (System.Windows.Forms.MessageBox.Show(
							"You have chosen another server! Do you want to configure this server?",
							"Warning",
							MessageBoxButtons.YesNo,
							System.Windows.Forms.MessageBoxIcon.Warning))
						{
							case (DialogResult.Yes):
								//clears the old configuration
								ClearSubscriptionList();

								ClearSubscribedItemsListView();

								m_session.Disconnect(m_executionOptions);
								foreach (DaSubscription subscription in m_session.SubscriptionList)
								{
									m_session.RemoveDaSubscription(subscription);
								}

								m_instance.RemoveDaSession(m_session);
								m_session.StoredUrl = string.Empty;
								m_session = null;

								#region //Add server
								//------------------

								m_session = new ControlDaSession(this.urlTextBox.Text);
								if (m_session.Valid)
								{
									m_session.Connect(
										false,
										false,
										m_executionOptions);

									if (m_session.CurrentState == EnumObjectState.CONNECTED)
									{
										ControlDaSubscription controlSubscription = new ControlDaSubscription(
											1000,
											m_session);
										if (controlSubscription.Valid)
										{
											controlSubscription.Name = "Subscription1";
											controlSubscription.StoredName = controlSubscription.Name;
											controlSubscription.StoredUpdateRate = controlSubscription.RequestedUpdateRate;
											ListViewItem newItem = new ListViewItem(controlSubscription.StoredName);
											subscriptionsList.Items.Add(newItem);
											controlSubscription.Connect(true, false, m_executionOptions);
											controlSubscription.IsActivated = true;
											newItem.SubItems.Add("true");

											newItem.SubItems.Add(controlSubscription.StoredUpdateRate.ToString());
											newItem.SubItems.Add("0");
											newItem.Tag = controlSubscription;
											m_selectedSubscription = controlSubscription;
											m_selectedSubscriptionItemListView = this.subscriptionsList.Items[0];
										} //	if
									}
								} //	add server succeeded
								else
								{
									m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "tabControl1_Click,is not first time",
									                 "Add session failed!");
									return;
								}
								//-
								#endregion

								if (!m_reload)
								{
									RefreshBrowseItemsTreeView();
								}
								if (m_reload)
								{
									#region //Redraw the itemsTreeView and SubscribedItemsListView
									FindSelectedSubscription();
									RefreshBrowseItemsTreeView();
									RefreshSubscribedItemsListView();

									#endregion
								}

								break;
							case (DialogResult.No):
								break;
						} //	end switch
						return;
					} //	end if

					#endregion
				} //	if itemsPage
				if (tabControl1.SelectedTab.Name.Equals("serverPage"))
				{
				}
			} //	end try
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.tabControl1_Click",
					exc.ToString());
			}
		}

		//-
		#endregion

		#region //Buttons
		//---------------

		/// <summary>
		/// Adds a new subscription to the server
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void insertButton_Click(object sender, System.EventArgs e)
		{
			try
			{
				if (m_session == null)
					return;
				if ((m_session.CurrentState != EnumObjectState.CONNECTED) && (m_session.CurrentState != EnumObjectState.ACTIVATED))
				{
					return;
				}
				m_subscriptionProperties = new SubscriptionProperties(m_session, this.subscriptionsList);
				m_subscriptionProperties.IsAddingSubscription = true;
				m_subscriptionProperties.Show();
			}
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.insertButton_Click",
					exc.ToString());
			}
		}

		/// <summary>
		/// Edits an existing subscription
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void editButton_Click(object sender, System.EventArgs e)
		{
			FindSelectedSubscription();
			try
			{
				if (m_session == null)
					return;
				if ((m_session.CurrentState != EnumObjectState.CONNECTED) && (m_session.CurrentState != EnumObjectState.ACTIVATED))
				{
					return;
				}
				int position = subscriptionsList.Items.IndexOf(m_selectedSubscriptionItemListView);
				bool active = false;
				if (m_selectedSubscription.IsActivated)
				{
					active = true;
				}

				m_subscriptionProperties = new SubscriptionProperties(m_session, this.subscriptionsList, position, active);
				m_subscriptionProperties.IsEditingSubscription = true;
				m_subscriptionProperties.Show();
			}
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.insertButton_Click",
					exc.ToString());
			}
		}

		/// <summary>
		/// deletes an existing subscription
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void deleteButton_Click(object sender, System.EventArgs e)
		{
			if (m_session == null)
				return;

			try
			{
				if ((m_session.CurrentState != EnumObjectState.CONNECTED)
				    && (m_session.CurrentState != EnumObjectState.ACTIVATED))
				{
					return;
				}
				ListView.SelectedListViewItemCollection items = subscriptionsList.SelectedItems;

				if (items.Count == m_session.SubscriptionList.Length)
				{
					MessageBox.Show("Cannot delete all the subscriptions that belong to the current session!");
					return;
				} //	end if

				foreach (ListViewItem item in items)
				{
					ControlDaSubscription subscription = (ControlDaSubscription) item.Tag;

					foreach (ControlDaItem daItem in subscription.ItemList)
					{
						subscription.RemoveDaItem(daItem);
					} //	end foreach
					m_session.RemoveDaSubscription(subscription);
					RemoveItemFromListView(subscriptionsList, item);
				} //	end foreach
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.deleteButton_Click", exc.ToString());
			}
		}

		/// <summary>
		/// Adds the selected item to the current subscription
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void addButton_Click(object sender, System.EventArgs e)
		{
			if (m_session == null)
				return;

			if ((m_session.CurrentState != EnumObjectState.CONNECTED) && (m_session.CurrentState != EnumObjectState.ACTIVATED))
			{
				return;
			}
			try
			{
				System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.WaitCursor;
				//the server cannot be added to the subscriptionList
				if (itemsTreeView.SelectedNode.Parent == null)
					return;
				FindSelectedSubscription();
				// the focus is on the itemsListView
				if (!m_isItemsTreeViewSelected)
				{
					m_selectedSubscription.Disconnect(m_executionOptions);

					foreach (ListViewItem item in this.itemsListView.SelectedItems)
					{
						//create the item ID of the selected item
						string itemID = (string) item.Tag;

						//add the item in the ItemsList of the selectedSubscription
						ControlDaItem controlItem = new ControlDaItem(
							itemID,
							m_selectedSubscription);
					} //	end foreach

					m_selectedSubscription.Connect(true, false, m_executionOptions);

					foreach (ListViewItem item in this.itemsListView.SelectedItems)
					{
						item.ImageIndex = 11;

						string itemID = (string) item.Tag;
						ControlDaItem controlItem = (ControlDaItem) (GetSubscriptionItems(m_selectedSubscription, itemID))[0];

						ListViewItem newItem = new ListViewItem(controlItem.StoredId);
						newItem.Tag = controlItem;
						subscribedItemsListView.Items.Add(newItem);
						newItem.SubItems.Add(controlItem.NativeDatatype.ToString());
						newItem.SubItems.Add(controlItem.AccessRights.ToString());

						// if all the items are selected the node became selected,too
						bool allAreGreen = true;
						foreach (ListViewItem anItem in this.itemsListView.Items)
						{
							if (anItem.ImageIndex == 10)
								allAreGreen = false;
						} //	end foreach
						if (allAreGreen)
						{
							this.itemsTreeView.SelectedNode.ImageIndex = 12;
							this.itemsTreeView.SelectedNode.SelectedImageIndex = 12;
						} //	end if
						if (!allAreGreen)
						{
							this.itemsTreeView.SelectedNode.ImageIndex = 15;
							this.itemsTreeView.SelectedNode.SelectedImageIndex = 15;
						} //	end if
						try
						{
							//change the selected listViewItem's tag with the info's of the new subscription
							this.m_selectedSubscriptionItemListView.Tag = m_selectedSubscription;
							if (this.subscriptionsList.InvokeRequired)
							{
								ChangeNumberOfItemsFieldCallback cnifh = new ChangeNumberOfItemsFieldCallback(ChangeNumberOfItemsField);
								this.subscriptionsList.BeginInvoke(cnifh, null);
							} //	end if
							else
							{
								//change the "Number of Items" column in the subscriptionList
								this.m_selectedSubscriptionItemListView.SubItems[3].Text =
									this.m_selectedSubscription.ItemList.Length.ToString();
							} //	end else
						} //	end try
						catch (Exception ex)
						{
							System.Diagnostics.Debug.WriteLine(ex.Message);
						} //	end catch
					} //	end foreach
				} //	end if
					// the focus is on the itemsTreeView
					// a node and his hole subtree is added
				else
				{
					this.itemsTreeView.SelectedNode.ImageIndex = 12;

					this.itemsTreeView.SelectedNode.SelectedImageIndex = 12;

					//the selected node and its branch of nodes are selected
					SelectBranch(itemsTreeView.SelectedNode, 12);

					//the node itself,the items belonging to the node or to the node's branches are added to the selected subscription
					AddItemsToSelectedSubscription((DaAddressSpaceElement) itemsTreeView.SelectedNode.Tag);

					//the items of the current added node must be marked as added(must become bright green)
					if (this.itemsListView.Items.Count != 0)
					{
						foreach (ListViewItem item in this.itemsListView.Items)
						{
							item.ImageIndex = 11;
						} //	end foreach
					} //	end if
				} //	end else
				System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.Default;
			} //	end try
			catch (Exception exc)
			{
				System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.Default;
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.addButton_Click",
					exc.ToString());
			} //	end catch
		} //	end add_button

		private void removeButton_Click(object sender, System.EventArgs e)
		{
			if (m_session == null)
				return;

			if ((m_session.CurrentState != EnumObjectState.CONNECTED) && (m_session.CurrentState != EnumObjectState.ACTIVATED))
			{
				return;
			}
			try
			{
				//the server cannot be removed
				if (itemsTreeView.SelectedNode.Parent == null)
					return;
				FindSelectedSubscription();
				// the focus is on the itemsListView
				if (!m_isItemsTreeViewSelected)
				{
					//remove the item from the ItemsList of the selectedSubscription
					foreach (ListViewItem item in this.itemsListView.SelectedItems)
					{
						// the item was previously added to the subscription
						string itemID = (string) item.Tag;
						if (item.ImageIndex == 11)
						{
							ArrayList items = GetSubscriptionItems(m_selectedSubscription, itemID);
							if (items != null)
							{
								for (int ii = 0; ii < items.Count; ii++)
								{
									if (ResultCode.SUCCEEDED(m_selectedSubscription.RemoveDaItem((DaItem) items[ii])))
									{
										item.ImageIndex = 10;
									} //	end if
								} //	end for
							} //	end if
							//remove selected items from the SubscribedItemsListView
							foreach (ListViewItem subscribedItem in this.subscribedItemsListView.Items)
							{
								if ((((ControlDaItem) subscribedItem.Tag).StoredId) == itemID)
								{
									RemoveItemFromListView(subscribedItemsListView, subscribedItem);
								}
							}
						}
							// the item wasn't added to the subscription
						else
						{
							return;
						}
						// if all the items are deselected the node became deselected,too
						bool allAreGrey = true;
						foreach (ListViewItem anItem in this.itemsListView.Items)
						{
							if (anItem.ImageIndex == 11)
								allAreGrey = false;
						}
						if (allAreGrey)
						{
							this.itemsTreeView.SelectedNode.ImageIndex = 14;
							this.itemsTreeView.SelectedNode.SelectedImageIndex = 14;
						}
						if (!allAreGrey)
						{
							this.itemsTreeView.SelectedNode.ImageIndex = 15;
							this.itemsTreeView.SelectedNode.SelectedImageIndex = 15;
						}
						try
						{
							//change the selected listViewItem's tag with the info's of the new subscription
							this.m_selectedSubscriptionItemListView.Tag = m_selectedSubscription;
							this.m_selectedSubscriptionItemListView.Tag = m_selectedSubscription;
							if (this.subscriptionsList.InvokeRequired)
							{
								ChangeNumberOfItemsFieldCallback cnifh = new ChangeNumberOfItemsFieldCallback(ChangeNumberOfItemsField);
								this.subscriptionsList.BeginInvoke(cnifh, null);
							}
							else
							{
								//change the "ItemCount" column in the subscriptionList
								this.m_selectedSubscriptionItemListView.SubItems[3].Text =
									this.m_selectedSubscription.ItemList.Length.ToString();
							}
						}
						catch (Exception exc)
						{
							m_instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT,
								"InfoCollector.removeButton_Click",
								exc.ToString());
						}
					} //	end foreach
				} //	end if
					// the focus is on the itemsTreeView
					// a node and his whole subtree is removed
				else
				{
					//the node was previously added
					if ((this.itemsTreeView.SelectedNode.ImageIndex == 12) || (this.itemsTreeView.SelectedNode.ImageIndex == 15))
					{
						this.itemsTreeView.SelectedNode.ImageIndex = 14;
						this.itemsTreeView.SelectedNode.SelectedImageIndex = 14;
						//the selected node and its branch of nodes are selected
						SelectBranch(itemsTreeView.SelectedNode, 14);
						//the node itself,the items belonging to the node or to the node's branches are removed from the selected subscription
						uint res = RemoveItemsFromSelectedSubscription((DaAddressSpaceElement) itemsTreeView.SelectedNode.Tag);

						//refreshes the lists
						ClearItemsTreeView();
						ClearItemsListView();
						ClearSubscribedItemsListView();

						#region //Redraw the itemsTreeView and SubscribedItemsListView
						//------------------------------------------------------------

						if ((m_session.CurrentState == EnumObjectState.CONNECTED) || (m_session.CurrentState == EnumObjectState.ACTIVATED))
						{
							TreeNode treeRoot = new TreeNode(m_session.Url, 14, 14);

							//creates the itemsTreeView
							itemsTreeView.Nodes.Add(treeRoot);
							DaAddressSpaceElement root = new DaAddressSpaceElement(EnumAddressSpaceElementType.BRANCH, String.Empty,
							                                                       String.Empty, String.Empty, 0, null);
							root.Name = m_session.Url;
							treeRoot.Tag = root;
							DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
							browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;
							DaAddressSpaceElement[] addressSpaceElements = null;
							if (
								ResultCode.SUCCEEDED(m_session.Browse(String.Empty, String.Empty, browseOptions, out addressSpaceElements,
								                                      m_executionOptions)))
							{
								//MessageBox.Show("Browse succeeded");
								for (int i = 0; i < addressSpaceElements.Length; i++)
								{
									int imageIndex = FindNodeImageIndex(addressSpaceElements[i]);
									TreeNode node = new TreeNode(addressSpaceElements[i].Name.ToString(), imageIndex, imageIndex);
									node.Tag = addressSpaceElements[i];
									treeRoot.Nodes.Add(node);
								} //	end for
							} //	end if

							//creates the subscribedItemsTreeView
							int[] results = new int[m_selectedSubscription.ItemList.Length];
							DaItem[] items = m_selectedSubscription.ItemList;
							//validate existing added item
							if (ResultCode.SUCCEEDED(
								m_selectedSubscription.ValidateDaItems(items, out results, m_executionOptions)))
							{
								for (int ii = 0; ii < items.Length; ii++)
								{
									if (ResultCode.SUCCEEDED(results[ii]))
									{
										//add item to the subscribedItemsList
										ListViewItem newItem = new ListViewItem(items[ii].Id);
										newItem.Tag = items[ii];
										subscribedItemsListView.Items.Add(newItem);
										newItem.SubItems.Add(items[ii].NativeDatatype.ToString());
										newItem.SubItems.Add(items[ii].AccessRights.ToString());
									} //	end if
									else
									{
										ListViewItem newItem = new ListViewItem(items[ii].Id);
										newItem.Tag = items[ii];
										newItem.ForeColor = System.Drawing.Color.Red;
										subscribedItemsListView.Items.Add(newItem);
										newItem.SubItems.Add("EMPTY");
										newItem.SubItems.Add(string.Empty);
									} //	end else
								} //	end for
							} //	if validate seccedeed
						} //	if server is connected
						//-

						#endregion
					} //	if
						// the node wasn't previously added to the subscription
					else
					{
						return;
					} //	else
				} //	else
			} //	try
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.removeButton_Click",
					exc.ToString());
			}
		}

		private void addSubscribedItemButton_Click(object sender, System.EventArgs e)
		{
			FindSelectedSubscription();
			try
			{
				if (itemIdTextBox.Text != string.Empty)
				{
					//create the item ID of the selected item
					string itemId = itemIdTextBox.Text;

					EnumObjectState subscriptionCurrentState = m_selectedSubscription.CurrentState;
					m_selectedSubscription.Disconnect(m_executionOptions);

					//add the item in the ItemsList of the selectedSubscription
					ControlDaItem controlItem = new ControlDaItem(
						itemId,
						m_selectedSubscription);
					//item was succesfully added to the subscription
					if (controlItem.Valid)
					{
						m_selectedSubscription.Connect(true, false, m_executionOptions);

						//validate added item
						int[] results = new int[1];
						results[0] = -1;
						DaItem[] items = new DaItem[1];
						items[0] = (DaItem) controlItem;
						if (ResultCode.SUCCEEDED(
							m_selectedSubscription.ValidateDaItems(items, out results, m_executionOptions)))
						{
							if (ResultCode.SUCCEEDED(results[0]))
							{
								//add item to the subscribedItemsList
								ListViewItem newItem = new ListViewItem(controlItem.StoredId);
								newItem.Tag = controlItem;
								subscribedItemsListView.Items.Add(newItem);
								newItem.SubItems.Add(controlItem.NativeDatatype.ToString());
								newItem.SubItems.Add(controlItem.AccessRights.ToString());
							}
							else
							{
								ListViewItem newItem = new ListViewItem(controlItem.StoredId);
								newItem.Tag = controlItem;
								newItem.ForeColor = System.Drawing.Color.Red;
								subscribedItemsListView.Items.Add(newItem);
								newItem.SubItems.Add("EMPTY");
								newItem.SubItems.Add(string.Empty);
							} //	end else
						} //	end if

						try
						{
							//change the selected listViewItem's tag with the infos of the new subscription
							this.m_selectedSubscriptionItemListView.Tag = m_selectedSubscription;
							if (this.subscriptionsList.InvokeRequired)
							{
								ChangeNumberOfItemsFieldCallback cnifh = new ChangeNumberOfItemsFieldCallback(ChangeNumberOfItemsField);
								this.subscriptionsList.BeginInvoke(cnifh, null);
							} //	end if
							else
							{
								//change the "ItemsCount" column in the subscriptionList
								this.m_selectedSubscriptionItemListView.SubItems[3].Text =
									this.m_selectedSubscription.ItemList.Length.ToString();
							} //	end else
						} //	end try
						catch (Exception ex)
						{
							System.Diagnostics.Debug.WriteLine(ex.Message);
						}
					} //	end if add item succedeed
					this.itemIdTextBox.Text = string.Empty;
					//refresh the lists
					RefreshBrowseItemsTreeView();
				} //	if textbox.text != empty
			}
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.addSubscribedItemButton_Click",
					exc.ToString());
			}
		}

		private void removeSubscribedItemButton_Click(object sender, System.EventArgs e)
		{
			if (m_session == null)
				return;

			if ((m_session.CurrentState != EnumObjectState.CONNECTED) && (m_session.CurrentState != EnumObjectState.ACTIVATED))
			{
				return;
			} //	end if

			try
			{
				FindSelectedSubscription();
				//remove the item from the ItemsList of the selectedSubscription
				foreach (ListViewItem item in this.subscribedItemsListView.SelectedItems)
				{
					// the item was previously added to the subscription

					string itemID = ((ControlDaItem) item.Tag).StoredId;

					ArrayList items = GetSubscriptionItems(m_selectedSubscription, itemID);
					if (items != null)
					{
						for (int ii = 0; ii < items.Count; ii++)
						{
							if (ResultCode.SUCCEEDED(m_selectedSubscription.RemoveDaItem((DaItem) items[ii])))
							{
								//remove selected items from the SubscribedItemsListView
								foreach (ListViewItem subscribedItem in this.subscribedItemsListView.Items)
								{
									if ((((ControlDaItem) subscribedItem.Tag).StoredId) == itemID)
									{
										RemoveItemFromListView(subscribedItemsListView, subscribedItem);
									} //	end if
								} //	end foreach
							} //	end if
						} //	end for
					} //	end if

					try
					{
						//change the selected listViewItem's tag with the info's of the new subscription
						this.m_selectedSubscriptionItemListView.Tag = m_selectedSubscription;
						if (this.subscriptionsList.InvokeRequired)
						{
							ChangeNumberOfItemsFieldCallback cnifh = new ChangeNumberOfItemsFieldCallback(ChangeNumberOfItemsField);
							this.subscriptionsList.BeginInvoke(cnifh, null);
						} //	end if
						else
						{
							//change the "ItemsCount" column in the subscriptionList
							this.m_selectedSubscriptionItemListView.SubItems[3].Text = this.m_selectedSubscription.ItemList.Length.ToString();
						} //	end else
					} //	end try
					catch (Exception exc)
					{
						m_instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"InfoCollector.removeButton_Click",
							exc.ToString());
					} //	end catch
				} //	end foreach
				RefreshBrowseItemsTreeView();
			}
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.removeSubscribedItemButton_Click",
					exc.ToString());
			}
		}

		#region //OK and Cancel buttons
		//-----------------------------

		private void serverPageOkButton_Click(object sender, System.EventArgs e)
		{
			try
			{
				if (m_closeSession)
				{
					if ((m_session.CurrentState == EnumObjectState.CONNECTED) || (m_session.CurrentState == EnumObjectState.ACTIVATED))
						m_session.Disconnect(m_executionOptions);

					m_instance.RemoveDaSession(m_session);
					m_instance.RemoveDaSession(m_initialSession);
					//m_instance.Terminate();
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.serverPageOkButton_Click",
					exc.ToString());
			}
			finally
			{
				this.Close();
			}
		} //	end serverPageOkButton_Click

		private void okFinalButton_Click(object sender, System.EventArgs e)
		{
			try
			{
				if (m_closeSession)
				{
					if ((m_session.CurrentState == EnumObjectState.CONNECTED) ||
					    (m_session.CurrentState == EnumObjectState.ACTIVATED))
					{
						m_session.Disconnect(m_executionOptions);
					}

					m_session.Disconnect(m_executionOptions);
					m_initialSession.Disconnect(m_executionOptions);
					m_instance.RemoveDaSession(m_session);
					m_instance.RemoveDaSession(m_initialSession);
				} //	end if
			} //	end try
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.okFinalButton_Click",
					exc.ToString());
			}
			finally
			{
				this.Close();
			}
		} //	end okFinalButton_Click

		private void serverPageCancelButton_Click(object sender, System.EventArgs e)
		{
			try
			{
				if (m_closeSession)
				{
					if (m_session != null)
					{
						if ((m_session.CurrentState == EnumObjectState.CONNECTED) || (m_session.CurrentState == EnumObjectState.ACTIVATED))
						{
							m_session.Disconnect(m_executionOptions);
							foreach (ControlDaSubscription subscription in m_session.SubscriptionList)
							{
								m_session.RemoveDaSubscription(subscription);
							} //	end foreach
							m_session.StoredUrl = String.Empty;
							m_instance.RemoveDaSession(m_session);
							m_session = null;
						} //	end if
					} //	end if

					#region Reload the saved configuration
					//the current session will be the one saved in the previous configuration
					//if this is the first configuration, no data about session will be saved
					if (m_session == null)
					{
						m_session = new ControlDaSession(m_initialSession.StoredUrl);
					}
					if (m_session.Valid)
					{
						foreach (ControlDaSubscription aSubscription in m_initialSession.SubscriptionList)
						{
							ControlDaSubscription newSubscription = new ControlDaSubscription(
								aSubscription.RequestedUpdateRate,
								m_session);
							if (newSubscription.Valid)
							{
								newSubscription.Name = aSubscription.Name;
								newSubscription.StoredName = aSubscription.StoredName;
								newSubscription.StoredUpdateRate = aSubscription.StoredUpdateRate;
								newSubscription.IsActivated = aSubscription.IsActivated;
							} //	end if

							DaItem[] items = aSubscription.ItemList;
							for (int i = 0; i < items.Length; i++)
							{
								ControlDaItem anItem = new ControlDaItem(
									((ControlDaItem) items[i]).StoredId,
									newSubscription);
							} //	end for
						} //	end foreach subscription
					} //	end if adding succeeded
					else
					{
						m_instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"InfoCollector.serverPageCancelButton_Click",
							"Adding session failed");
					} //	end if...else

					#endregion

					foreach (ControlDaSubscription subscription in m_initialSession.SubscriptionList)
					{
						m_initialSession.RemoveDaSubscription(subscription);
					} //	end foreach
					m_instance.RemoveDaSession(m_initialSession);
					m_initialSession = null;
				} //	end if
			}
			catch (Exception ex)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.serverPageCancelButton_Click",
					ex.ToString());
			}
			finally
			{
				this.Close();
			}
		} //	end serverPageCancelButton_Click

		private void cancelButton_Click(object sender, System.EventArgs e)
		{
			try
			{
				if (m_closeSession)
				{
					if (m_session != null)
					{
						if ((m_session.CurrentState == EnumObjectState.CONNECTED) || (m_session.CurrentState == EnumObjectState.ACTIVATED))
						{
							m_session.Disconnect(m_executionOptions);
							foreach (ControlDaSubscription subscription in m_session.SubscriptionList)
							{
								m_session.RemoveDaSubscription(subscription);
							} //	end foreach
							m_session.StoredUrl = String.Empty;
							m_instance.RemoveDaSession(m_session);
							m_session = null;
						} //	end if
					} //	end if

					#region Reload the saved configuration
					//the current session will be the one saved in the previous configuration
					//if this is the first configuration, no data about session will be saved
					if (m_session == null)
					{
						m_session = new ControlDaSession(m_initialSession.StoredUrl);
					}

					if (m_session.Valid)
					{
						m_session.StoredUrl = m_initialSession.StoredUrl;

						foreach (ControlDaSubscription aSubscription in m_initialSession.SubscriptionList)
						{
							ControlDaSubscription newSubscription = new ControlDaSubscription(
								aSubscription.RequestedUpdateRate,
								m_session);
							if (newSubscription.Valid)
							{
								newSubscription.Name = aSubscription.Name;
								newSubscription.StoredName = aSubscription.StoredName;
								newSubscription.StoredUpdateRate = aSubscription.StoredUpdateRate;
								newSubscription.IsActivated = aSubscription.IsActivated;
							} //	end if

							DaItem[] items = aSubscription.ItemList;
							for (int i = 0; i < items.Length; i++)
							{
								ControlDaItem anItem = new ControlDaItem(
									((ControlDaItem) items[i]).StoredId,
									newSubscription);
							} //	end for
						} //	end foreach subscription
					} //	end if adding succeeded
					else
					{
						m_instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"InfoCollector.serverPageCancelButton_Click",
							"Adding session failed");
					} //	end if...else

					#endregion

					foreach (ControlDaSubscription subscription in m_initialSession.SubscriptionList)
					{
						m_initialSession.RemoveDaSubscription(subscription);
					} //	end foreach
					m_instance.RemoveDaSession(m_initialSession);
					m_initialSession = null;
				} //	end if
			}
			catch (Exception ex)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.serverPageCancelButton_Click",
					ex.ToString());
			}
			finally
			{
				this.Close();
			}
		} //	end cancelButton_Click

		//-
		#endregion

		//-
		#endregion

		#region //ItemsTreeView
		//---------------------

		private void itemsTreeView_BeforeExpand(object sender, System.Windows.Forms.TreeViewCancelEventArgs e)
		{
			try
			{
				itemsTreeView.Cursor = System.Windows.Forms.Cursors.WaitCursor;
				FindSelectedSubscription();
				// the node is selected
				int imageIndexForSelectedNodes = 12;
				if (e.Node.Nodes != null)
				{
					foreach (TreeNode rootNode in e.Node.Nodes)
					{
						bool isSelected = true;
						DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
						browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;
						DaAddressSpaceElement[] addressSpaceElements = null;
						if (
							ResultCode.SUCCEEDED(m_session.Browse(((DaAddressSpaceElement) rootNode.Tag).ItemId,
							                                      ((DaAddressSpaceElement) rootNode.Tag).ItemPath, browseOptions,
							                                      out addressSpaceElements, m_executionOptions)))
						{
							rootNode.Nodes.Clear();
							if (addressSpaceElements.Length == 0)
							{
								isSelected = false;
							}
							for (int i = 0; i < addressSpaceElements.Length; i++)
							{
								int imageIndex = FindNodeImageIndex(addressSpaceElements[i]);
								TreeNode node1 = new TreeNode(addressSpaceElements[i].Name, imageIndex, imageIndex);
								node1.Tag = addressSpaceElements[i];
								rootNode.Nodes.Add(node1);
								if (imageIndexForSelectedNodes != imageIndex)
								{
									isSelected = false;
								}
							}
							//checks if all the subnodes of the current node are selected and the node doesn't have it's own items
							if ((isSelected) && (! AddressSpaceElementHasItems((DaAddressSpaceElement) rootNode.Tag)))
							{
								rootNode.ImageIndex = 12;
								rootNode.SelectedImageIndex = 12;
							}
						}
					}
				}
				itemsTreeView.Cursor = System.Windows.Forms.Cursors.Default;
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.itemsTreeView_BeforeExpand",
				                 exc.ToString());
			}
		}

		/// <summary>
		/// Displays in the itemsListBox the items of the selected node
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void itemsTreeView_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			try
			{
				if (this.itemsListView.Items.Count != 0)
				{
					ClearItemsListView();
				}

				FindSelectedSubscription();

				DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.DA_ITEM;

				DaAddressSpaceElement[] addressSpaceElements = null;

				ListViewItem item;
				if (
					ResultCode.SUCCEEDED(m_session.Browse(((DaAddressSpaceElement) e.Node.Tag).ItemId,
					                                      ((DaAddressSpaceElement) e.Node.Tag).ItemPath, browseOptions,
					                                      out addressSpaceElements, m_executionOptions)))
				{
					for (int i = 0; i < addressSpaceElements.Length; i++)
					{
						//the node is selected with all its items
						switch (e.Node.ImageIndex)
						{
								//all the node item's are selected
							case 12:
								item = new ListViewItem(addressSpaceElements[i].Name, 11);
								item.Tag = addressSpaceElements[i].ItemId;
								this.itemsListView.Items.Add(item);
								break;
								//none of the nodes items are selected
							case 14:
								{
									item = new ListViewItem(addressSpaceElements[i].Name, 10);
									item.Tag = addressSpaceElements[i].ItemId;
									this.itemsListView.Items.Add(item);
								}
								break;
								//at least one of the node's items are selected
							case 15:
								{
									string itemID = addressSpaceElements[i].ItemId;
									if (SubscriptionContainsItem(m_selectedSubscription, itemID))
									{
										item = new ListViewItem(addressSpaceElements[i].Name, 11);
										item.Tag = addressSpaceElements[i].ItemId;
										this.itemsListView.Items.Add(item);
									}
									else
									{
										item = new ListViewItem(addressSpaceElements[i].Name, 10);
										item.Tag = addressSpaceElements[i].ItemId;
										this.itemsListView.Items.Add(item);
									}
								}
								break;
						}
					}
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "InfoCollector.itemsTreeView_AfterSelect",
				                 exc.ToString());
			}
		}

		private void itemsTreeView_Click(object sender, System.EventArgs e)
		{
			this.m_isItemsTreeViewSelected = true;
		}

		//-
		#endregion

		#region //SubscriptionList
		//----------------------

		private void subscriptionsList_Click(object sender, EventArgs e)
		{
			System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.WaitCursor;
			try
			{
				if (this.subscriptionsList.SelectedItems.Count != 0)
				{
					if (this.subscriptionsList.SelectedItems[0] != null)
					{
						m_selectedSubscriptionItemListView = this.subscriptionsList.SelectedItems[0];
					}
					else
					{
						m_selectedSubscriptionItemListView = this.subscriptionsList.Items[0];
					} //	end if...else
				}
				else
				{
					return;
				} //	end if...else
				m_selectedSubscription = (ControlDaSubscription) m_selectedSubscriptionItemListView.Tag;
				// the itemsTreeView changes acording to the selected subscription settings
				ClearItemsTreeView();
				ClearItemsListView();
				ClearSubscribedItemsListView();

				#region //Redraw the itemsTreeView and SubscribedItemsListView
				//------------------------------------------------------------

				if ((m_session.CurrentState == EnumObjectState.CONNECTED) || (m_session.CurrentState == EnumObjectState.ACTIVATED))
				{
					TreeNode treeRoot = new TreeNode(m_session.Url, 14, 14);

					//creates the itemsTreeView
					itemsTreeView.Nodes.Add(treeRoot);
					DaAddressSpaceElement root = new DaAddressSpaceElement(EnumAddressSpaceElementType.BRANCH, String.Empty,
					                                                       String.Empty, String.Empty, 0, null);
					root.Name = m_session.Url;
					treeRoot.Tag = root;
					DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();
					browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH;
					DaAddressSpaceElement[] addressSpaceElements = null;
					if (
						ResultCode.SUCCEEDED(m_session.Browse(String.Empty, String.Empty, browseOptions, out addressSpaceElements,
						                                      m_executionOptions)))
					{
						//MessageBox.Show("Browse succeeded");
						for (int i = 0; i < addressSpaceElements.Length; i++)
						{
							int imageIndex = FindNodeImageIndex(addressSpaceElements[i]);
							TreeNode node = new TreeNode(addressSpaceElements[i].Name.ToString(), imageIndex, imageIndex);
							node.Tag = addressSpaceElements[i];
							treeRoot.Nodes.Add(node);
						} //	end for
					} //	end if

					//creates the subscribedItemsTreeView
					int[] results = new int[m_selectedSubscription.ItemList.Length];
					DaItem[] items = m_selectedSubscription.ItemList;
					//validate existing added item
					if (ResultCode.SUCCEEDED(
						m_selectedSubscription.ValidateDaItems(items, out results, m_executionOptions)))
					{
						for (int ii = 0; ii < items.Length; ii++)
						{
							if (ResultCode.SUCCEEDED(results[ii]))
							{
								//add item to the subscribedItemsList
								ListViewItem newItem = new ListViewItem(items[ii].Id);
								newItem.Tag = items[ii];
								subscribedItemsListView.Items.Add(newItem);
								newItem.SubItems.Add(items[ii].NativeDatatype.ToString());
								newItem.SubItems.Add(items[ii].AccessRights.ToString());
							} //	end if
							else
							{
								ListViewItem newItem = new ListViewItem(items[ii].Id);
								newItem.Tag = items[ii];
								newItem.ForeColor = System.Drawing.Color.Red;
								subscribedItemsListView.Items.Add(newItem);
								newItem.SubItems.Add("EMPTY");
								newItem.SubItems.Add(string.Empty);
							} //	end else
						} //	end for
					} //	if validate seccedeed
				} //	if server is connected
				//-

				#endregion
			}
			catch (Exception exc)
			{
				System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.Default;
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.subscriptionList_SelectedIndexChanged",
					exc.ToString());
			}
			System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.Default;
		}

		//-
		#endregion

		#region //ItemsListView
		//---------------------

		private void itemsListView_Click(object sender, System.EventArgs e)
		{
			this.m_isItemsTreeViewSelected = false;
		}

		/// <summary>
		/// Add or remove the current item.If the item is already added,it removes it.
		/// If the item is not added it adds it
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void itemsListView_DoubleClick(object sender, System.EventArgs e)
		{
			try
			{
				FindSelectedSubscription();
				ListViewItem item = itemsListView.FocusedItem;
				string itemId = (string) item.Tag;
				if (SubscriptionContainsItem(m_selectedSubscription, itemId))
				{
					this.removeButton_Click(this, new System.EventArgs());
				}
				else
				{
					addButton_Click(this, new System.EventArgs());
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"InfoCollector.itemsListView_DoubleClick",
					exc.ToString());
			}
		}

		//-
		#endregion
	} //	end class InfoCollector

	internal delegate void ChangeNumberOfItemsFieldCallback();

	internal delegate void RemoveAllItemsFromListView(System.Windows.Forms.ListView aListView);

	internal delegate void RemoveAllItemsFromTreeView(System.Windows.Forms.TreeView aTreeView);

	internal delegate void RemoveItemFromListView(ListView aListView, ListViewItem anItem);

	internal delegate void RemoveNodeFromTreeView(TreeView aTreeView, TreeNode aTreeNode);
}