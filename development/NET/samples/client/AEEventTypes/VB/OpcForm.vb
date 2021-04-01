Imports System
Imports System.Windows.Forms
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Client

Namespace AEEventTypes
	''' <summary>
	''' Summary description for OpcForm.
	''' </summary>
	Public Class OpcForm
		Inherits System.Windows.Forms.Form
		''' <summary>
		''' Required designer variable.
		''' </summary>
		Private components As System.ComponentModel.Container = Nothing
		Private m_outProc As OutProc = Nothing
		Private m_opcClient As OpcClient = Nothing
		Private m_session As MyAeSession = Nothing
		Private eventsTreeView As System.Windows.Forms.TreeView
		Private imageList1 As System.Windows.Forms.ImageList
		Private m_QC_executionOptions As New ExecutionOptions
		Private m_QA_executionOptions As New ExecutionOptions
		Private m_QCN_executionOptions As New ExecutionOptions
		Private m_QSCN_executionOptions As New ExecutionOptions
		Private m_AF_executionOptions As New ExecutionOptions
		Private label1 As System.Windows.Forms.Label
		Private filtersTextBox As System.Windows.Forms.RichTextBox


		Public Sub New(ByVal anOutProc As OutProc)
			InitializeComponent()

			Try

				m_outProc = anOutProc

				m_opcClient = m_outProc.OpcClient

				m_session = m_opcClient.m_aeSession

				'create the events tree
				Dim root As New TreeNode(m_session.Url + " - Event Space ", 0, 0)
				eventsTreeView.Nodes.Add(root)

				'query categories for each event type
				Dim categories As AeCategory() = Nothing
				m_session.QueryAeCategories(categories, m_QC_executionOptions)

				Dim i As Integer = 0
				While i < categories.Length
					Dim category As New TreeNode(categories(i).Description, 1, 1)
					root.Nodes.Add(category)

					'Query categories' attributes
					Dim attributes As AeAttribute() = Nothing
					categories(i).QueryAeAttributes(attributes, m_QA_executionOptions)

					Dim j As Integer = 0
					While j < attributes.Length
						Dim attribute As New TreeNode(attributes(j).Description, 2, 2)
						category.Nodes.Add(attribute)
						System.Math.Max(System.Threading.Interlocked.Increment(j), -1)
					End While
					'Query ConditionNames
					Dim conditionNames As String() = Nothing
					categories(i).QueryAeConditionNames(conditionNames, m_QCN_executionOptions)

					Dim ii As Integer = 0
					While ii < conditionNames.Length
						Dim conditionName As New TreeNode(conditionNames(ii), 3, 3)
						category.Nodes.Add(conditionName)

						'Query SubConditionNames
						Dim subConditionNames As String() = Nothing
						categories(i).QueryAeSubConditionNames(conditionNames(ii), subConditionNames, m_QSCN_executionOptions)

						Dim iii As Integer = 0
						While iii < subConditionNames.Length
							Console.WriteLine(" The category " + categories(ii).Description + " has for the condition " + conditionNames(ii) + " the subcondition " + subConditionNames(iii))
							System.Math.Max(System.Threading.Interlocked.Increment(iii), iii - 1)
						End While
						System.Math.Max(System.Threading.Interlocked.Increment(ii), ii - 1)
					End While
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
				'Query available filters
				Dim availableFilters As EnumFilterBy
				m_session.QueryAvailableAeFilters(availableFilters, m_AF_executionOptions)

				If (availableFilters And EnumFilterBy.AREA) = EnumFilterBy.AREA Then
					filtersTextBox.AppendText("Filter by Area " & Chr(10) & "")
				End If
				If (availableFilters And EnumFilterBy.CATEGORY) = EnumFilterBy.CATEGORY Then
					filtersTextBox.AppendText("Filter by Category " & Chr(10) & "")
				End If
				If (availableFilters And EnumFilterBy.[EVENT]) = EnumFilterBy.[EVENT] Then
					filtersTextBox.AppendText("Filter by Event " & Chr(10) & "")
				End If
				If (availableFilters And EnumFilterBy.SEVERITY) = EnumFilterBy.SEVERITY Then
					filtersTextBox.AppendText("Filter by Severity " & Chr(10) & "")
				End If
				If (availableFilters And EnumFilterBy.SOURCE) = EnumFilterBy.SOURCE Then
					filtersTextBox.AppendText("Filter by Source " & Chr(10) & "")
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
			Me.eventsTreeView = New System.Windows.Forms.TreeView
			Me.imageList1 = New System.Windows.Forms.ImageList(Me.components)
			Me.label1 = New System.Windows.Forms.Label
			Me.filtersTextBox = New System.Windows.Forms.RichTextBox
			Me.SuspendLayout()
			' 
			' eventsTreeView
			' 
			Me.eventsTreeView.ImageList = Me.imageList1
			Me.eventsTreeView.Location = New System.Drawing.Point(0, 0)
			Me.eventsTreeView.Name = "eventsTreeView"
			Me.eventsTreeView.Size = New System.Drawing.Size(640, 528)
			Me.eventsTreeView.TabIndex = 0
			' 
			' imageList1
			' 
			Me.imageList1.ImageSize = New System.Drawing.Size(16, 16)
			Me.imageList1.ImageStream = (DirectCast((resources.GetObject("imageList1.ImageStream")), System.Windows.Forms.ImageListStreamer))
			Me.imageList1.TransparentColor = System.Drawing.Color.Transparent
			' 
			' label1
			' 
			Me.label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, Convert.ToByte(0))
			Me.label1.Location = New System.Drawing.Point(8, 536)
			Me.label1.Name = "label1"
			Me.label1.Size = New System.Drawing.Size(152, 16)
			Me.label1.TabIndex = 1
			Me.label1.Text = "Available Filters"
			' 
			' filtersTextBox
			' 
			Me.filtersTextBox.AutoWordSelection = True
			Me.filtersTextBox.Location = New System.Drawing.Point(0, 568)
			Me.filtersTextBox.Name = "filtersTextBox"
			Me.filtersTextBox.Size = New System.Drawing.Size(640, 80)
			Me.filtersTextBox.TabIndex = 2
			Me.filtersTextBox.Text = ""
			' 
			' OpcForm
			' 
			Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
			Me.ClientSize = New System.Drawing.Size(640, 654)
			Me.Controls.Add(Me.filtersTextBox)
			Me.Controls.Add(Me.label1)
			Me.Controls.Add(Me.eventsTreeView)
			Me.Name = "OpcForm"
			Me.Text = "AE Event Types"
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

	End Class
End Namespace

