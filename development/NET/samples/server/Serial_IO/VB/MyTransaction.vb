Imports System
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace SerialIO

	Public Class MyTransaction
		Inherits DaTransaction

#Region "Public Methods"
		'--------------------

		Public Sub New(ByVal aTransactionType As EnumTransactionType, ByVal requestList As DaRequest(), ByVal aSessionKey As UInt32)
			MyBase.New(aTransactionType, requestList, aSessionKey)
		End Sub

		Public Overloads Overrides Function HandleReadRequests() As Integer
			'  Handle reads asynchrousouly
			Console.OpcServer.AddRequsts(Me.m_requestList)
			Return [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.S_OK)

		End Function

		Public Overloads Overrides Function HandleWriteRequests() As Integer
			'  Handle writesasynchrousouly

			Console.OpcServer.AddRequsts(Me.m_requestList)
			Return [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.S_OK)

		End Function

		'--
#End Region

	End Class

End Namespace

