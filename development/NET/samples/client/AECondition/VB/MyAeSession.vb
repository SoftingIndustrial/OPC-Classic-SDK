Imports System
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace AECondition
	Public Class MyAeSession
		Inherits AeSession

#Region "Constructor"
		'-----------------

		Public Sub New(ByVal url As String)
			MyBase.New(url)
			AddHandler EnableAeConditionsBySourceCompleted, AddressOf HandleEnableAeConditionsBySourceCompleted

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

		'--
#End Region

#Region "Public Properties"
		'-----------------------


		'--
#End Region

#Region "Handles"
		'---------------------

		Private Sub HandleEnableAeConditionsBySourceCompleted(ByVal anAeSession As AeSession, ByVal executionContext As UInt32, ByVal enable As Boolean, ByVal path As String, ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				Console.WriteLine("Enabling conditions succeeded!")
			End If
		End Sub

		'--
#End Region

	End Class

End Namespace

