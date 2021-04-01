Imports System
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace Console
	Class MyWebTemplate
		Inherits WebTemplate

		Public Overloads Overrides Function HandleWebTemplate(ByVal aTemplateName As String, ByVal aNumArgs As Long, ByVal anArgs As String(), ByRef aResult As String) As Integer

			aResult = String.Empty

			If aTemplateName.StartsWith("CURRENTTIME") Then

				Dim isGerman As Boolean = False

				If aNumArgs >= 1 Then
					isGerman = (anArgs(0) = "GERMAN")
				End If

				Dim now As DateTime = DateTime.Now
				Dim format As System.Globalization.DateTimeFormatInfo = Nothing
				If isGerman Then
					format = New System.Globalization.CultureInfo("de-DE", False).DateTimeFormat
				Else
					format = New System.Globalization.CultureInfo("en-US", False).DateTimeFormat
				End If

				aResult = now.ToString("G", format)
				Return EnumResultCode.S_OK
			End If

			Return EnumResultCode.E_NOTIMPL
		End Function

	End Class
End Namespace

