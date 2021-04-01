Imports System
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace Console
	Public Class MyAeAddressSpaceElement
		Inherits AeAddressSpaceElement
#Region "Constructors"
		'-------------------

		Public Sub New(ByVal aName As String, ByVal anUserData As UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32)
			MyBase.New(aName, anUserData, anObjectHandle, aParentHandle)
		End Sub

		Public Sub New()
		End Sub

		'--
#End Region

#Region "Public Methods"
		'-------------------

		Public Overloads Overrides Function QueryConditions(ByVal aSourcePath As String, ByRef aConditionNames As String()) As Integer

			aConditionNames = New String(1) {OpcServer.COND_NAME_BETWEEN_MULTIPLE, OpcServer.COND_NAME_BETWEEN_SINGLE}
			Return EnumResultCode.S_OK
		End Function

		'--
#End Region

	End Class
End Namespace

