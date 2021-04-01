'-----------------------------------------------------------------------------
'                                                                            |
'                   Softing Industrial Automation GmbH                       |
'                        Richard-Reitzner-Allee 6                            |
'                           85540 Haar, Germany                              |
'                                                                            |
'                 This is a part of the Softing OPC Toolkit                  |
'        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
'                           All Rights Reserved                              |
'                                                                            |
'-----------------------------------------------------------------------------
'-----------------------------------------------------------------------------
'                             OPC Toolkit NET                                |
'                                                                            |
'  Filename    : OpcForm.vb                                                  |
'  Version     : 4.47.1                                                      |
'  Date        : 27-July-2020                                                |
'                                                                            |
'  Description : Form used for the OutProc debug version                     |
'                                                                            |
'-----------------------------------------------------------------------------
Imports System
Imports System.Collections
Imports System.Windows.Forms
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Client
Imports System.Runtime.InteropServices

Namespace OPCEnumerate
	''' <summary>
	''' Summary description for OpcForm.
	''' </summary>
	Public Class OpcForm
		Inherits System.Windows.Forms.Form
		''' <summary>
		''' Required variable.
		''' </summary>

#Region "Private Attributes"

		Private components As System.ComponentModel.Container = Nothing
		Private m_outProc As OutProc = Nothing
		Private m_opcClient As OpcClient = Nothing
		Private tvServers As System.Windows.Forms.TreeView
		Private label1 As System.Windows.Forms.Label
		Private txtUrl As System.Windows.Forms.TextBox


		Private Const ERROR_SUCCESS As Int32 = 0
		Private Const ERROR_MORE_DATA As Int32 = 234
		Private Enum SV_101_TYPES As Long
			SV_TYPE_WORKSTATION = 1
			SV_TYPE_SERVER = 2
			SV_TYPE_SQLSERVER = 4
			SV_TYPE_DOMAIN_CTRL = 8
			SV_TYPE_DOMAIN_BAKCTRL = 16
			SV_TYPE_TIME_SOURCE = 32
			SV_TYPE_AFP = 64
			SV_TYPE_NOVELL = 128
			SV_TYPE_DOMAIN_MEMBER = 256
			SV_TYPE_PRINTQ_SERVER = 512
			SV_TYPE_DIALIN_SERVER = 1024
			SV_TYPE_XENIX_SERVER = 2048
			SV_TYPE_SERVER_UNIX = 2048
			SV_TYPE_NT = 4096
			SV_TYPE_WFW = 8192
			SV_TYPE_SERVER_MFPN = 16384
			SV_TYPE_SERVER_NT = 32768
			SV_TYPE_POTENTIAL_BROWSER = 65536
			SV_TYPE_BACKUP_BROWSER = 131072
			SV_TYPE_MASTER_BROWSER = 262144
			SV_TYPE_DOMAIN_MASTER = 524288
			SV_TYPE_SERVER_OSF = 1048576
			SV_TYPE_SERVER_VMS = 2097152
			SV_TYPE_WINDOWS = 4194304
			SV_TYPE_DFS = 8388608
			SV_TYPE_CLUSTER_NT = 16777216
			SV_TYPE_TERMINALSERVER = 33554432
			SV_TYPE_CLUSTER_VS_NT = 67108864
			SV_TYPE_DCE = 268435456
			SV_TYPE_ALTERNATE_XPORT = 536870912
			SV_TYPE_LOCAL_LIST_ONLY = 1073741824
			SV_TYPE_DOMAIN_ENUM = 2147483648
			SV_TYPE_ALL = 4294967295
		End Enum
		<StructLayout(LayoutKind.Sequential)> _
		Private Structure SERVER_INFO_101
			<MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)> _
			Public sv101_platform_id As UInt32
			<MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPWStr)> _
			Public sv101_name As String
			<MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)> _
			Public sv101_version_major As UInt32
			<MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)> _
			Public sv101_version_minor As UInt32
			<MarshalAs(System.Runtime.InteropServices.UnmanagedType.U4)> _
			Public sv101_type As UInt32
			<MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPWStr)> _
			Public sv101_comment As String
		End Structure
		Private Enum PLATFORM_ID
			PLATFORM_ID_DOS = 300
			PLATFORM_ID_OS2 = 400
			PLATFORM_ID_NT = 500
			PLATFORM_ID_OSF = 600
			PLATFORM_ID_VMS = 700
		End Enum
		Private m_executionOptions As New ExecutionOptions
		Private imageList1 As System.Windows.Forms.ImageList

#End Region

