Imports Softing.OPCToolbox.Client

Namespace DABrowse
	Public Class MyDaSession
		Inherits DaSession

#Region "Constructor"
		'-----------------

		Public Sub New(ByVal url As String)
			MyBase.New(url)
			AddHandler StateChangeCompleted, AddressOf HandleStateChanged
		End Sub

		'--
#End Region

#Region "Private Members"
		'---------------------
		Dim m_connected As Boolean
#Region "Private Attributes"
		'------------------------


		'--
#End Region

		'--
#End Region

#Region "Public Methods"
		'---------------------

		'--
#End Region

#Region "Public Properties"
		'-----------------------
		Public Function isConnected() As Boolean
			Return m_connected
		End Function

		'--
#End Region

#Region "Handles"
		'---------------------
		Public Sub HandleStateChanged(ByVal sender As ObjectSpaceElement, ByVal state As EnumObjectState)

			If state = EnumObjectState.CONNECTED Then
				m_connected = True
			Else
				m_connected = False
			End If
		End Sub


		'--
#End Region

	End Class

End Namespace

