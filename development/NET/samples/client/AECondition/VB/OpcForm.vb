Imports System
Imports System.Windows.Forms
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Client

Namespace AECondition
	''' <summary>
	''' Summary description for OpcForm.
	''' </summary>
	Public Class OpcForm
		Inherits Form
		''' <summary>
		''' Required designer variable.
		''' </summary>
		Private components As System.ComponentModel.Container = Nothing
		Private m_outProc As OutProc = Nothing
		Private m_opcClient As OpcClient = Nothing
		Private panel1 As Panel
		Private conditionStateColumnHeader As ColumnHeader
		Private sourceColumnHeader As ColumnHeader
		Private nameColumnHeader As ColumnHeader
		Private severityColumnHeader As ColumnHeader
		Private messageColumnHeader As ColumnHeader
		Private timeColumnHeader As ColumnHeader
		Private actorIDcolumnHeader As ColumnHeader
		Private subConditionColumnHeader As ColumnHeader
		Private acknowledgeButton As Button
		Private getConditionStateButton As Button
		Private enableConditionsButton As Button
		Private disableConditionsButton As Button
		Private panel2 As Panel
		Private conditionStateGroupBox As GroupBox
		Private conditionListGroupBox As GroupBox
		Public conditionStateRichTextBox As RichTextBox
		Public conditionlistView As ListView
		Private m_session As MyAeSession = Nothing
		Private m_subscription As MyAeSubscription = Nothing
		Private m_executionOptions As ExecutionOptions = Nothing

		Public Sub New()
			InitializeComponent()
		End Sub

		Public Sub SetOutProc(ByVal anOutProc As OutProc)
			Try
				m_outProc = anOutProc
				m_opcClient = m_outProc.OpcClient
				m_session = m_opcClient.GetSession()
				m_subscription = m_opcClient.GetSubscription()
				m_executionOptions = m_opcClient.GetExecutionOptions()
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
			Dim resources As New System.Resources.ResourceManager(GetType(OpcForm))
			Me.panel1 = New Panel
			Me.panel2 = New Panel
			Me.conditionStateGroupBox = New GroupBox
			Me.conditionStateRichTextBox = New RichTextBox
			Me.getConditionStateButton = New Button
			Me.conditionListGroupBox = New GroupBox
			Me.conditionlistView = New System.Windows.Forms.ListView
			Me.conditionStateColumnHeader = New ColumnHeader
			Me.sourceColumnHeader = New ColumnHeader
			Me.nameColumnHeader = New ColumnHeader
			Me.severityColumnHeader = New ColumnHeader
			Me.messageColumnHeader = New ColumnHeader
			Me.timeColumnHeader = New ColumnHeader
			Me.actorIDcolumnHeader = New ColumnHeader
			Me.subConditionColumnHeader = New ColumnHeader
			Me.acknowledgeButton = New Button
			Me.disableConditionsButton = New Button
			Me.enableConditionsButton = New Button
			Me.panel1.SuspendLayout()
			Me.panel2.SuspendLayout()
			Me.conditionStateGroupBox.SuspendLayout()
			Me.conditionListGroupBox.SuspendLayout()
			Me.SuspendLayout()
			' 
			' panel1
			' 
			Me.panel1.Controls.Add(Me.panel2)
			Me.panel1.Dock = System.Windows.Forms.DockStyle.Fill
			Me.panel1.Location = New System.Drawing.Point(0, 0)
			Me.panel1.Name = "panel1"
			Me.panel1.Size = New System.Drawing.Size(904, 750)
			Me.panel1.TabIndex = 0
			' 
			' panel2
			' 
			Me.panel2.Controls.Add(Me.conditionStateGroupBox)
			Me.panel2.Controls.Add(Me.conditionListGroupBox)
			Me.panel2.Location = New System.Drawing.Point(8, 8)
			Me.panel2.Name = "panel2"
			Me.panel2.Size = New System.Drawing.Size(896, 736)
			Me.panel2.TabIndex = 11
			' 
			' conditionStateGroupBox
			' 
			Me.conditionStateGroupBox.Controls.Add(Me.conditionStateRichTextBox)
			Me.conditionStateGroupBox.Controls.Add(Me.getConditionStateButton)
			Me.conditionStateGroupBox.Dock = System.Windows.Forms.DockStyle.Bottom
			Me.conditionStateGroupBox.Location = New System.Drawing.Point(0, 368)
			Me.conditionStateGroupBox.Name = "conditionStateGroupBox"
			Me.conditionStateGroupBox.Size = New System.Drawing.Size(896, 368)
			Me.conditionStateGroupBox.TabIndex = 1
			Me.conditionStateGroupBox.TabStop = False
			Me.conditionStateGroupBox.Text = "AE Condition State"
			' 
			' conditionStateRichTextBox
			' 
			Me.conditionStateRichTextBox.BackColor = System.Drawing.SystemColors.Window
			Me.conditionStateRichTextBox.Location = New System.Drawing.Point(8, 24)
			Me.conditionStateRichTextBox.Name = "conditionStateRichTextBox"
			Me.conditionStateRichTextBox.[ReadOnly] = True
			Me.conditionStateRichTextBox.Size = New System.Drawing.Size(752, 336)
			Me.conditionStateRichTextBox.TabIndex = 5
			Me.conditionStateRichTextBox.Text = ""
			' 
			' getConditionStateButton
			' 
			Me.getConditionStateButton.Location = New System.Drawing.Point(768, 40)
			Me.getConditionStateButton.Name = "getConditionStateButton"
			Me.getConditionStateButton.Size = New System.Drawing.Size(120, 40)
			Me.getConditionStateButton.TabIndex = 7
			Me.getConditionStateButton.Text = "Get Condition State"
			AddHandler Me.getConditionStateButton.Click, AddressOf Me.GetConditionStateButtonOnClick
			' 
			' conditionListGroupBox
			' 
			Me.conditionListGroupBox.Controls.Add(Me.conditionlistView)
			Me.conditionListGroupBox.Controls.Add(Me.acknowledgeButton)
			Me.conditionListGroupBox.Controls.Add(Me.disableConditionsButton)
			Me.conditionListGroupBox.Controls.Add(Me.enableConditionsButton)
			Me.conditionListGroupBox.Dock = System.Windows.Forms.DockStyle.Top
			Me.conditionListGroupBox.Location = New System.Drawing.Point(0, 0)
			Me.conditionListGroupBox.Name = "conditionListGroupBox"
			Me.conditionListGroupBox.Size = New System.Drawing.Size(896, 352)
			Me.conditionListGroupBox.TabIndex = 0
			Me.conditionListGroupBox.TabStop = False
			Me.conditionListGroupBox.Text = "AE Condition List"
			' 
			' conditionlistView
			' 
			Me.conditionlistView.Columns.AddRange(New ColumnHeader() {Me.conditionStateColumnHeader, Me.sourceColumnHeader, Me.nameColumnHeader, Me.severityColumnHeader, Me.messageColumnHeader, Me.timeColumnHeader, _
			Me.actorIDcolumnHeader, Me.subConditionColumnHeader})
			Me.conditionlistView.FullRowSelect = True
			Me.conditionlistView.Location = New System.Drawing.Point(8, 24)
			Me.conditionlistView.MultiSelect = False
			Me.conditionlistView.Name = "conditionlistView"
			Me.conditionlistView.Size = New System.Drawing.Size(760, 320)
			Me.conditionlistView.TabIndex = 0
			Me.conditionlistView.View = System.Windows.Forms.View.Details
			' 
			' conditionStateColumnHeader
			' 
			Me.conditionStateColumnHeader.Text = "ConditionState"
			Me.conditionStateColumnHeader.Width = 83
			' 
			' sourceColumnHeader
			' 
			Me.sourceColumnHeader.Text = "Source"
			Me.sourceColumnHeader.Width = 148
			' 
			' nameColumnHeader
			' 
			Me.nameColumnHeader.Text = "Condition Name"
			Me.nameColumnHeader.Width = 98
			' 
			' severityColumnHeader
			' 
			Me.severityColumnHeader.Text = "Severity"
			Me.severityColumnHeader.Width = 61
			' 
			' messageColumnHeader
			' 
			Me.messageColumnHeader.Text = "Message"
			Me.messageColumnHeader.Width = 92
			' 
			' timeColumnHeader
			' 
			Me.timeColumnHeader.Text = "Time"
			Me.timeColumnHeader.Width = 105
			' 
			' actorIDcolumnHeader
			' 
			Me.actorIDcolumnHeader.Text = "ActorID"
			' 
			' subConditionColumnHeader
			' 
			Me.subConditionColumnHeader.Text = "SubconditionName"
			Me.subConditionColumnHeader.Width = 108
			' 
			' acknowledgeButton
			' 
			Me.acknowledgeButton.Location = New System.Drawing.Point(776, 56)
			Me.acknowledgeButton.Name = "acknowledgeButton"
			Me.acknowledgeButton.Size = New System.Drawing.Size(112, 40)
			Me.acknowledgeButton.TabIndex = 8
			Me.acknowledgeButton.Text = "Acknowledge conditions"
			AddHandler Me.acknowledgeButton.Click, AddressOf Me.AcknowledgeButtonOnClick
			' 
			' disableConditionsButton
			' 
			Me.disableConditionsButton.Location = New System.Drawing.Point(776, 168)
			Me.disableConditionsButton.Name = "disableConditionsButton"
			Me.disableConditionsButton.Size = New System.Drawing.Size(112, 40)
			Me.disableConditionsButton.TabIndex = 10
			Me.disableConditionsButton.Text = "Disable conditions"
			AddHandler Me.disableConditionsButton.Click, AddressOf Me.DisableConditionsButtonOnClick
			' 
			' enableConditionsButton
			' 
			Me.enableConditionsButton.Location = New System.Drawing.Point(776, 112)
			Me.enableConditionsButton.Name = "enableConditionsButton"
			Me.enableConditionsButton.Size = New System.Drawing.Size(112, 40)
			Me.enableConditionsButton.TabIndex = 9
			Me.enableConditionsButton.Text = "Enable conditions"
			AddHandler Me.enableConditionsButton.Click, AddressOf Me.EnableConditionsButtonOnClick
			' 
			' AEConditions
			' 
			Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
			Me.ClientSize = New System.Drawing.Size(904, 750)
			Me.Controls.Add(Me.panel1)
			Me.Icon = (DirectCast((resources.GetObject("$this.Icon")), System.Drawing.Icon))
			Me.MaximizeBox = False
			Me.Name = "AEConditions"
			Me.Text = "AE Conditions"
			'this.Closing += new System.ComponentModel.CancelEventHandler(m_opcClient.Form1_Closing);
			Me.panel1.ResumeLayout(False)
			Me.panel2.ResumeLayout(False)
			Me.conditionStateGroupBox.ResumeLayout(False)
			Me.conditionListGroupBox.ResumeLayout(False)
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

				Dim opcForm As New OpcForm()

				'  initialize the AE client simulation
				result = result Or opcClient.InitializeAeObjects()

				opcClient.SetForm(opcForm)

				opcForm.SetOutProc(outProc)

				System.Windows.Forms.Application.Run(opcForm)

				opcClient.Terminate()
				opcClient = Nothing
			Catch exc As Exception
				MessageBox.Show(exc.ToString())
			End Try

		End Sub

		Private Function GetState(ByVal state As EnumConditionState) As String
			Dim stateToString As String = String.Empty
			If (state And EnumConditionState.ACTIVE) = EnumConditionState.ACTIVE Then
				stateToString &= " ACT"
			End If
			If (state And EnumConditionState.ENABLED) = EnumConditionState.ENABLED Then
				stateToString &= " ENA"
			End If
			If (state And EnumConditionState.ACKED) = EnumConditionState.ACKED Then
				stateToString &= " ACK"
			End If
			If state = 0 Then
				stateToString &= " DIS"
			End If
			Return stateToString
		End Function

		Public Sub ChangeItem(ByVal condition As Client.AeCondition, ByVal state As String)

			Dim item As New ListViewItem(state)
			item.Tag = condition
			conditionlistView.Items.Add(item)
			item.SubItems.Add(condition.SourcePath)
			item.SubItems.Add(condition.ConditionName)
			item.SubItems.Add(condition.Severity.ToString())
			item.SubItems.Add(condition.Message)
			item.SubItems.Add(condition.OccurenceTime.ToString("hh:mm:ss.fff"))
			item.SubItems.Add(condition.ActorId)
			item.SubItems.Add(condition.SubConditionName)

		End Sub
		Public Sub RemoveItems()

			conditionlistView.Items.Clear()

		End Sub

		Private Sub AcknowledgeButtonOnClick(ByVal sender As Object, ByVal e As System.EventArgs)

			Dim results As Integer() = Nothing
			m_session.AcknowledgeAeConditions("aUser", "AECondition", m_subscription.AeConditionList, results, m_executionOptions)

		End Sub

		Private Sub EnableConditionsButtonOnClick(ByVal sender As Object, ByVal e As System.EventArgs)

			For Each condition As Client.AeCondition In m_subscription.AeConditionList
				Dim source As String = condition.SourcePath
				m_session.EnableAeConditionsBySource(New String() {source}, True, m_executionOptions)
			Next
		End Sub

		Private Sub DisableConditionsButtonOnClick(ByVal sender As Object, ByVal e As System.EventArgs)

			For Each condition As Client.AeCondition In m_subscription.AeConditionList
				Dim source As String = condition.SourcePath
				m_session.EnableAeConditionsBySource(New String() {source}, False, m_executionOptions)

			Next
		End Sub

		Private Sub GetConditionStateButtonOnClick(ByVal sender As Object, ByVal e As System.EventArgs)
			If conditionlistView.SelectedItems.Count <> 0 Then
				Dim condition As Client.AeCondition = (DirectCast(conditionlistView.SelectedItems(0).Tag, Client.AeCondition))
				Dim conditionState As AeConditionState = Nothing
				Me.conditionStateRichTextBox.Text = Nothing
				Dim res As Integer = m_session.GetAeConditionState(condition.SourcePath, condition.ConditionName, Nothing, conditionState, m_executionOptions)
				If ResultCode.SUCCEEDED(res) AndAlso (m_executionOptions.ExecutionType = EnumExecutionType.SYNCHRONOUS) Then
					Me.conditionStateRichTextBox.Text &= " Source Path: " + condition.SourcePath + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Condition Name: " + condition.ConditionName + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " State: " + GetState(conditionState.State) + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Quality: " + conditionState.Quality.ToString() + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Active Time: " + conditionState.ConditionActiveTime + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Inactive Time: " + conditionState.ConditionInactiveTime + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " AcknowledgeTime: " + conditionState.ConditionAckTime + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " AcknowledgerComment: " + conditionState.AcknowledgerComment + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " AcknowledgerID: " + conditionState.AcknowledgerId + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Active subcondition time: " + conditionState.SubConditionActiveTime + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Active subcondition name: " + conditionState.ActiveSubConditionName + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Active subcondition definition: " + conditionState.ActiveSubConditionDefinition + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Active subcondition description: " + conditionState.ActiveSubConditionDescription + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Active subcondition severity: " + conditionState.ActiveSubConditionSeverity.ToString() + "" & Chr(10) & ""
					Me.conditionStateRichTextBox.Text &= " Number of subconditions: " + conditionState.SubConditionsNames.Length.ToString() + "" & Chr(10) & ""
					Dim i As Integer = 0
					While i < conditionState.SubConditionsNames.Length
						Me.conditionStateRichTextBox.Text &= "  Subcondition name: " + conditionState.SubConditionsNames(i) + "" & Chr(10) & ""
						Me.conditionStateRichTextBox.Text &= "  Subcondition definition: " + conditionState.SubConditionsDefinitions(i) + "" & Chr(10) & ""
						Me.conditionStateRichTextBox.Text &= "  Subcondition description: " + conditionState.SubConditionsDescriptions(i) + "" & Chr(10) & ""
						Me.conditionStateRichTextBox.Text &= "  Subcondition severity: " + conditionState.SubConditionsSeverities(i).ToString() + "" & Chr(10) & ""
						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
				End If
			Else
				MessageBox.Show(" Select a condition in the condition list ! ", "Select condition")
			End If
		End Sub

		Public Delegate Sub ChangeConditionListView(ByVal condition As Client.AeCondition, ByVal state As String)
		Public Delegate Sub RemoveItemsFromListView()

	End Class
End Namespace

