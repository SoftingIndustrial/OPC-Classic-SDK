Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace DllSample

	Public Class MyDaAddressSpaceRoot
		Inherits DaAddressSpaceRoot

		Public Overloads Overrides Function QueryAddressSpaceElementData(ByVal elementId As String, ByRef anAddressSpaceElement As AddressSpaceElement) As Integer
			'  TODO: add string based address space validations
			anAddressSpaceElement = Nothing
			Return CType(EnumResultCode.E_NOTIMPL, Integer)
		End Function

		Public Overrides Function QueryAddressSpaceElementChildren(ByVal anElementID As String, ByVal aChildrenList As System.Collections.ArrayList) As Integer

		End Function
	End Class
End Namespace

