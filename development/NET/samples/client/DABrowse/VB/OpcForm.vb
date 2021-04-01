Imports System
Imports System.Windows.Forms
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Client

Namespace DABrowse
	''' <summary>
	''' Summary description for OpcForm.
	''' </summary>
	Public Class OpcForm
		Inherits System.Windows.Forms.Form
		''' <summary>
		''' Required designer variable.
		''' </summary>
		Private components As System.ComponentModel.Container = Nothing
		Private imageList1 As System.Windows.Forms.ImageList
		Private itemsTreeView As System.Windows.Forms.TreeView
		Private m_outProc As OutProc = Nothing
		Private m_opcClient As OpcClient = Nothing
		Private m_propertyGetOptions As New DaGetPropertiesOptions
		Private m_executionOptions As ExecutionOptions = Nothing
		Private m_session As MyDaSession = Nothing
		Public Sub New(ByVal anOutProc As OutProc)
			InitializeComponent()

			Try

				m_outProc = anOutProc

				m_opcClient = m_outProc.OpcClient

				m_session = m_opcClient.GetSession()

				m_executionOptions = m_opcClient.GetExecutionOptions()

				'create the browse tree root element
				Dim treeRoot As New TreeNode(m_session.Url, 0, 0)
				Dim rootDaAddressSpaceElement As New DaAddressSpaceElement(EnumAddressSpaceElementType.BRANCH, String.Empty, String.Empty, String.Empty, System.Convert.ToUInt32(0), Nothing)
				rootDaAddressSpaceElement.Session = m_session
				treeRoot.Tag = rootDaAddressSpaceElement
				itemsTreeView.Nodes.Add(treeRoot)

				Dim addressSpaceElements As DaAddressSpaceElement() = Nothing

				'Browse branches"
				'---------------------

				Dim browseOptions As New DaAddressSpaceElementBrowseOptions
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH

				'get the root's children
				If ResultCode.SUCCEEDED(m_session.Browse((DirectCast(treeRoot.Tag, DaAddressSpaceElement)).ItemId, (DirectCast(treeRoot.Tag, DaAddressSpaceElement)).ItemPath, browseOptions, addressSpaceElements, m_executionOptions)) Then
					If Not addressSpaceElements Is Nothing Then
						Dim i As Integer = 0
						While i < addressSpaceElements.Length
							Dim node As New TreeNode(addressSpaceElements(i).Name, 1, 1)
							node.Tag = addressSpaceElements(i)
							treeRoot.Nodes.Add(node)
							System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
						End While
					End If
				End If

				addressSpaceElements = Nothing

				'Browse Leaves"
				'-------------------                    
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.LEAF

				'get the root's children
				If ResultCode.SUCCEEDED(m_session.Browse((DirectCast(treeRoot.Tag, DaAddressSpaceElement)).ItemId, (DirectCast(treeRoot.Tag, DaAddressSpaceElement)).ItemPath, browseOptions, addressSpaceElements, m_executionOptions)) Then
					If Not addressSpaceElements Is Nothing Then
						Dim i As Integer = 0
						While i < addressSpaceElements.Length
							Dim node As New TreeNode(addressSpaceElements(i).Name, 2, 2)
							node.Tag = addressSpaceElements(i)
							treeRoot.Nodes.Add(node)
							System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
						End While
					End If
				End If


				'GetDaProperties"
				'---------------------

				Dim daProperties As DaProperty() = Nothing
				m_propertyGetOptions.WhatPropertyData = EnumPropertyData.ALL

				If ResultCode.SUCCEEDED(rootDaAddressSpaceElement.GetDaProperties(m_propertyGetOptions, daProperties, m_executionOptions)) Then
					If Not daProperties Is Nothing Then
						Dim i As Integer = 0
						While i < daProperties.Length
							Dim node As New TreeNode(daProperties(i).Name, 3, 3)
							'add all properties except OPC-specific properties
							If (daProperties(i).Id.CompareTo(Convert.ToUInt32(100))) >= 0 Then
								treeRoot.Nodes.Add(node)
							End If
							System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
						End While
					End If
				End If

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
			Me.itemsTreeView = New System.Windows.Forms.TreeView
			Me.imageList1 = New System.Windows.Forms.ImageList(Me.components)
			Me.SuspendLayout()
			' 
			' itemsTreeView
			' 
			Me.itemsTreeView.ImageIndex = 3
			Me.itemsTreeView.ImageList = Me.imageList1
			Me.itemsTreeView.Location = New System.Drawing.Point(0, 0)
			Me.itemsTreeView.Name = "itemsTreeView"
			Me.itemsTreeView.SelectedImageIndex = 3
			Me.itemsTreeView.Size = New System.Drawing.Size(576, 520)
			Me.itemsTreeView.TabIndex = 0
			AddHandler Me.itemsTreeView.BeforeExpand, AddressOf Me.TreeViewBeforeExpand
			' 
			' imageList1
			' 
			Me.imageList1.ImageSize = New System.Drawing.Size(16, 16)
			Me.imageList1.ImageStream = (DirectCast((resources.GetObject("imageList1.ImageStream")), System.Windows.Forms.ImageListStreamer))
			Me.imageList1.TransparentColor = System.Drawing.Color.Transparent
			' 
			' OpcForm
			' 
			Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
			Me.ClientSize = New System.Drawing.Size(576, 518)
			Me.Controls.Add(Me.itemsTreeView)
			Me.Name = "OpcForm"
			Me.Text = "DABrowse"
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

				'  initialize the DA client simulation
				result = result Or opcClient.InitializeDaObjects()

				Dim opcForm As New OpcForm(outProc)

				System.Windows.Forms.Application.Run(opcForm)


				opcClient.Terminate()
				opcClient = Nothing
			Catch exc As Exception
				MessageBox.Show(exc.ToString())
			End Try

		End Sub

		Private Sub TreeViewBeforeExpand(ByVal sender As Object, ByVal e As System.Windows.Forms.TreeViewCancelEventArgs)

			Try
				itemsTreeView.Cursor = System.Windows.Forms.Cursors.WaitCursor

				For Each node As TreeNode In e.Node.Nodes
					m_propertyGetOptions.WhatPropertyData = EnumPropertyData.ALL
					Dim addressSpaceElements As DaAddressSpaceElement() = Nothing

					node.Nodes.Clear()

					If TypeOf node.Tag Is DaAddressSpaceElement Then
						Dim element As DaAddressSpaceElement = DirectCast(node.Tag, DaAddressSpaceElement)
						Dim browseOptions As New DaAddressSpaceElementBrowseOptions

						'Branches
						'-----------
						If (m_session.isConnected()) Then
							browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.BRANCH

							If ResultCode.SUCCEEDED(m_session.Browse(element.ItemId, element.ItemPath, browseOptions, addressSpaceElements, m_executionOptions)) Then
								If Not addressSpaceElements Is Nothing Then
									Dim i As Integer = 0
									While i < addressSpaceElements.Length
										Dim newNode As New TreeNode(addressSpaceElements(i).Name, 1, 1)
										newNode.Tag = addressSpaceElements(i)
										node.Nodes.Add(newNode)
										System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
									End While
								End If
							End If
						End If
						addressSpaceElements = Nothing

						'Leaves
						'-----------
						If (m_session.isConnected()) Then
							browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.LEAF

							If ResultCode.SUCCEEDED(m_session.Browse(element.ItemId, element.ItemPath, browseOptions, addressSpaceElements, m_executionOptions)) Then
								If Not addressSpaceElements Is Nothing Then
									Dim i As Integer = 0
									While i < addressSpaceElements.Length
										Dim newNode As New TreeNode(addressSpaceElements(i).Name, 2, 2)
										newNode.Tag = addressSpaceElements(i)
										node.Nodes.Add(newNode)
										System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
									End While
								End If
							End If
						End If

						Dim daProperties As DaProperty() = Nothing

						'Properties"
						'----------------
						If (m_session.isConnected()) Then
							If ResultCode.SUCCEEDED(element.GetDaProperties(m_propertyGetOptions, daProperties, m_executionOptions)) Then
								If Not daProperties Is Nothing Then
									Dim i As Integer = 0
									While i < daProperties.Length
										'add all properties except OPC-specific properties
										If (daProperties(i).Id.CompareTo(Convert.ToUInt32(100))) >= 0 Then
											Dim newNode As New TreeNode(daProperties(i).Description, 3, 3)
											node.Nodes.Add(newNode)
										End If
										System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
									End While
								End If
							End If
						End If

					End If

					node.Collapse()
				Next
				itemsTreeView.Cursor = System.Windows.Forms.Cursors.[Default]
			Catch exc As Exception
				MessageBox.Show(exc.ToString())

			End Try
		End Sub

	End Class
End Namespace

