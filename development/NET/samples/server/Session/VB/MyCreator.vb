Imports System
Imports Softing.OPCToolbox.Server

Namespace Session
	Public Class MyCreator
		Inherits Creator
#Region "Public Methods"
		'---------------------

		Public Overloads Overrides Function CreateInternalDaAddressSpaceElement(ByVal anItemId As String, ByVal aName As String, ByVal anUserData As UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32) As DaAddressSpaceElement
			Return DirectCast(New MyDaAddressSpaceElement(anItemId, aName, anUserData, anObjectHandle, aParentHandle), DaAddressSpaceElement)
		End Function


		Public Overloads Overrides Function CreateDaAddressSpaceRoot() As DaAddressSpaceRoot
			Return DirectCast(New MyDaAddressSpaceRoot, DaAddressSpaceRoot)
		End Function

		Public Overloads Overrides Function CreateTransaction(ByVal transactionType As EnumTransactionType, ByVal requestList As DaRequest(), ByVal sessionKey As UInt32) As DaTransaction
			Return DirectCast(New MyTransaction(transactionType, requestList, sessionKey), DaTransaction)
		End Function

		Public Overridable Function CreateMyDaAddressSpaceElement() As DaAddressSpaceElement
			Return DirectCast(New MyDaAddressSpaceElement, DaAddressSpaceElement)
		End Function

		Public Overloads Overrides Function CreateRequest(ByVal aTransactionType As EnumTransactionType, ByVal aSessionHandle As UInt32, ByVal anElement As DaAddressSpaceElement, ByVal aPropertyId As Integer, ByVal aRequestHandle As UInt32) As DaRequest
			Return New MyRequest(aTransactionType, aSessionHandle, anElement, aPropertyId, aRequestHandle)
		End Function

		Public Overloads Overrides Function CreateSession(ByVal aType As EnumSessionType, ByVal aHandle As UInt32) As DaSession
			Return New MySession(aType, aHandle)
		End Function

		'--
#End Region

	End Class
End Namespace

