Imports System
Imports Softing.OPCToolbox.Server

Namespace AddressSpace
	Public Class MyCreator
		Inherits Creator

		Public Overloads Overrides Function CreateDaAddressSpaceRoot() As DaAddressSpaceRoot
			Return DirectCast(New MyDaAddressSpaceRoot(), DaAddressSpaceRoot)
		End Function

		Public Overloads Overrides Function CreateTransaction(ByVal aTransactionType As EnumTransactionType, ByVal aRequests As DaRequest(), ByVal aSessionKey As UInt32) As DaTransaction

			Return DirectCast(New MyTransaction(aTransactionType, aRequests, aSessionKey), DaTransaction)
		End Function

		Public Overloads Overrides Function CreateInternalDaAddressSpaceElement(ByVal anItemId As String, ByVal aName As String, ByVal anUserData As System.UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32) As DaAddressSpaceElement

			Return DirectCast(New MyDaAddressSpaceElement(anItemId, aName, anUserData, anObjectHandle, aParentHandle), DaAddressSpaceElement)

		End Function

	End Class

End Namespace

