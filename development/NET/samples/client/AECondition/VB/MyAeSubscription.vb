Imports System
Imports System.Windows.Forms
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace AECondition
	Public Class MyAeSubscription
		Inherits AeSubscription

#Region "Constructor"
		'-----------------

		Public Sub New(ByVal parentSession As MyAeSession)
			MyBase.New(parentSession)
			AddHandler AeConditionsChanged, AddressOf HandleAeConditionsChanged

		End Sub

		'--
#End Region

#Region "Private Members"
		'---------------------


#Region "Private Attributes"
		'------------------------

		Private m_opcForm As OpcForm = Nothing

		'--
#End Region


		'--
#End Region

#Region "Public Methods"
		'---------------------

		Public Sub SetForm(ByVal form As OpcForm)
			m_opcForm = form
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

		'--
#End Region

#Region "Public Properties"
		'-----------------------


		'--
#End Region

#Region "Handles"
		'-----------------------

		Private Sub HandleAeConditionsChanged(ByVal anAeSubscription As AeSubscription, ByVal conditions As Client.AeCondition())

			If m_opcForm.conditionlistView.InvokeRequired Then
				Dim riflw As New RemoveItemsFromListView(AddressOf m_opcForm.RemoveItems)
				m_opcForm.conditionlistView.BeginInvoke(riflw, Nothing)
			Else
				m_opcForm.conditionlistView.Items.Clear()
			End If
			Dim state As String = [String].Empty
			Dim i As Integer = 0
			While i < conditions.Length
				Dim condition As Client.AeCondition = conditions(i)
				state = GetState(condition.State)

				If m_opcForm.conditionlistView.InvokeRequired Then
					Dim cclw As New ChangeConditionListView(AddressOf m_opcForm.ChangeItem)
					m_opcForm.conditionlistView.BeginInvoke(cclw, New Object() {condition, state})
				Else

					Dim item As New ListViewItem(state)
					item.Tag = condition
					m_opcForm.conditionlistView.Items.Add(item)
					item.SubItems.Add(condition.SourcePath)
					item.SubItems.Add(condition.ConditionName)
					item.SubItems.Add(condition.Severity.ToString())
					item.SubItems.Add(condition.Message)
					item.SubItems.Add(condition.OccurenceTime.ToString("hh:mm:ss.fff"))
					item.SubItems.Add(condition.ActorId)
					item.SubItems.Add(condition.SubConditionName)
					state = [String].Empty
				End If
				System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
			End While
			System.Diagnostics.Debug.WriteLine("The conditions list has changed ")
		End Sub

		'--
#End Region

		Public Delegate Sub ChangeConditionListView(ByVal condition As Client.AeCondition, ByVal state As String)
		Public Delegate Sub RemoveItemsFromListView()

	End Class

End Namespace

