Namespace AEBrowse
	''' <summary>
	''' Summary description for OutProc.
	''' </summary>
	Public Class OutProc
#Region "Private Attributes"
		'-------------------------
		Private Shared m_opcClient As OpcClient = Nothing

		'--
#End Region

#Region "Public Properties"
		'------------------------

		Public ReadOnly Property OpcClient() As OpcClient
			Get
				Return m_opcClient
			End Get
		End Property
		'--
#End Region

#Region "Public Methods"
		'--------------------------
		Public Sub CreateOpcClient()
			If m_opcClient Is Nothing Then
				m_opcClient = New OpcClient
			End If
		End Sub
		'--
#End Region

	End Class
End Namespace

