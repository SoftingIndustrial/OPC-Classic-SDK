Imports System
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace UpdateMode

	Public Class MyTransaction
		Inherits DaTransaction

#Region "Public Methods"
		'--------------------

		Public Sub New(ByVal aTransactionType As EnumTransactionType, ByVal requestList As DaRequest(), ByVal aSessionKey As UInt32)
			MyBase.New(aTransactionType, requestList, aSessionKey)
		End Sub

		Public Overloads Overrides Function HandleReadRequests() As Integer
			Dim count As Integer = Requests.Count

			Dim i As Integer = 0
			While i < count
				Dim request As DaRequest = DirectCast(Requests(i), DaRequest)
				If request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) Then
					Dim element As MyDaAddressSpaceElement = CType(request.AddressSpaceElement, MyDaAddressSpaceElement)

					If element Is Nothing Then
						request.Result = EnumResultCode.E_FAIL
					Else
						If request.PropertyId = 0 Then

							If element.Type = MyDaAddressSpaceElement.TYPE_POLL_SEC Then

								' get address space element value
								Dim now As DateTime = DateTime.Now
								Dim cacheValue As New ValueQT(now.Second, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), now)
								request.Value = cacheValue
								request.Result = EnumResultCode.S_OK
							ElseIf element.Type = MyDaAddressSpaceElement.TYPE_POLL_MIN Then

								' get address space element value
								Dim now As DateTime = DateTime.Now
								Dim cacheValue As New ValueQT(now.Minute, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), now)
								request.Value = cacheValue
								request.Result = EnumResultCode.S_OK
							ElseIf element.Type = MyDaAddressSpaceElement.TYPE_REPORT_SEC Then

								' get address space element value
								'    this is a direct device read request
								Dim now As DateTime = DateTime.Now
								Dim cacheValue As New ValueQT(now.Second, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), now)
								request.Value = cacheValue
								request.Result = EnumResultCode.S_OK
							ElseIf element.Type = MyDaAddressSpaceElement.TYPE_REPORT_MIN Then

								' get address space element value
								'    this is a direct device read request
								Dim now As DateTime = DateTime.Now
								Dim cacheValue As New ValueQT(now.Minute, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), now)
								request.Value = cacheValue
								request.Result = EnumResultCode.S_OK
							End If
						Else
							' get property value
							' get the value from the address space element
							element.GetPropertyValue(request)
						End If
					End If
				End If
				i = i + 1
			End While
			Return CompleteRequests()
		End Function


		'--
#End Region
	End Class
End Namespace