#Region "Private Methods"

		Private Sub CreateMainTree()

			Dim local As New TreeNode("Local", 1, 1)
			Me.tvServers.Nodes.Add(local)

			Dim da1 As New TreeNode("Data Access V1", 4, 4)
			local.Nodes.Add(da1)
			da1.Nodes.Add(New TreeNode("", 1, 1))

			Dim da2 As New TreeNode("Data Access V2", 4, 4)
			local.Nodes.Add(da2)
			da2.Nodes.Add(New TreeNode("", 1, 1))

			Dim da3 As New TreeNode("Data Access V3", 4, 4)
			local.Nodes.Add(da3)
			da3.Nodes.Add(New TreeNode("", 1, 1))

			Dim ae As New TreeNode("Alarms and Events", 4, 4)
			local.Nodes.Add(ae)
			ae.Nodes.Add(New TreeNode("", 1, 1))

			Dim remote As New TreeNode("Remote", 6, 6)
			Me.tvServers.Nodes.Add(remote)

			Dim domains As ArrayList = OpcForm.GetServerList(SV_101_TYPES.SV_TYPE_DOMAIN_ENUM, Nothing)

			Dim i As Integer = 0
			While i < domains.Count
				Dim domain As New TreeNode((DirectCast(domains(i), SERVER_INFO_101)).sv101_name.ToString(), 3, 3)
				remote.Nodes.Add(domain)
				domain.Nodes.Add(New TreeNode("", 1, 1))
				System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
			End While


		End Sub

		Private Sub CreateBrowseTree(ByVal ipAddress As String, ByVal specification As EnumOPCSpecification, ByVal parent As TreeNode)
			Try
				parent.Nodes.Clear()

				Dim serverBrowser As Softing.OPCToolbox.Client.ServerBrowser = New ServerBrowser(ipAddress)
				Dim serverData As ServerBrowserData() = Nothing

				If ResultCode.SUCCEEDED(serverBrowser.Browse(specification, EnumServerBrowserData.ALL, serverData, m_executionOptions)) Then

					Dim i As Integer = 0
					While i < serverData.Length
						Dim aNode As TreeNode = Nothing

						If serverData(i).Description <> [String].Empty Then
							aNode = New TreeNode(serverData(i).Description, 7, 7)
							aNode.Tag = serverData(i).Url
							parent.Nodes.Add(aNode)
						End If
						If serverData(i).ClsId <> [String].Empty Then
							Dim clsID As New TreeNode(serverData(i).ClsId, 5, 5)
							clsID.Tag = serverData(i).Url
							aNode.Nodes.Add(clsID)
						End If

						If serverData(i).ProgId <> [String].Empty Then
							Dim progID As New TreeNode(serverData(i).ProgId, 5, 5)
							progID.Tag = serverData(i).Url
							aNode.Nodes.Add(progID)
						End If

						If serverData(i).ProgIdVersionIndependent <> [String].Empty Then
							Dim vprogID As New TreeNode(serverData(i).ProgIdVersionIndependent, 5, 5)
							vprogID.Tag = serverData(i).Url
							aNode.Nodes.Add(vprogID)
						End If

						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
				Else
				End If
			Catch ex As Exception
				MessageBox.Show(ex.ToString())
			End Try
		End Sub

		Private Shared Function GetServerList(ByVal ServerType As OpcForm.SV_101_TYPES, ByVal domain As String) As ArrayList
			Dim entriesread As Integer = 0, totalentries As Integer = 0
			Dim alServers As New ArrayList

			Do
				' Buffer to store the available servers
				' Filled by the NetServerEnum function
				Dim buf As New IntPtr

				Dim server As SERVER_INFO_101
				Dim ret As Integer = NetServerEnum(Nothing, 101, buf, -1, entriesread, totalentries, _
				  Convert.ToUInt32(ServerType), domain, 0)
				' if the function returned any data, fill the tree view
				If ret = ERROR_SUCCESS OrElse ret = ERROR_MORE_DATA OrElse entriesread > 0 Then
					Dim ptr As Int32 = buf.ToInt32()

					Dim i As Integer = 0
					While i < entriesread
						' cast pointer to a SERVER_INFO_101 structure
						server = DirectCast(Marshal.PtrToStructure(New IntPtr(ptr), GetType(SERVER_INFO_101)), SERVER_INFO_101)

						ptr += Marshal.SizeOf(server)

						' add the machine name and comment to the arrayList. 
						'You could return the entire structure here if desired
						alServers.Add(server)
						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
				End If

				' free the buffer 
				NetApiBufferFree(buf)

			Loop While entriesread < totalentries AndAlso entriesread <> 0

			Return alServers
		End Function

