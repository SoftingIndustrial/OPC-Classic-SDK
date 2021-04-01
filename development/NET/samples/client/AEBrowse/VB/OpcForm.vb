Imports System
Imports System.Windows.Forms
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Client
Namespace AEBrowse
	''' <summary>
	''' Summary description for OpcForm.
	''' </summary>
	Public Class OpcForm
		Inherits Form
		''' <summary>
		''' Required designer variable.
		''' </summary>
		Private panel1 As Panel
		Private imageList1 As ImageList
		Private addressSpaceTreeView As TreeView
		Private components As System.ComponentModel.Container
		Private m_session As MyAeSession = Nothing
		Private m_executionOptions As ExecutionOptions
		Public Shared index As Integer = 0
		Private m_outProc As OutProc = Nothing
		Private m_opcClient As OpcClient = Nothing

		Public Sub New(ByVal anOutProc As OutProc)
			InitializeComponent()

			Try

				m_executionOptions = New ExecutionOptions

				m_outProc = anOutProc

				m_opcClient = m_outProc.OpcClient

				m_session = m_opcClient.getSession()

				Dim treeRoot As New TreeNode(m_session.Url + " - Area space", 0, 0)
				addressSpaceTreeView.Nodes.Add(treeRoot)
				treeRoot.Nodes.Add(New TreeNode(""))
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
			Me.panel1 = New Panel
			Me.addressSpaceTreeView = New TreeView
			Me.imageList1 = New ImageList(Me.components)
			Me.panel1.SuspendLayout()
			Me.SuspendLayout()
			' 
			' panel1
			' 
			Me.panel1.Controls.Add(Me.addressSpaceTreeView)
			Me.panel1.Dock = System.Windows.Forms.DockStyle.Fill
			Me.panel1.Location = New System.Drawing.Point(0, 0)
			Me.panel1.Name = "panel1"
			Me.panel1.Size = New System.Drawing.Size(712, 518)
			Me.panel1.TabIndex = 0
			' 
			' addressSpaceTreeView
			' 
			Me.addressSpaceTreeView.Dock = System.Windows.Forms.DockStyle.Fill
			Me.addressSpaceTreeView.ImageList = Me.imageList1
			Me.addressSpaceTreeView.Location = New System.Drawing.Point(0, 0)
			Me.addressSpaceTreeView.Name = "addressSpaceTreeView"
			Me.addressSpaceTreeView.Size = New System.Drawing.Size(712, 518)
			Me.addressSpaceTreeView.TabIndex = 0
			AddHandler Me.addressSpaceTreeView.BeforeExpand, AddressOf Me.AddressSpaceTreeViewBeforeExpand
			' 
			' imageList1
			' 
			Me.imageList1.ImageSize = New System.Drawing.Size(16, 16)
			Me.imageList1.ImageStream = (DirectCast((resources.GetObject("imageList1.ImageStream")), System.Windows.Forms.ImageListStreamer))
			Me.imageList1.TransparentColor = System.Drawing.Color.Transparent
			' 
			' Form1
			' 
			Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
			Me.ClientSize = New System.Drawing.Size(712, 518)
			Me.Controls.Add(Me.panel1)
			Me.Icon = (DirectCast((resources.GetObject("$this.Icon")), System.Drawing.Icon))
			Me.Name = "OpcForm"
			Me.Text = "Browse Area Space"
			Me.panel1.ResumeLayout(False)
			Me.ResumeLayout(False)

		End Sub
#End Region

		''' <summary>
		''' The main entry point for the application.
		''' </summary>
		Shared Sub Main()
			Dim outProc As New OutProc
			Dim result As Integer = EnumResultCode.S_OK
			Try
				outProc.CreateOpcClient()
				Dim opcClient As OpcClient = outProc.OpcClient

				'  initialize the client instance
				If Not ResultCode.SUCCEEDED(opcClient.Initialize()) Then
					opcClient = Nothing
					Return
				End If

				'  initialize the AE client simulation
				result = result Or opcClient.InitializeAeObjects()

				Dim opcForm As New OpcForm(outProc)

				System.Windows.Forms.Application.Run(opcForm)

				opcClient.Terminate()
				opcClient = Nothing
			Catch exc As Exception
				MessageBox.Show(exc.ToString())
			End Try

		End Sub

		Private Sub AddressSpaceTreeViewBeforeExpand(ByVal sender As Object, ByVal e As System.Windows.Forms.TreeViewCancelEventArgs)

			addressSpaceTreeView.Cursor = System.Windows.Forms.Cursors.WaitCursor

			Dim rootNode As TreeNode = e.Node
			Dim browseOptions As New AddressSpaceElementBrowseOptions
			browseOptions.ElementTypeFilter = Softing.OPCToolbox.Client.EnumAddressSpaceElementType.BRANCH

			Dim addressSpaceElements As AddressSpaceElement() = Nothing
			If ResultCode.SUCCEEDED(m_session.Browse(DirectCast(rootNode.Tag, AddressSpaceElement), browseOptions, addressSpaceElements, m_executionOptions)) Then
				rootNode.Nodes.Clear()
				Dim i As Integer = 0
				While i < addressSpaceElements.Length
					Dim node As New TreeNode(addressSpaceElements(i).Name, 1, 1)
					node.Tag = addressSpaceElements(i)
					rootNode.Nodes.Add(node)
					node.Nodes.Add(New TreeNode(""))
					Dim conditions As String() = Nothing
					Dim sourcePath As String = addressSpaceElements(i).QualifiedName
					If ResultCode.SUCCEEDED(m_session.QueryAeSourceConditions(sourcePath, conditions, m_executionOptions)) Then

						Dim j As Integer = 0
						While j < conditions.Length
							Dim condition As New TreeNode(conditions(j), 3, 3)
							condition.Tag = [String].Empty
							node.Nodes.Add(condition)

							System.Math.Max(System.Threading.Interlocked.Increment(j), j - 1)
						End While
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			End If
			Dim browseOptions1 As New AddressSpaceElementBrowseOptions
			browseOptions1.ElementTypeFilter = Softing.OPCToolbox.Client.EnumAddressSpaceElementType.LEAF
			If ResultCode.SUCCEEDED(m_session.Browse(DirectCast(rootNode.Tag, AddressSpaceElement), browseOptions1, addressSpaceElements, m_executionOptions)) Then
				Dim i As Integer = 0
				While i < addressSpaceElements.Length
					Dim node As New TreeNode(addressSpaceElements(i).Name, 2, 2)
					node.Tag = addressSpaceElements(i)
					rootNode.Nodes.Add(node)
					Dim conditions As String() = Nothing
					Dim sourcePath As String = addressSpaceElements(i).QualifiedName
					If ResultCode.SUCCEEDED(m_session.QueryAeSourceConditions(sourcePath, conditions, m_executionOptions)) Then

						Dim j As Integer = 0
						While j < conditions.Length
							Dim condition As New TreeNode(conditions(j), 3, 3)
							condition.Tag = [String].Empty
							node.Nodes.Add(condition)
							System.Math.Max(System.Threading.Interlocked.Increment(j), j - 1)
						End While
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			End If

			addressSpaceTreeView.Cursor = System.Windows.Forms.Cursors.[Default]

		End Sub

	End Class
End Namespace

