Imports System
Imports Softing.OPCToolbox.Server

Namespace DllSample

	Public Class MyRequest
		Inherits DaRequest

		Public Sub New(ByVal transactionType As EnumTransactionType, ByVal sessionHandle As UInt32, ByVal aDaAddressSpaceElement As DaAddressSpaceElement, ByVal propertyID As Integer, ByVal requestHandle As UInt32)
			MyBase.New(transactionType, sessionHandle, aDaAddressSpaceElement, propertyID, requestHandle)
		End Sub
	End Class
End Namespace