#End Region

#Region "//Extern methods"
		<DllImport("netapi32.dll", EntryPoint:="NetServerEnum")> _
		Private Shared Function NetServerEnum(<MarshalAs(UnmanagedType.LPWStr)> _
		  ByVal servername As String, ByVal level As Integer, ByRef bufptr As IntPtr, ByVal prefmaxlen As Integer, ByRef entriesread As Integer, ByRef totalentries As Integer, _
		  ByVal servertype As UInt32, <MarshalAs(UnmanagedType.LPWStr)> _
		  ByVal domain As String, ByVal resume_handle As Integer) As Integer
		End Function

		<DllImport("netapi32.dll", EntryPoint:="NetApiBufferFree")> _
		Private Shared Function NetApiBufferFree(ByVal buffer As IntPtr) As Integer
		End Function

#End Region


		Public Sub New(ByVal anOutProc As OutProc)
			InitializeComponent()

			Try

				m_outProc = anOutProc

				m_opcClient = m_outProc.OpcClient
			Catch exc As Exception
				MessageBox.Show(exc.ToString())
			End Try
		End Sub

		''' <summary>
		''' Clean up any resources being used.
		''' </summary>
		Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
			If disposing Then
				If Not components Is Nothing Then
					components.Dispose()
				End If
			End If
			MyBase.Dispose(disposing)
		End Sub

#Region "Windows Form Designer generated code"
		''' <summary>
		''' Required method for Designer support - do not modify
		''' the contents of this method with the code editor.
		''' </summary>
		Private Sub InitializeComponent()
			Me.components = New System.ComponentModel.Container
			Dim resources As New System.Resources.ResourceManager(GetType(OpcForm))
			Me.tvServers = New System.Windows.Forms.TreeView
			Me.imageList1 = New System.Windows.Forms.ImageList(Me.components)
			Me.label1 = New System.Windows.Forms.Label
			Me.txtUrl = New System.Windows.Forms.TextBox
			Me.SuspendLayout()
			' 
			' tvServers
			' 
			Me.tvServers.ImageList = Me.imageList1
			Me.tvServers.Location = New System.Drawing.Point(24, 16)
			Me.tvServers.Name = "tvServers"
			Me.tvServers.Size = New System.Drawing.Size(824, 424)
			Me.tvServers.TabIndex = 0
			AddHandler Me.tvServers.AfterSelect, AddressOf Me.tvServers_AfterSelect
			AddHandler Me.tvServers.BeforeExpand, AddressOf Me.tvServers_BeforeExpand
			' 
			' imageList1
			' 
			Me.imageList1.ImageSize = New System.Drawing.Size(16, 16)
			Me.imageList1.ImageStream = (DirectCast((resources.GetObject("imageList1.ImageStream")), System.Windows.Forms.ImageListStreamer))
			Me.imageList1.TransparentColor = System.Drawing.Color.Transparent
			' 
			' label1
			' 
			Me.label1.Location = New System.Drawing.Point(32, 456)
			Me.label1.Name = "label1"
			Me.label1.Size = New System.Drawing.Size(32, 23)
			Me.label1.TabIndex = 1
			Me.label1.Text = "URL"
			' 
			' txtUrl
			' 
			Me.txtUrl.Location = New System.Drawing.Point(72, 456)
			Me.txtUrl.Name = "txtUrl"
			Me.txtUrl.Size = New System.Drawing.Size(776, 20)
			Me.txtUrl.TabIndex = 2
			Me.txtUrl.Text = ""
			' 
			' OpcForm
			' 
			Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
			Me.ClientSize = New System.Drawing.Size(872, 510)
			Me.Controls.Add(Me.txtUrl)
			Me.Controls.Add(Me.label1)
			Me.Controls.Add(Me.tvServers)
			Me.Icon = (DirectCast((resources.GetObject("$this.Icon")), System.Drawing.Icon))
			Me.Name = "OpcForm"
			Me.Text = "OPCEnumerate"
			AddHandler Me.Load, AddressOf Me.OpcForm_Load
			Me.ResumeLayout(False)

		End Sub
#End Region

		''' <summary>
		''' The main entry point for the application.
		''' </summary>
		Shared Sub Main()
			Dim outProc As New OutProc

			Try
				outProc.CreateOpcClient()
				Dim opcClient As OpcClient = outProc.OpcClient

				'  initialize the client instance
				If Not ResultCode.SUCCEEDED(opcClient.Initialize()) Then
					opcClient = Nothing
					Return
				End If


				Dim opcForm As New OpcForm(outProc)

				System.Windows.Forms.Application.Run(opcForm)


				opcClient.Terminate()
				opcClient = Nothing
			Catch exc As Exception
				MessageBox.Show(exc.ToString())
			End Try

		End Sub

		Private Sub OpcForm_Load(ByVal sender As Object, ByVal e As System.EventArgs)

			CreateMainTree()

		End Sub

		Private Sub tvServers_AfterSelect(ByVal sender As Object, ByVal e As System.Windows.Forms.TreeViewEventArgs)
			If Not e.Node.Tag Is Nothing Then
				Me.txtUrl.Text = e.Node.Tag.ToString().TrimEnd()
			End If
		End Sub

		Private Sub tvServers_BeforeExpand(ByVal sender As Object, ByVal e As System.Windows.Forms.TreeViewCancelEventArgs)

			tvServers.Cursor = System.Windows.Forms.Cursors.WaitCursor
			Dim ipAddress As String = Nothing

			If e.Node.Text.Equals("Data Access V1") Then
				If e.Node.Parent.Text.Equals("Local") Then
					ipAddress = "localhost"
					CreateBrowseTree(ipAddress, EnumOPCSpecification.DA10, e.Node)
				End If
			End If
			If e.Node.Text.Equals("Data Access V2") Then
				If e.Node.Parent.Text.Equals("Local") Then
					ipAddress = "localhost"
					CreateBrowseTree(ipAddress, EnumOPCSpecification.DA20, e.Node)
				End If
			End If
			If e.Node.Text.Equals("Data Access V3") Then
				If e.Node.Parent.Text.Equals("Local") Then
					ipAddress = "localhost"
					CreateBrowseTree(ipAddress, EnumOPCSpecification.DA30, e.Node)
				End If
			End If
			If e.Node.Text.Equals("Alarms and Events") Then
				If e.Node.Parent.Text.Equals("Local") Then
					ipAddress = "localhost"
					CreateBrowseTree(ipAddress, EnumOPCSpecification.AE10, e.Node)
				End If
			End If

			If Not e.Node.Parent Is Nothing Then
				If e.Node.Parent.Text.Equals("Remote") Then
					If e.Node.Nodes(0).ImageIndex = 1 Then
						e.Node.Nodes.Clear()
						Dim hosts As ArrayList = OpcForm.GetServerList(SV_101_TYPES.SV_TYPE_WORKSTATION, e.Node.Text.ToLower())
						Dim j As Integer = 0
						While j < hosts.Count
							Dim host As New TreeNode((DirectCast(hosts(j), SERVER_INFO_101)).sv101_name.ToLower(), 0, 0)
							e.Node.Nodes.Add(host)
							host.Nodes.Add(New TreeNode("Data Access V1", 4, 4))
							host.Nodes(0).Nodes.Add(New TreeNode("", 0, 0))
							host.Nodes.Add(New TreeNode("Data Access V2", 4, 4))
							host.Nodes(1).Nodes.Add(New TreeNode("", 0, 0))
							host.Nodes.Add(New TreeNode("Data Access V3", 4, 4))
							host.Nodes(2).Nodes.Add(New TreeNode("", 0, 0))
							host.Nodes.Add(New TreeNode("Alarms and Events", 4, 4))
							host.Nodes(3).Nodes.Add(New TreeNode("", 0, 0))
							System.Math.Max(System.Threading.Interlocked.Increment(j), j - 1)
						End While
					End If
				End If
				If Not e.Node.Parent.Parent Is Nothing Then
					If Not e.Node.Parent.Parent.Parent Is Nothing Then
						If e.Node.Parent.Parent.Parent.Text.Trim().Equals("Remote") Then
							ipAddress = e.Node.Parent.Text.ToLower()
							If e.Node.Text.Equals("Data Access V1") Then
								CreateBrowseTree(ipAddress, EnumOPCSpecification.DA10, e.Node)
							End If
							If e.Node.Text.Equals("Data Access V2") Then
								CreateBrowseTree(ipAddress, EnumOPCSpecification.DA20, e.Node)
							End If
							If e.Node.Text.Equals("Data Access V3") Then
								CreateBrowseTree(ipAddress, EnumOPCSpecification.DA30, e.Node)
							End If
							If e.Node.Text.Equals("Alarms and Events") Then
								CreateBrowseTree(ipAddress, EnumOPCSpecification.AE10, e.Node)
							End If
						End If
					End If
				End If
			End If
			tvServers.Cursor = System.Windows.Forms.Cursors.[Default]

		End Sub

	End Class
End Namespace

