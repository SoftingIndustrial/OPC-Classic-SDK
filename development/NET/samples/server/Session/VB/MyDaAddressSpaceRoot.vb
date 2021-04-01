Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace Session
	Public Class MyDaAddressSpaceRoot
		Inherits DaAddressSpaceRoot
#Region "Public Methods"
		'--------------------

		Public Overloads Overrides Function QueryAddressSpaceElementData(ByVal elementId As String, ByRef anAddressSpaceElement As AddressSpaceElement) As Integer
			'  TODO: add string based address space validations
			anAddressSpaceElement = Nothing
			Return EnumResultCode.E_NOTIMPL
		End Function

		'--
#End Region

	End Class
End Namespace

