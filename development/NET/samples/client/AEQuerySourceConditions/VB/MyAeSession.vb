Imports System
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace AEQuerySourceConditions
	Public Class MyAeSession
		Inherits AeSession

#Region "Constructor"
		'-----------------

		Public Sub New(ByVal url As String)
			MyBase.New(url)
			AddHandler QueryAeSourceConditionsCompleted, AddressOf HandleQueryAeSourceConditionsCompleted
		End Sub

		'--
#End Region

#Region "Private Members"
		'---------------------

#Region "Private Attributes"
		'------------------------


		'--
#End Region

		'--
#End Region

#Region "Public Methods"
		'---------------------

		'--
#End Region

#Region "Public Properties"
		'-----------------------


		'--
#End Region

#Region "Handles"
		'---------------------

		Public Shared Sub HandleQueryAeSourceConditionsCompleted(ByVal anAeSession As AeSession, ByVal executionOptions As UInt32, ByVal sourcePath As String, ByVal conditionNames As String(), ByVal result As Integer)

			If ResultCode.SUCCEEDED(result) Then

				System.Console.WriteLine("" & Chr(10).ToString() & " Source conditions of " + sourcePath + " :")
				Dim i As Integer = 0
				While i < conditionNames.Length
					System.Console.WriteLine("  [" + i.ToString() + "] " + conditionNames(i))
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			Else

				System.Console.WriteLine("Failed to asynchronously query the conditions of source: " + sourcePath)
			End If

		End Sub


		'--
#End Region

	End Class

End Namespace

