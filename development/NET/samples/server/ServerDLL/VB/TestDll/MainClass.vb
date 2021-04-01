Imports System
Imports Softing.OPCToolbox
Imports DllProject.DllSample

Namespace TestDll

	Class Console
		Shared Sub Main(ByVal args As String())
			Try
				'create an instance of the generated DLL
				Dim usedDll As New Dll

				' call the DLL's Start method; server will be initialized, 
				' namespaces will be created and simulation thread will be started
				Dim result As Integer = usedDll.Start()

				If Not (result = EnumResultCode.S_OK) Then
					Return
				End If

				System.Console.WriteLine("Press 'e' or 'q' and then 'ENTER' to exit")

				Dim [end] As Boolean = False

				While Not [end]
					Dim read As String = System.Console.ReadLine()
					Select Case read
						Case "E", "e", "Q", "q"
							[end] = True
						Case Else
					End Select
				End While

				'calls the DLL's Stop method which terminates the server application and closes the simulation thread
				usedDll.[Stop]()

			Catch exc As Exception
				System.Console.WriteLine(exc.ToString())
			End Try
		End Sub
	End Class

End Namespace

