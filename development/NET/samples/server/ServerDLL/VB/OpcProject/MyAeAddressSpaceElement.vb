Imports System
Imports Softing.OPCToolbox.Server

Namespace DllSample
	Public Class MyAeAddressSpaceElement
		Inherits AeAddressSpaceElement
#Region "Constructors"
		'-------------------

		Public Sub New(ByVal aName As String, ByVal anUserData As System.UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32)
			MyBase.New(aName, anUserData, anObjectHandle, aParentHandle)
		End Sub

		Public Sub New()
		End Sub

		'--
#End Region
	End Class
End Namespace

