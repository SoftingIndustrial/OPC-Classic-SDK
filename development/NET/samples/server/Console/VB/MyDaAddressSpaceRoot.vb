Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace Console
	Public Class MyDaAddressSpaceRoot
		Inherits DaAddressSpaceRoot
#Region "Public Methods"
		'--------------------

		Public Overloads Overrides Function QueryAddressSpaceElementData(ByVal anElementId As String, ByRef anElement As AddressSpaceElement) As Integer

			Dim element As New MyDaAddressSpaceElement
			element.ItemId = anElementId
			anElement = element

			Return EnumResultCode.S_OK

		End Function

		'--
#End Region

	End Class
End Namespace

