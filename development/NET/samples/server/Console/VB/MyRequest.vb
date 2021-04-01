Imports System
Imports Softing.OPCToolbox.Server

Namespace Console
	Public Class MyRequest
		Inherits DaRequest
#Region "Constructor"
		'-----------------

		Public Sub New(ByVal transactionType As EnumTransactionType, ByVal sessionHandle As UInt32, ByVal aDaAddressSpaceElement As DaAddressSpaceElement, ByVal propertyID As Integer, ByVal requestHandle As UInt32)
			MyBase.New(transactionType, sessionHandle, aDaAddressSpaceElement, propertyID, requestHandle)
		End Sub

		'--
#End Region

	End Class
End Namespace

